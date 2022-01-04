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
* $FileName: soselset.c$
* $Version : 3.8.11.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the RTCS_select() implementation.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"

#include "select.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_select
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      in a specified set.
*
*END*-----------------------------------------------------------------*/

uint_32 _RTCS_select
   (
      pointer     readset,
         /* [IN] set of sockets to block on */
      uint_32     read_size,
         /* [IN] size of sockset */

      pointer     writeset,
         /* [IN] set of sockets to block on */
      uint_32     write_size,
         /* [IN] size of sockset */

      uint_32     timeout
         /* [IN] specifies the maximum amount of time to wait for data */
   )
{ /* Body */
   SOCKSELECT_PARM   parms;
   uint_32           error;

   parms.sock_ptr   = readset;
   parms.sock_count = read_size;

   parms.writesock_ptr   = writeset;
   parms.writesock_count = write_size;

   parms.timeout    = timeout;
   error = RTCSCMD_issue(parms, SOCK_select);
   if (error) return RTCS_SOCKET_ERROR;

   return parms.sock;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : _RTCS_selectset
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      in a specified set.  This code copies the sockset, and calls
*      _RTCS_select with the copy.  This is because SOCK_select_signal
*      zeros out the entries that are not active as part of a change
*      for the unix-like RTCS_select.  This corrupted the sockset in
*      legacy code.  Now only the copy is modified, maintaining backward
*      compatibility.
*
*END*-----------------------------------------------------------------*/

uint_32 _RTCS_selectset
   (
      pointer sockset,
      uint_32 size,
      uint_32 timeout
   )
{
   int i;
   int k = 0;
   uint_32 copy_sockset[RTCS_SELECT_MAX_FDS];

   if ((size <= 0) || (size > RTCS_SELECT_MAX_FDS) || (sockset == NULL))
      return (uint_32) RTCS_SELECT_ERROR;

   for (i=0; i<size; i++)
   {
	   if (((uint_32 *)sockset)[i] != 0)
		   copy_sockset[k++] = ((uint_32 *)sockset)[i];
   }

   return (uint_32)_RTCS_select(copy_sockset, k, NULL, 0, timeout);
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select
* Parameters      :
*
*     _rtcs_taskid      owner            not used
*     uint_32 _PTR_     sock_ptr         [IN] array of socket handles
*     uint_32           sock_count       [IN] size of socket array
*     uint_32 _PTR_     writesock_ptr    [IN] array of write socket handles
*     uint_32           writesock_count  [IN] size of write socket array
*     uint_32           timeout          [IN] timeout, 0=forever, -1=none
*     uint_32           sock             [OUT] socket with activity
*
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

void SOCK_select
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   int i, i2;
   uint_32 fds[RTCS_SELECT_MAX_FDS];
   int active = FALSE;

   // param check
   if ((parms->sock_count != 0 && parms->sock_ptr == NULL) || (parms->writesock_count != 0 && parms->writesock_ptr == NULL)) {
      RTCSCMD_complete(parms, RTCSERR_INVALID_PARAMETER);
      return;
   }

   // read fds - check every socket in the supplied array
   if (parms->sock_count) {
      _mem_zero(fds, parms->sock_count * sizeof(uint_32));
      for (i = 0, i2 = 0; i < parms->sock_count; i++) {
         if (SOCK_select_activity((SOCKET_STRUCT_PTR)parms->sock_ptr[i], 0)) {
            fds[i2++] = parms->sock_ptr[i];
         }
      }

      // update socket array - only active sockets
      if (i2) {
         _mem_zero(parms->sock_ptr, parms->sock_count * sizeof(uint_32));
         _mem_copy(fds, parms->sock_ptr, i2 * sizeof(uint_32));
         parms->sock_count = i2;
         parms->sock = fds[0];

         active = TRUE;
      }
   }

   // write fds - check every socket in the supplied array
   if (parms->writesock_count) {
      _mem_zero(fds, parms->writesock_count * sizeof(uint_32));
      for (i = 0, i2 = 0; i < parms->writesock_count; i++) {
         if (SOCK_select_activity((SOCKET_STRUCT_PTR)parms->writesock_ptr[i], 1)) {
            fds[i2++] = parms->writesock_ptr[i];
         }
      }

      // update socket array - only active sockets
      if (i2) {
         _mem_zero(parms->writesock_ptr, parms->writesock_count * sizeof(uint_32));
         _mem_copy(fds, parms->writesock_ptr, i2 * sizeof(uint_32));
         parms->writesock_count = i2;

         if (active == FALSE) {
            // write fds active but read fds no - set read fds to zero
            _mem_zero(parms->sock_ptr, parms->sock_count * sizeof(uint_32));
            parms->sock_count = 0;
            parms->sock = fds[0];
         }

         active = TRUE;
      }
      else if (active == TRUE) {
         // read fds active but write fds no - set write fds to zero
         _mem_zero(parms->writesock_ptr, parms->writesock_count * sizeof(uint_32));
         parms->writesock_count = 0;
      }
   }

   if (active == TRUE) {
      // some fds active
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   }

   if (parms->timeout == (uint_32)-1) {
      parms->sock = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   parms->owner = 0;
   SOCK_select_block(parms);

} /* Endbody */


/* EOF */
