/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: lwgpio_siul.c$
* $Version : 3.8.6.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the GPIO standard functions used on boards
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "lwgpio.h"

#include "lwgpio_siul.h"


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
{
    /* no decoding necessary */
    handle->id=id;
    handle->eirq=-1; /* not assigned until required - performance */
    handle->siul_ptr = _bsp_get_gpio_base_address();
    if (handle->siul_ptr == NULL) return FALSE;

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
* Returned Value   : TRUE if succesfull, FALSE otherwise
* Comments         :
*    Sets attributes
*
*END*********************************************************************/
boolean lwgpio_set_attribute
(
    /* Pin handle to get function from */
    LWGPIO_STRUCT_PTR   handle,
    /* PORT attribute */
    uint_32             attribute_id,
    /* Attribute value */
    uint_32             value
)
{
    switch (attribute_id)
    {
        case LWGPIO_ATTR_PULL_UP:
            if (value == LWGPIO_AVAL_ENABLE)
            {
                handle->siul_ptr->PCR[handle->id] |= (SIUL_PCR_WPS_MASK | SIUL_PCR_WPE_MASK);
            }
            else if ((handle->siul_ptr->PCR[handle->id] & SIUL_PCR_WPS_MASK) != 0)
            {
                /* disable pull resistor only if pull-up was selected */
                handle->siul_ptr->PCR[handle->id] &= ~SIUL_PCR_WPE_MASK;
            }
            break;

        case LWGPIO_ATTR_PULL_DOWN:
            if (value == LWGPIO_AVAL_ENABLE)
            {
                handle->siul_ptr->PCR[handle->id] &= ~SIUL_PCR_WPS_MASK;
                handle->siul_ptr->PCR[handle->id] |=  SIUL_PCR_WPE_MASK;
            }
            else if ((handle->siul_ptr->PCR[handle->id] & SIUL_PCR_WPS_MASK) == 0)
            {
                /* disable pull resistor only if pull-down was selected */
                handle->siul_ptr->PCR[handle->id] &= ~SIUL_PCR_WPE_MASK;
            }
            break;

        case LWGPIO_ATTR_OPEN_DRAIN:
            if (value == LWGPIO_AVAL_ENABLE)
            {
                handle->siul_ptr->PCR[handle->id] |= SIUL_PCR_ODE_MASK;
            }
            else /* disable */
            {
            	handle->siul_ptr->PCR[handle->id] &= ~SIUL_PCR_ODE_MASK;
            }
            break;

        case LWGPIO_ATTR_SLEW_RATE:
            if (value == LWGPIO_AVAL_SLEW_RATE_SLOW)
            {
            	handle->siul_ptr->PCR[handle->id] &= ~SIUL_PCR_SRC_MASK;
            }
            else /* fast */
            {
            	handle->siul_ptr->PCR[handle->id] |= SIUL_PCR_SRC_MASK;
            }
            break;

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
    /* keep pin direction settings */
    handle->siul_ptr->PCR[handle->id] = (handle->siul_ptr->PCR[handle->id]&SIUL_PCR_DIR_MASK) | function;
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
    return handle->siul_ptr->PCR[handle->id];
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
        handle->siul_ptr->PCR[handle->id] = (handle->siul_ptr->PCR[handle->id] & ~SIUL_PCR_DIR_MASK) | SIUL_PCR_IBE_MASK;
    }
    else if (dir == LWGPIO_DIR_OUTPUT) {
        handle->siul_ptr->PCR[handle->id] = (handle->siul_ptr->PCR[handle->id] & ~SIUL_PCR_DIR_MASK) | SIUL_PCR_OBE_MASK;
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
        handle->siul_ptr->GPDO[handle->id] = 0;
    }
    else if (out_value == LWGPIO_VALUE_HIGH) {
        handle->siul_ptr->GPDO[handle->id] = 1;
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
    handle->siul_ptr->GPDO[handle->id] = handle->siul_ptr->GPDO[handle->id] ^ 1;
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
    if (handle->siul_ptr->PCR[handle->id] & SIUL_PCR_OBE_MASK)
        /* return value from the output latch */
        return (handle->siul_ptr->GPDO[handle->id] & 1) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    else
        /* return value read from input */
        return (handle->siul_ptr->GPDI[handle->id] & 1) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
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
    return (handle->siul_ptr->GPDI[handle->id] & 1) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
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
{
    uint_32 eirq_mask;

    /* acquire eirq for pin, if necessary */
    if (handle->eirq < 0) handle->eirq=_bsp_siul_pin_to_eirq(handle->id);
    if (handle->eirq < 0) return FALSE; /* pin does not support irq */

    /* rising and falling edge supported only */
    if ((mode | LWGPIO_INT_MODE_RISING | LWGPIO_INT_MODE_FALLING) != (LWGPIO_INT_MODE_RISING | LWGPIO_INT_MODE_FALLING))
        return FALSE; /* requested mode not supported */

    eirq_mask = 1 << handle->eirq;

    if (mode & LWGPIO_INT_MODE_RISING)
        handle->siul_ptr->IREER |= eirq_mask;
    else 
        handle->siul_ptr->IREER &= ~eirq_mask;

    if (mode & LWGPIO_INT_MODE_FALLING)
        handle->siul_ptr->IFEER |= eirq_mask;
    else
        handle->siul_ptr->IFEER &= ~eirq_mask;

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
{
    uint_32 eirq_mask;

    /* acquire eirq for pin, if necessary */
    if (handle->eirq < 0) handle->eirq=_bsp_siul_pin_to_eirq(handle->id);
    if (handle->eirq < 0) return; /* pin does not support irq */

    eirq_mask = 1 << handle->eirq;

    if (ena) {
        /* enable pin interrupts */
        handle->siul_ptr->IRER |= eirq_mask;
    }
    else {
        /* disable pin interrupts */
        handle->siul_ptr->IRER &= ~eirq_mask;
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
{
    uint_32 eirq_mask;

    /* acquire eirq for pin, if necessary */
    if (handle->eirq < 0) handle->eirq=_bsp_siul_pin_to_eirq(handle->id);
    if (handle->eirq < 0) return FALSE; /* pin does not support irq */

    eirq_mask = 1 << handle->eirq;

    return (handle->siul_ptr->ISR & eirq_mask) ? TRUE : FALSE;
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
{
    uint_32 eirq_mask;

    /* acquire eirq for pin, if necessary */
    if (handle->eirq < 0) handle->eirq=_bsp_siul_pin_to_eirq(handle->id);
    if (handle->eirq < 0) return; /* pin does not support irq */

    eirq_mask = 1 << handle->eirq;

    handle->siul_ptr->ISR = eirq_mask;
}


/*FUNCTION****************************************************************
* 
* Function Name    : lwgpio_int_get_vector
* Returned Value   : pointer to base of GPIO registers
* Comments         :
*    This function returns the vector associated with the specified pin.
*
*END*********************************************************************/
uint_32 lwgpio_int_get_vector
(
    /* Pin handle to get vector of */
    LWGPIO_STRUCT_PTR  handle
)
{
    /* acquire eirq for pin, if necessary */
    if (handle->eirq < 0) handle->eirq=_bsp_siul_pin_to_eirq(handle->id);
    if (handle->eirq < 0) return 0; /* pin does not support irq */

    return _bsp_siul_int_get_vector(handle->eirq);
}
