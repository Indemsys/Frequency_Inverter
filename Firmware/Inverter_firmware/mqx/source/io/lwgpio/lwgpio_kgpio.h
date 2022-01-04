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
* $FileName: lwgpio_kgpio.h$
* $Version : 3.8.6.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/
#ifndef __lwgpio_kgpio_h__
#define __lwgpio_kgpio_h__ 1

#define LWGPIO_PIN_VALID (0x80000000) /* Note: this is for backwards compatibility with GPIO driver only */

/* Pin field definitions */
#define LWGPIO_PIN_SHIFT (0)
#define LWGPIO_PIN_MASK (0x1F << LWGPIO_PIN_SHIFT)

#define LWGPIO_PIN(x) (((x) << LWGPIO_PIN_SHIFT) & LWGPIO_PIN_MASK)

/* Pin attributes */
#define LWGPIO_ATTR_BASE            (0x100)
#define LWGPIO_ATTR_SLEW_RATE       (LWGPIO_ATTR_BASE+1)
#define LWGPIO_ATTR_FILTER          (LWGPIO_ATTR_BASE+2)
#define LWGPIO_ATTR_DRIVE_STRENGTH  (LWGPIO_ATTR_BASE+3)

/*attributes value*/
#define LWGPIO_AVAL_SLEW_RATE_FAST          (0)
#define LWGPIO_AVAL_SLEW_RATE_SLOW          (1)
#define LWGPIO_AVAL_DRIVE_STRENGTH_HIGH     (1)
#define LWGPIO_AVAL_DRIVE_STRENGTH_LOW      (0)

/* Port field definitions */
#define LWGPIO_PORT_SHIFT (5)
#define LWGPIO_PORT_MASK (0xFF << LWGPIO_PORT_SHIFT)
#define LWGPIO_PORT(x) (((x) << LWGPIO_PORT_SHIFT) & LWGPIO_PORT_MASK)
#define LWGPIO_PORT_A (LWGPIO_PORT(0) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_B (LWGPIO_PORT(1) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_C (LWGPIO_PORT(2) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_D (LWGPIO_PORT(3) | LWGPIO_PIN_VALID)
#define LWGPIO_PORT_E (LWGPIO_PORT(4) | LWGPIO_PIN_VALID)

#define LWGPIO_PIN0   (0)
#define LWGPIO_PIN1   (1)
#define LWGPIO_PIN2   (2)
#define LWGPIO_PIN3   (3)
#define LWGPIO_PIN4   (4)
#define LWGPIO_PIN5   (5)
#define LWGPIO_PIN6   (6)
#define LWGPIO_PIN7   (7)
#define LWGPIO_PIN8   (8)
#define LWGPIO_PIN9   (9)
#define LWGPIO_PIN10  (10)
#define LWGPIO_PIN11  (11)
#define LWGPIO_PIN12  (12)
#define LWGPIO_PIN13  (13)
#define LWGPIO_PIN14  (14)
#define LWGPIO_PIN15  (15)
#define LWGPIO_PIN16  (16)
#define LWGPIO_PIN17  (17)
#define LWGPIO_PIN18  (18)
#define LWGPIO_PIN19  (19)
#define LWGPIO_PIN20  (20)
#define LWGPIO_PIN21  (21)
#define LWGPIO_PIN22  (22)
#define LWGPIO_PIN23  (23)
#define LWGPIO_PIN24  (24)
#define LWGPIO_PIN25  (25)
#define LWGPIO_PIN26  (26)
#define LWGPIO_PIN27  (27)
#define LWGPIO_PIN28  (28)
#define LWGPIO_PIN29  (29)
#define LWGPIO_PIN30  (30)
#define LWGPIO_PIN31  (31)

typedef struct {
    uint_32_ptr    pcr_reg;
    GPIO_MemMapPtr gpio_ptr;
    uint_32 pinmask; /* since struct holds one pin, pinmask will have only one bit set */
    uint_32 flags;
} LWGPIO_STRUCT, _PTR_ LWGPIO_STRUCT_PTR;

#define LWGPIO_MUX_A0_GPIO  (1)
#define LWGPIO_MUX_A1_GPIO  (1)
#define LWGPIO_MUX_A2_GPIO  (1)
#define LWGPIO_MUX_A3_GPIO  (1)
#define LWGPIO_MUX_A4_GPIO  (1)
#define LWGPIO_MUX_A5_GPIO  (1)
#define LWGPIO_MUX_A6_GPIO  (1)
#define LWGPIO_MUX_A7_GPIO  (1)
#define LWGPIO_MUX_A8_GPIO  (1)
#define LWGPIO_MUX_A9_GPIO  (1)
#define LWGPIO_MUX_A10_GPIO (1)
#define LWGPIO_MUX_A11_GPIO (1)
#define LWGPIO_MUX_A12_GPIO (1)
#define LWGPIO_MUX_A13_GPIO (1)
#define LWGPIO_MUX_A14_GPIO (1)
#define LWGPIO_MUX_A15_GPIO (1)
#define LWGPIO_MUX_A16_GPIO (1)
#define LWGPIO_MUX_A17_GPIO (1)
#define LWGPIO_MUX_A18_GPIO (1)
#define LWGPIO_MUX_A19_GPIO (1)
#define LWGPIO_MUX_A20_GPIO (1)
#define LWGPIO_MUX_A21_GPIO (1)
#define LWGPIO_MUX_A22_GPIO (1)
#define LWGPIO_MUX_A23_GPIO (1)
#define LWGPIO_MUX_A24_GPIO (1)
#define LWGPIO_MUX_A25_GPIO (1)
#define LWGPIO_MUX_A26_GPIO (1)
#define LWGPIO_MUX_A27_GPIO (1)
#define LWGPIO_MUX_A28_GPIO (1)
#define LWGPIO_MUX_A29_GPIO (1)
#define LWGPIO_MUX_A30_GPIO (1)
#define LWGPIO_MUX_A31_GPIO (1)

#define LWGPIO_MUX_B0_GPIO  (1)
#define LWGPIO_MUX_B1_GPIO  (1)
#define LWGPIO_MUX_B2_GPIO  (1)
#define LWGPIO_MUX_B3_GPIO  (1)
#define LWGPIO_MUX_B4_GPIO  (1)
#define LWGPIO_MUX_B5_GPIO  (1)
#define LWGPIO_MUX_B6_GPIO  (1)
#define LWGPIO_MUX_B7_GPIO  (1)
#define LWGPIO_MUX_B8_GPIO  (1)
#define LWGPIO_MUX_B9_GPIO  (1)
#define LWGPIO_MUX_B10_GPIO (1)
#define LWGPIO_MUX_B11_GPIO (1)
#define LWGPIO_MUX_B12_GPIO (1)
#define LWGPIO_MUX_B13_GPIO (1)
#define LWGPIO_MUX_B14_GPIO (1)
#define LWGPIO_MUX_B15_GPIO (1)
#define LWGPIO_MUX_B16_GPIO (1)
#define LWGPIO_MUX_B17_GPIO (1)
#define LWGPIO_MUX_B18_GPIO (1)
#define LWGPIO_MUX_B19_GPIO (1)
#define LWGPIO_MUX_B20_GPIO (1)
#define LWGPIO_MUX_B21_GPIO (1)
#define LWGPIO_MUX_B22_GPIO (1)
#define LWGPIO_MUX_B23_GPIO (1)
#define LWGPIO_MUX_B24_GPIO (1)
#define LWGPIO_MUX_B25_GPIO (1)
#define LWGPIO_MUX_B26_GPIO (1)
#define LWGPIO_MUX_B27_GPIO (1)
#define LWGPIO_MUX_B28_GPIO (1)
#define LWGPIO_MUX_B29_GPIO (1)
#define LWGPIO_MUX_B30_GPIO (1)
#define LWGPIO_MUX_B31_GPIO (1)

#define LWGPIO_MUX_C0_GPIO  (1)
#define LWGPIO_MUX_C1_GPIO  (1)
#define LWGPIO_MUX_C2_GPIO  (1)
#define LWGPIO_MUX_C3_GPIO  (1)
#define LWGPIO_MUX_C4_GPIO  (1)
#define LWGPIO_MUX_C5_GPIO  (1)
#define LWGPIO_MUX_C6_GPIO  (1)
#define LWGPIO_MUX_C7_GPIO  (1)
#define LWGPIO_MUX_C8_GPIO  (1)
#define LWGPIO_MUX_C9_GPIO  (1)
#define LWGPIO_MUX_C10_GPIO (1)
#define LWGPIO_MUX_C11_GPIO (1)
#define LWGPIO_MUX_C12_GPIO (1)
#define LWGPIO_MUX_C13_GPIO (1)
#define LWGPIO_MUX_C14_GPIO (1)
#define LWGPIO_MUX_C15_GPIO (1)
#define LWGPIO_MUX_C16_GPIO (1)
#define LWGPIO_MUX_C17_GPIO (1)
#define LWGPIO_MUX_C18_GPIO (1)
#define LWGPIO_MUX_C19_GPIO (1)
#define LWGPIO_MUX_C20_GPIO (1)
#define LWGPIO_MUX_C21_GPIO (1)
#define LWGPIO_MUX_C22_GPIO (1)
#define LWGPIO_MUX_C23_GPIO (1)
#define LWGPIO_MUX_C24_GPIO (1)
#define LWGPIO_MUX_C25_GPIO (1)
#define LWGPIO_MUX_C26_GPIO (1)
#define LWGPIO_MUX_C27_GPIO (1)
#define LWGPIO_MUX_C28_GPIO (1)
#define LWGPIO_MUX_C29_GPIO (1)
#define LWGPIO_MUX_C30_GPIO (1)
#define LWGPIO_MUX_C31_GPIO (1)

#define LWGPIO_MUX_D0_GPIO  (1)
#define LWGPIO_MUX_D1_GPIO  (1)
#define LWGPIO_MUX_D2_GPIO  (1)
#define LWGPIO_MUX_D3_GPIO  (1)
#define LWGPIO_MUX_D4_GPIO  (1)
#define LWGPIO_MUX_D5_GPIO  (1)
#define LWGPIO_MUX_D6_GPIO  (1)
#define LWGPIO_MUX_D7_GPIO  (1)
#define LWGPIO_MUX_D8_GPIO  (1)
#define LWGPIO_MUX_D9_GPIO  (1)
#define LWGPIO_MUX_D10_GPIO (1)
#define LWGPIO_MUX_D11_GPIO (1)
#define LWGPIO_MUX_D12_GPIO (1)
#define LWGPIO_MUX_D13_GPIO (1)
#define LWGPIO_MUX_D14_GPIO (1)
#define LWGPIO_MUX_D15_GPIO (1)
#define LWGPIO_MUX_D16_GPIO (1)
#define LWGPIO_MUX_D17_GPIO (1)
#define LWGPIO_MUX_D18_GPIO (1)
#define LWGPIO_MUX_D19_GPIO (1)
#define LWGPIO_MUX_D20_GPIO (1)
#define LWGPIO_MUX_D21_GPIO (1)
#define LWGPIO_MUX_D22_GPIO (1)
#define LWGPIO_MUX_D23_GPIO (1)
#define LWGPIO_MUX_D24_GPIO (1)
#define LWGPIO_MUX_D25_GPIO (1)
#define LWGPIO_MUX_D26_GPIO (1)
#define LWGPIO_MUX_D27_GPIO (1)
#define LWGPIO_MUX_D28_GPIO (1)
#define LWGPIO_MUX_D29_GPIO (1)
#define LWGPIO_MUX_D30_GPIO (1)
#define LWGPIO_MUX_D31_GPIO (1)

#define LWGPIO_MUX_E0_GPIO  (1)
#define LWGPIO_MUX_E1_GPIO  (1)
#define LWGPIO_MUX_E2_GPIO  (1)
#define LWGPIO_MUX_E3_GPIO  (1)
#define LWGPIO_MUX_E4_GPIO  (1)
#define LWGPIO_MUX_E5_GPIO  (1)
#define LWGPIO_MUX_E6_GPIO  (1)
#define LWGPIO_MUX_E7_GPIO  (1)
#define LWGPIO_MUX_E8_GPIO  (1)
#define LWGPIO_MUX_E9_GPIO  (1)
#define LWGPIO_MUX_E10_GPIO (1)
#define LWGPIO_MUX_E11_GPIO (1)
#define LWGPIO_MUX_E12_GPIO (1)
#define LWGPIO_MUX_E13_GPIO (1)
#define LWGPIO_MUX_E14_GPIO (1)
#define LWGPIO_MUX_E15_GPIO (1)
#define LWGPIO_MUX_E16_GPIO (1)
#define LWGPIO_MUX_E17_GPIO (1)
#define LWGPIO_MUX_E18_GPIO (1)
#define LWGPIO_MUX_E19_GPIO (1)
#define LWGPIO_MUX_E20_GPIO (1)
#define LWGPIO_MUX_E21_GPIO (1)
#define LWGPIO_MUX_E22_GPIO (1)
#define LWGPIO_MUX_E23_GPIO (1)
#define LWGPIO_MUX_E24_GPIO (1)
#define LWGPIO_MUX_E25_GPIO (1)
#define LWGPIO_MUX_E26_GPIO (1)
#define LWGPIO_MUX_E27_GPIO (1)
#define LWGPIO_MUX_E28_GPIO (1)
#define LWGPIO_MUX_E29_GPIO (1)
#define LWGPIO_MUX_E30_GPIO (1)
#define LWGPIO_MUX_E31_GPIO (1)

#endif
