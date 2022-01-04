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
 * $FileName: msg.c$
 * $Version : 3.8.2.0$
 * $Date    : Jun-25-2012$
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

/*!
 * \brief Allocates a message from the private message pool.
 * 
 * The size of the message is determined by the message size that a task 
 * specified when it called _msgpool_create(). The message is a resource of the 
 * task until the task either frees it (_msg_free()) or puts it on a message 
 * queue (_msgq_send() family of functions.)
 * 
 * \param[in] pool A pool ID from _msgpool_create().
 * 
 * \return Pointer to a message (Success.)
 * \return NULL (Failure.)
 *  
 * \warning On failure, calls _task_set_error() to set one the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li MSGPOOL_INVALID_POOL_ID (Pool_id is not valid.)
 * \li MSGPOOL_OUT_OF_MESSAGES (All the messages in the pool are allocated.)
 * \li Task error codes from _mem_alloc_system() (If MQX needs to grow the pool.)
 * 
 * \see _msg_alloc_system
 * \see _msg_free
 * \see _msgpool_create
 * \see _msgpool_destroy
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
 * \see MESSAGE_HEADER_STRUCT     
 */ 
pointer _msg_alloc
(
    _pool_id pool
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
#if MQX_CHECK_ERRORS
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
#endif
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register MESSAGE_HEADER_STRUCT_PTR   message_ptr;
    register MSGPOOL_STRUCT_PTR          msgpool_ptr;
    uint_16                              grow_number;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_msg_alloc, pool);

#if MQX_CHECK_ERRORS
    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX3(KLOG_msg_alloc, NULL, MQX_COMPONENT_DOES_NOT_EXIST);
        return(NULL);
    } /* Endif */
#endif

    message_ptr = NULL;
    msgpool_ptr = (MSGPOOL_STRUCT_PTR)pool;

#if MQX_CHECK_VALIDITY || MQX_CHECK_ERRORS
    if (
#if MQX_CHECK_VALIDITY
                    (msgpool_ptr->VALID != MSG_VALID)
#if MQX_CHECK_ERRORS
                    ||
#endif
#endif
#if MQX_CHECK_ERRORS
                    (msgpool_ptr->MSGPOOL_TYPE != MSG_POOL)
#endif
    )
    {
        _task_set_error(MSGPOOL_INVALID_POOL_ID);
        _KLOGX3(KLOG_msg_alloc, message_ptr, MSGPOOL_INVALID_POOL_ID);
        return (message_ptr);
    } /* Endif */
#endif

    if ( (msgpool_ptr->SIZE == 0) && (msgpool_ptr->GROW_NUMBER) &&
                    ( (msgpool_ptr->MAX < msgpool_ptr->GROW_LIMIT) ||
                                    (msgpool_ptr->GROW_LIMIT == 0) ) )
    {
        /* Attempt to add elements to the pool */
        grow_number = msgpool_ptr->GROW_NUMBER;
        if (grow_number > (uint_16)(msgpool_ptr->GROW_LIMIT - msgpool_ptr->MAX))
        {
            grow_number = msgpool_ptr->GROW_LIMIT - msgpool_ptr->MAX;
        } /* Endif */
        _msgpool_add_internal(msgpool_ptr, grow_number);
    } /* Endif */

    _INT_DISABLE();
    imsg_ptr = msgpool_ptr->MSG_FREE_LIST_PTR;
    if (imsg_ptr == NULL)
    {
        _int_enable();
        _task_set_error(MSGPOOL_OUT_OF_MESSAGES);
        _KLOGX3(KLOG_msg_alloc, message_ptr, MSGPOOL_OUT_OF_MESSAGES);
        return((pointer)message_ptr);
    } /* Endif */

    msgpool_ptr->MSG_FREE_LIST_PTR = imsg_ptr->NEXT;
    --msgpool_ptr->SIZE;
    _INT_ENABLE();
    imsg_ptr->FREE = FALSE;
    imsg_ptr->QUEUED = FALSE;
    if (kernel_data->IN_ISR)
    {
        imsg_ptr->TD_PTR = NULL;
    }
    else
    {
        imsg_ptr->TD_PTR = kernel_data->ACTIVE_PTR;
    } /* Endif */
    message_ptr = (MESSAGE_HEADER_STRUCT_PTR)&imsg_ptr->MESSAGE;
    message_ptr->TARGET_QID = MSGQ_NULL_QUEUE_ID;
    message_ptr->SOURCE_QID = MSGQ_NULL_QUEUE_ID;
    message_ptr->SIZE = msgpool_ptr->MESSAGE_SIZE;
    message_ptr->CONTROL = MSG_HDR_ENDIAN | MSG_DATA_ENDIAN;
    _KLOGX3(KLOG_msg_alloc, message_ptr, MQX_OK);
    return (pointer)message_ptr;

} /* Endbody */

/*!
 * \brief Creates the message component.
 * 
 * The function uses fields in the MQX initialization structure to create the 
 * number of message pools (MAX_MSGPOOLS) and message queues (MAX_MSGQS). MQX 
 * creates the message component if it is not created when an application calls 
 * one of:
 * \li _msgpool_create()
 * \li _msgpool_create_system()
 * \li _msgq_open()
 * \li _msgq_open_system()
 * 
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * \return MSGPOOL_POOL_NOT_CREATED (MQX cannot allocate the data structures for 
 * message pools.)
 * \return MSGQ_TOO_MANY_QUEUES (MQX cannot allocate the data structures for 
 * message queues.)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.) 
 * \return MQX_INVALID_LWSEM (Sem_ptr is for a lightweight semaphore that is not 
 * longer valid.)
 * \return MQX_LWSEM_WAIT_TIMEOUT (Timeout expired before the task could get the 
 * lightweight semaphore.)       
 */ 
_mqx_uint _msg_create_component(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR            kernel_data;
    register MSG_COMPONENT_STRUCT_PTR msg_component_ptr;
    pointer                           pools_ptr;
    pointer                           msgqs_ptr;
    _mqx_uint                         error;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE1(KLOG_msg_create_component);

    error = _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
#if MQX_CHECK_ERRORS
    if (error != MQX_OK)
    {
        _KLOGX2(KLOG_msg_create_component, error);
        return(error);
    } /* Endif */
#endif

    if (kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES] != NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_msg_create_component, MQX_OK);
        return(MQX_OK);
    } /* Endif */

    msg_component_ptr = (MSG_COMPONENT_STRUCT_PTR)
    _mem_alloc_system_zero((_mem_size)sizeof(MSG_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (msg_component_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_msg_create_component, MQX_OUT_OF_MEMORY);
        return(MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif
    _mem_set_type(msg_component_ptr, MEM_TYPE_MESSAGE_COMPONENT);

    if (kernel_data->INIT.MAX_MSGPOOLS == 0)
    {
        kernel_data->INIT.MAX_MSGPOOLS = 1;
    } /* Endif */
    pools_ptr = _mem_alloc_system_zero((_mem_size)(kernel_data->INIT.MAX_MSGPOOLS *
                                    sizeof(MSGPOOL_STRUCT)));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (pools_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_msg_create_component, MSGPOOL_POOL_NOT_CREATED);
        return MSGPOOL_POOL_NOT_CREATED;
    }/* Endif */
#endif
    _mem_set_type(pools_ptr, MEM_TYPE_MESSAGE_POOLS);

    if (kernel_data->INIT.MAX_MSGQS >= MAX_UINT_16)
    {
        kernel_data->INIT.MAX_MSGQS = MAX_UINT_16 - 1;
    }
    else if (kernel_data->INIT.MAX_MSGQS < 1)
    {
        kernel_data->INIT.MAX_MSGQS = 1;
    } /* Endif */
    msgqs_ptr = _mem_alloc_system_zero( (_mem_size)((kernel_data->INIT.MAX_MSGQS + 1) *
                                    sizeof(MSGQ_STRUCT)));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (msgqs_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);
        _mem_free(pools_ptr);
        _KLOGX2(KLOG_msg_create_component, MSGQ_TOO_MANY_QUEUES);
        return MSGQ_TOO_MANY_QUEUES;
    } /* Endif */
#endif
    _mem_set_type(msgqs_ptr, MEM_TYPE_MESSAGE_QUEUES);

    if (msg_component_ptr->MSGPOOLS_PTR == NULL)
    {
        msg_component_ptr->MAX_MSGPOOLS_EVER = 0;
        msg_component_ptr->SMALLEST_MSGPOOL_PTR = NULL;
        msg_component_ptr->LARGEST_MSGPOOL_PTR = NULL;
        msg_component_ptr->MAX_MSGPOOLS = kernel_data->INIT.MAX_MSGPOOLS;
        msg_component_ptr->MAX_MSGQS = kernel_data->INIT.MAX_MSGQS;
        msg_component_ptr->MSGPOOLS_PTR = (MSGPOOL_STRUCT_PTR)pools_ptr;
        pools_ptr = NULL;
        msg_component_ptr->MSGQS_PTR = (MSGQ_STRUCT_PTR)msgqs_ptr;
        msgqs_ptr = NULL;
    }/* Endif */

    msg_component_ptr->VALID = MESSAGE_VALID;

    kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES] = msg_component_ptr;

#if MQX_COMPONENT_DESTRUCTION
    kernel_data->COMPONENT_CLEANUP[KERNEL_MESSAGES] = _msg_cleanup;
#endif

    _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM);

    if (pools_ptr)
    {
        _mem_free(pools_ptr);
    }/* Endif */
    if (msgqs_ptr)
    {
        _mem_free(msgqs_ptr);
    }/* Endif */

    _KLOGX2(KLOG_msg_create_component, MQX_OK);
    return MQX_OK;

} /* Endbody */

/*!
 * \brief Marks the message as "free".
 * 
 * Only the task that has the message as its resource can free the message. A 
 * message becomes a task's resource when the task allocates the message, and it 
 * continues to be a resource until the task either frees it or puts it in a 
 * message queue. A message becomes a resource of the task that got it from a 
 * message queue.
 * \n The function returns the message to the message pool from which it was 
 * allocated.
 * 
 * \param[in] msg_ptr Pointer to a message struct which is to be freed.
 * 
 * \warning On failure, calls _task_set_error() to set one the following task 
 * error codes:
 * \li MQX_INVALID_POINTER (Msg_ptr does not point to a valid message.)
 * \li MQX_NOT_RESOURCE_OWNER (Message is already freed.)
 * \li MSGQ_MESSAGE_IS_QUEUED (Message is in a queue.)
 * 
 * \see _msgpool_create
 * \see _msgpool_create_system
 * \see _msgpool_destroy
 * \see _msg_alloc_system
 * \see _msg_alloc
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT    
 */ 
void _msg_free
(
    pointer msg_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register MSGPOOL_STRUCT_PTR          msgpool_ptr;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_msg_free, msg_ptr);

    imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);
#if MQX_CHECK_VALIDITY
    if ( imsg_ptr->VALID != MSG_VALID )
    {
        _KLOGX2(KLOG_msg_free, MQX_INVALID_POINTER);
        _task_set_error(MQX_INVALID_POINTER);
        return;
    } /* Endif */
#endif

#if MQX_CHECK_ERRORS
    if (imsg_ptr->FREE)
    {
        _KLOGX2(KLOG_msg_free, MQX_NOT_RESOURCE_OWNER);
        _task_set_error(MQX_NOT_RESOURCE_OWNER);
        return;
    } /* Endif */
    if (imsg_ptr->QUEUED)
    {
        _KLOGX2(KLOG_msg_free, MSGQ_MESSAGE_IS_QUEUED);
        _task_set_error(MSGQ_MESSAGE_IS_QUEUED);
        return;
    } /* Endif */
#endif

    msgpool_ptr = imsg_ptr->MSGPOOL_PTR;
    imsg_ptr->FREE = TRUE;
    imsg_ptr->QUEUED = FALSE;

    _INT_DISABLE();
    /* Link onto the free list */
    imsg_ptr->NEXT = msgpool_ptr->MSG_FREE_LIST_PTR;
    msgpool_ptr->MSG_FREE_LIST_PTR = imsg_ptr;
    ++msgpool_ptr->SIZE;
    _INT_ENABLE();

    _KLOGX2(KLOG_msg_free, MQX_OK);

} /* Endbody */

/*!
 * \brief Gets the number of free messages in the message pool.
 * 
 * The function fails if either:
 * \li Message component is not created.
 * \li Pool_id is for a private message pool, but does not represent a valid one.
 * 
 * \param[in] pool One of the following:
 * \li Private message pool for which to get the number of free messages.
 * \li MSGPOOL_NULL_POOL_ID (for system message pools).
 * 
 * \return The number of free messages in the private message pool (success).
 * \return The number of free messages in all system message pools (success).
 * \return 0 (Success: No free messages.)
 * \return 0 (Failure: see Description.)
 * 
 * \warning If pool_id does not represent a valid private message pool, calls 
 * _task_set_error() to set the task error code to MSGPOOL_INVALID_POOL_ID
 * 
 * \see _msgpool_create
 * \see _msgpool_destroy
 * \see _msg_free
 * \see _msg_alloc_system
 * \see _task_set_error
 * \see _msg_create_component      
 */ 
_mqx_uint _msg_available
(
    _pool_id pool
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MSG_COMPONENT_STRUCT_PTR    msg_component_ptr;
    register MSGPOOL_STRUCT_PTR msgpool_ptr;
    _mqx_uint                   i;
    _mqx_uint                   count;

    _GET_KERNEL_DATA(kernel_data);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        return(0);
    } /* Endif */
#endif

    msgpool_ptr = msg_component_ptr->MSGPOOLS_PTR;
#if MQX_CHECK_ERRORS
    if (msgpool_ptr == NULL)
    {
        return(0);
    }/* Endif */
#endif

    if (pool == MSGPOOL_NULL_POOL_ID)
    {

        count = 0;
        _INT_DISABLE();
        i = msg_component_ptr->MAX_MSGPOOLS_EVER + 1;
        while ( --i )
        {
            if (
#if MQX_CHECK_VALIDITY
                (msgpool_ptr->VALID == MSG_VALID) &&
#endif
                (msgpool_ptr->MSGPOOL_TYPE == SYSTEM_MSG_POOL))
            {
                count += msgpool_ptr->SIZE;
            } /* Endif */
            ++msgpool_ptr;
        } /* Endwhile */
        _INT_ENABLE();
        return count;

    }
    else
    {

        msgpool_ptr = (MSGPOOL_STRUCT_PTR)pool;
        if (
#if MQX_CHECK_VALIDITY
            (msgpool_ptr->VALID != MSG_VALID) ||
#endif
            (msgpool_ptr->MSGPOOL_TYPE != MSG_POOL) )
        {
            _task_set_error(MSGPOOL_INVALID_POOL_ID);
            return (0);
        } /* Endif */
        return (_mqx_uint)msgpool_ptr->SIZE;

    } /* Endif */

} /* Endbody */

/*!
 * \brief Converts the messages header from intel to motorola byte order (or vice 
 * versa).
 * 
 * The function is not needed for general application code because the IPC 
 * component converts the message header. Use it only if you are writing IPC 
 * message drivers for a new BSP.
 * \n The function calls _mem_swap_endian() and uses the field sizes of 
 * MESSAGE_HEADER_STRUCT to convert the header to the other endian format.
 * \n The macro MSG_MUST_CONVERT_HDR_ENDIAN determines whether the message 
 * header needs to be converted to the other endian format.
 * 
 * \param[in] message_ptr Pointer to a message whosenn header is to be converted.
 * 
 * \warning Sets CONTROL in the message header to indicate the correct endian 
 * format for the processor.
 * 
 * \see _mem_swap_endian
 * \see _msg_swap_endian_data
 * \see MSG_MUST_CONVERT_HDR_ENDIAN
 * \see MESSAGE_HEADER_STRUCT  
 */ 
static const uchar _KRNL_Hdr_def[] =
{
    sizeof(_msg_size),
#if MQX_USE_32BIT_MESSAGE_QIDS
    sizeof(uint_16),
#endif
    sizeof(_queue_id),
    sizeof(_queue_id),
    0
};

void _msg_swap_endian_header
(
    register MESSAGE_HEADER_STRUCT_PTR message_ptr
)
{ /* Body */

    _mem_swap_endian( (uchar _PTR_)_KRNL_Hdr_def, (pointer)&(message_ptr->SIZE));
    message_ptr->CONTROL &= ~MSG_HDR_ENDIAN_MASK;
    message_ptr->CONTROL |= MSG_HDR_ENDIAN;

} /* Endbody */

/*!
 * \brief Converts the data portion of the message from intel to motorola byte 
 * order ( or vice versa ).
 * 
 * The function calls _mem_swap_endian() and uses definition to swap 
 * single-addressable units: message_ptr + sizeof(MESSAGE_HEADER_STRUCT)
 * \n The macro MSG_MUST_CONVERT_DATA_ENDIAN determines whether the data portion 
 * of the message needs to be converted to the other endian format.
 * 
 * \param[in] definition  Pointer to an array (NULL-terminated), each element of 
 * which defines the size (in single-addressable units) of fields in the data 
 * portion of the message.
 * \param[in] message_ptr Pointer to the message whose data is to be converted.
 * 
 * \warning Sets CONTROL in the message header to indicate the correct endian 
 * format for the processor.
 * 
 * \see _mem_swap_endian
 * \see MSG_MUST_CONVERT_DATA_ENDIAN
 * \see MESSAGE_HEADER_STRUCT 
 */ 
void _msg_swap_endian_data
(
    uchar _PTR_                        definition,
    register MESSAGE_HEADER_STRUCT_PTR message_ptr
)
{ /* Body */

    _mem_swap_endian( definition, (pointer)((uchar _PTR_)message_ptr +
                                    sizeof(MESSAGE_HEADER_STRUCT)) );
    message_ptr->CONTROL &= ~MSG_DATA_ENDIAN_MASK;
    message_ptr->CONTROL |= MSG_DATA_ENDIAN;

} /* Endbody */

/*!
 * \brief This function is called when a task is being destroyed.
 * 
 * Closes all open message queues and then free all messages owned by this task.
 * 
 * \param[in] td_ptr The task descriptor of the task that is being destroyed.   
 */ 
void _msg_cleanup
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MSG_COMPONENT_STRUCT_PTR    msg_component_ptr;
    MSGQ_STRUCT_PTR             msgq_ptr;
    MSGQ_STRUCT_PTR             qprev_ptr;
    MSGPOOL_STRUCT_PTR          msgpool_ptr;
    MSGPOOL_BLOCK_STRUCT_PTR    msgpool_block_ptr;
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    INTERNAL_MESSAGE_STRUCT_PTR tmp_imsg_ptr;
    _mqx_uint                   i,j,raw_message_size;

    _GET_KERNEL_DATA(kernel_data);
    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        return;
    } /* Endif */
#endif

    /* Delete the message queues owned by the task, and free the messages
     * on the queues
     */
    msgq_ptr = (MSGQ_STRUCT_PTR)td_ptr->MSG_QUEUE_HEAD;
    while (msgq_ptr != NULL)
    {
        if (msgq_ptr->TD_PTR != NULL)
        {
            msgq_ptr->TD_PTR->MESSAGES_AVAILABLE -= msgq_ptr->NO_OF_ENTRIES;
        } /* Endif */

        imsg_ptr = msgq_ptr->FIRST_MSG_PTR;
        while ( imsg_ptr != NULL )
        {
            tmp_imsg_ptr = imsg_ptr->NEXT;
            imsg_ptr->QUEUED = FALSE;
            _msg_free((pointer)&imsg_ptr->MESSAGE);
            imsg_ptr = tmp_imsg_ptr;
        } /* Endwhile */
        qprev_ptr = msgq_ptr;
        msgq_ptr = msgq_ptr->NEXT_MSGQ_PTR;
        qprev_ptr->FIRST_MSG_PTR = NULL;
        qprev_ptr->NEXT_MSGQ_PTR = NULL;
        qprev_ptr->QUEUE = 0;
    } /* Endwhile */

    td_ptr->MSG_QUEUE_HEAD = NULL;
    td_ptr->MSG_QUEUE_TAIL = NULL;

    /* Search through all of the message pools, looking for any messages
     * owned by this task.  If found free them.
     */
    msgpool_ptr = msg_component_ptr->MSGPOOLS_PTR;
    i = msg_component_ptr->MAX_MSGPOOLS + 1;
    while (--i)
    {
        if (msgpool_ptr->VALID == MSG_VALID)
        {

            /* Search through all of the message pool blocks for this pool */
            msgpool_block_ptr = msgpool_ptr->MSGPOOL_BLOCK_PTR;
            while (msgpool_block_ptr != NULL)
            {
                raw_message_size = msgpool_block_ptr->RAW_MESSAGE_SIZE;
                imsg_ptr = (INTERNAL_MESSAGE_STRUCT_PTR)
                ((uchar _PTR_)msgpool_block_ptr + sizeof(MSGPOOL_BLOCK_STRUCT));

                /* if the message is not free, not queued and the
                 * owner is the task being destroyed, then Free the message
                 */
                j = msgpool_block_ptr->NUM_MESSAGES + 1;
                while (--j)
                {
                    if ( (imsg_ptr->TD_PTR == td_ptr) &&
                                    (imsg_ptr->FREE == FALSE) &&
                                    (imsg_ptr->QUEUED == FALSE) )
                    {
                        _msg_free(&imsg_ptr->MESSAGE);
                    } /* Endif */
                    imsg_ptr =(INTERNAL_MESSAGE_STRUCT_PTR)
                    ((uchar _PTR_)imsg_ptr + raw_message_size);
                } /* Endwhile */

                msgpool_block_ptr = msgpool_block_ptr->NEXT_BLOCK_PTR;
            } /* Endwhile */

        } /* Endif */
        msgpool_ptr++;
    } /* Endwhile */

} /* Endbody */

/*!
 * \brief Allocates a message from a system message pool.
 * 
 * The size of the message is determined by the message size that a task 
 * specified when it called _msgpool_create_system().
 * \n The message is a resource of the task until the task either frees it 
 * (_msg_free()) or puts it on a message queue (_msgq_send family of functions.)
 * 
 * \param[in] message_size Maximum size (in single-addressable units) of the 
 * message.
 * 
 * \return Pointer to a message of at least message_size single-addressable 
 * units (success).
 * \return NULL (Failure: message component is not created.)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \li Task error codes from _mem_alloc_system() (If MQX needs to grow the pool.)
 * 
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
 * \see _msg_alloc
 * \see _msg_free
 * \see _msgpool_create_system
 * \see _msgq_send
 * \see _task_set_error
 * \see MESSAGE_HEADER_STRUCT 
 */ 
pointer _msg_alloc_system
(
    _msg_size message_size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    MSG_COMPONENT_STRUCT_PTR             msg_component_ptr;
    register INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    register MESSAGE_HEADER_STRUCT_PTR   message_ptr;
    register MSGPOOL_STRUCT_PTR          msgpool_ptr;
    uint_16                              grow_number;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_msg_alloc_system, message_size );

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
        _KLOGX2( KLOG_msg_alloc_system, NULL );
        return(NULL);
    }/* Endif */
#endif

    message_ptr = NULL;
    _INT_DISABLE();
    msgpool_ptr = msg_component_ptr->SMALLEST_MSGPOOL_PTR;
    while (msgpool_ptr != NULL)
    {
        if (msgpool_ptr->MESSAGE_SIZE >= message_size)
        {
            imsg_ptr = msgpool_ptr->MSG_FREE_LIST_PTR;
            if ( (imsg_ptr == NULL) &&
                            (msgpool_ptr->GROW_NUMBER) &&
                            (msgpool_ptr->MAX < msgpool_ptr->GROW_LIMIT) )
            {
                /* Attempt to add elements to the pool */
                grow_number = msgpool_ptr->GROW_NUMBER;
                if ( ((uint_16)(msgpool_ptr->MAX + grow_number) > msgpool_ptr->GROW_LIMIT))
                {
                    grow_number = msgpool_ptr->GROW_LIMIT - msgpool_ptr->MAX;
                } /* Endif */
                _msgpool_add_internal(msgpool_ptr, grow_number);
                imsg_ptr = msgpool_ptr->MSG_FREE_LIST_PTR;
            } /* Endif */
            if ( imsg_ptr != NULL )
            {
                msgpool_ptr->MSG_FREE_LIST_PTR = imsg_ptr->NEXT;
                --msgpool_ptr->SIZE;
                _INT_ENABLE();
                imsg_ptr->FREE = FALSE;
                imsg_ptr->QUEUED = FALSE;
                if (kernel_data->IN_ISR)
                {
                    imsg_ptr->TD_PTR = NULL;
                }
                else
                {
                    imsg_ptr->TD_PTR = kernel_data->ACTIVE_PTR;
                } /* Endif */
                message_ptr = (MESSAGE_HEADER_STRUCT_PTR)&imsg_ptr->MESSAGE;
                message_ptr->TARGET_QID = MSGQ_NULL_QUEUE_ID;
                message_ptr->SOURCE_QID = MSGQ_NULL_QUEUE_ID;
                message_ptr->SIZE = message_size;
                message_ptr->CONTROL = MSG_HDR_ENDIAN | MSG_DATA_ENDIAN;

                _KLOGX2(KLOG_msg_alloc_system, message_ptr);
                return (pointer)message_ptr;
            } /* Endif */
        } /* Endif */
        msgpool_ptr = msgpool_ptr->NEXT_MSGPOOL_PTR;
    } /* Endwhile */

    _int_enable();
    _KLOGX2(KLOG_msg_alloc_system, message_ptr);
    return (pointer)message_ptr;

} /* Endbody */

#endif /* MQX_USE_MESSAGES */

/* EOF */
