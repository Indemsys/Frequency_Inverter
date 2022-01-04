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
* $FileName: lwgpio.c$
* $Version : 3.8.1.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   The file contains functions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"
#include "lwgpio.h"

/*FUNCTION*****************************************************************
* 
* Function Name    : LWGPIO_SET_PIN_OUTPUT
* Returned Value   : TRUE if successfull
* Comments         :
*    Set pin to output and set its state
*
*END*********************************************************************/
boolean lwgpio_set_pin_output(LWGPIO_PIN_ID id, LWGPIO_VALUE pin_state)
{
    LWGPIO_STRUCT tmp;
    return lwgpio_init(&tmp, id, LWGPIO_DIR_OUTPUT, pin_state);
}
        
/*FUNCTION*****************************************************************
* 
* Function Name    : LWGPIO_TOGGLE_PIN_OUTPUT
* Returned Value   : TRUE if succesfull 
* Comments         :
*    Toggles output pin state
*
*END*********************************************************************/
boolean lwgpio_toggle_pin_output(LWGPIO_PIN_ID id)
{
    LWGPIO_STRUCT tmp;
    if (lwgpio_init(&tmp, id, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)) {
        lwgpio_toggle_value(&tmp);
        return TRUE;
    }
    return FALSE;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : LWGPIO_GET_PIN_INPUT
* Returned Value   : LWGPIO_VALUE, LWGPIO_VALUE_NOCHANGE if error
* Comments         :
*    Gets input pin state
*
*END*********************************************************************/
LWGPIO_VALUE lwgpio_get_pin_input(LWGPIO_PIN_ID id)
{
    LWGPIO_STRUCT tmp;
    if (lwgpio_init(&tmp, id, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE)) {
        return lwgpio_get_value(&tmp);
    }
    return LWGPIO_VALUE_NOCHANGE;
}
