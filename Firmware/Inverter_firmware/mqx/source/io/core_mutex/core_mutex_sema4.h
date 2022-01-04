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
* $FileName: core_mutex_sema4.h$
* $Version : 3.8.2.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains the type definitions for the core mutex driver for the SEMA4.
*
*END************************************************************************/

#ifndef __core_mutex_sema4_h__
#define __core_mutex_sema4_h__

#define __core_mutex_sema4_h__version "$Version:3.8.2.0$"
#define __core_mutex_sema4_h__date    "$Date:Sep-24-2012$"

#include <sema4.h>

#ifndef BSPCFG_CORE_MUTEX_STATS
#define BSPCFG_CORE_MUTEX_STATS 0
#endif

typedef struct core_mutex_struct {
    uint_32                 VALID;
    uint_32                 GATE_NUM;
    vuchar_ptr              GATE_PTR;
    pointer                 WAIT_Q;
#if BSPCFG_CORE_MUTEX_STATS
    uint_32                 LOCKS;
    uint_32                 WAITS;
    uint_32                 UNLOCKS;
#endif
} CORE_MUTEX_STRUCT, * CORE_MUTEX_PTR;


typedef struct core_mutex_device {
    SEMA4_MemMapPtr SEMA4_PTR;
    vuint_16_ptr            CPNTF_PTR;

    uint_32                 ALLOCED;
    CORE_MUTEX_PTR          MUTEX_PTR[SEMA4_NUM_GATES];
} CORE_MUTEX_DEVICE, * CORE_MUTEX_DEVICE_PTR;

#ifdef __cplusplus
extern "C" {
#endif


SEMA4_MemMapPtr _bsp_get_sema4_base_address(uint_32);
uint_32 _bsp_get_sema4_vector(uint_32 device);

#ifdef __cplusplus
}
#endif

#endif
