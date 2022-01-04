/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: debug.c$
* $Version : 3.8.3.0$
* $Date    : Sep-16-2011$
*
* Comments:
*
*   This file contains the debug driver functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "iodebug.h"
#include "io_prv.h"
#include "iodebug_prv.h"




/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name     : _io_debug_install
* Returned Value    : _mqx_uint a task error code or MQX_OK
* Comments          :
*    Install a debug driver.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_debug_install 
    ( 
        /* [IN] A string that identifies the device for fopen */ 
        char_ptr                    identifier, 

        /* [IN] pointer to init structure */ 
        IODEBUG_INIT_STRUCT_CPTR    init_ptr 
    ) 
{ /* Body */
    _mqx_int                        result = IO_OK; 
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr;
    
    dev_ptr = _mem_alloc_system_zero((_mem_size)sizeof(IO_DEBUG_DEVICE_STRUCT));
    
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if(dev_ptr == NULL) {
        return(MQX_OUT_OF_MEMORY); 
    } /* Endif */
#endif

    dev_ptr->BUFFER_PTR = NULL;
    dev_ptr->BUFFER_POS = 0;
    dev_ptr->COUNT = 0;
    dev_ptr->DEV_INIT_DATA_PTR = (IODEBUG_INIT_STRUCT_PTR)init_ptr;
    dev_ptr->FLUSH_CHAR = init_ptr->FLUSH_CHAR; 
    _lwsem_create(&dev_ptr->LWSEM, 1);
    result = _io_dev_install_ext(identifier,
        _io_debug_open,
        _io_debug_close,
        _io_debug_read,
        _io_debug_write,
        _io_debug_ioctl,
        _io_debug_uninstall,
        dev_ptr); 
    
    return result; 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Uninstall a debug driver.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_debug_uninstall 
    ( 
        /* [IN] The devices context */
        IO_DEVICE_STRUCT_PTR  io_dev_ptr
    )
{ /* Body */
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    
    if(dev_ptr->COUNT == 0) { 
        if(dev_ptr->BUFFER_PTR != NULL) { 
            _mem_free(dev_ptr->BUFFER_PTR); 
        } /* Endif */
        _lwsem_destroy(&dev_ptr->LWSEM);
        _mem_free(dev_ptr);
        return(IO_OK); 
    } else { 
        return(IO_ERROR_DEVICE_BUSY); 
    } /* Endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_open
* Returned Value   : error code
* Comments         : 
*    Opens and initializes driver.
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_debug_open
    (
        /* [IN] the file handle for the device being opened */
        MQX_FILE_PTR    fd_ptr,
        
        /* [IN] the remaining portion of the name of the device */
        char_ptr        open_name_ptr,
        
        /* [IN] the flags to be used during operation: */
        char_ptr        flags
    )
{ /* Body */
    _mqx_uint                       result = MQX_OK; 
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IODEBUG_INIT_STRUCT_PTR         init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    
    _lwsem_wait(&dev_ptr->LWSEM);
    /* 
    ** First file open && buffer required 
    */  
    if(dev_ptr->COUNT == 0 && init_ptr->MODE == IODEBUG_MODE_SEMIHOST  && 
        init_ptr->DATA_LENGTH) {
        dev_ptr->BUFFER_PTR = _mem_alloc_system_zero( 
            (_mem_size)(sizeof(char) * (init_ptr->DATA_LENGTH + 1)) 
        ); 
        #if MQX_CHECK_MEMORY_ALLOCATION_ERRORS 
        if(dev_ptr->BUFFER_PTR == NULL) { 
            return(MQX_OUT_OF_MEMORY); 
        } /* Endif */
        #endif 
        dev_ptr->COUNT = 1;
    }
    else {
        dev_ptr->COUNT++;
    } /* Endif */
    _lwsem_post(&dev_ptr->LWSEM);
    
    return result; 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_close
* Returned Value   : error code
* Comments         : 
*    Closes debug driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_debug_close
    (
        /* [IN] the file handle for the device being closed */
        MQX_FILE_PTR    fd_ptr
    )
{ /* Body */
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IODEBUG_INIT_STRUCT_PTR         init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    
    _lwsem_wait(&dev_ptr->LWSEM);
    if(dev_ptr->COUNT == 1 && dev_ptr->BUFFER_PTR != NULL) { 
        _io_debug_buffer_flush(dev_ptr);
        _mem_free(dev_ptr->BUFFER_PTR); 
        dev_ptr->BUFFER_PTR = NULL; 
    } /* Endif */
    dev_ptr->COUNT--; 
    _lwsem_post(&dev_ptr->LWSEM);
    
    return(MQX_OK);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_read
* Returned Value   : number of characters read
* Comments         : 
*    Reads data from debug device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_debug_read
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR    fd_ptr,

    /* [IN] where the characters are to be stored */
    char_ptr        data_ptr,

    /* [IN] the number of characters to input */
    _mqx_int        num
)
{ /* Body */
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IODEBUG_INIT_STRUCT_PTR         init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    uint_32                         i = 0;
    
    _lwsem_wait(&dev_ptr->LWSEM);
    /* File not open */
    if(dev_ptr->COUNT <= 0) {
        return IO_ERROR; 
    } /* Endif */
    
    switch(init_ptr->MODE){
        case IODEBUG_MODE_SEMIHOST :
            for(i = 0; i < num; i++) {
                data_ptr[i] = _io_debug_semi_read_char();
            } /* Endfor */
            break;
        default:
            num = IO_ERROR;
            break;
    }/* Endswitch */
    _lwsem_post(&dev_ptr->LWSEM);
    
    return(num);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_write
* Returned Value   : number of characters written
* Comments         : \
*    Writes data to the debug device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_debug_write
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR    fd_ptr,

    /* [IN] where the characters are */
    char_ptr        data_ptr,

    /* [IN] the number of characters to output */
    _mqx_int        num
)
{ /* Body */
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IODEBUG_INIT_STRUCT_PTR         init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    int_32                          i = 0; 
    int_32                          num_success = num; 
    
    _lwsem_wait(&dev_ptr->LWSEM);
    /* File not open */
    if(dev_ptr->COUNT <= 0) {
        return IO_ERROR; 
    } /* Endif */
    
    switch(init_ptr->MODE) {
        case IODEBUG_MODE_ITM:
            for(i = 0; i < num; i++) { 
                if(_io_debug_itm_write_char(&data_ptr[i]) == FALSE) {
                    num_success--;
                } /* Endif */
            } /* Endfor */
            break;
        case IODEBUG_MODE_SEMIHOST :
            if(init_ptr->DATA_LENGTH) {
                for(i = 0; i < num; i++) { 
                    _io_debug_semi_buffer_write(dev_ptr, &data_ptr[i]);
                } /* Endfor */
            } else {
                for(i = 0; i < num; i++) { 
                    _io_debug_semi_write_char(&data_ptr[i]);
                } /* Endfor */
            } /* Endif */
            break;
        default:
            num_success = IO_ERROR;
            break;
    } /* Endswitch */
    _lwsem_post(&dev_ptr->LWSEM);
    
    return(num_success); 
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_debug_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_debug_ioctl
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR    fd_ptr,

    /* [IN] the ioctl command */
    _mqx_uint       cmd,

    /* [IN] the ioctl parameters */
    pointer         param_ptr
)
{ /* Body */
    IO_DEBUG_DEVICE_STRUCT_PTR      dev_ptr = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    IODEBUG_INIT_STRUCT_PTR         init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    _mqx_int                        result = MQX_OK;
    
    _lwsem_wait(&dev_ptr->LWSEM);
    switch(cmd) { 
    case IO_IOCTL_FLUSH_OUTPUT: 
        _io_debug_buffer_flush(dev_ptr);
        break; 
    case IO_IOCTL_IODEBUG_SET_FLUSH_CHAR:
        dev_ptr->FLUSH_CHAR = (*(char*)param_ptr);
        break;
    default: 
        result = IO_ERROR_INVALID_IOCTL_CMD; 
    } /* Endswitch */
    _lwsem_post(&dev_ptr->LWSEM);
    
    return result; 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_buffer_flush
* Returned Value   : none
* Comments         : 
*    Write character to buffer.
* 
*END*----------------------------------------------------------------------*/

void _io_debug_buffer_flush
    (
        /* [IN] the file handle for the device */
        IO_DEBUG_DEVICE_STRUCT_PTR  dev_ptr
    )
{
    IODEBUG_INIT_STRUCT_PTR     init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    
    if(dev_ptr->COUNT && init_ptr->MODE == IODEBUG_MODE_SEMIHOST  && 
        init_ptr->DATA_LENGTH) {
        _io_debug_semi_buffer_flush(dev_ptr);
    } /* Endif */
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_semi_buffer_flush
* Returned Value   : none
* Comments         : 
*    Flushs semihost buffer. In current position writes '\0', 
*    call _io_debug_semi_write_string and resets buffer position.
* 
*END*----------------------------------------------------------------------*/

void _io_debug_semi_buffer_flush
    (
        /* [IN] the file handle for the device */
        IO_DEBUG_DEVICE_STRUCT_PTR  dev_ptr
    )
{
    /* IAR will crash down message is "\0" */
    if (dev_ptr->BUFFER_POS) {
        dev_ptr->BUFFER_PTR[dev_ptr->BUFFER_POS] = '\0';
        _io_debug_semi_write_string(dev_ptr->BUFFER_PTR);
        dev_ptr->BUFFER_POS = 0;
    }
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_semi_buffer_write
* Returned Value   : none
* Comments         : 
*    Write character to buffer.
* 
*END*----------------------------------------------------------------------*/

void _io_debug_semi_buffer_write
    (
        /* [IN] the file handle for the device */
        IO_DEBUG_DEVICE_STRUCT_PTR  dev_ptr,

        /* [IN] character to write */
        char_ptr                     character_ptr
    )
{
    IODEBUG_INIT_STRUCT_PTR     init_ptr = dev_ptr->DEV_INIT_DATA_PTR;
    
    dev_ptr->BUFFER_PTR[dev_ptr->BUFFER_POS] = *character_ptr;
    dev_ptr->BUFFER_POS++;
    if((init_ptr->DATA_LENGTH <= dev_ptr->BUFFER_POS) || ( 
        dev_ptr->BUFFER_PTR[dev_ptr->BUFFER_POS - 1] == dev_ptr->FLUSH_CHAR && 
        dev_ptr->FLUSH_CHAR != IODEBUG_NOFLUSH_CHAR 
    )){
        _io_debug_semi_buffer_flush(dev_ptr);
    } /* Endif */
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_debug_itm_write_char
* Returned Value   : returns TRUE on success else FALSE
* Comments         : 
*    Sends character to ITM channel 0.
* 
*END*----------------------------------------------------------------------*/

boolean _io_debug_itm_write_char
    (
        /* [IN] character to write */
        char_ptr                    character_ptr
    )
{
    if(
        (DEMCR & (1 << 24)) &&          /* TRCENA reg on - trace system enable*/
        (ITM_BASE_PTR->TER & 0x1)       /* TER channel 0 - channel 0 enable */
    ) {
        while(!ITM_BASE_PTR->STIM_WRITE[0]) {
            _ASM_NOP();
        }
        ITM_BASE_PTR->STIM_WRITE[0] = *character_ptr;
        return TRUE;
    }
    return FALSE;
}


/* EOF */
