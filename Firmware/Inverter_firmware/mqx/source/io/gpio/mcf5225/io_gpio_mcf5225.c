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
* $FileName: io_gpio_mcf5225.c$
* $Version : 3.8.43.0$
* $Date    : Aug-1-2011$
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

/* This structure will hold 'used pins'. If somebody would like to open 'already used pin',
** the operation will fail. All open pins within all GPIO driver files will be written here.
** We can predefine this structure with 'already used' bits for those bits, which are not
** used at all - so we will prevent open operation to be successful.
*/
GPIO_PIN_MAP  gpio_global_pin_map = {
    0x00, /* portte */
    0x00, /* porttf */
    0x00, /* porttg */
    0x00, /* portth */
    0x00, /* portti */
    0xff, /* unusedA */
    0x00, /* porttj */
    0xff, /* unusedB */
    0x01, /* portnq */
    0xff, /* unusedC */
    0x00, /* portan */
    0xf8, /* portas */
    0x80, /* portqs */
    0xff, /* unusedD */
    0xf0, /* portta */
    0xf0, /* porttc */
    0xff, /* unusedE */
    0xf0, /* portua */
    0xf0, /* portub */
    0xf0, /* portuc */
    0x00, /* portdd */
    {
        0xff, 0xff, 0xff /* unusedF[] */
    }
};

GPIO_PIN_MAP  gpio_global_irq_map = {
    0xff, /* portte */
    0xff, /* porttf */
    0xff, /* porttg */
    0xff, /* portth */
    0xff, /* portti */
    0xff, /* unusedA */
    0xff, /* porttj */
    0xff, /* unusedB */
    0x55, /* portnq */
    0xff, /* unusedC */
    0xff, /* portan */
    0xff, /* portas */
    0xff, /* portqs */
    0xff, /* unusedD */
    0xff, /* portta */
    0xff, /* porttc */
    0xff, /* unusedE */
    0xff, /* portua */
    0xff, /* portub */
    0xff, /* portuc */
    0xff, /* portdd */
    {
        0xff, 0xff, 0xff /* unusedF[] */
    }
};

extern MQX_INITIALIZATION_STRUCT MQX_init_struct;

extern GPIO_DEV_DATA_PTR first_irq; /* first file having used IRQ */

/* pointer to the start of GPIO registers */
VMCF5225_GPIO_STRUCT_PTR mcf5225_gpio_ptr;
VMCF522XX_EPORT_STRUCT_PTR mcf5225_eport_ptr;

void gpio_cpu_configure(GPIO_DEV_DATA_PTR);
_mqx_int gpio_cpu_init();
_mqx_int gpio_cpu_open(MQX_FILE_PTR, char_ptr, char_ptr);
_mqx_int gpio_cpu_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

static void gpio_eport_irq (pointer param);

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
    if (dev_data_ptr->type == DEV_OUTPUT) { /* set GPIO output behavior */
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 0) |= dev_data_ptr->pin_map.memory32[0];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 1) |= dev_data_ptr->pin_map.memory32[1];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 2) |= dev_data_ptr->pin_map.memory32[2];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 3) |= dev_data_ptr->pin_map.memory32[3];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 4) |= dev_data_ptr->pin_map.memory32[4];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 5) |= dev_data_ptr->pin_map.memory32[5];
    }
    else { /* DEV_INPUT type is supposed */
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 0) &= ~dev_data_ptr->pin_map.memory32[0];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 1) &= ~dev_data_ptr->pin_map.memory32[1];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 2) &= ~dev_data_ptr->pin_map.memory32[2];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 3) &= ~dev_data_ptr->pin_map.memory32[3];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 4) &= ~dev_data_ptr->pin_map.memory32[4];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->DDRTE)) + 5) &= ~dev_data_ptr->pin_map.memory32[5];
    }
 
    /* set pin functionality to be GPIO for the whole file */
    mcf5225_gpio_ptr->PTEPAR = mcf5225_gpio_ptr->PTEPAR & ~dev_data_ptr->pin_map.reg.portte;
    mcf5225_gpio_ptr->PTFPAR = mcf5225_gpio_ptr->PTFPAR & ~dev_data_ptr->pin_map.reg.porttf;
    mcf5225_gpio_ptr->PTGPAR = mcf5225_gpio_ptr->PTGPAR & ~dev_data_ptr->pin_map.reg.porttg;
    mcf5225_gpio_ptr->PTIPAR = mcf5225_gpio_ptr->PTIPAR & ~dev_data_ptr->pin_map.reg.portti;
    mcf5225_gpio_ptr->PTJPAR = mcf5225_gpio_ptr->PTJPAR & ~dev_data_ptr->pin_map.reg.porttj;
    mcf5225_gpio_ptr->PANPAR = mcf5225_gpio_ptr->PANPAR & ~dev_data_ptr->pin_map.reg.portan;
    mcf5225_gpio_ptr->PDDPAR = mcf5225_gpio_ptr->PDDPAR & ~dev_data_ptr->pin_map.reg.portdd;
    mcf5225_gpio_ptr->PTHPAR = mcf5225_gpio_ptr->PTHPAR & ~(
        ( dev_data_ptr->pin_map.reg.portth & 0x01 ? 0x0003 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x02 ? 0x000C : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x04 ? 0x0030 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x08 ? 0x00C0 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x10 ? 0x0300 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x20 ? 0x0C00 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x40 ? 0x3000 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portth & 0x80 ? 0xC000 : 0x0000 ));
    mcf5225_gpio_ptr->PQSPAR = mcf5225_gpio_ptr->PQSPAR & ~(
        ( dev_data_ptr->pin_map.reg.portqs & 0x01 ? 0x0003 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portqs & 0x02 ? 0x000C : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portqs & 0x04 ? 0x0030 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portqs & 0x08 ? 0x00C0 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portqs & 0x10 ? 0x0300 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portqs & 0x20 ? 0x0C00 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portqs & 0x40 ? 0x3000 : 0x0000 ));
    mcf5225_gpio_ptr->PASPAR = mcf5225_gpio_ptr->PASPAR & ~(
        ( dev_data_ptr->pin_map.reg.portas & 0x01 ? 0x03 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portas & 0x02 ? 0x0C : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portas & 0x04 ? 0x30 : 0x00 ));
    mcf5225_gpio_ptr->PTAPAR = mcf5225_gpio_ptr->PTAPAR & ~(
        ( dev_data_ptr->pin_map.reg.portta & 0x01 ? 0x03 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portta & 0x02 ? 0x0C : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portta & 0x04 ? 0x30 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portta & 0x08 ? 0xC0 : 0x00 ));
    mcf5225_gpio_ptr->PTCPAR = mcf5225_gpio_ptr->PTCPAR & ~(
        ( dev_data_ptr->pin_map.reg.porttc & 0x01 ? 0x03 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.porttc & 0x02 ? 0x0C : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.porttc & 0x04 ? 0x30 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.porttc & 0x08 ? 0xC0 : 0x00 ));
    mcf5225_gpio_ptr->PUAPAR = mcf5225_gpio_ptr->PUAPAR & ~(
        ( dev_data_ptr->pin_map.reg.portua & 0x01 ? 0x03 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portua & 0x02 ? 0x0C : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portua & 0x04 ? 0x30 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portua & 0x08 ? 0xC0 : 0x00 ));
    mcf5225_gpio_ptr->PUBPAR = mcf5225_gpio_ptr->PUBPAR & ~(
        ( dev_data_ptr->pin_map.reg.portub & 0x01 ? 0x03 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portub & 0x02 ? 0x0C : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portub & 0x04 ? 0x30 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portub & 0x08 ? 0xC0 : 0x00 ));
    mcf5225_gpio_ptr->PUCPAR = mcf5225_gpio_ptr->PUCPAR & ~(
        ( dev_data_ptr->pin_map.reg.portuc & 0x01 ? 0x03 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portuc & 0x02 ? 0x0C : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portuc & 0x04 ? 0x30 : 0x00 ) |
        ( dev_data_ptr->pin_map.reg.portuc & 0x08 ? 0xC0 : 0x00 ));
    mcf5225_gpio_ptr->PNQPAR = mcf5225_gpio_ptr->PNQPAR & ~(
        ( dev_data_ptr->pin_map.reg.portnq & 0x02 ? 0x000C : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portnq & 0x04 ? 0x0030 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portnq & 0x08 ? 0x00C0 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portnq & 0x10 ? 0x0300 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portnq & 0x20 ? 0x0C00 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portnq & 0x40 ? 0x3000 : 0x0000 ) |
        ( dev_data_ptr->pin_map.reg.portnq & 0x80 ? 0xC000 : 0x0000 )) | (
        ( dev_data_ptr->irqp_map.reg.portnq & 0x02 ? 0x0004 : 0x0000 ) |
        ( dev_data_ptr->irqp_map.reg.portnq & 0x08 ? 0x0040 : 0x0000 ) |
        ( dev_data_ptr->irqp_map.reg.portnq & 0x20 ? 0x0400 : 0x0000 ) |
        ( dev_data_ptr->irqp_map.reg.portnq & 0x80 ? 0x4000 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x02 ? 0x0004 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x08 ? 0x0040 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x20 ? 0x0400 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x80 ? 0x4000 : 0x0000 ));

    /* configure EPORT module */
    mcf5225_eport_ptr->EPDDR = 0x00; /* all eport irqs are inputs */
    mcf5225_eport_ptr->EPPAR |= /* react on rising / falling edge */
        ( dev_data_ptr->irqp_map.reg.portnq & 0x02 ? 0x0004 : 0x0000 ) |
        ( dev_data_ptr->irqp_map.reg.portnq & 0x08 ? 0x0040 : 0x0000 ) |
        ( dev_data_ptr->irqp_map.reg.portnq & 0x20 ? 0x0400 : 0x0000 ) |
        ( dev_data_ptr->irqp_map.reg.portnq & 0x80 ? 0x4000 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x02 ? 0x0008 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x08 ? 0x0080 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x20 ? 0x0800 : 0x0000 ) |
        ( dev_data_ptr->irqn_map.reg.portnq & 0x80 ? 0x8000 : 0x0000 );
    mcf5225_eport_ptr->EPIER |= (dev_data_ptr->irqp_map.reg.portnq | dev_data_ptr->irqn_map.reg.portnq); /* set interrupt enable */

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
    VMCF5225_STRUCT_PTR  mcf5225_ptr;
 
    mcf5225_ptr = _PSP_GET_IPSBAR();
    mcf5225_gpio_ptr = &mcf5225_ptr->GPIO;
    mcf5225_eport_ptr = &mcf5225_ptr->EPORT[0];
 
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
    _mqx_int i;
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
 
    /* check which irq pin to be opened and prepare interrupt service routine */
    for (i = 1; i < 8; i++) {
        if ((i & 0x01) == 0)
            continue;
        if ((dev_data_ptr->irqp_map.reg.portnq | dev_data_ptr->irqn_map.reg.portnq) & (1 << i)) {
            if (MQX_init_struct.MQX_HARDWARE_INTERRUPT_LEVEL_MAX < i)
                return IO_ERROR; /* hardware level must be higher than interrupt level */
            if (NULL == _int_install_isr(MCF5225_INT_EPORT0_EPF1 - 1 + i, gpio_eport_irq, NULL))
                return IO_ERROR; /* could not install interrupt routine */
            /* this is only to enable the interrupt source, level and sublevel values are hardwired */                
            _mcf5225_int_init(MCF5225_INT_EPORT0_EPF1 - 1 + i, i, BSP_EPORT_EPFX_INT_SUBLEVEL_MIDPOINT, TRUE);
        }
    }

    if ((param_ptr != NULL) && (dev_data_ptr->type == DEV_OUTPUT)) { /* set pins status before selecting GPIO function */
        /* note that this is similar to GPIO_IOCTL_WRITE function, but no checking is performed
           (was done in io_gpio_open function) */
        GPIO_PIN_STRUCT _PTR_  pin_table;
        uint_32        addr;
        uint_8         pin;
        GPIO_PIN_MAP_PTR    temp_pin0_map_ptr;
        GPIO_PIN_MAP_PTR    temp_pin1_map_ptr;

        if (NULL == (temp_pin0_map_ptr = (GPIO_PIN_MAP_PTR) _mem_alloc_system_zero(sizeof(GPIO_PIN_MAP))))
            return IO_ERROR;

        if (NULL == (temp_pin1_map_ptr = (GPIO_PIN_MAP_PTR) _mem_alloc_system_zero(sizeof(GPIO_PIN_MAP)))) {
            _mem_free(temp_pin0_map_ptr);
            return IO_ERROR;
        }

        /* prepare pin map */
        for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
            addr = (*pin_table & GPIO_PIN_ADDR) >> 3; /* prepare address of port */ 
            pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
            if (*pin_table & GPIO_PIN_STATUS)
                temp_pin1_map_ptr->memory8[addr] |= pin;
            else
                temp_pin0_map_ptr->memory8[addr] |= pin;
        }
        /* ok, now we can apply new map */
        /* note: applying the map after collecting pins is due to have pins applied in one instruction */
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 0) = temp_pin1_map_ptr->memory32[0];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 0) = ~temp_pin0_map_ptr->memory32[0];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 1) = temp_pin1_map_ptr->memory32[1];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 1) = ~temp_pin0_map_ptr->memory32[1];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 2) = temp_pin1_map_ptr->memory32[2];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 2) = ~temp_pin0_map_ptr->memory32[2];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 3) = temp_pin1_map_ptr->memory32[3];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 3) = ~temp_pin0_map_ptr->memory32[3];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 4) = temp_pin1_map_ptr->memory32[4];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 4) = ~temp_pin0_map_ptr->memory32[4];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 5) = temp_pin1_map_ptr->memory32[5];
        * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 5) = ~temp_pin0_map_ptr->memory32[5];

        _mem_free(temp_pin1_map_ptr);
        _mem_free(temp_pin0_map_ptr);
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
        case GPIO_IOCTL_ADD_PINS: {
        /* Add pins to this file. Params: table with pin addresses */
            GPIO_PIN_STRUCT _PTR_  pin_table;
            uint_32        addr;
            uint_8         pin;

            /* check pin_table if they are not in global pin_map */
            _int_disable();
            for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
                if (*pin_table & GPIO_PIN_VALID) { /* check pin validity bit */
                    addr = (*pin_table & GPIO_PIN_ADDR) >> 3; /* prepare address of port */ 
                    pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
                    if (addr < sizeof(GPIO_PIN_MAP)) /* pin address out of map scope? */
                        if (! (gpio_global_pin_map.memory8[addr] & pin)) /* pin address already used? */
                            continue; /* manage next pin */
                }
                /* some problem occured */
                _int_enable();
                return IO_ERROR;
            }
            /* check successful, now we have to copy these bits to local pin map */
            for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
                addr = (*pin_table & GPIO_PIN_ADDR)  >> 3; /* prepare address of port */ 
                pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
                dev_data_ptr->pin_map.memory8[addr] |= pin; /* mark pin as used by this file */
                gpio_global_pin_map.memory8[addr] |= pin; /* mark pin as used globally */
            }
            gpio_cpu_configure(dev_data_ptr);
            _int_enable();
        }
        break;

        case GPIO_IOCTL_WRITE_LOG1: {
        /* If this file is configured as output, sets every pin to 1. Params:
           list of pin addresses to be used or NULL for every pin in the file */
            if (dev_data_ptr->type != DEV_OUTPUT)
                return IO_ERROR;
            if (param_ptr == NULL) { /* apply command to whole file */
                _int_disable(); /* _lwsem_wait(&gpio_sem) replacement */
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 0) = dev_data_ptr->pin_map.memory32[0];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 1) = dev_data_ptr->pin_map.memory32[1];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 2) = dev_data_ptr->pin_map.memory32[2];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 3) = dev_data_ptr->pin_map.memory32[3];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 4) = dev_data_ptr->pin_map.memory32[4];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 5) = dev_data_ptr->pin_map.memory32[5];
                _int_enable(); /* _int_enable(); */
                break;
            }
            else {
                GPIO_PIN_STRUCT _PTR_  pin_table;
                uint_32        addr;
                uint_8         pin;
                GPIO_PIN_MAP_PTR    temp_pin_map_ptr;

                if (NULL == (temp_pin_map_ptr = (GPIO_PIN_MAP_PTR) _mem_alloc_system_zero(sizeof(GPIO_PIN_MAP))))
                    return IO_ERROR;
    
                _int_disable();
                /* check if there is not pin in the list which this file does not contain */
                for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
                    if (*pin_table & GPIO_PIN_VALID) { /* check pin validity bit */
                        addr = (*pin_table & GPIO_PIN_ADDR) >> 3; /* prepare address of port */ 
                        pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
                        if (addr < sizeof(GPIO_PIN_MAP)) /* pin address out of map scope? */
                            if (dev_data_ptr->pin_map.memory8[addr] & pin) { /* pin address already used? */
                                temp_pin_map_ptr->memory8[addr] |= pin;
                                continue; /* manage next pin */
                            }
                    }
                    /* some problem occured */
                    _int_enable();
                    _mem_free(temp_pin_map_ptr);
                    return IO_ERROR;
                }
                /* ok, now we can apply new map */
                /* note: applying the map after collecting pins is due to have pins applied in one instruction */
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 0) = temp_pin_map_ptr->memory32[0];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 1) = temp_pin_map_ptr->memory32[1];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 2) = temp_pin_map_ptr->memory32[2];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 3) = temp_pin_map_ptr->memory32[3];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 4) = temp_pin_map_ptr->memory32[4];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 5) = temp_pin_map_ptr->memory32[5];
                _int_enable();
                _mem_free(temp_pin_map_ptr);
            }
        }
        break;

        case GPIO_IOCTL_WRITE_LOG0: {
        /* If this file is configured as output, clears every pin to 0. Params:
           list of pin addresses to be used or NULL for every pin in the file */
            if (dev_data_ptr->type != DEV_OUTPUT)
                return IO_ERROR;
            if (param_ptr == NULL) { /* apply command to whole file */
                _int_disable(); /* _lwsem_wait(&gpio_sem) replacement */
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 0) = ~dev_data_ptr->pin_map.memory32[0];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 1) = ~dev_data_ptr->pin_map.memory32[1];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 2) = ~dev_data_ptr->pin_map.memory32[2];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 3) = ~dev_data_ptr->pin_map.memory32[3];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 4) = ~dev_data_ptr->pin_map.memory32[4];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 5) = ~dev_data_ptr->pin_map.memory32[5];
                _int_enable(); /* _int_enable(); */
                break;
            }
            else {
                GPIO_PIN_STRUCT _PTR_  pin_table;
                uint_32        addr;
                uint_8         pin;
                GPIO_PIN_MAP_PTR    temp_pin_map_ptr;

                if (NULL == (temp_pin_map_ptr = (GPIO_PIN_MAP_PTR) _mem_alloc_system_zero(sizeof(GPIO_PIN_MAP))))
                    return IO_ERROR;
    
                _int_disable();
                /* check if there is not pin in the list which this file does not contain */
                for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
                    if (*pin_table & GPIO_PIN_VALID) { /* check pin validity bit */
                        addr = (*pin_table & GPIO_PIN_ADDR) >> 3; /* prepare address of port */ 
                        pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
                        if (addr < sizeof(GPIO_PIN_MAP)) /* pin address out of map scope? */
                            if (dev_data_ptr->pin_map.memory8[addr] & pin) { /* pin address already used? */
                                temp_pin_map_ptr->memory8[addr] |= pin;
                                continue; /* manage next pin */
                            }
                    }
                    /* some problem occured */
                    _int_enable();
                    _mem_free(temp_pin_map_ptr);
                    return IO_ERROR;
                }
                /* ok, now we can apply new map */
                /* note: applying the map after collecting pins is due to have pins applied in one instruction */
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 0) = ~temp_pin_map_ptr->memory32[0];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 1) = ~temp_pin_map_ptr->memory32[1];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 2) = ~temp_pin_map_ptr->memory32[2];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 3) = ~temp_pin_map_ptr->memory32[3];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 4) = ~temp_pin_map_ptr->memory32[4];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 5) = ~temp_pin_map_ptr->memory32[5];
                _int_enable();
                _mem_free(temp_pin_map_ptr);
            }
        }
        break;

        case GPIO_IOCTL_WRITE: {
        /* If this file is configured as output, sets every pin to the state defined in GPIO_PIN_STATUS. Params:
           list of pin addresses to be used or NULL for every pin in the file */
            if (dev_data_ptr->type != DEV_OUTPUT)
                return IO_ERROR;
            if (param_ptr == NULL) /* no param specified , but must be */
                return IO_ERROR;
            else {
                GPIO_PIN_STRUCT _PTR_  pin_table;
                uint_32        addr;
                uint_8         pin;
                GPIO_PIN_MAP_PTR    temp_pin0_map_ptr;
                GPIO_PIN_MAP_PTR    temp_pin1_map_ptr;

                if (NULL == (temp_pin0_map_ptr = (GPIO_PIN_MAP_PTR) _mem_alloc_system_zero(sizeof(GPIO_PIN_MAP))))
                    return IO_ERROR;
    
                if (NULL == (temp_pin1_map_ptr = (GPIO_PIN_MAP_PTR) _mem_alloc_system_zero(sizeof(GPIO_PIN_MAP)))) {
                    _mem_free(temp_pin0_map_ptr);
                    return IO_ERROR;
                }

                _int_disable();
                /* check if there is not pin in the list which this file does not contain */
                for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
                    if (*pin_table & GPIO_PIN_VALID) { /* check pin validity bit */
                        addr = (*pin_table & GPIO_PIN_ADDR) >> 3; /* prepare address of port */ 
                        pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
                        if (addr < sizeof(GPIO_PIN_MAP)) /* pin address out of map scope? */
                            if (dev_data_ptr->pin_map.memory8[addr] & pin) { /* pin address already used? */
                                if (*pin_table & GPIO_PIN_STATUS)
                                    temp_pin1_map_ptr->memory8[addr] |= pin;
                                else
                                    temp_pin0_map_ptr->memory8[addr] |= pin;
                                continue; /* manage next pin */
                            }
                    }
                    /* some problem occured */
                    _int_enable();
                    _mem_free(temp_pin1_map_ptr);
                    _mem_free(temp_pin0_map_ptr);
                    return IO_ERROR;
                }
                /* ok, now we can apply new map */
                /* note: applying the map after collecting pins is due to have pins applied in one instruction */
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 0) = temp_pin1_map_ptr->memory32[0];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 0) = ~temp_pin0_map_ptr->memory32[0];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 1) = temp_pin1_map_ptr->memory32[1];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 1) = ~temp_pin0_map_ptr->memory32[1];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 2) = temp_pin1_map_ptr->memory32[2];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 2) = ~temp_pin0_map_ptr->memory32[2];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 3) = temp_pin1_map_ptr->memory32[3];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 3) = ~temp_pin0_map_ptr->memory32[3];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 4) = temp_pin1_map_ptr->memory32[4];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 4) = ~temp_pin0_map_ptr->memory32[4];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->PORTTEP_SETTE)) + 5) = temp_pin1_map_ptr->memory32[5];
                * (((uint_32 _PTR_) (&mcf5225_gpio_ptr->CLRTE)) + 5) = ~temp_pin0_map_ptr->memory32[5];
                _int_enable();
                _mem_free(temp_pin1_map_ptr);
                _mem_free(temp_pin0_map_ptr);
            }
        }
        break;

        case GPIO_IOCTL_READ: {
        /* Read status of pins, if configured as input. Params: pointer to predefined table where data will be written */
            GPIO_PIN_STRUCT _PTR_  pin_table;
            uint_32        addr;
            uint_8         pin;

            if (dev_data_ptr->type != DEV_INPUT)
                return IO_ERROR;
            if (param_ptr == NULL) /* no list defined */
                return IO_ERROR;
            _int_disable();
            /* check if there is not pin in the list which this file does not contain */
            for (pin_table = (GPIO_PIN_STRUCT _PTR_) param_ptr; *pin_table != GPIO_LIST_END; pin_table++) {
                if (*pin_table & GPIO_PIN_VALID) { /* check pin validity bit */
                    addr = (*pin_table & GPIO_PIN_ADDR) >> 3; /* prepare address of port */ 
                    pin = 1 << (*pin_table & 0x07);  /* prepare bit mask */
                    if (addr < sizeof(GPIO_PIN_MAP)) /* pin address out of map scope? */
                        if (dev_data_ptr->pin_map.memory8[addr] & pin) { /* pin address already used? */
                            if ((addr == 8) && (gpio_global_irq_map.memory8[addr] & 0xAA & pin)) { /* EPORT port and pin 1, 3, 5, or 7 */
                                if ( *(((uint_8 _PTR_) &mcf5225_eport_ptr->EPPDR)) & pin) /* check pin status */
                                    *pin_table |= GPIO_PIN_STATUS; /* set pin status to 1 in the list */
                                else
                                    *pin_table &= ~GPIO_PIN_STATUS; /* clear pin status to 0 in the list */
                                continue;
                            }
                            else { /* GPIO port */
                                if ( *(((uint_8 _PTR_) &mcf5225_gpio_ptr->PORTTEP_SETTE) + addr) & pin) /* check pin status */
                                    *pin_table |= GPIO_PIN_STATUS; /* set pin status to 1 in the list */
                                else
                                    *pin_table &= ~GPIO_PIN_STATUS; /* clear pin status to 0 in the list */
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

        case GPIO_IOCTL_ENABLE_IRQ:
            mcf5225_eport_ptr->EPIER |= (dev_data_ptr->irqp_map.reg.portnq | dev_data_ptr->irqn_map.reg.portnq); /* set interrupt enable */
            break;

        case GPIO_IOCTL_DISABLE_IRQ:
            mcf5225_eport_ptr->EPIER &= ~(dev_data_ptr->irqp_map.reg.portnq | dev_data_ptr->irqn_map.reg.portnq); /* set interrupt disable */
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
*
*END*********************************************************************/
static void gpio_eport_irq (pointer param) {
    GPIO_DEV_DATA_PTR gptr = first_irq;
    
    while (gptr) { /* list through all irq files */
        if ((gptr->irqp_map.reg.portnq | gptr->irqn_map.reg.portnq) & mcf5225_eport_ptr->EPFR)
            gptr->irq_func(NULL);
        gptr = gptr->NEXT;
    }
    mcf5225_eport_ptr->EPFR = 0xFF; /* clear flag(s) */
}

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
    return (pointer)(&((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO);
}

/* EOF */
