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
* $FileName: lwgpio_mcf5225.h$
* $Version : 3.8.9.1$
* $Date    : Aug-1-2012$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/
#ifndef __lwgpio_mcf5225_h__
#define __lwgpio_mcf5225_h__ 1

#define LWGPIO_PIN_VALID (0x80000000) /* Note: this is for backwards compatibility with GPIO driver only */

/* Pin field definitions */
#define LWGPIO_PIN_SHIFT (0)
#define LWGPIO_PIN_MASK (0x7 << LWGPIO_PIN_SHIFT)

#define LWGPIO_PIN(x) (((x) << LWGPIO_PIN_SHIFT) & LWGPIO_PIN_MASK)

/* Pin attributes */
#define LWGPIO_ATTR_BASE            (0x100)
#define LWGPIO_ATTR_SLEW_RATE       (LWGPIO_ATTR_BASE+1)
#define LWGPIO_ATTR_DRIVE_STRENGTH  (LWGPIO_ATTR_BASE+2)

/*attributes value*/
#define LWGPIO_AVAL_SLEW_RATE_FAST        (0)
#define LWGPIO_AVAL_SLEW_RATE_SLOW        (1)
#define LWGPIO_AVAL_DRIVE_STRENGTH_HIGH   (1)
#define LWGPIO_AVAL_DRIVE_STRENGTH_LOW    (0)

/* Port field definitions */
#define LWGPIO_PORT_SHIFT (3)
#define LWGPIO_PORT_MASK (0xFF << LWGPIO_PORT_SHIFT)
#define LWGPIO_PORT(x) (((x) << LWGPIO_PORT_SHIFT) & LWGPIO_PORT_MASK)
#define LWGPIO_PORT_TE (LWGPIO_PORT(0) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TF (LWGPIO_PORT(1) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TG (LWGPIO_PORT(2) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TH (LWGPIO_PORT(3) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TI (LWGPIO_PORT(4) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TJ (LWGPIO_PORT(6) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_NQ (LWGPIO_PORT(8) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_AN (LWGPIO_PORT(10) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_AS (LWGPIO_PORT(11) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_QS (LWGPIO_PORT(12) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TA (LWGPIO_PORT(14) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_TC (LWGPIO_PORT(15) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_UA (LWGPIO_PORT(17) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_UB (LWGPIO_PORT(18) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_UC (LWGPIO_PORT(19) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_DD (LWGPIO_PORT(20) | LWGPIO_PIN_VALID)

#define LWGPIO_PIN0   (0)
#define LWGPIO_PIN1   (1)
#define LWGPIO_PIN2   (2)
#define LWGPIO_PIN3   (3)
#define LWGPIO_PIN4   (4)
#define LWGPIO_PIN5   (5)
#define LWGPIO_PIN6   (6)
#define LWGPIO_PIN7   (7)

typedef struct {
    uint_8  port_idx;
    uint_8  pinmask; /* since struct holds one pin, pinmask will have only one bit set */
    uint_32 flags;
} LWGPIO_STRUCT, _PTR_ LWGPIO_STRUCT_PTR;

#define LWGPIO_MUX_DD0_GPIO  (0)
#define LWGPIO_MUX_DD1_GPIO  (0)
#define LWGPIO_MUX_DD2_GPIO  (0)
#define LWGPIO_MUX_DD3_GPIO  (0)
#define LWGPIO_MUX_DD4_GPIO  (0)
#define LWGPIO_MUX_DD5_GPIO  (0)
#define LWGPIO_MUX_DD6_GPIO  (0)
#define LWGPIO_MUX_DD7_GPIO  (0)

#define LWGPIO_MUX_AN0_GPIO  (0)
#define LWGPIO_MUX_AN1_GPIO  (0)
#define LWGPIO_MUX_AN2_GPIO  (0)
#define LWGPIO_MUX_AN3_GPIO  (0)
#define LWGPIO_MUX_AN4_GPIO  (0)
#define LWGPIO_MUX_AN5_GPIO  (0)
#define LWGPIO_MUX_AN6_GPIO  (0)
#define LWGPIO_MUX_AN7_GPIO  (0)

#define LWGPIO_MUX_TE0_GPIO  (0)
#define LWGPIO_MUX_TE1_GPIO  (0)
#define LWGPIO_MUX_TE2_GPIO  (0)
#define LWGPIO_MUX_TE3_GPIO  (0)
#define LWGPIO_MUX_TE4_GPIO  (0)
#define LWGPIO_MUX_TE5_GPIO  (0)
#define LWGPIO_MUX_TE6_GPIO  (0)
#define LWGPIO_MUX_TE7_GPIO  (0)

#define LWGPIO_MUX_TF0_GPIO  (0)
#define LWGPIO_MUX_TF1_GPIO  (0)
#define LWGPIO_MUX_TF2_GPIO  (0)
#define LWGPIO_MUX_TF3_GPIO  (0)
#define LWGPIO_MUX_TF4_GPIO  (0)
#define LWGPIO_MUX_TF5_GPIO  (0)
#define LWGPIO_MUX_TF6_GPIO  (0)
#define LWGPIO_MUX_TF7_GPIO  (0)

#define LWGPIO_MUX_TG0_GPIO  (0)
#define LWGPIO_MUX_TG1_GPIO  (0)
#define LWGPIO_MUX_TG2_GPIO  (0)
#define LWGPIO_MUX_TG3_GPIO  (0)
#define LWGPIO_MUX_TG4_GPIO  (0)
#define LWGPIO_MUX_TG5_GPIO  (0)
#define LWGPIO_MUX_TG6_GPIO  (0)
#define LWGPIO_MUX_TG7_GPIO  (0)

#define LWGPIO_MUX_TI0_GPIO  (0)
#define LWGPIO_MUX_TI1_GPIO  (0)
#define LWGPIO_MUX_TI2_GPIO  (0)
#define LWGPIO_MUX_TI3_GPIO  (0)
#define LWGPIO_MUX_TI4_GPIO  (0)
#define LWGPIO_MUX_TI5_GPIO  (0)
#define LWGPIO_MUX_TI6_GPIO  (0)
#define LWGPIO_MUX_TI7_GPIO  (0)

#define LWGPIO_MUX_TJ0_GPIO  (0)
#define LWGPIO_MUX_TJ1_GPIO  (0)
#define LWGPIO_MUX_TJ2_GPIO  (0)
#define LWGPIO_MUX_TJ3_GPIO  (0)
#define LWGPIO_MUX_TJ4_GPIO  (0)
#define LWGPIO_MUX_TJ5_GPIO  (0)
#define LWGPIO_MUX_TJ6_GPIO  (0)
#define LWGPIO_MUX_TJ7_GPIO  (0)

#define LWGPIO_MUX_UC0_GPIO  (0)
#define LWGPIO_MUX_UC1_GPIO  (0)
#define LWGPIO_MUX_UC2_GPIO  (0)
#define LWGPIO_MUX_UC3_GPIO  (0)

#define LWGPIO_MUX_QS0_GPIO  (0)
#define LWGPIO_MUX_QS1_GPIO  (0)
#define LWGPIO_MUX_QS2_GPIO  (0)
#define LWGPIO_MUX_QS3_GPIO  (0)
#define LWGPIO_MUX_QS5_GPIO  (0)
#define LWGPIO_MUX_QS6_GPIO  (0)

#define LWGPIO_MUX_TH0_GPIO  (0)
#define LWGPIO_MUX_TH1_GPIO  (0)
#define LWGPIO_MUX_TH2_GPIO  (0)
#define LWGPIO_MUX_TH3_GPIO  (0)
#define LWGPIO_MUX_TH4_GPIO  (0)
#define LWGPIO_MUX_TH5_GPIO  (0)
#define LWGPIO_MUX_TH6_GPIO  (0)
#define LWGPIO_MUX_TH7_GPIO  (0)

#define LWGPIO_MUX_AS0_GPIO  (0)
#define LWGPIO_MUX_AS1_GPIO  (0)
#define LWGPIO_MUX_AS2_GPIO  (0)

#define LWGPIO_MUX_TA0_GPIO  (0)
#define LWGPIO_MUX_TA1_GPIO  (0)
#define LWGPIO_MUX_TA2_GPIO  (0)
#define LWGPIO_MUX_TA3_GPIO  (0)

#define LWGPIO_MUX_TC0_GPIO  (0)
#define LWGPIO_MUX_TC1_GPIO  (0)
#define LWGPIO_MUX_TC2_GPIO  (0)
#define LWGPIO_MUX_TC3_GPIO  (0)

#define LWGPIO_MUX_UA0_GPIO  (0)
#define LWGPIO_MUX_UA1_GPIO  (0)
#define LWGPIO_MUX_UA2_GPIO  (0)
#define LWGPIO_MUX_UA3_GPIO  (0)

#define LWGPIO_MUX_UB0_GPIO  (0)
#define LWGPIO_MUX_UB1_GPIO  (0)
#define LWGPIO_MUX_UB2_GPIO  (0)
#define LWGPIO_MUX_UB3_GPIO  (0)

#define LWGPIO_MUX_UC0_GPIO  (0)
#define LWGPIO_MUX_UC1_GPIO  (0)
#define LWGPIO_MUX_UC2_GPIO  (0)
#define LWGPIO_MUX_UC3_GPIO  (0)

#define LWGPIO_MUX_NQ1_GPIO  (0)
#define LWGPIO_MUX_NQ3_GPIO  (0)
#define LWGPIO_MUX_NQ5_GPIO  (0)
#define LWGPIO_MUX_NQ7_GPIO  (0)

#endif
