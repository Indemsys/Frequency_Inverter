/**HEADER**********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*******************************************************************************
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
*******************************************************************************
*
* $FileName: psp_cpu.h$
* $Version : 3.8.10.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This file provides a generic header file for use by the mqx kernel
*   for including processor specific information
*
*END**************************************************************************/

#ifndef __psp_cpu_h__
    #define __psp_cpu_h__

#include "psp_cpudef.h"


/*
** The main requirement is to define target processor
*/
#ifndef MQX_CPU
    #error  You must define target processor in "user_config.h" (MQX_CPU)
#endif

#if PSP_MQX_CPU_IS_KINETIS
    #include <kinetis.h>
#elif PSP_MQX_CPU_IS_VYBRID
    #include <vybrid.h>
#else
    #error INCORRECT MQX_CPU SETTING
#endif

#include <cortex.h>

/* Needed for smarter _PSP_SET_CACR macro & backward compatibility */
#ifndef CACR_AUTO_CLEAR_BITS
    #define CACR_AUTO_CLEAR_BITS    0
#endif

#ifndef PSP_CACHE_SPLIT
    #define PSP_CACHE_SPLIT         0
#endif

#endif /* __psp_cpu_h__ */
