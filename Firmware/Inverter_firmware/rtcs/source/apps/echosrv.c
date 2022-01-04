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
* $FileName: echosrv.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains an implementation of an
*   RFC 862 Echo server.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

#define BUFFER_SIZE  1500
static char buffer[BUFFER_SIZE];

/* Start CR 1658C */
typedef struct echo_context  {
   uint_32     UDPSOCK;
   uint_32     LISTENSOCK;
} ECHOSRV_CONTEXT, _PTR_ ECHOSRV_CONTEXT_PTR;

_rtcs_taskid ECHOSRV_task_id = 0;

#ifdef __MQX__ 
void ECHOSRV_Exit_handler(void);
#endif
/* End CR 1658C */

void ECHOSRV_task(pointer, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_init
* Returned Value   : error code
* Comments  :  Start the port 7 echo server.
*
*END*-----------------------------------------------------------------*/

uint_32 ECHOSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   /* Start CR 1658C */
   if (ECHOSRV_task_id == 0)  {
      return RTCS_task_create(name, priority, stacksize, ECHOSRV_task, NULL);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
   /* End CR 1658C */
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHO_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHO_task
   (
      uint_32 dummy
   )
{ /* Body */
   ECHOSRV_task(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHOSRV_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHOSRV_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint_32        sock, listensock;
   uint_32        error;
   uint_32        optval, optlen;
   int_32         length;
   uint_16        rlen;
   /* Start CR 1658C */
   ECHOSRV_CONTEXT   echo_context =  { 0 };

   ECHOSRV_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if the Echo Server is terminated */
   _task_set_environment( _task_get_id(), (pointer) &echo_context );
   _task_set_exit_handler( _task_get_id(), ECHOSRV_Exit_handler );
#endif 
   
   /* Echo server services port 7 */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_ECHO;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   echo_context.UDPSOCK = socket(PF_INET, SOCK_DGRAM, 0);
   if (echo_context.UDPSOCK == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(echo_context.UDPSOCK, (const sockaddr *)&laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   echo_context.LISTENSOCK = socket(PF_INET, SOCK_STREAM, 0);
   listensock = echo_context.LISTENSOCK;
   if (listensock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(listensock, (const sockaddr *)&laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(listensock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   /* End CR 1658C */

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock,(sockaddr *)&raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char_ptr)&optval, &optlen);

         if (optval == SOCK_STREAM) {

            length = recv(sock, buffer, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
               shutdown(sock, FLAG_CLOSE_TX);
            } else {
               send(sock, buffer, length, 0);
            } /* Endif */

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr *)&raddr, &rlen);
            sendto(sock, buffer, length, 0, (sockaddr *)&raddr, rlen);

         } /* Endif */

      } /* Endif */

   } /* Endfor */

} /* Endbody */

/* Start CR 1658C */
#ifdef __MQX__ 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_stop
* Returned Value   : error code
* Comments  :  Start the ECHO task.
*
*END*-----------------------------------------------------------------*/

uint_32 ECHOSRV_stop( void )
{ /* Body */
   if (ECHOSRV_task_id == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(ECHOSRV_task_id);
   ECHOSRV_task_id = 0;
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_Exit_handler
* Returned Value   : error code
* Comments  :  Start the ECHO task.
*
*END*-----------------------------------------------------------------*/

void ECHOSRV_Exit_handler( void )
{ /* Body */
   ECHOSRV_CONTEXT_PTR    echo_context_ptr;
   
   echo_context_ptr = (ECHOSRV_CONTEXT_PTR) _task_get_environment( _task_get_id() );
   if (echo_context_ptr != NULL)  {
      if (echo_context_ptr->UDPSOCK)  {
         shutdown(echo_context_ptr->UDPSOCK, 0);
         echo_context_ptr->UDPSOCK = 0;   
      }   
      if (echo_context_ptr->LISTENSOCK)  {
         shutdown(echo_context_ptr->LISTENSOCK, FLAG_ABORT_CONNECTION);   
         echo_context_ptr->LISTENSOCK = 0;   
      }   
   }
   ECHOSRV_task_id = 0;
} /* Endbody */
#endif
/* End CR 1658 */

#endif
/* EOF */
