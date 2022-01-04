/**HEADER********************************************************************
* 
* Copyright (c) 2010-2011 Freescale Semiconductor;
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
* $FileName: lwgpio.h$
* $Version : 3.8.8.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/
#ifndef __lwgpio_h__
#define __lwgpio_h__ 1

#include <mqx.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Type holding pin ID */
typedef uint_32 LWGPIO_PIN_ID;

/* Type holding pin direction */
typedef enum {
    LWGPIO_DIR_INPUT,
    LWGPIO_DIR_OUTPUT,
    /* special value; for function parameters it means that value should not be changed */
    LWGPIO_DIR_NOCHANGE
} LWGPIO_DIR; 

/* Type holding pin value */
typedef enum {
    LWGPIO_VALUE_LOW,
    LWGPIO_VALUE_HIGH,
    /* special value; for function parameters it means that value should not be changed */
    LWGPIO_VALUE_NOCHANGE
} LWGPIO_VALUE;

/* Type identifying pin interrupt mode */
#ifndef LWGPIO_INT_MODE_NONE
    #define LWGPIO_INT_MODE_NONE    (0x00)
#endif
#ifndef LWGPIO_INT_MODE_RISING
    #define LWGPIO_INT_MODE_RISING  (0x01)
#endif
#ifndef LWGPIO_INT_MODE_FALLING
    #define LWGPIO_INT_MODE_FALLING (0x02)
#endif
#ifndef LWGPIO_INT_MODE_HIGH
    #define LWGPIO_INT_MODE_HIGH    (0x04)
#endif
#ifndef LWGPIO_INT_MODE_LOW
    #define LWGPIO_INT_MODE_LOW     (0x08)
#endif

/* Pin attributes*/
#define LWGPIO_ATTR_PULL_UP         (1)
#define LWGPIO_ATTR_PULL_DOWN       (2)
#define LWGPIO_ATTR_OPEN_DRAIN      (3)

/*attributes value*/
#define LWGPIO_AVAL_ENABLE          (1)
#define LWGPIO_AVAL_DISABLE         (0)

typedef uchar LWGPIO_INT_MODE;

boolean lwgpio_init(LWGPIO_STRUCT_PTR, LWGPIO_PIN_ID, LWGPIO_DIR, LWGPIO_VALUE);
void lwgpio_set_functionality(LWGPIO_STRUCT_PTR, uint_32);
boolean lwgpio_set_attribute(LWGPIO_STRUCT_PTR, uint_32, uint_32);
uint_32 lwgpio_get_functionality(LWGPIO_STRUCT_PTR);
void lwgpio_set_direction(LWGPIO_STRUCT_PTR, LWGPIO_DIR);
void lwgpio_set_value(LWGPIO_STRUCT_PTR, LWGPIO_VALUE);
void lwgpio_toggle_value(LWGPIO_STRUCT_PTR);
LWGPIO_VALUE lwgpio_get_value(LWGPIO_STRUCT_PTR);
LWGPIO_VALUE lwgpio_get_raw(LWGPIO_STRUCT_PTR);

boolean lwgpio_int_init(LWGPIO_STRUCT_PTR, LWGPIO_INT_MODE);
uint_32 lwgpio_int_get_vector(LWGPIO_STRUCT_PTR);
void lwgpio_int_enable(LWGPIO_STRUCT_PTR, boolean);
boolean lwgpio_int_get_flag(LWGPIO_STRUCT_PTR);
void lwgpio_int_clear_flag(LWGPIO_STRUCT_PTR);

extern boolean lwgpio_set_pin_output(LWGPIO_PIN_ID, LWGPIO_VALUE);
extern boolean lwgpio_toggle_pin_output(LWGPIO_PIN_ID);
extern LWGPIO_VALUE lwgpio_get_pin_input(LWGPIO_PIN_ID);

#ifdef __cplusplus
}
#endif

#endif /* __lwgpio_h__ */
