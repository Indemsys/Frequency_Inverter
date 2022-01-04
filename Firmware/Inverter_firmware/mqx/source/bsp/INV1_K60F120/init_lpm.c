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
* $FileName: init_lpm.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains Low Power Manager initialization information.
*
*END************************************************************************/


#include "mqx.h"
#include "bsp.h"


#if MQX_ENABLE_LOW_POWER


#ifndef PE_LDD_VERSION


const LPM_CPU_OPERATION_MODE LPM_CPU_OPERATION_MODES[LPM_OPERATION_MODES] = 
{
    // LPM_OPERATION_MODE_RUN
    {
        LPM_CPU_POWER_MODE_RUN,                     // Index of predefined mode
        0,                                          // Additional mode flags
        0,                                          // Mode wake up events from pins 0..3
        0,                                          // Mode wake up events from pins 4..7
        0,                                          // Mode wake up events from pins 8..11
        0,                                          // Mode wake up events from pins 12..15
        0                                           // Mode wake up events from internal input sources
    },
    // LPM_OPERATION_MODE_WAIT
    {
        LPM_CPU_POWER_MODE_VLPR,                    // Index of predefined mode
        0,                                          // Additional mode flags
        0,                                          // Mode wake up events from pins 0..3
        0,                                          // Mode wake up events from pins 4..7
        0,                                          // Mode wake up events from pins 8..11
        0,                                          // Mode wake up events from pins 12..15
        0                                           // Mode wake up events from internal input sources
    },
    // LPM_OPERATION_MODE_SLEEP
    {
        LPM_CPU_POWER_MODE_WAIT,                    // Index of predefined mode
        LPM_CPU_POWER_MODE_FLAG_SLEEP_ON_EXIT,      // Additional mode flags
        0,                                          // Mode wake up events from pins 0..3
        0,                                          // Mode wake up events from pins 4..7
        0,                                          // Mode wake up events from pins 8..11
        0,                                          // Mode wake up events from pins 12..15
        0                                           // Mode wake up events from internal input sources
    },
    // LPM_OPERATION_MODE_STOP
    {
        LPM_CPU_POWER_MODE_LLS,                     // Index of predefined mode
        0,                                          // Additional mode flags
        0,                                          // Mode wake up events from pins 0..3
        0,                                          // Mode wake up events from pins 4..7
        0,                                          // Mode wake up events from pins 8..11
        0,                                          // Mode wake up events from pins 12..15
        LLWU_ME_WUME0_MASK                          // Mode wake up events from internal input sources - LPT
    }
};


#else


const LPM_CPU_OPERATION_MODE LPM_CPU_OPERATION_MODES[1] = {0};


#endif


#endif


/* EOF */
