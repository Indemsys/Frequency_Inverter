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
 * $FileName: lwmem.c$
 * $Version : 3.8.2.0$
 * $Date    : Jun-25-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Lightweight Memory component.
 *
 *END************************************************************************/

#include "mqx_inc.h"
#if MQX_USE_LWMEM
#include "lwmem.h"
#include "lwmem_prv.h"

/*!
 * \private
 *
 * \brief Allocates a block of memory for a task from the free list.
 *
 * \param[in] requested_size The size of the memory block.
 * \param[in] td_ptr         The owner TD.
 * \param[in] pool_id        Which pool to allocate from.
 * \param[in] zero           Zero the memory after it is allocated.
 *
 * \return Pointer to the lightweight-memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 */
pointer _lwmem_alloc_internal
(
    _mem_size      requested_size,
    TD_STRUCT_PTR  td_ptr,
    _lwmem_pool_id pool_id,
    boolean        zero
)
{ /* Body */
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    LWMEM_BLOCK_STRUCT_PTR next_block_ptr;
    LWMEM_BLOCK_STRUCT_PTR prev_block_ptr = NULL;
    _mem_size              block_size;
    _mem_size              next_block_size;
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) pool_id;

#if MQX_CHECK_VALIDITY
    if (mem_pool_ptr->VALID != LWMEM_POOL_VALID)
    {
        _task_set_error(MQX_LWMEM_POOL_INVALID);
        return (NULL);
    } /* Endif */
#endif

    /*
     * Adjust message size to allow for block management overhead
     * and force size to be aligned.
     */
    requested_size += (_mem_size) sizeof(LWMEM_BLOCK_STRUCT);
    if (requested_size < LWMEM_MIN_MEMORY_STORAGE_SIZE)
    {
        requested_size = LWMEM_MIN_MEMORY_STORAGE_SIZE;
    } /* Endif */

    _MEMORY_ALIGN_VAL_LARGER(requested_size);

    _int_disable();
    block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    while (block_ptr != NULL)
    {
        /* Provide window for higher priority tasks */
        mem_pool_ptr->POOL_ALLOC_PTR = block_ptr;
        _int_enable();
        _int_disable();
        block_ptr = mem_pool_ptr->POOL_ALLOC_PTR;
        if (block_ptr == NULL)
        {
            /* The pool became full during preemption, no free block exists anymore */
            break;
        }
        if (block_ptr == mem_pool_ptr->POOL_FREE_LIST_PTR)
        {
            prev_block_ptr = block_ptr;
        } /* Endif */
        block_size = block_ptr->BLOCKSIZE;
        if (block_size >= requested_size)
        {
            /* request fits into this block */
            next_block_size = block_size - requested_size;
            if (next_block_size >= LWMEM_MIN_MEMORY_STORAGE_SIZE)
            {
                /*
                 * The current block is big enough to split.
                 * into 2 blocks.... the part to be allocated is one block,
                 * and the rest remains as a free block on the free list.
                 */
                next_block_ptr = (LWMEM_BLOCK_STRUCT_PTR) (pointer) ((uchar_ptr) block_ptr + requested_size);
                /* Initialize the new physical block values */
                next_block_ptr->BLOCKSIZE = next_block_size;
                /* Link new block into the free list */
                next_block_ptr->POOL = mem_pool_ptr;
                next_block_ptr->U.NEXTBLOCK = block_ptr->U.NEXTBLOCK;
                block_ptr->U.NEXTBLOCK = (pointer) next_block_ptr;

                /* Modify the current block, to point to this newly created block*/
                block_ptr->BLOCKSIZE = requested_size;
            }
            else
            {
                /* Take the entire block */
                requested_size = block_size;
                next_block_ptr = block_ptr->U.NEXTBLOCK;
            } /* Endif */

            if (block_ptr == mem_pool_ptr->POOL_FREE_LIST_PTR)
            {
                /* At the head of the free list */
                mem_pool_ptr->POOL_FREE_LIST_PTR = next_block_ptr;
            }
            else
            {
                prev_block_ptr->U.NEXTBLOCK = next_block_ptr;
            } /* Endif */

            mem_pool_ptr->POOL_ALLOC_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_FREE_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_TEST_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_TEST2_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#if MQX_TASK_DESTRUCTION
            mem_pool_ptr->POOL_DESTROY_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#endif
            /* Indicate the block is in use */
            block_ptr->U.S.TASK_NUMBER = TASK_NUMBER_FROM_TASKID(td_ptr->TASK_ID);
            block_ptr->U.S.MEM_TYPE = 0;
            block_ptr->POOL = (_lwmem_pool_id) mem_pool_ptr;

            if ((uint_32) block_ptr > (uint_32) mem_pool_ptr->HIGHWATER)
            {
                mem_pool_ptr->HIGHWATER = block_ptr;
            }

            _int_enable();

            if (zero)
            {
                _mem_zero((pointer) ((uchar_ptr) block_ptr + sizeof(LWMEM_BLOCK_STRUCT)), requested_size
                                - sizeof(LWMEM_BLOCK_STRUCT));
            } /* Endif */
            return ((pointer) ((uchar_ptr) block_ptr + sizeof(LWMEM_BLOCK_STRUCT)));
        }
        else
        {
            prev_block_ptr = block_ptr;
            block_ptr = block_ptr->U.NEXTBLOCK;
        } /* Endif */
    } /* Endwhile */

    _int_enable();
    _task_set_error(MQX_OUT_OF_MEMORY);
    return (NULL);

} /* Endbody */

/*!
 * \private
 *
 * \brief Allocates a block of memory for a task from the free list.
 *
 * \param[in] requested_size The size of the memory block.
 * \param[in] requested_addr The address of the memory block.
 * \param[in] td_ptr         The owner TD.
 * \param[in] pool_id        Which pool to allocate from.
 * \param[in] zero           Zero the memory after it is allocated.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 */
pointer _lwmem_alloc_at_internal
(
    _mem_size      requested_size,
    pointer        requested_addr,
    TD_STRUCT_PTR  td_ptr,
    _lwmem_pool_id pool_id,
    boolean        zero
)
{ /* Body */
    LWMEM_BLOCK_STRUCT_PTR block_ptr, requested_block_ptr;
    LWMEM_BLOCK_STRUCT_PTR next_block_ptr;
    LWMEM_BLOCK_STRUCT_PTR prev_block_ptr = NULL;
    _mem_size              block_size;
    _mem_size              next_block_size, free_block_size;
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) pool_id;

#if MQX_CHECK_VALIDITY
    if (mem_pool_ptr->VALID != LWMEM_POOL_VALID)
    {
        _task_set_error(MQX_LWMEM_POOL_INVALID);
        return (NULL);
    } /* Endif */
#endif

    /*
     * Adjust message size to allow for block management overhead
     * and force size to be aligned.
     */
    requested_size += (_mem_size) sizeof(LWMEM_BLOCK_STRUCT);
    if (requested_size < LWMEM_MIN_MEMORY_STORAGE_SIZE)
    {
        requested_size = LWMEM_MIN_MEMORY_STORAGE_SIZE;
    } /* Endif */

    _MEMORY_ALIGN_VAL_LARGER(requested_size);

    _int_disable();
    block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    while (block_ptr != NULL)
    {
        /* Provide window for higher priority tasks */
        mem_pool_ptr->POOL_ALLOC_PTR = block_ptr;

        _int_enable();
        _int_disable();

        block_ptr = mem_pool_ptr->POOL_ALLOC_PTR;
        if (block_ptr == NULL)
        {
            /* The pool became full during preemption, no free block exists anymore */
            break;
        }
        if (block_ptr == mem_pool_ptr->POOL_FREE_LIST_PTR)
        {
            prev_block_ptr = block_ptr;
        }

        block_size = block_ptr->BLOCKSIZE;
        if ((uchar_ptr) block_ptr + LWMEM_MIN_MEMORY_STORAGE_SIZE <= (uchar_ptr) requested_addr
                        - sizeof(LWMEM_BLOCK_STRUCT) && (uchar_ptr) block_ptr + block_size
                        >= (uchar_ptr) requested_addr - sizeof(LWMEM_BLOCK_STRUCT) + requested_size)
        {
            /* request fits into this block */

            requested_block_ptr = (LWMEM_BLOCK_STRUCT_PTR)((uchar_ptr) requested_addr - sizeof(LWMEM_BLOCK_STRUCT));

            free_block_size = (uchar_ptr) requested_block_ptr - (uchar_ptr) block_ptr;
            /* requested block size is known */
            next_block_size = block_size - requested_size - free_block_size;

            /* free_block_size is always > LWMEM_MIN_MEMORY_STORAGE_SIZE */
            block_ptr->BLOCKSIZE = free_block_size;

            /* chek and prepare free block after requested block */
            if (next_block_size >= LWMEM_MIN_MEMORY_STORAGE_SIZE)
            {
                /*
                 * The current block is big enough to split.
                 * into 2 blocks.... the part to be allocated is one block,
                 * and the rest remains as a free block on the free list.
                 */
                next_block_ptr = (LWMEM_BLOCK_STRUCT_PTR) (pointer) ((uchar_ptr) requested_block_ptr + requested_size);
                /* Initialize the new physical block values */
                next_block_ptr->BLOCKSIZE = next_block_size;
                /* Link new block into the free list */
                next_block_ptr->POOL = mem_pool_ptr;
                next_block_ptr->U.NEXTBLOCK = block_ptr->U.NEXTBLOCK;
                block_ptr->U.NEXTBLOCK = (pointer) next_block_ptr;
            }
            else
            {
                /* Take the entire block */
                requested_size += next_block_size;
                next_block_ptr = block_ptr->U.NEXTBLOCK;
            }

            /* first free block always stay first */

            /* add next free block to list */
            //prev_block_ptr->U.NEXTBLOCK = next_block_ptr;

            /* Modify the allocated block information to point to next block */
            requested_block_ptr->BLOCKSIZE = requested_size;
            requested_block_ptr->U.NEXTBLOCK = next_block_ptr;

            mem_pool_ptr->POOL_ALLOC_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_FREE_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_TEST_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_TEST2_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#if MQX_TASK_DESTRUCTION
            mem_pool_ptr->POOL_DESTROY_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#endif
            /* Indicate the block is in use */
            requested_block_ptr->U.S.TASK_NUMBER = TASK_NUMBER_FROM_TASKID(td_ptr->TASK_ID);
            requested_block_ptr->U.S.MEM_TYPE = 0;
            requested_block_ptr->POOL = (_lwmem_pool_id) mem_pool_ptr;

            if ((uint_32) block_ptr > (uint_32) mem_pool_ptr->HIGHWATER)
            {
                mem_pool_ptr->HIGHWATER = block_ptr;
            }

            _int_enable();

            if (zero)
            {
                _mem_zero((pointer) requested_addr, requested_size - sizeof(LWMEM_BLOCK_STRUCT));
            } /* Endif */
            return requested_addr;
        }
        else
        {
            prev_block_ptr = block_ptr;
            block_ptr = block_ptr->U.NEXTBLOCK;
        } /* Endif */
    } /* Endwhile */

    _int_enable();
    _task_set_error(MQX_OUT_OF_MEMORY);
    return (NULL);
}

/*!
 * \private
 *
 * \brief Allocate an aligned block of memory for a task from the free list.
 *
 * \param[in] requested_size The size of the memory block.
 * \param[in] req_align      Requested alignement (e.g. 8 for alignement to 8 bytes).
 * \param[in] td_ptr         The owner TD.
 * \param[in] pool_id        Which pool to allocate from.
 * \param[in] zero           Zero the memory after it is allocated.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 */
pointer _lwmem_alloc_align_internal
(
    _mem_size      requested_size,
    _mem_size      req_align,
    TD_STRUCT_PTR  td_ptr,
    _lwmem_pool_id pool_id,
    boolean        zero
)
{ /* Body */
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    LWMEM_BLOCK_STRUCT_PTR next_block_ptr;
    LWMEM_BLOCK_STRUCT_PTR prev_block_ptr;
    _mem_size              block_size;
    _mem_size              next_block_size;
    _mem_size              shift;
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) pool_id;

#if MQX_CHECK_VALIDITY
    if (mem_pool_ptr->VALID != LWMEM_POOL_VALID)
    {
        _task_set_error(MQX_LWMEM_POOL_INVALID);
        return (NULL);
    }
#endif

    /*
     * Adjust message size to allow for block management overhead
     * and force size to be aligned.
     */
    requested_size += (_mem_size) sizeof(LWMEM_BLOCK_STRUCT);
    if (requested_size < LWMEM_MIN_MEMORY_STORAGE_SIZE)
    {
        requested_size = LWMEM_MIN_MEMORY_STORAGE_SIZE;
    }

    _MEMORY_ALIGN_VAL_LARGER(requested_size);

    _int_disable();
    block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    while (block_ptr != NULL)
    {
        /* Provide window for higher priority tasks */
        mem_pool_ptr->POOL_ALLOC_PTR = block_ptr;
        _int_enable();
        _int_disable();
        block_ptr = mem_pool_ptr->POOL_ALLOC_PTR;
        if (block_ptr == NULL)
        {
            /* The pool became full during preemption, no free block exists anymore */
            break;
        }
        if (block_ptr == mem_pool_ptr->POOL_FREE_LIST_PTR)
        {
            prev_block_ptr = block_ptr;
        }

        shift = (((_mem_size) block_ptr + sizeof(LWMEM_BLOCK_STRUCT) + req_align) & ~(req_align - 1))
                        - ((_mem_size) block_ptr + sizeof(LWMEM_BLOCK_STRUCT));

        if (shift < (2 * LWMEM_MIN_MEMORY_STORAGE_SIZE))
        {
            shift = (((_mem_size) block_ptr + sizeof(LWMEM_BLOCK_STRUCT) + (3 * LWMEM_MIN_MEMORY_STORAGE_SIZE)
                            + req_align) & ~(req_align - 1)) - ((_mem_size) block_ptr + sizeof(LWMEM_BLOCK_STRUCT));
        }

        block_size = block_ptr->BLOCKSIZE;
        if (block_size >= requested_size + shift)
        {
            /* request fits into this block */

            /* create new free block */
            prev_block_ptr = block_ptr;
            block_ptr = (LWMEM_BLOCK_STRUCT_PTR)(((uchar_ptr) block_ptr) + shift);
            prev_block_ptr->BLOCKSIZE = shift;
            block_ptr->U.NEXTBLOCK = prev_block_ptr->U.NEXTBLOCK;

            next_block_size = block_size - requested_size - shift;
            if (next_block_size >= LWMEM_MIN_MEMORY_STORAGE_SIZE)
            {
                /*
                 * The current block is big enough to split.
                 * into 2 blocks.... the part to be allocated is one block,
                 * and the rest remains as a free block on the free list.
                 */
                next_block_ptr = (LWMEM_BLOCK_STRUCT_PTR) (pointer) ((uchar_ptr) block_ptr + requested_size);
                /* Initialize the new physical block values */
                next_block_ptr->BLOCKSIZE = next_block_size;
                /* Link new block into the free list */
                next_block_ptr->POOL = mem_pool_ptr;
                next_block_ptr->U.NEXTBLOCK = block_ptr->U.NEXTBLOCK;
                block_ptr->U.NEXTBLOCK = (pointer) next_block_ptr;

                /* Modify the current block, to point to this newly created block*/
                block_ptr->BLOCKSIZE = requested_size;
            }
            else
            {
                /* Take the entire block */
                requested_size = block_size;
                next_block_ptr = block_ptr->U.NEXTBLOCK;
            } /* Endif */

            if (block_ptr == mem_pool_ptr->POOL_FREE_LIST_PTR)
            {
                /* At the head of the free list */
                mem_pool_ptr->POOL_FREE_LIST_PTR = next_block_ptr;
            }
            else
            {
                prev_block_ptr->U.NEXTBLOCK = next_block_ptr;
            } /* Endif */

            mem_pool_ptr->POOL_ALLOC_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_FREE_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_TEST_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
            mem_pool_ptr->POOL_TEST2_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#if MQX_TASK_DESTRUCTION
            mem_pool_ptr->POOL_DESTROY_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#endif
            /* Indicate the block is in use */
            block_ptr->U.S.TASK_NUMBER = TASK_NUMBER_FROM_TASKID(td_ptr->TASK_ID);
            block_ptr->U.S.MEM_TYPE = 0;
            block_ptr->POOL = (_lwmem_pool_id) mem_pool_ptr;

            if ((uint_32) block_ptr > (uint_32) mem_pool_ptr->HIGHWATER)
            {
                mem_pool_ptr->HIGHWATER = block_ptr;
            }

            _int_enable();

            if (zero)
            {
                _mem_zero((pointer) ((uchar_ptr) block_ptr + sizeof(LWMEM_BLOCK_STRUCT)), requested_size
                                - sizeof(LWMEM_BLOCK_STRUCT));
            }

            return ((pointer) ((uchar_ptr) block_ptr + sizeof(LWMEM_BLOCK_STRUCT)));
        }
        else
        {
            prev_block_ptr = block_ptr;
            block_ptr = block_ptr->U.NEXTBLOCK;
        }
    }

    _int_enable();
    _task_set_error(MQX_OUT_OF_MEMORY);
    return (NULL);
}

#if MQX_ENABLE_USER_MODE
/*!
 * \private
 *
 * \brief Allocates a block of memory.
 *
 * \param[in] requested_size The size of the memory block.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 */
pointer _usr_lwmem_alloc_internal
(
    _mem_size requested_size
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_usr_lwmem_alloc, requested_size);

    result = _lwmem_alloc_internal(requested_size, kernel_data->ACTIVE_PTR,
                    (_lwmem_pool_id)kernel_data->KD_USER_POOL, FALSE);

    _KLOGX2(KLOG_usr_lwmem_alloc, result);
    return(result);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Allocates a private block of lightweight memory block from the default
 * memory pool.
 *
 * The application must first set a value for the default lightweight memory pool
 * by calling _lwmem_set_default_pool().
 * \n The _lwmem_alloc functions allocate at least size single-addressable units;
 * the actual number might be greater. The start address of the block is aligned
 * so that tasks can use the returned pointer as a pointer to any data type without
 * causing an error.
 * \n Tasks cannot use lightweight memory blocks as messages. Tasks must use
 * _msg_alloc() or _msg_alloc_system() to allocate messages.
 * \n Only the task that owns a lightweight memory block that was allocated with
 * one of the following functions can free the block:
 * \li _lwmem_alloc()
 * \li _lwmem_alloc_zero()
 * \li _lwmem_alloc_at()
 *
 * \n Any task can free a lightweight memory block that is allocated with one of
 * the following functions:
 * \li _lwmem_alloc_system()
 * \li _lwmem_alloc_system_zero()
 *
 * \n Function types:
 * <table>
 *  <tr>
 *    <td></td>
 *    <td><b>Allocate this type of lighweight memory block form the default memory
 *    pool:</b></td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc()</b></td>
 *    <td>Private</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_system()</b></td>
 *    <td>System</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_system_zero()</b></td>
 *    <td>System (zero-filled)</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_zero()</b></td>
 *    <td>Private (zero-filled)</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_at()</b></td>
 *    <td>Private (start address defined)</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_align()</b></td>
 *    <td>Private (aligned) </td>
 *  </tr>
 * </table>
 *
 * \param[in] requested_size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_get_size
 * \see _lwmem_set_default_pool
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc
(
    _mem_size requested_size
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwmem_alloc(requested_size);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc, requested_size);

    result = _lwmem_alloc_internal(requested_size, kernel_data->ACTIVE_PTR,
                    (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL, FALSE);

    _KLOGX2(KLOG_lwmem_alloc, result);
    return (result);
}

#if MQX_ENABLE_USER_MODE
/*!
 * /brief Allocates a private block of lightweight memory block from the default
 * memory pool.
 *
 * This function is an equivalent to the _lwmem_alloc() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] requested_size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _usr_lwmem_alloc_from
 * \see _usr_lwmem_create_pool
 * \see _usr_lwmem_free
 */
pointer _usr_lwmem_alloc
(
    _mem_size requested_size
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)requested_size, 0, 0, 0, 0};
    return (pointer)_mqx_api_call(MQX_API_LWMEM_ALLOC, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Allocates a private block (with defined start address) of
 * lightweight memory block from the default memory pool.
 *
 * See Description of _lwmem_alloc() function.
 *
 * \param[in] requested_size Number of single-addressable units to allocate.
 * \param[in] requested_addr Start address of the memory block.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_get_size
 * \see _lwmem_set_default_pool
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_at
(
    _mem_size requested_size,
    pointer   requested_addr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc_at, requested_size);

    result = _lwmem_alloc_at_internal(requested_size, requested_addr, kernel_data->ACTIVE_PTR,
                    (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL, FALSE);

    _KLOGX2(KLOG_lwmem_alloc_at, result);
    return (result);
}

/*!
 * \brief Allocates an aligned block of lightweight memory block from the default
 * memory pool.
 *
 * See Description of _lwmem_alloc() function.
 *
 * \param[in] requested_size Number of single-addressable units to allocate.
 * \param[in] req_align      Align requested value.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_get_size
 * \see _lwmem_set_default_pool
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_align
(
    _mem_size requested_size,
    _mem_size req_align
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_lwmem_alloc_align, requested_size, req_align);

    result = _lwmem_alloc_align_internal(requested_size, req_align, kernel_data->ACTIVE_PTR,
                    (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL, FALSE);

    _KLOGX2(KLOG_lwmem_alloc_align, result);

    return (result);
}

/*!
 * \brief Gets default system lwmem pool.
 *
 * \return Pointer to default szstem lwmem pool.
 * \return NULL (failure)
 */
_lwmem_pool_id _lwmem_get_system_pool_id(void)
{
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL;
}

/*!
 * \brief Allocates a private block of lightweight memory block from the specified
 * memory pool.
 *
 * The function is similar to _lwmem_alloc(), _lwmem_alloc_system(),
 * _lwmem_alloc_system_zero() and _lwmem_alloc_zero() except that the application
 * does not call _lwmem_set_default_pool() first.
 * \n Only the task that owns a lightweight memory block that was allocated with
 * one of the following functions can free the block:
 * \li _lwmem_alloc_from()
 * \li _lwmem_alloc_zero_from()
 *
 * \n Any task can free a lightweight memory block that is allocated with one of
 * the following functions:
 * \li _lwmem_alloc_system_from()
 * \li _lwmem_alloc_system_zero_from()
 *
 * \n Function types:
 * <table>
 *  <tr>
 *    <td></td>
 *    <td><b>Allocate this type of lighweight memory block form the specified
 *    lightweight memory pool:</b></td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_from()</b></td>
 *    <td>Private</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_system_from()</b></td>
 *    <td>System</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_system_zero_from()</b></td>
 *    <td>System (zero-filled)</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_zero_from()</b></td>
 *    <td>Private (zero-filled)</td>
 *  </tr>
 *  <tr>
 *    <td><b>_lwmem_alloc_align_from()</b></td>
 *    <td>Private (aligned) </td>
 *  </tr>
 * </table>
 *
 * \param[in] pool_id        Lightweight memory pool from which to allocate the
 * lightweight memory block (from _lwmem_create_pool()).
 * \param[in] requested_size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_from
(
    _lwmem_pool_id pool_id,
    _mem_size      requested_size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwmem_alloc_from(pool_id, requested_size);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_lwmem_alloc_from, pool_id, requested_size);

    result = _lwmem_alloc_internal(requested_size, kernel_data->ACTIVE_PTR, pool_id, FALSE);

    _KLOGX2(KLOG_lwmem_alloc_from, result);
    return (result);
}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Allocates a private block of lightweight memory block from the specified
 * memory pool.
 *
 * This function is an equivalent to the _lwmem_alloc_from() API call but it can
 * be executed from within the User task or other code running in the CPU User
 * mode. Parameters passed to this function by pointer are required to meet the
 * memory protection requirements as described in the parameter list below.
 *
 * \param[in] pool_id        Read/write. Lightweight memory pool from which to
 * allocate the lightweight memory block (pool created with _usr_lwmem_create_pool()
 * or ordinary lightweight memory pool for which the user-mode access has been
 * enabled by calling _watchdog_create_component()).
 * \param[in] requested_size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc_from
 * \see _watchdog_create_component
 * \see _usr_lwmem_alloc
 * \see _usr_lwmem_create_pool
 * \see _usr_lwmem_free
 */
pointer _usr_lwmem_alloc_from
(
    _lwmem_pool_id pool_id,
    _mem_size      requested_size
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)pool_id, (uint_32)requested_size, 0, 0, 0};
    return (pointer)_mqx_api_call(MQX_API_LWMEM_ALLOC_FROM, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Allocates a private block of lightweight memory block from the specified
 * memory pool.
 *
 * See Description of _lwmem_alloc_from() function.
 *
 * \param[in] pool_id        Lightweight memory pool from which to allocate the
 * lightweight memory block (from _lwmem_create_pool()).
 * \param[in] requested_size Number of single-addressable units to allocate.
 * \param[in] req_align      Align requested value.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_align_from
(
    _lwmem_pool_id pool_id,
    _mem_size      requested_size,
    _mem_size      req_align
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE4(KLOG_lwmem_alloc_align_from, pool_id, requested_size, req_align);

    result = _lwmem_alloc_align_internal(requested_size, req_align, kernel_data->ACTIVE_PTR, pool_id, FALSE);

    _KLOGX2(KLOG_lwmem_alloc_align_from, result);

    return (result);
}

/*!
 * \brief Allocates a private (zero-filled) block of lightweight memory block from
 * the default memory pool.
 *
 * See Description of _lwmem_alloc() function.
 *
 * \param[in] size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_get_size
 * \see _lwmem_set_default_pool
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_zero
(
    _mem_size size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc_zero, size);

    result
                    = _lwmem_alloc_internal(size, kernel_data->ACTIVE_PTR,
                                    (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL, TRUE);

    _KLOGX2(KLOG_lwmem_alloc_zero, result);
    return (result);

} /* Endbody */

/*!
 * \brief Allocates a private (zero-filled) block of lightweight memory block from the specified
 * memory pool.
 *
 * See Description of _lwmem_alloc_from() function.
 *
 * \param[in] pool_id        Lightweight memory pool from which to allocate the
 * lightweight memory block (from _lwmem_create_pool()).
 * \param[in] size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_zero_from
(
    pointer   pool_id,
    _mem_size size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_lwmem_alloc_zero_from, pool_id, size);

    result = _lwmem_alloc_internal(size, kernel_data->ACTIVE_PTR, pool_id, TRUE);

    _KLOGX2(KLOG_lwmem_alloc_zero_from, result);
    return (result);

} /* Endbody */

/*!
 * \brief Creates the lightweight memory pool from memory that is outside the
 * default memory pool.
 *
 * Tasks use the pool ID to allocate (variable-size) lightweight memory blocks
 * from the pool.
 *
 * \param[in] mem_pool_ptr Pointer to the definition of the pool.
 * \param[in] start        Start of the memory for the pool.
 * \param[in] size         Number of single-addressable units in the pool.
 *
 * \return Pool ID
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 */
_lwmem_pool_id _lwmem_create_pool
(
    LWMEM_POOL_STRUCT_PTR mem_pool_ptr,
    pointer               start,
    _mem_size             size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    uchar_ptr              end;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwmem_create_pool(mem_pool_ptr, start, size);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE3(KLOG_lwmem_create_pool, start, size);

    /* Set the end of memory (aligned) */
    end = (uchar_ptr) start + size;
    mem_pool_ptr->POOL_ALLOC_END_PTR = (pointer) _ALIGN_ADDR_TO_LOWER_MEM(end);

    /* Align the start of the pool */
    block_ptr = (LWMEM_BLOCK_STRUCT_PTR)_ALIGN_ADDR_TO_HIGHER_MEM(start);
    mem_pool_ptr->POOL_ALLOC_START_PTR = (pointer) block_ptr;
    mem_pool_ptr->HIGHWATER = (pointer) block_ptr;

    /* Set up the first block as an idle block */
    block_ptr->BLOCKSIZE = (uchar_ptr) mem_pool_ptr->POOL_ALLOC_END_PTR - (uchar_ptr) block_ptr;
    block_ptr->U.NEXTBLOCK = NULL;
    block_ptr->POOL = mem_pool_ptr;
    mem_pool_ptr->POOL_FREE_LIST_PTR = block_ptr;
    mem_pool_ptr->POOL_ALLOC_PTR = block_ptr;
    mem_pool_ptr->POOL_FREE_PTR = block_ptr;
    mem_pool_ptr->POOL_TEST_PTR = block_ptr;

    /* Protect the list of pools while adding new pool */
    _int_disable();
    if (kernel_data->LWMEM_POOLS.NEXT == NULL)
    {
        /* Initialize the light weight memory */
        _QUEUE_INIT(&kernel_data->LWMEM_POOLS, 0);
    } /* Endif */
    _QUEUE_ENQUEUE(&kernel_data->LWMEM_POOLS, &mem_pool_ptr->LINK);
    _int_enable();
    mem_pool_ptr->VALID = LWMEM_POOL_VALID;

    _KLOGX2(KLOG_lwmem_create_pool, mem_pool_ptr);
    return ((_lwmem_pool_id) mem_pool_ptr);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Sets lightweight memory pool access rights for User-mode tasks.
 *
 * This function sets access rights for a (lightweight) memory pool. Setting
 * correct access rights is important for tasks and other code running in the
 * User-mode. User-mode access to a memory pool whose access rights are not set
 * properly causes memory protection exception to be risen.
 *
 * \param[in] mem_pool_id Lightweight memory pool for access rights to set
 * (returned by _lwmem_create_pool()).
 * \param[in] access      Access rights to set. Possible values:
 * \li POOL_USER_RW_ACCESS
 * \li POOL_USER_RO_ACCESS
 * \li POOL_USER_NO_ACCESS
 *
 * \return MQX_OK
 */

_mqx_uint _mem_set_pool_access
(
    _lwmem_pool_id mem_pool_id,
    uint_32        access
)
{
    _mqx_uint res = MQX_LWMEM_POOL_INVALID;
    LWMEM_POOL_STRUCT_PTR mem_pool_ptr = (_lwmem_pool_id)mem_pool_id;

    if (LWMEM_POOL_VALID == mem_pool_ptr->VALID)
    {
        res = _psp_mpu_add_region(mem_pool_ptr->POOL_ALLOC_START_PTR, mem_pool_ptr->POOL_ALLOC_END_PTR, access);
    }

    return res;
}

/*!
 * \brief Creates the lightweight memory pool from memory that is outside the
 * default memory pool.
 *
 * This function is an equivalent to the _lwmem_create_pool() API call but
 * it can be executed from within the User task or other code running in the CPU
 * User mode. Parameters passed to this function by pointer are required to meet
 * the memory protection requirements as described in the parameter list below.
 *
 * \param[in] mem_pool_ptr Read/write. Pointer to the definition of the pool.
 * \param[in] start        Start of the memory for the pool.
 * \param[in] size         Number of single-addressable units in the pool.
 *
 * \return Pool ID
 *
 * \see _lwmem_create_pool
 * \see _usr_lwmem_alloc
 * \see _usr_lwmem_alloc_from
 * \see _usr_lwmem_free
 */
_lwmem_pool_id _usr_lwmem_create_pool
(
    LWMEM_POOL_STRUCT_PTR mem_pool_ptr,
    pointer               start,
    _mem_size             size
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)mem_pool_ptr, (uint_32)start, (uint_32)size, 0, 0};
    return (_lwmem_pool_id)_mqx_api_call(MQX_API_LWMEM_CREATE_POOL, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Initializes a memory storage pool. Will set task error code if error occurs.
 *
 * \param[in] start The start of the memory pool.
 * \param[in] size  The size of the memory pool.
 *
 * \return A handle to the memory pool.
 */
_lwmem_pool_id _lwmem_create_pool_mapped
(
    pointer   start,
    _mem_size size
)
{ /* Body */
    LWMEM_POOL_STRUCT_PTR mem_pool_ptr;

    mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)_ALIGN_ADDR_TO_HIGHER_MEM(start);
    _mem_zero((pointer) mem_pool_ptr, (_mem_size) sizeof(LWMEM_POOL_STRUCT));

    start = (pointer) ((uchar_ptr) mem_pool_ptr + sizeof(LWMEM_POOL_STRUCT));
    _lwmem_create_pool(mem_pool_ptr, start, size - sizeof(LWMEM_POOL_STRUCT)); /* real pool size is decreased by pool header struct (must be - memory overwrite) */

    return ((_mem_pool_id) mem_pool_ptr);

} /* Endbody */

/*!
 * \brief Frees the lightweight memory block.
 *
 * If the block was allocated with one of the following functions, only the task
 * that owns the block can free it:
 * \li _lwmem_alloc()
 * \li _lwmem_alloc_from()
 * \li _lwmem_alloc_zero()
 * \li _lwmem_alloc_zero_from()
 *
 * \n Any task can free a block that was allocated with one of the following functions:
 * \li _lwmem_alloc_system()
 * \li _lwmem_alloc_system_from()
 * \li _lwmem_alloc_system_zero()
 * \li _lwmem_alloc_system_zero_from()
 *
 * \n The function also coalesces any free block found physically on either side
 * of the block being freed. If coalescing is not possible, then the block is
 * placed onto the free list.
 *
 * \param[in] mem_ptr Pointer to the block to free.
 *
 * \return MQX_OK
 * \return MQX_INVALID_POINTER (mem_ptr is NULL.)
 * \return MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 * \return MQX_NOT_RESOURCE_OWNER (If the block was allocated with _lwmem_alloc()
 * or _lwmem_alloc_zero(), only the task that allocated it can free part of it.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following
 * task error codes:
 * \li MQX_INVALID_POINTER (mem_ptr is NULL.)
 * \li MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 * \li MQX_NOT_RESOURCE_OWNER (If the block was allocated with _lwmem_alloc() or
 * _lwmem_alloc_zero(), only the task that allocated it can free part of it.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_free
 * \see _task_set_error
 */
_mqx_uint _lwmem_free
(
    pointer mem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    LWMEM_BLOCK_STRUCT_PTR free_list_ptr;
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr;
    boolean                insert;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE)
    {
        return _usr_lwmem_free(mem_ptr);
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_lwmem_free, mem_ptr);

#if MQX_CHECK_ERRORS
    /* Verify the passed in parameter */
    if (mem_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_POINTER);
        _KLOGX2(KLOG_lwmem_free, MQX_INVALID_POINTER);
        return (MQX_INVALID_POINTER);
    } /* Endif */
#endif

    block_ptr = GET_LWMEMBLOCK_PTR(mem_ptr);
    mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) block_ptr->POOL;
#if MQX_CHECK_VALIDITY
    if (mem_pool_ptr->VALID != LWMEM_POOL_VALID)
    {
        _task_set_error(MQX_LWMEM_POOL_INVALID);
        _KLOGX2(KLOG_lwmem_free, MQX_LWMEM_POOL_INVALID);
        return (MQX_LWMEM_POOL_INVALID);
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    /* Verify the passed in parameter */
    if (!((block_ptr->U.S.TASK_NUMBER == TASK_NUMBER_FROM_TASKID(kernel_data->ACTIVE_PTR->TASK_ID))
                    || (block_ptr->U.S.TASK_NUMBER == SYSTEM_TASK_NUMBER)))
    {
        _task_set_error(MQX_NOT_RESOURCE_OWNER);
        return (MQX_NOT_RESOURCE_OWNER);
    } /* Endif */
#endif

    _int_disable();
    free_list_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    while (TRUE)
    {
        if ((mem_pool_ptr->POOL_FREE_LIST_PTR == NULL) || ((pointer) block_ptr < mem_pool_ptr->POOL_FREE_LIST_PTR))
        {
            free_list_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
            if (((uchar_ptr) block_ptr + block_ptr->BLOCKSIZE) == (uchar_ptr) free_list_ptr)
            {
                /* Join with the next block on the list */
                block_ptr->BLOCKSIZE += free_list_ptr->BLOCKSIZE;
                block_ptr->U.NEXTBLOCK = free_list_ptr->U.NEXTBLOCK;
                /* The merged block should have cleared header to assure failure at multiple freeing with the same pointer */
                _mem_zero(free_list_ptr, sizeof(LWMEM_BLOCK_STRUCT));
            }
            else
            {
                block_ptr->U.NEXTBLOCK = free_list_ptr;
            } /* Endif */
            mem_pool_ptr->POOL_FREE_LIST_PTR = block_ptr;
            insert = FALSE;
            break;
        }/* Endif */
        if (((pointer) block_ptr < free_list_ptr->U.NEXTBLOCK) || (free_list_ptr->U.NEXTBLOCK == NULL))
        {
            insert = TRUE;
            break;
        }/* Endif */
        free_list_ptr = free_list_ptr->U.NEXTBLOCK;
        /* Provide window for higher priority tasks */
        mem_pool_ptr->POOL_FREE_PTR = free_list_ptr;
        _int_enable();
        _int_disable();
        free_list_ptr = mem_pool_ptr->POOL_FREE_PTR;
    } /* Endwhile */

    if (insert)
    {
        /*
         * We are between the two blocks where we are to be inserted,
         * free_list_ptr is before block_ptr,
         * block_ptr may be at end of list.
         */
        if (((uchar_ptr) block_ptr + block_ptr->BLOCKSIZE) == free_list_ptr->U.NEXTBLOCK)
        {
            /* Join with the next block on the list */
            block_ptr->BLOCKSIZE += ((LWMEM_BLOCK_STRUCT_PTR)(free_list_ptr->U.NEXTBLOCK))->BLOCKSIZE;
            block_ptr->U.NEXTBLOCK = ((LWMEM_BLOCK_STRUCT_PTR)(free_list_ptr->U.NEXTBLOCK))->U.NEXTBLOCK;
            /* The merged block should have cleared header to assure failure at multiple freeing with the same pointer */
            _mem_zero(free_list_ptr->U.NEXTBLOCK, sizeof(LWMEM_BLOCK_STRUCT));
        }
        else
        {
            /* this block is to be inserted */
            block_ptr->U.NEXTBLOCK = free_list_ptr->U.NEXTBLOCK;
        } /* Endif */

        if (((uchar_ptr) free_list_ptr + free_list_ptr->BLOCKSIZE == (uchar_ptr) block_ptr))
        {
            free_list_ptr->BLOCKSIZE += block_ptr->BLOCKSIZE;
            free_list_ptr->U.NEXTBLOCK = block_ptr->U.NEXTBLOCK;
            /* The merged block should have cleared header to assure failure at multiple freeing with the same pointer */
            _mem_zero(block_ptr, sizeof(LWMEM_BLOCK_STRUCT));
        }
        else
        {
            free_list_ptr->U.NEXTBLOCK = block_ptr;
        } /* Endif */

    } /* Endif */

    mem_pool_ptr->POOL_ALLOC_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
    mem_pool_ptr->POOL_FREE_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
    mem_pool_ptr->POOL_TEST_PTR = mem_pool_ptr->POOL_FREE_LIST_PTR;
    mem_pool_ptr->POOL_TEST2_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#if MQX_TASK_DESTRUCTION
    mem_pool_ptr->POOL_DESTROY_PTR = mem_pool_ptr->POOL_ALLOC_START_PTR;
#endif
    _int_enable();

    _KLOGX2(KLOG_lwmem_free, MQX_OK);
    return (MQX_OK);

} /* Endbody */

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Frees the given block of memory.
 *
 * This function is an equivalent to the _lwmem_free() API call but it can be
 * executed from within the User task or other code running in the CPU User mode.
 * Parameters passed to this function by pointer are required to meet the memory
 * protection requirements as described in the parameter list below.
 *
 * \param[in] mem_ptr Pointer to the block to free.
 *
 * \return MQX_OK
 * \return MQX_INVALID_POINTER (mem_ptr is NULL.)
 * \return MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 * \return MQX_NOT_RESOURCE_OWNER (If the block was allocated with _lwmem_alloc()
 * or _lwmem_alloc_zero(), only the task that allocated it can free part of it.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following
 * task error codes:
 * \li MQX_INVALID_POINTER (mem_ptr is NULL.)
 * \li MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 * \li MQX_NOT_RESOURCE_OWNER (If the block was allocated with _lwmem_alloc() or
 * _lwmem_alloc_zero(), only the task that allocated it can free part of it.)
 *
 * \see _lwmem_free
 * \see _usr_lwmem_alloc
 * \see _usr_lwmem_alloc_from
 * \see _usr_lwmem_create_pool
 */
_mqx_uint _usr_lwmem_free
(
    pointer mem_ptr
)
{
    MQX_API_CALL_PARAMS params =
    {   (uint_32)mem_ptr, 0, 0, 0, 0};
    return _mqx_api_call(MQX_API_LWMEM_FREE, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

/*!
 * \brief Gets the size of unallocated (free) memory.
 *
 * \return Size of free memory (success).
 * \return 0 (failure)
 *
 * \warning On failure, calls _task_set_error() to set the following task error code:
 * \li MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 */
_mem_size _lwmem_get_free()
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mem_size              result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE1(KLOG_lwmem_get_free);

    result = _lwmem_get_free_internal((_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL);

    _KLOGX2(KLOG_lwmem_get_free, result);
    return (result);

} /* Endbody */

/*!
 * \brief Gets the size of unallocated (free) memory from a specified pool.
 *
 * \param[in] pool_id The pool to get free size from.
 *
 * \return Size of free memory (success).
 * \return 0 (failure)
 *
 * \warning On failure, calls _task_set_error() to set the following task error code:
 * \li MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 */
_mem_size _lwmem_get_free_from
(
    pointer pool_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mem_size              result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_get_free_from, pool_id);

    result = _lwmem_get_free_internal((_lwmem_pool_id) pool_id);

    _KLOGX2(KLOG_lwmem_get_free_from, result);
    return (result);

} /* Endbody */

/*!
 * \private
 *
 * \brief Returns summarized size of free memory blocks in the pool.
 *
 * \param[in] pool_id Which pool to get free size from.
 *
 * \return Size of free memory (success).
 * \return 0 (failure)
 *
 * \warning On failure, calls _task_set_error() to set the following task error code:
 * \li MQX_LWMEM_POOL_INVALID (Pool that contains the block is not valid.)
 */
_mem_size _lwmem_get_free_internal
(
    _lwmem_pool_id pool_id
)
{ /* Body */
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    _mem_size              total_size = 0;
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) pool_id;

#if MQX_CHECK_VALIDITY
    if (mem_pool_ptr->VALID != LWMEM_POOL_VALID)
    {
        _task_set_error(MQX_LWMEM_POOL_INVALID);
        return (0);
    } /* Endif */
#endif

    _int_disable();
    block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    while (block_ptr != NULL)
    {
        /* Provide window for higher priority tasks */
        mem_pool_ptr->POOL_ALLOC_PTR = block_ptr;
        _int_enable();
        _int_disable();
        block_ptr = mem_pool_ptr->POOL_ALLOC_PTR;
        if (block_ptr == mem_pool_ptr->POOL_FREE_LIST_PTR)
        {
            total_size = 0; /* some task with higher priority did reset our loop pointer */
        } /* Endif */
        total_size += block_ptr->BLOCKSIZE;
        block_ptr = block_ptr->U.NEXTBLOCK;
    } /* Endwhile */

    _int_enable();
    return (total_size);
} /* Endbody */

/*!
 * \brief Allocates a system block of lightweight memory block from the specified
 * memory pool that is available system-wide.
 *
 * See Description of _lwmem_alloc_from() function.
 *
 * \param[in] pool_id Lightweight memory pool from which to allocate the
 * lightweight memory block (from _lwmem_create_pool()).
 * \param[in] size    Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_system_from
(
    _lwmem_pool_id pool_id,
    _mem_size      size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc_system_from, size);

    result = _lwmem_alloc_internal(size, SYSTEM_TD_PTR(kernel_data), pool_id, FALSE);

    _KLOGX2(KLOG_lwmem_alloc_system_from, result);
    return (result);

} /* Endbody */

/*!
 * \brief Allocates a system block of lightweight memory block from the default
 * memory pool that is available system-wide.
 *
 * See Description of _lwmem_alloc() function.
 *
 * \param[in] size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_get_size
 * \see _lwmem_set_default_pool
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_system
(
    _mem_size size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc_system, size);

    result = _lwmem_alloc_internal(size, SYSTEM_TD_PTR(kernel_data), (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL,
                    FALSE);

    _KLOGX2(KLOG_lwmem_alloc_system, result);
    return (result);

} /* Endbody */

/*!
 * \private
 *
 * \brief Initializes MQX to use the lwmem manager.
 *
 * \return MQX_OK
 */
_mqx_uint _lwmem_init_internal(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_POOL_STRUCT_PTR  lwmem_pool_ptr;
    uchar_ptr              start;

    _GET_KERNEL_DATA(kernel_data);

    /*
     * Move the MQX memory pool pointer past the end of kernel data.
     */
    start = (pointer) ((uchar_ptr) kernel_data + sizeof(KERNEL_DATA_STRUCT));
    lwmem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) start;
    kernel_data->KERNEL_LWMEM_POOL = lwmem_pool_ptr;

    start = (pointer) ((uchar_ptr) start + sizeof(LWMEM_POOL_STRUCT));

    _lwmem_create_pool(lwmem_pool_ptr, start, (uchar_ptr) kernel_data->INIT.END_OF_KERNEL_MEMORY - (uchar_ptr) start);

    return (MQX_OK);

} /* Endbody */

/*!
 * \private
 *
 * \brief Find the next block associated with the specified Task.
 *
 * \param[in] td_ptr       The TD whose blocks are being looked for.
 * \param[in] in_block_ptr The block last obtained.
 *
 * \return Pointer to the next block.
 * \return NULL (failure)
 */
pointer _lwmem_get_next_block_internal
(
    TD_STRUCT_PTR td_ptr,
    pointer       in_block_ptr
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_POOL_STRUCT_PTR  lwmem_pool_ptr;
    LWMEM_BLOCK_STRUCT_PTR block_ptr = in_block_ptr;
    LWMEM_BLOCK_STRUCT_PTR free_ptr;

    _GET_KERNEL_DATA(kernel_data);

    if (block_ptr == NULL)
    {
        /* first item, start on first item in first pool */
        lwmem_pool_ptr = kernel_data->KERNEL_LWMEM_POOL;
        block_ptr = lwmem_pool_ptr->POOL_ALLOC_START_PTR;
    }
    else
    {
        /* continued, get lwmem pool from provided in_block_ptr */
        block_ptr = GET_LWMEMBLOCK_PTR(in_block_ptr);
        lwmem_pool_ptr = block_ptr->POOL;

        block_ptr = (LWMEM_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + block_ptr->BLOCKSIZE);
    }

    _int_disable();

    do
    {
        free_ptr = lwmem_pool_ptr->POOL_FREE_LIST_PTR;

        while ((uchar_ptr) block_ptr < (uchar_ptr) lwmem_pool_ptr->POOL_ALLOC_END_PTR)
        {
            if (block_ptr->U.S.TASK_NUMBER == TASK_NUMBER_FROM_TASKID(td_ptr->TASK_ID))
            {
                /* check for block is not free block */
                while (free_ptr && free_ptr < block_ptr)
                {
                    free_ptr = free_ptr->U.NEXTBLOCK;
                }

                if (free_ptr != block_ptr)
                {
                    /* This block is owned by the target task and it's not free block*/
                    _int_enable();

                    return ((pointer) ((uchar_ptr) block_ptr + sizeof(LWMEM_BLOCK_STRUCT)));
                }
            }
            block_ptr = (LWMEM_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + block_ptr->BLOCKSIZE);
        }

        /* continue in next lwmem pool */
        lwmem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)(lwmem_pool_ptr->LINK.NEXT);
        block_ptr = lwmem_pool_ptr->POOL_ALLOC_START_PTR;
    } while (lwmem_pool_ptr != kernel_data->KERNEL_LWMEM_POOL); /* repeat until processed lwmem pool is not first pool (pool list is circular list) */

    _int_enable();

    return (NULL);
}

/*!
 * \brief Sets the value of the default lightweight memory pool.
 *
 * Because MQX allocates lightweight memory blocks from the default lightweight
 * memory pool when an application calls _lwmem_alloc(), _lwmem_alloc_system(),
 * _lwmem_alloc_system_zero() or _lwmem_alloc_zero(), the application must first
 * call _lwmem_set_default_pool().
 *
 * \param[in] pool_id New pool ID.
 *
 * \return Previous pool ID.
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwsem_destroy
 * \see _lwsem_post
 * \see _lwsem_test
 * \see _lwsem_wait
 * \see _lwsem_wait_for
 * \see _lwsem_wait_ticks
 * \see _lwsem_wait_until
 */
_lwmem_pool_id _lwmem_set_default_pool
(
    _lwmem_pool_id pool_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _lwmem_pool_id         old_pool_id;

    _GET_KERNEL_DATA(kernel_data);

    old_pool_id = kernel_data->KERNEL_LWMEM_POOL;
    kernel_data->KERNEL_LWMEM_POOL = pool_id;
    return (old_pool_id);

} /* Endbody */

/*!
 * \brief Gets the size of the lightweight memory block.
 *
 * The size is the actual size of the block and might be larger than the size
 * that a task requested.
 *
 * \param[in] mem_ptr Pointer to the lightweight memory block.
 *
 * \return Number of single-addressable units in the block (success).
 * \return 0 (failure)
 */
_mem_size _lwmem_get_size
(
    pointer mem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_BLOCK_STRUCT_PTR block_ptr;

    _GET_KERNEL_DATA(kernel_data);
#if MQX_CHECK_ERRORS
    if (mem_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_POINTER);
        return (0);
    } /* Endif */
#endif

    /* Compute the start of the block  */
    block_ptr = GET_LWMEMBLOCK_PTR(mem_ptr);
    /* The size includes the block overhead, which the user is not
     * interested in. If the size is less than the overhead,
     * then there is a bad block or bad block pointer.
     */
#if MQX_CHECK_ERRORS
    if (block_ptr->BLOCKSIZE <= (_mem_size) sizeof(LWMEM_BLOCK_STRUCT))
    {
        _task_set_error(MQX_INVALID_POINTER);
        return (0);
    } /* Endif */
#endif

    return (block_ptr->BLOCKSIZE - (_mem_size) sizeof(LWMEM_BLOCK_STRUCT));

} /* Endbody */

/* Move to standalone file */

#if MQX_ALLOW_TYPED_MEMORY
/*!
 * \brief Gets type of the specified block.
 *
 * \param[in] mem_ptr Pointer to the lightweight memory block.
 *
 * \return Type of memory block.
 */
_mem_type _lwmem_get_type
(
    pointer mem_ptr
)
{
    LWMEM_BLOCK_STRUCT_PTR block_ptr;

    block_ptr = GET_LWMEMBLOCK_PTR(mem_ptr);
    return block_ptr->U.S.MEM_TYPE;
}

/*!
 * \brief Sets the type of the specified block.
 *
 * \param[in] mem_ptr  Pointer to the lightweight memory block.
 * \param[in] mem_type Type of lightweight memory block to set.
 *
 * \return TRUE (success) or FALSE (failure: mem_ptr ic NULL).
 */
boolean _lwmem_set_type
(
    pointer   mem_ptr,
    _mem_type mem_type
)
{
    LWMEM_BLOCK_STRUCT_PTR block_ptr;

    if (mem_ptr != NULL)
    {
        block_ptr = GET_LWMEMBLOCK_PTR(mem_ptr);
        block_ptr->U.S.MEM_TYPE = mem_type;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

/*!
 * \brief Sets the type of the specified block.
 *
 * \return Highest size of used memory.
 */
pointer _lwmem_get_highwater(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return (((LWMEM_POOL_STRUCT_PTR)(kernel_data->KERNEL_LWMEM_POOL))->HIGHWATER);

}

/*!
 * \brief Tests all lightweight memory for errors.
 *
 * The function checks the checksums in the headers of all lightweight memory blocks.
 * \n The function can be called by only one task at a time because it keeps
 * state-in-progress variables that MQX controls. This mechanism lets other tasks
 * allocate and free lightweight memory while _lwmem_test() runs.
 *
 * \param[out] pool_error_ptr  Pointer to the pool in error (points to NULL if no
 * error was found).
 * \param[out] block_error_ptr Pointer to the block in error (points to NULL if
 * no error was found).
 *
 * \return MQX_OK
 * \return MQX_LWMEM_POOL_INVALID (Lightweight memory pool is corrupted.)
 * \return MQX_CORRUPT_STORAGE_POOL (A memory pool pointer is not correct.)
 * \return MQX_CORRUPT_STORAGE_POOL_FREE_LIST (Memory pool freelist is corrupted.)
 * \return MQX_CORRUPT_QUEUE (An error was found.)
 *
 * \warning Can be called by only one task at a time (see description).
 * \warning Disables and enables interrupts.
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 */
_mqx_uint _lwmem_test
(
    _lwmem_pool_id _PTR_ pool_error_ptr,
    pointer _PTR_        block_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr;
    LWMEM_BLOCK_STRUCT_PTR queue_ptr = NULL;
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    _mqx_uint              i;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    /*
     * It is not considered an error if the lwmem component has not been
     * created yet
     */
    if (kernel_data->LWMEM_POOLS.NEXT == NULL)
    {
        return (MQX_OK);
    } /* Endif */

    result = _queue_test(&kernel_data->LWMEM_POOLS, pool_error_ptr);
    if (result != MQX_OK)
    {
        return (result);
    } /* Endif */
    _int_disable();
    i = _QUEUE_GET_SIZE(&kernel_data->LWMEM_POOLS);
    mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) (pointer) kernel_data->LWMEM_POOLS.NEXT;
    while (i--)
    {
        if (mem_pool_ptr->VALID != LWMEM_POOL_VALID)
        {
            _int_enable();
            *pool_error_ptr = mem_pool_ptr;
            *block_error_ptr = NULL;
            return (MQX_LWMEM_POOL_INVALID);
        } /* Endif */

        /* Make sure pool is ok */
        block_ptr = mem_pool_ptr->POOL_ALLOC_START_PTR;
        while ((uchar_ptr) block_ptr < (uchar_ptr) mem_pool_ptr->POOL_ALLOC_END_PTR)
        {
            mem_pool_ptr->POOL_TEST2_PTR = block_ptr;
            _int_enable();
            _int_disable();
            block_ptr = mem_pool_ptr->POOL_TEST2_PTR;
            if (block_ptr->POOL != mem_pool_ptr)
            {
                _int_enable();
                *pool_error_ptr = mem_pool_ptr;
                *block_error_ptr = (pointer) block_ptr;
                return (MQX_CORRUPT_STORAGE_POOL);
            } /* Endif */
            block_ptr = (LWMEM_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + block_ptr->BLOCKSIZE);
        } /* Endwhile */

        /* Make sure Freelist is ok */
        block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
        while (block_ptr)
        {
            /* Provide window for higher priority tasks */
            mem_pool_ptr->POOL_TEST_PTR = block_ptr;
            _int_enable();
            _int_disable();
            block_ptr = mem_pool_ptr->POOL_TEST_PTR;
            if (((pointer) block_ptr < mem_pool_ptr->POOL_ALLOC_START_PTR) || ((pointer) block_ptr
                            > mem_pool_ptr->POOL_ALLOC_END_PTR) || (block_ptr->POOL != mem_pool_ptr)
                            || (block_ptr->U.NEXTBLOCK && (block_ptr->U.NEXTBLOCK <= (pointer) ((uchar_ptr) block_ptr
                                            + block_ptr->BLOCKSIZE))))
            {
                /* This block is in error */
                _int_enable();
                *pool_error_ptr = mem_pool_ptr;
                *block_error_ptr = block_ptr;
                return (MQX_CORRUPT_STORAGE_POOL_FREE_LIST);
            } /* Endif */
            block_ptr = block_ptr->U.NEXTBLOCK;
        } /* Endwhile */
        mem_pool_ptr = (pointer) mem_pool_ptr->LINK.NEXT;
    } /* Endwhile */
    _int_enable();

    *pool_error_ptr = NULL;
    *block_error_ptr = NULL;
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Transfers the ownership of the lightweight memory block from one task
 * to another.
 *
 * \param[in] memory_ptr The memory block whose ownership is to be transferred.
 * \param[in] source_id  Task ID of the current owner.
 * \param[in] target_id  Task ID of the new owner.
 *
 * \return MQX_OK
 * \return MQX_INVALID_POINTER (Block_ptr is NULL.)
 * \return MQX_INVALID_TASK_ID (Source or target does not represent a valid task.)
 * \return MQX_NOT_RESOURCE_OWNER (Block is not a resource of the task represented
 * by source.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_INVALID_POINTER (Block_ptr is NULL.)
 * \li MQX_INVALID_TASK_ID (Source or target does not represent a valid task.)
 * \li MQX_NOT_RESOURCE_OWNER (Block is not a resource of the task represented
 * by source.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _task_set_error
 */
_mqx_uint _lwmem_transfer
(
    pointer  memory_ptr,
    _task_id source_id,
    _task_id target_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_BLOCK_STRUCT_PTR block_ptr;
    TD_STRUCT_PTR          source_td;
    TD_STRUCT_PTR          target_td;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_lwmem_transfer, memory_ptr, source_id, target_id);

#if MQX_CHECK_ERRORS
    if (memory_ptr == NULL)
    {
        _task_set_error(MQX_INVALID_POINTER);
        _KLOGX2(KLOG_lwmem_transfer, MQX_INVALID_POINTER);
        return (MQX_INVALID_POINTER);
    } /* Endif */
#endif

    /* Verify the block */
    block_ptr = GET_LWMEMBLOCK_PTR(memory_ptr);

    source_td = (TD_STRUCT_PTR) _task_get_td(source_id);
    target_td = (TD_STRUCT_PTR) _task_get_td(target_id);
#if MQX_CHECK_ERRORS
    if ((source_td == NULL) || (target_td == NULL))
    {
        _task_set_error(MQX_INVALID_TASK_ID);
        _KLOGX2(KLOG_lwmem_transfer, MQX_INVALID_TASK_ID);
        return (MQX_INVALID_TASK_ID);
    } /* Endif */
#endif
#if MQX_CHECK_ERRORS
    if (block_ptr->U.S.TASK_NUMBER != TASK_NUMBER_FROM_TASKID(source_td->TASK_ID))
    {
        _task_set_error(MQX_NOT_RESOURCE_OWNER);
        return (MQX_NOT_RESOURCE_OWNER);
    } /* Endif */
#endif

    block_ptr->U.S.TASK_NUMBER = TASK_NUMBER_FROM_TASKID(target_td->TASK_ID);

    _KLOGX2(KLOG_lwmem_transfer, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \private
 *
 * \brief This function transfers the ownership of a block of memory from an owner
 * task to another task.
 *
 * \param[in] memory_ptr The address of the USER_AREA in the memory block to transfer.
 * \param[in] target_td  The target task descriptor.
 */
void _lwmem_transfer_internal
(
    pointer       memory_ptr,
    TD_STRUCT_PTR target_td
)
{ /* Body */
    LWMEM_BLOCK_STRUCT_PTR block_ptr;

    /* Verify the block */
    block_ptr = GET_LWMEMBLOCK_PTR(memory_ptr);
    block_ptr->U.S.TASK_NUMBER = TASK_NUMBER_FROM_TASKID(target_td->TASK_ID);

} /* Endbody */

/*!
 * \private
 *
 * \brief This routine transfers the ownership of a block of memory from an owner
 * task to another task.
 *
 * \param[in] memory_ptr The memory block whose ownership is to be transferred.
 * \param[in] source_td  The source (owner) TD.
 * \param[in] target_td  The target (new owner) TD.
 *
 * \return MQX_OK
 */
_mqx_uint _lwmem_transfer_td_internal
(
    pointer       memory_ptr,
    TD_STRUCT_PTR source_td,
    TD_STRUCT_PTR target_td
)
{ /* Body */
    LWMEM_BLOCK_STRUCT_PTR block_ptr;

    block_ptr = GET_LWMEMBLOCK_PTR(memory_ptr);
    block_ptr->U.S.TASK_NUMBER = TASK_NUMBER_FROM_TASKID(target_td->TASK_ID);

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Allocates a system(zero-filled) block of lightweight memory block from
 * the specified memory pool.
 *
 * See Description of _lwmem_alloc_from() function.
 *
 * \param[in] pool_id        Lightweight memory pool from which to allocate the
 * lightweight memory block (from _lwmem_create_pool()).
 * \param[in] size           Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_system_zero
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_system_zero_from
(
    _lwmem_pool_id pool_id,
    _mem_size      size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc_system_zero_from, size);

    result = _lwmem_alloc_internal(size, SYSTEM_TD_PTR(kernel_data), pool_id, TRUE);

    _KLOGX2(KLOG_lwmem_alloc_system_zero_from, result);
    return (result);

} /* Endbody */

/*!
 * \brief Allocates a system (zero-filled) block of lightweight memory block from
 * the default memory pool.
 *
 * See Description of _lwmem_alloc() function.
 *
 * \param[in] size Number of single-addressable units to allocate.
 *
 * \return Pointer to the lightweight memory block (success).
 * \return NULL (Failure: see Task error codes.)
 *
 * \warning On failure, calls _task_set_error() to set one of the following task
 * error codes:
 * \li MQX_LWMEM_POOL_INVALID (Memory pool to allocate from is invalid.)
 * \li MQX_OUT_OF_MEMORY (MQX cannot find a block of the requested size.)
 *
 * \see _lwmem_alloc
 * \see _lwmem_alloc_system
 * \see _lwmem_alloc_zero
 * \see _lwmem_alloc_at
 * \see _lwmem_alloc_align
 * \see _lwmem_alloc_align_from
 * \see _lwmem_alloc_from
 * \see _lwmem_alloc_system_from
 * \see _lwmem_alloc_system_zero_from
 * \see _lwmem_alloc_zero_from
 * \see _lwmem_create_pool
 * \see _lwmem_free
 * \see _lwmem_get_size
 * \see _lwmem_set_default_pool
 * \see _lwmem_transfer
 * \see _msg_alloc
 * \see _msg_alloc_system
 * \see _task_set_error
 */
pointer _lwmem_alloc_system_zero
(
    _mem_size size
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_lwmem_alloc_system_zero, size);

    result = _lwmem_alloc_internal(size, SYSTEM_TD_PTR(kernel_data), (_lwmem_pool_id) kernel_data->KERNEL_LWMEM_POOL,
                    TRUE);

    _KLOGX2(KLOG_lwmem_alloc_system_zero, result);
    return (result);

} /* Endbody */

#endif /* MQX_USE_LWMEM */
