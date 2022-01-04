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
* $FileName: adt_pit_mcf522xx.c$
* $Version : 3.6.7.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the ADC driver CPU specific functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <fio_prv.h>
#include "io.h"
#include "adc.h"
#include "adc_prv.h"
#include "adc_conf.h"
#include "adc_mcf522xx_prv.h"

#if BSPCFG_ENABLE_ADC

/* Peripherals pointer */
static VMCF522XX_PIT_STRUCT_PTR  mcf522xx_timer_ptr;

/* State values for timer */
static struct 
{
    ADC_DRIVER_BUNDLE_PTR adc_d_bundle;
    uint_32  running_mask;
} adt_internal;

/* static variables storing information for deinitialization */
static void (_CODE_PTR_ old_isr_timer)(pointer);
static pointer          old_isr_timer_param;


/* Declarations of static functions */
static void _adt_scheduler(_mqx_uint time);
static void _adt_hw_set_wait(uint_32 time);
static uint_32 _adt_hw_get_time();
static _mqx_int _adt_hw_run();
static _mqx_int _adt_hw_stop();

/* Declarations of IRQ handlers */
static void adt_setup_timer(uint_32);
static void adt_cpu_isr(pointer);

/* Let's say that we want to set preset and prescaler value.
** The 32b. value representing [ prescaler | preset ] represents some time.
** If the 32b. value is 2^i, then the time is:
** 2^i / BSP_SYSTEM_CLOCK / 10^-6 [usec] =
** (1 << i) * 10^6 / BSP_SYSTEM_CLOCK [usec] =
** (10^6 << i) / BSP_SYSTEM_CLOCK [usec] =
** (0xF4240 << i) / BSP_SYSTEM_CLOCK [usec] = 
** (0x3D09 << (6 + i)) / BSP_SYSTEM_CLOCK [usec] =
** ((0x3D09 << (6 + i + 12 + 32)) / BSP_SYSTEM_CLOCK) >> (12 + 32) [usec] =
** ((0x3D09 << (6 + 12 + 32)) / BSP_SYSTEM_CLOCK) >> (12 + 32 - i) [usec] =
** TIMER_CONST >> (12 + 32 - i)
*/
#define TIMER_CONST (uint_64)(0xF424000000000000 / (BSP_SYSTEM_CLOCK / 2))

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/
_mqx_int _adt_hw_ioctl(MQX_FILE_PTR f, _mqx_uint cmd, pointer param_ptr)
{
    return IO_ERROR_INVALID_IOCTL_CMD;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_install
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adt_hw_install(char_ptr drivername, ADC_DRIVER_BUNDLE_PTR adc_d_bundle, pointer init_struct)
{
    adt_internal.adc_d_bundle = adc_d_bundle;
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_init
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific initialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adt_hw_init(MQX_FILE_PTR f, ADT_PTR adt)
{ /* Body */
#if PSP_MQX_CPU_IS_MCF5225X
    VMCF5225_STRUCT_PTR mcf522xx_ptr;
#elif PSP_MQX_CPU_IS_MCF5223X
    VMCF5223_STRUCT_PTR mcf522xx_ptr;
#elif PSP_MQX_CPU_IS_MCF5222X
    VMCF5222_STRUCT_PTR mcf522xx_ptr;
#else
    #error This CPU is not supported here
#endif
    
    _mqx_uint        div_register;
    _mqx_int         i;
    uint_32          frq_system_kHz;
    uint_16          prescaler, preset;
    uint_32          result;

    /* Prepare pointers to peripherals */
    mcf522xx_ptr = _PSP_GET_IPSBAR();
    mcf522xx_timer_ptr = &mcf522xx_ptr->PIT[BSP_ADC_TIMER];

    mcf522xx_timer_ptr->PCSR = 0x0038;
   
    old_isr_timer_param = _int_get_isr_data(BSP_ADC_TIMER_INTERRUPT_VECTOR);
    old_isr_timer = _int_install_isr(BSP_ADC_TIMER_INTERRUPT_VECTOR,
        adt_cpu_isr, NULL);
    if (NULL == old_isr_timer) {
        _int_install_isr(BSP_ADC_TIMER_INTERRUPT_VECTOR, old_isr_timer, old_isr_timer_param);
        return ADC_ERROR_ISR; /* could not install new interrupt routine */
    }

    /* We have two timer scales:
    ** the first one is generic driver time (32 bit)
    ** the second one is low level driver time (31 bit) and presents directly preset + prescaler settings
    ** and the relation between them is:
    ** Eq.1: generic_time = lowlevel_time << shift
    **
    ** From above, following implies:
    ** 1) lowlevel_time (31 bit) cannot fit all the scale range of generic_time
    ** (and to solve that, generic_time wait will be achieved by sequential lowlevel_time periods
    ** 2) generic_time has the smallest resolution of "shift" bits. Shift should be found to be
    ** less or equal to 1usec.
    ** 3) all generic_time(s) from init structures must be recomputed to match Eq.1
    **/
    result = (uint_32)(TIMER_CONST >> (12 + 32 - 31));
    for (i = 16 + 15; i > 0; i--) { /* preset_value = 0xFFFF, preset = 0x10000, prescaler = 1 << n, i = n + (log2 (preset)) */
        if (result == 0) { /* we reached the lowest resolution (lower than 1 usec) */
            /* store the exponent of 2 between 2 timer scales */
            adt->shift = i;
            break;
        }
        result = result >> 1;
    }

#if PSP_MQX_CPU_IS_MCF5225X
    /* Setup ISR levels for timer */
    _mcf5225_int_init(BSP_ADC_TIMER_INTERRUPT_VECTOR, BSP_ADC_TIMER_INTERRUPT_LEVEL, BSP_ADC_TIMER_INTERRUPT_SUBLEVEL, TRUE);
#elif PSP_MQX_CPU_IS_MCF5223X
    /* Setup ISR levels for timer */
    _mcf5223_int_init(BSP_ADC_TIMER_INTERRUPT_VECTOR, BSP_ADC_TIMER_INTERRUPT_LEVEL, BSP_ADC_TIMER_INTERRUPT_SUBLEVEL, TRUE);
#elif PSP_MQX_CPU_IS_MCF5222X
    /* Setup ISR levels for timer */
    _mcf5222_int_init(BSP_ADC_TIMER_INTERRUPT_VECTOR, BSP_ADC_TIMER_INTERRUPT_LEVEL, BSP_ADC_TIMER_INTERRUPT_SUBLEVEL, TRUE);
#else
        #error This CPU is not supported here
#endif
    
    return ADC_OK;
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC driver
*
*END*********************************************************************/
_mqx_int _adt_hw_deinit(MQX_FILE_PTR f, ADT_PTR adt)
{
    mcf522xx_timer_ptr->PCSR &= 0x007e; /* stop timer */
    if (old_isr_timer != NULL)
        _int_install_isr(BSP_ADC_TIMER_INTERRUPT_VECTOR, old_isr_timer, old_isr_timer_param);
    
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_channel_init
* Returned Value   : int_32
* Comments         :
*    Returns result of HW channel initialization
*
*END*********************************************************************/
_mqx_int _adt_hw_channel_init(MQX_FILE_PTR f, ADT_CHANNEL_PTR adt_ch)
{ /* Body */
    _mqx_uint i, j = 0;

    /* we need to recompute timers (cross-multiplication):
    ** source (init value)                                                dest (working value)
    ** ---------------------------------------------------------------------------------------
    ** ((0x3D09 << (6 + 12 + 32)) / BSP_SYSTEM_CLOCK) >> (12 + 32 - i)    2^i
    ** ((0x3D09 << (6 + 12 + 32)) / BSP_SYSTEM_CLOCK) >> (12 + 32 - 31)   2^31
    ** input [usec]                                                       output
    **
    ** output = input * (TIMER_CONST >> (12 + 32 - 31)) * 2^31 = 
    */
    adt_ch->g.period = (((uint_64) adt_ch->g.period << 31) / (TIMER_CONST >> (12 + 32 - 31))) >> adt_internal.adc_d_bundle->adt->shift; /* note: we are losing bits for timer setting */;
    adt_ch->g.offset = (((uint_64) adt_ch->g.offset << 31) / (TIMER_CONST >> (12 + 32 - 31))) >> adt_internal.adc_d_bundle->adt->shift; /* note: we are losing bits for timer setting */;

    return ADC_OK;
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_channel_deinit
* Returned Value   : _mqx_int
* Comments         :
*    Performs CPU specific deinitialization for ADC channel
*
*END*********************************************************************/
_mqx_int _adt_hw_channel_deinit(MQX_FILE_PTR f, ADT_CHANNEL_PTR adt_ch)
{
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_scheduler
* Returned Value   : 
* Comments         : Schedules next ADC conversion and informs low-level
*                    driver about that time. Low-level driver must call
*                    this scheduler in that time.
*                    This function can run in interrupt (depends on
                     SW timer implementation)
*END*********************************************************************/
static void _adt_scheduler(_mqx_uint time)
{ /* gets actual time as parameter */
    ADT_CHANNEL_PTR *adt_ch = adt_internal.adc_d_bundle->adt_ch;
    ADC_CHANNEL_PTR *adc_ch = adt_internal.adc_d_bundle->adc_ch;
    _mqx_int i;
    _mqx_uint wait;
    _mqx_int rem;

    if (adt_internal.running_mask) { /* if there is any channel in the mask for measurement */
        _adc_hw_measure(adt_internal.running_mask); /* measure channels */

        for (i = 0; i < ADC_MAX_CHANNELS; i++) { /* update time of last measurement in channels */
            if (!adt_ch[i] || !(adt_internal.running_mask & (1 << adc_ch[i]->g.source)))
                continue;
            
            adt_ch[i]->g.time = time;
        }
    }
    
    /* look for closest AD conversion */
    for (i = 0, wait = -1, adt_internal.running_mask = 0; i < ADC_MAX_CHANNELS; i++) {
        if (adt_ch[i] && adt_ch[i]->g.runtime_flags & ADC_CHANNEL_RUNNING) {
            /* compute remaining time to next measurement:
            ** elapsed time from last measurement: time - adc_ch[i]->time
            ** remaining time to next measurement: adc_ch[i]->period - elapsed
            */
            rem = adt_ch[i]->g.period - (time - adt_ch[i]->g.time);
        
            if (rem < 0)
                rem = 0;
        
            if (rem < wait) {
                wait = rem;
                adt_internal.running_mask = (1 << adc_ch[i]->g.source);
            }
            else if (rem == wait) {
                adt_internal.running_mask |= (1 << adc_ch[i]->g.source);
            }
        }
    }
    
    /* test if any conversion should be performed next time */
    if (adt_internal.running_mask) {
        if (!adt_internal.adc_d_bundle->adt->g.run) {
            _adt_hw_run();
            adt_internal.adc_d_bundle->adt->g.run = 1;
        }
    }
    else {
        _adt_hw_stop();
        adt_internal.adc_d_bundle->adt->g.run = 0;
    }

    /* inform low level driver that next dispatch call should be done in 'wait' time */
    _adt_hw_set_wait(wait);
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_trigger
* Returned Value   : IO_OK
* Comments         : Starts channel measurement or more channels defined
*                    by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_trigger(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADT_CHANNEL_PTR *adt_ch = adt_internal.adc_d_bundle->adt_ch;
    _mqx_int i;
    _mqx_uint time;

    _int_disable();
    time = _adt_hw_get_time();
    
    if (channel) {
        channel->time = time + channel->offset;
        channel->samples = channel->samples_preset;
        channel->runtime_flags |= ADC_CHANNEL_RUNNING | ADC_CHANNEL_RESUMED;
    }
    else {
        for (i = 0; i < ADC_MAX_CHANNELS; i++) {
            if (adt_ch[i] && (adt_ch[i]->g.trigger & mask)) {
                adt_ch[i]->g.time = time + adt_ch[i]->g.offset;
                /* period was subtracted because next scheduling will add it */
                adt_ch[i]->g.samples = adt_ch[i]->g.samples_preset;
                adt_ch[i]->g.runtime_flags |= ADC_CHANNEL_RUNNING | ADC_CHANNEL_RESUMED;
            }
        }
    }
    
    adt_internal.running_mask = 0;
    _adt_scheduler(time);

    _int_enable();
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_pause
* Returned Value   : IO_OK
* Comments         : Pauses measurement of single channel, or more channels
*                    defined by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_pause(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADT_CHANNEL_PTR *adt_ch = adt_internal.adc_d_bundle->adt_ch;
    _mqx_int i;
    _mqx_uint time;

    _int_disable();

    if (channel) {
        channel->runtime_flags &= ~ADC_CHANNEL_RESUMED;
    }
    else {
        for (i = 0; i < ADC_MAX_CHANNELS; i++) {
            if (adt_ch[i] && (adt_ch[i]->g.trigger & mask)) {
                adt_ch[i]->g.runtime_flags &= ~ADC_CHANNEL_RESUMED;
            }
        }
    }

    _int_enable();
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_resume
* Returned Value   : IO_OK
* Comments         : Resumes measurement of single paused channel, or more
*                    channels defined by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_resume(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADT_CHANNEL_PTR *adt_ch = adt_internal.adc_d_bundle->adt_ch;
    _mqx_int i;

    _int_disable();

    if (channel) {
        channel->runtime_flags |= ADC_CHANNEL_RESUMED;
    }
    else {
        for (i = 0; i < ADC_MAX_CHANNELS; i++) {
            if (adt_ch[i] && (adt_ch[i]->g.trigger & mask)) {
                adt_ch[i]->g.runtime_flags |= ADC_CHANNEL_RESUMED;
            }
        }
    }

    _int_enable();
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_stop
* Returned Value   : IO_OK
* Comments         : Stops (resets) measurement of single channel, or more
*                    channels defined by trigger mask
*
*END*********************************************************************/
_mqx_int _adt_stop(MQX_FILE_PTR f, ADT_CHANNEL_GENERIC_PTR channel, ADT_TRIGGER_MASK mask)
{
    ADT_CHANNEL_PTR *adt_ch = adt_internal.adc_d_bundle->adt_ch;
    _mqx_int i;
    _mqx_uint time;

    _int_disable();
    time = _adt_hw_get_time();
        
    if (channel) {
        channel->runtime_flags &= ~ADC_CHANNEL_RUNNING;
    }
    else {
        for (i = 0; i < ADC_MAX_CHANNELS; i++) {
            if (adt_ch[i] && (adt_ch[i]->g.trigger & mask)) {
                    adt_ch[i]->g.runtime_flags &= ~ADC_CHANNEL_RUNNING;
            }
        }
    }
    
    adt_internal.running_mask = 0;
    _adt_scheduler(time);

    _int_enable();
    return ADC_OK;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_set_wait
* Returned Value   : void
* Comments         : sets the timer to elapse (to call dispatcher) in 'time'
*
*END**********************************************************************/
static void _adt_hw_set_wait(uint_32 time)
{
    /* dispatcher inform us to wait time since now */
    adt_internal.adc_d_bundle->adt->time_to_next_adc = time;
    adt_setup_timer(time);
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_get_time
* Returned Value   : uint_32 - actual absolute time
* Comments         : 
*
*END**********************************************************************/
static uint_32 _adt_hw_get_time()
{
    uint_32 elapsed;

    elapsed = adt_internal.adc_d_bundle->adt->time_to_next_irq * (mcf522xx_timer_ptr->PMR - mcf522xx_timer_ptr->PCNTR) / (mcf522xx_timer_ptr->PMR + 1);
    return adt_internal.adc_d_bundle->adt->dispatch_time + elapsed;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : adt_setup_timer
* Returned Value   : void
* Comments         : sets HW timer to be able to measure time effectivelly
*
*END**********************************************************************/
static void adt_setup_timer(uint_32 time)
{
    uint_32  j;
    _mqx_int i;
    uint_16  preset, prescaler;

    /* search for next IRQ time */
    j = (time << adt_internal.adc_d_bundle->adt->shift) & 0x7FFFFFFF;
    if (time != (j >> adt_internal.adc_d_bundle->adt->shift)) { /* only 31 bits for timer setting */
        /* Till now, we need to have all AD conversions done, because we are going to modify ADC
        ** interrupt internal structures.
        */
        preset = 0xFFFF;  /* use maximum time for preset */
        prescaler = 15 << 8; /* use maximum prescaler  */
        adt_internal.adc_d_bundle->adt->time_to_next_irq = 0x80000000 >> adt_internal.adc_d_bundle->adt->shift;
        adt_internal.adc_d_bundle->adt->dispatch_next = FALSE;
    }
    else {
        /* compute time needed for next ADC: searching for first 1 */
        for (i = 0; i < 15; i++, j <<= 1) {
            if (j & 0x40000000) {
                preset = 0xFFFF; /* set maximum preset */
                prescaler = (14 - i) << 8; /* set prescaler */
                adt_internal.adc_d_bundle->adt->time_to_next_irq = 0x40000000 >> (i + adt_internal.adc_d_bundle->adt->shift);
                if (j == 0x40000000)
                    adt_internal.adc_d_bundle->adt->dispatch_next = TRUE;
                else
                    adt_internal.adc_d_bundle->adt->dispatch_next = FALSE;
                goto Set_Timer;
            }
        }
        preset = j >> i; /* set preset */
        if (preset)
            preset--;
        prescaler = 0 << 8; /* set prescaler */
        adt_internal.adc_d_bundle->adt->time_to_next_irq = j >> (15 + adt_internal.adc_d_bundle->adt->shift);
        adt_internal.adc_d_bundle->adt->dispatch_next = TRUE;
    }

Set_Timer:
    /* setup timer constants that correspond to time_to_next_irq */
    mcf522xx_timer_ptr->PCSR &= 0x007e; /* stop timer */
    mcf522xx_timer_ptr->PCNTR = mcf522xx_timer_ptr->PMR = preset;
    mcf522xx_timer_ptr->PCSR |= prescaler | 0x0021; /* starts timer */
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_run
* Returned Value   : void
* Comments         : starts timer
*
*END*********************************************************************/
static _mqx_int _adt_hw_run()
{
    return 0;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _adt_hw_stop
* Returned Value   : void
* Comments         : stops timer
*
*END*********************************************************************/
_mqx_int _adt_hw_stop()
{
    return adt_internal.adc_d_bundle->adt->dispatch_time = 0;
}

/*FUNCTION*****************************************************************
* 
* Function Name    : adt_cpu_isr
* Returned Value   : void
* Comments         : PIT (= timer) interrupt for ADC driver planning
*
*END*********************************************************************/
static void adt_cpu_isr
    (
        /* [IN] the address of the device specific information */
        pointer parameter
    )
{
    _mqx_int i, j;
    uint_16  prescaler;
    uint_16  preset;
    boolean  performed_adc;

    adt_internal.adc_d_bundle->adt->time_to_next_adc -= adt_internal.adc_d_bundle->adt->time_to_next_irq; /* update remaining time to ADC */
    adt_internal.adc_d_bundle->adt->dispatch_time += adt_internal.adc_d_bundle->adt->time_to_next_irq; /* update absolute time */
    
    if (adt_internal.adc_d_bundle->adt->dispatch_next) {
        mcf522xx_timer_ptr->PCSR &= 0xfffe; /* stop timer */
        _adt_scheduler(adt_internal.adc_d_bundle->adt->dispatch_time);
    }
    else
        adt_setup_timer(adt_internal.adc_d_bundle->adt->time_to_next_adc);
}

#endif // BSPCFG_ENABLE_ADC
/* EOF */
