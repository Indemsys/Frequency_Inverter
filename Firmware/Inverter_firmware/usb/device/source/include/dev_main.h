#ifndef __dev_main_h__
#define __dev_main_h__ 1
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: dev_main.h$
* $Version : 3.8.1.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*  This file contains the declarations specific to the USB Device API
*
*END*********************************************************************/
#include "mqx_dev.h"

typedef struct xd_struct 
{
   uchar          EP_NUM;           /* Endpoint number */
   uchar          BDIRECTION;       /* Direction : Send/Receive */
   uint_8         EP_TYPE;          /* Type of the endpoint: Ctrl, Isoch, Bulk, Int */
   uchar          BSTATUS;          /* Current transfer status */
   uint_8_ptr     WSTARTADDRESS;    /* Address of first byte */
   uint_32        WTOTALLENGTH;     /* Number of bytes to send/recv */
   uint_32        WSOFAR;           /* Number of bytes recv'd so far */
   uint_16        WMAXPACKETSIZE;   /* Max Packet size */
   boolean        DONT_ZERO_TERMINATE;
   SCRATCH_STRUCT_PTR SCRATCH_PTR;
} XD_STRUCT, _PTR_ XD_STRUCT_PTR;

typedef struct usb_dev_state_struct {
   pointer                          INIT_PARAM;          /* Device controller init parameters */
   pointer                          DEV_PTR;             /* Device Controller Register base address */
   const USB_DEV_CALLBACK_FUNCTIONS_STRUCT * CALLBACK_STRUCT_PTR;
   SCRATCH_STRUCT_PTR               XD_SCRATCH_STRUCT_BASE;
   SCRATCH_STRUCT_PTR               SCRATCH_STRUCT_BASE;
   
   SERVICE_STRUCT_PTR               SERVICE_HEAD_PTR;    /* Head struct address of registered services */
   struct xd_struct               * TEMP_XD_PTR;         /* Temp xd for ep init */
   struct xd_struct               * XD_BASE;
   struct xd_struct               * XD_HEAD;             /* Head Transaction descriptors */
   struct xd_struct               * XD_TAIL;             /* Tail Transaction descriptors */
   struct xd_struct               * PENDING_XD_PTR;      /* pending transfer */
   uint_32                          XD_ENTRIES;

   /* These fields are kept only for USB_shutdown() */
   uint_16                          USB_STATE;
   uint_16                          USB_DEVICE_STATE;
   uint_16                          USB_SOF_COUNT;
   uint_16                          ERRORS;
   uint_16                          USB_DEV_STATE_B4_SUSPEND;
   uint_8                           DEV_NUM;             /* Deprecated, avoid usage; USB device number on the board */
   uint_8                           DEV_VEC;             /* Interrupt vector number for USB OTG */
   uint_8                           SPEED;               /* Low Speed, High Speed, Full Speed */
   uint_8                           MAX_ENDPOINTS;       /* Max endpoints supported by this device */
                                                         
   uint_16                          USB_CURR_CONFIG;                                                         
   uint_8                           DEVICE_ADDRESS;
} USB_DEV_STATE_STRUCT, _PTR_ USB_DEV_STATE_STRUCT_PTR;

#define USB_XD_QADD(head,tail,XD)      \
   if ((head) == NULL) {         \
      (head) = (XD);            \
   } else {                      \
      (tail)->SCRATCH_PTR->PRIVATE = (XD);   \
   } /* Endif */                 \
   (tail) = (XD);               \
   (XD)->SCRATCH_PTR->PRIVATE = NULL
   
#define USB_XD_QGET(head,tail,XD)      \
   (XD) = (head);               \
   if (head) {                   \
      (head) = (XD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#ifdef __cplusplus
{
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
