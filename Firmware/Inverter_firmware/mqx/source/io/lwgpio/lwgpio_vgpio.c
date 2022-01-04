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
* $FileName: lwgpio_vgpio.c$
* $Version : 3.8.3.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the GPIO standard functions used on boards
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "lwgpio_vgpio.h"
#include "lwgpio.h"


const volatile uint32_t *pcr_reg_arr[] = { 
    &PORT_PCR_REG(PORT0_BASE_PTR, 0),
    &PORT_PCR_REG(PORT1_BASE_PTR, 0),
    &PORT_PCR_REG(PORT2_BASE_PTR, 0),
    &PORT_PCR_REG(PORT3_BASE_PTR, 0),
    &PORT_PCR_REG(PORT4_BASE_PTR, 0)
#ifdef PORT5_BASE_PTR
    ,&PORT_PCR_REG(PORT5_BASE_PTR, 0)
#endif
};

const static GPIO_MemMapPtr gpio_ptr_arr[] = {
    PT0_BASE_PTR,
    PT1_BASE_PTR,
    PT2_BASE_PTR,
    PT3_BASE_PTR,
    PT4_BASE_PTR
#ifdef PORT5_BASE_PTR
    ,PT5_BASE_PTR
#endif
};


#define LWGPIO_PIN_FROM_ID(id) (((id) & LWGPIO_PIN_MASK) >> LWGPIO_PIN_SHIFT)

#define LWGPIO_PORT_FROM_ID(id) (((id) & LWGPIO_PORT_MASK) >> LWGPIO_PORT_SHIFT)

/* Interrupt mode definitions */
#define LWGPIO_INT_MODE_SHIFT (16) /* place it here to have on the same place as PCR_IRQC */
#define LWGPIO_INT_MODE_MASK (0x0F << LWGPIO_INT_MODE_SHIFT)
#define LWGPIO_INT_MODE(x) (((x) << LWGPIO_INT_MODE_SHIFT) & LWGPIO_INT_MODE_MASK)
#define LWGPIO_INT_MODE_FROM_ID(id) (((id) & LWGPIO_INT_MODE_MASK) >> LWGPIO_INT_MODE_SHIFT)

// /* Direction field definitions */
// #define LWGPIO_DIR_SHIFT (20)
// #define LWGPIO_DIR_MASK (0x01 << LWGPIO_DIR_SHIFT)

#define MUX_GPIO 0x0 /* GPIO is in Alternative 0 peripheral mode */

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_init
* Returned Value   : TRUE if succesfull, FALSE otherwise
* Comments         :
*    Decodes ID to HW specific struct and then performs pin initialization
*
*END*********************************************************************/
boolean lwgpio_init
(
    /* Pointer to LWGPIO internal structure to be filled in */
    LWGPIO_STRUCT_PTR handle,
    /* Pin ID, bitmask integer value */
    LWGPIO_PIN_ID     id,
    /* Direction to be set within initialization */
    LWGPIO_DIR        dir,
    /* Value to be set within initialization */
    LWGPIO_VALUE      value
)
{ /* Body */
    uint_32 port_idx, pin_idx, temp, ta, tb;
    PORT_MemMapPtr port_ptr;
    
    port_idx = LWGPIO_PORT_FROM_ID(id);
    pin_idx = LWGPIO_PIN_FROM_ID(id);
    handle->flags = id;
    handle->pcr_reg = (uint_32 *) pcr_reg_arr[port_idx] + pin_idx;
    handle->gpio_ptr = gpio_ptr_arr[port_idx];
    handle->pinmask = 1 << pin_idx;
    handle->iomuxc_reg = (uint_32 *)&IOMUXC_RGPIO(((port_idx << 5) + pin_idx));
    
    /* Set value prior to set to output */
    if (value != LWGPIO_VALUE_NOCHANGE) {
        /* Note: there is no check for values not defined as LWGPIO_VALUE enum */
        lwgpio_set_value(handle, value);
    }

    if (dir != LWGPIO_DIR_NOCHANGE) {
        /* Note: there is no check for values not defined as LWGPIO_DIR enum */
        lwgpio_set_direction(handle, dir);
    }

    return TRUE;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_set_attribute
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    Sets attributes
*
*END*********************************************************************/
boolean lwgpio_set_attribute
(
    /* Pin handle to get function from */
    LWGPIO_STRUCT_PTR  handle,
    /* PORT attribute */
    uint_32 attribute_id,
    /* Attribute value */
    uint_32 value
)
{
    uint_32 temp = 0;
    switch (attribute_id) 
    {
        case(LWGPIO_ATTR_PULL_UP):
        {
            if (value == LWGPIO_AVAL_ENABLE)
            {
                /* select pull mode, enable pull mode */
                temp = *handle->iomuxc_reg;
                temp |= IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) | IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1);
                /* if pull down resistor is selected, change to 47k pull up */
                if (!(temp & IOMUXC_SW_MUX_CTL_PAD_PAD_PUS_MASK))
                {
                    temp = IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1);
                }
                *handle->iomuxc_reg = temp;
            }
            else
            {
                *handle->iomuxc_reg &= ~IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK;
            }
            break;
        }
        case(LWGPIO_ATTR_PULL_DOWN):
        {
            if (value == LWGPIO_AVAL_ENABLE)
            {
                temp = *handle->iomuxc_reg;
                /* select 100k pull down resistor */
                temp &= ~(IOMUXC_SW_MUX_CTL_PAD_PAD_PUS_MASK);
                /* select pull mode, enable pull mode */
                temp |= IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) | IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1);
                *handle->iomuxc_reg = temp;
            } 
            else 
            {
                *handle->iomuxc_reg &= ~IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK;
            }
            break;
        }
        case(LWGPIO_ATTR_SLEW_RATE):
        {
            if (value == LWGPIO_AVAL_SLEW_RATE_SLOW)
            {
                *handle->iomuxc_reg |= IOMUXC_SW_MUX_CTL_PAD_PAD_SRE(1);
            }
            else
            {
                *handle->iomuxc_reg &= ~(IOMUXC_SW_MUX_CTL_PAD_PAD_SRE(1));
            }
            break;
        }
        case(LWGPIO_ATTR_OPEN_DRAIN):
        {
            if (value == LWGPIO_AVAL_ENABLE)
            {
                *handle->iomuxc_reg |= IOMUXC_SW_MUX_CTL_PAD_ODE(1);
            }
            else
            {
                *handle->iomuxc_reg &= ~(IOMUXC_SW_MUX_CTL_PAD_ODE(1));
            }
            break;
        }
        case(LWGPIO_ATTR_DRIVE_STRENGTH):
        {
            *handle->pcr_reg = \
                (*handle->pcr_reg & (~IOMUXC_SW_MUX_CTL_PAD_PAD_DSE_MASK)) | \
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(value);
            break;
        }
        default:
            return FALSE;
    }
    return TRUE;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_set_functionality
* Returned Value   : void
* Comments         :
*    Sets functionality (peripheral mode) of the pin
*
*END*********************************************************************/
void lwgpio_set_functionality
(
    /* Pin handle to set function on */
    LWGPIO_STRUCT_PTR  handle, 
    /* Function to be set (integer value) */
    uint_32            function
)
{
    *handle->iomuxc_reg &= ~IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE_MASK;
    *handle->iomuxc_reg |= IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(function);
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_get_functionality
* Returned Value   : void
* Comments         :
*    Gets functionality (peripheral mode) of the pin
*
*END*********************************************************************/
uint_32 lwgpio_get_functionality
(
    /* Pin handle to get function from */
    LWGPIO_STRUCT_PTR  handle
)
{
    return (*handle->iomuxc_reg & IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE_MASK) >> IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE_SHIFT;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_set_direction
* Returned Value   : void
* Comments         :
*    Sets direction of the pin
*
*END*********************************************************************/
void lwgpio_set_direction
(
    /* Pin handle to set direction on */
    LWGPIO_STRUCT_PTR  handle, 
    /* Direction to be set */
    LWGPIO_DIR         dir
)
{
    uint_32 temp;
    if (dir == LWGPIO_DIR_INPUT) {
        temp = *handle->iomuxc_reg;
        temp &= ~(IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK | IOMUXC_SW_MUX_CTL_PAD_PAD_DSE_MASK);
        temp |= IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1);
        *handle->iomuxc_reg = temp;
    }
    else if (dir == LWGPIO_DIR_OUTPUT) {
        temp = *handle->iomuxc_reg;
        temp &= ~(IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK);
        temp |= IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1);
        /* DSE disabled, set 1 - k150 */
        if (!(temp & IOMUXC_SW_MUX_CTL_PAD_PAD_DSE_MASK))
        {
            temp |= IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1);
        }
        *handle->iomuxc_reg = temp;
    }
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_set_value
* Returned Value   : void
* Comments         :
*    Sets value (output latch) of the pin
*
*END*********************************************************************/
void lwgpio_set_value
(
    /* Pin handle to set value on */
    LWGPIO_STRUCT_PTR  handle, 
    /* Direction to be set */
    LWGPIO_VALUE       out_value
)
{
    if (out_value == LWGPIO_VALUE_LOW) {
        handle->gpio_ptr->PDOR &= ~handle->pinmask;
    }
    else if (out_value == LWGPIO_VALUE_HIGH) {
        handle->gpio_ptr->PDOR |= handle->pinmask;
    }
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_toggle_value
* Returned Value   : void
* Comments         :
*    Toggles value of output latch of the pin
*
*END*********************************************************************/
void lwgpio_toggle_value
(
    /* Pin handle to toggle value on */
    LWGPIO_STRUCT_PTR  handle
)
{
    handle->gpio_ptr->PTOR = handle->pinmask;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_get_value
* Returned Value   : LWGPIO_VALUE of pin status
* Comments         :
*    Returns value (output latch or read data) of the pin
*
*END*********************************************************************/
LWGPIO_VALUE lwgpio_get_value
(
    /* Pin handle to get value from */
    LWGPIO_STRUCT_PTR  handle
)
{
    uint_32 temp = IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;
    /* if the pin is set to input */
    // TODO: temp is used as workaround, otherwise compiler will use ldrh/ldrb instruction and cause unhandled interrupt
    if ((uint_32)(*handle->iomuxc_reg) & temp)
       return (handle->gpio_ptr->PDIR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    /* if the pin is set to output */
    temp = IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;
    if ((*handle->iomuxc_reg) & temp)
       return (handle->gpio_ptr->PDOR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    
    return (handle->gpio_ptr->PDIR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_get_raw
* Returned Value   : LWGPIO_VALUE of pin status
* Comments         :
*    Returns read level of the pin
*
*END*********************************************************************/
LWGPIO_VALUE lwgpio_get_raw
(
    /* Pin handle to get value from */
    LWGPIO_STRUCT_PTR  handle
)
{
    return (handle->gpio_ptr->PDIR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_int_init
* Returned Value   : TRUE if succesfull, FALSE otherwise
* Comments         :
*    Initializes pin to generate interrupt
*
*END*********************************************************************/
boolean lwgpio_int_init
(
    /* Pin handle to initialize interrupt on */
    LWGPIO_STRUCT_PTR handle,
    /* Interrupt mode */
    LWGPIO_INT_MODE   mode
)
{ /* Body */
    /*
    Settings of PORT_PCR_IRQC according mode:
    PORT_PCR_IRQC | LOW | HIGH | FALLING | RISING | mode
    ----------------------------------------------------
               0  |   0 |    0 |       0 |      0 |    0
     0x08 + 0x01  |   0 |    0 |       0 |      1 |    1
     0x08 + 0x02  |   0 |    0 |       1 |      0 |    2
     0x08 + 0x03  |   0 |    0 |       1 |      1 |    3
     0x08 + 0x04  |   0 |    1 |       0 |      0 |    4
     unsupported  |   0 |    1 |       0 |      1 |    5
     unsupported  |   0 |    1 |       1 |      0 |    6
     unsupported  |   0 |    1 |       1 |      1 |    7
     0x08 + 0x00  |   1 |    0 |       0 |      0 |    8
     unsupported  |   1 |    0 |       0 |      1 |    9
     unsupported  |   1 |    0 |       1 |      0 |   10
     unsupported  |   1 |    0 |       1 |      1 |   11
     unsupported  |   1 |    1 |       0 |      0 |   12
     unsupported  |   1 |    1 |       0 |      1 |   13
     unsupported  |   1 |    1 |       1 |      0 |   14
     unsupported  |   1 |    1 |       1 |      1 |   15
    */

     if (mode > 4 && mode != LWGPIO_INT_MODE_LOW)
         return FALSE;
     
     if (mode) {
         /* Store mode value and use it when enabling interrupt */
         handle->flags &= ~LWGPIO_INT_MODE_MASK;
         handle->flags |= LWGPIO_INT_MODE(0x08 | mode & 0x07); /* see PCR_IRQC register field definition */
     }

    return TRUE;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_int_enable
* Returned Value   : void
* Comments         :
*    Enables / disables interrupts for specified pin
*
*END*********************************************************************/
void lwgpio_int_enable
(
    /* Pin handle to enable interrupt on */
    LWGPIO_STRUCT_PTR handle,
    /* Enable or disable interrupt? TRUE = enable */
    boolean           ena
)
{ /* Body */
     if (ena) {
         /* enable pin interrupts */
         *handle->pcr_reg &= ~PORT_PCR_IRQC_MASK;
         *handle->pcr_reg |= handle->flags & PORT_PCR_IRQC_MASK; /* the IRQ mode flags are on the same place as PCR_IRQC */
     }
     else {
         /* disable pin interrupts */
         *handle->pcr_reg &= ~PORT_PCR_IRQC_MASK;
     }
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_int_get_flag
* Returned Value   : TRUE if interrupt flag is set
* Comments         :
*    Checks if there is pending interrupt flag for specified pin
*
*END*********************************************************************/
boolean lwgpio_int_get_flag
(
    /* Pin handle to get interrupt flag on */
    LWGPIO_STRUCT_PTR handle
)
{ /* Body */
    return (*handle->pcr_reg & PORT_PCR_ISF_MASK) ? TRUE : FALSE;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_int_clear_flag
* Returned Value   : void
* Comments         :
*    Clears pending interrupt flag on peripheral
*
*END*********************************************************************/
void lwgpio_int_clear_flag
(
    /* Pin handle to clear interrupt flag on */
    LWGPIO_STRUCT_PTR handle
)
{ /* Body */
    /* Clear interrupt flag. Note that if an interrupt was acknowledged by NVIC,
    it will generate interrupt anyway. Interrupts on NVIC are grouped for each
    port */
    *handle->pcr_reg |= PORT_PCR_ISF_MASK;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_get_gpio_base_address
* Returned Value   : pointer to base of GPIO registers
* Comments         :
*    This function returns base address of GPIO related register space.
*
*END*********************************************************************/
uint_32 lwgpio_int_get_vector
(
    /* Pin handle to get vector of */
    LWGPIO_STRUCT_PTR  handle
)
{
#if PSP_MQX_CPU_IS_VYBRID_A5
	// return GIC ID
	return 155 + LWGPIO_PORT_FROM_ID(handle->flags);
#elif PSP_MQX_CPU_IS_VYBRID_M4
	// return CM4 VECTOR
	return 123 + LWGPIO_PORT_FROM_ID(handle->flags);
#else
	#error "Unsupported core"
#endif
}

/* EOF */
