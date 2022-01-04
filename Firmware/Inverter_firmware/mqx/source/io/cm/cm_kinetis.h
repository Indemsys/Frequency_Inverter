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
* $FileName: cm_kinetis.h$
* $Version : 3.8.2.0$
* $Date    : Aug-29-2011$
*
* Comments:
*   Clock manager Kinetis specific definitions.
*
*END************************************************************************/

#ifndef __cm_kinetis_h__
    #define __cm_kinetis_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PE_LDD_VERSION
    
typedef struct  {
    uint32_t cpu_core_clk_hz;            /* Core clock frequency in clock configuration */       
    uint32_t cpu_bus_clk_hz;             /* Bus clock frequency in clock configuration */
    uint32_t cpu_flexbus_clk_hz;         /* Flexbus clock frequency in clock configuration */
    uint32_t cpu_flash_clk_hz;           /* FLASH clock frequency in clock configuration */
    uint32_t cpu_usb_clk_hz;             /* USB clock frequency in clock configuration */
    uint32_t cpu_pll_fll_clk_hz;         /* PLL/FLL clock frequency in clock configuration */
    uint32_t cpu_mcgir_clk_hz;           /* MCG internal reference clock frequency in clock configuration */
    uint32_t cpu_oscer_clk_hz;           /* System OSC external reference clock frequency in clock configuration */
    uint32_t cpu_erclk32k_clk_hz;        /* External reference clock 32k frequency in clock configuration */
    uint32_t cpu_mcgff_clk_hz;           /* MCG fixed frequency clock */
} TCpuClockConfiguration;
    
#endif  /* PE_LDD_VERSION */

typedef enum  {
    CM_CLOCK_SOURCE_CORE = 0,
    CM_CLOCK_SOURCE_BUS,
    CM_CLOCK_SOURCE_FLEXBUS,
    CM_CLOCK_SOURCE_FLASH,
    CM_CLOCK_SOURCE_USB,
    CM_CLOCK_SOURCE_PLLFLL,
    CM_CLOCK_SOURCE_MCGIR,
    CM_CLOCK_SOURCE_OSCER,
    CM_CLOCK_SOURCE_ERCLK32K,
    CM_CLOCK_SOURCE_MCGFF,
    CM_CLOCK_SOURCES,            /* Number of clock sources available */
    CM_CLOCK_SOURCE_SYSTEM = CM_CLOCK_SOURCE_CORE
} CM_CLOCK_SOURCE;

#ifdef __cplusplus
}
#endif


#endif /* __cm_kinetis_h__ */
