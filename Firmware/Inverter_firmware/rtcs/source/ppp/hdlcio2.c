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
* $FileName: hdlcio2.c$
* $Version : 3.8.9.0$
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
#include <rtcs.h>
#include <io_pcb.h>
#include <rtcspcb.h>


typedef struct hdlcio2_struct {
   struct _iopcb_table PCB_TABLE;
   HDLCIO_STATS        STATS;
   _io_pcb_pool_id     TX_ID;
   _io_pcb_pool_id     RX_ID;
   _rtcs_part          PART_ID;
   MQX_FILE_PTR        DEVICE;
   void (_CODE_PTR_    UP)(pointer);
   void (_CODE_PTR_    DOWN)(pointer);
   pointer             PARAM;

} HDLCIO2_STRUCT, _PTR_ HDLCIO2_STRUCT_PTR;

#define RTCS_HDLCIO_TX_INIT_PCB    16
#define RTCS_HDLCIO_TX_GROW_PCB    0
#define RTCS_HDLCIO_TX_MAX_PCB     16
#define RTCS_HDLCIO_RX_INIT_PCB    16
#define RTCS_HDLCIO_RX_GROW_PCB    0
#define RTCS_HDLCIO_RX_MAX_PCB     16
#define RTCS_HDCLIO_RX_BUF_SIZE   256

static uint_32 _iopcb_hdlcio_open  (_iopcb_handle, void (_CODE_PTR_)(pointer), void (_CODE_PTR_)(pointer), pointer);
static uint_32 _iopcb_hdlcio_close (_iopcb_handle);
static void    _iopcb_hdlcio_write (_iopcb_handle, PCB_PTR, uint_32);
static PCB_PTR _iopcb_hdlcio_read  (_iopcb_handle, uint_32);
static uint_32 _iopcb_hdlcio_ioctl (_iopcb_handle, uint_32, pointer);


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_free_rx
*  Returned Value :
*  Comments       :
*      Frees the inner PCB and then frees the IO_PCB
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void _iopcb_hdlcio_free_rx
   (
      PCB_PTR    pcb
   )
{ /* Body */
   IO_PCB_STRUCT_PTR       iopcb_ptr = pcb->PRIVATE;

   RTCS_part_free(pcb);
   _io_pcb_free(iopcb_ptr);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_free_tx
*  Returned Value :
*  Comments       :
*      Frees the inner PCB and then frees the IO_PCB
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlcio_free_tx
   (
      IO_PCB_STRUCT_PTR    iopcb
   )
{ /* Body */
   PCB_PTR                 pcb_ptr = iopcb->OWNER_PRIVATE;

   pcb_ptr->FREE(pcb_ptr);
   return _io_pcb_free_internal(iopcb);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_rcv_unblock
*  Returned Value : void
*  Comments       : Unblocks the PPP rx task when a messge is received
*
*
*END*-----------------------------------------------------------------*/

void _iopcb_hdlcio_rcv_unblock
   (
      pointer param
      /* [IN] The semaphore to unblock when the message arrives */
   )
{  /* Body */

#if RTCSCFG_ENABLE_IP4

   LWSEM_STRUCT _PTR_ sem = (LWSEM_STRUCT _PTR_)param;
   PPP_mutex_unlock(sem);

#endif /* RTCSCFG_ENABLE_IP4 */

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_init
*  Returned Value : a HANDLE to be used by other functions
*  Comments       :
*      Sets some of the values of the hdlc structure and
*  returns the handle of the structure.
*
*END*-----------------------------------------------------------------*/

_iopcb_handle _iopcb_hdlcio_init
   (
      /* [IN] The lower level device */
      MQX_FILE_PTR      device
   )
{  /* Body */

#if RTCSCFG_ENABLE_IP4

   HDLCIO2_STRUCT_PTR   hdlcio_ptr;
   int_32               error;

   hdlcio_ptr = RTCS_memalloc(sizeof(HDLCIO2_STRUCT));
   if (!hdlcio_ptr) {
      return NULL;
   } /* Endif */

   /*
   ** Create IO PCB transmit pool
   **
   ** The first parameter (number of frags) is the same number as found in an
   ** RTCSPCB to allow the RTCSPCB->IOPCB conversion, plus one extra to allow 2
   ** address/control bytes to be prepended and one extra to set to LENGTH 0 to
   ** mark end of array. The second parameter is the extra data size needed for
   ** for adddress/control bytes
   */
   hdlcio_ptr->TX_ID = _io_pcb_create_pool(
      RTCSPCB_FRAG_MAX + 2, 2,
      RTCS_HDLCIO_TX_INIT_PCB,
      RTCS_HDLCIO_TX_GROW_PCB,
      RTCS_HDLCIO_TX_MAX_PCB,
      NULL, NULL, NULL, NULL);
   if (hdlcio_ptr->TX_ID == IO_PCB_NULL_POOL_ID) {
      goto errorlevel1;
   } /* Endif */

   /*
   ** Create IO PCB receive_pool
   **
   ** The first parameter (number of frags) is 1, as the lower level receives
   ** everyting as one fragment. No extra data space is needed.
   */
   hdlcio_ptr->RX_ID = _io_pcb_create_pool(
      1, RTCS_HDLC_MESSAGE_SIZE + 2,
      RTCS_HDLCIO_RX_INIT_PCB,
      RTCS_HDLCIO_RX_GROW_PCB,
      RTCS_HDLCIO_RX_MAX_PCB,
      NULL, NULL, NULL, NULL);
   if (hdlcio_ptr->RX_ID == IO_PCB_NULL_POOL_ID) {
      goto errorlevel2;
   } /* Endif */

   /*
   ** Create a partition of PCB + 1 FRAG structs. The FRAG inside the PCB will
   ** point to the data received by the lower level, and the second FRAG outside
   ** the PCB will have a LENGTH of 0 to mark it as the last fragment.
   */
   hdlcio_ptr->PART_ID = RTCS_part_create(
      sizeof(PCB) + sizeof(PCB_FRAGMENT),
      RTCS_HDLCIO_RX_INIT_PCB,
      RTCS_HDLCIO_RX_GROW_PCB,
      RTCS_HDLCIO_RX_MAX_PCB,
      NULL, NULL);
   if (!hdlcio_ptr->PART_ID) {
      goto errorlevel3;
   } /* Endif */

   /* Set the input pool for incoming data */
   error = _io_ioctl(device, IO_PCB_IOCTL_SET_INPUT_POOL, (pointer)&hdlcio_ptr->RX_ID);
   if (error != MQX_OK) {
      goto errorlevel4;
   } /* Endif */

   hdlcio_ptr->DEVICE          = device;
   hdlcio_ptr->PCB_TABLE.OPEN  = _iopcb_hdlcio_open;
   hdlcio_ptr->PCB_TABLE.CLOSE = _iopcb_hdlcio_close;
   hdlcio_ptr->PCB_TABLE.READ  = _iopcb_hdlcio_read;
   hdlcio_ptr->PCB_TABLE.WRITE = _iopcb_hdlcio_write;
   hdlcio_ptr->PCB_TABLE.IOCTL = _iopcb_hdlcio_ioctl;

   /* Everthing OK */
   return &hdlcio_ptr->PCB_TABLE;

   /* Error occured, jump to errorlevel needed to cleanup */
errorlevel4: RTCS_part_destroy(hdlcio_ptr->PART_ID);
errorlevel3: _io_pcb_destroy_pool(hdlcio_ptr->RX_ID);
errorlevel2: _io_pcb_destroy_pool(hdlcio_ptr->TX_ID);
errorlevel1: RTCS_memfree(hdlcio_ptr);

#endif /* RTCSCFG_ENABLE_IP4 */

   return NULL;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_open
*  Returned Value : uint_32, an ERROR code
*  Comments       :
*      Sets some of the values of the hdlc structure and opens the link.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlcio_open
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
   HDLCIO2_STRUCT_PTR hdlcio_ptr = (HDLCIO2_STRUCT_PTR)handle;

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
*  Function Name  : _iopcb_hdlcio_write
*  Returned Value :
*  Comments       :
*      Transmits the data
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void _iopcb_hdlcio_write
   (
      _iopcb_handle      handle,
      /* [IN] - the handle */
      PCB_PTR            pcb_ptr,
      /* [IN] - the packet */
      uint_32            flags
      /* [IN] - the flags */
   )
{ /* Body */
   HDLCIO2_STRUCT_PTR   hdlcio_ptr = (HDLCIO2_STRUCT_PTR)handle;
   IO_PCB_STRUCT_PTR    send_pcb_ptr;
   int_32               error;
   uint_32              i, dlen, j;

   /* Allocate an IO_PCB to send data */
   send_pcb_ptr = _io_pcb_alloc(hdlcio_ptr->TX_ID, FALSE);
   if (send_pcb_ptr == NULL) {
      /* Free the PCB and return */
      hdlcio_ptr->STATS.COMMON.ST_TX_MISSED++;
      PCB_free(pcb_ptr);
      return;
   } /* Endif */

   /* Setup the IO_PCB */
   send_pcb_ptr->OWNER_PRIVATE = pcb_ptr;
   send_pcb_ptr->FREE_PCB_FUNCTION_PTR = _iopcb_hdlcio_free_tx;

   for (i = 0, dlen = 0; i <= RTCSPCB_FRAG_MAX; i++) {
      send_pcb_ptr->FRAGMENTS[i + 1].LENGTH = pcb_ptr->FRAG[i].LENGTH;
      send_pcb_ptr->FRAGMENTS[i + 1].FRAGMENT = pcb_ptr->FRAG[i].FRAGMENT;
      if (!pcb_ptr->FRAG[i].LENGTH) break;
      dlen += pcb_ptr->FRAG[i].LENGTH;
   } /* Endfor */
   send_pcb_ptr->NUMBER_OF_FRAGMENTS = i;

   if (dlen > RTCS_HDLC_MESSAGE_SIZE) {
      hdlcio_ptr->STATS.COMMON.ST_TX_ERRORS++;
      _io_pcb_free(send_pcb_ptr);
      return;
   }/* Endif */

   /* Add address and control bytes */
   send_pcb_ptr->FRAGMENTS[0].LENGTH = 2;
   send_pcb_ptr->FRAGMENTS[0].FRAGMENT[0] = 0xFF;
   send_pcb_ptr->FRAGMENTS[0].FRAGMENT[1] = 0x03;

   i = 0;
   j = 0;
   while (i < send_pcb_ptr->NUMBER_OF_FRAGMENTS) {
      j += send_pcb_ptr->FRAGMENTS[i].LENGTH;
      i++;
   } /* Endwhile */

   /* Write it to the lower level */
   error = _io_pcb_write(hdlcio_ptr->DEVICE, send_pcb_ptr);
   if (error) {
      _io_pcb_free(send_pcb_ptr);
      hdlcio_ptr->STATS.COMMON.ST_TX_ERRORS++;
      return;
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_read
*  Returned Value : PCB_PTR
*  Comments       :
*      Receives the data
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static PCB_PTR _iopcb_hdlcio_read
   (
      _iopcb_handle    handle,
         /* [IN] - the structure handle */
      uint_32          flags
         /* [IN] - flags */
   )
{  /* Body */
   HDLCIO2_STRUCT_PTR   hdlcio_ptr = (HDLCIO2_STRUCT_PTR)handle;
   IO_PCB_STRUCT_PTR    recv_pcb_ptr;
   PCB_FRAGMENT_PTR     frag_ptr;
   PCB_PTR              pcb;
   int_32               error;

   while (TRUE) {
      /* Read from the lower level. Block until we get data */
      error = _io_pcb_read(hdlcio_ptr->DEVICE, &recv_pcb_ptr);
      if (error) {
         _task_block();
         hdlcio_ptr->STATS.COMMON.ST_TX_ERRORS++;
         continue;
      } /* Endif */

      /* Error checking */
      if (recv_pcb_ptr->FRAGMENTS[0].LENGTH > RTCS_HDLC_MESSAGE_SIZE ) {
         hdlcio_ptr->STATS.ST_RX_GIANT++;
         _io_pcb_free(recv_pcb_ptr);
         continue;
      } else if (recv_pcb_ptr->FRAGMENTS[0].LENGTH < 2) {
         hdlcio_ptr->STATS.ST_RX_RUNT++;
         _io_pcb_free(recv_pcb_ptr);
         continue;
      } /* Endif */

      /* Allocate a simple PCB for it */
      pcb = RTCS_part_alloc(hdlcio_ptr->PART_ID);
      if (!pcb) {
         hdlcio_ptr->STATS.COMMON.ST_RX_MISSED++;
         _io_pcb_free(recv_pcb_ptr);
         continue;
      } /* Endif */

      /* All checks passed */
      break;
   } /* Endwhile */

   /* Convert IO_PCB to simple PCB */
   pcb->FREE = _iopcb_hdlcio_free_rx;
   pcb->PRIVATE = recv_pcb_ptr;
   /* Skip address and control bytes */
   frag_ptr = pcb->FRAG;
   frag_ptr->FRAGMENT = recv_pcb_ptr->FRAGMENTS[0].FRAGMENT + 2;
   frag_ptr->LENGTH = recv_pcb_ptr->FRAGMENTS[0].LENGTH - 2;
   frag_ptr++;
   frag_ptr->FRAGMENT = NULL;
   frag_ptr->LENGTH = 0;
   return pcb;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_close
*  Returned Value : uint_32, an ERROR code
*  Comments       :
*     Close link and free memory
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlcio_close
   (
      _iopcb_handle  handle
         /* [IN] - the structure handle */
   )
{  /* Body */
   HDLCIO2_STRUCT_PTR hdlcio_ptr = (HDLCIO2_STRUCT_PTR)((void _PTR_)handle);

   /* close connection */
   if (hdlcio_ptr->DOWN) {
      hdlcio_ptr->DOWN(hdlcio_ptr->PARAM);
   } /* Endif */

   /* Free memory */
   RTCS_part_destroy(hdlcio_ptr->PART_ID);
   _io_pcb_destroy_pool(hdlcio_ptr->TX_ID);
   _io_pcb_destroy_pool(hdlcio_ptr->RX_ID);
   RTCS_memfree(hdlcio_ptr);

   return HDLCIO_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_hdlcio_ioctl
*  Returned Value : an ERROR code
*  Comments       :
*      Store and set the options
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_hdlcio_ioctl
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
