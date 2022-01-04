#ifndef __io_gpio_cpu_h__
#define __io_gpio_cpu_h__
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
* $FileName: io_gpio_mcf5225x.h$
* $Version : 3.0.14.0$
* $Date    : May-21-2009$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct irq_map_struct {
   uint_8  portnq;
} GPIO_IRQ_MAP, _PTR_ GPIO_IRQ_MAP_PTR;

#define GPIO_PORT_TE ((0x0000 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TF ((0x0001 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TG ((0x0002 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TH ((0x0003 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TI ((0x0004 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TJ ((0x0006 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_NQ ((0x0008 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_AN ((0x000A << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_AS ((0x000B << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_QS ((0x000C << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TA ((0x000E << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_TC ((0x000F << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_UA ((0x0011 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_UB ((0x0012 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_UC ((0x0013 << 3) | GPIO_PIN_VALID)
#define GPIO_PORT_DD ((0x0014 << 3) | GPIO_PIN_VALID)

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
