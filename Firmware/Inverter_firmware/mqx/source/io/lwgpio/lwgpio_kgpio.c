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
* $FileName: lwgpio_kgpio.c$
* $Version : 3.8.6.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   This file contains the GPIO standard functions used on boards
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "lwgpio.h"

const volatile uint32_t *pcr_reg_arr[] = { 
    &PORT_PCR_REG(PORTA_BASE_PTR, 0),
    &PORT_PCR_REG(PORTB_BASE_PTR, 0),
    &PORT_PCR_REG(PORTC_BASE_PTR, 0),
    &PORT_PCR_REG(PORTD_BASE_PTR, 0),
    &PORT_PCR_REG(PORTE_BASE_PTR, 0)
#ifdef PORTF_BASE_PTR 
    ,&PORT_PCR_REG(PORTF_BASE_PTR, 0)
#endif
};

const static GPIO_MemMapPtr gpio_ptr_arr[] = {
    PTA_BASE_PTR,
    PTB_BASE_PTR,
    PTC_BASE_PTR,
    PTD_BASE_PTR,
    PTE_BASE_PTR
#ifdef PTF_BASE_PTR 
    ,PTF_BASE_PTR
#endif
};

#define LWGPIO_PIN_FROM_ID(id) (((id) & LWGPIO_PIN_MASK) >> LWGPIO_PIN_SHIFT)

#define LWGPIO_PORT_FROM_ID(id) (((id) & LWGPIO_PORT_MASK) >> LWGPIO_PORT_SHIFT)

/* Interrupt mode definitions */
#define LWGPIO_INT_MODE_SHIFT (16) /* place it here to have on the same place as PCR_IRQC */
#define LWGPIO_INT_MODE_MASK (0x0F << LWGPIO_INT_MODE_SHIFT)
#define LWGPIO_INT_MODE(x) (((x) << LWGPIO_INT_MODE_SHIFT) & LWGPIO_INT_MODE_MASK)
#define LWGPIO_INT_MODE_FROM_ID(id) (((id) & LWGPIO_INT_MODE_MASK) >> LWGPIO_INT_MODE_SHIFT)

/* Direction field definitions */
#define LWGPIO_DIR_SHIFT (20)
#define LWGPIO_DIR_MASK (0x01 << LWGPIO_DIR_SHIFT)

#define MUX_GPIO 0x01 /* GPIO is in Alternative 1 peripheral mode */

/* Flags for Pull-up/down */
#define LWGPIO_PULL_UP_SHIFT    (21)
#define LWGPIO_PULL_DOWN_SHIFT  (22)
#define LWGPIO_PULL_UP_MASK     (0x01 << LWGPIO_PULL_UP_SHIFT)
#define LWGPIO_PULL_DOWN_MASK   (0x01 << LWGPIO_PULL_DOWN_SHIFT)

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
    uint_32 port_idx, pin_idx;
    PORT_MemMapPtr port_ptr;

    port_idx = LWGPIO_PORT_FROM_ID(id);
    pin_idx = LWGPIO_PIN_FROM_ID(id);

    handle->flags = id;
    handle->pcr_reg = (uint_32 *) pcr_reg_arr[port_idx] + pin_idx;
    handle->gpio_ptr = gpio_ptr_arr[port_idx];
    handle->pinmask = 1 << pin_idx;

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
    switch (attribute_id) 
    {
        case(LWGPIO_ATTR_PULL_UP):
        {
            if (value == LWGPIO_AVAL_ENABLE) 
            {
                *handle->pcr_reg |= (PORT_PCR_PS_MASK | PORT_PCR_PE_MASK);
                handle->flags |=  LWGPIO_PULL_UP_MASK;
                handle->flags &= ~LWGPIO_PULL_DOWN_MASK;
            } 
            /* disable only if pull down is not enabled */
            else  
            {
                if ((value == LWGPIO_AVAL_DISABLE) && !(handle->flags & LWGPIO_PULL_DOWN_MASK))
                {
                    *handle->pcr_reg &= ~PORT_PCR_PE_MASK;
                    handle->flags &= ~LWGPIO_PULL_UP_MASK;
                }
            }
            break;
        }
        case(LWGPIO_ATTR_PULL_DOWN):
        {
            if (value == LWGPIO_AVAL_ENABLE) 
            {
                *handle->pcr_reg &= ~(PORT_PCR_PS_MASK);
                *handle->pcr_reg |= (PORT_PCR_PE_MASK);
                handle->flags &= ~LWGPIO_PULL_UP_MASK;
                handle->flags |=  LWGPIO_PULL_DOWN_MASK;
            } 
            /* disable only if pull up is not enabled */
            else 
            {
                if ((value == LWGPIO_AVAL_DISABLE) && !(handle->flags & LWGPIO_PULL_UP_MASK))
                {
                    *handle->pcr_reg &= ~PORT_PCR_PE_MASK;
                    handle->flags &= ~LWGPIO_PULL_DOWN_MASK;
                }
            }
            break;
        }
        case(LWGPIO_ATTR_SLEW_RATE):
        {
            if (value == LWGPIO_AVAL_SLEW_RATE_SLOW)
            {
                *handle->pcr_reg |= PORT_PCR_SRE_MASK;
            }
            else
            {
                *handle->pcr_reg &= ~(PORT_PCR_SRE_MASK);
            }
            break;
        }
        case(LWGPIO_ATTR_FILTER):
        {
            if (value == LWGPIO_AVAL_ENABLE)
            {
                *handle->pcr_reg |= PORT_PCR_PFE_MASK;
            }
            else
            {
                *handle->pcr_reg &= ~(PORT_PCR_PFE_MASK);
            }
            break;
        }
        case(LWGPIO_ATTR_OPEN_DRAIN):
        {
            if (value == LWGPIO_AVAL_ENABLE)
            {
                *handle->pcr_reg |= PORT_PCR_ODE_MASK;
            }
            else
            {
                *handle->pcr_reg &= ~(PORT_PCR_ODE_MASK);
            }
            break;
        }
        case(LWGPIO_ATTR_DRIVE_STRENGTH):
        {
            if (value == LWGPIO_AVAL_DRIVE_STRENGTH_HIGH)
            {
                *handle->pcr_reg |= PORT_PCR_DSE_MASK;
            }
            else
            {
                *handle->pcr_reg &= ~(PORT_PCR_DSE_MASK);
            }
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
    *handle->pcr_reg &= ~PORT_PCR_MUX_MASK;
    *handle->pcr_reg |= PORT_PCR_MUX(function);
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
    return (*handle->pcr_reg & PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT;
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
    if (dir == LWGPIO_DIR_INPUT) {
        handle->gpio_ptr->PDDR &= ~handle->pinmask;
        handle->flags |= LWGPIO_DIR_MASK; /* mark the pin is input */
    }
    else if (dir == LWGPIO_DIR_OUTPUT) {
        handle->gpio_ptr->PDDR |= handle->pinmask;
        handle->flags &= ~LWGPIO_DIR_MASK; /* mark the pin is output */
    }
    else { /* LWGPIO_DIR_NOCHANGE or other value */
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
    else { /* LWGPIO_VALUE_NOCHANGE or other value */
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
    /* if the pin is set to output */
    if (handle->gpio_ptr->PDDR & handle->pinmask) /* alternative: (handle->flags & LWGPIO_DIR_MASK == 0) */
        return (handle->gpio_ptr->PDOR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    else
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
    return INT_PORTA + LWGPIO_PORT_FROM_ID(handle->flags);
}

/* EOF */
