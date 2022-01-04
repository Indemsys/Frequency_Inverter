/*HEADER*********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
***************************************************************************
* $FileName: mcc_mqx.h$
* $Version : 3.8.1.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains prototypes for MQX-specific MCC library functions
*
*END************************************************************************/

#ifndef __MCC_MQX__
#define __MCC_MQX__

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "core_mutex.h"
#include "vybrid.h"

#define MCC_MQX_LWEVENT_COMPONENTS_COUNT   (8)
#define MCC_MQX_LWEVENT_GROUP_SIZE         (32)

#define MCC_MQX_NODE_NUMBER                (0)

#define MCC_DCACHE_ENABLE(n)               _DCACHE_ENABLE(n)
#define MCC_DCACHE_DISABLE()               _DCACHE_DISABLE()
#define MCC_DCACHE_FLUSH()                 _DCACHE_FLUSH()
#define MCC_DCACHE_FLUSH_LINE(p)           _DCACHE_FLUSH_LINE(p)
#define MCC_DCACHE_FLUSH_MLINES(p, m)      _DCACHE_FLUSH_MLINES(p, m)
#define MCC_DCACHE_INVALIDATE()            _DCACHE_INVALIDATE()
#define MCC_DCACHE_INVALIDATE_LINE(p)      _DCACHE_INVALIDATE_LINE(p)
#define MCC_DCACHE_INVALIDATE_MLINES(p, m) _DCACHE_INVALIDATE_MLINES(p, m)

/* Semaphore-related functions */
int mcc_init_semaphore(unsigned int);
int mcc_deinit_semaphore(unsigned int);
int mcc_get_semaphore(void);
int mcc_release_semaphore(void);

/* CPU-to-CPU interrupt-related functions */
int mcc_register_cpu_to_cpu_isr(void);
int mcc_generate_cpu_to_cpu_interrupt(void);

/* Memory management-related functions */
void mcc_memcpy(void*, void*, unsigned int);

#endif /* __MCC_MQX__ */
