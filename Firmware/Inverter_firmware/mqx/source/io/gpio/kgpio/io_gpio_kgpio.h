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
* $FileName: io_gpio_kgpio.h$
* $Version : 3.8.3.0$
* $Date    : Sep-22-2011$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/

#ifndef __io_gpio_cpu_h__
#define __io_gpio_cpu_h__

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


#define GPIO_PORT_A ((0x0000 << GPIO_PIN_PORT_SHIFT) | GPIO_PIN_VALID)
#define GPIO_PORT_B ((0x0001 << GPIO_PIN_PORT_SHIFT) | GPIO_PIN_VALID)
#define GPIO_PORT_C ((0x0002 << GPIO_PIN_PORT_SHIFT) | GPIO_PIN_VALID)
#define GPIO_PORT_D ((0x0003 << GPIO_PIN_PORT_SHIFT) | GPIO_PIN_VALID)
#define GPIO_PORT_E ((0x0004 << GPIO_PIN_PORT_SHIFT) | GPIO_PIN_VALID)
#ifdef PORTF_BASE_PTR 
	#define GPIO_PORT_F	((0x0005 << GPIO_PIN_PORT_SHIFT) | GPIO_PIN_VALID)
#endif
/*----------------------------------------------------------------------*/
/*
**                          TYPE DEFINITIONS
*/


typedef struct irq_map_struct {
   uint_8  portnq;
} GPIO_IRQ_MAP, _PTR_ GPIO_IRQ_MAP_PTR;



#ifdef __cplusplus
}
#endif

#endif

/* EOF */
