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
* $FileName: hdlcio.c$
* $Version : 3.8.8.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the HDLC device functions needed
*   for communication.
*
*END************************************************************************/

#include <rtcshdlc.h>
#include <rtcs_in.h>


static uint_32 _iopcb_hdlc_open  (_iopcb_handle, void (_CODE_PTR_)(pointer), void (_CODE_PTR_)(pointer), pointer);
static uint_32 _iopcb_hdlc_close (_iopcb_handle);
static void    _iopcb_hdlc_write (_iopcb_handle, PCB_PTR, uint_32);
static PCB_PTR _iopcb_hdlc_read  (_iopcb_handle, uint_32);
static uint_32 _iopcb_hdlc_ioctl (_iopcb_handle, uint_32, pointer);


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_rcv_unblock
*  Returned Value : void
*  Comments       : Unblocks the PPP rx task when a messge is received
*
*
*END*-----------------------------------------------------------------*/

void _iopcb_hdlc_rcv_unblock
   (
   pointer param
   /* [IN] The semaphore to unblock when the message arrives */
   )
{  /* Body */

#if RTCSCFG_ENABLE_IP4

   LWSEM_STRUCT _PTR_ sem = (LWSEM_STRUCT _PTR_)param;
   PPP_mutex_unlock(sem);
   
#endif /* RTCSCFG_ENABLE_IP4  */

}  /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_init
*  Returned Value : a HANDLE to be used by other functions
*  Comments       :
*      Sets some of the values of the ppphdlc structure and
*  returns the handle of the structure.
*
*END*-----------------------------------------------------------------*/

_iopcb_handle _iopcb_hdlc_init
   (
   uint_16       channel_queue,
   /* [IN] - the HDLC message queue for this channel */
   _RTCS_poolid  pool_id,
   /* [IN] - the HDLC message pool */
   _RTCS_queueid channel_qid,
   /* [IN] - the HDLC queue id */
   _ppp_mutex _PTR_ mutex_ptr
   )
{  /* Body */

#if RTCSCFG_ENABLE_IP4

   HDLCIO_STRUCT_PTR hdlcio_ptr;
   _rtcs_part        part_id;

   hdlcio_ptr = RTCS_memalloc(sizeof(HDLCIO_STRUCT));
   if (!hdlcio_ptr) {
      return NULL;
   } /* Endif */
   part_id = RTCS_part_create(sizeof(PCB)
                            + sizeof(PCB_FRAGMENT)
                            + RTCS_HDLC_MESSAGE_SIZE,
                              8, 0, 8, NULL, NULL);

   hdlcio_ptr->PART_ID         = part_id;
   hdlcio_ptr->MUTEX_PTR       = mutex_ptr;
   hdlcio_ptr->POOL_ID         = pool_id;
   hdlcio_ptr->QID             = channel_qid;
   hdlcio_ptr->CHANNEL_Q       = channel_queue;
   hdlcio_ptr->PCB_TABLE.OPEN  = _iopcb_hdlc_open;
   hdlcio_ptr->PCB_TABLE.CLOSE = _iopcb_hdlc_close;
   hdlcio_ptr->PCB_TABLE.READ  = _iopcb_hdlc_read;
   hdlcio_ptr->PCB_TABLE.WRITE = _iopcb_hdlc_write;
   hdlcio_ptr->PCB_TABLE.IOCTL = _iopcb_hdlc_ioctl;

   return &hdlcio_ptr->PCB_TABLE;
   
#else   

    return FALSE;

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_open
*  Returned Value : uint_32, an ERROR code
*  Comments       :
*      Sets some of the values of the ppphdlc structure and opens the link.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlc_open
   (
      _iopcb_handle       handle,
         /* [IN] - the IO handle*/
      void (_CODE_PTR_    up)(pointer),
         /* [IN] - function to put the link up */
      void (_CODE_PTR_    down)(pointer),
         /* [IN] - function to put the link down */
      pointer             param
         /* [IN] - for the up/down functions */
   )
{  /* Body */
   HDLCIO_STRUCT_PTR hdlcio_ptr = (HDLCIO_STRUCT_PTR)((void _PTR_)handle);
   hdlcio_ptr->UP      = up;
   hdlcio_ptr->DOWN    = down;
   hdlcio_ptr->PARAM   = param;

   /* signal the PPP layer that the connection is open */
   if (hdlcio_ptr->UP) {
      up(param);
   } /* Endif */

   return HDLCIO_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_write
*  Returned Value :
*  Comments       :
*      Transmits the data
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void _iopcb_hdlc_write
   (
   _iopcb_handle      handle,
   /* [IN] - the handle */
   PCB_PTR            pcb_ptr,
   /* [IN] - the packet */
   uint_32            flags
   /* [IN] - the flags */
   )
{ /* Body */
   RTCS_HDLC_MESSAGE_STRUCT_PTR msg_ptr;
   HDLCIO_STRUCT_PTR            hdlcio_ptr = (HDLCIO_STRUCT_PTR)((void _PTR_)handle);
   uint_32                      data_length = 0;
   uint_32                      i = 0;
   uint_32                      j = 0;
   _RTCS_queueid                qid;

   while (pcb_ptr->FRAG[i].LENGTH && (data_length <= RTCS_HDLC_MESSAGE_SIZE)) {
      data_length += pcb_ptr->FRAG[i].LENGTH;
      i++;
   } /* Endwhile */
   i = 0;

   if ( data_length > RTCS_HDLC_MESSAGE_SIZE ) {
      PCB_free(pcb_ptr);
      return;
   }/* Endif */

   hdlcio_ptr->STATS.COMMON.ST_TX_TOTAL++;
   msg_ptr = (RTCS_HDLC_MESSAGE_STRUCT_PTR)RTCS_msg_alloc(hdlcio_ptr->POOL_ID);
   if (msg_ptr == NULL) {
      /* Free the PCB and return */
      hdlcio_ptr->STATS.COMMON.ST_TX_MISSED++;
      PCB_free(pcb_ptr);
      return;
   } /* Endif */

   /* assemble message to send */
   qid = RTCS_msgq_get_id(0, hdlcio_ptr->CHANNEL_Q);
   msg_ptr->HDLC_HEADER.HEADER.TARGET_QID = qid;
   msg_ptr->HDLC_HEADER.HEADER.SOURCE_QID = hdlcio_ptr->QID;
   msg_ptr->HDLC_HEADER.HEADER.SIZE = sizeof(RTCS_HDLC_MESSAGE_STRUCT)
      + data_length + 2;
   msg_ptr->HDLC_HEADER.PACKET_SIZE = data_length + 2;

   /* Add address and control bytes */
   msg_ptr->DATA[0] = 0xFF;
   msg_ptr->DATA[1] = 0x03;
   j+=2;



   while (pcb_ptr->FRAG[i].LENGTH) {
      RTCS_memcopy(pcb_ptr->FRAG[i].FRAGMENT, &msg_ptr->DATA[j],
         pcb_ptr->FRAG[i].LENGTH);
      j += pcb_ptr->FRAG[i].LENGTH;
      i++;
   } /* Endwhile */

   PCB_free(pcb_ptr);

   if (!RTCS_msgq_send(msg_ptr, hdlcio_ptr->POOL_ID)) {
      hdlcio_ptr->STATS.COMMON.ST_TX_MISSED++;
   } /* Endif */ ;

} /* Endbody */


#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_read
*  Returned Value : PCB_PTR
*  Comments       :
*      Receives the data
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static PCB_PTR _iopcb_hdlc_read
   (
      _iopcb_handle    handle,
         /* [IN] - the structure handle */
      uint_32          flags
         /* [IN] - flags */
   )
{  /* Body */

   RTCS_HDLC_MESSAGE_STRUCT_PTR msg_ptr;
   HDLCIO_STRUCT_PTR            hdlcio_ptr = (HDLCIO_STRUCT_PTR)((void _PTR_)handle);
   PCB_FRAGMENT _PTR_           pcb_frag_ptr;
   uint_32                      data_length;
   PCB_PTR                      pcb;

   while (TRUE) {
      PPP_mutex_lock(hdlcio_ptr->MUTEX_PTR);
      msg_ptr = RTCS_msgq_receive(hdlcio_ptr->QID, 0, hdlcio_ptr->POOL_ID);
      hdlcio_ptr->STATS.COMMON.ST_RX_TOTAL++;
      if ( msg_ptr == NULL ) {
         hdlcio_ptr->STATS.COMMON.ST_RX_ERRORS++;
         RTCS_time_delay(1);
      } else {
         data_length = msg_ptr->HDLC_HEADER.PACKET_SIZE;
         if (data_length > RTCS_HDLC_MESSAGE_SIZE ) {
            hdlcio_ptr->STATS.ST_RX_GIANT++;
            RTCS_msg_free(msg_ptr);
            continue;
         } else if (data_length < 2) {
            hdlcio_ptr->STATS.ST_RX_RUNT++;
            RTCS_msg_free(msg_ptr);
            continue;
         } else {
            pcb = RTCS_part_alloc(hdlcio_ptr->PART_ID);
            if (!pcb) {
               hdlcio_ptr->STATS.COMMON.ST_RX_MISSED++;
               RTCS_msg_free(msg_ptr);
               continue;
            }/* Endif */
            break;
         } /* Endif */
      } /* Endif */
   } /* Endwhile */

   pcb->FREE = (void (_CODE_PTR_)(PCB_PTR))RTCS_part_free;
   pcb->PRIVATE = pcb;
   pcb_frag_ptr = pcb->FRAG;
   pcb_frag_ptr->FRAGMENT = (uchar_ptr)pcb + sizeof(PCB) + sizeof(PCB_FRAGMENT);
   pcb_frag_ptr->LENGTH = data_length - 2;
   pcb_frag_ptr++;
   pcb_frag_ptr->LENGTH = 0;
   pcb_frag_ptr->FRAGMENT = NULL;
   RTCS_memcopy(msg_ptr->DATA+2, pcb->FRAG[0].FRAGMENT, data_length - 2);
   RTCS_msg_free(msg_ptr);
   return pcb;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_close
*  Returned Value : uint_32, an ERROR code
*  Comments       :
*     Close link and free memory
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlc_close
   (
      _iopcb_handle  handle
         /* [IN] - the structure handle */
   )
{  /* Body */
   HDLCIO_STRUCT_PTR hdlcio_ptr = (HDLCIO_STRUCT_PTR)((void _PTR_)handle);

   /* close connection */
   if (hdlcio_ptr->DOWN) {
      hdlcio_ptr->DOWN(hdlcio_ptr->PARAM);
   } /* Endif */

   RTCS_memfree(hdlcio_ptr);

   return HDLCIO_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlc_ioctl
*  Returned Value : an ERROR code
*  Comments       :
*      Store and set the options
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlc_ioctl
   (
      _iopcb_handle    handle,
         /* [IN] - the structure handle */
      uint_32          option,
         /* [IN] - options */
      pointer          value
         /* [IN] - option value */
   )
{  /* Body */
   uint_32 result = HDLCIO_OK;

   switch(option)  {
      case IOPCB_IOCTL_S_ACCM:
         break;
      case IOPCB_IOCTL_R_ACCM:
         break;
      case IOPCB_IOCTL_S_PFC:
         break;
      case IOPCB_IOCTL_R_PFC:
         break;
      case IOPCB_IOCTL_S_ACFC:
         break;
      case IOPCB_IOCTL_R_ACFC:
         break;
      case IOPCB_IOCTL_GET_IFTYPE:
         *(uint_32_ptr)value = IPIFTYPE_HDLC;
          break;
      default:
         result = IO_ERROR_INVALID_IOCTL_CMD;
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
