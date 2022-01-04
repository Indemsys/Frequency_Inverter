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
* $FileName: lpm_smc.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains Low Power Manager functionality specific to Kinetis.
*
*END************************************************************************/


#include "mqx.h"
#include "bsp.h"


#if MQX_ENABLE_LOW_POWER


#ifndef PE_LDD_VERSION


static const LPM_CPU_POWER_MODE LPM_CPU_POWER_MODES_KINETIS[LPM_CPU_POWER_MODES] =
{
    // Kinetis RUN
    {
        SMC_PMCTRL_LPWUI_MASK,                      // Mode PMCTRL register == voltage regulator ON after wakeup
        0,                                          // Mode flags == clear settings
    },
    // Kinetis WAIT
    {
        SMC_PMCTRL_LPWUI_MASK,                      // Mode PMCTRL register == voltage regulator ON after wakeup
        LPM_CPU_POWER_MODE_FLAG_USE_WFI,            // Mode flags == execute WFI
    },
    // Kinetis STOP
    {
        SMC_PMCTRL_LPWUI_MASK,                      // Mode PMCTRL register == voltage regulator ON after wakeup
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI,   // Mode flags == deepsleep, execute WFI
    },
    // Kinetis VLPR
    {
        SMC_PMCTRL_RUNM(2),                         // Mode PMCTRL register == VLPR
        0,                                          // Mode flags == clear settings
    },
    // Kinetis VLPW
    {
        SMC_PMCTRL_RUNM(2),                         // Mode PMCTRL register == VLPW
        LPM_CPU_POWER_MODE_FLAG_USE_WFI,            // Mode flags == execute WFI
    },
    // Kinetis VLPS
    {
        SMC_PMCTRL_STOPM(2),                                                    // Mode PMCTRL register == VLPS
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI,   // Mode flags == deepsleep, execute WFI
    },
    // Kinetis LLS
    {
    	SMC_PMCTRL_LPWUI_MASK | SMC_PMCTRL_STOPM(3), // Mode PMCTRL register == voltage regulator ON after wakeup, LLS
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI,   // Mode flags == deepsleep, execute WFI
    }
};


#else


static const LDD_TDriverOperationMode LPM_PE_OPERATION_MODE_MAP[LPM_OPERATION_MODES] =
{
    DOM_RUN,
    DOM_WAIT,
    DOM_SLEEP,
    DOM_STOP
};


#endif


/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_set_cpu_operation_mode
* Returned Value   : MQX error code
* Comments         :
*    This function changes operation mode of the CPU core.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _lpm_set_cpu_operation_mode
    (
        /* [IN] Specification of CPU core low power operation modes available */
        const LPM_CPU_OPERATION_MODE _PTR_ operation_modes,

        /* [IN] Low power operation mode identifier */
        LPM_OPERATION_MODE                 target_mode
    )
{

#ifndef PE_LDD_VERSION

    const LPM_CPU_POWER_MODE _PTR_         mode_ptr;
    _mqx_uint                              scr, flags, mcg, index, cme;

    /* Check parameters */
    if ((NULL == operation_modes) || (LPM_OPERATION_MODES <= (_mqx_uint)target_mode))
    {
        return MQX_INVALID_PARAMETER;
    }
    index = operation_modes[target_mode].MODE_INDEX;

    if (LPM_CPU_POWER_MODES <= index)
    {
        return MQX_INVALID_CONFIGURATION;
    }
    mode_ptr = &(LPM_CPU_POWER_MODES_KINETIS[index]);
    flags = mode_ptr->FLAGS | (operation_modes[target_mode].FLAGS & LPM_CPU_POWER_MODE_FLAG_USER_MASK);

    /* Go through Kinetis Run */
    scr = (SCB_SCR & (~ (SCB_SCR_SLEEPDEEP_MASK | SCB_SCR_SLEEPONEXIT_MASK)));
    SCB_SCR = scr;
    SMC_PMCTRL = LPM_CPU_POWER_MODES_KINETIS[LPM_CPU_POWER_MODE_RUN].PMCTRL;
    while (0 == (PMC_REGSC & PMC_REGSC_REGONS_MASK))
        { };
    while (SMC_PMSTAT_PMSTAT(1) != SMC_PMSTAT)
        { };

    /* Go to VLPW through VLPR */
    if (LPM_CPU_POWER_MODE_VLPW == index)
    {
        SMC_PMCTRL = LPM_CPU_POWER_MODES_KINETIS[LPM_CPU_POWER_MODE_VLPR].PMCTRL;
        while (SMC_PMSTAT_PMSTAT(4) != SMC_PMSTAT)
            { };
    }

    /* Setup ARM System control register */
    if (flags & LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP)
    {
        scr |= SCB_SCR_SLEEPDEEP_MASK;
    }
    if (flags & LPM_CPU_POWER_MODE_FLAG_SLEEP_ON_EXIT)
    {
        scr |= SCB_SCR_SLEEPONEXIT_MASK;
    }
    SCB_SCR = scr;

    /* Setup wakeup unit for LLS mode */
    if (LPM_CPU_POWER_MODE_LLS == index)
    {
        LLWU_PE1 = operation_modes[target_mode].PE1;
        LLWU_PE2 = operation_modes[target_mode].PE2;
        LLWU_PE3 = operation_modes[target_mode].PE3;
        LLWU_PE4 = operation_modes[target_mode].PE4;
        LLWU_ME = operation_modes[target_mode].ME;
        LLWU_F1 = 0xFF;
        LLWU_F2 = 0xFF;
        LLWU_F3 = 0xFF;
    }

    /* Keep status of MCG before mode change */
    mcg = MCG_S & MCG_S_CLKST_MASK;

    /* Disable CME if enabled before entering changing Power mode */
    cme = MCG_C6 & MCG_C6_CME0_MASK;            /* Save CME state */
    MCG_C6 &= ~(MCG_C6_CME0_MASK);              /* Clear CME */

    /* Operation mode setup */
    SMC_PMCTRL = mode_ptr->PMCTRL;

    /* Wait for proper setup of VLPR */
    if (LPM_CPU_POWER_MODE_VLPR == index)
    {
        while (SMC_PMSTAT_PMSTAT(4) != SMC_PMSTAT)
            { };
    }

    /* Go to sleep if required */
    if (flags & LPM_CPU_POWER_MODE_FLAG_USE_WFI)
    {
        _ASM_SLEEP();
    }

    /* After stop modes, reconfigure MCG if needed */
    if ( (LPM_CPU_POWER_MODE_STOP == index)
      || (LPM_CPU_POWER_MODE_VLPS == index)
      || (LPM_CPU_POWER_MODE_LLS == index) )
    {
        if ((MCG_S_CLKST(3) == mcg) && (MCG_S_CLKST(2) == (MCG_S & MCG_S_CLKST_MASK)))
        {
            MCG_C1 &= (~ (MCG_C1_CLKS_MASK | MCG_C1_IREFS_MASK));
            while (0 == (MCG_S & MCG_S_LOCK0_MASK))
                { };
        }
    }

    /* Restore CME */
    MCG_C6 |= cme;

    return MQX_OK;

#else

#ifdef Cpu_SetOperationMode_METHOD_ENABLED
    if (LPM_OPERATION_MODES <= (_mqx_uint)target_mode)
    {
        return MQX_INVALID_PARAMETER;
    }
    return Cpu_SetOperationMode (LPM_PE_OPERATION_MODE_MAP[target_mode], NULL, NULL);
#else
    #error Undefined method Cpu_SetOperationMode() in PE CPU component!
#endif

#endif

}

/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_wakeup_core
* Returned Value   : None
* Comments         :
*    This function must be called from ISR not to let the core to return to sleep again.
*
*END*----------------------------------------------------------------------*/

void _lpm_wakeup_core
    (
        void
    )
{
    SCB_SCR &= (~ (SCB_SCR_SLEEPONEXIT_MASK));
}


/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_idle_sleep_check
* Returned Value   : TRUE if idle sleep possible, FALSE otherwise
* Comments         :
*    This function checks whether cpu core can sleep during execution of idle task
*    in current power mode.
*
*END*----------------------------------------------------------------------*/

boolean _lpm_idle_sleep_check
    (
        void
    )
{
    _mqx_uint pmctrl, stop;

    pmctrl = SMC_PMCTRL;
    stop = SCB_SCR & SCB_SCR_SLEEPDEEP_MASK;

    /* Idle sleep is available only in normal RUN/WAIT and VLPR/VLPW with LPWUI disabled */
    if ((0 == stop) && (0 == (pmctrl & SMC_PMCTRL_STOPM_MASK)) && (! ((SMC_PMCTRL_LPWUI_MASK | SMC_PMCTRL_RUNM(2)) == (pmctrl & (SMC_PMCTRL_LPWUI_MASK | SMC_PMCTRL_RUNM_MASK)))))
    {
        return TRUE;
    }

    return FALSE;
}


#endif


/* EOF */
