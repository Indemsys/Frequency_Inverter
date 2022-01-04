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
* $FileName: io_gpio_kgpio.c$
* $Version : 3.8.14.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This file contains the GPIO standard functions used on boards
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "../io_gpio.h"
#include "../io_gpio_prv.h"
#include <string.h>


#define MUX_GPIO    0x01
#define PORT_END    5
#define PIN_IDX_NUM 32

/* This structure will hold 'used pins'. If somebody would like to open 'already used pin',
** the operation will fail. All open pins within all GPIO driver files will be written here.
** We can predefine this structure with 'already used' bits for those bits, which are not
** used at all - so we will prevent open operation to be successful.
*/
GPIO_PIN_MAP  gpio_global_pin_map = {
    0x0000,  /* porta */
    0x0000,  /* portb */
    0x0000,  /* portc */
    0x0000,  /* portd */
    0x0000   /* porte */
};

GPIO_PIN_MAP  gpio_global_irq_map = {
    0x0000,  /* porta */
    0x0000,  /* portb */
    0x0000,  /* portc */
    0x0000,  /* portd */
    0x0000   /* porte */
};

static uint32_t volatile * const pctl_ptr_arr[] = { 
    &(PORTA_PCR(0)),
    &(PORTB_PCR(0)),
    &(PORTC_PCR(0)),
    &(PORTD_PCR(0)),
    &(PORTE_PCR(0))
};

const static uint_8 _port_int_prio[] = {
    BSP_PORTA_INT_LEVEL,
    BSP_PORTB_INT_LEVEL,
    BSP_PORTC_INT_LEVEL,
    BSP_PORTD_INT_LEVEL,
    BSP_PORTE_INT_LEVEL,
};

extern GPIO_DEV_DATA_PTR first_irq; /* first file having used IRQ */

static _mqx_int install_isr( MQX_FILE_PTR );
static uint_32 get_irq_mask( boolean n, boolean p, boolean l);
void gpio_cpu_configure(GPIO_DEV_DATA_PTR);
_mqx_int gpio_cpu_init(void);
_mqx_int gpio_cpu_open(MQX_FILE_PTR, char_ptr, char_ptr);

_mqx_int gpio_cpu_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
static void gpio_eport_irq (pointer param);

/*FUNCTION*****************************************************************
* 
* Function Name    : get_irg_mask
* Returned Value   : IRQ MASK
* Comments         :
*    return the IRQ mask
*
*END*********************************************************************/

static uint_32 get_irq_mask
    ( 
        /* IN first bitmask of decode table */ 
        uint_32 n,
        /* IN second bitmask of decode table */ 
        uint_32 p, 
        /* IN third bitmask of decode table */ 
        uint_32 l
     )
{ /* Body */
    /* Create index to the table below is done through these bit definitions */
    #define GPIO_LEVEL_MASK    (0x01)
    #define GPIO_RISING_MASK   (0x02)
    #define GPIO_FALLING_MASK  (0x04)

    #define IRQ_MASK_RISING     0x09    //1001 Interrupt on rising edge.
    #define IRQ_MASK_FALLING    0x0A    //1010 Interrupt on falling edge.
    #define IRQ_MASK_EITHER     0x0B    //1011 Interrupt on either edge.
    #define IRQ_MASK_ZERO       0x08    //1000 Interrupt when logic zero.
    #define IRQ_MASK_ONE        0x0C    //1100 Interrupt when logic one.

    _mqx_uint index = (n ? GPIO_FALLING_MASK : 0) | (p ? GPIO_RISING_MASK : 0) | (l ? GPIO_LEVEL_MASK : 0);

    const static uint_8 gpio_irq_mask_table[] = {
        0,                /* 000, no valid value */
        0,                /* 001, no valid value */
        IRQ_MASK_RISING,  /* 010, rising edge irq */
        IRQ_MASK_ONE,     /* 011, level one irq */
        IRQ_MASK_FALLING, /* 100, falling edge irq */
        IRQ_MASK_ZERO,    /* 101, level zero irq */
        IRQ_MASK_EITHER,  /* 110, falling and rising edge irq */
        0,                /* 111, no valid value */
    };

    return (uint_32) gpio_irq_mask_table[index];
} /* Endbody */
  
/*FUNCTION*****************************************************************
* 
* Function Name    : gpio_cpu_configure
* Returned Value   : IO_OK
* Comments         :
*    Configures pins to be GPIO based on pin map
*
*END*********************************************************************/

void gpio_cpu_configure
   (
      /* [IN] pointer to file data */
      GPIO_DEV_DATA_PTR   dev_data_ptr
   )
{ /* Body */  
    _mqx_int  port_pointer,
              pin_idx;        
  
    /* set GPIO output behavior */
    if (dev_data_ptr->type == DEV_OUTPUT) {       
        GPIOA_PDDR |= dev_data_ptr->pin_map.reg.porta;
        GPIOB_PDDR |= dev_data_ptr->pin_map.reg.portb;
        GPIOC_PDDR |= dev_data_ptr->pin_map.reg.portc;
        GPIOD_PDDR |= dev_data_ptr->pin_map.reg.portd;
        GPIOE_PDDR |= dev_data_ptr->pin_map.reg.porte;
    }
    else { /* DEV_INPUT type is supposed */
        GPIOA_PDDR &= ~dev_data_ptr->pin_map.reg.porta;
        GPIOB_PDDR &= ~dev_data_ptr->pin_map.reg.portb;
        GPIOC_PDDR &= ~dev_data_ptr->pin_map.reg.portc;
        GPIOD_PDDR &= ~dev_data_ptr->pin_map.reg.portd;
        GPIOE_PDDR &= ~dev_data_ptr->pin_map.reg.porte;
    }
 
    /* set pin functionality to be GPIO for the whole file */
    for( port_pointer = 0; port_pointer < PORT_END; port_pointer++)
    {
        /* This variable is for quick access to determine if we should perform any change
        ** interrupt configuration for port.
        */
        uint_32 port_imask =
            dev_data_ptr->irqp_map.memory32[ port_pointer ] | \
            dev_data_ptr->irqn_map.memory32[ port_pointer ] | \
            dev_data_ptr->irql_map.memory32[ port_pointer ];

        for( pin_idx = 0; pin_idx < PIN_IDX_NUM; pin_idx ++)
        {
            /* set pin mux as gpio */
            if( dev_data_ptr->pin_map.memory32[ port_pointer ] & ( 1 << pin_idx ) )
            {
                /* set PCRx to default state */
                *(pctl_ptr_arr[port_pointer] + pin_idx) = PORT_PCR_MUX(MUX_GPIO) 
                    | PORT_PCR_IRQC( get_irq_mask( 
                        dev_data_ptr->irqn_map.memory32[ port_pointer ] & ( 1 << pin_idx),
                        dev_data_ptr->irqp_map.memory32[ port_pointer ] & ( 1 << pin_idx),
                        dev_data_ptr->irql_map.memory32[ port_pointer ] & ( 1 << pin_idx)));
                /* enable internal pull-up resistor for output pin */
                if(dev_data_ptr->type == DEV_INPUT)
                {
                    *(pctl_ptr_arr[port_pointer] + pin_idx) |= (PORT_PCR_PE_MASK|PORT_PCR_PS_MASK);
                }
            }
        }

        if (port_imask) {
            /* Enable and configure interrupts on NVIC */
            _bsp_int_init(INT_PORTA + port_pointer, _port_int_prio[port_pointer], 0, TRUE);
        }
    }  
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : gpio_cpu_install
* Returned Value   : IO_OK
* Comments         :
*    Performs basic initialization specific to board
*
*END*********************************************************************/

_mqx_int gpio_cpu_init()
{ /* Body */

    _bsp_gpio_io_init();    
    return IO_OK;
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : gpio_cpu_open
* Returned Value   : IO_OK or error
* Comments         :
*    Implements initialization for DATA GPIO
*
*END*********************************************************************/

static _mqx_int install_isr
( 
    MQX_FILE_PTR fd_ptr 
)
{
    /* clear all flags */
    PORTA_ISFR = 0xFFFFFFFF;
    PORTB_ISFR = 0xFFFFFFFF;
    PORTC_ISFR = 0xFFFFFFFF;
    PORTD_ISFR = 0xFFFFFFFF;
    PORTE_ISFR = 0xFFFFFFFF;
  
    if (NULL == _int_install_isr(INT_PORTA, gpio_eport_irq, NULL))
    {
        return IO_ERROR; /* could not install interrupt routine */
    }
    if (NULL == _int_install_isr(INT_PORTB, gpio_eport_irq, NULL))
    {
        return IO_ERROR; 
    }
    if (NULL == _int_install_isr(INT_PORTC, gpio_eport_irq, NULL))
    {
        return IO_ERROR; 
    }
    if (NULL == _int_install_isr(INT_PORTD, gpio_eport_irq, NULL))
    {
        return IO_ERROR; 
    }
    if (NULL == _int_install_isr(INT_PORTE, gpio_eport_irq, NULL))
    {
        return IO_ERROR; 
    } 
    
    gpio_cpu_ioctl( fd_ptr, GPIO_IOCTL_ENABLE_IRQ, NULL);
    
    return IO_OK;   
}
/*FUNCTION*****************************************************************
* 
* Function Name    : gpio_cpu_open
* Returned Value   : IO_OK or error
* Comments         :
*    Implements initialization for DATA GPIO
*
*END*********************************************************************/

_mqx_int gpio_cpu_open
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the file name */
      char_ptr   file_name,

      /* [IN] pointer to parameters */
      char_ptr   param_ptr
   )
{ /* Body */
    GPIO_DEV_DATA_PTR dev_data_ptr = (GPIO_DEV_DATA_PTR) fd_ptr->DEV_DATA_PTR;
 
    /* if file_name is used, then the user wants to open peripheral */
    if ((file_name != NULL) && (*file_name != 0)) {
        if (!strncmp(file_name, "gpio:write", 11))  /* user wants write access to GPIO */
            dev_data_ptr->type = DEV_OUTPUT;
        else if (!strncmp(file_name, "gpio:output", 12))  /* user wants write access to GPIO */
            dev_data_ptr->type = DEV_OUTPUT;
        else if (!strncmp(file_name, "gpio:read", 10)) /* user wants read access to GPIO */
            dev_data_ptr->type = DEV_INPUT;
        else if (!strncmp(file_name, "gpio:input", 11)) /* user wants read access to GPIO */
            dev_data_ptr->type = DEV_INPUT;
        else
        /* peripherals not used yet */
            return IO_ERROR;
    }
    else
        return IO_ERROR;
    
    /* install interrupt service routine */
    if( IO_OK != install_isr( fd_ptr ) )
    {
        return IO_ERROR;  
    }
    
    /* set pins status before selecting GPIO function */
    if ((param_ptr != NULL) && (dev_data_ptr->type == DEV_OUTPUT)) { 
        /* note that this is similar to GPIO_IOCTL_WRITE function, but no checking 
        ** is performed (was done in io_gpio_open function) */
        GPIO_PIN_STRUCT _PTR_  pin_table;
        uint_32        addr;
        uint_8         pin;
        GPIO_PIN_MAP   temp_pin0_map = {0};
        GPIO_PIN_MAP   temp_pin1_map = {0};
        
        /* prepare pin map */
        for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
            /* prepare address of port */
            addr = GPIO_GET_PORT(*pin_table);  
            /* prepare bit mask */
            pin = GPIO_GET_BIT_MASK( *pin_table );  
            if (*pin_table & GPIO_PIN_STATUS)
                temp_pin1_map.memory32[addr] |= pin;
            else
                temp_pin0_map.memory32[addr] |= pin;
        }
        /* ok, now we can apply new map */
        /* note: applying the map after collecting pins is due to have pins applied in one instruction */
        GPIOA_PSOR = temp_pin1_map.memory32[0];
        GPIOA_PCOR = temp_pin0_map.memory32[0];
        GPIOB_PSOR = temp_pin1_map.memory32[1];
        GPIOB_PCOR = temp_pin0_map.memory32[1];
        GPIOC_PSOR = temp_pin1_map.memory32[2];
        GPIOC_PCOR = temp_pin0_map.memory32[2];
        GPIOD_PSOR = temp_pin1_map.memory32[3];
        GPIOD_PCOR = temp_pin0_map.memory32[3];
        GPIOE_PSOR = temp_pin1_map.memory32[4];
        GPIOE_PCOR = temp_pin0_map.memory32[4];
 
    }

    gpio_cpu_configure(dev_data_ptr);
    return IO_OK;
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : gpio_cpu_ioctl
* Returned Value   : depends on IOCTL command
* Comments         :
*    Implements all ioctl for GPIO
*
*END*********************************************************************/

_mqx_int gpio_cpu_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint  cmd,

      /* [IN] the ioctl parameters */
      pointer    param_ptr
   )
{ /* Body */
    GPIO_DEV_DATA_PTR  dev_data_ptr = (GPIO_DEV_DATA_PTR) fd_ptr->DEV_DATA_PTR;
 
    switch (cmd) {
        case GPIO_IOCTL_ADD_PINS: 
        {
            /* Add pins to this file. Params: table with pin addresses */
            GPIO_PIN_STRUCT _PTR_  pin_table;
            uint_32        addr;
            uint_32         pin;

            /* check pin_table if they are not in global pin_map */
            _int_disable();
            for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) 
            {
                /* check pin validity bit */
                if (*pin_table & GPIO_PIN_VALID) 
                { 
                    /* prepare address of port */ 
                    addr = GPIO_GET_PORT(*pin_table); 
                    /* prepare bit mask */
                    pin = GPIO_GET_BIT_MASK( *pin_table );  
                    /* pin address out of map scope? */
                    if (addr < sizeof(GPIO_PIN_MAP) * 8 / (GPIO_PIN_BIT_MASK + 1))
                    { 
                        /* pin address already used? */
                        if (! (gpio_global_pin_map.memory32[addr] & pin))
                        { 
                            /* manage next pin */
                            continue; 
                        }      
                    }
                }
                /* some problem occured */
                _int_enable();
                return IO_ERROR;
            }
            /* check successful, now we have to copy these bits to local pin map */
            for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) 
            {
                /* prepare address of port */
                addr = GPIO_GET_PORT(*pin_table);
                /* prepare bit mask */
                pin = GPIO_GET_BIT_MASK( *pin_table ); 
                /* mark pin as used by this file */
                dev_data_ptr->pin_map.memory32[addr] |= pin;
                /* mark pin as used globally */
                gpio_global_pin_map.memory32[addr] |= pin; 
            }
            gpio_cpu_configure(dev_data_ptr);
            _int_enable();
        }
        break;

        case GPIO_IOCTL_WRITE_LOG1: 
        {
            /* If this file is configured as output, sets every pin to 1. Params:
            list of pin addresses to be used or NULL for every pin in the file */
            if (dev_data_ptr->type != DEV_OUTPUT)
                return IO_ERROR;
            if (param_ptr == NULL) 
            { 
                /* apply command to whole file */
                _int_disable(); /* _lwsem_wait(&gpio_sem) replacement */
                GPIOA_PSOR = dev_data_ptr->pin_map.memory32[0];
                GPIOB_PSOR = dev_data_ptr->pin_map.memory32[1];
                GPIOC_PSOR = dev_data_ptr->pin_map.memory32[2];
                GPIOD_PSOR = dev_data_ptr->pin_map.memory32[3];
                GPIOE_PSOR = dev_data_ptr->pin_map.memory32[4];
                _int_enable(); 
                break;
            }
            else 
            {
                GPIO_PIN_STRUCT _PTR_   pin_table;
                uint_32                 addr;
                uint_32                 pin;
                GPIO_PIN_MAP            temp_pin_map = {0};

                _int_disable();
                /* check if there is not pin in the list which this file does not contain */
                for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) 
                {
                    /* check pin validity bit */
                    if (*pin_table & GPIO_PIN_VALID) 
                    { 
                        /* prepare address of port */
                        addr = GPIO_GET_PORT(*pin_table);
                        /* prepare bit mask */
                        pin = GPIO_GET_BIT_MASK( *pin_table );
                        /* pin address out of map scope? */
                        if (addr < sizeof(GPIO_PIN_MAP) * 8 / (GPIO_PIN_BIT_MASK + 1))
                        {
                            /* pin address already used? */
                            if (dev_data_ptr->pin_map.memory32[addr] & pin) 
                            { 
                                temp_pin_map.memory32[addr] |= pin;
                                continue; /* manage next pin */
                            }
                        }    
                    }
                    /* some problem occured */
                    _int_enable();
                    return IO_ERROR;
                }
                /* ok, now we can apply new map */
                /* note: applying the map after collecting pins is due to have pins applied in one instruction */
                GPIOA_PSOR = temp_pin_map.memory32[0];
                GPIOB_PSOR = temp_pin_map.memory32[1];
                GPIOC_PSOR = temp_pin_map.memory32[2];
                GPIOD_PSOR = temp_pin_map.memory32[3];
                GPIOE_PSOR = temp_pin_map.memory32[4];
                _int_enable();
            }
        }
        break;

        case GPIO_IOCTL_WRITE_LOG0: 
        {
            /* If this file is configured as output, clears every pin to 0. Params:
            list of pin addresses to be used or NULL for every pin in the file */
            if (dev_data_ptr->type != DEV_OUTPUT)
                return IO_ERROR;
            /* apply command to whole file */
            if (param_ptr == NULL) 
            { 
                _int_disable(); /* _lwsem_wait(&gpio_sem) replacement */
                GPIOA_PCOR = dev_data_ptr->pin_map.memory32[0];
                GPIOB_PCOR = dev_data_ptr->pin_map.memory32[1];
                GPIOC_PCOR = dev_data_ptr->pin_map.memory32[2];
                GPIOD_PCOR = dev_data_ptr->pin_map.memory32[3];
                GPIOE_PCOR = dev_data_ptr->pin_map.memory32[4];
                _int_enable();
                break;
            }
            else {
                GPIO_PIN_STRUCT _PTR_   pin_table;
                uint_32                 addr;
                uint_32                 pin;
                GPIO_PIN_MAP            temp_pin_map = {0};

                _int_disable();
                /* check if there is not pin in the list which this file does not contain */
                for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) 
                {
                    /* check pin validity bit */
                    if (*pin_table & GPIO_PIN_VALID) 
                    { 
                        /* prepare address of port */
                        addr = GPIO_GET_PORT(*pin_table);
                        /* prepare bit mask */
                        pin = GPIO_GET_BIT_MASK( *pin_table );
                        /* pin address out of map scope? */
                        if (addr < sizeof(GPIO_PIN_MAP) * 8 / (GPIO_PIN_BIT_MASK + 1))
                        {  
                            /* pin address already used? */
                            if (dev_data_ptr->pin_map.memory32[addr] & pin) { 
                                temp_pin_map.memory32[addr] |= pin;
                                continue; /* manage next pin */
                            }
                        }
                    }
                    /* some problem occurred */
                    _int_enable();
                    return IO_ERROR;
                }
                /* ok, now we can apply new map */
                /* note: applying the map after collecting pins is due to have pins applied in one instruction */
                GPIOA_PCOR = temp_pin_map.memory32[0];
                GPIOB_PCOR = temp_pin_map.memory32[1];
                GPIOC_PCOR = temp_pin_map.memory32[2];
                GPIOD_PCOR = temp_pin_map.memory32[3];
                GPIOE_PCOR = temp_pin_map.memory32[4];
                _int_enable();
            }
        }
        break;

        case GPIO_IOCTL_WRITE: 
        {
            /* If this file is configured as output, sets every pin to the state 
            ** defined in GPIO_PIN_STATUS. Params:
            ** list of pin addresses to be used or NULL for every pin in the file */
            if (dev_data_ptr->type != DEV_OUTPUT)
                return IO_ERROR;
            
            if (param_ptr == NULL) /* no param specified , but must be */
                return IO_ERROR;
            else {
                GPIO_PIN_STRUCT _PTR_   pin_table;
                uint_32                 addr;
                uint_32                  pin;
                GPIO_PIN_MAP             temp_pin0_map = {0};
                GPIO_PIN_MAP             temp_pin1_map = {0};

                _int_disable();
                /* check if there is not pin in the list which this file does not contain */
                for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) 
                {
                    /* check pin validity bit */
                    if (*pin_table & GPIO_PIN_VALID) 
                    { 
                        /* prepare address of port */
                        addr = GPIO_GET_PORT(*pin_table);  
                        /* prepare bit mask */
                        pin = GPIO_GET_BIT_MASK( *pin_table );  
                        /* pin address out of map scope? */
                        if (addr < sizeof(GPIO_PIN_MAP) * 8 / (GPIO_PIN_BIT_MASK + 1)) 
                        {
                            /* pin address already used? */
                            if (dev_data_ptr->pin_map.memory32[addr] & pin) 
                            { 
                                if (*pin_table & GPIO_PIN_STATUS)
                                    temp_pin1_map.memory32[addr] |= pin;
                                else
                                    temp_pin0_map.memory32[addr] |= pin;
                                continue; /* manage next pin */
                            }
                        }
                    }
                    /* some problem occurred */
                    _int_enable();
                    return IO_ERROR;
                }
                /* ok, now we can apply new map */
                /* note: applying the map after collecting pins is due to have pins applied in one instruction */
                GPIOA_PSOR = temp_pin1_map.memory32[0];
                GPIOA_PCOR = temp_pin0_map.memory32[0];
                GPIOB_PSOR = temp_pin1_map.memory32[1];
                GPIOB_PCOR = temp_pin0_map.memory32[1];
                GPIOC_PSOR = temp_pin1_map.memory32[2];
                GPIOC_PCOR = temp_pin0_map.memory32[2];
                GPIOD_PSOR = temp_pin1_map.memory32[3];
                GPIOD_PCOR = temp_pin0_map.memory32[3];
                GPIOE_PSOR = temp_pin1_map.memory32[4];
                GPIOE_PCOR = temp_pin0_map.memory32[4];
                _int_enable();
            }
        }
        break;

        case GPIO_IOCTL_READ: 
        {
            /* Read status of pins, if configured as input. Params: pointer to 
            ** predefined table where data will be written */
            GPIO_PIN_STRUCT _PTR_   pin_table;
            uint_32                 addr;
            volatile uint32_t _PTR_ rgpio_pdir_ptr;
            uint_32                 pin;
            
            volatile uint32_t _PTR_ rgpio_ptr_arr[] = { 
                &GPIOA_PDIR,
                &GPIOB_PDIR,
                &GPIOC_PDIR,
                &GPIOD_PDIR,
                &GPIOE_PDIR
            }; 

            if (dev_data_ptr->type != DEV_INPUT)
                return IO_ERROR;
            if (param_ptr == NULL) /* no list defined */
                return IO_ERROR;
            
            _int_disable();
            /* check if there is not pin in the list which this file does not contain */
            for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) 
            {
                /* check pin validity bit */
                if (*pin_table & GPIO_PIN_VALID) 
                { 
                    /* prepare address of port */ 
                    addr = GPIO_GET_PORT(*pin_table);
                    rgpio_pdir_ptr = rgpio_ptr_arr[ addr ];                    
                    /* prepare bit mask */
                    pin = GPIO_GET_BIT_MASK( *pin_table );  
                    /* pin address out of map scope? */
                    if (addr < sizeof(GPIO_PIN_MAP) * 8 / (GPIO_PIN_BIT_MASK + 1)) 
                    {  
                        /* pin address already used? */
                        if (dev_data_ptr->pin_map.memory32[addr] & pin) 
                        { 
                            /* GPIO port */
                            /* check pin status */
                            if ( *rgpio_pdir_ptr & pin)
                            {  
                                /* set pin status to 1 in the list */
                                *pin_table |= GPIO_PIN_STATUS; 
                            }
                            else
                            {
                                /* clear pin status to 0 in the list */
                                *pin_table &= ~GPIO_PIN_STATUS; 
                            }    
                            continue; /* manage next pin */
                        }
                    }
                }
                /* some problem occured */
                _int_enable();
                return IO_ERROR;
            }
            _int_enable();
        }
        break;

        case GPIO_IOCTL_ENABLE_IRQ: {

            uint_32 port_pointer, pin_idx;

            /* set "interrupt enabled" for the whole file */
            for( port_pointer = 0; port_pointer < PORT_END; port_pointer++)
            {
                /* This variable is for quick access to determine if we should perform any change
                ** to IRQC register field.
                ** We can set IRQC for every pin of port also, but it will be time consuming
                */
                uint_32 port_imask =
                    dev_data_ptr->irqp_map.memory32[ port_pointer ] | \
                    dev_data_ptr->irqn_map.memory32[ port_pointer ] | \
                    dev_data_ptr->irql_map.memory32[ port_pointer ];

                for( pin_idx = 0; pin_idx < PIN_IDX_NUM; pin_idx ++)
                {
                    if (port_imask & 0x00000001) {
                        /* clear PCRx.IRQC field */
                        *(pctl_ptr_arr[port_pointer] + pin_idx) &= ~PORT_PCR_IRQC(0x0F);
                        *(pctl_ptr_arr[port_pointer] + pin_idx) |=  PORT_PCR_IRQC( get_irq_mask( 
                                dev_data_ptr->irqn_map.memory32[ port_pointer ] & ( 1 << pin_idx),
                                dev_data_ptr->irqp_map.memory32[ port_pointer ] & ( 1 << pin_idx),
                                dev_data_ptr->irql_map.memory32[ port_pointer ] & ( 1 << pin_idx)));
                    }
                    port_imask >>= 1; 
                }
            }
        }
        break;

        case GPIO_IOCTL_DISABLE_IRQ: {

            uint_32 port_pointer, pin_idx;

            /* set "interrupt disabled" for whole file */
            for( port_pointer = 0; port_pointer < PORT_END; port_pointer++)
            {
                /* This variable is for quick access to determine if we should perform any change
                ** to IRQC register field.
                ** We can set IRQC for every pin of port also, but it will be time consuming
                */
                uint_32 port_imask =
                    dev_data_ptr->irqp_map.memory32[ port_pointer ] | \
                    dev_data_ptr->irqn_map.memory32[ port_pointer ] | \
                    dev_data_ptr->irql_map.memory32[ port_pointer ];
                for( pin_idx = 0; pin_idx < PIN_IDX_NUM; pin_idx ++)
                {
                    if (port_imask & 0x00000001) {
                        /* set PCRx to default state */
                        *(pctl_ptr_arr[port_pointer] + pin_idx) &= ~PORT_PCR_IRQC(0x0F);
                    }
                    port_imask >>= 1; 
                }
            }
        }
        break;

        case GPIO_IOCTL_SET_IRQ_FUNCTION: {
          
            if (dev_data_ptr->type == DEV_OUTPUT)
                return IO_ERROR; /* cannot set IRQ for output devices */

            dev_data_ptr->irq_func = (IRQ_FUNC)param_ptr;
            _int_disable();
            
            if (param_ptr == NULL) {  /* remove function from list */
                if (first_irq != NULL) {
                    GPIO_DEV_DATA_PTR gptr = first_irq;
                    while (gptr->NEXT != NULL) {
                        if (gptr == dev_data_ptr)
                            break;
                        if (gptr->NEXT == dev_data_ptr)
                            break;
                        gptr = gptr->NEXT;
                    }
                    if (gptr == dev_data_ptr) /* found as first in the list */
                        first_irq = first_irq->NEXT;
                    else if (gptr->NEXT == dev_data_ptr) /* found in the middle or at the end of list */
                        gptr->NEXT = gptr->NEXT->NEXT;
                }
            }
            else { /* add new function to the list */
                if (first_irq == NULL) {
                    first_irq = dev_data_ptr;
                }
                else {
                    GPIO_DEV_DATA_PTR gptr = first_irq;
                    while (gptr->NEXT != NULL) {
                        if (gptr == dev_data_ptr)
                            break;
                        gptr = gptr->NEXT;
                    }
                    if (gptr != dev_data_ptr)
                        gptr->NEXT = dev_data_ptr;
                }
            }
            _int_enable();       

        }    
        break;

        default:
            return IO_ERROR_INVALID_IOCTL_CMD;
    }
 
    return IO_OK;
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : gpio_eport_irq
* Returned Value   : 
* Comments         : GPIO IRQ handler
* 
*   This function find through all gpio files interrupt flag, clear it 
*   and call installed irq function
*
*END*********************************************************************/

static void gpio_eport_irq 
    (
        pointer param
     ) 
{ /* Body */
    /* DO NOT REMOVE volatile KEYWORD!!!
    ** This is workaround for bug in CodeWarrior 10.1 ARMCC.
    */
    volatile GPIO_DEV_DATA_PTR   gptr = first_irq;  
    volatile boolean             irq_flag = 0;
    volatile _mqx_int            port_pointer;
    volatile _mqx_int            pin_idx;
    
    /* list through all files */
    while (gptr) 
    { 
        /* list through all ports */
        for( port_pointer = 0; port_pointer < PORT_END; port_pointer++)
        {
            /* list through all pins */ 
            for( pin_idx = 0; pin_idx < PIN_IDX_NUM; pin_idx ++)
            {
                /* is pin in file ? */
                if( gptr->pin_map.memory32[ port_pointer ] & ( 1 << pin_idx ) )
                {                               
                    /* have pin set irq flag ? */
                    if( * (pctl_ptr_arr[port_pointer] + pin_idx) & PORT_PCR_ISF_MASK )
                    {
                        /* clear ISF flag */
                        * (pctl_ptr_arr[port_pointer] + pin_idx) |= PORT_PCR_ISF_MASK;
                        /* set irq call function flag */      
                        irq_flag = 1;
                    }               
                }
            }
        }   
        if ( irq_flag )
        {
            /* call irq function */
            gptr->irq_func(NULL);
            irq_flag = 0;
        }
        /* go to next file */
        gptr = gptr->NEXT;
    }
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_gpio_base_address
* Returned Value   : pointer to base of GPIO registers
* Comments         :
*    This function returns base address of GPIO related register space.
*
*END*********************************************************************/
pointer _bsp_get_gpio_base_address
(
    void
)
{
    return NULL;
}

/* EOF */
