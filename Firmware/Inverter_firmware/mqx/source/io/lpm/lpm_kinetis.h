/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************** 
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
*****************************************************************************
*
* $FileName: lpm_kinetis.h$
* $Version : 3.8.5.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Low Power Manager declarations specific to Kinetis.
*
*END************************************************************************/


#ifndef __lpm_kinetis_h__
#define __lpm_kinetis_h__

/*-------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

typedef enum 
{
    LPM_CPU_POWER_MODE_RUN = 0,
    LPM_CPU_POWER_MODE_WAIT,
    LPM_CPU_POWER_MODE_STOP,
    LPM_CPU_POWER_MODE_VLPR,
    LPM_CPU_POWER_MODE_VLPW,
    LPM_CPU_POWER_MODE_VLPS,
    LPM_CPU_POWER_MODE_LLS,
    LPM_CPU_POWER_MODES
} LPM_CPU_POWER_MODE_INDEX;

#define LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP          (0x01)
#define LPM_CPU_POWER_MODE_FLAG_USE_WFI             (0x02)
#define LPM_CPU_POWER_MODE_FLAG_SLEEP_ON_EXIT       (0x04)
#define LPM_CPU_POWER_MODE_FLAG_USER_MASK           (LPM_CPU_POWER_MODE_FLAG_SLEEP_ON_EXIT)

#ifndef SMC_PMCTRL_LPWUI_MASK
#define SMC_PMCTRL_LPWUI_MASK                       (0x80)
#endif

/*-------------------------------------------------------------------------*/
/*
**                            MACRO DECLARATIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

typedef struct lpm_cpu_power_mode {
    /* Mode control register setup */
    uint_8      PMCTRL;

    /* Flags specifying low power mode behavior */
    uint_8      FLAGS;

} LPM_CPU_POWER_MODE, _PTR_ LPM_CPU_POWER_MODE_PTR;


typedef struct lpm_cpu_operation_mode {
    /* Index into predefined cpu operation modes */
    LPM_CPU_POWER_MODE_INDEX    MODE_INDEX;

    /* Additional modification flags */
    uint_8                      FLAGS;

    /* LLWU specific settings */
    uint_8                      PE1;
    uint_8                      PE2;
    uint_8                      PE3;
    uint_8                      PE4;
    uint_8                      ME;
    
} LPM_CPU_OPERATION_MODE, _PTR_ LPM_CPU_OPERATION_MODE_PTR;


/*-------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

    
extern _mqx_uint _lpm_set_cpu_operation_mode (const LPM_CPU_OPERATION_MODE _PTR_, LPM_OPERATION_MODE);
extern void      _lpm_wakeup_core (void);
extern _mqx_uint _lpm_idle_sleep_check (void);


#ifdef __cplusplus
}
#endif


#endif

/* EOF */
