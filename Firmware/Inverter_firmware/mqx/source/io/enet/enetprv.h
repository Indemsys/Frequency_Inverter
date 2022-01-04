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
* $FileName: enetprv.h$
* $Version : 3.8.5.0$
* $Date    : Aug-7-2012$
*
* Comments:
*
*   This file contains the private defines, externs and
*   data structure definitions required by the Ethernet
*   packet driver.
*
*END************************************************************************/
#ifndef __enetprv_h__
#define __enetprv_h__


#ifdef __cplusplus
extern "C" {
#endif


/***************************************
**
** Code macros
**
*/
#define ENET_ALIGN(x,pow)        (((x) + ((pow)-1)) & ~((pow)-1))

//#define ENET_lock_context()      _int_disable()
#define ENET_lock_context(enet_ptr)      \
    {   \
        _lwsem_wait(&enet_ptr->CONTEXT_LOCK);    \
    }


//#define ENET_unlock_context()    _int_enable()
#define ENET_unlock_context(enet_ptr)   \
    {   \
        _lwsem_post(&enet_ptr->CONTEXT_LOCK);    \
    }

#define QADD(head,tail,pcb)      \
   if ((head) == NULL) {         \
      (head) = (pcb);            \
   } else {                      \
      (tail)->PRIVATE = (pcb);   \
   }                             \
   (tail) = (pcb);               \
   (pcb)->PRIVATE = NULL

#define QGET(head,tail,pcb)      \
   (pcb) = (head);               \
   if (head) {                   \
      (head) = (head)->PRIVATE;  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      }                          \
   }             


/***************************************
**
** Data structures
**
*/


/* Joined multicast groups */
typedef struct enet_mcb_struct {
   _enet_address        GROUP;
   uint_32              HASH;
   struct enet_mcb_struct _PTR_     NEXT;
} ENET_MCB_STRUCT, _PTR_ ENET_MCB_STRUCT_PTR;

/* Registered protocol numbers */
typedef struct enet_ecb_struct {
   uint_16              TYPE;
   void (_CODE_PTR_     SERVICE)(PCB_PTR, pointer);
   ENET_MCB_STRUCT_PTR  MCB_HEAD;
   pointer              PRIVATE;
   struct enet_ecb_struct _PTR_     NEXT;
} ENET_ECB_STRUCT, _PTR_ ENET_ECB_STRUCT_PTR;

/* The Ethernet state structure */
struct enet_context_struct;

typedef struct enet_context_struct {
   struct enet_context_struct *  NEXT;
   ENET_ECB_STRUCT_PTR           ECB_HEAD;
   const ENET_PARAM_STRUCT *     PARAM_PTR;
   _enet_address                 ADDRESS;
   uchar                         PHY_ADDRESS;         // Note, keep adjacent to ADDRESS for alignment
   uint_16                       MaxRxFrameSize;
   uint_16                       MaxTxFrameSize;
   pointer                       MAC_CONTEXT_PTR;   
#if BSPCFG_ENABLE_ENET_STATS
   ENET_STATS                    STATS;      
#endif

   LWSEM_STRUCT                  CONTEXT_LOCK;
} ENET_CONTEXT_STRUCT, _PTR_ ENET_CONTEXT_STRUCT_PTR;


/***************************************
**
** Prototypes
**
*/

extern ENET_ECB_STRUCT_PTR ENET_find_receiver(ENET_CONTEXT_STRUCT_PTR, ENET_HEADER_PTR, uint_32_ptr);

extern uint_16 ENET_max_framesize(uint_16, uint_16, boolean );

extern void    ENET_Enqueue_Buffer( pointer * q, pointer b) ; 
extern pointer ENET_Dequeue_Buffer( pointer * q);

#ifdef __cplusplus
}
#endif

#endif /* __enetprv_h__ */
/* EOF */
