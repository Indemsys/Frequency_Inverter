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
 * $FileName: msgpool.c$
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

/*!
 * \brief Destroys the private message pool.
 * 
 * Any task can destroy the private message pool as long as all its messages have 
 * been freed.
 * 
 * \param[in] pool_id Pool to destroy.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \return MSGPOOL_INVALID_POOL_ID (Pool_id does not represent a message pool 
 * that was created by _msgpool_create().)
 * \return MSGPOOL_ALL_MESSAGES_NOT_FREE (All messages in the message pool have 
 * not been freed.)
 * 
 * \warning Calls _mem_free(), which on error sets the task error code.
 * 
 * \see _msgpool_create
 * \see _msg_free
 * \see _msg_alloc
 * \see _mem_free      
 */ 
_mqx_uint _msgpool_destroy
(
    _pool_id pool_id
)
{ /* Body */
#if MQX_KERNEL_LOGGING || MQX_CHECK_ERRORS
    KERNEL_DATA_STRUCT_PTR   kernel_data;
#endif
#if MQX_CHECK_ERRORS
    MSG_COMPONENT_STRUCT_PTR msg_component_ptr;
#endif
    MSGPOOL_STRUCT_PTR       msgpool_ptr;
    MSGPOOL_BLOCK_STRUCT_PTR msgpool_block_ptr;
    MSGPOOL_BLOCK_STRUCT_PTR next_block_ptr;

#if MQX_KERNEL_LOGGING || MQX_CHECK_ERRORS
    _GET_KERNEL_DATA(kernel_data);
#endif

    _KLOGE2(KLOG_msgpool_destroy, pool_id);

#if MQX_CHECK_ERRORS
    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
    if (msg_component_ptr == NULL)
    {
        _KLOGX2(KLOG_msgpool_destroy, MQX_COMPONENT_DOES_NOT_EXIST);
        return MQX_COMPONENT_DOES_NOT_EXIST;
    } /* Endif */
#endif

    msgpool_ptr = (MSGPOOL_STRUCT_PTR)pool_id;
#if MQX_CHECK_VALIDITY
    if ( msgpool_ptr->VALID != MSG_VALID )
    {
        _KLOGX2(KLOG_msgpool_destroy, MSGPOOL_INVALID_POOL_ID);
        return MSGPOOL_INVALID_POOL_ID;
    } /* Endif */
#endif

    _int_disable();
    if (msgpool_ptr->SIZE == msgpool_ptr->MAX)
    {
        /* All messages currently returned, lets delete them */
        msgpool_ptr->SIZE = 0;
        msgpool_ptr->GROW_NUMBER = 0;
        _int_enable();

        msgpool_block_ptr = msgpool_ptr->MSGPOOL_BLOCK_PTR;
        while (msgpool_block_ptr != NULL)
        {
            next_block_ptr = msgpool_block_ptr->NEXT_BLOCK_PTR;
            _mem_free((pointer)msgpool_block_ptr);
            msgpool_block_ptr = next_block_ptr;
        } /* Endwhile */

        msgpool_ptr->MSGPOOL_BLOCK_PTR = NULL;
        msgpool_ptr->VALID = 0;
        msgpool_ptr->MSGPOOL_TYPE = 0;
        _KLOGX2(KLOG_msgpool_destroy, MQX_OK);
        return MQX_OK;
    }
    else
    {
        _int_enable();
        _KLOGX2(KLOG_msgpool_destroy, MSGPOOL_ALL_MESSAGES_NOT_FREE);
        return MSGPOOL_ALL_MESSAGES_NOT_FREE;
    } /* Endif */

} /* Endbody */

/*!
 * \brief Creates a private message pool.
 * 
 * Any task can allocate messages from the pool by calling _msg_alloc() with the 
 * pool ID.
 * 
 * \param[in] message_size Size (in single-addressable units) of the messages 
 * (including the message header) to be created for the message pool.
 * \param[in] num_messages Initial number of messages to be created for the 
 * message pool.
 * \param[in] grow_number  Number of messages to be added if all the messages 
 * are allocated.
 * \param[in] grow_limit   If grow_number is not equal to 0; one of the following:
 * \li Maximum number of messages that the pool can have.
 * \li 0 (Unlimited growth.)
 * 
 * \return Pool ID to access the message pool (success).
 * \return 0 (Failure.)  
 * 
 * \warning Creates the message component if it was not previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MSGPOOL_MESSAGE_SIZE_TOO_SMALL (Message_size is less than the size of the 
 * message header structure.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory to create the message pool).
 * \li MSGPOOL_OUT_OF_POOLS (Maximum number of message pools have been created, 
 * where the number is defined at initialization time in MAX_MSGPOOLS in the MQX 
 * initialization structure.)
 * \li Task error codes from _mem_alloc_system()
 * \li Task error codes from _msg_create_component()
 * 
 * \see _msgpool_create_system
 * \see _msgpool_destroy
 * \see _msg_alloc
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
 * \see _msg_create_component
 * \see MQX_INITIALIZATION_STRUCT  
 */ 
_pool_id _msgpool_create
(
    uint_16 message_size,
    uint_16 num_messages,
    uint_16 grow_number,
    uint_16 grow_limit
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _pool_id result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE5(KLOG_msgpool_create, message_size, num_messages, grow_number, grow_limit);

    result = _msgpool_create_internal(message_size, num_messages, grow_number,
                    grow_limit, MSG_POOL);

    _KLOGX2(KLOG_msgpool_create, result);
    return(result);

} /* Endbody */

/*!
 * \private
 *  
 * \brief Internal function that creates a private message pool.
 * 
 * \param[in] message_size Size (in single-addressable units) of the messages 
 * (including the message header) to be created for the message pool.
 * \param[in] num_messages Initial number of messages to be created for the 
 * message pool.
 * \param[in] grow_number  Number of messages to be added if all the messages 
 * are allocated.
 * \param[in] grow_limit   If grow_number is not equal to 0; one of the following:
 * \li Maximum number of messages that the pool can have.
 * \li 0 (Unlimited growth.)
 * \param[in] pool_type     Selects a system pool or a regular pool.
 * 
 * \return Pool ID to access the message pool (success).
 * \return 0 (Failure.)  
 * 
 * \warning Creates the message component if it was not previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MSGPOOL_MESSAGE_SIZE_TOO_SMALL (Message_size is less than the size of the 
 * message header structure.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot allocate memory to create the message pool).
 * \li MSGPOOL_OUT_OF_POOLS (Maximum number of message pools have been created, 
 * where the number is defined at initialization time in MAX_MSGPOOLS in the MQX 
 * initialization structure.)
 * \li Task error codes from _mem_alloc_system()
 * \li Task error codes from _msg_create_component()
 *   
 * \see _msgpool_create 
 * \see _msgpool_add_internal 
 */ 
_pool_id _msgpool_create_internal
(
    uint_16 message_size,
    uint_16 num_messages,
    uint_16 grow_number,
    uint_16 grow_limit,
    _mqx_uint pool_type
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MSG_COMPONENT_STRUCT_PTR    msg_component_ptr;
    register MSGPOOL_STRUCT_PTR msgpool_ptr;
    register MSGPOOL_STRUCT_PTR temp_msgpool_ptr;
    register MSGPOOL_STRUCT_PTR prev_msgpool_ptr;
    register _mqx_uint          i;
    _mqx_uint                   result;

    _GET_KERNEL_DATA(kernel_data);
    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);

#if MQX_CHECK_ERRORS
    if (message_size < sizeof(MESSAGE_HEADER_STRUCT))
    {
        _task_set_error(MSGPOOL_MESSAGE_SIZE_TOO_SMALL);
        return ((_pool_id) 0);
    } /* Endif */
#endif

    /*
     * Try to find an available slot in the array of msgpools for a new pool
     * if MAX_MSGPOOLS_EVER has not yet reached MAX_MSGPOOLS then
     * simply use MAX_MSGPOOLS_EVER as an index value and then increment it
     * but if MAX_MSGPOOLS_EVER has reached MAX_MSGPOOLS then
     * go back and search through the previously assigned headers to see
     * if one has been deallocated and is available for use
     */
    if (msg_component_ptr == NULL)
    {
        result = _msg_create_component();
        msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (msg_component_ptr == NULL)
        {
            _task_set_error(result);
            return ((_pool_id)0);
        } /* Endif */
#endif
    } /* Endif */

    _int_disable();
    if (msg_component_ptr->MAX_MSGPOOLS_EVER >= msg_component_ptr->MAX_MSGPOOLS)
    {
        msgpool_ptr = &msg_component_ptr->MSGPOOLS_PTR[0];
        for ( i=0; i < msg_component_ptr->MAX_MSGPOOLS; i++ )
        {
            if (msgpool_ptr->VALID != MSG_VALID)
            {
                break;
            } /* Endif */
            msgpool_ptr++;
        } /* Endfor */
        if (i == msg_component_ptr->MAX_MSGPOOLS)
        {
            _int_enable();
            _task_set_error(MSGPOOL_OUT_OF_POOLS);
            return ((_pool_id)0);
        } /* Endif */
    }
    else
    {
        msgpool_ptr = &msg_component_ptr->MSGPOOLS_PTR[
        msg_component_ptr->MAX_MSGPOOLS_EVER++];
    } /* Endif */

    msgpool_ptr->VALID = MSG_VALID;
    msgpool_ptr->MESSAGE_SIZE = message_size;
    msgpool_ptr->GROW_NUMBER = 0;
    _int_enable();

    msgpool_ptr->MSGPOOL_BLOCK_PTR = NULL;
    msgpool_ptr->MSG_FREE_LIST_PTR = NULL;
    msgpool_ptr->NEXT_MSGPOOL_PTR = NULL;
    msgpool_ptr->SIZE = 0;
    msgpool_ptr->MAX = 0;
    if ( grow_number == 0 )
    {
        msgpool_ptr->GROW_LIMIT = num_messages;
    }
    else if (grow_limit == 0)
    {
        msgpool_ptr->GROW_LIMIT = 0xFFFF;
    }
    else
    {
        msgpool_ptr->GROW_LIMIT = grow_limit;
    } /* Endif */
    msgpool_ptr->MSGPOOL_TYPE = pool_type;

    if (num_messages)
    {
        _msgpool_add_internal(msgpool_ptr, num_messages);

        /* no messages could be created, so abort pool creation */
        if (msgpool_ptr->SIZE == 0)
        {
            msgpool_ptr->VALID = 0;
            _task_set_error(MQX_OUT_OF_MEMORY);
            return ((_pool_id)0);
        } /* Endif */
    } /* Endif */

    msgpool_ptr->GROW_NUMBER = grow_number;

    if ( pool_type == SYSTEM_MSG_POOL )
    {
        /* We must insert the pool into the link list of system message pools,
         * by order of size, smallest first.
         */

        _int_disable();
        prev_msgpool_ptr = msg_component_ptr->SMALLEST_MSGPOOL_PTR;
        if (prev_msgpool_ptr == NULL)
        {
            /* first entry in list */
            msg_component_ptr->SMALLEST_MSGPOOL_PTR = msgpool_ptr;
            msg_component_ptr->LARGEST_MSGPOOL_PTR = msgpool_ptr;
        }
        else if (prev_msgpool_ptr->MESSAGE_SIZE >= msgpool_ptr->MESSAGE_SIZE)
        {
            /* The new pool is smaller than that at head of list */
            msgpool_ptr->NEXT_MSGPOOL_PTR = prev_msgpool_ptr;
            msg_component_ptr->SMALLEST_MSGPOOL_PTR = msgpool_ptr;
        }
        else
        {
            temp_msgpool_ptr = prev_msgpool_ptr->NEXT_MSGPOOL_PTR;
            while (temp_msgpool_ptr != NULL)
            {
                /* check the relative message sizes */
                if (temp_msgpool_ptr->MESSAGE_SIZE < msgpool_ptr->MESSAGE_SIZE)
                {
                    /* continue to walk down linked list */
                    prev_msgpool_ptr = temp_msgpool_ptr;
                    temp_msgpool_ptr = prev_msgpool_ptr->NEXT_MSGPOOL_PTR;
                }
                else
                {
                    /* this entry belongs between prev_ptr and temp_msgpool_ptr */
                    prev_msgpool_ptr->NEXT_MSGPOOL_PTR = msgpool_ptr;
                    msgpool_ptr->NEXT_MSGPOOL_PTR = temp_msgpool_ptr;
                    break;
                } /* Endif */
            } /* Endwhile */
            if (temp_msgpool_ptr == NULL)
            {
                /* searched the list and this entry belongs at the bottom */
                prev_msgpool_ptr->NEXT_MSGPOOL_PTR = msgpool_ptr;
                msg_component_ptr->LARGEST_MSGPOOL_PTR = msgpool_ptr;
            }/* Endif */
        } /* Endif */
        _int_enable();

    } /* Endif */

    return ((_pool_id)msgpool_ptr);

} /* Endbody */
/*!
 * \private
 * 
 * \brief Creates a new block of messages, and adds the block to the msgpool.
 * 
 * The messages in the block are linked together onto the free list.
 * 
 * \param[in] msgpool_ptr  A pointer to the message pool.
 * \param[in] num_messages The number of messages to attempt to add. 
 */ 
void _msgpool_add_internal
(
    MSGPOOL_STRUCT_PTR msgpool_ptr,
    uint_16            num_messages
)
{ /* Body */
    INTERNAL_MESSAGE_STRUCT_PTR first_imsg_ptr;
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    MSGPOOL_BLOCK_STRUCT_PTR    msgpool_block_ptr;
    _mqx_int                    raw_message_size;
    _mqx_uint                   count;

    raw_message_size = sizeof(INTERNAL_MESSAGE_STRUCT) -
    sizeof(MESSAGE_HEADER_STRUCT) + msgpool_ptr->MESSAGE_SIZE;
    _MEMORY_ALIGN_VAL_LARGER(raw_message_size);

    msgpool_block_ptr = (MSGPOOL_BLOCK_STRUCT_PTR)_mem_alloc_system((_mem_size)
                    ((raw_message_size*num_messages) + sizeof(MSGPOOL_BLOCK_STRUCT) +
                                    PSP_MEMORY_ALIGNMENT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (msgpool_block_ptr == NULL)
    {
        return;
    } /* Endif */
#endif
    _mem_set_type(msgpool_block_ptr, MEM_TYPE_MESSAGE_POOL_BLOCKS);

    msgpool_block_ptr->NEXT_BLOCK_PTR = NULL;
    msgpool_block_ptr->RAW_MESSAGE_SIZE = raw_message_size;
    msgpool_block_ptr->NUM_MESSAGES = num_messages;

    first_imsg_ptr =(INTERNAL_MESSAGE_STRUCT_PTR)
    ((uchar _PTR_)msgpool_block_ptr + sizeof(MSGPOOL_BLOCK_STRUCT));
    first_imsg_ptr = (INTERNAL_MESSAGE_STRUCT_PTR)
    _ALIGN_ADDR_TO_HIGHER_MEM(first_imsg_ptr);

    msgpool_block_ptr->FIRST_IMSG_PTR = first_imsg_ptr;

    imsg_ptr = first_imsg_ptr;
    count = num_messages + 1;
    while (--count)
    {
        imsg_ptr->NEXT = NULL; /* Free list pointer */
        imsg_ptr->VALID = MSG_VALID;
        imsg_ptr->FREE = TRUE;
        imsg_ptr->QUEUED = FALSE;
        imsg_ptr->MSGPOOL_PTR = msgpool_ptr;
        imsg_ptr->TD_PTR = NULL;
        if (count > 1)
        {
            /* imsg is in the middle of the list */
            imsg_ptr->NEXT = (INTERNAL_MESSAGE_STRUCT_PTR)
            ((uchar _PTR_)imsg_ptr + raw_message_size);
            imsg_ptr = imsg_ptr->NEXT;
        }/* Endif */
    } /* Endwhile */

    /* Now make the pool entries available */
    _int_disable();
    msgpool_block_ptr->NEXT_BLOCK_PTR = msgpool_ptr->MSGPOOL_BLOCK_PTR;
    msgpool_ptr->MSGPOOL_BLOCK_PTR = msgpool_block_ptr;

    /* imsg_ptr here is the last message on the list */
    imsg_ptr->NEXT = msgpool_ptr->MSG_FREE_LIST_PTR;
    msgpool_ptr->MSG_FREE_LIST_PTR = first_imsg_ptr;
    msgpool_ptr->SIZE += num_messages;
    msgpool_ptr->MAX += num_messages;
    _int_enable();

} /* Endbody */

/*!
 * \brief Creates a system message pool.
 * 
 * Tasks can subsequently allocate messages from the pool by calling 
 * _msg_alloc_system().
 * 
 * \param[in] message_size Size (in single-addressable units) of the messages 
 * (including the message header) to be created for the message pool.
 * \param[in] num_messages Initial number of messages to be created for the 
 * message pool.
 * \param[in] grow_number  Number of messages to be added if all the messages 
 * are allocated.
 * \param[in] grow_limit   If grow_number is not equal to 0; one of the following:
 * \li Maximum number of messages that the pool can have.
 * \li 0 (Unlimited growth.)  
 * 
 * \return TRUE (Success.) or FALSE (Failure.)
 * 
 * \warning Creates the message component if it was not previously created.
 * \warning On failure, calls _task_set_error() to set the task error code as 
 * described for _msgpool_create().
 * 
 * \see _msgpool_create
 * \see _msgpool_destroy
 * \see _msg_alloc_system
 * \see _task_set_error
 * \see MQX_INITIALIZATION_STRUCT  
 */ 
boolean _msgpool_create_system
(
    uint_16 message_size,
    uint_16 num_messages,
    uint_16 grow_number,
    uint_16 grow_limit
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    _pool_id ret_value;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE5(KLOG_msgpool_create_system, message_size, num_messages, grow_number, grow_limit);

    ret_value = _msgpool_create_internal(message_size, num_messages, grow_number,
                    grow_limit, SYSTEM_MSG_POOL);

    _KLOGX2(KLOG_msgpool_create_system, ret_value == (_pool_id)0 );

    if ( ret_value == (_pool_id)0 )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    } /* Endif */

} /* Endbody */

/*!
 * \brief Tests all the message poolsin the system for consistency and validity.
 *  
 * The function checks the validity of each message in each private and system 
 * message pool. It reports the first error that it finds.
 *  
 * \param[out] pool_error_ptr (Initialized only if an error is found.) If the 
 * message in a message pool has an error; one of the following:
 * \li A pointer to a pool ID if the message is from a private message pool.
 * \li A pointer to a system message pool if the message is from a system 
 * message pool.
 * \param[out] msg_error_ptr  Pointer to the message that has an error 
 * (initialized only if an error is found).
 * 
 * \return MQX_OK (all messages in all message pools passed)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Message component is not created.)
 * \return MSGQ_INVALID_MESSAGE (At least one message in at least one message 
 * pool failed.)
 * 
 * \warning Disables and enables interrupts.
 *  
 * \see _msgpool_create
 * \see _msgpool_create_system   
 */ 
_mqx_uint _msgpool_test
(
    pointer _PTR_ pool_error_ptr,
    pointer _PTR_ msg_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MSG_COMPONENT_STRUCT_PTR    msg_component_ptr;
    MSGPOOL_STRUCT_PTR          msgpool_ptr;
    MSGPOOL_BLOCK_STRUCT_PTR    msgpool_block_ptr;
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    _mqx_uint                   i,j,raw_message_size;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_msgpool_test, pool_error_ptr, msg_error_ptr);

    msg_component_ptr = _GET_MSG_COMPONENT_STRUCT_PTR(kernel_data);
#if MQX_CHECK_ERRORS
    if (msg_component_ptr == NULL)
    {
        _KLOGX2(KLOG_msgpool_test, MQX_COMPONENT_DOES_NOT_EXIST);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif

    /* Check all the message pools */
    msgpool_ptr = msg_component_ptr->MSGPOOLS_PTR;
    i = msg_component_ptr->MAX_MSGPOOLS + 1;
    while (--i)
    {
        _int_disable();
        if (msgpool_ptr->VALID == MSG_VALID)
        {
            /* The pool has been created */

            /* Search through all of the message pool blocks for this pool */
            msgpool_block_ptr = msgpool_ptr->MSGPOOL_BLOCK_PTR;
            while (msgpool_block_ptr != NULL)
            {
                raw_message_size = msgpool_block_ptr->RAW_MESSAGE_SIZE;
                imsg_ptr = (INTERNAL_MESSAGE_STRUCT_PTR)
                msgpool_block_ptr->FIRST_IMSG_PTR;
                j = msgpool_block_ptr->NUM_MESSAGES + 1;
                while (--j)
                {
                    if ((imsg_ptr->VALID != MSG_VALID) ||
                                    (imsg_ptr->MSGPOOL_PTR != msgpool_ptr))
                    {
                        _int_enable();
                        *pool_error_ptr = msgpool_ptr;
                        *msg_error_ptr = imsg_ptr;
                        _KLOGX4(KLOG_msgpool_test, MSGQ_INVALID_MESSAGE, msgpool_ptr,
                                        imsg_ptr);
                        return(MSGQ_INVALID_MESSAGE);
                    } /* Endif */
                    imsg_ptr =(INTERNAL_MESSAGE_STRUCT_PTR)
                    ((uchar _PTR_)imsg_ptr + raw_message_size);
                } /* Endwhile */

                msgpool_block_ptr = msgpool_block_ptr->NEXT_BLOCK_PTR;
            } /* Endwhile */

        } /* Endif */
        _int_enable();
        msgpool_ptr++;
    } /* Endwhile */

    _KLOGX2(KLOG_msgpool_test, MQX_OK);
    return(MQX_OK);

} /* Endbody */

#endif /* MQX_USE_MESSAGES */

/* EOF */
