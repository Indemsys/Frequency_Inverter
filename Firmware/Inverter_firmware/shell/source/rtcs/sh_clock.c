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
* $FileName: sh_clock.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"

#define SHELL_CLOCK_SERVER_PRIO        8
#define SHELL_CLOCK_SERVER_STACK       4000
#define SHELL_CLOCK_CHILD_PRIO         7
#define SHELL_CLOCK_CHILD_STACK        4000

#define CREATE_WITH_RTCS 1

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Clock_child_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

static void Clock_child_task
   (
#if CREATE_WITH_RTCS
      pointer  param,
      pointer  creator
#else
   uint_32     sock
#endif
   )

{
#if CREATE_WITH_RTCS
   uint_32     sock = (uint_32) param;
#endif
   int_32      size;
   char        buffer[256];
   TIME_STRUCT time;

#if CREATE_WITH_RTCS
   RTCS_task_resume_creator(creator, RTCS_OK);
#endif

   printf("Clock child: Servicing socket %x\n", sock);
#if USE_RTCS_ATTACH_DETACH
   RTCS_attachsock(sock);
#endif

   while (sock) {
      _time_get(&time);
      size = sprintf(buffer, "\r\nSeconds elapsed = %d.%03d", time.SECONDS, time.MILLISECONDS);
      size++; // account for null byte
      if (size != send(sock, buffer, size, 0)) {
         printf("\nClock child: closing socket %x\n",sock);
         shutdown(sock, FLAG_CLOSE_TX);
         sock=0;
      } else {
         _time_delay(5000);
      }
   }
   printf("Clock child: Terminating\n");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Clock_server_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

static void Clock_server_task
(
   pointer unused1,
   pointer unused2
)
{
   sockaddr_in    laddr, raddr={0};
   uint_32        sock, listensock;
   uint_32        error;
   uint_16        rlen;


   /* Clock server services port 999 */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = 999;
   laddr.sin_addr.s_addr = INADDR_ANY;


   /* Listen on TCP port */
   listensock= socket(PF_INET, SOCK_STREAM, 0);
   if (listensock == RTCS_HANDLE_ERROR) {
      printf("\nCreate stream socket failed");
      _task_block();
   } 
   error = bind(listensock, (const struct sockaddr *)&laddr, sizeof(laddr));
   if (error != RTCS_OK) {
      printf("\nStream bind failed - 0x%lx", error);
      _task_block();
   } 
   error = listen(listensock, 0);
   if (error != RTCS_OK) {
      printf("\nListen failed - 0x%lx", error);
      _task_block();
   } 

   printf("\nClock Server active on port 999\n");

   for (;;) {
      /* Connection requested; accept it */
      rlen = sizeof(raddr);
      printf("Clock server: Waiting on accept\n");
      sock = accept(listensock, (struct sockaddr *)&raddr, &rlen);
      if (sock == RTCS_HANDLE_ERROR) {
         printf("\n\n*** Clock server: Accept failed, error 0x%lx *** \n\n\n",
            RTCS_geterror(listensock));
      } else {
         printf("Clock server: Connection from %ld.%ld.%ld.%ld, port %d, socket %x\n",
            (raddr.sin_addr.s_addr >> 24) & 0xFF,
            (raddr.sin_addr.s_addr >> 16) & 0xFF,
            (raddr.sin_addr.s_addr >>  8) & 0xFF,
             raddr.sin_addr.s_addr        & 0xFF,
             raddr.sin_port, sock);

         /* Create a task to look after it */
         printf("Clock server: detaching socket %x\n",sock);
#if USE_RTCS_ATTACH_DETACH
         RTCS_detachsock(sock);
#endif
         printf("Clock server: spawning child task\n");
         #if CREATE_WITH_RTCS
         RTCS_task_create("Clock_child", SHELL_CLOCK_CHILD_PRIO,
             SHELL_CLOCK_CHILD_STACK, Clock_child_task, (pointer) sock);
         #else
            {
               TASK_TEMPLATE_STRUCT    task_template = {0};
               task_template.TASK_NAME          = "Clock_child";
               task_template.TASK_PRIORITY      = SHELL_CLOCK_CHILD_PRIO;
               task_template.TASK_STACKSIZE     = SHELL_CLOCK_CHILD_STACK;
               task_template.TASK_ADDRESS       = Clock_child_task;
               task_template.CREATION_PARAMETER = (uint_32)sock;
               if (_task_create(0, 0, (uint_32)&task_template) == MQX_NULL_TASK_ID) {
                  printf("Clock server: failed to spawn child task\n");
               } 
            }
         #endif
      } 
   }
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_Clock_server_start
*  Returned Value:  none
*  Comments  :  SHELL utility to start clock server
*  Usage:  Clock_server 
*
*END*-----------------------------------------------------------------*/

int_32  Shell_Clock_server_start(int_32 argc, char_ptr argv[] )
{
   uint_32  result;
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 1)  {
         result =RTCS_task_create("Clock_server", SHELL_CLOCK_SERVER_PRIO, 
            SHELL_CLOCK_SERVER_STACK, Clock_server_task, NULL);
         if (result ==  0)  {
            printf("Clock Server Started.\n");
         } else  {
            printf("Unable to start Clock Server, error = 0x%x\n",result);
            return_code = SHELL_EXIT_ERROR;
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s \n", argv[0]);
      } else  {
         printf("Usage: %s\n",argv[0]);
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */

/* EOF */
