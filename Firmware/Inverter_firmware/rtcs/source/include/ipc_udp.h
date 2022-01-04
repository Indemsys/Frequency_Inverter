#ifndef __ipc_udp_h__
#define __ipc_udp_h__ 1
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved                       
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
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
* $FileName: ipc_udp.h$
* $Version : 3.8.2.0$
* $Date    : Oct-11-2011$
*
* Comments:
*
*   This file contains the definitions for the 
*   interprocessor drivers that work over Ethernet/UDP.
*
*END************************************************************************/

#include <rtcs.h>

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/

/*
** IPC_UDP_INIT
** This structure contains the initialization information for Ethernet/UDP
** based communication
**
*/
typedef struct ipc_udp_init
{
   /* 
   ** The following are parameters used to create two tasks of the
   ** IPC/UDP driver for handling incomming and outcomming messages
   */
   char_ptr    NAME;
   uint_32     STACK_SIZE;
   uint_32     PRIORITY;

   /* Maximum size of the message queue created by the IPC/UDP driver */
   uint_32     MAX_MSG_SIZE;

   /* UDP port number for receiving */
   uint_32     SOURCE_PORT;

   /* UDP port number for sending */
   uint_32     DEST_PORT;

   /* IP address of the device the IPC/UDP driver sends messages to */
   _ip_address DEST;
} IPC_UDP_INIT, * IPC_UDP_INIT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint IPC_UDP_Init( const IPC_PROTOCOL_INIT_STRUCT * ipc_init_ptr, pointer ipc_info_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
