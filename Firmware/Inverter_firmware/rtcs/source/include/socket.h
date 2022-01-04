#ifndef __socket_h__
#define __socket_h__
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
* $FileName: socket.h$
* $Version : 3.5.4.0$
* $Date    : Dec-8-2009$
*
* Comments:
*
*   Constants and prototype declarations which are internal
*   to the SOCKET module.
*
*END************************************************************************/


/*
** Connectionless-mode socket states
*/
#define SOCKSTATE_DGRAM_GROUND      0
#define SOCKSTATE_DGRAM_BOUND       1
#define SOCKSTATE_DGRAM_OPEN        2

/*
** Connection-mode socket states
*/
#define SOCKSTATE_STREAM_GROUND     0
#define SOCKSTATE_STREAM_BOUND      1
#define SOCKSTATE_STREAM_LISTENING  2
#define SOCKSTATE_STREAM_CONNECTED  3


/*
**  socket-specific structures
*/
struct sockselect_parm;
typedef struct socket_config_struct {

   boolean                       INITIALIZED;
   uint_32                       CURRENT_SOCKETS;
   pointer                       SOCKET_HEAD;
   pointer                       SOCKET_TAIL;
   struct sockselect_parm _PTR_  SELECT_HEAD;
   pointer                       SELECT_TIME_HEAD;    /* not used */
   _rtcs_mutex                   SOCK_MUTEX;

} SOCKET_CONFIG_STRUCT, _PTR_ SOCKET_CONFIG_STRUCT_PTR;


/****************************************************
** extern statements for socket procedures          *
*****************************************************/

extern SOCKET_STRUCT_PTR   SOCK_Get_sock_struct
(
   RTCS_SOCKET_CALL_STRUCT_PTR   type,
   _rtcs_taskid                  owner
);
extern void SOCK_Free_sock_struct
(
   SOCKET_STRUCT_PTR          socket_ptr
);

extern boolean SOCK_Remove_owner( SOCKET_STRUCT_PTR, pointer );
extern boolean SOCK_Is_owner( SOCKET_STRUCT_PTR, pointer );
extern boolean SOCK_Add_owner( SOCKET_STRUCT_PTR, pointer );


#endif
/* EOF */
