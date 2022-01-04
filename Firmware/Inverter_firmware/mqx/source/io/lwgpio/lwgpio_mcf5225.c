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
* $FileName: lwgpio_mcf5225.c$
* $Version : 3.8.9.1$
* $Date    : Aug-1-2012$
*
* Comments:
*
*   This file contains the GPIO standard functions used on boards
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "lwgpio.h"

enum {
    PORTTE = 0, PORTTF, PORTTG, PORTTH,
    PORTTI, DUMMY0, PORTTJ, DUMMY1,
    PORTNQ, DUMMY2, PORTAN, PORTAS, 
    PORTQS, DUMMY3, PORTTA, PORTTC, 
    DUMMY4, PORTUA, PORTUB, PORTUC, 
    PORTDD, DUMMY5, DUMMY6, DUMMY7
};

enum {
    REG_PORT = 0,
    REG_DDR = 1,
    REG_SET = 2, /* also REG_PORTP */
    REG_CLR = 3,
    REG_PAR = 4,
    REG_PCR = 5
    
};

#define LWGPIO_PIN_FROM_ID(id) (((id) & LWGPIO_PIN_MASK) >> LWGPIO_PIN_SHIFT)
#define LWGPIO_PORT_FROM_ID(id) (((id) & LWGPIO_PORT_MASK) >> LWGPIO_PORT_SHIFT)

/* Interrupt mode definitions */
#define LWGPIO_INT_MODE_SHIFT (11)
#define LWGPIO_INT_MODE_MASK (0x03 << LWGPIO_INT_MODE_SHIFT)
#define LWGPIO_INT_MODE(x) (((x) << LWGPIO_INT_MODE_SHIFT) & LWGPIO_INT_MODE_MASK)
#define LWGPIO_INT_MODE_FROM_ID(id) (((id) & LWGPIO_INT_MODE_MASK) >> LWGPIO_INT_MODE_SHIFT)

/* Direction field definitions */
#define LWGPIO_DIR_SHIFT (13)
#define LWGPIO_DIR_MASK (0x01 << LWGPIO_DIR_SHIFT)

static volatile uint_8 *gpio_ptr[REG_PCR + 1];

static VMCF522XX_EPORT_STRUCT_PTR eport_ptr;

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_init
* Returned Value   : TRUE if successful, FALSE otherwise
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
    static boolean first_run = TRUE;
    uint_32 port_idx, pin_idx;

    if (first_run) {
        /* Update gpio_ptr to point to the register based on IPSBAR */
        gpio_ptr[REG_PORT] = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PORTTE);
        gpio_ptr[REG_DDR] = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.DDRTE);
        gpio_ptr[REG_SET] = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PORTTEP_SETTE);
        gpio_ptr[REG_CLR] = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.CLRTE);
        gpio_ptr[REG_PAR] = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PTEPAR);
        (volatile uint_32 *)gpio_ptr[REG_PCR] = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PSRR);

        eport_ptr = &(((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->EPORT[0]);
        first_run = FALSE;
    }

    handle->port_idx = LWGPIO_PORT_FROM_ID(id);
    handle->pinmask = 1 << LWGPIO_PIN_FROM_ID(id);
    handle->flags = id;

    /* By default, port has pins 0..7 available. There are exceptions, handled here. */
    switch(handle->port_idx) {
        case PORTTA:
        case PORTTC:
        case PORTUA:
        case PORTUB:
        case PORTUC:
            if (LWGPIO_PIN_FROM_ID(id) > 3)
                return FALSE;
            break;
        case PORTQS:
            if ((LWGPIO_PIN_FROM_ID(id)== 4)||(LWGPIO_PIN_FROM_ID(id)== 7))
                return FALSE;
            break;
        case PORTTH:
        	break;
        case PORTAS:
            if (LWGPIO_PIN_FROM_ID(id) > 2)
                return FALSE;
            break;
        case PORTNQ:
            if (LWGPIO_PIN_FROM_ID(id) & 0x01 == 0x00)
                return FALSE;
            break;
    }

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
/* shift mask for set_attribute */
static const uint_8 port_shift[]={
    0, 8, 24, 16,
    0, 0, 8, 0,
    0, 0, 0, 18,
    1, 0, 20, 16,
    0, 8, 12, 24,
    0, 0, 0, 0
};
    switch(handle->port_idx)
    {
    case PORTTA:
    case PORTAS:
    case PORTQS:
    case PORTTC:
    case PORTUA:
    case PORTUB:
    case PORTUC:
    { 
    if (((LWGPIO_PIN_FROM_ID(handle->flags)) == 2) && handle->port_idx==PORTAS)    /* PAS2 exception */
        { 
        /* PSSRL */
            if (attribute_id == LWGPIO_ATTR_SLEW_RATE) 
            {
                volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+8);
                
                if (value == LWGPIO_AVAL_SLEW_RATE_FAST) 
                {
                    *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+18);
                }
                else
                {
                    *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+18);
                }
                return TRUE;
            /* PDSRL */
            } else if (attribute_id == LWGPIO_ATTR_DRIVE_STRENGTH)
            {
                volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+16);
                
                if (value == LWGPIO_AVAL_DRIVE_STRENGTH_LOW) 
                {
                    *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+18);
                }
                else
                {
                    *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+18);
                }
                return TRUE;
            }
            break;
        }
    /* PSSR */
        if (attribute_id == LWGPIO_ATTR_SLEW_RATE) 
        {
            volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]);
            
            if (value == LWGPIO_AVAL_SLEW_RATE_FAST) 
            {
                *pcr &= value << ((LWGPIO_PIN_FROM_ID(handle->flags)) + port_shift[handle->flags]);
            }
            else
            {
                *pcr |= value  << ((LWGPIO_PIN_FROM_ID(handle->flags)) + port_shift[handle->flags]);
            }
            return TRUE;
    /* PDSR */
        } else if (attribute_id == LWGPIO_ATTR_DRIVE_STRENGTH)
        {
            volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+4);
            
            if (value == LWGPIO_AVAL_DRIVE_STRENGTH_LOW) 
            {
                *pcr &= value << ((LWGPIO_PIN_FROM_ID(handle->flags))+port_shift[handle->flags]);
            }
            else
            {
                *pcr |= value  << ((LWGPIO_PIN_FROM_ID(handle->flags))+port_shift[handle->flags]);
            }
            return TRUE;
        }
        break;
    }    
    case PORTTJ:
    case PORTTI:
    {    
    /* PSRRH */
        if (attribute_id == LWGPIO_ATTR_SLEW_RATE) 
        {
            volatile uint_16 *pcr = (volatile uint_16 *)(gpio_ptr[REG_PCR]+14);
            
            if (value == LWGPIO_AVAL_SLEW_RATE_FAST) 
            {
                *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            else
            {
                *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            return TRUE;
    /* PDSRH */
        } else if (attribute_id == LWGPIO_ATTR_DRIVE_STRENGTH)
        {
            volatile uint_16 *pcr = (volatile uint_16 *)(gpio_ptr[REG_PCR]+22);
            
            if (value == LWGPIO_AVAL_DRIVE_STRENGTH_LOW) 
            {
                *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            else
            {
                *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            return TRUE;
        }
        break;
    }
    case PORTTE:
    case PORTTF:
    case PORTTG:
    case PORTTH:
    { 
    /* PSSRL */
        if (LWGPIO_PIN_FROM_ID(handle->flags) >5 && (handle->port_idx == PORTTH)) /* PTH6 & PTH7 exception */
        {
            if (attribute_id == LWGPIO_ATTR_SLEW_RATE) 
            {
                volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+8);
                
                if (value == LWGPIO_AVAL_SLEW_RATE_FAST) 
                {
                    *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+16);
                }
                else
                {
                    *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+16);
                }
                return TRUE;
        /* PDSRL */
            } else if ( attribute_id == LWGPIO_ATTR_DRIVE_STRENGTH)
            {
                volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+16);
                
                if (value == LWGPIO_AVAL_DRIVE_STRENGTH_LOW) 
                {
                    *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+16);
                }
                else
                {
                    *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+16);
                }
                return TRUE;
            }
        }
        if (LWGPIO_PIN_FROM_ID(handle->flags) >5 && (handle->port_idx == PORTTG)) /* PTG6 & PTG7 exception */
        {
            if (attribute_id == LWGPIO_ATTR_SLEW_RATE) 
            {
                volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+8);
                
                if (value == LWGPIO_AVAL_SLEW_RATE_FAST) 
                {
                    *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+24);
                }
                else
                {
                    *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+24);
                }
                return TRUE;
        /* PDSRL */
            } else if (attribute_id == LWGPIO_ATTR_DRIVE_STRENGTH)
            {
                volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+16);
                
                if (value == LWGPIO_AVAL_DRIVE_STRENGTH_LOW) 
                {
                    *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+24);
                }
                else
                {
                    *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+24);
                }
                return TRUE;
            }
        }
        if (attribute_id == LWGPIO_ATTR_SLEW_RATE) 
        {
            volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+8);
            
            if (value == LWGPIO_AVAL_SLEW_RATE_FAST) 
            {
                *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            else
            {
                *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            return TRUE;
    /* PDSRL */
        } else if (attribute_id == LWGPIO_ATTR_DRIVE_STRENGTH)
        {
            volatile uint_32 *pcr = (volatile uint_32 *)(gpio_ptr[REG_PCR]+16);
            
            if (value == LWGPIO_AVAL_DRIVE_STRENGTH_LOW) 
            {
                *pcr &= value << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            else
            {
                *pcr |= value  << (LWGPIO_PIN_FROM_ID(handle->flags)+port_shift[handle->flags]);
            }
            return TRUE;
        }
        break;
    }
    default:
        return FALSE;
    }
    return FALSE;
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
    switch (handle->port_idx) {
        case PORTTE:
        case PORTTF:
        case PORTTG:
        case PORTTI:
        case PORTTJ:
        case PORTAN:
        case PORTAS:
        case PORTDD: /* 8-bit PAR registers, standard */
            {
                volatile uint_8 *par = gpio_ptr[REG_PAR] + handle->port_idx;

                if (function == 0)
                    *par &= ~handle->pinmask;
                else if (function == 1)
                    *par |= handle->pinmask;
            }
            break;
        case PORTTA:
        case PORTTC:
        case PORTUA:
        case PORTUB:
        case PORTUC: /* 8-bit PAR registers with quad-function */
            {
                volatile uint_8 *par = gpio_ptr[REG_PAR] + handle->port_idx;
                /* Create temp. new special pinmask for 16-bit registers */
                uint_8 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
                uint_8 functionmask = (function & 0x03) << (2 * LWGPIO_PIN_FROM_ID(handle->flags));

                *par &= ~pinmask;
                *par |= functionmask;
            }
            break;
        case PORTNQ:
        case PORTQS: /* 16-bit PAR registers with quad-function */
            {
                volatile uint_16 *par = (volatile uint_16 *) (gpio_ptr[REG_PAR] + handle->port_idx);
                /* Create temp. new special pinmask for 16-bit registers */
                uint_16 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
                uint_16 functionmask = (function & 0x03) << (2 * LWGPIO_PIN_FROM_ID(handle->flags));

                *par &= ~pinmask;
                *par |= functionmask;
            }
            break;
        case PORTTH: /* 16-bit PAR register with quad-funcition residing outside PAR area */
            {
                /* Create temp. new special pinmask for 16-bit registers */
                uint_16 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
                uint_16 functionmask = (function & 0x03) << (2 * LWGPIO_PIN_FROM_ID(handle->flags));

                ((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PTHPAR &= ~pinmask;
                ((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PTHPAR |= functionmask;
            }
            break;
    }
}

/*FUNCTION*****************************************************************
* 
* Function Name    : lwgpio_get_functionality
* Returned Value   : void
* Comments         :
*    Gets function (peripheral mode) of the pin
*
*END*********************************************************************/
uint_32 lwgpio_get_functionality
(
    /* Pin handle to get function from */
    LWGPIO_STRUCT_PTR  handle
)
{
    uint_32 retval;

    switch (handle->port_idx) {
        case PORTTE:
        case PORTTF:
        case PORTTG:
        case PORTTI:
        case PORTTJ:
        case PORTAN:
        case PORTAS:
        case PORTDD: /* 8-bit PAR registers, standard */
            retval = (*(gpio_ptr[REG_PAR] + handle->port_idx) & handle->pinmask) ? 1 : 0;
            break;
        case PORTTA:
        case PORTTC:
        case PORTUA:
        case PORTUB:
        case PORTUC: /* 8-bit PAR registers with quad-function */
            {
                /* Create temp. new special pinmask for 16-bit registers */
                uint_8 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
                /* Get 16-bit PAR value for specific port, mask to get 2-bits for specific pin */
                uint_8 parval = *(gpio_ptr[REG_PAR] + handle->port_idx) & handle->pinmask;

                retval = (parval >> (2 * LWGPIO_PIN_FROM_ID(handle->flags))) & 0x03;
            }
            break;
        case PORTNQ:
        case PORTQS: /* 16-bit PAR registers with quad-function */
            {
                /* Create temp. new special pinmask for 16-bit registers */
                uint_16 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
                /* Get 16-bit PAR value for specific port, mask to get 2-bits for specific pin */
                uint_16 parval = *(volatile uint_16 *)(gpio_ptr[REG_PAR] + handle->port_idx);

                retval = (parval >> (2 * LWGPIO_PIN_FROM_ID(handle->flags))) & 0x03;
            }                            
            break;
        case PORTTH: /* 16-bit PAR register with quad-funcition residing outside PAR area */
            {
                /* Create temp. new special pinmask for 16-bit registers */
                uint_16 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
                /* Get 16-bit PAR value for specific port, mask to get 2-bits for specific pin */
                uint_16 parval = ((VMCF5225_STRUCT_PTR)_PSP_GET_IPSBAR())->GPIO.PTHPAR;

                retval = (parval >> (2 * LWGPIO_PIN_FROM_ID(handle->flags))) & 0x03;
            }                            
            break;
        default:
            /* Bad port specified, return *any* value. MAX_UINT_32 is quite ok to signal some error */
            retval = MAX_UINT_32;
            break;
    }

    return retval;
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
        *(gpio_ptr[REG_DDR] + handle->port_idx) &= ~handle->pinmask;
        handle->flags |= LWGPIO_DIR_MASK; /* mark the pin is input */
    }
    else if (dir == LWGPIO_DIR_OUTPUT) {
        *(gpio_ptr[REG_DDR] + handle->port_idx) |= handle->pinmask;
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
        *(gpio_ptr[REG_CLR] + handle->port_idx) = ~handle->pinmask;
    }
    else if (out_value == LWGPIO_VALUE_HIGH) {
        *(gpio_ptr[REG_SET] + handle->port_idx) = handle->pinmask;
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
    lwgpio_set_value(handle, lwgpio_get_value(handle) == LWGPIO_VALUE_HIGH ? LWGPIO_VALUE_LOW : LWGPIO_VALUE_HIGH);
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
    /* if the port is set to the interrupt functionality */
    if ((handle->port_idx == PORTNQ) && (eport_ptr->EPIER & handle->pinmask))
        return (eport_ptr->EPPDR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    /* if the pin is set to output */
    if (*(gpio_ptr[REG_DDR] + handle->port_idx) & handle->pinmask) /* alternative: (handle->flags & LWGPIO_DIR_MASK == 0) */
        return (*(gpio_ptr[REG_PORT] + handle->port_idx) & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    else
        /* Note: REG_SET is an union with REG_PORTP = getting value */
        return (*(gpio_ptr[REG_SET] + handle->port_idx) & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
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
    /* if the port is set to the interrupt functionality */
    if ((handle->port_idx == PORTNQ) && (eport_ptr->EPIER & handle->pinmask))
        return (eport_ptr->EPPDR & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
    /* Note: REG_SET is an union with REG_PORTP = getting value */
    return (*(gpio_ptr[REG_SET] + handle->port_idx) & handle->pinmask) ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW;
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
    /* Create temp. new special pinmask for 16-bit registers */
    uint_16 pinmask = 0x03 << (2 * LWGPIO_PIN_FROM_ID(handle->flags));

    if (handle->port_idx != PORTNQ)
        return FALSE; /* MCF5225 ONLY: interrupts are allowed only on PORTNQ */
    if (handle->pinmask & 0x55)
        return FALSE; /* MCF5225 ONLY: interrupts are allowed only on pin 1, 3, 5 and 7 */
    if (mode > 0x04)
        return FALSE; /* interrupts are only rising, falling, rising+falling and high */
    if (mode == LWGPIO_INT_MODE_NONE)
        return TRUE; /* don't setup interrupts */

    /* setup EPPAR */
    switch (mode) {
        case LWGPIO_INT_MODE_HIGH:
            eport_ptr->EPPAR &= ~pinmask;
            break;
        case LWGPIO_INT_MODE_FALLING | LWGPIO_INT_MODE_RISING:
            eport_ptr->EPPAR |= pinmask;
            break;
        case LWGPIO_INT_MODE_FALLING:
        case LWGPIO_INT_MODE_RISING:
            eport_ptr->EPPAR &= ~pinmask;
            eport_ptr->EPPAR |= (mode & 0x03) << (2 * LWGPIO_PIN_FROM_ID(handle->flags));
            break;
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
        eport_ptr->EPIER |= handle->pinmask;
    }
    else {
        /* disable pin interrupts */
        eport_ptr->EPIER &= ~handle->pinmask;
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
    return (eport_ptr->EPFR & handle->pinmask) ? TRUE : FALSE;
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
    eport_ptr->EPFR = handle->pinmask;
}


/*FUNCTION****************************************************************
* 
* Function Name    : _bsp_int_get_vector
* Returned Value   : vector index
* Comments         :
*    This function returns vector index for specified pin
*
*END*********************************************************************/
uint_32 lwgpio_int_get_vector
(
    /* Pin handle to get vector of */
    LWGPIO_STRUCT_PTR  handle
)
{
    return MCF5225_INT_EPORT0_EPF1 - 1 + LWGPIO_PIN_FROM_ID(handle->flags);
}

/* EOF */
