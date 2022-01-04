/*HEADER**********************************************************************
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
 *****************************************************************************
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
 *****************************************************************************
 *
 * $FileName: queue.h$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains the queue functions.
  *
 *END************************************************************************/

#ifndef __queue_h__
#define __queue_h__ 1

/*--------------------------------------------------------------------------*/
/*                     DATA STRUCTURE DEFINITIONS                           */



/*--------------------------------------------------------------------------*/
/* QUEUE ELEMENT STRUCTURE */
/*!
 * \brief Header for a queue element.
 *  
 * This structure is required in each queue element.
 * The address of this structure is used to enqueue, dequeue elements.
 * 
 * \see _queue_dequeue
 * \see _queue_enqueue
 * \see _queue_init
 * \see QUEUE_STRUCT
 */
typedef struct queue_element_struct
{

    /*! \brief Pointer to the next element in the queue. */
    struct queue_element_struct _PTR_ NEXT;
    
    /*! \brief Pointer to the previous element in the queue. */
    struct queue_element_struct _PTR_ PREV;

} QUEUE_ELEMENT_STRUCT, _PTR_ QUEUE_ELEMENT_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/* QUEUE STRUCTURE */
/*!
 * \brief Queue of any type of element that has a header of type 
 * QUEUE_ELEMENT_STRUCT.
 *   
 * This structure represents a generic queue head structure. Each queue element 
 * is made up of a data structure consisting of a NEXT pointer followed by a 
 * PREV pointer. Thus any type of element may be queued onto this queue.
 * 
 * \see _queue_init 
 * \see QUEUE_ELEMENT_STRUCT
 */
typedef struct queue_struct
{
    /*! 
     * \brief Pointer to the next element in the queue. If there are no elements
     * in the queue, the field is a pointer to the structure itself. 
     */
    struct queue_element_struct _PTR_ NEXT;
    
    /*
     * \brief Pointer to the last element in the queue. If there are no elements
     * in the queue, the field is a pointer to the structure itself. 
     */
    struct queue_element_struct _PTR_ PREV;
    
    /*! \brief Number of elements in the queue. */
    uint_16                           SIZE;
    
    /*! 
     * \brief Maximum number of elements that the queue can hold. If the field
     * is 0, the number is unlimited. 
     */
    uint_16                           MAX;

} QUEUE_STRUCT, _PTR_ QUEUE_STRUCT_PTR;



/*--------------------------------------------------------------------------*/
/*                       EXTERNAL DECLARATIONS                              */

#ifdef __cplusplus
extern "C" {
#endif

extern QUEUE_ELEMENT_STRUCT_PTR _queue_dequeue  (QUEUE_STRUCT_PTR);
extern boolean                  _queue_enqueue  (QUEUE_STRUCT_PTR, 
                                                 QUEUE_ELEMENT_STRUCT_PTR);
extern _mqx_uint                _queue_get_size (QUEUE_STRUCT_PTR);
extern void                     _queue_init     (QUEUE_STRUCT_PTR, uint_16);
extern boolean                  _queue_insert   (QUEUE_STRUCT_PTR, 
                                                 QUEUE_ELEMENT_STRUCT_PTR, 
                                                 QUEUE_ELEMENT_STRUCT_PTR);
extern boolean                  _queue_is_empty (QUEUE_STRUCT_PTR);
extern QUEUE_ELEMENT_STRUCT_PTR _queue_head     (QUEUE_STRUCT_PTR);
extern QUEUE_ELEMENT_STRUCT_PTR _queue_next     (QUEUE_STRUCT_PTR, 
                                                 QUEUE_ELEMENT_STRUCT_PTR);
extern void                     _queue_unlink   (QUEUE_STRUCT_PTR,
                                                 QUEUE_ELEMENT_STRUCT_PTR);
extern _mqx_uint                _queue_test     (QUEUE_STRUCT_PTR, 
                                                 pointer _PTR_);

#ifdef __cplusplus
}
#endif

#endif  /* __lwtimer_h__ */
/* EOF */
