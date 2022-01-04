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
* $FileName: rtcs.c$
* $Version : 3.8.6.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the interface functions to the
*   RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "socket.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_socket
* Returned Value  : socket handle
* Comments  :  Routine to open a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_socket
   (
      uint_32     pf,
         /*[IN] specifies the protocol family */
      uint_32     type,
         /*[IN] specifies the type of communication */
      uint_32     protocol
         /*[IN] select a specific protocol */
   )
{ /* Body */
   SOCKET_STRUCT_PTR socket_ptr;
   uint_32           error;

   RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_SOCKET, type);

   socket_ptr = SOCK_Get_sock_struct((RTCS_SOCKET_CALL_STRUCT_PTR)type,
                                     RTCS_task_getid());
   if (socket_ptr == NULL) {
      RTCSLOG_API(RTCSLOG_TYPE_FNEXIT, RTCSLOG_FN_SOCKET, RTCS_SOCKET_ERROR);
      return RTCS_SOCKET_ERROR;
   } /* Endif */

   if (socket_ptr->PROTOCOL->SOCK_SOCKET) {
      error = socket_ptr->PROTOCOL->SOCK_SOCKET((uint_32)socket_ptr);
      if (error) {
         SOCK_Free_sock_struct(socket_ptr);
         RTCSLOG_API(RTCSLOG_TYPE_FNEXIT, RTCSLOG_FN_SOCKET, RTCS_SOCKET_ERROR);
         return RTCS_SOCKET_ERROR;
      } /* Endif */
   } /* Endif */

   /*Setup internet family*/
   socket_ptr->AF = pf;

   RTCSLOG_API(RTCSLOG_TYPE_FNEXIT, RTCSLOG_FN_SOCKET, (uint_32)socket_ptr);
   return (uint_32)socket_ptr;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_shutdown
* Returned Value  : OK or error code
* Comments  :  Routine to close a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_shutdown
   (
      uint_32     sock,
         /*[IN] socket handle for this socket */
      uint_32     how
         /*[IN] specifies whether or not connection should be immediate */
   )
{
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;
            uint_32           error;

   error = RTCS_OK;

   if ((sock==0) || (sock == RTCS_SOCKET_ERROR) || (socket_ptr->VALID != SOCKET_VALID))
   {
      return RTCSERR_SOCK_INVALID;
   }

   if (socket_ptr->PROTOCOL->SOCK_SHUTDOWN)
   {
      error = socket_ptr->PROTOCOL->SOCK_SHUTDOWN(sock, how);
   }
   SOCK_Free_sock_struct(socket_ptr);
   return error;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_attachsock
* Returned Value  : socket handle or RTCS_ERROR
* Comments  :  Routine to take ownership of a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_attachsock
   (
      uint_32     sock
         /*[IN] specifies the handle of the existing socket */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   if (!SOCK_Add_owner(socket_ptr, RTCS_task_getid())) {
      RTCS_setsockerror(sock, RTCSERR_OUT_OF_MEMORY);
      return RTCS_SOCKET_ERROR;
   } /* Endif */

   return sock;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_detachsock
* Returned Value  : error code
* Comments  :  Routine to relinquish ownership of a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_detachsock
   (
      uint_32     sock
         /*[IN] specifies the handle of the existing socket */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   if (!SOCK_Remove_owner(socket_ptr, RTCS_task_getid())) {
      return RTCSERR_SOCK_NOT_OWNER;
   } /* Endif */

   return RTCS_OK;

} /*EndBody*/


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_geterror
* Returned Value  : error code
* Comments  :  Routine is used to get the last error code associated
*              with a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_geterror
   (
      uint_32     sock
         /*[IN] socket handle for this socket */
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   /*
   ** Verify the socket
   */
   if ((sock==0) || (sock==RTCS_SOCKET_ERROR)) {
      return RTCSERR_SOCK_INVALID;
   } /* Endif */


   if (socket_ptr->VALID != SOCKET_VALID) {
      return RTCSERR_SOCK_INVALID;
   } /* Endif */

   return socket_ptr->ERROR_CODE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_setsockerror
* Returned Value  : error code
* Comments  :  Routine is used to set the error code on a socket.
*              Used internally by RTCS.
*
*END*-----------------------------------------------------------------*/

void RTCS_setsockerror
   (
      uint_32     sock,
         /* [IN] socket handle for this socket */
      uint_32     error
   )
{ /* Body */
   register SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)sock;

   socket_ptr->ERROR_CODE = error;

} /* Endbody */

/* Begin CR 2248 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_transfersock
* Returned Value  : socket handle or RTCS_ERROR
* Comments  :  Routine to take ownership of a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  RTCS_transfersock
   (
      uint_32     in_sock,
         /*[IN] specifies the handle of the existing socket */
      _task_id    new_owner
   )
{ /* Body */
   SOCKET_STRUCT_PTR socket_ptr = (SOCKET_STRUCT_PTR)in_sock;
   _rtcs_taskid      old_owner_id;
   _rtcs_taskid      new_owner_id;
   uint_32           out_sock = RTCS_SOCKET_ERROR;

   old_owner_id = RTCS_task_getid();

   if (SOCK_Is_owner(socket_ptr, old_owner_id)) {

      new_owner_id = _task_get_td(new_owner);
      if (new_owner_id) {
         if (old_owner_id==new_owner_id) {
            out_sock=in_sock;
         } else if (SOCK_Add_owner(socket_ptr, new_owner_id)) {
            out_sock=in_sock;
            SOCK_Remove_owner(socket_ptr, old_owner_id);
         } else {
            RTCS_setsockerror(in_sock, RTCSERR_OUT_OF_MEMORY);
         } /* Endif */
      } else {
         RTCS_setsockerror(in_sock, RTCSERR_INVALID_PARAMETER);
      }
   } else {
      RTCS_setsockerror(in_sock, RTCSERR_SOCK_NOT_OWNER);
   } /* Endif */

   return out_sock;
} /* Endbody */
/* End CR 2248 */

/* EOF */
