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
* $FileName: soselall.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the RTCS_selectall() implementation.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_selectall
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_selectall
   (
      uint_32     timeout
         /* [IN] specifies the maximum amount of time to wait for data */
   )
{ /* Body */
   SOCKSELECT_PARM   parms;
   uint_32           error;

   parms.owner   = RTCS_task_getid();
   parms.timeout = timeout;
   error = RTCSCMD_issue(parms, SOCK_selectall);
   if (error) return RTCS_SOCKET_ERROR;

   return parms.sock;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_selectall
* Parameters      :
*
*     _rtcs_taskid      owner       [IN] socket owner
*     uint_32 _PTR_     sock_ptr    not used
*     uint_32           sock_count  not used
*     uint_32           timeout     [IN] timeout, 0=forever, -1=none
*     uint_32           sock        [OUT] socket with activity
*
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

void SOCK_selectall
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCKET_STRUCT_PTR          socket_ptr;

   /* cycle through sockets looking for one owned by this task */
   for (socket_ptr = socket_cfg_ptr->SOCKET_HEAD;
        socket_ptr;
        socket_ptr = socket_ptr->NEXT) 
   {
      if (SOCK_Is_owner(socket_ptr, parms->owner)
         && SOCK_select_activity(socket_ptr, 0)) 
      {   
         break;
      } /* Endif */
   } /* Endfor */

   if (socket_ptr) 
   {
      parms->sock = (uint_32)socket_ptr;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   if (parms->timeout == (uint_32)-1) 
   {
      parms->sock = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   SOCK_select_block(parms);

} /* Endbody */


/* EOF */
