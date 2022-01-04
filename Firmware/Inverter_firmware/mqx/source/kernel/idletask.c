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
* $FileName: idletask.c$
* $Version : 3.8.3.2$
* $Date    : Mar-12-2012$
*
* Comments:
*
*   This file contains the idle task.
*
*END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_IDLE_TASK

/*!
 * \brief This function is the code for the idle task.
 *
 * Idle Task is a MQX task that runs when all application tasks are blocked.
 * \n This function implements a simple counter.
 * \n Counter can be read from a debugger and idle CPU time can be calculated.
 *
 * \param[in] parameter Parameter passed to the task when created.
 */
void _mqx_idle_task
    (
        uint_32 parameter
    )
{ /* Body */
    volatile KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    while (1) {
#if !defined(MQX_ENABLE_IDLE_LOOP) || MQX_ENABLE_IDLE_LOOP
        if (++kernel_data->IDLE_LOOP.IDLE_LOOP1 == 0) {
            if (++kernel_data->IDLE_LOOP.IDLE_LOOP2 == 0) {
                if (++kernel_data->IDLE_LOOP.IDLE_LOOP3 == 0) {
                ++kernel_data->IDLE_LOOP.IDLE_LOOP4;
                } /* Endif */
            } /* Endif */
        } /* Endif */
#endif
#if MQX_ENABLE_LOW_POWER
        if (parameter)
        {
            _ASM_SLEEP();
        }
#endif

    } /* Endwhile */

} /* Endbody */
#endif /* MQX_USE_IDLE_TASK */

/* EOF */

