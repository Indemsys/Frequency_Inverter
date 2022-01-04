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
* $FileName: watchdog.h$
* $Version : 3.8.6.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   watchdog component.
*
*END************************************************************************/
#ifndef __watchdog_h__
#define __watchdog_h__ 1

#include <mqx_cnfg.h>
#if (! MQX_USE_SW_WATCHDOGS) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error WATCHDOG component is currently disabled in MQX kernel. Please set MQX_USE_SW_WATCHDOGS to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Error codes */

#define WATCHDOG_INVALID_ERROR_FUNCTION   (WATCHDOG_ERROR_BASE|0x01)
#define WATCHDOG_INVALID_INTERRUPT_VECTOR (WATCHDOG_ERROR_BASE|0x02)

/*--------------------------------------------------------------------------*/
/*
 *                    TYPEDEFS FOR _CODE_PTR_ FUNCTIONS
 */
typedef void (_CODE_PTR_  WATCHDOG_ERROR_FPTR)(pointer);


/*--------------------------------------------------------------------------*/
/*                        DATA STRUCTURE DEFINITIONS                        */

/*
 *  external declarations for the interface procedures
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _watchdog_create_component(_mqx_uint, WATCHDOG_ERROR_FPTR);
extern boolean   _watchdog_stop(void);
extern boolean   _watchdog_start(uint_32);
extern boolean   _watchdog_start_ticks(MQX_TICK_STRUCT_PTR);
extern _mqx_uint _watchdog_test(pointer _PTR_, pointer _PTR_);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __watchdog_h__ */
/* EOF */
