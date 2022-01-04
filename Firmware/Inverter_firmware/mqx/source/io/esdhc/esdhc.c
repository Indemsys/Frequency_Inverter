/**HEADER********************************************************************
*
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: esdhc.c$
* $Version : 3.8.12.0$
* $Date    : Jul-19-2012$
*
* Comments:
*
*   The file contains low level eSDHC driver functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <string.h>
#include "esdhc.h"
#include "esdhc_prv.h"


/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_set_baudrate
* Returned Value   : None
* Comments         :
*    Find and set closest divider values for given baudrate.
*
*END*********************************************************************/
static void _esdhc_set_baudrate
    (
        /* [IN/OUT] Module registry pointer */
        SDHC_MemMapPtr esdhc_ptr,

        /* [IN] Module input clock in Hz */
        uint_32         clock,

        /* [IN] Desired baudrate in Hz */
        uint_32         baudrate
    )
{
    uint_32 pres, div, min, minpres = 0x80, mindiv = 0x0F;
    int_32  val;

    /* Find closest setting */
    min = (uint_32)-1;
    for (pres = 2; pres <= 256; pres <<= 1)
    {
        for (div = 1; div <= 16; div++)
        {
            val = pres * div * baudrate - clock;
            if (val >= 0)
            {
                if (min > val)
                {
                    min = val;
                    minpres = pres;
                    mindiv = div;
                }
            }
        }
    }

    /* Disable ESDHC clocks */
    esdhc_ptr->SYSCTL &= (~ SDHC_SYSCTL_SDCLKEN_MASK);

    /* Change dividers */
    div = esdhc_ptr->SYSCTL & (~ (SDHC_SYSCTL_DTOCV_MASK | SDHC_SYSCTL_SDCLKFS_MASK | SDHC_SYSCTL_DVS_MASK));
    esdhc_ptr->SYSCTL = div | (SDHC_SYSCTL_DTOCV(0x0E) | SDHC_SYSCTL_SDCLKFS(minpres >> 1) | SDHC_SYSCTL_DVS(mindiv - 1));

    /* Wait for stable clock */
    while (0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_SDSTB_MASK))
    {
        /* Workaround... */
        _time_delay (BSP_ALARM_RESOLUTION);

     /* not every controller has this bit */
#if defined (BSP_M53015EVB) || (BSP_MPC8377RDB)
        break;
#endif
    };

    /* Enable ESDHC clocks */
    esdhc_ptr->SYSCTL |= SDHC_SYSCTL_SDCLKEN_MASK;
    esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DTOE_MASK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_is_running
* Returned Value   : TRUE if running, FALSE otherwise
* Comments         :
*    Checks whether eSDHC module is currently in use.
*
*END*********************************************************************/
static boolean _esdhc_is_running
    (
        /* [IN] Module registry pointer */
        SDHC_MemMapPtr esdhc_ptr
    )
{
    return (0 != (esdhc_ptr->PRSSTAT & (SDHC_PRSSTAT_RTA_MASK | SDHC_PRSSTAT_WTA_MASK | SDHC_PRSSTAT_DLA_MASK | SDHC_PRSSTAT_CDIHB_MASK | SDHC_PRSSTAT_CIHB_MASK)));
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_status_wait
* Returned Value   : bits set for given mask
* Comments         :
*    Waits for ESDHC interrupt status register bits according to given mask.
*
*END*********************************************************************/
static uint_32 _esdhc_status_wait
    (
        /* [IN] Module registry pointer */
        SDHC_MemMapPtr esdhc_ptr,

        /* [IN] Mask of IRQSTAT bits to wait for */
        uint_32         mask
    )
{
    uint_32             result;
    do
    {
        result = esdhc_ptr->IRQSTAT & mask;
    }
    while (0 == result);
    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_init
* Returned Value   : MQX error code
* Comments         :
*    ESDHC registers initialization and card detection.
*
*END*********************************************************************/
static int_32 _esdhc_init
    (
        /* [IN/OUT] Module registry pointer */
        SDHC_MemMapPtr        esdhc_ptr,

        /* [IN/OUT] Device runtime information */
        ESDHC_INFO_STRUCT_PTR  esdhc_info_ptr,

        /* [IN] Device initialization data */
        ESDHC_INIT_STRUCT_CPTR esdhc_init_ptr
    )
{
    esdhc_info_ptr->CARD = ESDHC_CARD_NONE;

    /* De-init GPIO - to prevent unwanted clocks on bus */
    if (_bsp_esdhc_io_init (esdhc_init_ptr->CHANNEL, 0) == -1)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Reset ESDHC */
    esdhc_ptr->SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);
    while (esdhc_ptr->SYSCTL & SDHC_SYSCTL_RSTA_MASK)
        { };

    /* Initial values */
    esdhc_ptr->VENDOR = 0;
    esdhc_ptr->BLKATTR = SDHC_BLKATTR_BLKCNT(1) | SDHC_BLKATTR_BLKSIZE(512);
#if PSP_ENDIAN == MQX_BIG_ENDIAN
    esdhc_ptr->PROCTL = SDHC_PROCTL_EMODE(ESDHC_PROCTL_EMODE_BIG) | SDHC_PROCTL_D3CD_MASK;
#else
    esdhc_ptr->PROCTL = SDHC_PROCTL_EMODE(ESDHC_PROCTL_EMODE_LITTLE) | SDHC_PROCTL_D3CD_MASK;
#endif
    esdhc_ptr->WML = SDHC_WML_RDWML(2) | SDHC_WML_WRWML(1);

    /* Set the ESDHC initial baud rate divider and start */
    _esdhc_set_baudrate (esdhc_ptr, esdhc_init_ptr->CLOCK_SPEED, 400000);

    /* Poll inhibit bits */
    while (esdhc_ptr->PRSSTAT & (SDHC_PRSSTAT_CIHB_MASK | SDHC_PRSSTAT_CDIHB_MASK))
        { };

    /* Init GPIO again */
    if (_bsp_esdhc_io_init (esdhc_init_ptr->CHANNEL, 0xFFFF) == -1)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Enable requests */
    esdhc_ptr->IRQSTAT = 0xFFFF;
    esdhc_ptr->IRQSTATEN = SDHC_IRQSTATEN_DEBESEN_MASK | SDHC_IRQSTATEN_DCESEN_MASK | SDHC_IRQSTATEN_DTOESEN_MASK
                         | SDHC_IRQSTATEN_CIESEN_MASK | SDHC_IRQSTATEN_CEBESEN_MASK | SDHC_IRQSTATEN_CCESEN_MASK | SDHC_IRQSTATEN_CTOESEN_MASK
                         | SDHC_IRQSTATEN_BRRSEN_MASK | SDHC_IRQSTATEN_BWRSEN_MASK | SDHC_IRQSTATEN_CRMSEN_MASK
                         | SDHC_IRQSTATEN_TCSEN_MASK | SDHC_IRQSTATEN_CCSEN_MASK;

    /* 80 initial clocks */
    esdhc_ptr->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
    while (esdhc_ptr->SYSCTL & SDHC_SYSCTL_INITA_MASK)
        { };

    /* Check card */
    if (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_CINS_MASK)
    {
        esdhc_info_ptr->CARD = ESDHC_CARD_UNKNOWN;
    }
    esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;

    return ESDHC_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_send_command
* Returned Value   : 0 on success, 1 on error, -1 on timeout
* Comments         :
*    One ESDHC command transaction.
*
*END*********************************************************************/
static int_32 _esdhc_send_command
    (
        /* [IN/OUT] Module registry pointer */
        SDHC_MemMapPtr          esdhc_ptr,

        /* [IN/OUT] Command specification */
        ESDHC_COMMAND_STRUCT_PTR command
    )
{
    uint_32                      xfertyp;
    uint_32                      blkattr;

    /* Check command */
    xfertyp = command->COMMAND;

    if (ESDHC_XFERTYP_CMDTYP_RESUME == ((xfertyp & SDHC_XFERTYP_CMDTYP_MASK) >> SDHC_XFERTYP_CMDTYP_SHIFT))
    {
        /* DPSEL shall always be set for resume type commands */
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
    }

    if ((0 != command->BLOCKS) && (0 != command->BLOCKSIZE))
    {
        xfertyp |= SDHC_XFERTYP_DPSEL_MASK;
        if (command->BLOCKS != 1)
        {
            /* multiple block transfer */
            xfertyp |= SDHC_XFERTYP_MSBSEL_MASK;
        }
        if ((uint_32)-1 == command->BLOCKS)
        {
            /* infinite transfer */
            blkattr = SDHC_BLKATTR_BLKSIZE(command->BLOCKSIZE) | SDHC_BLKATTR_BLKCNT(0xFFFF);
        }
        else
        {
            blkattr = SDHC_BLKATTR_BLKSIZE(command->BLOCKSIZE) | SDHC_BLKATTR_BLKCNT(command->BLOCKS);
            xfertyp |= SDHC_XFERTYP_BCEN_MASK;
        }
    }
    else
    {
        blkattr = 0;
    }

    /* Card removal check preparation */
    esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;

    /* Wait for cmd line idle */
    while (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_CIHB_MASK)
        { };

    /* Setup command */
    esdhc_ptr->CMDARG = command->ARGUMENT;
    esdhc_ptr->BLKATTR = blkattr;
    esdhc_ptr->DSADDR = 0;

    /* Issue command */
    esdhc_ptr->XFERTYP = xfertyp;

    /* Wait for response */
    if (_esdhc_status_wait (esdhc_ptr, SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK) != SDHC_IRQSTAT_CC_MASK)
    {
        esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK;
        return 1;
    }

    /* Check card removal */
    if (esdhc_ptr->IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
    {
        esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;
        return 1;
    }

    /* Get response, if available */
    if (esdhc_ptr->IRQSTAT & SDHC_IRQSTAT_CTOE_MASK)
    {
        esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;
        return -1;
    }
    if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) != SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_NO))
    {
        command->RESPONSE[0] = esdhc_ptr->CMDRSP[0];
        if ((xfertyp & SDHC_XFERTYP_RSPTYP_MASK) == SDHC_XFERTYP_RSPTYP(ESDHC_XFERTYP_RSPTYP_136))
        {
            command->RESPONSE[1] = esdhc_ptr->CMDRSP[1];
            command->RESPONSE[2] = esdhc_ptr->CMDRSP[2];
            command->RESPONSE[3] = esdhc_ptr->CMDRSP[3];
        }
    }
    esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CC_MASK;

    return 0;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_install
* Returned Value   : MQX error code
* Comments         :
*    Install an ESDHC device.
*
*END*********************************************************************/
_mqx_int _esdhc_install
    (
        /* [IN] A string that identifies the device for fopen */
        char_ptr               identifier,

        /* [IN] The I/O init data pointer */
        ESDHC_INIT_STRUCT_CPTR esdhc_init_ptr
    )
{
    ESDHC_DEVICE_STRUCT_PTR    esdhc_device_ptr;
    _mqx_uint                  result;

    /* Check parameters */
    if ((NULL == identifier) || (NULL == esdhc_init_ptr))
    {
        return MQX_INVALID_PARAMETER;
    }

    /* Check previous installation */
    _int_disable();

    /* Create device context */
    esdhc_device_ptr = _mem_alloc_system_zero ((_mem_size) sizeof (ESDHC_DEVICE_STRUCT));
    if (NULL == esdhc_device_ptr)
    {
        _int_enable();
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (esdhc_device_ptr, MEM_TYPE_IO_ESDHC_DEVICE_STRUCT);

    /* Install device */
    result = _io_dev_install_ext(identifier,
        _esdhc_open,
        _esdhc_close,
        _esdhc_read,
        _esdhc_write,
        _esdhc_ioctl,
        _esdhc_uninstall,
        (pointer)esdhc_device_ptr);

    /* Context initialization or cleanup */
    if (MQX_OK == result)
    {
        esdhc_device_ptr->INIT = esdhc_init_ptr;
        esdhc_device_ptr->COUNT = 0;
    }
    else
    {
        _mem_free (esdhc_device_ptr);
    }
    _int_enable();

    return result;
}


/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall an ESDHC device.
*
*END**********************************************************************/

_mqx_int _esdhc_uninstall
    (
        /* [IN/OUT] The device to uninstall */
        IO_DEVICE_STRUCT_PTR dev_ptr
    )
{
    ESDHC_DEVICE_STRUCT_PTR  esdhc_device_ptr;

    /* Check parameters */
    if (NULL == dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    esdhc_device_ptr = dev_ptr->DRIVER_INIT_PTR;
    if (NULL == esdhc_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Uninstall only when not opened */
    if (esdhc_device_ptr->COUNT)
    {
        return IO_ERROR_DEVICE_BUSY;
    }

    /* Cleanup */
    _mem_free (esdhc_device_ptr);
    dev_ptr->DRIVER_INIT_PTR = NULL;

    return IO_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_open
* Returned Value   : MQX error code
* Comments         :
*    This function exclusively opens the ESDHC device.
*
*END*********************************************************************/
_mqx_int _esdhc_open
    (
        /* [IN/OUT] ESDHC file descriptor */
        MQX_FILE_PTR        esdhc_fd_ptr,

        /* [IN] The remaining portion of the name of the device */
        char_ptr            open_name_ptr,

        /* [IN] The flags to be used during operation */
        char_ptr            open_flags_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    ESDHC_INFO_STRUCT_PTR   esdhc_info_ptr;
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr;
    ESDHC_INIT_STRUCT_CPTR  esdhc_init_ptr;
    SDHC_MemMapPtr          esdhc_ptr;

    /* Check parameters */
    if (NULL == esdhc_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = esdhc_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    esdhc_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == esdhc_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    esdhc_init_ptr = esdhc_device_ptr->INIT;
    if (NULL == esdhc_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Get register base */
    esdhc_ptr = _bsp_get_esdhc_base_address (esdhc_init_ptr->CHANNEL);
    if (NULL == esdhc_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Exclusive access till close */
    _int_disable();
    if (esdhc_device_ptr->COUNT)
    {
        /* Device is already opened */
        _int_enable();
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }
    esdhc_device_ptr->COUNT = 1;
    _int_enable();

    /* Set device info */
    esdhc_info_ptr = (ESDHC_INFO_STRUCT_PTR)_mem_alloc_system_zero ((_mem_size) sizeof (ESDHC_INFO_STRUCT));
    if (NULL == esdhc_info_ptr)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (esdhc_info_ptr, MEM_TYPE_IO_ESDHC_INFO_STRUCT);
    esdhc_fd_ptr->DEV_DATA_PTR = esdhc_info_ptr;

    return _esdhc_init (esdhc_ptr, esdhc_info_ptr, esdhc_init_ptr);
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_close
* Returned Value   : MQX error code
* Comments         :
*    This function closes opened ESDHC device.
*
*END*********************************************************************/
_mqx_int _esdhc_close
    (
        /* [IN/OUT] Opened file pointer for ESDHC */
        MQX_FILE_PTR        esdhc_fd_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr;
    ESDHC_INIT_STRUCT_CPTR  esdhc_init_ptr;
    SDHC_MemMapPtr         esdhc_ptr;

    /* Check parameters */
    if (NULL == esdhc_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)esdhc_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    esdhc_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == esdhc_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    esdhc_init_ptr = esdhc_device_ptr->INIT;
    if (NULL == esdhc_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Get register base */
    esdhc_ptr = _bsp_get_esdhc_base_address (esdhc_init_ptr->CHANNEL);
    if (NULL == esdhc_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Disable ESDHC device */
    _int_disable();
    if (--esdhc_device_ptr->COUNT == 0)
    {
        esdhc_ptr->SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);
        while (esdhc_ptr->SYSCTL & SDHC_SYSCTL_RSTA_MASK)
            { };

        _mem_free (esdhc_fd_ptr->DEV_DATA_PTR);
        esdhc_fd_ptr->DEV_DATA_PTR = NULL;
    }
    _int_enable();

    return ESDHC_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_ioctl
* Returned Value   : MQX error code
* Comments         :
*    This function performs miscellaneous services for the ESDHC I/O device.
*
*END*********************************************************************/
_mqx_int _esdhc_ioctl
    (
        /* [IN] Opened file pointer for ESDHC */
        MQX_FILE_PTR        esdhc_fd_ptr,

        /* [IN] The command to perform */
        uint_32             cmd,

        /* [IN/OUT] Parameters for the command */
        pointer             param_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    ESDHC_INFO_STRUCT_PTR   esdhc_info_ptr;
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr;
    ESDHC_INIT_STRUCT_CPTR  esdhc_init_ptr;
    SDHC_MemMapPtr          esdhc_ptr;
    ESDHC_COMMAND_STRUCT    command;
    boolean                 mem, io, mmc, ceata, mp, hc;
    int_32                  val;
    uint_32                 result = ESDHC_OK;
    uint_32_ptr             param32_ptr = param_ptr;

    /* Check parameters */
    if (NULL == esdhc_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)esdhc_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    esdhc_info_ptr = (ESDHC_INFO_STRUCT_PTR)esdhc_fd_ptr->DEV_DATA_PTR;
    if (NULL == esdhc_info_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    esdhc_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == esdhc_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    esdhc_init_ptr = esdhc_device_ptr->INIT;
    if (NULL == esdhc_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Get register base */
    esdhc_ptr = _bsp_get_esdhc_base_address (esdhc_init_ptr->CHANNEL);
    if (NULL == esdhc_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    switch (cmd)
    {
        case IO_IOCTL_ESDHC_INIT:

            result = _esdhc_init (esdhc_ptr, esdhc_info_ptr, esdhc_init_ptr);
            if (ESDHC_OK != result)
            {
                break;
            }

            mem = FALSE;
            io = FALSE;
            mmc = FALSE;
            ceata = FALSE;
            hc = FALSE;
            mp = FALSE;

            /* CMD0 - Go to idle - reset card */
            command.COMMAND = ESDHC_CMD0;
            command.ARGUMENT = 0;
            command.BLOCKS = 0;
            if (_esdhc_send_command (esdhc_ptr, &command))
            {
                result = ESDHC_ERROR_INIT_FAILED;
                break;
            }

            /* CMD8 - Send interface condition - check HC support */
            command.COMMAND = ESDHC_CMD8;
            command.ARGUMENT = 0x000001AA;
            command.BLOCKS = 0;
            val = _esdhc_send_command (esdhc_ptr, &command);
            if (val > 0)
            {
                result = ESDHC_ERROR_INIT_FAILED;
                break;
            }
            if (val == 0)
            {
                if (command.RESPONSE[0] != command.ARGUMENT)
                {
                    result = ESDHC_ERROR_INIT_FAILED;
                    break;
                }
                hc = TRUE;
            }

            /* CMD5 - Send operating conditions - test IO */
            command.COMMAND = ESDHC_CMD5;
            command.ARGUMENT = 0;
            command.BLOCKS = 0;
            val = _esdhc_send_command (esdhc_ptr, &command);
            if (val > 0)
            {
                result = ESDHC_ERROR_INIT_FAILED;
                break;
            }
            if (val == 0)
            {
                if (((command.RESPONSE[0] >> 28) & 0x07) && (command.RESPONSE[0] & 0x300000))
                {
                    /* CMD5 - Send operating conditions - init IO */
                    command.COMMAND = ESDHC_CMD5;
                    command.ARGUMENT = 0x300000;
                    command.BLOCKS = 0;
                    val = 0;
                    do
                    {
                        _time_delay (BSP_ALARM_RESOLUTION);
                        val++;
                        if (_esdhc_send_command (esdhc_ptr, &command))
                        {
                            result = ESDHC_ERROR_INIT_FAILED;
                            break;
                        }
                    } while ((0 == (command.RESPONSE[0] & 0x80000000)) && (val < BSP_ALARM_FREQUENCY));
                    if (ESDHC_OK != result)
                    {
                        break;
                    }
                    if (command.RESPONSE[0] & 0x80000000)
                    {
                        io = TRUE;
                    }
                    if (command.RESPONSE[0] & 0x08000000)
                    {
                        mp = TRUE;
                    }
                }
            }
            else
            {
                mp = TRUE;
            }

            if (mp)
            {
                /* CMD55 - Application specific command - check MMC */
                command.COMMAND = ESDHC_CMD55;
                command.ARGUMENT = 0;
                command.BLOCKS = 0;
                val = _esdhc_send_command (esdhc_ptr, &command);
                if (val > 0)
                {
                    result = ESDHC_ERROR_INIT_FAILED;
                    break;
                }
                if (val < 0)
                {
                    /* MMC or CE-ATA */
                    io = FALSE;
                    mem = FALSE;
                    hc = FALSE;

                    /* CMD1 - Send operating conditions - check HC */
                    command.COMMAND = ESDHC_CMD1;
                    command.ARGUMENT = 0x40300000;
                    command.BLOCKS = 0;
                    if (_esdhc_send_command (esdhc_ptr, &command))
                    {
                        result = ESDHC_ERROR_INIT_FAILED;
                        break;
                    }
                    if (0x20000000 == (command.RESPONSE[0] & 0x60000000))
                    {
                        hc = TRUE;
                    }
                    mmc = TRUE;

                    /* CMD39 - Fast IO - check CE-ATA signature CE */
                    command.COMMAND = ESDHC_CMD39;
                    command.ARGUMENT = 0x0C00;
                    command.BLOCKS = 0;
                    if (_esdhc_send_command (esdhc_ptr, &command))
                    {
                        result = ESDHC_ERROR_INIT_FAILED;
                        break;
                    }
                    if (0xCE == ((command.RESPONSE[0] >> 8) & 0xFF))
                    {
                        /* CMD39 - Fast IO - check CE-ATA signature AA */
                        command.COMMAND = ESDHC_CMD39;
                        command.ARGUMENT = 0x0D00;
                        command.BLOCKS = 0;
                        if (_esdhc_send_command (esdhc_ptr, &command))
                        {
                            result = ESDHC_ERROR_INIT_FAILED;
                            break;
                        }
                        if (0xAA == ((command.RESPONSE[0] >> 8) & 0xFF))
                        {
                            mmc = FALSE;
                            ceata = TRUE;
                        }
                    }
                }
                else
                {
                    /* SD */
                    /* ACMD41 - Send Operating Conditions */
                    command.COMMAND = ESDHC_ACMD41;
                    command.ARGUMENT = 0;
                    command.BLOCKS = 0;
                    if (_esdhc_send_command (esdhc_ptr, &command))
                    {
                        result = ESDHC_ERROR_INIT_FAILED;
                        break;
                    }
                    if (command.RESPONSE[0] & 0x300000)
                    {
                        val = 0;
                        do
                        {
                            _time_delay (BSP_ALARM_RESOLUTION);
                            val++;

                            /* CMD55 + ACMD41 - Send OCR */
                            command.COMMAND = ESDHC_CMD55;
                            command.ARGUMENT = 0;
                            command.BLOCKS = 0;
                            if (_esdhc_send_command (esdhc_ptr, &command))
                            {
                                result = ESDHC_ERROR_INIT_FAILED;
                                break;
                            }

                            command.COMMAND = ESDHC_ACMD41;
                            if (hc)
                            {
                                command.ARGUMENT = 0x40300000;
                            }
                            else
                            {
                                command.ARGUMENT = 0x00300000;
                            }
                            command.BLOCKS = 0;
                            if (_esdhc_send_command (esdhc_ptr, &command))
                            {
                                result = ESDHC_ERROR_INIT_FAILED;
                                break;
                            }
                        } while ((0 == (command.RESPONSE[0] & 0x80000000)) && (val < BSP_ALARM_FREQUENCY));
                        if (ESDHC_OK != result)
                        {
                            break;
                        }
                        if (val >= BSP_ALARM_FREQUENCY)
                        {
                            hc = FALSE;
                        }
                        else
                        {
                            mem = TRUE;
                            if (hc)
                            {
                                hc = FALSE;
                                if (command.RESPONSE[0] & 0x40000000)
                                {
                                    hc = TRUE;
                                }
                            }
                        }
                    }
                }
            }
            if (mmc)
            {
                esdhc_info_ptr->CARD = ESDHC_CARD_MMC;
            }
            if (ceata)
            {
                esdhc_info_ptr->CARD = ESDHC_CARD_CEATA;
            }
            if (io)
            {
                esdhc_info_ptr->CARD = ESDHC_CARD_SDIO;
            }
            if (mem)
            {
                esdhc_info_ptr->CARD = ESDHC_CARD_SD;
                if (hc)
                {
                    esdhc_info_ptr->CARD = ESDHC_CARD_SDHC;
                }
            }
            if (io && mem)
            {
                esdhc_info_ptr->CARD = ESDHC_CARD_SDCOMBO;
                if (hc)
                {
                    esdhc_info_ptr->CARD = ESDHC_CARD_SDHCCOMBO;
                }
            }

            /* De-Init GPIO */
            if (_bsp_esdhc_io_init (esdhc_init_ptr->CHANNEL, 0) == -1)
            {
                return IO_ERROR_DEVICE_INVALID;
            }

            /* Set the ESDHC default baud rate */
            _esdhc_set_baudrate (esdhc_ptr, esdhc_init_ptr->CLOCK_SPEED, esdhc_init_ptr->BAUD_RATE);

            /* Init GPIO */
            if (_bsp_esdhc_io_init (esdhc_init_ptr->CHANNEL, 0xFFFF) == -1)
            {
                return IO_ERROR_DEVICE_INVALID;
            }
            break;
        case IO_IOCTL_ESDHC_SEND_COMMAND:
            val = _esdhc_send_command (esdhc_ptr, (ESDHC_COMMAND_STRUCT_PTR)param32_ptr);
            if (val > 0)
            {
                result = ESDHC_ERROR_COMMAND_FAILED;
            }
            if (val < 0)
            {
                result = ESDHC_ERROR_COMMAND_TIMEOUT            ;
            }
            break;
        case IO_IOCTL_ESDHC_GET_BAUDRATE:
            if (NULL == param32_ptr)
            {
                result = MQX_INVALID_PARAMETER;
            }
            else
            {
                /* Get actual baudrate */
                val = ((esdhc_ptr->SYSCTL & SDHC_SYSCTL_SDCLKFS_MASK) >> SDHC_SYSCTL_SDCLKFS_SHIFT) << 1;
                val *= ((esdhc_ptr->SYSCTL & SDHC_SYSCTL_DVS_MASK) >> SDHC_SYSCTL_DVS_SHIFT) + 1;
                *param32_ptr = (uint_32)((esdhc_init_ptr->CLOCK_SPEED) / val);
            }
            break;
        case IO_IOCTL_ESDHC_SET_BAUDRATE:
            if (NULL == param32_ptr)
            {
                result = MQX_INVALID_PARAMETER;
            }
            else if (0 == (*param32_ptr))
            {
                result = MQX_INVALID_PARAMETER;
            }
            else
            {
                if (! _esdhc_is_running (esdhc_ptr))
                {
                    /* De-Init GPIO */
                    if (_bsp_esdhc_io_init (esdhc_init_ptr->CHANNEL, 0) == -1)
                    {
                        result = IO_ERROR_DEVICE_INVALID;
                        break;
                    }

                    /* Set closest baudrate */
                    _esdhc_set_baudrate (esdhc_ptr, esdhc_init_ptr->CLOCK_SPEED, *param32_ptr);

                    /* Init GPIO */
                    if (_bsp_esdhc_io_init (esdhc_init_ptr->CHANNEL, 0xFFFF) == -1)
                    {
                        result = IO_ERROR_DEVICE_INVALID;
                        break;
                    }
                }
                else
                {
                    result = IO_ERROR_DEVICE_BUSY;
                }
            }
            break;
        case IO_IOCTL_ESDHC_GET_BUS_WIDTH:
            if (NULL == param32_ptr)
            {
                result = MQX_INVALID_PARAMETER;
            }
            else
            {
                /* Get actual ESDHC bus width */
                val = (esdhc_ptr->PROCTL & SDHC_PROCTL_DTW_MASK) >> SDHC_PROCTL_DTW_SHIFT;
                if (ESDHC_PROCTL_DTW_1BIT == val)
                {
                    *param32_ptr = ESDHC_BUS_WIDTH_1BIT;
                }
                else if (ESDHC_PROCTL_DTW_4BIT == val)
                {
                    *param32_ptr = ESDHC_BUS_WIDTH_4BIT;
                }
                else if (ESDHC_PROCTL_DTW_8BIT == val)
                {
                    *param32_ptr = ESDHC_BUS_WIDTH_8BIT;
                }
                else
                {
                    result = ESDHC_ERROR_INVALID_BUS_WIDTH;
                }
            }
            break;
        case IO_IOCTL_ESDHC_SET_BUS_WIDTH:
            if (NULL == param32_ptr)
            {
                result = MQX_INVALID_PARAMETER;
            }
            else
            {
                /* Set actual ESDHC bus width */
                if (! _esdhc_is_running (esdhc_ptr))
                {
                    if (ESDHC_BUS_WIDTH_1BIT == *param32_ptr)
                    {
                        esdhc_ptr->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
                        esdhc_ptr->PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_1BIT);
                    }
                    else if (ESDHC_BUS_WIDTH_4BIT == *param32_ptr)
                    {
                        esdhc_ptr->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
                        esdhc_ptr->PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_4BIT);
                    }
                    else if (ESDHC_BUS_WIDTH_8BIT == *param32_ptr)
                    {
                        esdhc_ptr->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
                        esdhc_ptr->PROCTL |= SDHC_PROCTL_DTW(ESDHC_PROCTL_DTW_8BIT);
                    }
                    else
                    {
                        result = ESDHC_ERROR_INVALID_BUS_WIDTH;
                    }
                }
                else
                {
                    result = IO_ERROR_DEVICE_BUSY;
                }
            }
            break;
        case IO_IOCTL_ESDHC_GET_CARD:
            if (NULL == param32_ptr)
            {
                result = MQX_INVALID_PARAMETER;
            }
            else
            {
                /* 80 clocks to update levels */
                esdhc_ptr->SYSCTL |= SDHC_SYSCTL_INITA_MASK;
                while (esdhc_ptr->SYSCTL & SDHC_SYSCTL_INITA_MASK)
                    { };

                /* Update and return actual card status */
                if (esdhc_ptr->IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
                {
                    esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;
                    esdhc_info_ptr->CARD = ESDHC_CARD_NONE;
                }
                if (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_CINS_MASK)
                {
                    if (ESDHC_CARD_NONE == esdhc_info_ptr->CARD)
                    {
                        esdhc_info_ptr->CARD = ESDHC_CARD_UNKNOWN;
                    }
                }
                else
                {
                    esdhc_info_ptr->CARD = ESDHC_CARD_NONE;
                }
                *param32_ptr = esdhc_info_ptr->CARD;
            }
            break;
        case IO_IOCTL_DEVICE_IDENTIFY:
            /* Get ESDHC device parameters */
            param32_ptr[IO_IOCTL_ID_PHY_ELEMENT]  = IO_DEV_TYPE_PHYS_ESDHC;
            param32_ptr[IO_IOCTL_ID_LOG_ELEMENT]  = IO_DEV_TYPE_LOGICAL_MFS;
            param32_ptr[IO_IOCTL_ID_ATTR_ELEMENT] = IO_ESDHC_ATTRIBS;
            if (esdhc_fd_ptr->FLAGS & IO_O_RDONLY)
            {
                param32_ptr[IO_IOCTL_ID_ATTR_ELEMENT] &= (~ IO_DEV_ATTR_WRITE);
            }
            break;
        case IO_IOCTL_FLUSH_OUTPUT:
            /* Wait for transfer complete */
            _esdhc_status_wait (esdhc_ptr, SDHC_IRQSTAT_TC_MASK);
            if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK;
                result = ESDHC_ERROR_DATA_TRANSFER;
            }
            esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_TC_MASK | SDHC_IRQSTAT_BRR_MASK | SDHC_IRQSTAT_BWR_MASK;
            break;
        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }
    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_read
* Returned Value   : Returns the number of bytes received or IO_ERROR
* Comments         :
*   Reads the data into provided array.
*
*END*********************************************************************/
_mqx_int _esdhc_read
    (
        /* [IN] Opened file pointer for ESDHC */
        FILE_DEVICE_STRUCT_PTR esdhc_fd_ptr,

        /* [OUT] Where the characters are to be stored */
        char_ptr               data_ptr,

        /* [IN] The number of bytes to read */
        _mqx_int               n
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    ESDHC_INFO_STRUCT_PTR   esdhc_info_ptr;
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr;
    ESDHC_INIT_STRUCT_CPTR  esdhc_init_ptr;
    SDHC_MemMapPtr          esdhc_ptr;
    uint_32                 buffer;
    _mqx_int                remains;
    
    /* Check parameters */
    if (NULL == esdhc_fd_ptr)
    {
        return IO_ERROR;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)esdhc_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_ERROR;
    }
    esdhc_info_ptr = (ESDHC_INFO_STRUCT_PTR)esdhc_fd_ptr->DEV_DATA_PTR;
    if (NULL == esdhc_info_ptr)
    {
        return IO_ERROR;
    }
    esdhc_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == esdhc_device_ptr)
    {
        return IO_ERROR;
    }
    esdhc_init_ptr = esdhc_device_ptr->INIT;
    if (NULL == esdhc_init_ptr)
    {
        return IO_ERROR;
    }

    /* Get register base */
    esdhc_ptr = _bsp_get_esdhc_base_address (esdhc_init_ptr->CHANNEL);
    if (NULL == esdhc_ptr)
    {
        return IO_ERROR;
    }

    remains = n;
    if (((uint_32)data_ptr & 0x03) == 0)
    {
        /* data is word aligned, maximize speed by copying directly from the register */      

        while (remains >= 4)
        {
            if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
                return IO_ERROR;
            }

            /* wait until amount of available data reaches the watermark or the transfer is finished */
            while ((0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) && (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_DLA_MASK))
                { };

            *((uint_32 *)data_ptr) = esdhc_ptr->DATPORT;
            data_ptr += 4;
            remains -= 4;
        }
    }
    else
    {
        /* missaligned data, read to temporary location and copy byte by byte */

        while (remains >= 4)
        {
            if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
                return IO_ERROR;
            }

            /* wait until amount of available data reaches the watermark or the transfer is finished */
            while ((0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) && (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_DLA_MASK))
                { };

            buffer = esdhc_ptr->DATPORT;
#if PSP_ENDIAN == MQX_BIG_ENDIAN
            *data_ptr++ = (buffer >> 24) & 0xFF;
            *data_ptr++ = (buffer >> 16) & 0xFF;
            *data_ptr++ = (buffer >> 8) & 0xFF;
            *data_ptr++ = buffer & 0xFF;
#else
            *data_ptr++ = buffer & 0xFF;
            *data_ptr++ = (buffer >> 8) & 0xFF;
            *data_ptr++ = (buffer >> 16) & 0xFF;
            *data_ptr++ = (buffer >> 24) & 0xFF;
#endif
            remains -= 4;
        }      
    }

    if (remains)
    {
        /* there is less than a single word left for sure */
      
        if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
        {
            esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
            return IO_ERROR;
        }

        /* wait until amount of available data reaches the watermark or the transfer is finished */
        while ((0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BREN_MASK)) && (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_DLA_MASK))
            { };
        
        buffer = esdhc_ptr->DATPORT;
        while (remains)
        {
#if PSP_ENDIAN == MQX_BIG_ENDIAN
            /* assumes left justified data */
            *data_ptr++ = (buffer >> 24) & 0xFF;
            buffer <<= 8;
#else
            *data_ptr++ = buffer & 0xFF;
            buffer >>= 8;
#endif
            remains--;
        }
    }

    if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
    {
        esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BRR_MASK;
        return IO_ERROR;
    }

    return (n - remains);
}

/*FUNCTION****************************************************************
*
* Function Name    : _esdhc_write
* Returned Value   : return number of byte transmitted or IO_ERROR
* Comments         :
*   Writes the provided data buffer to the device.
*
*END*********************************************************************/
_mqx_int _esdhc_write
    (
        /* [IN] Opened file pointer for ESDHC */
        FILE_DEVICE_STRUCT_PTR esdhc_fd_ptr,

        /* [OUT] Where the characters are to be taken from */
        char_ptr               data_ptr,

        /* [IN] The number of bytes to read */
        _mqx_int               n
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    ESDHC_INFO_STRUCT_PTR   esdhc_info_ptr;
    ESDHC_DEVICE_STRUCT_PTR esdhc_device_ptr;
    ESDHC_INIT_STRUCT_CPTR  esdhc_init_ptr;
    SDHC_MemMapPtr          esdhc_ptr;
    uint_8_ptr              udata_ptr;
    uint_32                 buffer;
    _mqx_int                remains;

    /* Check parameters */
    if (NULL == esdhc_fd_ptr)
    {
        return IO_ERROR;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)esdhc_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_ERROR;
    }
    esdhc_info_ptr = (ESDHC_INFO_STRUCT_PTR)esdhc_fd_ptr->DEV_DATA_PTR;
    if (NULL == esdhc_info_ptr)
    {
        return IO_ERROR;
    }
    esdhc_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == esdhc_device_ptr)
    {
        return IO_ERROR;
    }
    esdhc_init_ptr = esdhc_device_ptr->INIT;
    if (NULL == esdhc_init_ptr)
    {
        return IO_ERROR;
    }

    /* Get register base */
    esdhc_ptr = _bsp_get_esdhc_base_address (esdhc_init_ptr->CHANNEL);
    if (NULL == esdhc_ptr)
    {
        return IO_ERROR;
    }

    /* pointer to unsigned data */
    udata_ptr = (uint_8_ptr)data_ptr;

    remains = n;
    if (((uint_32)udata_ptr & 0x03) == 0)
    {
        /* data is word aligned, maximize speed by copying directly to the register */      

        while (remains >= 4)
        {
            if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
                return IO_ERROR;
            }

            /* wait until at least watermark space is available */
            while (0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
                { };

            esdhc_ptr->DATPORT = *((uint_32 *)udata_ptr);
            udata_ptr += 4;
            remains -= 4;
        }
    }
    else
    {
        /* missaligned data, read to temporary location and copy byte by byte */

        while (remains >= 4)
        {
            if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
            {
                esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
                return IO_ERROR;
            }

            /* wait until at least watermark space is available */
            while (0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
                { };

#if PSP_ENDIAN == MQX_BIG_ENDIAN
            buffer  = (*udata_ptr++) << 24;
            buffer |= (*udata_ptr++) << 16;
            buffer |= (*udata_ptr++) << 8;
            buffer |= (*udata_ptr++);
#else
            buffer  = (*udata_ptr++);
            buffer |= (*udata_ptr++) << 8;
            buffer |= (*udata_ptr++) << 16;
            buffer |= (*udata_ptr++) << 24;
#endif

            /* wait until at least watermark space is available */
            while (0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
                { };
            
            esdhc_ptr->DATPORT = buffer;
            remains -= 4;
        }      
    }
    
    if (remains)
    {
        /* there is less than a single word left for sure */
  
        if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
        {
            esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
            return IO_ERROR;
        }
      
        buffer = 0xFFFFFFFF;
        while (remains)
        {
#if PSP_ENDIAN == MQX_BIG_ENDIAN
            /* assumes left justified data */
            buffer >>= 8;
            buffer |= (udata_ptr[remains] << 24);
#else
            buffer <<= 8;
            buffer |= udata_ptr[remains];
#endif
            remains--;
        }

        /* wait until at least watermark space is available */
        while (0 == (esdhc_ptr->PRSSTAT & SDHC_PRSSTAT_BWEN_MASK))
            { };

        esdhc_ptr->DATPORT = buffer;        
    }

    if (esdhc_ptr->IRQSTAT & (SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK))
    {
        esdhc_ptr->IRQSTAT |= SDHC_IRQSTAT_DEBE_MASK | SDHC_IRQSTAT_DCE_MASK | SDHC_IRQSTAT_DTOE_MASK | SDHC_IRQSTAT_BWR_MASK;
        return IO_ERROR;
    }

    return (n - remains);
}
/* EOF */
