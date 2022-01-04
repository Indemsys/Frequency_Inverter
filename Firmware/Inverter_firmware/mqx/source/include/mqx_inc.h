/*HEADER*********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
******************************************************************************
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
******************************************************************************
*
* $FileName: mqx_inc.h$
* $Version : 3.8.6.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains include statements for all header files
*   required when compiling the kernel.
*
*END*************************************************************************/

#ifndef __mqx_inc_h__
#define __mqx_inc_h__

#ifdef MQX_REDUCE_DEBUG /* CR1446 & CR1434 */
#if defined(__HIGHC__) && !defined(MQX_CRIPPLED_EVALUATION)
   /* Minimize symbolic debug info when MetaWare compiler */
#  pragma on(nodebug)
# endif
#endif

/*
** These are the 'MQX' include files
*/
#include "mqx.h"
#include "mqx_ioc.h"
#include "psp.h"
#include <psp_comp.h>
#include "mem_prv.h"
#include "mqx_prv.h"
#include "psp_prv.h"
#include "psp_abi.h"

#ifdef MQX_REDUCE_DEBUG /* CR1446 & CR1434 */
# if defined(__HIGHC__) && !defined(MQX_CRIPPLED_EVALUATION)
   /* Minimize symbolic debug info when MetaWare compiler */
#  pragma pop(nodebug)
# endif
#endif

#if MQX_KERNEL_LOGGING
#include "lwlog.h"
#include "klog.h"
#endif
#if MQX_USE_LWMEM_ALLOCATOR
#ifndef __MEMORY_MANAGER_COMPILE__
#include "lwmem_prv.h"
#endif
#endif

#endif /* __mqx_inc_h__ */
/* EOF */
