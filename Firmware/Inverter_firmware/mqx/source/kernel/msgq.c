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
 * $FileName: msgq.c$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Message component.
 *
 *END************************************************************************/

#include "mqx_inc.h"
#if MQX_USE_MESSAGES
#include "message.h"
#include "msg_prv.h"
#if MQX_USE_IPC
#include "ipc.h"
#include "ipcrtprv.h"
#endif
#define DEQUEUE_TOP_MSG_ENTRY(mq, im, m, td) \
               --(mq->NO_OF_ENTRIES); \
               im = mq->FIRST_MSG_PTR; \
               mq->FIRST_MSG_PTR = im->NEXT; \
               if ( mq->FIRST_MSG_PTR == NULL ) { \
                  mq->LAST_MSG_PTR = NULL; \
               } else { \
                  (mq->FIRST_MSG_PTR)->PREV = NULL; \
               } \
               if ( mq->TD_PTR ) { \
                  --(mq->TD_PTR->MESSAGES_AVAILABLE); \
               } \
               m = &im->MESSAGE; \
               im->QUEUED = FALSE; \
               im->TD_PTR = td;

/*!
 * \brief Sends the message to multiple message queues.
 * 
 * For conditions on the message, see _msgq_send().
 * \n The function sends a priority 0 message.
 * \n For each copy of the message, the function sets the target queue ID in the 
 * message header with a queue ID from the array of queue IDs.
 * \n The function does not block.
 * \n If the function returns successfully, the message is no longer a resource 
 * of the task.
 * \n It is the responsibility of the application to handle the consequences of 
 * messages being lost.
 * 
 * \param[in] input_msg_ptr Pointer to the message to be sent.
 * \param[in] qid_ptr       An array of _queue_ids, MSGQ_NULL_QUEUE_ID terminated
 * to which the message is to be sent.  A successfull message send will result in 
 * the queue id being overwritten with a MSGQ_NULL_QUEUE_ID.
 * \param[in] pool          One of the following:
 * \li Pool ID to allocate messages from.
 * \li MSGPOOL_NULL_POOL_ID (Messages will be allocated from a system message pool.)
 * 
 * \return Number that represents the size of the array of queue IDs (success).
 * \return Number less than the size of the array of queue IDs (failure).
 * 
 * \warning Calls _mem_copy()
 * \warning Calls _mem_alloc() or _mem_alloc_system() depending on whether pool_id 
 * represents a private or system message pool.
 * \warning Might dispatch one or more tasks.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_PARAMETER (Qid_ptr does not point to a valid queue ID.)
 * \li MSGPOOL_OUT_OF_MESSAGES (MQX could not allocate a message from the message 
 * pool.)
 * \li MSGQ_INVALID_MESSAGE (Msg_ptr does not point to a message that was allocated 
 * as described for _msgq_send().)
 * \li Task error codes from _msg_alloc() (If pool_id represents a private 
 * message pool.)
 * \li Task error codes from _msg_alloc_system() (If pool_id represents a system 
 * message pool.) 
 * 
 * \see _msgq_send   
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_poll
 * \see _msgq_send_priority
 * \see _msgq_send_urgent
 * \see _task_set_error
 * \see _mem_alloc
 * \see _mem_alloc_from
 * \see _mem_alloc_system
 * \see _mem_alloc_system_from
 * \see _mem_alloc_system_zero
 * \see _mem_alloc_system_zero_from
 * \see _mem_alloc_zero
 * \see _mem_alloc_zero_from
 * \see _mem_alloc_align
 * \see _mem_alloc_align_from
 * \see _mem_alloc_at
 * \see _mem_copy
 * \see MESSAGE_HEADER_STRUCT   
 */ 
_mqx_uint _msgq_send_broadcast
(
    pointer         input_msg_ptr,
    _queue_id _PTR_ qid_ptr,
    _pool_id        pool
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MESSAGE_HEADER_STRUCT_PTR msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)
    input_msg_ptr;
    MESSAGE_HEADER_STRUCT_PTR sending_msg_ptr;
#if MQX_CHECK_ERRORS
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
#endif
    _mqx_uint size;
    _mqx_uint count;
    _mqx_uint error;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE4(KLOG_msgq_send_broadcast, msg_ptr, qid_ptr, pool);

    count = 0;

#if MQX_CHECK_ERRORS
    if (msg_ptr == NULL )
    {
        _task_set_error(MSGQ_INVALID_MESSAGE);
        _KLOGX2(KLOG_msgq_send_broadcast, count);
        return(count);
    }/* Endif */
    if (qid_ptr == NULL )
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_msgq_send_broadcast, count);
        return(count);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);
    if (imsg_ptr->VALID != MSG_VALID)
    {
        /* An invalid message was input by the application. */
        _task_set_error(MSGQ_INVALID_MESSAGE);
        return(count);
    } /* Endif */
#endif

    if (*qid_ptr == MSGQ_NULL_QUEUE_ID )
    {
        /* Nothing to do */
        _KLOGX2(KLOG_msgq_send_broadcast, count);
        return(count);
    } /* Endif */

    size = msg_ptr->SIZE;

    while (*(qid_ptr + 1) != MSGQ_NULL_QUEUE_ID)
    {
        if (pool == MSGPOOL_NULL_POOL_ID)
        {
            sending_msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)
            _msg_alloc_system((_msg_size)size);
        }
        else
        {
            sending_msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)_msg_alloc(pool);
        } /* Endif */
        
        if (sending_msg_ptr != NULL)
        {
            _mem_copy(msg_ptr, sending_msg_ptr, (_mem_size)size);
            sending_msg_ptr->TARGET_QID = *qid_ptr;
            error = _task_get_error();
            if (_msgq_send_internal(sending_msg_ptr, FALSE, *qid_ptr++) )
            {
                count++;
            } /* Endif */
            _task_set_error(error);
        } /* Endif */
        else {
            // out of messages, stop sending loop
            // task error is set to MSGPOOL_OUT_OF_MESSAGES in _msg_alloc
            break;
        }
    } /* Endwhile */

    msg_ptr->TARGET_QID = *qid_ptr;
    error = _task_get_error();
    if (_msgq_send_internal(msg_ptr, FALSE, *qid_ptr))
    {
        count++;
    } /* Endif */
    _task_set_error(error);
    _KLOGX2(KLOG_msgq_send_broadcast, count);
    return(count);

} /* Endbody */

/*!
 * \brief Closes the message queue.
 * 
 * Only the task that opens a private message queue (_msgq_open()) can close it. 
 * Any task can close an opened system message queue (_msgq_open_system()).
 * \li If _msgq_close() closes the message queue, it frees any messages that are 
 * in the queue.
 * \li If _msgq_close() closes the message queue, a task can no longer use 
 * queue_id to access the message queue.
 * \li The message queue can subsequently be opened again with _msgq_open() or
 * _msgq_open_system().
 * 
 * \param[in] queue_id Queue ID of the message queue to be closed.
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \warning Calls _msg_free() to free messages that are in the queue.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is not valid for this processor.)
 * \li MSGQ_NOT_QUEUE_OWNER (Task that got queue_id did so by opening a private 
 * message queue (_msgq_open()) and is not the task calling _msgq_close().)
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Queue_id does not represent a queue that is open.)
 * \li Task error codes from _msg_free() (If MQX cannot free messages that are 
 * in the queue.)
 * 
 * \see _msgq_open_system
 * \see _msgq_open
 * \see _msg_free
 * \see _msgq_send
 * \see _task_set_error       
 */ 
boolean _msgq_close
(
    _queue_id queue_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    register TD_STRUCT_PTR               td_ptr;
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
    register MSGQ_STRUCT_PTR             msgq_ptr;
    register MSGQ_STRUCT_PTR             td_msgq_ptr;
    register MSGQ_STRUCT_PTR             qprev_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR tmp_imsg_ptr;
    register uint_16                     queue;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_msgq_close, queue_id);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX3(KLOG_msgq_close, FALSE, MQX_COMPONENT_DOES_NOT_EXIST);
        return(FALSE);
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if ( PROC_NUMBER_FROM_QID(queue_id) != kernel_data->INIT.PROCESSOR_NUMBER )
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        _KLOGX3(KLOG_msgq_close, FALSE, MSGQ_INVALID_QUEUE_ID);
        return FALSE;
    } /* Endif */
#endif

    queue = QUEUE_FROM_QID(queue_id);

#if MQX_CHECK_ERRORS
    if ( ! VALID_QUEUE(queue) )
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        _KLOGX3(KLOG_msgq_close, FALSE, MSGQ_INVALID_QUEUE_ID);
        return FALSE;
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];

#if MQX_CHECK_ERRORS
    if ( msgq_ptr->QUEUE != (queue) )
    {
        _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        _KLOGX3(KLOG_msgq_close, FALSE, MSGQ_QUEUE_IS_NOT_OPEN);
        return FALSE;
    } /* Endif */
#endif

    td_ptr = kernel_data->ACTIVE_PTR;
    if ( ((msgq_ptr->TD_PTR == NULL) || (msgq_ptr->TD_PTR == td_ptr)))
    {
        /*
         *  this queue is currently active and is owned by
         *  the requesting task so DELETE the queue.
         */
        _int_disable();
        if ( msgq_ptr->TD_PTR != NULL )
        {
            td_ptr->MESSAGES_AVAILABLE -= msgq_ptr->NO_OF_ENTRIES;
        } /* Endif */
        imsg_ptr = msgq_ptr->FIRST_MSG_PTR;
        while ( imsg_ptr != NULL )
        {
            tmp_imsg_ptr = imsg_ptr->NEXT;
            imsg_ptr->QUEUED = FALSE;
            _msg_free((pointer)&imsg_ptr->MESSAGE);
            imsg_ptr = tmp_imsg_ptr;
        } /* Endwhile */
        msgq_ptr->FIRST_MSG_PTR = NULL;

        if ( msgq_ptr->TD_PTR )
        {
            /* Remove the queue from the Task descriptors linked list */
            td_msgq_ptr = (MSGQ_STRUCT_PTR)td_ptr->MSG_QUEUE_HEAD;
            if ( td_msgq_ptr == msgq_ptr )
            {
                td_ptr->MSG_QUEUE_HEAD = (pointer)td_msgq_ptr->NEXT_MSGQ_PTR;
                if ( td_ptr->MSG_QUEUE_HEAD == NULL )
                {
                    td_ptr->MSG_QUEUE_TAIL = NULL;
                } /* Endif */
            }
            else
            {
                qprev_ptr = td_msgq_ptr;
                td_msgq_ptr = qprev_ptr->NEXT_MSGQ_PTR;
                while ( td_msgq_ptr != msgq_ptr )
                {
                    qprev_ptr = td_msgq_ptr;
                    td_msgq_ptr = qprev_ptr->NEXT_MSGQ_PTR;
                } /* Endwhile */
                qprev_ptr->NEXT_MSGQ_PTR = td_msgq_ptr->NEXT_MSGQ_PTR;
                if ( td_ptr->MSG_QUEUE_TAIL == (pointer)td_msgq_ptr )
                {
                    td_ptr->MSG_QUEUE_TAIL = (pointer)qprev_ptr;
                } /* Endif */
            } /* Endif */
        } /* Endif */
        msgq_ptr->QUEUE = 0;
        _int_enable();
        _KLOGX3(KLOG_msgq_close, TRUE, 0);
        return TRUE;
    }
    else
    {
        _task_set_error(MSGQ_NOT_QUEUE_OWNER);
        _KLOGX3(KLOG_msgq_close, FALSE, MSGQ_NOT_QUEUE_OWNER);
        return FALSE;
    } /* Endif */

} /* Endbody */

/*!
 * \brief Gets the number of messages in the message queue.
 * 
 * The message queue must be previously opened on this processor.
 * 
 * \param[in] queue_id One of the following:
 * \li Queue ID of the queue to be checked.
 * \li MSGQ_ANY_QUEUE (Get the number of messages waiting in all message queues 
 * that the task has open.)
 * 
 * \return Number of messages (Success.)
 * \return 0 (Success: queue is empty.)
 * \return 0 (Failure.)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is not valid for this processor.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Queue_id does not represent a message queue that 
 * is open.)
 * 
 * \see _msgq_open
 * \see _msgq_open_system
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_poll
 * \see _task_set_error    
 */ 
_mqx_uint _msgq_get_count
(
    _queue_id queue_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MSG_COMPONENT_STRUCT_PTR    msg_component_ptr;
    register MSGQ_STRUCT_PTR    msgq_ptr;
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register _mqx_uint          pending;
    register uint_16            queue;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_msgq_get_count, queue_id);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX3(KLOG_msgq_get_count, MQX_COMPONENT_DOES_NOT_EXIST, 0);
        return(0);
    } /* Endif */
#endif

    if (queue_id == MSGQ_ANY_QUEUE)
    {
        _KLOGX3(KLOG_msgq_get_count, MQX_OK, kernel_data->ACTIVE_PTR->MESSAGES_AVAILABLE);
        return(kernel_data->ACTIVE_PTR->MESSAGES_AVAILABLE );
    } /* Endif */

    pending = 0;
    queue = QUEUE_FROM_QID(queue_id);
    if ( (PROC_NUMBER_FROM_QID(queue_id) == kernel_data->INIT.PROCESSOR_NUMBER) &&
                    VALID_QUEUE(queue) )
    {
        msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
        if ( msgq_ptr->QUEUE == (queue) )
        {
            pending = msgq_ptr->NO_OF_ENTRIES;

            /* Check for short-cutted message ie not on q but could have been */
            if (msgq_ptr->TD_PTR != NULL)
            {
                imsg_ptr = (INTERNAL_MESSAGE_STRUCT_PTR)msgq_ptr->TD_PTR->MESSAGE;
                if ((imsg_ptr != NULL) &&
                                (QUEUE_FROM_QID(imsg_ptr->MESSAGE.TARGET_QID) == queue))
                {
                    ++pending;
                } /* Endif */
            } /* Endif */
        }
        else
        {
            _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        } /* Endif */
    }
    else
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
    } /* Endif */

    _KLOGX2(KLOG_msgq_get_count, pending);
    return(pending);

} /* Endbody */

/*!
 * \brief Converts a message-queue number and processor number to a queue ID.
 * 
 * The queue ID might not represent an open message queue. The queue ID can be 
 * used with functions that access message queues.
 * 
 * \param[in] processor_number One of the following:
 * \li Processor on which the message queue resides.
 * \li 0 (Indicates the local processor.)
 * \param[in] queue_number     Image-wide unique number that identifies the 
 * message queue.
 * 
 * \return Queue ID for the queue (Success.)
 * \return MSGQ_NULL_QUEUE_ID (Failure: _processor_number is not valid.)
 * 
 * \see _msgq_open_system
 * \see _msgq_open 
 */ 
_queue_id _msgq_get_id
(
    _processor_number processor_number,
    _queue_number     queue_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (processor_number > MQX_MAX_PROCESSOR_NUMBER)
    {
        return(MSGQ_NULL_QUEUE_ID);
    }/* Endif */
#endif   
    if ( processor_number == 0 )
    {
        processor_number = (_processor_number)kernel_data->INIT.PROCESSOR_NUMBER;
    } /* Endif */
    return BUILD_QID(processor_number, queue_number);

} /* Endbody */

/*!
 * \brief Sets the notification function for the private or the system message queue.
 * 
 * If the message queue is a system message queue, the function replaces the 
 * notification function and data that were installed with _msgq_open_system().
 * \n The notification function for a system message queue can get messages from 
 * the queue with _msgq_poll().
 * \n The notification function for a private message queue cannot get messages 
 * from the queue.
 *  
 * \param[in] qid                   Private or system message queue for which to 
 * install the notification function. 
 * \param[in] notification_function Function that MQX calls when MQX puts a 
 * message in the queue.
 * \param[in] notification_data     Data that MQX passes when it calls 
 * notification_function.
 * 
 * \return Pointer to the previous notification function (Success.) Notification function installed.
 * \return NULL (Success: Previous notification function was NULL.) Notification function installed.
 * \return NULL (Failure.) Notification function NOT installed.
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_OK (Notification function is installed; the previous function was NULL.)
 * \li MSGQ_INVALID_QUEUE_ID (Qid is not valid.) 
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Queue is not open.)
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * 
 * \see _msgq_open_system
 * \see _msgq_open
 * \see _msgq_poll
 * \see _msgq_get_notification_function
 * \see _task_set_error
 */ 
MSGQ_NOTIFICATION_FPTR _msgq_set_notification_function
(
    _queue_id              qid,
    MSGQ_NOTIFICATION_FPTR notification_function,
    pointer                notification_data
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    MSG_COMPONENT_STRUCT_PTR msg_component_ptr;
    MSGQ_STRUCT_PTR          msgq_ptr;
    MSGQ_NOTIFICATION_FPTR   result;
    _queue_number            queue;
#if MQX_CHECK_ERRORS
    _processor_number        pnum;
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_msgq_set_notification_function, qid, notification_function, notification_data);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        result = NULL;
        _KLOGX2(KLOG_msgq_set_notification_function, result);
        return(result);
    } /* Endif */
#endif

    queue = QUEUE_FROM_QID(qid);

#if MQX_CHECK_ERRORS
    pnum = PROC_NUMBER_FROM_QID(qid);
    if (( pnum != kernel_data->INIT.PROCESSOR_NUMBER) || ( ! VALID_QUEUE(queue)))
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        result = NULL;
        _KLOGX2(KLOG_msgq_set_notification_function, result);
        return result;
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
    if ( msgq_ptr->QUEUE != (queue) )
    {
        _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        result = NULL;
    }
    else
    {
        result = msgq_ptr->NOTIFICATION_FUNCTION;
        msgq_ptr->NOTIFICATION_FUNCTION = notification_function;
        msgq_ptr->NOTIFICATION_FUNCTION_PARAMETER = notification_data;
    } /* Endif */

    _KLOGX2(KLOG_msgq_set_notification_function, result);
    return result;

} /* Endbody */
/*!
 * \brief Gets the notification function and its data that are associated with 
 * the private or the system message queue.
 * 
 * \param[in]  qid                   Queue ID of the message queue for which to 
 * get the notification function.
 * \param[out] notification_function Pointer (which might be NULL) to the function 
 * that MQX calls when it puts a message in the message queue.
 * \param[out] notification_data     Pointer (which might be NULL) to data that 
 * MQX passes to the notification function.
 * 
 * \return MQX_OK
 * \return MSGQ_INVALID_QUEUE_ID (Queue_id does not represent a valid message 
 * queue on this processor.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \return MSGQ_QUEUE_IS_NOT_OPEN (Queue_id does not represent an open message 
 * queue.)
 * 
 * \warning On error, does not initialize notification_function or notification_data.
 * 
 * \see _msgq_open_system      
 * \see _msgq_open
 * \see _msgq_set_notification_function   
 */ 
_mqx_uint _msgq_get_notification_function
(
    _queue_id                    qid,
    MSGQ_NOTIFICATION_FPTR _PTR_ notification_function,
    pointer _PTR_                notification_data
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    MSG_COMPONENT_STRUCT_PTR msg_component_ptr;
    MSGQ_STRUCT_PTR          msgq_ptr;
    _mqx_uint                result = MQX_OK;
    _queue_number            queue;
#if MQX_CHECK_ERRORS
    _processor_number        pnum;
#endif

    _GET_KERNEL_DATA(kernel_data);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    queue = QUEUE_FROM_QID(qid);

#if MQX_CHECK_ERRORS
    pnum = PROC_NUMBER_FROM_QID(qid);
    if (( pnum != kernel_data->INIT.PROCESSOR_NUMBER) || ( ! VALID_QUEUE(queue)))
    {
        result = MSGQ_INVALID_QUEUE_ID;
    } /* Endif */
#endif

    if (result == MQX_OK)
    {
        msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
        if ( msgq_ptr->QUEUE != (queue) )
        {
            result = MSGQ_QUEUE_IS_NOT_OPEN;
        }
        else
        {
            *notification_function = msgq_ptr->NOTIFICATION_FUNCTION;
            *notification_data = msgq_ptr->NOTIFICATION_FUNCTION_PARAMETER;
        } /* Endif */
    } /* Endif */

    return result;

} /* Endbody */

/*!
 * \brief Opens the private message queue.
 * 
 * The open message queue has a NULL notification function.
 * \n Only the task that opens a private message queue can receive messages from 
 * the queue.
 * \n A task can subsequently attach a notification function and notification data 
 * to the message queue with _msgq_set_notification_function().
 * 
 * \param[in] queue One of the following:
 * \li Queue number of the message queue to be opened on this processor (min. 8, 
 * max. as defined in the MQX initialization structure).
 * \li MSGQ_FREE_QUEUE (MQX opens an unopened message queue.)
 * \param[in] max_queue_size One of the following:
 * \li Maximum queue size.
 * \li 0 (Unlimited size.)
 * 
 * \return Queue ID (Success.)
 * \return MSGQ_NULL_QUEUE_ID (Failure.)
 * 
 * \warning Creates the message component if it was not previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MSGQ_INVALID_QUEUE_ID (Queue_number is out of range)
 * \li MSGQ_INVALID_QUEUE_NUMBER
 * \li MSGQ_QUEUE_IN_USE One of the following: message queue is already open or 
 * MQX cannot get a queue number for an unopened queue.
 * 
 * \see _msgq_close  
 * \see _msgq_open_system
 * \see _msg_create_component
 * \see _msgq_set_notification_function
 * \see _task_set_error 
 */ 
_queue_id _msgq_open
(
    _queue_number queue,
    uint_16       max_queue_size
)
{ /* Body */
    _queue_id result;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_msgq_open, queue, max_queue_size);

    result = _msgq_open_internal(queue, max_queue_size, MSG_QUEUE,
                    (MSGQ_NOTIFICATION_FPTR)NULL, NULL);

    _KLOGX2(KLOG_msgq_open, result);

    return result;

} /* Endbody */

/*!
 * \private
 *  
 * \brief This is an internal function used to open a message queue.
 * 
 * \param[in] queue                 The queue being opened.
 * \param[in] max_queue_size        The maximum number of entries allowed in this 
 * queue.
 * \param[in] queue_type            The type of queue, NORMAL or SYSTEM.
 * \param[in] notification_function The function to be called when an entry is 
 * put on the queue.
 * \param[in] notification_data     Information to pass to the notification 
 * function.
 * 
 * \return Queue ID (Success.)
 * \return MSGQ_NULL_QUEUE_ID (Failure.)
 * 
 * \see _msgq_open   
 */ 
_queue_id _msgq_open_internal
(
    _queue_number          queue,
    uint_16                max_queue_size,
    _mqx_uint              queue_type,
    MSGQ_NOTIFICATION_FPTR notification_function,
    pointer                notification_data
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    TD_STRUCT_PTR            td_ptr;
    MSG_COMPONENT_STRUCT_PTR msg_component_ptr;
    register MSGQ_STRUCT_PTR temp_msgq_ptr;
    register MSGQ_STRUCT_PTR msgq_ptr;
    register _queue_id       qid;
    register _mqx_uint       i;
    _mqx_uint                result;

    _GET_KERNEL_DATA(kernel_data);
    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
    qid = (_queue_id)0;

    if (msg_component_ptr == NULL)
    {
        result = _msg_create_component();
        msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (msg_component_ptr == NULL)
        {
            _task_set_error(result);
            return(qid);
        } /* Endif */
#endif      
    } /* Endif */

    _int_disable();
    if (queue == MSGQ_FREE_QUEUE)
    {
        /* Lets find a free queue entry */
        msgq_ptr = &msg_component_ptr->MSGQS_PTR[MSGQ_FIRST_USER_QUEUE];
        for (i = MSGQ_FIRST_USER_QUEUE; i <= msg_component_ptr->MAX_MSGQS; ++i)
        {
            if (msgq_ptr->QUEUE != (_queue_number)i)
            {
                queue = (_queue_number)i;
                break;
                /* Found one! */
            } /* Endif */
            ++msgq_ptr;
        } /* Endfor */
        if (queue == MSGQ_FREE_QUEUE)
        {
            /* Did not get one */
            _int_enable();
            _task_set_error(MSGQ_QUEUE_IN_USE);
            return(qid);
        }/* Endif */
    }
    else
    {
        if (queue >= msg_component_ptr->MAX_MSGQS)
        {
            _int_enable();
            _task_set_error(MSGQ_INVALID_QUEUE_NUMBER);
            return(qid);
        }
    }

#if MQX_CHECK_ERRORS
    if ( !VALID_QUEUE(queue))
    {
        _int_enable();
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        return(qid);
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];

#if MQX_CHECK_ERRORS
    if (msgq_ptr->QUEUE == queue)
    {
        _int_enable();
        _task_set_error(MSGQ_QUEUE_IN_USE);
        return(qid);
    } /* Endif */
#endif

    msgq_ptr->QUEUE = queue;
    msgq_ptr->FIRST_MSG_PTR = NULL;
    msgq_ptr->LAST_MSG_PTR = NULL;
    msgq_ptr->NEXT_MSGQ_PTR = NULL;
    msgq_ptr->NO_OF_ENTRIES = 0;
    msgq_ptr->TYPE = (uint_16)queue_type;
    msgq_ptr->MAX_ENTRIES = max_queue_size;
    msgq_ptr->NOTIFICATION_FUNCTION = notification_function;
    msgq_ptr->NOTIFICATION_FUNCTION_PARAMETER = notification_data;

    if ( queue_type == MSG_QUEUE )
    {
        td_ptr = kernel_data->ACTIVE_PTR;
        msgq_ptr->TD_PTR = td_ptr;
        if ( td_ptr->MSG_QUEUE_HEAD == NULL )
        {
            td_ptr->MSG_QUEUE_HEAD = (pointer)msgq_ptr;
            td_ptr->MSG_QUEUE_TAIL = (pointer)msgq_ptr;
            td_ptr->MESSAGES_AVAILABLE = 0;
        }
        else
        {
            temp_msgq_ptr = (MSGQ_STRUCT_PTR)(td_ptr->MSG_QUEUE_TAIL);
            temp_msgq_ptr->NEXT_MSGQ_PTR = msgq_ptr;
            td_ptr->MSG_QUEUE_TAIL = (pointer)msgq_ptr;
        } /* Endif */
    }
    else
    {
        msgq_ptr->TD_PTR = NULL;
    } /* Endif */

    _INT_ENABLE();
    qid = BUILD_QID(kernel_data->INIT.PROCESSOR_NUMBER, queue);
    return (qid);

} /* Endbody */

/*!
 * \brief Gets the task ID of the task that owns the message queue.
 * 
 * \param[in] queue_id Queue ID of the message queue.
 * 
 * \return Task ID (Success.)
 * \return MQX_NULL_TASK_ID (Failure.)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MQX_INVALID_PROCESSOR_NUMBER (Processor number that queue_id specifies is 
 * not valid.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Message queue with queue ID queue_id is not open.)
 * 
 * \see _msgq_open
 * \see _msgq_open_system
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_send family
 * \see _task_set_error 
 */ 
_task_id _msgq_get_owner
(
    _queue_id queue_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    MSG_COMPONENT_STRUCT_PTR msg_component_ptr;
    register MSGQ_STRUCT_PTR msgq_ptr;
    _queue_number            queue;
    _task_id                 task_id;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_msgq_get_owner, queue_id);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX3(KLOG_msgq_get_owner, MQX_NULL_TASK_ID, MQX_COMPONENT_DOES_NOT_EXIST);
        return(MQX_NULL_TASK_ID);
    } /* Endif */
#endif

    queue = QUEUE_FROM_QID(queue_id);

#if MQX_CHECK_ERRORS
    if ((PROC_NUMBER_FROM_QID(queue_id) != kernel_data->INIT.PROCESSOR_NUMBER) ||
                    (! VALID_QUEUE(queue)) )
    {
        _task_set_error(MQX_INVALID_PROCESSOR_NUMBER);
        _KLOGX3(KLOG_msgq_get_owner, MQX_NULL_TASK_ID, MQX_INVALID_PROCESSOR_NUMBER);
        return (MQX_NULL_TASK_ID);
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
    if (msgq_ptr->QUEUE != (queue))
    {
        _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        _KLOGX3(KLOG_msgq_get_owner, MQX_NULL_TASK_ID, MSGQ_QUEUE_IS_NOT_OPEN);
        return (MQX_NULL_TASK_ID);
    } /* Endif */

    task_id = msgq_ptr->TD_PTR->TASK_ID;
    _KLOGX3(KLOG_msgq_get_owner, task_id, MQX_OK);
    return (task_id);

} /* Endbody */

/*!
 * \brief Gets a pointer to the message that is at the start of the message 
 * queue, but do not remove the message.
 * 
 * Call _msgq_get_count() first to determine whether there are messages in the 
 * queue. If there are no messages, _msgq_peek() calls _task_set_error() with 
 * MSGQ_MESSAGE_NOT_AVAILABLE.
 * 
 * \param[in] queue_id The queue to look at.
 * 
 * \return Pointer to the message that is at the start of the message queue 
 * (Success.)
 * \return NULL (Failure.) 
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is not valid.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (There are no messages in the message queue.)
 * \li MSGQ_NOT_QUEUE_OWNER (Task is not the owner of the private message queue.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Queue is not open.)
 * 
 * \see _msgq_get_count
 * \see _msgq_open_system
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_send
 * \see _task_set_error
 * \see _msg_create_component
 * \see MESSAGE_HEADER_STRUCT  
 */ 
pointer _msgq_peek
(
    _queue_id queue_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
    register MESSAGE_HEADER_STRUCT_PTR   message_ptr;
    register MSGQ_STRUCT_PTR             msgq_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    _queue_number                        queue;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_msgq_peek, queue_id);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX3(KLOG_msgq_peek, NULL, MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
#endif

    message_ptr = NULL;
    queue = QUEUE_FROM_QID(queue_id);

#if MQX_CHECK_ERRORS
    if ((PROC_NUMBER_FROM_QID(queue_id) != kernel_data->INIT.PROCESSOR_NUMBER) ||
                    (! VALID_QUEUE(queue)) )
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        _KLOGX3(KLOG_msgq_peek, NULL, MSGQ_INVALID_QUEUE_ID);
        return (pointer)message_ptr;
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
    if (msgq_ptr->QUEUE != (queue))
    {
        _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        _KLOGX3(KLOG_msgq_poll, NULL, MSGQ_QUEUE_IS_NOT_OPEN);
        return((pointer)message_ptr);
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (msgq_ptr->TD_PTR != NULL)
    {
        if (msgq_ptr->TD_PTR != kernel_data->ACTIVE_PTR)
        {
            _task_set_error(MSGQ_NOT_QUEUE_OWNER);
            _KLOGX3(KLOG_msgq_poll, NULL, MSGQ_NOT_QUEUE_OWNER);
            return((pointer)message_ptr);
        } /* Endif */
    } /* Endif */
#endif

    /* check the specified queue for an entry */
    _INT_DISABLE();
    if ( msgq_ptr->NO_OF_ENTRIES == 0 )
    {
        _INT_ENABLE();
#if MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
        _task_set_error(MSGQ_MESSAGE_NOT_AVAILABLE);
#endif
        _KLOGX3(KLOG_msgq_peek, NULL, MSGQ_MESSAGE_NOT_AVAILABLE);
    }
    else
    {
        /* Return the top entry, but keep it on the queue */
        imsg_ptr = msgq_ptr->FIRST_MSG_PTR;
        message_ptr = (MESSAGE_HEADER_STRUCT_PTR)&imsg_ptr->MESSAGE;

        _INT_ENABLE();
        _KLOGX5(KLOG_msgq_peek, message_ptr, message_ptr->TARGET_QID, message_ptr->SOURCE_QID, *(_mqx_uint_ptr)((uchar_ptr)message_ptr+sizeof(MESSAGE_HEADER_STRUCT)));
    } /* Endif */
    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \brief Polls the message queue for a message, but do not wait if a message is 
 * not in the queue. The function is a non-blocking alternative to _msgq_receive(); 
 * therefore, ISRs can use it.
 * 
 * The function is the only way for tasks to receive messages from a system 
 * message queue.
 * \li If a system message queue has a notification function, the function can get 
 * messages from the queue with _msgq_poll().
 * \li If a message is returned, the message becomes a resource of the task.
 * 
 * \param[in] queue_id Private or system message queue from which to receive a 
 * message.
 * 
 * \return Pointer to a message (Success.)
 * \return NULL (Failure.)  
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is not valid or is not on this processor.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (There are no messages in the message queue.)
 * \li MSGQ_NOT_QUEUE_OWNER (Queue is a private message queue that the task does 
 * not own.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Queue is not open.)
 * 
 * \see _msgq_get_count
 * \see _msgq_open_system
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_send
 * \see _task_set_error
 * \see _msg_create_component
 * \see MESSAGE_HEADER_STRUCT  
 */ 
pointer _msgq_poll
(
    _queue_id queue_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
    register MESSAGE_HEADER_STRUCT_PTR   message_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register MSGQ_STRUCT_PTR             msgq_ptr;
    _queue_number                        queue;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_msgq_poll, queue_id);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX3(KLOG_msgq_poll, NULL, MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
#endif

    message_ptr = NULL;
    queue = QUEUE_FROM_QID(queue_id);

#if MQX_CHECK_ERRORS
    if ((PROC_NUMBER_FROM_QID(queue_id) != kernel_data->INIT.PROCESSOR_NUMBER) ||
                    (! VALID_QUEUE(queue)) )
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        _KLOGX3(KLOG_msgq_poll, NULL, MSGQ_INVALID_QUEUE_ID);
        return (pointer)message_ptr;
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
    if (msgq_ptr->QUEUE != (queue))
    {
        _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        _KLOGX3(KLOG_msgq_poll, NULL, MSGQ_QUEUE_IS_NOT_OPEN);
        return((pointer)message_ptr);
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (msgq_ptr->TD_PTR != NULL)
    {
        if (msgq_ptr->TD_PTR != kernel_data->ACTIVE_PTR)
        {
            _task_set_error(MSGQ_NOT_QUEUE_OWNER);
            _KLOGX3(KLOG_msgq_poll, NULL, MSGQ_NOT_QUEUE_OWNER);
            return((pointer)message_ptr);
        } /* Endif */
    } /* Endif */
#endif

    /* check the specified queue for an entry */
    _INT_DISABLE();
    if ( msgq_ptr->NO_OF_ENTRIES == 0 )
    {
        _INT_ENABLE();
#if MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
        _task_set_error(MSGQ_MESSAGE_NOT_AVAILABLE);
#endif
        _KLOGX3(KLOG_msgq_poll, NULL, MSGQ_MESSAGE_NOT_AVAILABLE);
    }
    else
    {

        --(msgq_ptr->NO_OF_ENTRIES);
        imsg_ptr = msgq_ptr->FIRST_MSG_PTR;
        msgq_ptr->FIRST_MSG_PTR = imsg_ptr->NEXT;
        if ( msgq_ptr->FIRST_MSG_PTR == NULL )
        {
            msgq_ptr->LAST_MSG_PTR = NULL;
        }
        else
        {
            (msgq_ptr->FIRST_MSG_PTR)->PREV = NULL;
        } /* Endif */

        if ( msgq_ptr->TD_PTR )
        {
            --(msgq_ptr->TD_PTR->MESSAGES_AVAILABLE);
        } /* Endif */
        message_ptr = &imsg_ptr->MESSAGE;
        imsg_ptr->QUEUED = FALSE;
        imsg_ptr->TD_PTR = NULL;

        _INT_ENABLE();
        _KLOGX2(KLOG_msgq_poll, message_ptr);
    } /* Endif */
    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \brief Wait for a message from the private message queue for the specified 
 * number of milliseconds.
 * 
 * The function removes the first message from the queue and returns a pointer to 
 * the message. The message becomes a resource of the task.
 * \n The function cannot be used to receive messages from system message queues; 
 * this must be done with _msgq_poll().
 * 
 * \param[in] queue_id One of the following:
 * \li Private message queue from which to receive a message.
 * \li MSGQ_ANY_QUEUE (Any queue that the task owns.)
 * \param[in] timeout  One of the following:
 * \li Maximum number of milliseconds to wait.
 * \li 0 (Unlimited wait.)
 * 
 * \return Pointer to a message (Success.)
 * \return NULL (Failure.)
 * 
 * \warning If no message is available, blocks the task until the message queue 
 * gets a message or the timeout expires.
 * \warning Cannot be called from an ISR.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is for a specific queue, but the ID is 
 * not valid.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (No messages were in the message queue before 
 * the timeout expired.)
 * \li MSGQ_NOT_QUEUE_OWNER (Message is not a resource of the task.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN One of the following: specific queue is not open 
 * or queue_id is MSGQ_ANY_QUEUE, but the task has no queues open.
 * 
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_get_count
 * \see _msgq_open
 * \see _msgq_poll
 * \see _msgq_send
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT    
 */ 
pointer _msgq_receive
(
    _queue_id queue_id,
    uint_32   timeout
)
{ /* Body */
    /* TIME_STRUCT                 time; */
    MQX_TICK_STRUCT           ticks;
#if MQX_KERNEL_LOGGING
    KERNEL_DATA_STRUCT_PTR    kernel_data;
#endif
    MESSAGE_HEADER_STRUCT_PTR message_ptr;
    _mqx_uint                 error;

#if MQX_KERNEL_LOGGING
    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_msgq_receive, queue_id, timeout);
#endif

    if (timeout)
    {
        /* Convert milliseconds to ticks */
        /* time.MILLISECONDS = timeout % 1000; */
        /* time.SECONDS      = timeout / 1000; */
        /*                                     */
        /* PSP_TIME_TO_TICKS(&time, &ticks);   */
        PSP_MILLISECONDS_TO_TICKS_QUICK(timeout, &ticks);

        message_ptr = _msgq_receive_internal(queue_id, &ticks,
                        MSG_TIMEOUT_RELATIVE, &error);
    }
    else
    {
        message_ptr = _msgq_receive_internal(queue_id, &ticks,
                        MSG_TIMEOUT_NONE, &error);
    } /* Endif */

#if MQX_KERNEL_LOGGING
    if ( (error == MQX_OK) && (message_ptr == NULL) )
    {
        _KLOGX3(KLOG_msgq_receive, message_ptr, MSGQ_MESSAGE_NOT_AVAILABLE);
    }
    else if (error == MQX_OK)
    {
        _KLOGX5(KLOG_msgq_receive, message_ptr, message_ptr->TARGET_QID,
                        message_ptr->SOURCE_QID, *(_mqx_uint_ptr)((uchar_ptr)message_ptr+
                                        sizeof(MESSAGE_HEADER_STRUCT)));
    }
    else
    {
        _KLOGX3(KLOG_msgq_receive, message_ptr, error);
    } /* Endif */
#endif

    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \brief Wait for a message from the private message queue for the specified 
 * number of ticks (in tick time).
 * 
 * The function removes the first message from the queue and returns a pointer to 
 * the message. The message becomes a resource of the task.
 * \n The function cannot be used to receive messages from system message queues; 
 * this must be done with _msgq_poll().
 * 
 * \param[in] queue_id One of the following:
 * \li Private message queue from which to receive a message.
 * \li MSGQ_ANY_QUEUE (Any queue that the task owns.)
 * \param[in] tick_ptr  One of the following:
 * \li Pointer to the maximum number of ticks to wait.
 * \li NULL (Unlimited wait.)
 * 
 * \return Pointer to a message (Success.)
 * \return NULL (Failure.)
 * 
 * \warning If no message is available, blocks the task until the message queue 
 * gets a message or the timeout expires.
 * \warning Cannot be called from an ISR.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is for a specific queue, but the ID is 
 * not valid.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (No messages were in the message queue before 
 * the timeout expired.)
 * \li MSGQ_NOT_QUEUE_OWNER (Message is not a resource of the task.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN One of the following: specific queue is not open 
 * or queue_id is MSGQ_ANY_QUEUE, but the task has no queues open.
 * 
 * \see _msgq_receive
 * \see _msgq_receive_ticks
 * \see _msgq_receive_until
 * \see _msgq_get_count
 * \see _msgq_open
 * \see _msgq_poll
 * \see _msgq_send
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT    
 */
pointer _msgq_receive_for
(
    _queue_id           queue_id,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
#if MQX_KERNEL_LOGGING
    KERNEL_DATA_STRUCT_PTR    kernel_data;
#endif
    MESSAGE_HEADER_STRUCT_PTR message_ptr;
    _mqx_uint                 error;

#if MQX_KERNEL_LOGGING
    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_msgq_receive_for, queue_id, tick_ptr);
#endif

    message_ptr = _msgq_receive_internal(queue_id, tick_ptr,
                    MSG_TIMEOUT_RELATIVE, &error);

#if MQX_KERNEL_LOGGING
    if ( (error == MQX_OK) && (message_ptr == NULL) )
    {
        _KLOGX3(KLOG_msgq_receive_for, message_ptr, MSGQ_MESSAGE_NOT_AVAILABLE);
    }
    else if (error == MQX_OK)
    {
        _KLOGX5(KLOG_msgq_receive_for, message_ptr, message_ptr->TARGET_QID,
                        message_ptr->SOURCE_QID, *(_mqx_uint_ptr)((uchar_ptr)message_ptr+
                                        sizeof(MESSAGE_HEADER_STRUCT)));
    }
    else
    {
        _KLOGX3(KLOG_msgq_receive_for, message_ptr, error);
    } /* Endif */
#endif

    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \private
 * 
 * \brief Dequeue the first item from the specified queue.
 * 
 * \param[in]  queue_id         ID of the queue from which a message is to be 
 * received.
 * \param[in]  timeout_tick_ptr Indication of the number of ticks which can expire 
 * before this request times out.
 * \param[in]  mode             Relative or absolute time specified in tick_ptr.
 * \param[out] error_ptr        Where the error code is to be stored.   
 *  
 * \return Pointer to the message (Success).
 * \return NULL (Failure.)   
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_CANNOT_CALL_FUNCTION_FROM_ISR
 * \li MQX_COMPONENT_DOES_NOT_EXIST
 * \li MSGQ_QUEUE_IS_NOT_OPEN
 * \li MSGQ_INVALID_QUEUE_ID
 * \li MSGQ_NOT_QUEUE_OWNER
 * \li MSGQ_MESSAGE_NOT_AVAILABLE
 * \li MSGQ_INVALID_MESSAGE  
 */ 
MESSAGE_HEADER_STRUCT_PTR _msgq_receive_internal
(
    _queue_id           queue_id,
    MQX_TICK_STRUCT_PTR timeout_tick_ptr,
    _mqx_uint           mode,
    _mqx_uint_ptr       error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
    register TD_STRUCT_PTR               td_ptr;
    MESSAGE_HEADER_STRUCT_PTR            message_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register MSGQ_STRUCT_PTR             msgq_ptr;
    _queue_number                        queue;

    *error_ptr = MQX_OK;

    _GET_KERNEL_DATA(kernel_data);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _task_set_error(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        *error_ptr = MQX_CANNOT_CALL_FUNCTION_FROM_ISR;
        return(NULL);
    }/* Endif */
#endif

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        *error_ptr = MQX_COMPONENT_DOES_NOT_EXIST;
        return(NULL);
    } /* Endif */
#endif

    message_ptr = NULL;
    td_ptr = kernel_data->ACTIVE_PTR;

    if (queue_id == MSGQ_ANY_QUEUE)
    {
        /* if queue_id is 0 than a receive from any queue is performed */

        /* Does the task own a queue */
        if (td_ptr->MSG_QUEUE_HEAD == NULL)
        {
            /* Does NOT */
            _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
            *error_ptr = MSGQ_QUEUE_IS_NOT_OPEN;
            return NULL;
        } /* Endif */

        _INT_DISABLE();
        if (td_ptr->MESSAGES_AVAILABLE == 0)
        {
            td_ptr->STATE = RCV_ANY_BLOCKED;
            td_ptr->INFO = queue_id;
            td_ptr->MESSAGE = NULL;

            if (mode == MSG_TIMEOUT_NONE)
            {
                _task_block();
            }
            else if (mode == MSG_TIMEOUT_RELATIVE)
            {
                _time_delay_for(timeout_tick_ptr);
            }
            else
            {
                _time_delay_until(timeout_tick_ptr);
            } /* Endif */

            /* 
             * SHORTCUT...
             * The message send routine does not queue up a message in this case.
             * the message is deposited directly into the task descriptor
             */
            message_ptr = (MESSAGE_HEADER_STRUCT_PTR)td_ptr->MESSAGE;
            if (message_ptr == NULL)
            {
                /* A timeout has occurred */
#if MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
                _task_set_error(MSGQ_MESSAGE_NOT_AVAILABLE);
#endif
            } /* Endif */
            td_ptr->MESSAGE = NULL;

        }
        else
        {

            /*  Check all queues for an available entry .. There must be at least
             *  one entry available
             */
            msgq_ptr = (MSGQ_STRUCT_PTR)td_ptr->MSG_QUEUE_HEAD;
            while (msgq_ptr != NULL)
            {

                if (msgq_ptr->NO_OF_ENTRIES)
                {
                    /* dequeue the top entry */
                    DEQUEUE_TOP_MSG_ENTRY(msgq_ptr, imsg_ptr, message_ptr, td_ptr);
                    break;
                } /* Endif */
                msgq_ptr = msgq_ptr->NEXT_MSGQ_PTR;

            } /* Endwhile */
        } /* Endif */

    }
    else
    {

        /* RECEIVE from a specific qid */
        queue = QUEUE_FROM_QID(queue_id);
#if MQX_CHECK_ERRORS
        if ( (PROC_NUMBER_FROM_QID(queue_id) != kernel_data->INIT.PROCESSOR_NUMBER) ||
                        (! VALID_QUEUE(queue)) )
        {
            _task_set_error(MSGQ_INVALID_QUEUE_ID);
            *error_ptr = MSGQ_INVALID_QUEUE_ID;
            return (pointer)message_ptr;
        } /* Endif */
#endif

        msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
#if MQX_CHECK_ERRORS
        if ( msgq_ptr->QUEUE != queue )
        {
            _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
            *error_ptr = MSGQ_QUEUE_IS_NOT_OPEN;
            return message_ptr;
        } /* Endif */
        if ( (msgq_ptr->TD_PTR != NULL) && (msgq_ptr->TD_PTR != td_ptr) )
        {
            _task_set_error(MSGQ_NOT_QUEUE_OWNER);
            *error_ptr = MSGQ_NOT_QUEUE_OWNER;
            return message_ptr;
        } /* Endif */
#endif

        /*
         * check the specified queue for an entry
         * if not entry, then block until an entry is received or
         * timeout occurs
         */
        _INT_DISABLE();
        if (msgq_ptr->NO_OF_ENTRIES == 0)
        {
            if (msgq_ptr->TD_PTR == NULL)
            {
                /* A system message queue, indicate none available */
                message_ptr = NULL;
            }
            else
            {
                td_ptr->STATE = RCV_SPECIFIC_BLOCKED;
                td_ptr->INFO = queue;
                td_ptr->MESSAGE = NULL;

                if (mode == MSG_TIMEOUT_NONE)
                {
                    _task_block();
                }
                else if (mode == MSG_TIMEOUT_RELATIVE)
                {
                    _time_delay_for(timeout_tick_ptr);
                }
                else
                {
                    _time_delay_until(timeout_tick_ptr);
                } /* Endif */

                message_ptr = (MESSAGE_HEADER_STRUCT_PTR)td_ptr->MESSAGE;
                if ( message_ptr == NULL )
                {
#if MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
                    _task_set_error(MSGQ_MESSAGE_NOT_AVAILABLE);
#endif
                }
                else if ((message_ptr->TARGET_QID != queue_id) &&
                                (msgq_ptr->NO_OF_ENTRIES > 0))
                {
                    /* The original msg was swapped out in msgq_sendi() for 
                     * a higher priority msg with a different target_qid.
                     * Enqueue this msg, and then dequeue the msg we need.
                     */
                    register MSGQ_STRUCT_PTR tmp_msgq_ptr;
                    register _queue_number tmp_queue;

                    /* Get the msg's queue */
                    tmp_queue = QUEUE_FROM_QID(message_ptr->TARGET_QID);
                    tmp_msgq_ptr = &msg_component_ptr->MSGQS_PTR[tmp_queue];

                    if ((tmp_msgq_ptr->MAX_ENTRIES == 0) ||
                                    (tmp_msgq_ptr->NO_OF_ENTRIES < tmp_msgq_ptr->MAX_ENTRIES))
                    {
                        /* the msg's queue has room */
                        imsg_ptr = GET_INTERNAL_MESSAGE_PTR(message_ptr);

#if MQX_CHECK_ERRORS
                        if (imsg_ptr->VALID != MSG_VALID)
                        {
                            /* An invalid message was input by the application. */
                            _task_set_error(MSGQ_INVALID_MESSAGE);
                            message_ptr = NULL;
                        }
                        else
#endif
                        {
                            /* enqueue the msg */
                            _msgq_insert_message_internal(tmp_msgq_ptr, imsg_ptr, TRUE);

                            if (tmp_msgq_ptr->TD_PTR)
                            {
                                ++(tmp_msgq_ptr->TD_PTR->MESSAGES_AVAILABLE);
                            } /* Endif */

                            /* now dequeue our queue's top entry */
                            DEQUEUE_TOP_MSG_ENTRY(msgq_ptr, imsg_ptr, message_ptr, td_ptr);
                        }
                    }
                    else
                    {
                        /* Queue full, error - this should not happen 
                         * since msgq_sendi() checks for room on the queue 
                         * for all msgs, including short-cut msgs. 
                         */
                        _task_set_error(MSGQ_QUEUE_FULL);
                        message_ptr = NULL;
                    }
                } /* Endif */
                td_ptr->MESSAGE = NULL;
            } /* Endif */
        }
        else
        {
            /* dequeue the top entry */
            DEQUEUE_TOP_MSG_ENTRY(msgq_ptr, imsg_ptr, message_ptr, td_ptr);
        } /* Endif */
    } /* Endif */

    _INT_ENABLE();

    return message_ptr;

} /* Endbody */

/*!
 * \brief Wait for a message from the private message queue for the specified 
 * number of ticks.
 * 
 * The function removes the first message from the queue and returns a pointer to 
 * the message. The message becomes a resource of the task.
 * \n The function cannot be used to receive messages from system message queues; 
 * this must be done with _msgq_poll().
 * 
 * \param[in] queue_id One of the following:
 * \li Private message queue from which to receive a message.
 * \li MSGQ_ANY_QUEUE (Any queue that the task owns.)
 * \param[in] time_in_ticks  One of the following:
 * \li Maximum number of ticks to wait.
 * \li 0 (Unlimited wait.)
 * 
 * \return Pointer to a message (Success.)
 * \return NULL (Failure.)
 * 
 * \warning If no message is available, blocks the task until the message queue 
 * gets a message or the timeout expires.
 * \warning Cannot be called from an ISR.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is for a specific queue, but the ID is 
 * not valid.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (No messages were in the message queue before 
 * the timeout expired.)
 * \li MSGQ_NOT_QUEUE_OWNER (Message is not a resource of the task.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN One of the following: specific queue is not open 
 * or queue_id is MSGQ_ANY_QUEUE, but the task has no queues open.
 * 
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_until
 * \see _msgq_get_count
 * \see _msgq_open
 * \see _msgq_poll
 * \see _msgq_send
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT    
 */ 
pointer _msgq_receive_ticks
(
    _queue_id queue_id,
    _mqx_uint time_in_ticks
)
{ /* Body */
    MQX_TICK_STRUCT ticks;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MESSAGE_HEADER_STRUCT_PTR message_ptr;
    _mqx_uint error;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE3(KLOG_msgq_receive_ticks, queue_id, time_in_ticks);

    if (time_in_ticks)
    {
        _time_init_ticks(&ticks, time_in_ticks);

        message_ptr = _msgq_receive_internal(queue_id, &ticks,
                        MSG_TIMEOUT_RELATIVE, &error);
    }
    else
    {
        message_ptr = _msgq_receive_internal(queue_id, &ticks,
                        MSG_TIMEOUT_NONE, &error);
    } /* Endif */

#if MQX_KERNEL_LOGGING
    if ( (error == MQX_OK) && (message_ptr == NULL) )
    {
        _KLOGX3(KLOG_msgq_receive_ticks, message_ptr, MSGQ_MESSAGE_NOT_AVAILABLE);
    }
    else if (error == MQX_OK)
    {
        _KLOGX5(KLOG_msgq_receive_ticks, message_ptr, message_ptr->TARGET_QID,
                        message_ptr->SOURCE_QID, *(_mqx_uint_ptr)((uchar_ptr)message_ptr+
                                        sizeof(MESSAGE_HEADER_STRUCT)));
    }
    else
    {
        _KLOGX3(KLOG_msgq_receive_ticks, message_ptr, error);
    } /* Endif */
#endif

    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \brief Wait for a message from the private message queue until the specified 
 * time (in tick time).
 * 
 * The function removes the first message from the queue and returns a pointer to 
 * the message. The message becomes a resource of the task.
 * \n The function cannot be used to receive messages from system message queues; 
 * this must be done with _msgq_poll().
 * 
 * \param[in] queue_id One of the following:
 * \li Private message queue from which to receive a message.
 * \li MSGQ_ANY_QUEUE (Any queue that the task owns.)
 * \param[in] tick_ptr One of the following:
 * \li Pointer to the time (in tick time) until which to wait.
 * \li NULL (Unlimited wait)
 * 
 * \return Pointer to a message (Success.)
 * \return NULL (Failure.)
 * 
 * \warning If no message is available, blocks the task until the message queue 
 * gets a message or the timeout expires.
 * \warning Cannot be called from an ISR.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is for a specific queue, but the ID is 
 * not valid.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (No messages were in the message queue before 
 * the timeout expired.)
 * \li MSGQ_NOT_QUEUE_OWNER (Message is not a resource of the task.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN One of the following: specific queue is not open 
 * or queue_id is MSGQ_ANY_QUEUE, but the task has no queues open.
 * 
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks
 * \see _msgq_get_count
 * \see _msgq_open
 * \see _msgq_poll
 * \see _msgq_send
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT    
 */
pointer _msgq_receive_until
(
    _queue_id           queue_id,
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
#if MQX_KERNEL_LOGGING
    KERNEL_DATA_STRUCT_PTR    kernel_data;
#endif
    MESSAGE_HEADER_STRUCT_PTR message_ptr;
    _mqx_uint                 error;

#if MQX_KERNEL_LOGGING
    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_msgq_receive_for, queue_id, tick_ptr);
#endif

    message_ptr = _msgq_receive_internal(queue_id, tick_ptr,
                    MSG_TIMEOUT_ABSOLUTE, &error);

#if MQX_KERNEL_LOGGING
    if ( (error == MQX_OK) && (message_ptr == NULL) )
    {
        _KLOGX3(KLOG_msgq_receive_until, message_ptr, MSGQ_MESSAGE_NOT_AVAILABLE);
    }
    else if (error == MQX_OK)
    {
        _KLOGX5(KLOG_msgq_receive_until, message_ptr, message_ptr->TARGET_QID,
                        message_ptr->SOURCE_QID, *(_mqx_uint_ptr)((uchar_ptr)message_ptr+
                                        sizeof(MESSAGE_HEADER_STRUCT)));
    }
    else
    {
        _KLOGX3(KLOG_msgq_receive_until, message_ptr, error);
    } /* Endif */
#endif

    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \brief Sends the message to the message queue.
 * 
 * The function sends a message (priority 0) to a private message queue or a system 
 * message queue. The function does not block. The message must be from one of:
 * \li _msg_alloc()
 * \li _msg_alloc_system()
 * \li _msgq_poll()
 * \li _msgq_receive()
 * The message must be overlaid with MESSAGE_HEADER_STRUCT, with the data portion 
 * following the header. In the header, the sending task sets:
 * \li TARGET_ID to a valid queue ID for the local processor or for a remote 
 * processor (if TARGET_ID is for a remote processor, the function cannot verify 
 * the ID or determine whether the maximum size of the queue is reached).
 * \li SIZE to the number of single-addressable units in the message, including 
 * the header. If the message is for a meesage queue on a local processor, then MQX send the 
 * message to the message queue. Otherwise, it is sent to the remote processor. 
 * If the function returns successfully, the message is no longer a resource 
 * of the task.
 *  
 * \param[in] input_msg_ptr Pointer to the message to be sent.
 * 
 * \return TRUE (Success: see Description.) or FALSE (Failure.)
 * 
 * \warning Might dispatch a task.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * 
 * <table>
 *   <tr>
 *     <td><b>Task error code</b></td>
 *     <td><b>Meaning</b></td>
 *     <td><b>Message accepted</b></td>
 *     <td><b>Message freed</b></td>    
 *   </tr>
 *   <tr>
 *     <td>MQX_COMPONENT_DOES_NOT_EXIST</td>
 *     <td>Message component is not created.</td>
 *     <td>No</td>
 *     <td>No</td>    
 *   </tr>
 *   <tr>
 *     <td>MSGQ_INVALID_MESSAGE</td>
 *     <td>msg_ptr is NULL or points to a message that is one of:
 *     \li not valid
 *     \li on a message queue
 *     \li free</td>
 *     <td>No</td>
 *     <td>No</td>    
 *   </tr>
 *   <tr>
 *     <td>MSGQ_INVALID_QUEUE_ID</td>
 *     <td>Target ID is not a valid queue ID.</td>
 *     <td>No</td>
 *     <td>Yes</td>    
 *   </tr>
 *   <tr>
 *     <td>MSGQ_QUEUE_FULL</td>
 *     <td>Target message queue has reached its maximum size.</td>
 *     <td>No</td>
 *     <td>Yes</td>    
 *   </tr>
 *   <tr>
 *     <td>MSGQ_QUEUE_IS_NOT_OPEN</td>
 *     <td>Target ID does not represent an open message queue.</td>
 *     <td>No</td>
 *     <td>Yes</td>    
 *   </tr>
 *   <tr>
 *     <td>Task error codes from _msg_free()</td>
 *     <td>(If message needs to be freed.)</td>
 *     <td>Yes</td>
 *     <td>No</td>    
 *   </tr>        
 * </table>   
 * 
 * \see _msg_alloc_system
 * \see _msg_alloc
 * \see _msgq_open
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks 
 * \see _msgq_receive_until
 * \see _msgq_poll
 * \see _msgq_send_priority
 * \see _msgq_send_urgent
 * \see _msg_free
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT  
 */ 
boolean _msgq_send
(
    pointer input_msg_ptr
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    MESSAGE_HEADER_STRUCT_PTR msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)
    input_msg_ptr;
    boolean result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_msgq_send, msg_ptr, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->TARGET_QID, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->SOURCE_QID);

    result = _msgq_send_internal(msg_ptr, FALSE, msg_ptr->TARGET_QID);

    _KLOGX2(KLOG_msgq_send, result);

    return(result);

} /* Endbody */
/*!
 * \private
 *  
 * \brief Verify the input queue_id and try to send the message directly.
 *
 * This can only be used by tasks.
 * \n The calling task is blocked after the send.
 * 
 * \param[in] input_msg_ptr Pointer to the message being sent by application.
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \see _msgq_send_internal  
 */ 
boolean _msgq_send_blocked_internal
(
    pointer input_msg_ptr
)
{ /* Body */
    boolean result;
    MESSAGE_HEADER_STRUCT_PTR msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)
    input_msg_ptr;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE4(KLOG_msgq_send_blocked, msg_ptr, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->TARGET_QID, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->SOURCE_QID);

    result = _msgq_send_internal(msg_ptr, TRUE, msg_ptr->TARGET_QID);

    _KLOGX2(KLOG_msgq_send_blocked, result);
    return(result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Verify the input queue_id and try to send the message directly.
 * 
 * \param[in] msg_ptr    Pointer to the message being sent by application.
 * \param[in] blocking   Is the calling task to be blocked after the call.
 * \param[in] target_qid The queue to put the message onto.
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_MESSAGE (Msg_ptr is NULL or points to a message that is: not 
 * valid, on a message queue or free}.
 * \li MSGQ_INVALID_QUEUE_ID (Target ID is not a valid queue ID.)
 * \li MSGQ_QUEUE_FULL (Target message queue has reached its maximum size.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN (Target ID does not represent an open message queue.)
 * \li Task error codes from _msg_free() (If message needs to be freed.) 
 */ 
boolean _msgq_send_internal
(
    MESSAGE_HEADER_STRUCT_PTR msg_ptr,
    boolean                   blocking,
    _queue_id                 target_qid
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register MSGQ_STRUCT_PTR             msgq_ptr;
    register TD_STRUCT_PTR               td_ptr;
    MESSAGE_HEADER_STRUCT_PTR            tmp_msg_ptr;
    register _mqx_uint                   state;
    register _queue_number               queue;
    register _processor_number           pnum;
    boolean                              swapped_msg;

    _GET_KERNEL_DATA(kernel_data);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        return(FALSE);
    } /* Endif */
    if (msg_ptr == NULL)
    {
        _task_set_error(MSGQ_INVALID_MESSAGE);
        return(FALSE);
    } /* Endif */
#endif

    imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);

#if MQX_CHECK_ERRORS
    if (imsg_ptr->VALID != MSG_VALID)
    {
        /* An invalid message was input by the application. */
        _task_set_error(MSGQ_INVALID_MESSAGE);
        return FALSE;
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (imsg_ptr->FREE || imsg_ptr->QUEUED)
    {
        /* Trying to send a free message, or one on a message queue. */
        _task_set_error(MSGQ_INVALID_MESSAGE);
        return FALSE;
    } /* Endif */
#endif

    pnum = PROC_NUMBER_FROM_QID(target_qid);

    /* If processor number is zero then the message is for this processor */
    if (pnum == 0)
    {
        /* Fix up the target QID in the message header */
        msg_ptr->TARGET_QID = BUILD_QID(kernel_data->INIT.PROCESSOR_NUMBER,
                        msg_ptr->TARGET_QID);
    }
    else if (pnum != kernel_data->INIT.PROCESSOR_NUMBER)
    {
#if MQX_IS_MULTI_PROCESSOR
        IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR ipc_msg_comp_ptr;
        ipc_msg_comp_ptr = (IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR)
        kernel_data->KERNEL_COMPONENTS[KERNEL_IPC_MSG_ROUTING];
        if (ipc_msg_comp_ptr && ipc_msg_comp_ptr->MSG_ROUTER)
        {
            return( (*ipc_msg_comp_ptr->MSG_ROUTER)(pnum, msg_ptr, blocking));
        }
        else
        {
#endif
            _msg_free(msg_ptr);
            _task_set_error(MSGQ_INVALID_QUEUE_ID);
            return FALSE;
#if MQX_IS_MULTI_PROCESSOR
        }/* Endif */
#endif
    } /* Endif */

    queue = QUEUE_FROM_QID(target_qid);

#if MQX_CHECK_ERRORS
    if ( ! VALID_QUEUE(queue))
    {
        _msg_free(msg_ptr);
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        return FALSE;
    } /* Endif */
#endif

    msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];
    if (msgq_ptr->QUEUE != queue)
    {
        msgq_ptr = NULL;
    } /* Endif */

#if MQX_CHECK_ERRORS
    if (msgq_ptr == NULL)
    {
        _msg_free(msg_ptr);
        _task_set_error(MSGQ_QUEUE_IS_NOT_OPEN);
        return FALSE;
    } /* Endif */
#endif

    _INT_DISABLE();

    if ((msgq_ptr->MAX_ENTRIES == 0) ||
                    (msgq_ptr->NO_OF_ENTRIES < msgq_ptr->MAX_ENTRIES))
    {

        /* There is room on the queue, so add the msg.  We 
         * need to check for room here even if the msg ends up
         * being short-cutted to the receiver (via td_ptr->MESSAGE)
         * in case msg_receive needs to enqueue the msg.
         */

        if (msgq_ptr->TYPE == MSG_QUEUE)
        {
            /* THIS MESSAGE QUEUE IS ATTACHED TO A TASK */

            /* check for pending receive
             * if a receive is pending then satisfy the request
             * and add the receiving task onto the ready-to-run queue
             */

            td_ptr = msgq_ptr->TD_PTR;
            state = td_ptr->STATE & STATE_MASK;
            if ( (state == RCV_ANY_BLOCKED) ||
                            ((state == RCV_SPECIFIC_BLOCKED) && (td_ptr->INFO == queue)))
            {
                /* The task is blocked, waiting for a message */
                td_ptr->MESSAGE = &imsg_ptr->MESSAGE;
                imsg_ptr->TD_PTR = td_ptr;
                _TIME_DEQUEUE(td_ptr,kernel_data);
                _TASK_READY(td_ptr,kernel_data);

                /* Now run the notification function */
                if (msgq_ptr->NOTIFICATION_FUNCTION != NULL)
                {
                    (*msgq_ptr->NOTIFICATION_FUNCTION)(msgq_ptr->NOTIFICATION_FUNCTION_PARAMETER);
                } /* Endif */

                if (blocking)
                {
                    if ( ! kernel_data->IN_ISR)
                    {
                        td_ptr = kernel_data->ACTIVE_PTR;
                        td_ptr->STATE = SEND_BLOCKED;
                        _task_block();
                    } /* Endif */
                }
                else
                {
                    /*
                     *  if the highest priority ready task is not the
                     *  same priority as the sending task, then a higher
                     *  priority task was made ready and it has to be allowed
                     *  to run.
                     */
                    _CHECK_RUN_SCHEDULER(); /* Let a higher priority task run */
                } /* Endif */

            }
            else
            {
                /* The task is ready to run and pre-empted OR blocked and
                 * on a different queue.
                 */

                swapped_msg = FALSE;
                if ((msg_ptr->CONTROL & MSG_PRIORITY_MASK) &&
                                (td_ptr->MESSAGE != NULL))
                {
                    /* Check the message in the TD */
                    tmp_msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)td_ptr->MESSAGE;
                    if ( (msg_ptr->CONTROL & MSG_HDR_URGENT) ||
                                    /* Urgent messages first */
                                    ( (! (tmp_msg_ptr->CONTROL & MSG_HDR_URGENT)) &&
                                                    ( (_mqx_uint)(tmp_msg_ptr->CONTROL & MSG_HDR_PRIORITY_MASK) <
                                                                    (_mqx_uint)(msg_ptr->CONTROL & MSG_HDR_PRIORITY_MASK))
                                    )
                    )
                    /* Higher priority messages first */
                    {
                        /* Put new message into TD */
                        td_ptr->MESSAGE = msg_ptr;

                        /* Set the new message's ownership to the receiving queue's TD */
                        imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);
                        imsg_ptr->TD_PTR = td_ptr;

                        /* Old message which we pulled from TD, need to add to queue, below */
                        msg_ptr = tmp_msg_ptr;
                        imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);

                        /* Don't know the sender's TD for the swapped out msg,
                         * so set it to NULL; 
                         */
                        imsg_ptr->TD_PTR = NULL;
                        /* Indicate that a swap occurred */
                        swapped_msg = TRUE;
                        /* Set the queue to the swapped msg's queue. */
                        if (target_qid != msg_ptr->TARGET_QID)
                        {
                            queue = QUEUE_FROM_QID(msg_ptr->TARGET_QID);
                            msgq_ptr = &msg_component_ptr->MSGQS_PTR[queue];

                            /* This msg's queue was not full when it was short-cut,
                             * so we should not get here.  Check anyway. 
                             */
                            if ((msgq_ptr->MAX_ENTRIES != 0) &&
                                            (msgq_ptr->NO_OF_ENTRIES >= msgq_ptr->MAX_ENTRIES))
                            {
                                /* Queue full, error */
                                _INT_ENABLE();
                                _msg_free(msg_ptr);
                                _task_set_error(MSGQ_QUEUE_FULL);
                                return FALSE;
                            } /* Endif */
                        } /* Endif */
                    } /* Endif */
                } /* Endif */

                /* add the message */
                _msgq_insert_message_internal(msgq_ptr, imsg_ptr, swapped_msg);

                if (msgq_ptr->TD_PTR)
                {
                    ++(msgq_ptr->TD_PTR->MESSAGES_AVAILABLE);
                } /* Endif */

                /* Now run the notification function */
                if (msgq_ptr->NOTIFICATION_FUNCTION != NULL)
                {
                    (*msgq_ptr->NOTIFICATION_FUNCTION)(msgq_ptr->NOTIFICATION_FUNCTION_PARAMETER);
                } /* Endif */

                if (blocking && ! kernel_data->IN_ISR )
                {
                    td_ptr = kernel_data->ACTIVE_PTR;
                    td_ptr->STATE = SEND_BLOCKED;
                    _task_block();
                } /* Endif */

            } /* Endif */

        }
        else
        {
            /* THIS IS A SYSTEM QUEUE NOT ATTACHED TO A TASK */

            /* add the message to the queue */
            _msgq_insert_message_internal(msgq_ptr, imsg_ptr, FALSE);

            /* Run the notification function. */
            if ( msgq_ptr->NOTIFICATION_FUNCTION != NULL )
            {
                (*msgq_ptr->NOTIFICATION_FUNCTION)(msgq_ptr->NOTIFICATION_FUNCTION_PARAMETER);
            } /* Endif */

        } /* Endif */

    }
    else
    {
        /* Queue full, error */
        _INT_ENABLE();
        _task_set_error(MSGQ_QUEUE_FULL);
        _msg_free(&imsg_ptr->MESSAGE);
        return FALSE;
    } /* Endif */

    _INT_ENABLE();
    return TRUE; /* Message sent MQX_OK */

} /* Endbody */

/*!
 * \private
 * 
 * \brief Inserts the message into the message queue according to its priority 
 * and urgency.
 * 
 * \param[in] msgq_ptr    The message queue to put the message onto.
 * \param[in] imsg_ptr    The message which is to be inserted.
 * \param[in] swapped_msg Was this message swapped out from the TD?
 */ 
void _msgq_insert_message_internal
(
    register MSGQ_STRUCT_PTR             msgq_ptr,
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr,
    register boolean                     swapped_msg
)
{ /* Body */
    register INTERNAL_MESSAGE_STRUCT_PTR prev_imsg_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR next_imsg_ptr;
    _mqx_uint                            control;
    _mqx_uint                            priority;

    if (msgq_ptr->FIRST_MSG_PTR == NULL)
    {
        imsg_ptr->PREV = NULL;
        imsg_ptr->NEXT = NULL;
        msgq_ptr->FIRST_MSG_PTR = imsg_ptr;
        msgq_ptr->LAST_MSG_PTR = imsg_ptr;
    }
    else
    {
        if (imsg_ptr->MESSAGE.CONTROL & MSG_HDR_URGENT || swapped_msg)
        {
            /* Insert at front if urgent 
             * OR if we are dealing with message previously in TD that was pulled 
             * out by a higher priority message. 
             */
            imsg_ptr->NEXT = msgq_ptr->FIRST_MSG_PTR;
            imsg_ptr->PREV = NULL;
            (msgq_ptr->FIRST_MSG_PTR)->PREV = imsg_ptr;
            msgq_ptr->FIRST_MSG_PTR = imsg_ptr;
        }
        else if (imsg_ptr->MESSAGE.CONTROL & MSG_HDR_PRIORITY_MASK)
        {
            /* We must insert the message into the queue */
            priority = imsg_ptr->MESSAGE.CONTROL & MSG_HDR_PRIORITY_MASK;
            prev_imsg_ptr = NULL;
            next_imsg_ptr = msgq_ptr->FIRST_MSG_PTR;
            while (next_imsg_ptr)
            {
                control = next_imsg_ptr->MESSAGE.CONTROL & MSG_HDR_PRIORITY_MASK;
                if (priority > control)
                {
                    break;
                } /* Endif */
                prev_imsg_ptr = next_imsg_ptr;
                next_imsg_ptr = next_imsg_ptr->NEXT;
            } /* Endwhile */

            if (prev_imsg_ptr == NULL)
            {
                /* Insert at front */
                imsg_ptr->NEXT = msgq_ptr->FIRST_MSG_PTR;
                imsg_ptr->PREV = NULL;
                (msgq_ptr->FIRST_MSG_PTR)->PREV = imsg_ptr;
                msgq_ptr->FIRST_MSG_PTR = imsg_ptr;
            }
            else if (next_imsg_ptr == NULL)
            {
                /* Insert at end */
                imsg_ptr->PREV = msgq_ptr->LAST_MSG_PTR;
                imsg_ptr->NEXT = NULL;
                (msgq_ptr->LAST_MSG_PTR)->NEXT = imsg_ptr;
                msgq_ptr->LAST_MSG_PTR = imsg_ptr;
            }
            else
            {
                /* Insert in middle */
                imsg_ptr->PREV = prev_imsg_ptr;
                imsg_ptr->NEXT = next_imsg_ptr;
                prev_imsg_ptr->NEXT = imsg_ptr;
                next_imsg_ptr->PREV = imsg_ptr;
            } /* Endif */
        }
        else
        {
            /* Tack on to end */
            imsg_ptr->PREV = msgq_ptr->LAST_MSG_PTR;
            imsg_ptr->NEXT = NULL;
            (msgq_ptr->LAST_MSG_PTR)->NEXT = imsg_ptr;
            msgq_ptr->LAST_MSG_PTR = imsg_ptr;
        } /* Endif */
    } /* Endif */

    ++(msgq_ptr->NO_OF_ENTRIES);
    imsg_ptr->QUEUED = TRUE;

} /* Endbody */

/*!
 * \brief Sends the priority message to the message queue.
 * 
 * The function inserts the message in a message queue based on the priority of 
 * the message; it inserts higher-priority messages ahead of lower-priority ones. 
 * Messages with the same priority are inserted in FIFO order.
 * \n If the function returns successfully, the message is no longer a resource 
 * of the task.
 * \n Messages sent with _msgq_send() and _msgq_send_broadcast() are priority 0 
 * messages.
 * 
 * \param[in] input_msg_ptr Pointer to the message to be sent.
 * \param[in] priority      Priority of the message, between:
 * \li 0 (Lowest.)
 * \li MSG_MAX_PRIORITY (highest; 15.)
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \warning Might dispatch a task.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li See _msgq_send() for error codes.
 * \li MSGQ_INVALID_MESSAGE_PRIORITY (Priority is greater than MSG_MAX_PRIORITY 
 * (message is not accepted and is not freed).)
 * 
 * \see _msgq_send   
 * \see _msg_alloc_system
 * \see _msg_alloc
 * \see _msgq_send_broadcast
 * \see _msgq_send_urgent
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks 
 * \see _msgq_receive_until
 * \see _msgq_poll
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT     
 */ 
boolean _msgq_send_priority
(
    pointer   input_msg_ptr,
    _mqx_uint priority

)
{ /* Body */
    boolean result;
    MESSAGE_HEADER_STRUCT_PTR msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)
    input_msg_ptr;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE6(KLOG_msgq_send_priority, msg_ptr, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->TARGET_QID, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->SOURCE_QID, priority, urgent);

    if (priority > MSG_MAX_PRIORITY)
    {
        _task_set_error(MSGQ_INVALID_MESSAGE_PRIORITY);
        _KLOGX2(KLOG_msgq_send_priority, FALSE);
        return FALSE;
    } /* Endif */

    msg_ptr->CONTROL &= 0xF0;
    msg_ptr->CONTROL |= (priority & 0xF);

    result = _msgq_send_internal(msg_ptr, FALSE, msg_ptr->TARGET_QID);

    _KLOGX2(KLOG_msgq_send_priority, result);
    return(result);

} /* Endbody */

/*!
 * \brief Sends the message directly to the private or system message queue.
 * 
 * The function uses qid instead of the target queue ID that the message header 
 * specifies and sends the message as described for _msgq_send().
 * \n If the function returns successfully, the message is no longer a resource 
 * of the task.
 * 
 * \param[in] msg_ptr Pointer to the message to be sent.
 * \param[in] qid     Message queue into which to put the message.
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \warning Might dispatch a task
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li See _msgq_send() for error codes.
 * 
 * \see _msgq_send   
 * \see _msgq_send_broadcast
 * \see _msgq_send_urgent
 * \see _msgq_send_priority
 * \see _msg_alloc_system
 * \see _msg_alloc
 * \see _msgq_open
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks 
 * \see _msgq_receive_until
 * \see _msgq_poll
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT
 */ 
boolean _msgq_send_queue
(
    pointer   msg_ptr,
    _queue_id qid
)
{ /* Body */
    boolean result;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_msgq_send_queue, msg_ptr, qid, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->SOURCE_QID);

    result = _msgq_send_internal((MESSAGE_HEADER_STRUCT_PTR)msg_ptr, FALSE, qid);

    _KLOGX2(KLOG_msgq_send_queue, result);
    return(result);

} /* Endbody */

/*!
 * \brief Sends the urgent message to the message queue.
 * 
 * The function sends the message as described for _msgq_send().
 * \n The function puts the message at the start of the message queue, ahead of 
 * any other urgent messages.
 * \n If the function returns successfully, the message is no longer a resource 
 * of the task.
 * 
 * \param[in] input_msg_ptr Pointer to the message to be sent.
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \warning Might dispatch a task
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li See _msgq_send() for error codes.
 * 
 * \see _msgq_send   
 * \see _msgq_send_priority
 * \see _msgq_send_queue
 * \see _msg_alloc_system
 * \see _msg_alloc
 * \see _msgq_receive
 * \see _msgq_receive_for
 * \see _msgq_receive_ticks 
 * \see _msgq_receive_until
 * \see _msgq_poll
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT    
 */ 
boolean _msgq_send_urgent
(
    pointer input_msg_ptr
)
{ /* Body */
    boolean result;
    MESSAGE_HEADER_STRUCT_PTR msg_ptr = (MESSAGE_HEADER_STRUCT_PTR)
    input_msg_ptr;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE4(KLOG_msgq_send_urgent, msg_ptr, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->TARGET_QID, ((MESSAGE_HEADER_STRUCT_PTR)msg_ptr)->SOURCE_QID);

    msg_ptr->CONTROL |= MSG_HDR_URGENT;

    result = _msgq_send_internal(msg_ptr, FALSE, msg_ptr->TARGET_QID);

    _KLOGX2(KLOG_msgq_send_urgent, result);
    return(result);

} /* Endbody */

/*!
 * \brief Opens the system message queue.
 * 
 * Once a system message queue is opened, any task can use the queue ID to receive 
 * messages with _msgq_poll().
 * \li Tasks cannot receive messages from system message queues with _msgq_receive().
 * \li The notification function can get messages from the message queue with 
 * _msgq_poll().
 * \li A task can change the notification function and its data with 
 * _msgq_set_notification_function().
 * 
 * \param[in] queue                 One of the following:
 * \li System message queue to be opened (min. 8, max. as defined in the MQX 
 * initialization structure).
 * \li MSGQ_FREE_QUEUE (MQX chooses an unopened system queue number.)
 * \param[in] max_queue_size        One of the following:
 * \li Maximum queue size.
 * \li 0 (Unlimited size.)
 * \param[in] notification_function One of the following:              
 * \li Pointer to the function that MQX calls when it puts a message in the queue.
 * \li NULL (MQX does not call a function when it puts a message in the queue.)
 * \param[in] notification_data     Data that MQX passes when it calls 
 * notification_function.
 * 
 * \return Queue ID (Success.)
 * \return 0 (Failure.)
 * 
 * \warning Creates the message component if it was not previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li See _msgq_open() for error codes
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGQ_INVALID_QUEUE_ID (Queue_id is not valid.)
 * \li MSGQ_MESSAGE_NOT_AVAILABLE (There are no messages in the message queue.)
 * \li MSGQ_NOT_QUEUE_OWNER (Task is not the owner of the private message queue.)
 * \li MSGQ_QUEUE_IS_NOT_OPEN Queue is not open.  
 * 
 * \see _msgq_close
 * \see _msgq_open
 * \see _msgq_poll
 * \see _msgq_set_notification_function
 * \see _task_set_error  
 */ 
_queue_id _msgq_open_system
(
    _queue_number          queue,
    uint_16                max_queue_size,
    MSGQ_NOTIFICATION_FPTR notification_function,
    pointer                notification_data
)
{ /* Body */
    _queue_id result;
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE5(KLOG_msgq_open_system, queue, max_queue_size, notification_function, notification_data);

    result = _msgq_open_internal(queue, max_queue_size, SYSTEM_MSG_QUEUE,
                    notification_function, notification_data);

    _KLOGX2(KLOG_msgq_open_system, result);

    return(result);

} /* Endbody */

/*!
 * \brief Tests all messages in all open message queues for validity and consistency.
 * 
 * The function checks the consistency and validity of all messages in all 
 * private and system message queues that are open.
 * 
 * \param[out] queue_error_ptr Pointer to the message queue that has a message 
 * with an error (initialized only if an error is found).
 * \param[out] msg_error_ptr   Pointer to the message that has an error 
 * (initialized only if an error is found).
 * 
 * \return MQX_OK (Success: no errors are found.)
 * \return MSGQ_INVALID_MESSAGE (Success: an error is found.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Failure: Message component is not created.)
 * 
 * \warning Disables and enables interrupts.
 * 
 * \see _msgq_open
 * \see _msgq_open_system 
 */ 
_mqx_uint _msgq_test
(
    pointer _PTR_ queue_error_ptr,
    pointer _PTR_ msg_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MSG_COMPONENT_STRUCT_PTR    msg_component_ptr;
    MSGQ_STRUCT_PTR             msgq_ptr;
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    _mqx_uint                   i,j;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_msgq_test, queue_error_ptr, msg_error_ptr);
    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _KLOGX2(KLOG_msgq_test, MQX_COMPONENT_DOES_NOT_EXIST);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    /* test the message queues */
    msgq_ptr = &msg_component_ptr->MSGQS_PTR[MSGQ_FIRST_USER_QUEUE];
    for (i = 1; i <= msg_component_ptr->MAX_MSGQS; i++)
    {
        _int_disable();
        if (msgq_ptr->QUEUE == i)
        {
            /* Queue is open */
            imsg_ptr = msgq_ptr->FIRST_MSG_PTR;
            for (j = 0; j < msgq_ptr->NO_OF_ENTRIES; ++j)
            {
                if ((imsg_ptr->VALID != MSG_VALID) && (!imsg_ptr->QUEUED))
                {
                    _int_enable();
                    *queue_error_ptr = msgq_ptr;
                    *msg_error_ptr = imsg_ptr;
                    _KLOGX4(KLOG_msgq_test, MSGQ_INVALID_MESSAGE, msgq_ptr,
                                    imsg_ptr);
                    return(MSGQ_INVALID_MESSAGE);
                } /* Endif */
                imsg_ptr = imsg_ptr->NEXT;
            } /* Endfor */
        } /* Endif */
        _int_enable();
        ++msgq_ptr;
    } /* Endfor */

    _KLOGX2(KLOG_msgq_test, MQX_OK);
    return(MQX_OK);

} /* Endbody */

#endif /* MQX_USE_MESSAGES */

/* EOF */
