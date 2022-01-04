/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: spi.c$
* $Version : 3.8.1.0$
* $Date    : Sep-25-2012$
*
* Comments:
*
*   This file contains functions for generic SPI layer.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <stdlib.h>

#include "spi.h"
#include "spi_prv.h"


/* Forward declarations */
static _mqx_int _io_spi_open(MQX_FILE_PTR fd_ptr, char _PTR_ open_name_ptr, char _PTR_ flags);
static _mqx_int _io_spi_close(MQX_FILE_PTR fd_ptr);
static _mqx_int _io_spi_read(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
static _mqx_int _io_spi_write(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
static _mqx_int _io_spi_ioctl(MQX_FILE_PTR fd_ptr, uint_32 cmd, pointer param_ptr);
static _mqx_int _io_spi_uninstall(IO_DEVICE_STRUCT_PTR io_dev_ptr);


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_install
* Returned Value   : MQX error code
* Comments         :
*    Installs SPI device.
*
*END**********************************************************************/
_mqx_int _io_spi_install
    (
        /* [IN] A string that identifies the device for fopen */
        char_ptr                       identifier,

        /* [IN] Pointer to driver initialization data */
        SPI_INIT_STRUCT_CPTR           init_data_ptr
    )
{
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int error_code;

    if ((init_data_ptr->DEVIF->SETPARAM == NULL) || (init_data_ptr->DEVIF->TX_RX == NULL))
    {
        /* Missing mandatory low level driver function */
        return IO_ERROR_DEVICE_INVALID;
    }

    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(SPI_DRIVER_DATA_STRUCT));
    if (driver_data == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(driver_data, MEM_TYPE_IO_SPI_POLLED_DEVICE_STRUCT);

    driver_data->CS_CALLBACK = init_data_ptr->CS_CALLBACK;
    driver_data->CS_USERDATA= init_data_ptr->CS_USERDATA;
    driver_data->PARAMS = init_data_ptr->PARAMS;
    driver_data->DEVIF = init_data_ptr->DEVIF;

    /* initialize low level driver */
    if (driver_data->DEVIF->INIT)
        error_code = driver_data->DEVIF->INIT(init_data_ptr->DEVIF_INIT, &(driver_data->DEVIF_DATA));

    if (error_code != MQX_OK)
    {
        _mem_free(driver_data);
        return error_code;
    }

    _lwsem_create(&driver_data->BUS_LOCK, 1);

    error_code = _io_dev_install_ext(identifier,
        _io_spi_open, _io_spi_close,
        _io_spi_read, _io_spi_write,
        _io_spi_ioctl,
        _io_spi_uninstall,
        (pointer)driver_data);

    if (error_code)
    {
        _lwsem_destroy(&driver_data->BUS_LOCK);
        _mem_free(driver_data);
        return error_code;
    }

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstalls SPI device.
*
*END**********************************************************************/
static _mqx_int _io_spi_uninstall
    (
        /* [IN] The IO device structure for the device */
        IO_DEVICE_STRUCT_PTR           io_dev_ptr
    )
{
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int error_code = MQX_OK;

    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(io_dev_ptr->DRIVER_INIT_PTR);

    /* deinitialize low level driver */
    if (driver_data->DEVIF->DEINIT)
        error_code = driver_data->DEVIF->DEINIT(driver_data->DEVIF_DATA);

    if (error_code != MQX_OK)
    {
        return error_code;
    }

    _lwsem_destroy(&driver_data->BUS_LOCK);
    _mem_free(driver_data);

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_read_write
* Returned Value   : Number of bytes read
* Comments         :
*    Claims the bus and performs SPI transfer by calling low level driver.
*    Returns number of bytes processed.
*
*END*********************************************************************/
static _mqx_int _io_spi_read_write
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* Buffer containing bytes to write */
        char_ptr                       wrbuf,

        /* Buffer containing bytes read after transfer */
        char_ptr                       rdbuf,

        /* Byte length of each buffer */
        uint_32                        len
    )
{
    SPI_DEV_DATA_STRUCT_PTR    dev_data;
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int result;
    _mqx_int error_code;

    /* parameter sanity check */
    if (len == 0 || ((wrbuf == NULL) && (rdbuf == NULL)))
    {
        return IO_ERROR;
    }

    dev_data = (SPI_DEV_DATA_STRUCT_PTR)(fd_ptr->DEV_DATA_PTR);
    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    if (!(dev_data->BUS_OWNER))
    {
        error_code = _lwsem_wait(&(driver_data->BUS_LOCK));
        if (error_code != MQX_OK)
        {
            _task_set_error(error_code);
            return IO_ERROR;
        }
        dev_data->BUS_OWNER = TRUE;
        dev_data->PARAMS_DIRTY = TRUE;
    }

    if (dev_data->PARAMS_DIRTY)
    {
        error_code = driver_data->DEVIF->SETPARAM(driver_data->DEVIF_DATA, &(dev_data->PARAMS));
        if (error_code != MQX_OK)
        {
            _task_set_error(error_code);
            return IO_ERROR;
        }
        dev_data->PARAMS_DIRTY = FALSE;
    }

    if (driver_data->CS_CALLBACK)
    {
        error_code = driver_data->CS_CALLBACK(dev_data->PARAMS.CS, driver_data->CS_USERDATA);
        if (error_code != MQX_OK)
        {
            _task_set_error(error_code);
            return IO_ERROR;
        }
    }

    if ((wrbuf == NULL) && (dev_data->FLAGS & SPI_FLAG_FULL_DUPLEX))
    {
        wrbuf = rdbuf;
    }

    result = driver_data->DEVIF->TX_RX(driver_data->DEVIF_DATA, (uint_8_ptr)wrbuf, (uint_8_ptr)rdbuf, len);

    #if BSPCFG_ENABLE_SPI_STATS
    /* update statistics */
    if (result > 0)
    {
        int frames;
        frames = result / ((dev_data->PARAMS.FRAMESIZE + 7 ) / 8);
        if (rdbuf) dev_data->STATS.RX_PACKETS += frames;
        if (wrbuf) dev_data->STATS.TX_PACKETS += frames;
    }
    #endif

    return result;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_flush
* Returned Value   : Error code
* Comments         :
*    Finishes the transfer deasserting CS and releasing the bus
*
*END*********************************************************************/
static _mqx_int _io_spi_flush
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] Requests keeping CS in current state (for SPI_FLAG_NO_DEASSERT_ON_FLUSH) */
        boolean                        cs_keep
    )
{
    SPI_DEV_DATA_STRUCT_PTR    dev_data;
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int result = MQX_OK;
    _mqx_int error_code;

    dev_data = (SPI_DEV_DATA_STRUCT_PTR)(fd_ptr->DEV_DATA_PTR);
    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    if (dev_data->BUS_OWNER)
    {
        if (!cs_keep)
        {
            if (driver_data->DEVIF->CS_DEASSERT)
            {
                error_code = driver_data->DEVIF->CS_DEASSERT(driver_data->DEVIF_DATA);
                if (result == MQX_OK)
                    result = error_code;
            }

            if (driver_data->CS_CALLBACK)
            {
                error_code = driver_data->CS_CALLBACK(0, driver_data->CS_USERDATA);
                if (result == MQX_OK)
                    result = error_code;
            }
        }
        error_code = _lwsem_post(&(driver_data->BUS_LOCK));
        dev_data->BUS_OWNER = FALSE;
        if (result == MQX_OK)
            result = error_code;
    }
    dev_data->PARAMS_DIRTY = TRUE; /* forces resetting parameters before next transfer */

    return result;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_open
* Returned Value   : MQX error code
* Comments         :
*    This routine opens the SPI I/O channel.
*
*END**********************************************************************/
static _mqx_int _io_spi_open
    (
        /* [IN] The file handle for the device being opened */
        MQX_FILE_PTR         fd_ptr,

        /* [IN] The remaining portion of the name of the device */
        char                 _PTR_ open_name_ptr,

        /* [IN] The flags to be used during operation */
        char                 _PTR_ flags
    )
{
    SPI_DEV_DATA_STRUCT_PTR    dev_data;
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    char_ptr file_name_ptr;
    char_ptr endptr;

    uint_32  cs = 0; /* initialization required to avoid compilation warning */

    file_name_ptr = open_name_ptr;
    while (*file_name_ptr && *file_name_ptr++ != ':') {}

    if (*file_name_ptr)
    {
        cs = strtoul(file_name_ptr, &endptr, 0);
        if (*endptr)
        {
            return IO_ERROR_DEVICE_INVALID;
        }
    }

    dev_data = _mem_alloc_system_zero(sizeof(SPI_DEV_DATA_STRUCT));
    if (dev_data == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    fd_ptr->DEV_DATA_PTR = dev_data;

    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    /* copy default parameters */
    dev_data->PARAMS = driver_data->PARAMS;
    dev_data->PARAMS_DIRTY = TRUE;

    /* optionally assign CS according to file name */
    if (*file_name_ptr)
    {
        dev_data->PARAMS.CS = cs;
    }

    /* open flags */
    dev_data->FLAGS = (uint_32)flags;

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the SPI I/O channel.
*
*END**********************************************************************/
static _mqx_int _io_spi_close
    (
        /* [IN] The file handle for the device being closed */
        MQX_FILE_PTR         fd_ptr
    )
{
    _io_spi_flush(fd_ptr, 0);
    _mem_free(fd_ptr->DEV_DATA_PTR);

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_read
* Returned Value   : Number of bytes read
* Comments         :
*    POSIX wrapper to perform SPI transfer storing received data into the buffer.
*    Returns number of bytes received.
*
*END*********************************************************************/
static _mqx_int _io_spi_read
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [OUT] Where the characters are to be stored */
        char_ptr                       data_ptr,

        /* [IN] The number of bytes to read */
        _mqx_int                       n
    )
{
    return _io_spi_read_write(fd_ptr, NULL, data_ptr, n);
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_write
* Returned Value   : Number of bytes to write
* Comments         :
*    POSIX wrapper to perform SPI transfer sending data from the buffer.
*    Returns number of bytes received.
*
*END**********************************************************************/
static _mqx_int _io_spi_write
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] Where the bytes are stored */
        char_ptr                       data_ptr,

        /* [IN] The number of bytes to output */
        _mqx_int                       n
    )
{
    return _io_spi_read_write(fd_ptr, data_ptr, NULL, n);
}


/*FUNCTION*****************************************************************
*
* Function Name    : _io_spi_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of SPI ioctl operation.
*
*END*********************************************************************/
static _mqx_int _io_spi_ioctl
    (
        /* [IN] the file handle for the device */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] the ioctl command */
        uint_32                        cmd,

        /* [IN] the ioctl parameters */
        pointer                        param_ptr
    )
{
    SPI_DEV_DATA_STRUCT_PTR    dev_data;
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int result = MQX_OK;
    _mqx_int lldrv_result = MQX_OK;

    dev_data = (SPI_DEV_DATA_STRUCT_PTR)(fd_ptr->DEV_DATA_PTR);
    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    switch (cmd) {

        case IO_IOCTL_DEVICE_IDENTIFY:
            /* return the device identification */
            ((_mqx_uint_ptr)param_ptr)[0] = IO_DEV_TYPE_PHYS_SPI;
            ((_mqx_uint_ptr)param_ptr)[1] = 0;
            ((_mqx_uint_ptr)param_ptr)[2] = IO_DEV_ATTR_READ | IO_DEV_ATTR_WRITE;
            break;

        case IO_IOCTL_FLUSH_OUTPUT:
            result = _io_spi_flush(fd_ptr, ((dev_data->FLAGS & SPI_FLAG_NO_DEASSERT_ON_FLUSH)!=0));
            break;

        case IO_IOCTL_SPI_FLUSH_DEASSERT_CS:
            result = _io_spi_flush(fd_ptr, 0);
            break;

        case IO_IOCTL_SPI_READ_WRITE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                SPI_READ_WRITE_STRUCT_PTR rw_ptr = (SPI_READ_WRITE_STRUCT_PTR)param_ptr;
                if (rw_ptr->BUFFER_LENGTH != _io_spi_read_write(fd_ptr, rw_ptr->WRITE_BUFFER, rw_ptr->READ_BUFFER, rw_ptr->BUFFER_LENGTH))
                    result = IO_ERROR;

            }
            break;

        case IO_IOCTL_SPI_GET_FLAGS:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = dev_data->FLAGS;
            }
            break;

        case IO_IOCTL_SPI_SET_FLAGS:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                dev_data->FLAGS = *((uint_32_ptr)param_ptr);
            }
            break;

        case IO_IOCTL_SPI_GET_BAUD:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = dev_data->PARAMS.BAUDRATE;

                /* Pass IOCTL also to the low level driver */
                if (driver_data->DEVIF->IOCTL != NULL)
                    result = driver_data->DEVIF->IOCTL(driver_data->DEVIF_DATA, &(dev_data->PARAMS), cmd, param_ptr);

                /* Invalid IOCTL reported by the low level driver is not an error (implementation is not mandatory) */
                if (result == IO_ERROR_INVALID_IOCTL_CMD)
                    result = MQX_OK;
            }
            break;

        case IO_IOCTL_SPI_SET_BAUD:
            if (NULL == param_ptr || 0 == *((uint_32_ptr)param_ptr))
            {
                /* use default baudrate */
                dev_data->PARAMS.BAUDRATE = driver_data->PARAMS.BAUDRATE;
                dev_data->PARAMS_DIRTY = TRUE;
            }
            else
            {
                dev_data->PARAMS.BAUDRATE = *((uint_32_ptr)param_ptr);
                dev_data->PARAMS_DIRTY = TRUE;
            }
            break;

        case IO_IOCTL_SPI_GET_MODE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = dev_data->PARAMS.MODE;
            }
            break;

        case IO_IOCTL_SPI_SET_MODE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                dev_data->PARAMS.MODE = *((uint_32_ptr)param_ptr);
                dev_data->PARAMS_DIRTY = TRUE;
            }
            break;

        case IO_IOCTL_SPI_GET_CS:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = dev_data->PARAMS.CS;
            }
            break;

        case IO_IOCTL_SPI_SET_CS:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                dev_data->PARAMS.CS = *((uint_32_ptr)param_ptr);
                dev_data->PARAMS_DIRTY = TRUE;
            }
            break;

        case IO_IOCTL_SPI_GET_FRAMESIZE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = dev_data->PARAMS.FRAMESIZE;
            }
            break;

        case IO_IOCTL_SPI_SET_FRAMESIZE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                dev_data->PARAMS.FRAMESIZE = *((uint_32_ptr)param_ptr);
                dev_data->PARAMS_DIRTY = TRUE;
            }
            break;

        case IO_IOCTL_SPI_GET_TRANSFER_MODE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else if ((dev_data->PARAMS.ATTR & SPI_ATTR_TRANSFER_MODE_MASK) == SPI_ATTR_SLAVE_MODE)
            {
                *((uint_32_ptr)param_ptr) = SPI_DEVICE_SLAVE_MODE;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = SPI_DEVICE_MASTER_MODE;
            }
            break;

        case IO_IOCTL_SPI_SET_TRANSFER_MODE:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else if (*((uint_32_ptr)param_ptr) == SPI_DEVICE_SLAVE_MODE)
            {
                dev_data->PARAMS.ATTR &= ~SPI_ATTR_TRANSFER_MODE_MASK;
                dev_data->PARAMS.ATTR |= SPI_ATTR_SLAVE_MODE;
                dev_data->PARAMS_DIRTY = TRUE;
            }
            else if (*((uint_32_ptr)param_ptr) == SPI_DEVICE_MASTER_MODE)
            {
                dev_data->PARAMS.ATTR &= ~SPI_ATTR_TRANSFER_MODE_MASK;
                dev_data->PARAMS.ATTR |= SPI_ATTR_MASTER_MODE;
                dev_data->PARAMS_DIRTY = TRUE;
            }
            else
            {
                result = SPI_ERROR_TRANSFER_MODE_INVALID;
            }
            break;

        case IO_IOCTL_SPI_GET_ENDIAN:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else if ((dev_data->PARAMS.ATTR & SPI_ATTR_ENDIAN_MASK) == SPI_ATTR_LITTLE_ENDIAN)
            {
                *((uint_32_ptr)param_ptr) = SPI_DEVICE_LITTLE_ENDIAN;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = SPI_DEVICE_BIG_ENDIAN;
            }
            break;

        case IO_IOCTL_SPI_SET_ENDIAN:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else if (*((uint_32_ptr)param_ptr) == SPI_DEVICE_LITTLE_ENDIAN)
            {
                dev_data->PARAMS.ATTR &= ~SPI_ATTR_ENDIAN_MASK;
                dev_data->PARAMS.ATTR |= SPI_ATTR_LITTLE_ENDIAN;
                dev_data->PARAMS_DIRTY = TRUE;
            }
            else if (*((uint_32_ptr)param_ptr) == SPI_DEVICE_BIG_ENDIAN)
            {
                dev_data->PARAMS.ATTR &= ~SPI_ATTR_ENDIAN_MASK;
                dev_data->PARAMS.ATTR |= SPI_ATTR_BIG_ENDIAN;
                dev_data->PARAMS_DIRTY = TRUE;
            }
            else
            {
                result = SPI_ERROR_ENDIAN_INVALID;
            }
            break;

        case IO_IOCTL_SPI_GET_DUMMY_PATTERN:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                *((uint_32_ptr)param_ptr) = dev_data->PARAMS.DUMMY_PATTERN;
            }
            break;

        case IO_IOCTL_SPI_SET_DUMMY_PATTERN:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                dev_data->PARAMS.DUMMY_PATTERN = *((uint_32_ptr)param_ptr);
                dev_data->PARAMS_DIRTY = TRUE;
            }
            break;

        case IO_IOCTL_SPI_SET_CS_CALLBACK:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                if (!(dev_data->BUS_OWNER) && ((result=_lwsem_wait(&(driver_data->BUS_LOCK))) != MQX_OK))
                    break;

                driver_data->CS_CALLBACK = ((SPI_CS_CALLBACK_STRUCT_PTR)param_ptr)->CALLBACK;
                driver_data->CS_USERDATA = ((SPI_CS_CALLBACK_STRUCT_PTR)param_ptr)->USERDATA;

                if (!(dev_data->BUS_OWNER))
                    result = _lwsem_post(&(driver_data->BUS_LOCK));
            }
            break;

        case IO_IOCTL_SPI_GET_STATS:
            if (NULL == param_ptr)
            {
                result = SPI_ERROR_INVALID_PARAMETER;
            }
            else
            {
                #if BSPCFG_ENABLE_SPI_STATS
                *((SPI_STATISTICS_STRUCT_PTR)param_ptr) = dev_data->STATS;
                #else
                _mem_zero(param_ptr, sizeof(SPI_STATISTICS_STRUCT));
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                #endif
            }
            break;

        case IO_IOCTL_SPI_CLEAR_STATS:
            #if BSPCFG_ENABLE_SPI_STATS
            _mem_zero(&(dev_data->STATS), sizeof(dev_data->STATS));
            #else
            result = MQX_IO_OPERATION_NOT_AVAILABLE;
            #endif
            break;

        case IO_IOCTL_SPI_DEVICE_ENABLE:
            break;

        case IO_IOCTL_SPI_DEVICE_DISABLE:
            break;

        default:
            /* Pass unhandled IOCTL to the low level driver */
            if (driver_data->DEVIF->IOCTL != NULL)
            {
                /* Do not assume that the low level driver is re-entrant, lock semaphore */
                if (!(dev_data->BUS_OWNER) && ((result=_lwsem_wait(&(driver_data->BUS_LOCK))) != MQX_OK))
                    break;

                lldrv_result = driver_data->DEVIF->IOCTL(driver_data->DEVIF_DATA, &(dev_data->PARAMS), cmd, param_ptr);

                if (!(dev_data->BUS_OWNER))
                    result = _lwsem_post(&(driver_data->BUS_LOCK));

                /* Report low level driver error, if any */
                if (lldrv_result != MQX_OK)
                    result = lldrv_result;
            }
            else
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            }
            break;
    }

    return result;
}

/* EOF */
