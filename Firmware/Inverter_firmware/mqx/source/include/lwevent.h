/*HEADER**********************************************************************
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
* $FileName: lwevent.h$
* $Version : 3.8.10.1$
* $Date    : Feb-16-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   light weight event component.
*
*END*************************************************************************/

#ifndef __lwevent_h__
#define __lwevent_h__   1

#include <mqx_cnfg.h>

#if (! MQX_USE_LWEVENTS) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error LWEVENT component is currently disabled in MQX kernel. Please set MQX_USE_LWEVENTS to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Creation flags */
#define LWEVENT_AUTO_CLEAR          (0x00000001)

/* Error code */
#define LWEVENT_WAIT_TIMEOUT        (EVENT_ERROR_BASE|0x10)

/*--------------------------------------------------------------------------*/
/*!
 * \brief This structure defines a lightweight event.
 * 
 * Tasks can wait on and set event bits.
 * 
 * \see _lwevent_clear
 * \see _lwevent_create
 * \see _lwevent_destroy
 * \see _lwevent_set
 * \see _lwevent_set_auto_clear
 * \see _lwevent_usr_check 
 * \see _lwevent_wait_for
 * \see _lwevent_wait_ticks
 * \see _lwevent_wait_until
 * \see _usr_lwevent_clear
 * \see _usr_lwevent_create
 * \see _usr_lwevent_destroy
 * \see _usr_lwevent_set
 * \see _usr_lwevent_set_auto_clear
 * \see _usr_lwevent_wait_for
 * \see _usr_lwevent_wait_ticks
 * \see _usr_lwevent_wait_until                    
 */   
typedef struct lwevent_struct
{
    /*! \brief Queue data structures. */
    QUEUE_ELEMENT_STRUCT LINK;

    /*! \brief Queue of tasks waiting for event bits to be set. */
    QUEUE_STRUCT WAITING_TASKS;

    /*! \brief Validation stamp. */
    _mqx_uint VALID;

    /*! \brief Current bit value of the lightweight event. */
    _mqx_uint VALUE;

    /*! \brief Flags associated with the light weight event. */
    _mqx_uint FLAGS;

    /*! \brief Mask specifying lightweight event bits that are configured as auto-clear. */
    _mqx_uint AUTO;

}LWEVENT_STRUCT, _PTR_ LWEVENT_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _lwevent_create(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _lwevent_destroy(LWEVENT_STRUCT_PTR);
extern _mqx_uint _lwevent_set(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _lwevent_set_auto_clear(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _lwevent_clear(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _lwevent_wait_for  (LWEVENT_STRUCT_PTR, _mqx_uint,
                                     boolean, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _lwevent_wait_ticks(LWEVENT_STRUCT_PTR, _mqx_uint,
                                     boolean, _mqx_uint);
extern _mqx_uint _lwevent_wait_until(LWEVENT_STRUCT_PTR, _mqx_uint,
                                     boolean, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _lwevent_get_signalled(void);
extern _mqx_uint _lwevent_test(pointer _PTR_, pointer _PTR_);


#if MQX_ENABLE_USER_MODE
extern _mqx_uint _usr_lwevent_create(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _usr_lwevent_destroy(LWEVENT_STRUCT_PTR);
extern _mqx_uint _usr_lwevent_set(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _usr_lwevent_set_auto_clear(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _usr_lwevent_clear(LWEVENT_STRUCT_PTR, _mqx_uint);
extern _mqx_uint _usr_lwevent_wait_for  (LWEVENT_STRUCT_PTR, _mqx_uint,
                                         boolean, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _usr_lwevent_wait_ticks(LWEVENT_STRUCT_PTR, _mqx_uint,
                                         boolean, _mqx_uint);
extern _mqx_uint _usr_lwevent_wait_until(LWEVENT_STRUCT_PTR, _mqx_uint,
                                         boolean, MQX_TICK_STRUCT_PTR);
extern _mqx_uint _usr_lwevent_get_signalled(void);
extern _mqx_uint _lwevent_usr_check(LWEVENT_STRUCT_PTR);
#endif /* MQX_ENABLE_USER_MODE */

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
