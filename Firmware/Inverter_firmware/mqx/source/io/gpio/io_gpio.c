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
* $FileName: io_gpio.c$
* $Version : 3.8.44.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the GPIO functions for MQX
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "fio.h"
#include "io.h"
#include "io_prv.h"
#include "io_gpio.h"
#include "io_gpio_prv.h"

_mqx_int _io_gpio_open(MQX_FILE_PTR, char_ptr, char_ptr);
_mqx_int _io_gpio_close(MQX_FILE_PTR);
_mqx_int _io_gpio_read (MQX_FILE_PTR, char_ptr, _mqx_int);
_mqx_int _io_gpio_write(MQX_FILE_PTR, char_ptr, _mqx_int);
_mqx_int _io_gpio_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

extern _mqx_int gpio_cpu_init(void);
extern _mqx_int gpio_cpu_open(MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int gpio_cpu_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
extern GPIO_PIN_MAP  gpio_global_pin_map; /* extern because it usually contains initializated values to non-zero */
extern GPIO_PIN_MAP  gpio_global_irq_map; /* extern because it usually contains initializated values to non-zero */

GPIO_DEV_DATA_PTR first_irq = NULL; /* first map having some IRQ */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_gpio_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a gpio driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_gpio_install
   (
      /* [IN] A string that identifies the device for fopen */
      /* input values are those identifiers defined in io_gpio.h file */
      char_ptr            identifier
   ) 
{ /* Body */
    if (IO_OK == gpio_cpu_init())
        return _io_dev_install(identifier, 
            _io_gpio_open,
            _io_gpio_close,
            _io_gpio_read,
            _io_gpio_write,
            gpio_cpu_ioctl,
            NULL);
    return (_mqx_uint)IO_ERROR;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_gpio_open
* Returned Value   : a gpio pointer
* Comments         : 
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_gpio_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char_ptr   open_name_ptr,

      /* [IN] the flags to be used during operation; pointer to pin table */
      char_ptr   flags
   )
{ /* Body */
    _mqx_int             i;
    uint_32              pin;
    uint_32              addr;
    GPIO_DEV_DATA_PTR    dev_data_ptr;
    GPIO_PIN_STRUCT _PTR_ pin_table = (GPIO_PIN_STRUCT _PTR_) flags;
 
    if (NULL == (dev_data_ptr = (GPIO_DEV_DATA_PTR) _mem_alloc_system_zero(sizeof(GPIO_DEV_DATA))))
        return IO_ERROR;
    _mem_set_type(dev_data_ptr, MEM_TYPE_IO_GPIO_DEV_DATA);  
 
    fd_ptr->DEV_DATA_PTR = (pointer) dev_data_ptr;
 
    _int_disable();
    if (pin_table != NULL) {
        /* rewrite pin_table to pin_map */
        for (; *pin_table != GPIO_LIST_END; pin_table++) {
            /* check pin validity bit */  
            if (*pin_table & GPIO_PIN_VALID) {
                /* prepare address of port */ 
                addr = GPIO_GET_PORT(*pin_table);
                /* prepare bit mask */
                pin = GPIO_GET_BIT_MASK(*pin_table); 
                /* pin address out of map scope? */
                if (addr < sizeof(GPIO_PIN_MAP) * 8 / (GPIO_PIN_BIT_MASK + 1)) {
                    /* pin address already used? */
                    if (! ( GPIO_GET_PIN_USED( gpio_global_pin_map, addr,  pin))) {
                        if (*pin_table & (GPIO_PIN_IRQ_LIST)) {
                            /* pin can be used as IRQ pin ? */
                            if (!( GPIO_GET_PIN_USED (gpio_global_irq_map, addr, pin ))) {
                                /* mark irq pin as used by this file by table
                                **       irqN_map   irqp_map    irql_map
                                ** RISING   0           1           0
                                ** FALLING  1           0           0
                                ** ZERO     1           0           1
                                ** ONE      0           1           1   */
                                _mqx_int error = 0;

                                switch( *pin_table & (GPIO_PIN_IRQ_LIST)) {
                                    case GPIO_PIN_IRQ_RISING:
                                        GPIO_SET_PIN_USED( dev_data_ptr->irqp_map, addr, pin);
                                        break;
                                    case GPIO_PIN_IRQ_FALLING:
                                        GPIO_SET_PIN_USED( dev_data_ptr->irqn_map, addr, pin);
                                        break;
                                    case GPIO_PIN_IRQ_RISING | GPIO_PIN_IRQ_FALLING:
                                        GPIO_SET_PIN_USED( dev_data_ptr->irqp_map, addr, pin);
                                        GPIO_SET_PIN_USED( dev_data_ptr->irqn_map, addr, pin);
                                        break;
                                    #if PSP_MQX_CPU_IS_KINETIS
                                    case GPIO_PIN_IRQ_ZERO:
                                        GPIO_SET_PIN_USED( dev_data_ptr->irqn_map, addr, pin);
                                        GPIO_SET_PIN_USED( dev_data_ptr->irql_map, addr, pin);
                                        break;
                                    case GPIO_PIN_IRQ_ONE:
                                        GPIO_SET_PIN_USED( dev_data_ptr->irqp_map, addr, pin);
                                        GPIO_SET_PIN_USED( dev_data_ptr->irql_map, addr, pin);
                                        break;
                                    #endif
                                    default:
                                        /* cannot handle another combination of IRQ flags */
                                        error = IO_ERROR;
                                        break;
                                }
                                if (!error) {
                                    /* mark pin as used by this file */
                                    GPIO_SET_PIN_USED( dev_data_ptr->pin_map, addr, pin );                                 
                                    continue; /* manage next pin */
                                }
                            }
                        }
                        else {
                            /* mark pin as used by this file */
                            GPIO_SET_PIN_USED( dev_data_ptr->pin_map, addr, pin );
                            continue; /* manage next pin */
                        }
                    }
                }
            }
            /* some error occured */
            _int_enable();
            _mem_free(dev_data_ptr);
            return IO_ERROR;
        }
    }
 
    if (IO_OK != gpio_cpu_open(fd_ptr, open_name_ptr, flags)) {
        _int_enable();
        _mem_free(dev_data_ptr);
        return IO_ERROR;
    }
 
    /* set bits in global pin maps that they are used by this file */
    for (i = 0; i < sizeof(gpio_global_pin_map) / sizeof(gpio_global_pin_map.memory32[0]); i++) {
        gpio_global_pin_map.memory32[i] |= dev_data_ptr->pin_map.memory32[i];
        gpio_global_irq_map.memory32[i] |= (dev_data_ptr->irqp_map.memory32[i] | dev_data_ptr->irqn_map.memory32[i]);
    }
 
    _int_enable();
    return IO_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_gpio_close
* Returned Value   : ERROR CODE
* Comments         : Closes gpio driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_gpio_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
    _mqx_int           i;
    GPIO_DEV_DATA_PTR  dev_data_ptr = (GPIO_DEV_DATA_PTR) fd_ptr->DEV_DATA_PTR;

    _int_disable();
    
    ioctl(fd_ptr, GPIO_IOCTL_DISABLE_IRQ,      NULL); /* disable interrupts for pin list */
    ioctl(fd_ptr, GPIO_IOCTL_SET_IRQ_FUNCTION, NULL); /* remove any IRQ routine from list */
    
    /* exclude pins from global pin memory */
    for (i = 0; i < sizeof(gpio_global_pin_map) / sizeof(gpio_global_pin_map.memory32[0]); i++) {
        gpio_global_pin_map.memory32[i] &= ~dev_data_ptr->pin_map.memory32[i];
        gpio_global_irq_map.memory32[i] &= ~(dev_data_ptr->irqp_map.memory32[i] | dev_data_ptr->irqn_map.memory32[i]);
    }
    _int_enable();

/* At closing, we can revert to alternate function
** But for now, we do nothing
*/
    
   _mem_free(dev_data_ptr);

   return MQX_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_gpio_read
* Returned Value   : number of characters read
* Comments         : 
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_gpio_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char_ptr   data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int   num
   )
{ /* Body */
   return 0;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_gpio_write
* Returned Value   : number of characters written
* Comments         : 
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_gpio_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are */
      char_ptr   data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int   num
   )
{ /* Body */
   return 0;
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_gpio_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_gpio_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint  cmd,

      /* [IN] the ioctl parameters */
      pointer    param_ptr
   )
{ /* Body */
     return IO_ERROR_INVALID_IOCTL_CMD;
} /* Endbody */

/* EOF */
