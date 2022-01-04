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
* $FileName: ipcrtprv.h$
* $Version : 3.0.4.0$
* $Date    : Oct-5-2011$
*
* Comments:
*
*   This file contains the definitions private to the router used to route
*   IPC messages.
*
*END************************************************************************/
#ifndef __ipcrtprv_h__
#define __ipcrtprv_h__ 1

/*--------------------------------------------------------------------------*/
/*
 *                          CONSTANT DECLARATIONS
 */

/*--------------------------------------------------------------------------*/
/*
 *                    TYPEDEFS FOR _CODE_PTR_ FUNCTIONS
 */ 
typedef boolean (_CODE_PTR_ IPC_MSGROUTER_FPTR)( _processor_number, pointer, boolean);
 
/*--------------------------------------------------------------------------*/
/*
 *                          DATATYPE DECLARATIONS
 */

/* IPC MSG ROUTING STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This structure contains info for a particular route.
 */
typedef struct ipc_msg_routing_struct
{

   /*! \brief Used to link all routing structures together. */
   QUEUE_ELEMENT_STRUCT  LINK;

   /*! \brief The minimum processor number in the range. */
   _processor_number     MIN_PROC_NUMBER;

   /*! \brief The maximum processor number in the range. */
   _processor_number     MAX_PROC_NUMBER;

   /*! \brief The queue to use if the processor number is in the above range. */
   _queue_number         QUEUE;

} IPC_MSG_ROUTING_STRUCT, _PTR_ IPC_MSG_ROUTING_STRUCT_PTR;
/*! \endcond */


/* IPC MSG ROUTING COMPONENT STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief The structure kept in the kernel data by the message routing component.
 */
typedef struct ipc_msg_routing_component_struct 
{
   /*! \brief Linked list of routes installed. */
   QUEUE_STRUCT         ROUTING_LIST;
   /*! \brief Message router function. */
   IPC_MSGROUTER_FPTR 	MSG_ROUTER;
   
} IPC_MSG_ROUTING_COMPONENT_STRUCT, _PTR_ IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/*
 *                          C PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _ipc_msg_route_init_internal( const IPC_ROUTING_STRUCT * route_ptr );
/*! \private */
extern _mqx_uint _ipc_msg_route_internal(_processor_number, pointer, boolean);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
