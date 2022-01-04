/**HEADER********************************************************************
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
* $FileName: rtcstask.c$
* $Version : 3.5.11.0$
* $Date    : Dec-11-2009$
*
* Comments:
*
*   This file contains the interface to the RTOS
*   task management functions.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>


struct rtcs_task_state {
   _rtcs_sem        sem;
   void (_CODE_PTR_ start)(pointer, pointer);
   pointer          arg;
   uint_32          error;
};

/*TASK*-----------------------------------------------------------------
*
* Function Name   : RTCS_task
* Returned Values :
* Comments        :
*     The parent of all RTCS tasks.
*
*END*-----------------------------------------------------------------*/

static void RTCS_task
   (
      uint_32 task_parm
   )
{ /* Body */
   struct rtcs_task_state _PTR_ task = (struct rtcs_task_state _PTR_)task_parm;
   /* This is completely unnecessary -- it's done only for Task Aware Debugging */
   _task_get_template_ptr (MQX_NULL_TASK_ID)->TASK_ADDRESS = (void(_CODE_PTR_)(uint_32))task->start;
 
   task->start(task->arg, task);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_create
* Returned Values : uint_32 (error code)
* Comments        :
*     Create a task and wait for it to complete initialization.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_task_create
   (
      char_ptr          name,
      uint_32           priority,
      uint_32           stacksize,
      void (_CODE_PTR_  start)(pointer, pointer),
      pointer           arg
   )
{ /* Body */
   TASK_TEMPLATE_STRUCT    task_template;
   struct rtcs_task_state  task;

   RTCS_sem_init(&task.sem);
   task.start = start;
   task.arg   = arg;
   task.error = RTCS_OK;

   _mem_zero((uchar_ptr)&task_template, sizeof(task_template));
   task_template.TASK_NAME          = name;
   task_template.TASK_PRIORITY      = priority;
   task_template.TASK_STACKSIZE     = stacksize;
   task_template.TASK_ADDRESS       = RTCS_task;
   task_template.CREATION_PARAMETER = (uint_32)&task;
   if (_task_create(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {
      RTCS_sem_destroy(&task.sem);
      return RTCSERR_CREATE_FAILED;
   } /* Endif */

   RTCS_sem_wait(&task.sem);
   RTCS_sem_destroy(&task.sem);
   return task.error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_resume_creator
* Returned Values : void
* Comments        :
*     Return an error code to the caller's creator.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_resume_creator
   (
      pointer creator,
      uint_32 error
   )
{ /* Body */
   struct rtcs_task_state _PTR_ task = creator;

   if (task) {
      task->error = error;
      RTCS_sem_post(&task->sem);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_exit
* Returned Values : void
* Comments        :
*     Return an error code to the caller's creator, and block.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_exit
   (
      pointer creator,
      uint_32 error
   )
{ /* Body */

   RTCS_task_resume_creator(creator, error);
   _task_set_error(error);
   _task_block();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_id_from_td
* Returned Values : MQX _task_id
* Comments        :
*     Returns MQX task id from RTCS task id.
*
*END*-----------------------------------------------------------------*/

_task_id RTCS_task_id_from_td
   (
      _rtcs_taskid   taskid
   )
{ /* Body */

   return _task_get_id_from_td (taskid);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_destroy
* Returned Values : void
* Comments        :
*     Destroy a task.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_destroy
   (
      _rtcs_taskid   taskid
   )
{ /* Body */

   _task_destroy (RTCS_task_id_from_td (taskid));

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_task_abort
* Returned Values : void
* Comments        :
*     Destroy a task.
*
*END*-----------------------------------------------------------------*/

void RTCS_task_abort
   (
      _rtcs_taskid   taskid
   )
{ /* Body */

   _task_abort (RTCS_task_id_from_td (taskid));

} /* Endbody */

/* EOF */
