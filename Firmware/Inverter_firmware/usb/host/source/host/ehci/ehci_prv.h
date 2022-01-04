#ifndef __ehci_prv_h__
#define __ehci_prv_h__ 1
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
* $FileName: ehci_prv.h$
* $Version : 3.8.17.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the internal USB Host specific type definitions
*
*END************************************************************************/

#include "hostapi.h"
#include "host_cnfg.h"

#include "ehci_cache.h"
#include "ehci.h"


#define  HUB_LS_SETUP                        (333)

/***************************************
**
** Code macros
**
*/

/* Macro for alignment to specific byte boundary */
#define  USB_MEM32_ALIGN(n)                  ((n) + (-(n) & 31))
#define  USB_MEM512_ALIGN(n)                 ((n) + (-(n) & 511))
#define  USB_MEM4096_ALIGN(n)                ((n) + (-(n) & 4095))

#define  BitStuffTime(x)                     (7* 8 * x / 6)

typedef struct ehci_tr_struct
{
   TR_STRUCT                     G;  /* generic transfer fields */
  /* number of ITds/SITDs used for this transfer */
   uint_32                       no_of_itds_sitds;
   uint_32                       SOFAR; /* number of bytes sent/recv'd so far */

   /****************************************************
   Data cache requirement is that any buffer that stores
   the data should be cache aligned. This means that the
   setup packet buffer must be cache aligned too. Structure
   fields above this have occupied 28 + 16 = 44 bytes.
   We add a 20 bytes padding to makesure that it is aligned
   on a 32 byte boundry since 44 + 20 = 64; We also
   add a padding at the end to ensure that we don't corrupt
   someother memory when we flush the setup packet
   from cache.
   ****************************************************/
#if PSP_HAS_DATA_CACHE  // TODO check align
    uint_8                       RESERVED1[USB_CACHE_ALIGN(40) - 40];
#endif // PSP_HAS_DATA_CACHE
   
#if PSP_HAS_DATA_CACHE  // TODO check align
    uint_8               RESERVED2[USB_CACHE_ALIGN(8) - 8];
#endif
} EHCI_TR_STRUCT, _PTR_ EHCI_TR_STRUCT_PTR;
    
typedef struct ehci_pipe_struct
{
   PIPE_STRUCT          G;

   uint_32              CURRENT_NAK_COUNT;
   pointer              QH_FOR_THIS_PIPE; /* Queue head for this pipe */

   /* Special fields defined for periodic transfers */
   uint_32              START_FRAME;    /*frame number from which this pipe is started to schedule*/
   uint_32              START_UFRAME;   /*Micro frame number from which this pipe is started to schedule*/
   uint_32              NO_OF_START_SPLIT; /* store the number of start splits (used for ISO OUT )*/
   uint_8               START_SPLIT;    /* store the start split slots for full speed devices*/
   uint_8               COMPLETE_SPLIT; /* store the complete split slots for full speed devices*/
   uint_16              BWIDTH;         /* time take by this pipe (for periodic pipes only */
   uint_8               BWIDTH_SLOTS[8];/*micro frame slots budgeted for transaction */
   
   /* 84 bytes so far add padding to ensure cache alignment*/
#if PSP_HAS_DATA_CACHE  // TODO check align
   uint_8               reserved[USB_CACHE_ALIGN(74) - 74]; 
#endif  
    
} EHCI_PIPE_STRUCT, _PTR_ EHCI_PIPE_STRUCT_PTR;

/* Queue head management data structure */
typedef struct active_qh_mgmt_struct {
   EHCI_QH_STRUCT_PTR                  QH_PTR;
   EHCI_QTD_STRUCT_PTR                 FIRST_QTD_PTR;
   int_32                              TIME;
   struct active_qh_mgmt_struct _PTR_  NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
} ACTIVE_QH_MGMT_STRUCT, _PTR_ ACTIVE_QH_MGMT_STRUCT_PTR;

/* ITD,SITD list management data structure (doubly link list )*/
typedef struct list_node_struct {
   struct list_node_struct _PTR_  next;        /* next member in the list */
   struct list_node_struct _PTR_  prev;        /* previous member in the list */
   boolean   next_active; /* is next node a active node */
   pointer   member;      /* pointer to the currently active ITD or SITD*/
} LIST_NODE_STRUCT, _PTR_ LIST_NODE_STRUCT_PTR;


/* Class Callback function storage structure */
typedef struct class_service_struct {
   uint_8               CLASS_TYPE;
   uint_8               SUB_CLASS;
   uint_8               PROTOCOL;
   /* index of the instance */
   uint_8               INDEX;
   /* identification string unique to this type */
   char_ptr             CLASS_ID_STRING;
   /* class handle will be NULL if it is not initialized */
   pointer              CLASS_HANDLE;
   pointer (_CODE_PTR_  INIT)(pointer, uint_32);
   void    (_CODE_PTR_  DEINIT)(pointer);
   void    (_CODE_PTR_  CALL_BACK_INIT)(pointer, char_ptr);
   void    (_CODE_PTR_  CALL_BACK_REMOVE)(pointer);
   struct class_service_struct _PTR_ NEXT_INSTANCE;
   struct class_service_struct _PTR_ NEXT;
} USB_HOST_CLASS_DRIVER_STRUCT, _PTR_ USB_HOST_CLASS_DRIVER_STRUCT_PTR;

typedef struct  usb_ehci_host_state_structure {
   USB_HOST_STATE_STRUCT               G; /* generic host state */
   uint_32                             FRAME_LIST_SIZE;
   uint_8_ptr                          PERIODIC_FRAME_LIST_BW_PTR;
   pointer                             PERIODIC_LIST_BASE_ADDR;
   pointer                             ALIGNED_PERIODIC_LIST_BASE_ADDR;
   EHCI_QH_STRUCT_PTR                  ASYNC_LIST_BASE_ADDR;
   EHCI_QH_STRUCT_PTR                  ALIGNED_ASYNCLIST_ADDR;
   EHCI_QH_STRUCT_PTR                  QH_BASE_PTR; 
   EHCI_QH_STRUCT_PTR                  QH_ALIGNED_BASE_PTR; 
   uint_32                             QH_ENTRIES;
   EHCI_QH_STRUCT_PTR                  QH_HEAD;
   EHCI_QH_STRUCT_PTR                  QH_TAIL;
   ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_ASYNC_LIST_PTR;
   ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
   EHCI_QTD_STRUCT_PTR                 QTD_BASE_PTR;
   EHCI_QTD_STRUCT_PTR                 QTD_HEAD;
   EHCI_QTD_STRUCT_PTR                 QTD_TAIL;
   EHCI_QTD_STRUCT_PTR                 QTD_ALIGNED_BASE_PTR;
   uint_32                             QTD_ENTRIES;
   boolean                             HIGH_SPEED_ISO_QUEUE_ACTIVE; 
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_BASE_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_ALIGNED_BASE_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_HEAD;
   EHCI_ITD_STRUCT_PTR                 ITD_TAIL;
   uint_32                             ITD_ENTRIES;
   boolean                             FULL_SPEED_ISO_QUEUE_ACTIVE; 
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_BASE_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_ALIGNED_BASE_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_HEAD;
   EHCI_SITD_STRUCT_PTR                SITD_TAIL;
   uint_32                             SITD_ENTRIES;
   uint_32                             RESET_RECOVERY_TIMER;
   uint_32                             PORT_NUM;
   uint_8                              UFRAME_COUNT;
   pointer                             CONTROLLER_MEMORY;
   boolean                             PERIODIC_LIST_INITIALIZED;
   boolean                             ITD_LIST_INITIALIZED;
   boolean                             SITD_LIST_INITIALIZED;
   boolean                             IS_RESETTING;
} USB_EHCI_HOST_STATE_STRUCT, _PTR_ USB_EHCI_HOST_STATE_STRUCT_PTR;

#define EHCI_GET_TYPE(data_struct_ptr) \
   (EHCI_MEM_READ(*((USB_Uncached uint_32 _PTR_)data_struct_ptr)) & EHCI_ELEMENT_TYPE_MASK)

#define EHCI_ITD_QADD(head, tail, ITD)      \
   if ((head) == NULL) {         \
      (head) = (ITD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (ITD);   \
   } /* Endif */                 \
   (tail) = (ITD);               \
   (ITD)->SCRATCH_PTR = NULL
   
#define EHCI_ITD_QGET(head, tail, ITD)      \
   (ITD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_ITD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_ACTIVE_QUEUE_ADD_NODE(tail,member_ptr)     \
   if (tail->prev != NULL) {                             \
       tail->prev->next_active = TRUE;                   \
   }                                                     \
   tail->member = (pointer) member_ptr;                                  \
   tail->next_active = FALSE;                            \
   tail = tail->next;                   

#define EHCI_QUEUE_FREE_NODE(head,tail,node_ptr) \
   if(node_ptr->prev != NULL) {                             \
     node_ptr->prev->next = node_ptr->next;                 \
     node_ptr->prev->next_active = node_ptr->next_active;   \
     node_ptr->next->prev = node_ptr->prev;                 \
   } else  {                                                \
     head = node_ptr->next;                                 \
     head->prev = NULL;                                     \
   }                                                        \
   node_ptr->next = tail->next;                             \
   node_ptr->prev = tail;                                   \
   node_ptr->next_active = FALSE;                           \
   node_ptr->member = NULL;                                 \
   if(tail->next != NULL)   {                               \
      tail->next->prev = node_ptr;                          \
   }                                                        \
   tail->next = node_ptr;                                   \
   tail->next_active = FALSE;                             




#define EHCI_SITD_QADD(head, tail, SITD)      \
   if ((head) == NULL) {         \
      (head) = (SITD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (SITD);   \
   } /* Endif */                 \
   (tail) = (pointer) (SITD);               \
   (SITD)->SCRATCH_PTR = NULL
   
#define EHCI_SITD_QGET(head, tail, SITD)      \
   (SITD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_SITD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */
   
#define EHCI_QTD_QADD(head, tail, QTD)      \
   if ((head) == NULL) {         \
      (head) = (QTD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (QTD);   \
   } /* Endif */                 \
   (tail) = (pointer) (QTD);               \
   (QTD)->SCRATCH_PTR = NULL
   
#define EHCI_QTD_QGET(head, tail, QTD)      \
   (QTD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_QTD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_QH_QADD(head, tail, QH)      \
   if ((head) == NULL) {         \
      (head) = (pointer) (QH);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (QH);   \
   } /* Endif */                 \
   (tail) = (QH);               \
   (QH)->SCRATCH_PTR = NULL
   
#define EHCI_QH_QGET(head, tail, QH)      \
   (QH) = (head);               \
   if (head) {                   \
      (head) = (EHCI_QH_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

/* Additional Includes */
#include "ehci_bw.h"
#include "ehci_cncl.h"
#include "ehci_intr.h"
#include "ehci_iso.h"
#include "ehci_main.h"
#include "ehci_shut.h"
#include "ehci_utl.h"

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
