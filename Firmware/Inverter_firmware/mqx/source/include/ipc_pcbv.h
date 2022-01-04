/*HEADER*********************************************************************
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
* $FileName: ipc_pcbv.h$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the private definitions for the IO PCB
*   interprocessor drivers.
*
*END************************************************************************/
#ifndef __ipc_pcbv_h__
#define __ipc_pcbv_h__ 1

/*--------------------------------------------------------------------------*/
/*
 *                          CONSTANT DECLARATIONS
 */

/* Stack sizes */
#define IPC_PCB_STACK_SIZE IPC_DEFAULT_STACK_SIZE

/*--------------------------------------------------------------------------*/
/*
 *                          DATATYPE DECLARATIONS
 */



/* IPC_PCB_INFO_STRUCT */
/*!
 * \brief This structure contains protocol information for the IPC over PCBs.
 */
typedef struct ipc_pcb_info_struct
{
   /*! \brief Queue headers for keeping track of driver. */
   QUEUE_ELEMENT_STRUCT QUEUE;

   /*! \brief The IO PCB device to use. */
   MQX_FILE_PTR         FD;

   /*! \brief PCB input pool. */
   _io_pcb_pool_id      PCB_INPUT_POOL;
   /*! \brief Message input pool. */
   _pool_id             MSG_INPUT_POOL;
 
   /*! \brief PCB output pool. */
   _io_pcb_pool_id      PCB_OUTPUT_POOL;
   /*! \brief Message output pool. */
   _queue_id            OUT_MSG_QID;
   
   /*! \brief Number of sent messages. */
   _mqx_uint            OUTPUT_MESSAGE_COUNT;
   /*! \brief Number of received messages. */
   _mqx_uint            INPUT_MESSAGE_COUNT;

} IPC_PCB_INFO_STRUCT, _PTR_ IPC_PCB_INFO_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
 *                          C PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _ipc_pcb_free(IO_PCB_STRUCT_PTR);
extern IO_PCB_STRUCT_PTR _ipc_pcb_alloc(IO_PCB_STRUCT_PTR, pointer);
extern void _ipc_pcb_output_notification(pointer);
extern void _ipc_pcb_input_notification(MQX_FILE_PTR, IO_PCB_STRUCT_PTR);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
