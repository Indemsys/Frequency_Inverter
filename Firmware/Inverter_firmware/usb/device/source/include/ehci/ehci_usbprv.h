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
* $FileName: ehci_usbprv.h$
* $Version : 3.8.4.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*  This file contains the private defines, externs and
*  data structure definitions required by the VUSB_HS Device
*  driver.
*                                                               
*END*********************************************************************/

#ifndef __ehci_dev_usbprv_h__
#define __ehci_dev_usbprv_h__ 1

#include "dev_cnfg.h"
#include "ehci_dev_main.h"

#define  MAX_EP_TR_DESCRS                    (32) 
#define  MAX_XDS_FOR_TR_CALLS                (32) 
#define  ZERO_LENGTH                         (0) 
#define  USB_MAX_ENDPOINTS                   (6) 

#define  USB_MAX_CTRL_PAYLOAD                (64)
 
/* Macro for aligning the EP queue head to 32 byte boundary */
#define USB_MEM32_ALIGN(n)                   ((n) + (-(n) & 31))

/* Macro for aligning the EP queue head to 1024 byte boundary */
#define USB_MEM1024_ALIGN(n)                 ((n) + (-(n) & 1023))

/* Macro for aligning the EP queue head to 1024 byte boundary */
#define USB_MEM2048_ALIGN(n)                 ((n) + (-(n) & 2047))

#define EHCI_DTD_QADD(head,tail,dTD)      \
   if ((head) == NULL) {         \
      (head) = (dTD);            \
   } else {                      \
      (tail)->SCRATCH_PTR->PRIVATE = (void *) (dTD);   \
   } /* Endif */                 \
   (tail) = (dTD);               \
   (dTD)->SCRATCH_PTR->PRIVATE = NULL
   
#define EHCI_DTD_QGET(head,tail,dTD)      \
   (dTD) = (head);               \
   if (head) {                   \
      (head) = (head)->SCRATCH_PTR->PRIVATE;  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

/***************************************
**
** Data structures
**
*/

typedef struct ehci_xd_struct 
{
   struct xd_struct G;

   uint_8         MAX_PKTS_PER_UFRAME;
} EHCI_XD_STRUCT, _PTR_ EHCI_XD_STRUCT_PTR;

/* The USB Device State Structure */
typedef struct _usb_ehci_device_state_struct 
{
   USB_DEV_STATE_STRUCT             G;
   void(_CODE_PTR_                  OLDISR_PTR)(pointer);
   pointer                          OLDISR_DATA;

   pointer                          CAP_BASE_PTR;

   boolean                          BUS_RESETTING;       /* Device is being reset */
   boolean                          TRANSFER_PENDING;    /* Transfer pending ? */

   USBHS_EP_QUEUE_HEAD_STRUCT_PTR   EP_QUEUE_HEAD_PTR;   /* Endpoint Queue head */
   uint_8_ptr                       DRIVER_MEMORY;       /* pointer to driver memory*/
   uint_32                          TOTAL_MEMORY;        /* total memory occupied by driver */
   USBHS_EP_QUEUE_HEAD_STRUCT_PTR   EP_QUEUE_HEAD_BASE;
   USBHS_EP_TR_STRUCT_PTR           DTD_BASE_PTR;         /* Device transfer descriptor pool address */
   USBHS_EP_TR_STRUCT_PTR           DTD_ALIGNED_BASE_PTR; /* Aligned transfer descriptor pool address */
   USBHS_EP_TR_STRUCT_PTR           DTD_HEAD;
   USBHS_EP_TR_STRUCT_PTR           DTD_TAIL;
   USBHS_EP_TR_STRUCT_PTR           EP_DTD_HEADS[USB_MAX_ENDPOINTS * 2];
   USBHS_EP_TR_STRUCT_PTR           EP_DTD_TAILS[USB_MAX_ENDPOINTS * 2];
   uint_16                          DTD_ENTRIES;

} USB_EHCI_DEV_STATE_STRUCT, _PTR_ USB_EHCI_DEV_STATE_STRUCT_PTR;

/***************************************
**
** Prototypes
**
*/
#ifdef __cplusplus
extern "C" {
#endif
extern void _usb_dci_usbhs_isr(pointer);

#ifdef __cplusplus
}
#endif

#endif
