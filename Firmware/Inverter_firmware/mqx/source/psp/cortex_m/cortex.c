/**HEADER********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: cortex.c$
* $Version : 3.8.12.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the type definitions for the ARM Cortex processors.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "cortex.h"

#if MQX_ENABLE_USER_MODE
#include "lwevent.h"

#if MQX_USE_LWEVENTS
#include "lwevent_prv.h"
#endif /* MQX_USE_LWEVENTS */

#if MQX_USE_LWMSGQ
#include "lwmsgq.h"
#include "lwmsgq_prv.h"
#endif

#if MQX_USE_LWMEM
#include "lwmem.h"
#endif

#endif /* MQX_ENABLE_USER_MODE */

#if !MQX_USE_IDLE_TASK
#error "MQX for ARM Cortex M requires to enable MQX_USE_IDLE_TASK"
#endif

#if PSP_MQX_CPU_IS_VYBRID
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _psp_core_num
* Returned Value   : int
* Comments         :
*    This function returns the core number
*
*END*----------------------------------------------------------------------*/
uint_32 _psp_core_num(void)
{
    return MSCM_CPxNUM & MSCM_CPxNUM_CPN_MASK;
}
#endif // PSP_MQX_CPU_IS_VYBRID

#if MQX_ENABLE_USER_MODE

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name     : _mqx_api_call_handler
* Returned Value    : uint_32
*       return returned value of called function
* Comments          :
* 	MQX API handler for usermode - part of wrapper around standard
* MQX API which require privilege mode.
*
*END*---------------------------------------------------------------------*/

uint_32 _mqx_api_call_handler
    (
        // [IN] API number - number of wrapped function
        MQX_API_NUMBER_ENUM api_no,
        // [IN] generic parameter - direct use with called MQX API fn
        MQX_API_CALL_PARAMS_PTR params
    )
{
    uint_32 res = -1;
    uint_32 param0 = params->param0;
    uint_32 param1 = params->param1;
    uint_32 param2 = params->param2;
    uint_32 param3 = params->param3;
    uint_32 param4 = params->param4;

    switch (api_no) {

    // _lwsem
    case MQX_API_LWSEM_POLL:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0))
            res = (uint_32)_lwsem_poll((LWSEM_STRUCT_PTR)param0);
        break;
    case MQX_API_LWSEM_POST:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0))
            res = _lwsem_post((LWSEM_STRUCT_PTR)param0);
        break;
    case MQX_API_LWSEM_WAIT:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0))
            res = _lwsem_wait((LWSEM_STRUCT_PTR)param0);
        break;
    case MQX_API_LWSEM_CREATE:
        res = _lwsem_create_internal((LWSEM_STRUCT_PTR)param0, (_mqx_int)param1, (boolean)param2, TRUE);
        break;
#if MQX_HAS_TICK
    case MQX_API_LWSEM_WAIT_FOR:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0) && (!param1 || _psp_mem_check_access(param1, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)))
            res = _lwsem_wait_for((LWSEM_STRUCT_PTR)param0, (MQX_TICK_STRUCT_PTR)param1);
        break;
    case MQX_API_LWSEM_WAIT_TICKS:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0))
            res = _lwsem_wait_ticks((LWSEM_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWSEM_WAIT_UNTIL:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0) && (!param1 || _psp_mem_check_access(param1, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)))
            res = _lwsem_wait_until((LWSEM_STRUCT_PTR)param0, (MQX_TICK_STRUCT_PTR)param1);
        break;
    case MQX_API_LWSEM_DESTROY:
        if (MQX_OK == _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)) {
            res = _lwsem_destroy_internal((LWSEM_STRUCT_PTR)param0, TRUE);
        }
        break;

#endif // MQX_HAS_TICK

    // _lwevent
#if MQX_USE_LWEVENTS
    case MQX_API_LWEVENT_CLEAR:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0))
            res = _lwevent_clear((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWEVENT_SET:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0))
            res = _lwevent_set((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWEVENT_SET_AUTO_CLEAR:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0))
            res = _lwevent_set_auto_clear((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWEVENT_WAIT_FOR:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0) && \
            (!param3 || _psp_mem_check_access(param3, sizeof(MQX_TICK_STRUCT), MPU_UM_RW))) {
            res = _lwevent_wait_for((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, (boolean)param2, (MQX_TICK_STRUCT_PTR)param3);
        }
        break;
    case MQX_API_LWEVENT_WAIT_FOR_TICKS:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)) {
            res = _lwevent_wait_ticks((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, (boolean)param2, (_mqx_uint)param3);
        }
        break;
    case MQX_API_LWEVENT_WAIT_UNTIL:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0) && \
            (!param3 || _psp_mem_check_access(param3, sizeof(MQX_TICK_STRUCT), MPU_UM_RW))) {
            res = _lwevent_wait_until((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, (boolean)param2, (MQX_TICK_STRUCT_PTR)param3);
        }
        break;
    case MQX_API_LWEVENT_GET_SIGNALLED:
        res = _lwevent_get_signalled();
        break;

    case MQX_API_LWEVENT_CREATE:
        res = _lwevent_create_internal((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, TRUE);
        break;

    case MQX_API_LWEVENT_DESTROY:
        if (MQX_OK == _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)) {
            res = _lwevent_destroy_internal((LWEVENT_STRUCT_PTR)param0, TRUE);
        }
        break;
#endif

#if MQX_USE_LWMSGQ
    case MQX_API_LWMSGQ_INIT:
        res = _lwmsgq_init_internal((pointer)param0, (_mqx_uint)param1, (_mqx_uint)param2, TRUE);
        break;
    case MQX_API_LWMSGQ_RECEIVE:
        if (MQX_OK == _lwmsgq_usr_check((LWMSGQ_STRUCT_PTR)param0) && \
            _psp_mem_check_access(param1, ((LWMSGQ_STRUCT_PTR)param0)->MSG_SIZE, MPU_UM_RW) && \
            (!param4 || _psp_mem_check_access(param4, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)))
            res = _lwmsgq_receive((pointer)param0, (_mqx_max_type_ptr)param1, (_mqx_uint)param2, (_mqx_uint)param3, (MQX_TICK_STRUCT_PTR)param4);

        break;
    case MQX_API_LWMSGQ_SEND:
        if (MQX_OK == _lwmsgq_usr_check((LWMSGQ_STRUCT_PTR)param0) && \
            _psp_mem_check_access(param1, ((LWMSGQ_STRUCT_PTR)param0)->MSG_SIZE, MPU_UM_RW))
            res = _lwmsgq_send((pointer)param0, (_mqx_max_type_ptr)param1, (_mqx_uint)param2);
        break;
#endif // MQX_USE_LWMSGQ

    case MQX_API_TASK_CREATE:
        res = _task_create_internal((_processor_number)param0, (_mqx_uint)param1, (uint_32)param2, TRUE);
        break;
    case MQX_API_TASK_DESTROY:
        res = _task_destroy_internal((_task_id)param0, TRUE);
        break;
    case MQX_API_TASK_ABORT:
        res = _task_abort_internal((_task_id)param0, TRUE);
        break;
    case MQX_API_TASK_READY:
        _task_ready((pointer)param0);
        res = MQX_OK;       // irelevant, function is without return value
        break;
    case MQX_API_TASK_SET_ERROR:
        res = _task_set_error((_mqx_uint)param0);
        break;
    case MQX_API_TASK_GET_TD:
        res = (uint_32)_task_get_td((_task_id)param0);
        break;

#if MQX_USE_LWMEM
    case MQX_API_LWMEM_ALLOC:
        res = (uint_32)_usr_lwmem_alloc_internal((_mem_size)param0);
        break;

    case MQX_API_LWMEM_ALLOC_FROM:
        if (_psp_mem_check_access(param0, sizeof(LWMEM_POOL_STRUCT), MPU_UM_RW) && \
            _psp_mem_check_access((uint_32)(((LWMEM_POOL_STRUCT_PTR)param0)->POOL_ALLOC_START_PTR), (char*)(((LWMEM_POOL_STRUCT_PTR)param0)->POOL_ALLOC_END_PTR) - (char*)(((LWMEM_POOL_STRUCT_PTR)param0)->POOL_ALLOC_START_PTR), MPU_UM_RW))
            res = (uint_32)_lwmem_alloc_from((_lwmem_pool_id)param0, (_mem_size)param1);
        else
            res = 0; // NULL, allocation failed
        break;

    case MQX_API_LWMEM_FREE:
        if (_psp_mem_check_access(param0, 4, MPU_UM_RW))
            res = _lwmem_free((pointer)param0);
        break;

    case MQX_API_LWMEM_CREATE_POOL:\
        if (_psp_mem_check_access(param0, sizeof(LWMEM_POOL_STRUCT), MPU_UM_RW) && \
            _psp_mem_check_access(param1, param2, MPU_UM_RW))
            res = (uint_32)_lwmem_create_pool((LWMEM_POOL_STRUCT_PTR)param0, (pointer)param1, (_mem_size)param2);
        break;
#endif // MQX_USE_LWMEM

    // _time
    case MQX_API_TIME_DELAY:
        _time_delay(param0);
        res = MQX_OK;       // irelevant, function is without return value
        break;

#if MQX_HAS_TICK
    case MQX_API_TIME_DELAY_TICKS:
        _time_delay_ticks(param0);
        res = MQX_OK;       // irelevant, function is without return value
        break;
    case MQX_API_TIME_GET_ELAPSED_TICKS:
        if (_psp_mem_check_access(param0, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)) {
            _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)param0);
            res = MQX_OK;       // irelevant, function is without return value
        }
        else {
            _task_set_error(MQX_ACCESS_ERROR);
        }

        break;
#endif // MQX_HAS_TICK
    default:
        while (1);
    }

    return res;
}

#endif // MQX_ENABLE_USER_MODE
