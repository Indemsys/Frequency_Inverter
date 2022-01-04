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
* $FileName: rtcscmd.c$
* $Version : 3.6.14.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the interface functions to the
*   RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "socket.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_create
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Initialize RTCS.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_create
   (
      void
   )
{ /* Body */
   RTCS_DATA_PTR              RTCS_data_ptr;
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
   uint_32                    error;

   /*
   ** Check and see if this is the first time we have initialized,
   */
   if (RTCS_get_data() != NULL) {
      return RTCSERR_INITIALIZED;
   } 
//
   RTCS_data_ptr = RTCS_mem_alloc_system_zero(sizeof(RTCS_DATA));

   if (RTCS_data_ptr == NULL)  {
      error = RTCSERR_OUT_OF_MEMORY;
   } else {

   _mem_set_type(RTCS_data_ptr, MEM_TYPE_RTCS_DATA);

   RTCS_set_data(RTCS_data_ptr);

   /*
   ** Initialize socket state
   */
   socket_cfg_ptr = RTCS_mem_alloc_system_zero(sizeof(SOCKET_CONFIG_STRUCT));

      if (socket_cfg_ptr == NULL)  {
         error =  RTCSERR_OUT_OF_MEMORY;
      } else {
         _mem_set_type(socket_cfg_ptr, MEM_TYPE_SOCKET_CONFIG_STRUCT);
      
         socket_cfg_ptr->INITIALIZED = TRUE;
         socket_cfg_ptr->SOCKET_HEAD = NULL;
         socket_cfg_ptr->SOCKET_TAIL = NULL;
         RTCS_mutex_init(&socket_cfg_ptr->SOCK_MUTEX);
         RTCS_setcfg(SOCKET, socket_cfg_ptr);

   /*
   ** Initialize global data
   */
   _IP_forward    = FALSE;
   _TCP_bypass_rx = FALSE;
   _TCP_bypass_tx = FALSE;

   RTCS_data_ptr->VERSION            = RTCS_VERSION;
#if RTCSCFG_LOG_SOCKET_API||RTCSCFG_LOG_PCB
   RTCS_data_ptr->RTCS_LOG_PROTCOUNT = RTCSLOG_PROT_MAX;
   RTCSLOG_disable(RTCS_LOGCTRL_ALL);
#endif
   RTCS_data_ptr->TCPIP_qid = RTCS_msgq_get_id(0,TCPIP_QUEUE);

         /*
         ** Create a pool of buffers for use in communicating to the TCP/IP task.
         */
         RTCS_data_ptr->TCPIP_msg_pool = RTCS_msgpool_create(sizeof(TCPIP_MESSAGE),
            _RTCS_msgpool_init, _RTCS_msgpool_grow, _RTCS_msgpool_max);
         if (RTCS_data_ptr->TCPIP_msg_pool == MSGPOOL_NULL_POOL_ID) {
            RTCS_log_error( ERROR_TCPIP, RTCSERR_CREATE_POOL_FAILED, 0, 0, 0);
            error =  RTCSERR_CREATE_POOL_FAILED;
         } else {
            /*
            ** Create the socket partition
            */
            RTCS_data_ptr->RTCS_socket_partition = RTCS_part_create(sizeof(SOCKET_STRUCT),
              _RTCS_socket_part_init, _RTCS_socket_part_grow, _RTCS_socket_part_max, NULL, NULL);
            if (RTCS_data_ptr->RTCS_socket_partition == 0) {
               RTCS_log_error(ERROR_RTCS, RTCSERR_CREATE_PARTITION_FAILED, 0, 0, 0);
               error = RTCSERR_CREATE_PARTITION_FAILED;
            } else {
               /*
               ** Create TCPIP task
               */
               error = RTCS_task_create("TCP/IP", _RTCSTASK_priority, _RTCSTASK_stacksize, TCPIP_task, NULL);
               if (error) {
                  RTCS_part_destroy(RTCS_data_ptr->RTCS_socket_partition);
               }
            }
            if (error) {
               RTCS_msgpool_destroy( RTCS_data_ptr->TCPIP_msg_pool );
            }     
         }
         if (error) {
            RTCS_setcfg(SOCKET, NULL);
            _mem_free(socket_cfg_ptr);
         }
      }
      if (error) {
         RTCS_set_data(NULL);
         _mem_free(RTCS_data_ptr);
      }
   }
   
   return error;
            
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_issue
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command to the TCP/IP task
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_issue
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;
   TCPIP_MESSAGE_PTR    message;

   RTCS_data_ptr = RTCS_get_data();

   message = RTCS_msg_alloc(RTCS_data_ptr->TCPIP_msg_pool);
   if (message == NULL) {
      parm_ptr->ERROR = RTCSERR_OUT_OF_BUFFERS;
   } else {
      parm_ptr->ERROR = RTCS_OK;
      parm_ptr->SYNC  = _task_get_td(0);

      message->HEAD.TARGET_QID = RTCS_data_ptr->TCPIP_qid;
      message->HEAD.SOURCE_QID = 0;
      message->HEAD.SIZE = sizeof(*message);
      message->COMMAND = command;
      message->DATA = parm_ptr;

      if (!RTCS_msgq_send_blocked(message, RTCS_data_ptr->TCPIP_msg_pool)) {
         parm_ptr->ERROR = RTCSERR_SEND_FAILED;
      } /* Endif */

   } /* Endif */

   return parm_ptr->ERROR;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_internal
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command from within the TCP/IP task
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_internal
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */

   parm_ptr->ERROR = RTCS_OK;
   parm_ptr->SYNC  = NULL;

   command(parm_ptr);

   return parm_ptr->ERROR;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_smartissue
* Returned Values : uint_32 (error code)
* Comments        :
*     Issue a command from any context
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_cmd_smartissue
   (
      TCPIP_PARM_PTR    parm_ptr,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;

   RTCS_data_ptr = RTCS_get_data();

   if (RTCS_task_getid() == RTCS_data_ptr->TCPIP_TASKID) {
      return RTCS_cmd_internal(parm_ptr, command);
   } else {
      return RTCS_cmd_issue(parm_ptr, command);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_complete
* Returned Values : void
* Comments        :
*     Unblocks an application following successful completion of a command.
*
*END*-----------------------------------------------------------------*/

void RTCS_cmd_complete
   (
      TCPIP_PARM_PTR    parm_ptr,
      uint_32           error
   )
{ /* Body */

   if (error) {
      parm_ptr->ERROR = error;
   } /* Endif */

   if (parm_ptr->SYNC) {
      _task_ready(parm_ptr->SYNC);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_cmd_service
* Returned Values : TRUE on success
* Comments        :
*     Issue a nonblocking command to the TCP/IP task
*
*END*-----------------------------------------------------------------*/

boolean RTCS_cmd_service
   (
      RTCSPCB_PTR       pcb,
      void (_CODE_PTR_  command)(pointer)
   )
{ /* Body */
   RTCS_DATA_PTR        RTCS_data_ptr;
   TCPIP_MESSAGE_PTR    message;

   RTCS_data_ptr = RTCS_get_data();

   message = RTCS_msg_alloc(RTCS_data_ptr->TCPIP_msg_pool);
   if (message == NULL) {
      return FALSE;
   } /* Endif */

   message->HEAD.TARGET_QID = RTCS_data_ptr->TCPIP_qid;
   message->HEAD.SOURCE_QID = 0;
   message->HEAD.SIZE = sizeof(*message);
   message->COMMAND = command;
   message->DATA = pcb;

   return RTCS_msgq_send(message, RTCS_data_ptr->TCPIP_msg_pool);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  RTCS_seterror
*  Returned Value:  void
*  Comments      :  Called to record an error condition in a
*                   provided error structure.
*
*END*-----------------------------------------------------------------*/

void RTCS_seterror
   (
      RTCS_ERROR_STRUCT_PTR   errptr,
         /* [IN/OUT] where to record the error */
      uint_32                 errcode,
         /* [IN] the error that occurred */
      uint_32                 errparm
         /* [IN] additional information */
   )
{ /* Body */

   /* Only record the first error that occurs */
   if (errptr->ERROR == RTCS_OK) {
      errptr->ERROR    = errcode;
      errptr->PARM     = errparm;
#ifdef __MQX__
      errptr->TASK_ID   = _task_get_id();
      errptr->TASKCODE = _task_get_error();
      errptr->MEMPTR   = _mem_get_highwater();
      errptr->STACK    = _task_check_stack();
#endif
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_log_error
* Returned Value   : none
* Comments  :  Routine to handle non-fatal errors.
*
*END*-----------------------------------------------------------------*/

void  RTCS_log_error
   (
      uint_32  module,
      uint_32  error,
      uint_32  p1,
      uint_32  p2,
      uint_32  p3
   )
{ /* Body */
#if RTCSCFG_LOG_SOCKET_API || RTCSCFG_LOG_PCB
   RTCS_log(RTCSLOG_TYPE_ERROR,module,error,p1,p2,p3);
#endif
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_io_open
* Returned Value  : open file descriptor
* Comments        : RTOS-specific wrapper for fopen().
*
*END*-----------------------------------------------------------------*/

pointer RTCS_io_open
   (
      char_ptr       filename,
      char_ptr       filemode,
      uint_32 _PTR_  error_ptr
   )
{ /* Body */
   MQX_FILE_PTR file;
   uint_32  i;

   /* Scan for device name delimiter */
   for (i = 0; filename[i] && filename[i] != ':'; i++) {};

   /* 
   ** If the next char is not nul then a file
   ** on the resident file system is probably
   ** being opened.
   */
   if (filename[i] == ':' && filename[i+1] != '\0') {
      /* Assume we are opening a file */
      file = fopen(filename, filemode);
   } else {
      file = fopen(filename, 0);
   } /* Endif */

   if (error_ptr) {
      if (file) {
         *error_ptr = RTCS_OK;
      } else {
         *error_ptr = _task_get_error();
         if (!*error_ptr) {
            *error_ptr = RTCS_ENOENT;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   return file;

} /* Endbody */


/* EOF */
