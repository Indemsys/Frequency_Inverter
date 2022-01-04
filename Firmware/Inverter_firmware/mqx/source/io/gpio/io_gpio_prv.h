#ifndef __io_gpio_driver_h__
#define __io_gpio_driver_h__
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
* $FileName: io_gpio_prv.h$
* $Version : 3.8.37.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the gpio driver.
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/* Internal functions to IO_GPIO */
#ifdef __cplusplus
extern "C" {
#endif

#if MQX_CPU == PSP_CPU_MCF5282
  #include "io_gpio_mcf5235_prv.h"
#elif PSP_MQX_CPU_IS_MCF5225X
  #include "io_gpio_mcf5225_prv.h"
#elif PSP_MQX_CPU_IS_MCF5227X
  #include "io_gpio_mcf5227_prv.h"
#elif PSP_MQX_CPU_IS_MCF51AC
  #include "io_gpio_mcf51ac_prv.h"   
#elif PSP_MQX_CPU_IS_MCF51JM
  #include "io_gpio_mcf51jm_prv.h"  
#elif PSP_MQX_CPU_IS_MCF51AG
  #include "io_gpio_mcf51ag_prv.h"  
#elif PSP_MQX_CPU_IS_MCF51CN
  #include "io_gpio_mcf51cn_prv.h"
#elif PSP_MQX_CPU_IS_MCF51EM
  #include "io_gpio_mcf51em_prv.h"
#elif PSP_MQX_CPU_IS_MCF51MM
  #include "io_gpio_mcf51mm_prv.h" 
#elif PSP_MQX_CPU_IS_MCF51JE
  #include "io_gpio_mcf51je_prv.h" 
#elif PSP_MQX_CPU_IS_MCF5222X
  #include "io_gpio_mcf5222_prv.h"
#elif PSP_MQX_CPU_IS_MCF5223X
  #include "io_gpio_mcf5223_prv.h"
#elif PSP_MQX_CPU_IS_MCF5441X
  #include "io_gpio_mcf5441_prv.h"
#elif PSP_MQX_CPU_IS_MCF5301X
  #include "io_gpio_mcf5301_prv.h"
#elif PSP_MQX_CPU_IS_MCF532X
  #include "io_gpio_mcf532_prv.h"
#elif PSP_MQX_CPU_IS_MCF520X
  #include "io_gpio_mcf520_prv.h"
#elif PSP_MQX_CPU_IS_KINETIS
  #include "io_gpio_kgpio_prv.h"  
#elif MQX_CPU==PSP_CPU_MPC8306
  #include "io_gpio_mpc8306_prv.h"
#elif MQX_CPU==PSP_CPU_MPC8308
  #include "io_gpio_mpc8308_prv.h"
#elif MQX_CPU==PSP_CPU_MPC8309
  #include "io_gpio_mpc8309_prv.h"
#elif PSP_MQX_CPU_IS_MCF51FD
  #include "io_gpio_mcf51fd_prv.h"
#elif PSP_MQX_CPU_IS_MCF51JF
  #include "io_gpio_mcf51jf_prv.h"
#elif PSP_MQX_CPU_IS_MCF51QM
  #include "io_gpio_mcf51qm_prv.h"
	#else
  #error IO_GPIO device driver not supported for processor.
#endif

typedef struct gpio_dev_data {
    struct gpio_dev_data _PTR_ NEXT;             /* this is used only to link IRQ maps */
    DEVICE_TYPE type;
    IRQ_FUNC         irq_func;
    GPIO_PIN_MAP     pin_map;
    GPIO_PIN_MAP     irqp_map;
    GPIO_PIN_MAP     irqn_map;
#if (defined PSP_MQX_CPU_IS_PIONEER2) || PSP_MQX_CPU_IS_KINETIS  
    GPIO_PIN_MAP     irql_map;
#endif    
} GPIO_DEV_DATA, _PTR_ GPIO_DEV_DATA_PTR;

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
