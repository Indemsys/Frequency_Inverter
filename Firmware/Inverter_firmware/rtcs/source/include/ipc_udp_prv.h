#ifndef __ipc_udp_prv_h__
#define __ipc_udp_prv_h__ 1
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
* $FileName: ipc_udp_prv.h$
* $Version : 3.8.2.0$
* $Date    : Oct-11-2011$
*
* Comments:
*
*   This file contains the private definitions for the 
*   inter-processor drivers that work over Ethernet/UDP.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/

#define IPC_UDP_POOL_INIT   16
#define IPC_UDP_POOL_GROW   16 
#define IPC_UDP_POOL_MAX    0

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/


/*
** IPC_UDP_CONTEXT
** This structure contains protocol information for the IPC over Ethernet/UDP
**
*/
typedef struct ipc_udp_context 
{
   /* Pointer to the standard inter-processor initialization structure */
   const IPC_PROTOCOL_INIT_STRUCT *    IPC_INIT_PTR;

   /* Pointer to the structure that contains initialization information 
      for Ethernet/UDP based communication */
   IPC_UDP_INIT_PTR                INIT_PTR;

   /* Message pool ID */
   _pool_id                        POOL_ID;

   /* UDP socket */
   uint_32                         SOCKET;

   /* TaskID of the task that handles outcomming messages */
   _task_id                        OUT_TID;

   /* TaskID of the task that handles incomming messages */
   _task_id                        IN_TID;

   /* Socket Address Structure */
   sockaddr_in                     RADDR;
}  IPC_UDP_CONTEXT, * IPC_UDP_CONTEXT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern boolean IPC_UDP_Init_internal(IPC_UDP_CONTEXT_PTR context_ptr );
extern void IPC_UDP_In( uint_32 param );
extern void IPC_UDP_Out( uint_32 param );
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
