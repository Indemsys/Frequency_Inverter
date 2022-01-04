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
 * $FileName: queue.c$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains the queue functions.
  *
 *END************************************************************************/

#define MQX_FORCE_USE_INLINE_MACROS     1

#include "mqx_inc.h"
/*!
 * \brief Initializes the provided queue.
 * 
 * \param[in] q_ptr Pointer to the queue to initialize.
 * \param[in] size  One of the following:
 * \li Maximum number of elements that the queue can hold.
 * \li 0 (Unlimited number.)
 * 
 * \warning If q_ptr is not a pointer to QUEUE_STRUCT, the function might behave
 * unpredictably.
 * 
 * \see _queue_enqueue
 * \see _queue_dequeue
 * \see QUEUE_STRUCT
 */ 
void _queue_init
(
    QUEUE_STRUCT_PTR q_ptr,
    uint_16          size
)
{ /* Body */

    _QUEUE_INIT(q_ptr, size);

} /* Endbody */
/*!
 * \brief Adds the element to the end of the queue.
 * 
 * \param[in] q_ptr Pointer to the queue to which to add the element; initialized 
 * with _queue_init().
 * \param[in] e_ptr Pointer to the element to add.
 * 
 * \return TRUE (success) or FALSE (Failure: the queue is full.)
 * 
 * \warning The function might behave unpredictably if either:
 * \li Q_ptr is not a pointer to QUEUE_STRUCT.
 * \li E_ptr is not a pointer to QUEUE_ELEMENT_STRUCT.
 * 
 * \see _queue_init 
 * \see _queue_dequeue
 * \see _queue_init
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT 
 */ 
boolean _queue_enqueue
(
    QUEUE_STRUCT_PTR         q_ptr,
    QUEUE_ELEMENT_STRUCT_PTR e_ptr
)
{ /* Body */

    _int_disable();
    if ((q_ptr->MAX != 0) && (q_ptr->SIZE >= q_ptr->MAX))
    {
        _int_enable();
        return (FALSE);
    } /* Endif */

    _QUEUE_ENQUEUE(q_ptr, e_ptr);
    _int_enable();
    return (TRUE);

} /* Endbody */

/*!
 * \brief Removes the first element from the queue.
 * 
 * \param[in] q_ptr Pointer to the queue from which to remove the first element; 
 * initialized with _queue_init().
 * 
 * \return Pointer to the removed first queue element.
 * \return NULL (Queue is empty.)
 * 
 * \warning If q_ptr is not a pointer to QUEUE_STRUCT, the function might behave
 * unpredictably.
 * 
 * \see _queue_enqueue
 * \see _queue_init
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT 
 */ 
QUEUE_ELEMENT_STRUCT_PTR _queue_dequeue
(
    QUEUE_STRUCT_PTR            q_ptr
)
{ /* Body */
    QUEUE_ELEMENT_STRUCT_PTR    e_ptr;

    _int_disable();
    if (q_ptr->SIZE == 0)
    {
        _int_enable();
        return (NULL);
    } /* Endif */

    _QUEUE_DEQUEUE(q_ptr, e_ptr);
    _int_enable();
    return (e_ptr);

} /* Endbody */

/*!
 * \brief Gets a pointer to the element at the start of the queue, but do not 
 * remove the element.
 * 
 * \param[in] q_ptr Pointer to the queue to use; initialized with _queue_init().
 * 
 * \return Pointer to the element that is at the start of the queue.
 * \return NULL (Queue is empty.)
 * 
 * \warning If q_ptr is not a pointer to QUEUE_STRUCT, the function might behave
 * unpredictably.
 * 
 * \see _queue_dequeue
 * \see _queue_init
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT 
 */ 
QUEUE_ELEMENT_STRUCT_PTR _queue_head
(
    QUEUE_STRUCT_PTR q_ptr
)
{ /* Body */
    QUEUE_ELEMENT_STRUCT_PTR e_ptr;

    _int_disable();
    if (q_ptr->SIZE == 0)
    {
        _int_enable();
        return (NULL);
    } /* Endif */

    e_ptr = q_ptr->NEXT;
    _int_enable();
    return (e_ptr);

} /* Endbody */

/*!
 * \brief Inserts the element in the queue.
 * 
 * \param[in] q_ptr  Pointer to the queue to insert into; initialized with 
 * _queue_init().
 * \param[in] qe_ptr One of the following:
 * \li Pointer to the element after which to insert the new element.
 * \li NULL (Insert the element at the start of the queue.)
 * \param[in] e_ptr  Pointer to the element to insert.
 * 
 * \return TRUE (success) or FALSE (Failure: queue is full.)
 * 
 * \warning The function might behave unpredictably if either:
 * \li Q_ptr is not a pointer to QUEUE_STRUCT.
 * \li E_ptr is not a pointer to QUEUE_ELEMENT_STRUCT.
 * 
 * \see _queue_init 
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT  
 */ 
boolean _queue_insert
(
    QUEUE_STRUCT_PTR         q_ptr,
    QUEUE_ELEMENT_STRUCT_PTR qe_ptr,
    QUEUE_ELEMENT_STRUCT_PTR e_ptr
)
{ /* Body */

    if (qe_ptr == NULL)
    {
        /* Insert at the front */
        qe_ptr = (QUEUE_ELEMENT_STRUCT_PTR) ((pointer) q_ptr);
    } /* Endif */
    _int_disable();
    if ((q_ptr->MAX != 0) && (q_ptr->SIZE >= q_ptr->MAX))
    {
        _int_enable();
        return (FALSE);
    } /* Endif */

    _QUEUE_INSERT(q_ptr, qe_ptr, e_ptr);
    _int_enable();
    return (TRUE);

} /* Endbody */

/*!
 * \brief Gets a pointer to the element after the provided one in the queue, but 
 * do not remove the element.
 * 
 * This function returns NULL if either:
 * \li E_ptr is NULL.
 * \li E_ptr is a pointer to the last element.
 * 
 * \param[in] q_ptr Pointer to the queue for which to get info; initialized with 
 * _queue_init().
 * \param[in] e_ptr Get the element after this one.
 * 
 * \return Pointer to the next queue element (success).
 * \return NULL (Failure: see Description.)
 * 
 * \warning The function might behave unpredictably if either:
 * \li Q_ptr is not a pointer to QUEUE_STRUCT.
 * \li E_ptr is not a pointer to QUEUE_ELEMENT_STRUCT.
 * 
 * \see _queue_init 
 * \see _queue_dequeue
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT  
 */ 
QUEUE_ELEMENT_STRUCT_PTR _queue_next
(
    QUEUE_STRUCT_PTR         q_ptr,
    QUEUE_ELEMENT_STRUCT_PTR e_ptr
)
{ /* Body */
    QUEUE_ELEMENT_STRUCT_PTR    next_ptr;

    if (e_ptr == NULL)
    {
        return NULL;
    } /* Endif */
    _int_disable();
    next_ptr = e_ptr->NEXT;
    _int_enable();
    if (next_ptr == (QUEUE_ELEMENT_STRUCT_PTR) ((pointer) q_ptr))
    {
        /* At end of queue */
        next_ptr = NULL;
    } /* Endif */
    return (next_ptr);

} /* Endbody */

/*!
 * \brief Tests the queue for consitstency and validity.
 * 
 * This function checks the queue pointers to ensure that they form a circular, 
 * double linked list, with the same number of elements that the queue header 
 * specifies.
 * 
 * \param[in]  q_ptr                Pointer to the queue to test; initialized 
 * with _queue_init().
 * \param[out] element_in_error_ptr Pointer to the first element with an error 
 * (initialized only if an error is found).
 * 
 * \return MQX_OK (No errors were found.)
 * \return MQX_CORRUPT_QUEUE (An error was found.)
 * 
 * \see _queue_init 
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT 
 */ 
_mqx_uint _queue_test
(
    QUEUE_STRUCT_PTR q_ptr,
    pointer _PTR_    element_in_error_ptr
)
{ /* Body */
    QUEUE_ELEMENT_STRUCT_PTR element_ptr;
    QUEUE_ELEMENT_STRUCT_PTR prev_ptr;
    _mqx_uint size;

    _int_disable();
    size = _QUEUE_GET_SIZE(q_ptr) + 1;
    element_ptr = q_ptr->NEXT;
    prev_ptr = (QUEUE_ELEMENT_STRUCT_PTR)((pointer)q_ptr);
    while (--size)
    {
        if (element_ptr == (pointer)q_ptr)
        {
            _int_enable();
            /* Size too big for # elements on queue */
            *element_in_error_ptr = element_ptr;
            return(MQX_CORRUPT_QUEUE);
        } /* Endif */
        if (element_ptr->PREV != prev_ptr)
        {
            _int_enable();
            *element_in_error_ptr = element_ptr;
            return(MQX_CORRUPT_QUEUE);
        } /* Endif */
        prev_ptr = element_ptr;
        element_ptr = element_ptr->NEXT;
    } /* Endwhile */

    /* Does the last element in the ring point back to the queue head */
    if ((pointer)element_ptr != (pointer)q_ptr)
    {
        _int_enable();
        *element_in_error_ptr = element_ptr;
        return(MQX_CORRUPT_QUEUE);
    } /* Endif */

    /* Is the last element in ring pointed to by queues PREV field */
    if (q_ptr->PREV != prev_ptr)
    {
        _int_enable();
        *element_in_error_ptr = element_ptr;
        return(MQX_CORRUPT_QUEUE);
    } /* Endif */

    _int_enable();
    return(MQX_OK);

} /* Endbody */

/*!
 * \brief Removes a specified element from the queue.
 * 
 * \param[in] q_ptr Pointer to the queue from which to remove the element; 
 * initialized with _queue_init().
 * \param[in] e_ptr Pointer to the element to remove.
 * 
 * \warning The function might behave unpredictably if either:
 * \li Q_ptr is not a pointer to QUEUE_STRUCT.
 * \li E_ptr is not a pointer to QUEUE_ELEMENT_STRUCT.
 * 
 * \see _queue_init 
 * \see _queue_dequeue
 * \see QUEUE_STRUCT
 * \see QUEUE_ELEMENT_STRUCT
 */ 
void _queue_unlink
(
    QUEUE_STRUCT_PTR         q_ptr,
    QUEUE_ELEMENT_STRUCT_PTR e_ptr
)
{ /* Body */

    _int_disable();
    _QUEUE_REMOVE(q_ptr, e_ptr);
    _int_enable();

} /* Endbody */

/*!
 * \brief Gets the number of elements in the queue.
 * 
 * \param[in] q_ptr Pointer to the queue for which to get info; initialized with 
 * _queue_init().
 * 
 * \return Number of elements in the queue.
 * 
 * \warning If q_ptr is not a pointer to QUEUE_STRUCT, the function might behave
 * unpredictably.
 * 
 * \see _queue_enqueue
 * \see _queue_init
 * \see QUEUE_STRUCT
 */ 
_mqx_uint _queue_get_size
(
    QUEUE_STRUCT_PTR        q_ptr
)
{ /* Body */

    return _QUEUE_GET_SIZE(q_ptr);

} /* Endbody */

/*!
 * \brief Determines whether the queue is empty.
 * 
 * \param[in] q_ptr Pointer to the queue for which to get info; initialized with 
 * _queue_init().
 * 
 * \return TRUE (Queue is empty.) or FALSE (Queue is not empty.)
 * 
 * \warning If q_ptr is not a pointer to QUEUE_STRUCT, the function might behave
 * unpredictably.
 * 
 * \see _queue_init
 * \see QUEUE_STRUCT 
 */ 
boolean _queue_is_empty
(
    QUEUE_STRUCT_PTR            q_ptr
)
{ /* Body */

    return _QUEUE_IS_EMPTY(q_ptr);

} /* Endbody */

/* EOF */
