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
 * $FileName: partition.c$
 * $Version : 3.8.2.0$
 * $Date    : Jun-25-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Partititon component.
 *
 *END************************************************************************/

#include "mqx_inc.h"
#if MQX_USE_PARTITIONS
#include "partition.h"
#include "partition_prv.h"

/*!
 * \brief Allocates a private partition block from the partition.
 * 
 * The functions allocate a fixed-size memory block, which the task owns.
 * 
 * \param[in] partition Partition from which to allocate the partition block.
 * 
 * \return Pointer to the partition block (success).
 * \return NULL (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * \li PARTITION_OUT_OF_BLOCKS (All the partition blocks in the partition are 
 * allocated (for static partitions only).)
 * \li PARTITION_BLOCK_INVALID_CHECKSUM (MQX found an incorrect checksum in the 
 * partition block header.)
 * \li Task error code set by _mem_alloc_system() MQX cannot allocate memory for 
 * the partition block (for dynamic partitions only).     
 * 
 * \see _partition_alloc_zero
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero
 * \see _partition_create
 * \see _task_set_error
 * \see _mem_alloc_system
 */ 
pointer _partition_alloc
(
    _partition_id partition
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_partition_alloc, partition);

    result = _partition_alloc_internal((PARTPOOL_STRUCT_PTR) partition, kernel_data->ACTIVE_PTR);

    _KLOGX2(KLOG_partition_alloc, result);

    return (result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Allocates a private partition block from the partition.
 * 
 * \param[in] partpool_ptr The partition to obtain the memory block from.
 * \param[in] td_ptr       The task to own the partition block.
 * 
 * \return Pointer to the partition block (success).
 * \return NULL (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * \li PARTITION_OUT_OF_BLOCKS (All the partition blocks in the partition are 
 * allocated (for static partitions only).)
 * \li PARTITION_BLOCK_INVALID_CHECKSUM (MQX found an incorrect checksum in the 
 * partition block header.)
 * \li Task error code set by _mem_alloc_system() MQX cannot allocate memory for 
 * the partition block (for dynamic partitions only). 
 */ 
pointer _partition_alloc_internal
(
    PARTPOOL_STRUCT_PTR partpool_ptr,
    TD_STRUCT_PTR       td_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
    PARTPOOL_BLOCK_STRUCT_PTR           partpool_block_ptr;
    _mqx_uint                           number_of_blocks;

#if MQX_CHECK_ERRORS
    if (partpool_ptr == NULL)
    {
        _task_set_error(PARTITION_INVALID);
        return NULL;
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _task_set_error(PARTITION_INVALID);
        return NULL;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    _GET_KERNEL_DATA(kernel_data);

    _INT_DISABLE();

    if (partpool_ptr->AVAILABLE == 0)
    {
        if ((partpool_ptr->PARTITION_TYPE == PARTITION_DYNAMIC) && partpool_ptr->GROW_BLOCKS
                        && ((partpool_ptr->MAXIMUM_BLOCKS == 0) || (partpool_ptr->TOTAL_BLOCKS
                                        < partpool_ptr->MAXIMUM_BLOCKS)))
        {
            /* We can attempt to grow the partition */
            number_of_blocks = partpool_ptr->GROW_BLOCKS;
            if ((partpool_ptr->MAXIMUM_BLOCKS != 0) && (number_of_blocks > (partpool_ptr->MAXIMUM_BLOCKS
                            - partpool_ptr->TOTAL_BLOCKS)))
            {
                number_of_blocks = partpool_ptr->MAXIMUM_BLOCKS - partpool_ptr->TOTAL_BLOCKS;
            } /* Endif */

            /* 
             * Temporarily boost up TOTAL BLOCKS to allow us to
             * enable interrupts during memory allocation
             */
            partpool_ptr->TOTAL_BLOCKS += number_of_blocks;
            _int_enable();

#if PSP_MEMORY_ALIGNMENT
            partpool_block_ptr = (PARTPOOL_BLOCK_STRUCT_PTR) _mem_alloc_system(
                            (_mem_size) (sizeof(PARTPOOL_BLOCK_STRUCT) + PSP_MEMORY_ALIGNMENT
                                            + (partpool_ptr->BLOCK_SIZE * number_of_blocks)));
#else
            partpool_block_ptr = _mem_alloc_system((_mem_size)
                            (sizeof(PARTPOOL_BLOCK_STRUCT) +
                                            (partpool_ptr->BLOCK_SIZE * number_of_blocks)));
#endif /* PSP_MEMORY_ALIGNMENT */
            if (partpool_block_ptr != NULL)
            {
                _mem_set_type(partpool_block_ptr, MEM_TYPE_PARTITION_BLOCKS);

                _partition_extend_internal(partpool_ptr, partpool_block_ptr, number_of_blocks);
            }
            else
            {
                partpool_ptr->TOTAL_BLOCKS -= number_of_blocks;
            } /* Endif */

            _int_disable();

        } /* Endif */

        if (((volatile PARTPOOL_STRUCT _PTR_) partpool_ptr)->AVAILABLE == 0)
        {
            _int_enable();
            _task_set_error(PARTITION_OUT_OF_BLOCKS);
            return NULL;
        } /* Endif */
    } /* Endif */

    block_ptr = partpool_ptr->FREE_LIST_PTR;

#if MQX_CHECK_VALIDITY
    if (!VALID_PARTITION_CHECKSUM(block_ptr))
    {
        _int_enable();
        _task_set_error(PARTITION_BLOCK_INVALID_CHECKSUM);
        return NULL;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    partpool_ptr->FREE_LIST_PTR = block_ptr->LINK.NEXT_BLOCK_PTR;
    --partpool_ptr->AVAILABLE;

    if ((partpool_ptr->TOTAL_BLOCKS - partpool_ptr->AVAILABLE) > partpool_ptr->MAX_BLOCKS_USED)
    {
        partpool_ptr->MAX_BLOCKS_USED = partpool_ptr->TOTAL_BLOCKS - partpool_ptr->AVAILABLE;
    }/* Endif */
    block_ptr->TASK_ID = td_ptr->TASK_ID;
    block_ptr->LINK.PARTITION_PTR = partpool_ptr;

    CALC_PARTITION_CHECKSUM(block_ptr);

    /* Reset partition test walker */
    partpool_ptr->TEST_FREE_PTR = partpool_ptr->FREE_LIST_PTR;

    _INT_ENABLE();

    return (pointer) ((uchar_ptr) block_ptr + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

} /* Endbody */

/*!
 * \brief Gets the number of free partition blocks in the partition.
 * 
 * \param[in] partition Partition about which to get info.
 * 
 * \return Number of free partition blocks (success).
 * \return MAX_MQX_UINT (failure)
 * 
 * \warning On failure, calls _task_set_error() to set the following task error 
 * code:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * 
 * \see _partition_get_block_size
 * \see _partition_get_max_used_blocks
 * \see _partition_get_total_blocks
 * \see _partition_get_total_size
 * \see _task_set_error 
 */ 
_mqx_uint _partition_get_free_blocks
(
    _partition_id partition
)
{ /* Body */
    PARTPOOL_STRUCT_PTR partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _task_set_error(PARTITION_INVALID);
        return (MAX_MQX_UINT);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    return (partpool_ptr->AVAILABLE);

} /* Endbody */

/*!
 * \brief Creates the partition kernel component.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)
 * 
 * \warning Cannot be called from an ISR.
 * \warning Might block the calling task.
 * 
 * \see _partition_create
 * \see _partition_destroy   
 */ 
_mqx_uint _partition_create_component(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR         kernel_data;
    PARTITION_COMPONENT_STRUCT_PTR part_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE1(KLOG_partition_create_component);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_partition_create_component, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    _lwsem_wait((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);

#if MQX_CHECK_ERRORS
    if (kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS] != NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_partition_create_component, MQX_OK);
        return (MQX_OK);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    part_component_ptr = _mem_alloc_system_zero((_mem_size) sizeof(PARTITION_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (part_component_ptr == NULL)
    {
        _lwsem_post((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);
        _KLOGX2(KLOG_partition_create_component, MQX_OUT_OF_MEMORY);
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
    _mem_set_type(part_component_ptr, MEM_TYPE_PARTITION_COMPONENT);

    kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS] = part_component_ptr;
    part_component_ptr->VALID = PARTITION_VALID;

    _QUEUE_INIT(&part_component_ptr->PARTITIONS, 0);

#if MQX_COMPONENT_DESTRUCTION
    kernel_data->COMPONENT_CLEANUP[KERNEL_PARTITIONS] = _partition_cleanup;
#endif /* MQX_COMPONENT_DESTRUCTION */

    _lwsem_post((LWSEM_STRUCT_PTR) &kernel_data->COMPONENT_CREATE_LWSEM);

    _KLOGX2(KLOG_partition_create_component, MQX_OK);

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Creates the partition at the specific location outside the default 
 * memory pool (a static partition).
 * 
 * \param[in] partition_location Pointer to the start of the partition.
 * \param[in] partition_size     The size of the partition.
 * \param[in] block_size         Number of single-addressable units in each 
 * partition block in the partition.
 * 
 * \return Partition ID (success).
 * \return PARTITION_NULL_ID (failure)
 * 
 * \warning Creates the partition component if it were not previously created.
 * \warning On failure, calls _task_set_error() to set the following task error 
 * code:
 * \li MAX_INVALID_PARAMETER (One of the following: block_size is 0 or 
 * partition_size is too small.)  
 * 
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero
 * \see _partition_calculate_size
 * \see _partition_create
 * \see _partition_extend
 * \see _task_set_error
 */ 
_partition_id _partition_create_at
(
    pointer   partition_location,
    _mem_size partition_size,
    _mem_size block_size
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    PARTPOOL_STRUCT_PTR partpool_ptr;
    uchar_ptr           firstblock_ptr;
    _mqx_uint           number_of_blocks;
    _mqx_uint           result;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_partition_create_at, partition_location, partition_size, block_size);

#if MQX_CHECK_ERRORS
    if (block_size == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_partition_create_at, PARTITION_NULL_ID);
        return (PARTITION_NULL_ID);
    } /* Endif */

    if (partition_size < sizeof(PARTPOOL_STRUCT))
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_partition_create_at, PARTITION_NULL_ID);
        return (PARTITION_NULL_ID);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    block_size += (_mem_size) sizeof(INTERNAL_PARTITION_BLOCK_STRUCT);
    _MEMORY_ALIGN_VAL_LARGER(block_size);

    partpool_ptr = (PARTPOOL_STRUCT_PTR)
    _ALIGN_ADDR_TO_HIGHER_MEM(partition_location);

    firstblock_ptr = (uchar_ptr) partpool_ptr + sizeof(PARTPOOL_STRUCT);
    firstblock_ptr = (uchar_ptr) _ALIGN_ADDR_TO_HIGHER_MEM(firstblock_ptr);
    partition_size -= (_mem_size) firstblock_ptr - (_mem_size) partition_location;

    number_of_blocks = (_mqx_uint) (partition_size / block_size);

#if MQX_CHECK_ERRORS
    if (!number_of_blocks)
    {
        _KLOGX2(KLOG_partition_create_at, PARTITION_NULL_ID);
        return (PARTITION_NULL_ID);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    result = _partition_create_internal(partpool_ptr, block_size, number_of_blocks);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _KLOGX2(KLOG_partition_create_at, result);
        return (PARTITION_NULL_ID);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    partpool_ptr->PARTITION_TYPE = PARTITION_STATIC;

    _KLOGX5(KLOG_partition_create_at, partpool_ptr, partition_size, block_size, MQX_OK);
    return ((_partition_id) partpool_ptr);

} /* Endbody */

/*!
 * \private
 * 
 * \brief Create a partition at a location with a specified number of blocks.
 * 
 * \param[in] partpool_ptr   The start of the partition.
 * \param[in] actual_size    The total size of each block with overheads.
 * \param[in] initial_blocks The initial number of blocks in the partition.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_OUT_OF_MEMORY (MQX is out of memory.)   
 */ 
_mqx_uint _partition_create_internal
(
    PARTPOOL_STRUCT_PTR partpool_ptr,
    _mem_size           actual_size,
    _mqx_uint           initial_blocks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
    PARTITION_COMPONENT_STRUCT_PTR      part_component_ptr;
    _mqx_uint                           result;

    _GET_KERNEL_DATA(kernel_data);

    part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];
    if (part_component_ptr == NULL)
    {
        result = _partition_create_component();
        part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (part_component_ptr == NULL)
        {
            return (result);
        } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
    } /* Endif */

    _mem_zero(partpool_ptr, (_mem_size) sizeof(PARTPOOL_STRUCT));

    partpool_ptr->BLOCK_SIZE = actual_size;
    partpool_ptr->POOL.VALID = PARTITION_VALID;
    partpool_ptr->POOL.NUMBER_OF_BLOCKS = initial_blocks;
    partpool_ptr->AVAILABLE = initial_blocks;
    partpool_ptr->TOTAL_BLOCKS = initial_blocks;

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) partpool_ptr + sizeof(PARTPOOL_STRUCT));

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)
    _ALIGN_ADDR_TO_HIGHER_MEM(block_ptr);

    partpool_ptr->POOL.FIRST_IBLOCK_PTR = block_ptr;

    while (initial_blocks--)
    {
        block_ptr->LINK.NEXT_BLOCK_PTR = partpool_ptr->FREE_LIST_PTR;
        CALC_PARTITION_CHECKSUM(block_ptr);
        partpool_ptr->FREE_LIST_PTR = block_ptr;
        block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + actual_size);
    } /* Endwhile */

    _int_disable();
    _QUEUE_ENQUEUE(&part_component_ptr->PARTITIONS, partpool_ptr);
    _int_enable();

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Creates the partition in the default memory pool (a dynamic partition).
 * 
 * The function creates a partition of fixed-size partition blocks in the default 
 * memory pool.
 * 
 * \param[in] block_size     Number of single-addressable units in each partition 
 * block.
 * \param[in] initial_blocks The initial number of blocks in the partition.
 * \param[in] grow_blocks    Number of blocks by which to grow the partition if 
 * all the partition blocks are allocated.
 * \param[in] maximum_blocks If grow_blocks is not 0, one of:
 * \li Maximum number of blocks in the partition.
 * \li 0 (Unlimited growth.)
 * 
 * \return Partition ID (success).
 * \return PARTITION_NULL_ID (failure)
 * 
 * \warning Creates the partition component if it were not previously created.
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li MQX_INVALID_PARAMETER (Block_size is 0.)
 * \li Task error codes returned by _mem_alloc_system()
 * 
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero
 * \see _partition_calculate_size
 * \see _partition_create_at
 * \see _partition_destroy
 * \see _task_set_error
 * \see _mem_alloc_system 
 */ 
_partition_id _partition_create
(
    _mem_size block_size,
    _mqx_uint initial_blocks,
    _mqx_uint grow_blocks,
    _mqx_uint maximum_blocks
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    PARTPOOL_STRUCT_PTR    partpool_ptr;
    _mem_size              actual_size;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE5(KLOG_partition_create, block_size, initial_blocks, grow_blocks,
                    maximum_blocks);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _task_set_error(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        _KLOGX2(KLOG_partition_create, PARTITION_NULL_ID);
        return (PARTITION_NULL_ID);
    } /* Endif */

    if (block_size == 0)
    {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_partition_create, PARTITION_NULL_ID);
        return (PARTITION_NULL_ID);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    actual_size = (_mem_size) sizeof(INTERNAL_PARTITION_BLOCK_STRUCT) + block_size;
    _MEMORY_ALIGN_VAL_LARGER(actual_size);

#if PSP_MEMORY_ALIGNMENT
    partpool_ptr = (PARTPOOL_STRUCT_PTR) _mem_alloc_system((_mem_size) (sizeof(PARTPOOL_STRUCT) + PSP_MEMORY_ALIGNMENT
                    + (actual_size * initial_blocks)));
#else
    partpool_ptr = ((PARTPOOL_STRUCT_PTR))_mem_alloc_system(
                    (_mem_size)(sizeof(PARTPOOL_STRUCT) + (actual_size * initial_blocks)));
#endif /* PSP_MEMORY_ALIGNMENT */
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (partpool_ptr == NULL)
    {
        _KLOGX2(KLOG_partition_create, PARTITION_NULL_ID);
        return (PARTITION_NULL_ID);
    }/* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
    _mem_set_type(partpool_ptr, MEM_TYPE_PARTITION);

    result = _partition_create_internal(partpool_ptr, actual_size, initial_blocks);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (result != MQX_OK)
    {
        _KLOGX2(KLOG_partition_create, PARTITION_NULL_ID);
        _mem_free(partpool_ptr);
        return (PARTITION_NULL_ID);
    } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */

    partpool_ptr->PARTITION_TYPE = PARTITION_DYNAMIC;
    partpool_ptr->GROW_BLOCKS = grow_blocks;
    partpool_ptr->MAXIMUM_BLOCKS = maximum_blocks;

    _KLOGX3(KLOG_partition_create, partpool_ptr, MQX_OK);
    return ((_partition_id) partpool_ptr);

} /* Endbody */

/*!
 * \brief Destroys a partition that is in the default memory pool (a dynamic 
 * partition).
 * 
 * If all the partition blocks in a dynamic partition are first freed, any task 
 * can destroy the partition.
 * 
 * \param[in] partition Partition ID of the partition to destroy.
 * 
 * \return MQX_OK 
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Partition component is not created.)
 * \return MQX_INVALID_PARAMETER (Partition_id is invalid.)
 * \return PARTITION_INVALID_TYPE (Partition is not a dynamic partition.)
 * \return PARTITION_ALL_BLOCKS_NOT_FREE (There are allocated partition blocks in 
 * the partition.)
 * \return Errors from _mem_free() 
 * 
 * \see _mem_free
 * \see _partition_create
 * \see _partition_free      
 */ 
_mqx_uint _partition_destroy
(
    _partition_id partition
)
{ /* Body */
    /* _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;) */
    KERNEL_DATA_STRUCT_PTR         kernel_data;
    PARTPOOL_STRUCT_PTR            partpool_ptr;
    PARTPOOL_BLOCK_STRUCT_PTR      current_ptr;
    PARTPOOL_BLOCK_STRUCT_PTR      next_ptr;
    _mqx_uint                      result;
    PARTITION_COMPONENT_STRUCT_PTR part_component_ptr;

    /* _KLOGM(_GET_KERNEL_DATA(kernel_data);) */
    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_partition_destroy, partition);

    part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];

    partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;

#if MQX_CHECK_ERRORS
    if (part_component_ptr == NULL)
    {
        _KLOGX2(KLOG_partition_destroy, MQX_COMPONENT_DOES_NOT_EXIST);
        return MQX_COMPONENT_DOES_NOT_EXIST;
    } /* Endif */

    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _KLOGX2(KLOG_partition_destroy, MQX_INVALID_PARAMETER);
        return MQX_INVALID_PARAMETER;
    } /* Endif */

    if (partpool_ptr->PARTITION_TYPE != PARTITION_DYNAMIC)
    {
        _KLOGX2(KLOG_partition_destroy, PARTITION_INVALID_TYPE);
        return PARTITION_INVALID_TYPE;
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    if (partpool_ptr->TOTAL_BLOCKS != partpool_ptr->AVAILABLE)
    {
        _KLOGX2(KLOG_partition_destroy, PARTITION_ALL_BLOCKS_NOT_FREE);
        return PARTITION_ALL_BLOCKS_NOT_FREE;
    } /* Endif */

    /* Remove the partition from the list of partitions maintained by MQX */
    _int_disable();
    _QUEUE_REMOVE(&part_component_ptr->PARTITIONS, partpool_ptr);
    _int_enable();

    partpool_ptr->POOL.VALID = 0;

    /* Free any extensions */
    current_ptr = partpool_ptr->POOL.NEXT_POOL_PTR;
    while (current_ptr)
    {
        next_ptr = current_ptr->NEXT_POOL_PTR;
        _mem_free(current_ptr);
        current_ptr = next_ptr;
    } /* Endwhile */

    result = _mem_free(partpool_ptr);

    _KLOGX2(KLOG_partition_destroy, result);

    return result;

} /* Endbody */

/*!
 * \brief Adds more blocks of memory to the static partition.
 * 
 * The function extends a partition that was created with _partition_create_at(). 
 * Based on the size of the partition's partition blocks, the function divides 
 * the additional memory into partition blocks and adds them to the partition.
 * 
 * \param[in] partition          Static partition to extend.
 * \param[in] partition_location Where the additional blocks are to start from.
 * \param[in] partition_size     Number of single-addressable units to add.
 * 
 * \return MQX_OK
 * \return PARTITION_INVALID (Partition does not represent a valid partition.)
 * \return MQX_INVALID_PARAMETER (One of the following:
 * \li Partition_size is 0.
 * \li Partition_id does not represent a static partition.)
 * 
 * \see _partition_create_at
 * \see _partition_alloc
 * \see _partition_alloc_zero
 */ 
_mqx_uint _partition_extend
(
    _partition_id partition,
    pointer       partition_location,
    _mem_size     partition_size
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)
    PARTPOOL_STRUCT_PTR                 partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;
    PARTPOOL_BLOCK_STRUCT_PTR           partpool_block_ptr;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
    _mqx_uint                           number_of_blocks;

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)

    _KLOGE4(KLOG_partition_extend, partition, partition_location, partition_size);

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _KLOGX2(KLOG_partition_extend, PARTITION_INVALID);
        return (PARTITION_INVALID);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

#if MQX_CHECK_ERRORS
    if (partpool_ptr->PARTITION_TYPE == PARTITION_DYNAMIC)
    {
        _KLOGX2(KLOG_partition_extend, MQX_INVALID_PARAMETER);
        return (MQX_INVALID_PARAMETER);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    partpool_block_ptr = (PARTPOOL_BLOCK_STRUCT_PTR)
    _ALIGN_ADDR_TO_HIGHER_MEM(partition_location);
    partition_size = partition_size - (_mem_size) partpool_block_ptr + (_mem_size) partition_location;
    _MEMORY_ALIGN_VAL_SMALLER(partition_size);

#if MQX_CHECK_ERRORS
    if (partition_size < (_mem_size) sizeof(PARTPOOL_STRUCT))
    {
        _KLOGX2(KLOG_partition_extend, MQX_INVALID_PARAMETER);
        return (MQX_INVALID_PARAMETER);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) partpool_block_ptr + sizeof(PARTPOOL_BLOCK_STRUCT));
    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)
    _ALIGN_ADDR_TO_HIGHER_MEM(block_ptr);

    number_of_blocks = (partition_size - ((uchar_ptr) block_ptr - (uchar_ptr) partpool_block_ptr))
                    / partpool_ptr->BLOCK_SIZE;
    partpool_ptr->TOTAL_BLOCKS += number_of_blocks;

    _partition_extend_internal(partpool_ptr, partpool_block_ptr, number_of_blocks);

    _KLOGX5(KLOG_partition_extend, partition, partpool_ptr, partition_size, MQX_OK);

    return MQX_OK;

} /* Endbody */

/*!
 * \private
 * 
 * \brief Adds more blocks of memory into the partition.
 * 
 * \param[in] partpool_ptr       The partition to add to.
 * \param[in] partpool_block_ptr The new block to add.
 * \param[in] number_of_blocks   The number of blocks to add.
 */ 
void _partition_extend_internal
(
    PARTPOOL_STRUCT_PTR       partpool_ptr,
    PARTPOOL_BLOCK_STRUCT_PTR partpool_block_ptr,
    _mqx_uint                 number_of_blocks
)
{ /* Body */
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;

    partpool_block_ptr->VALID = PARTITION_VALID;
    partpool_block_ptr->NUMBER_OF_BLOCKS = number_of_blocks;

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) partpool_block_ptr + sizeof(PARTPOOL_BLOCK_STRUCT));
    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)
    _ALIGN_ADDR_TO_HIGHER_MEM(block_ptr);

    partpool_block_ptr->FIRST_IBLOCK_PTR = block_ptr;

    while (number_of_blocks--)
    {
        _int_disable();
        block_ptr->LINK.NEXT_BLOCK_PTR = partpool_ptr->FREE_LIST_PTR;
        CALC_PARTITION_CHECKSUM(block_ptr);
        partpool_ptr->FREE_LIST_PTR = block_ptr;
        partpool_ptr->AVAILABLE++;
        _int_enable();
        block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + partpool_ptr->BLOCK_SIZE);
    } /* Endwhile */

    _int_disable();
    partpool_block_ptr->NEXT_POOL_PTR = partpool_ptr->POOL.NEXT_POOL_PTR;
    partpool_ptr->POOL.NEXT_POOL_PTR = partpool_block_ptr;
    _int_enable();

} /* Endbody */

/*!
 * \brief Frees the selected partition block and returns it to the partition.
 * 
 * Partition blocks created by _partition_alloc() or _partition_alloc_zero() can 
 * be freed only by task, that allocated them. However when partition block was 
 * created by _partition_alloc_system() or _partition_alloc_system_zero(), then 
 * it can be freed by any task.
 * 
 * \param[in] mem_ptr Pointer to the partition block to free.
 * 
 * \return MQX_OK
 * \return PARTITION_BLOCK_INVALID_CHECKSUM (Checksum in the partition block 
 * header is not correct; the integrity of the partition is in question.)
 * \return MQX_NOT_RESOURCE_OWNER (Task is not the one that owns the partition block.)
 * \return PARTITION_INVALID (Mem_ptr is part of a partition that is not valid.)
 * 
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero
 * \see _partition_create    
 */ 
_mqx_uint _partition_free
(
    pointer mem_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    PARTPOOL_STRUCT_PTR                 partpool_ptr;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_partition_free, mem_ptr);

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) mem_ptr - sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

#if MQX_CHECK_VALIDITY
    if (!VALID_PARTITION_CHECKSUM(block_ptr))
    {
        _KLOGX2(KLOG_partition_free, PARTITION_BLOCK_INVALID_CHECKSUM);
        return (PARTITION_BLOCK_INVALID_CHECKSUM);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

#if MQX_CHECK_ERRORS
    if (block_ptr->TASK_ID != SYSTEM_TD_PTR(kernel_data)->TASK_ID)
    {
        /* Let system blocks be freed by anyone */
        if (block_ptr->TASK_ID != kernel_data->ACTIVE_PTR->TASK_ID)
        {
            _KLOGX2(KLOG_partition_free, MQX_NOT_RESOURCE_OWNER);
            return (MQX_NOT_RESOURCE_OWNER);
        } /* Endif */
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    partpool_ptr = block_ptr->LINK.PARTITION_PTR;
    _INT_DISABLE();

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _int_enable();
        _KLOGX2(KLOG_partition_free, PARTITION_INVALID);
        return (PARTITION_INVALID);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    block_ptr->TASK_ID = 0;
    block_ptr->LINK.NEXT_BLOCK_PTR = partpool_ptr->FREE_LIST_PTR;
    partpool_ptr->FREE_LIST_PTR = block_ptr;

    ++partpool_ptr->AVAILABLE;
    CALC_PARTITION_CHECKSUM(block_ptr);

    _INT_ENABLE();
    _KLOGX2(KLOG_partition_free, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Allocates a system partition block from the partition.
 * 
 * The functions allocate a fixed-size block of memory that is not owned by any 
 * task.
 * 
 * \param[in] partition Partition from which to allocate the partition block.
 * 
 * \return Pointer to the partition block (success).
 * \return NULL (failure)
 *   
 * \warning On failure, calls _task_set_error() to set one of the task error codes 
 * described for _partition_alloc().
 *   
 * \see _partition_alloc_system_zero
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_create
 * \see _task_set_error
 */ 
pointer _partition_alloc_system
(
    _partition_id partition
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_partition_alloc_system, partition);

    result = _partition_alloc_internal((PARTPOOL_STRUCT_PTR) partition, SYSTEM_TD_PTR(kernel_data));

    _KLOGX2(KLOG_partition_alloc_system, result);
    return (result);

} /* Endbody */

/*!
 * \brief Allocates a zero-filled system partition block from the partition.
 * 
 * The functions allocate a fixed-size block of memory that is not owned by any 
 * task.
 * 
 * \param[in] partition Partition from which to allocate the partition block.
 * 
 * \return Pointer to the partition block (success).
 * \return NULL (failure)
 *   
 * \warning On failure, calls _task_set_error() to set one of the task error codes 
 * described for _partition_alloc(). 
 * 
 * \see _partition_alloc_system
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_create
 * \see _task_set_error
 */ 
pointer _partition_alloc_system_zero
(
    _partition_id partition
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    PARTPOOL_STRUCT_PTR    partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_partition_alloc_system_zero, partition);

    result = _partition_alloc_internal(partpool_ptr, SYSTEM_TD_PTR(kernel_data));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (result == NULL)
    {
        _KLOGX2(KLOG_partition_alloc_system_zero, result);
        return (result);
    } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */

    _mem_zero(result, (_mem_size) (partpool_ptr->BLOCK_SIZE - sizeof(INTERNAL_PARTITION_BLOCK_STRUCT)));

    _KLOGX2(KLOG_partition_alloc_system_zero, result);
    return (result);

} /* Endbody */

/*!
 * \brief Returns all partition blocks owned by the task to the pool.
 * 
 * \param[in] td_ptr The task being destroyed.
 */ 
void _partition_cleanup
(
    TD_STRUCT_PTR td_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    PARTPOOL_STRUCT_PTR                 partpool_ptr;
    PARTPOOL_BLOCK_STRUCT_PTR           partpool_block_ptr;
    PARTITION_COMPONENT_STRUCT_PTR      part_component_ptr;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
    _mqx_uint                           i;

    _GET_KERNEL_DATA(kernel_data);

    part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];

    if (part_component_ptr == NULL)
    {
        return; /* No work to do! */
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (part_component_ptr->VALID != PARTITION_VALID)
    {
        return;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    partpool_ptr = (PARTPOOL_STRUCT_PTR)((pointer) part_component_ptr->PARTITIONS.NEXT);
    while (partpool_ptr != (PARTPOOL_STRUCT_PTR)((pointer) &part_component_ptr->PARTITIONS))
    {
        /* Check each partition */
        partpool_block_ptr = &partpool_ptr->POOL;
        while (partpool_block_ptr != NULL)
        {
            /* Check each poolblock in the partition */
#if MQX_CHECK_VALIDITY
            if (partpool_block_ptr->VALID != PARTITION_VALID)
            {
                break;
            }/* Endif */
#endif /* MQX_CHECK_VALIDITY */
            block_ptr = partpool_block_ptr->FIRST_IBLOCK_PTR;
            i = partpool_block_ptr->NUMBER_OF_BLOCKS + 1;
            while (--i)
            {
                if ((block_ptr->TASK_ID == td_ptr->TASK_ID) && (block_ptr->LINK.PARTITION_PTR == partpool_ptr))
                { /* An allocated Block */
                    _int_disable();
                    block_ptr->LINK.NEXT_BLOCK_PTR = partpool_ptr->FREE_LIST_PTR;
                    partpool_ptr->FREE_LIST_PTR = block_ptr;
                    ++partpool_ptr->AVAILABLE;
                    CALC_PARTITION_CHECKSUM(block_ptr);
                    _int_enable();
                } /* Endif */
                block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + partpool_ptr->BLOCK_SIZE);
            } /* Endwhile */
            partpool_block_ptr = partpool_block_ptr->NEXT_POOL_PTR;
        } /* Endwhile */

        partpool_ptr = (PARTPOOL_STRUCT_PTR) partpool_ptr->NEXT;
    } /* Endwhile */

} /* Endbody */

/*!
 * \brief Tests all partition block checksums for validity.
 * 
 * \param[out] partpool_in_error       Pointer to the partition pool in error 
 * (initialized only if an error is found).
 * \param[out] partpool_block_in_error Pointer to the partition pool block in 
 * error (internal to MQX).
 * \param[out] block_in_error          Pointer to the partition block in error 
 * (initialized only if an error is found).
 * 
 * \return MQX_OK (No partitions have errors.)
 * \return MQX_INVALID_COMPONENT_BASE (Partition component data are not valid.)
 * \return PARTITION_INVALID (MQX found an invalid partition.)
 * \return PARTITION_BLOCK_INVALID_CHECKSUM (MQX found a partition block with an 
 * incorrect checksum.)
 * \return MQX_CORRUPT_QUEUE (An error is found.)
 * 
 * \warning Disables and enables interrupts.
 * 
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero
 * \see _partition_create
 * \see _partition_free     
 */ 
_mqx_uint _partition_test
(
    _partition_id _PTR_ partpool_in_error,
    pointer _PTR_       partpool_block_in_error,
    pointer _PTR_       block_in_error
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    PARTPOOL_STRUCT_PTR                 partpool_ptr;
    PARTPOOL_BLOCK_STRUCT_PTR           partpool_block_ptr;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
    PARTITION_COMPONENT_STRUCT_PTR      part_component_ptr;
    _mqx_uint                           i;
    _mqx_uint                           result;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE1(KLOG_partition_test);

    part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];
    if (part_component_ptr == NULL)
    {
        _KLOGX2(KLOG_partition_test, MQX_OK);
        return (MQX_OK);
    } /* Endif */

#if MQX_CHECK_VALIDITY
    if (part_component_ptr->VALID != PARTITION_VALID)
    {
        _KLOGX2(KLOG_partition_test, MQX_INVALID_COMPONENT_BASE);
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    _int_disable();

    /* Make sure that the queue of partitions is ok */
    result = _queue_test(&part_component_ptr->PARTITIONS, partpool_in_error);
    _int_enable();
    if (result != MQX_OK)
    {
        _KLOGX3(KLOG_partition_test, result, *partpool_in_error);
        return (result);
    } /* Endif */

    partpool_ptr = (PARTPOOL_STRUCT_PTR)((pointer) part_component_ptr->PARTITIONS.NEXT);
    while (partpool_ptr != (PARTPOOL_STRUCT_PTR)((pointer) &part_component_ptr->PARTITIONS))
    {
        /* Check all partitions */
        *partpool_in_error = partpool_ptr;

        partpool_block_ptr = &partpool_ptr->POOL;
        while (partpool_block_ptr != NULL)
        {
            /* Check all partition pool blocks contiguously */
            *partpool_block_in_error = partpool_block_ptr;

            block_ptr = partpool_block_ptr->FIRST_IBLOCK_PTR;
            i = partpool_block_ptr->NUMBER_OF_BLOCKS + 1;
            while (--i)
            {
#if MQX_CHECK_VALIDITY
                if (partpool_ptr->POOL.VALID != PARTITION_VALID)
                {
                    *block_in_error = NULL;
                    _KLOGX2(KLOG_partition_test, PARTITION_INVALID);
                    return (PARTITION_INVALID);
                } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
                _int_disable();
                if (!VALID_PARTITION_CHECKSUM(block_ptr))
                {
                    _int_enable();
                    *block_in_error = (pointer) ((uchar_ptr) block_ptr + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));
                    _KLOGX2(KLOG_partition_test, PARTITION_BLOCK_INVALID_CHECKSUM);
                    return (PARTITION_BLOCK_INVALID_CHECKSUM);
                } /* Endif */
                _int_enable();
                block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) block_ptr + partpool_ptr->BLOCK_SIZE);
            } /* Endwhile */

            partpool_block_ptr = partpool_block_ptr->NEXT_POOL_PTR;
        } /* Endwhile */

        /* Check partition free list */
        _int_disable();
        block_ptr = partpool_ptr->FREE_LIST_PTR;
        while (block_ptr != NULL)
        {
            partpool_ptr->TEST_FREE_PTR = block_ptr;
            _int_enable();
            _int_disable();
            block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR) partpool_ptr->TEST_FREE_PTR;
            if (!VALID_PARTITION_CHECKSUM(block_ptr))
            {
                _int_enable();
                *block_in_error = (pointer) ((uchar_ptr) block_ptr + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));
                _KLOGX2(KLOG_partition_test, PARTITION_BLOCK_INVALID_CHECKSUM);
                return (PARTITION_BLOCK_INVALID_CHECKSUM);
            } /* Endif */
            block_ptr = block_ptr->LINK.NEXT_BLOCK_PTR;
        } /* Endwhile */

        partpool_ptr = (PARTPOOL_STRUCT_PTR) partpool_ptr->NEXT;
        _int_enable();
    } /* Endwhile */

    _KLOGX2(KLOG_partition_test, MQX_OK);
    return MQX_OK;

} /* Endbody */

/*!
 * \brief Transfers the ownership of the partition block.
 * 
 * Any task can transfer the ownership of a private partition block or a system 
 * partition block.
 * \n If new_owner_id is the System Task ID, the partition block becomes a system 
 * partition block.
 * \n If the ownership of a system partition block is transferred to a task, the 
 * partition block becomes a resource of the task.
 * 
 * \param[in] mem_ptr      Pointer to the partition block to transfer.
 * \param[in] new_owner_id Task ID of the new owner.
 * 
 * \return MQX_OK 
 * \return PARTITION_BLOCK_INVALID_CHECKSUM (Checksum of the partition block 
 * header is not correct, which indicates that mem_ptr might not point to a valid 
 * partition block.)
 * \return PARTITION_INVALID_TASK_ID (Task_id is not valid.)
 * 
 * \see _partition_alloc
 * \see _partition_alloc_zero
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero   
 */ 
_mqx_uint _partition_transfer
(
    pointer  mem_ptr,
    _task_id new_owner_id
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    TD_STRUCT_PTR                       new_td_ptr;
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_partition_transfer, mem_ptr, new_owner_id);

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((uchar_ptr) mem_ptr - sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

#if MQX_CHECK_VALIDITY
    if (!VALID_PARTITION_CHECKSUM(block_ptr))
    {
        _KLOGX2(KLOG_partition_transfer, PARTITION_BLOCK_INVALID_CHECKSUM);
        return (PARTITION_BLOCK_INVALID_CHECKSUM);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    new_td_ptr = (TD_STRUCT_PTR) _task_get_td(new_owner_id);
#if MQX_CHECK_ERRORS
    if (new_td_ptr == NULL)
    {
        _KLOGX2(KLOG_partition_transfer, PARTITION_INVALID_TASK_ID);
        return (PARTITION_INVALID_TASK_ID);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    _INT_DISABLE();
    block_ptr->TASK_ID = new_owner_id;
    CALC_PARTITION_CHECKSUM(block_ptr);
    _INT_ENABLE();

    _KLOGX2(KLOG_partition_transfer, MQX_OK);
    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Gets the number of allocated partition blocks in the partition.
 * 
 * \param[in] partition The partition to obtain information about.
 * 
 * \return Number of allocated partition blocks (success).
 * \return 0 (failure)
 * 
 * \warning On failure, calls _task_set_error() to set the following task error 
 * code:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * 
 * \see _partition_get_block_size
 * \see _partition_get_free_blocks
 * \see _partition_get_total_blocks
 * \see _partition_get_total_size
 * \see _task_set_error  
 */ 
_mqx_uint _partition_get_max_used_blocks
(
    _partition_id partition
)
{ /* Body */
    PARTPOOL_STRUCT_PTR partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _task_set_error(PARTITION_INVALID);
        return 0;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    return (partpool_ptr->MAX_BLOCKS_USED);

} /* Endbody */

/*!
 * \brief Gets the total number of partition blocks in the partition.
 * 
 * The function returns the sum of the number of free partition blocks and the 
 * number of allocated partition blocks in the partition.
 * 
 * \param[in] partition The partition to obtain information about.
 * 
 * \return Total number of partition blocks in the partition (success).
 * \return 0 (failure)
 * 
 * \warning On failure, calls _task_set_error() to set the following task error 
 * code:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * 
 * \see _partition_get_block_size
 * \see _partition_get_free_blocks
 * \see _partition_get_max_used_blocks
 * \see _partition_get_total_size
 * \see _task_set_error  
 */ 
_mqx_uint _partition_get_total_blocks
(
    _partition_id partition
)
{ /* Body */
    PARTPOOL_STRUCT_PTR partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _task_set_error(PARTITION_INVALID);
        return 0;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    return (partpool_ptr->TOTAL_BLOCKS);

} /* Endbody */

/*!
 * \brief Gets the size of the partition blocks in the partition.
 * 
 * If the processor supports memory alignment, the function might return a value 
 * that is larger that what was specified when the partition was created.
 * 
 * \param[in] partition The partition to obtain information about.
 * 
 * \return Number of single-addressable units in a partition block (success).
 * \return 0 (failure) 
 * 
 * \warning On failure, calls _task_set_error() to set the following task error 
 * code:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * 
 * \see _partition_get_free_blocks
 * \see _partition_get_max_used_blocks
 * \see _partition_get_total_blocks
 * \see _partition_get_total_size
 * \see _partition_create_at
 * \see _task_set_error 
 */ 
_mem_size _partition_get_block_size
(
    _partition_id partition
)
{ /* Body */
    PARTPOOL_STRUCT_PTR partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _task_set_error(PARTITION_INVALID);
        return 0;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    return (partpool_ptr->BLOCK_SIZE - (_mem_size) sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

} /* Endbody */

/*!
 * \brief Gets the size of the partition.
 * 
 * The size of the partition includes extensions and internal overhead.
 * 
 * \param[in] partition The partition to obtain information about.
 * 
 * \return Number of single-addressable units in the partition (success).
 * \return 0 (failure)
 *    
 * \warning On failure, calls _task_set_error() to set the following task error 
 * code:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * 
 * \see _partition_get_block_size
 * \see _partition_get_free_blocks
 * \see _partition_get_max_used_blocks
 * \see _partition_get_total_blocks
 * \see _partition_extend
 * \see _task_set_error
 */ 
_mem_size _partition_get_total_size
(
    _partition_id partition
)
{ /* Body */
    PARTPOOL_STRUCT_PTR       partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;
    PARTPOOL_BLOCK_STRUCT_PTR partpool_block_ptr;
    register _mem_size        size;
    uchar_ptr                 tmp_ptr;

#if MQX_CHECK_VALIDITY
    if (partpool_ptr->POOL.VALID != PARTITION_VALID)
    {
        _task_set_error(PARTITION_INVALID);
        return 0;
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    size = 0;
    partpool_block_ptr = partpool_ptr->POOL.NEXT_POOL_PTR;
    while (partpool_block_ptr != NULL)
    {
        tmp_ptr = (uchar_ptr) partpool_block_ptr + sizeof(PARTPOOL_BLOCK_STRUCT);
        tmp_ptr = (uchar_ptr) _ALIGN_ADDR_TO_HIGHER_MEM(tmp_ptr);
        size += (_mem_size) (tmp_ptr - (uchar_ptr) partpool_block_ptr);
        partpool_block_ptr = partpool_block_ptr->NEXT_POOL_PTR;
    } /* Endif */

    tmp_ptr = (uchar_ptr) partition + sizeof(PARTPOOL_STRUCT);
    tmp_ptr = (uchar_ptr) _ALIGN_ADDR_TO_HIGHER_MEM(tmp_ptr);

    size += (_mem_size) (tmp_ptr - (uchar_ptr) partition);

    return (size + partpool_ptr->BLOCK_SIZE * partpool_ptr->TOTAL_BLOCKS);

} /* Endbody */

/*!
 * \brief Calculates the number of single-addressable units in a partition.
 * 
 * If an application wants to use as much as possible of some memory that is 
 * outside the default memory pool, it can use the function to determine the 
 * maximum number of blocks that can be created.
 * \n For a dynamic partition, the application might want to limit (based on the 
 * results of the function) the amount of memory in the default memory pool that 
 * it uses to create the partition.
 * 
 * \param[in] number_of_blocks Number of partition blocks in the partition.
 * \param[in] block_size       Number of single-addressable units in one partition 
 * block in the partition.
 * 
 * \return Number of single-addressable units in the partition.
 * 
 * \see _partition_calculate_blocks
 * \see _partition_create
 * \see _partition_create_at 
 */ 
_mem_size _partition_calculate_size
(
    _mqx_uint number_of_blocks,
    _mem_size block_size
)
{ /* Body */
    _mem_size actual_block_size;
    _mem_size size;

    actual_block_size = sizeof(INTERNAL_PARTITION_BLOCK_STRUCT) + block_size;
    _MEMORY_ALIGN_VAL_LARGER(actual_block_size);

#if PSP_MEMORY_ALIGNMENT
    size = sizeof(PARTPOOL_STRUCT) + PSP_MEMORY_ALIGNMENT + actual_block_size * number_of_blocks;
#else
    size = sizeof(PARTPOOL_STRUCT) + actual_block_size * number_of_blocks;
#endif /* PSP_MEMORY_ALIGNMENT */

    return (size);
} /* Endbody */

/*!
 * /brief Calculates the number of partition blocks in a static partition.
 * 
 * When a task creates a static partition (_partition_create_at()), it specifies 
 * the size of the partition and the size of partition blocks. The function 
 * _partition_calculate_blocks() calculates how many blocks MQX actually created, 
 * taking into account internal headers.
 * 
 * \param[in] partition_size Number of single-addressable units that the partition 
 * can occupy.
 * \param[in] block_size     Number of single-addressable units in one partition 
 * block of the partition.
 * 
 * \return Number of partition blocks in the partition.
 * 
 * \see _partition_calculate_size
 * \see _partition_create_at 
 */ 
_mqx_uint _partition_calculate_blocks
(
    _mem_size partition_size,
    _mem_size block_size
)
{ /* Body */
    _mem_size actual_size;
    _mem_size size;

    actual_size = sizeof(INTERNAL_PARTITION_BLOCK_STRUCT) + block_size;
    _MEMORY_ALIGN_VAL_LARGER(actual_size);

    size = (_mem_size) sizeof(PARTPOOL_STRUCT);
    _MEMORY_ALIGN_VAL_LARGER(size);
    return ((_mqx_uint) ((partition_size - size) / actual_size));

} /* Endbody */
/*!
 * \brief Gets size of partition.
 * 
 * \return Size of partition (Success).
 * \return 0 (Failure)   
 */ 
_mqx_uint _partition_get_number_of(void)
{
    uint_32                        count = 0;
    KERNEL_DATA_STRUCT_PTR         kernel_data;
    PARTITION_COMPONENT_STRUCT_PTR part_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];

    if (part_component_ptr)
    {
        count = part_component_ptr->PARTITIONS.SIZE;
    }
    else
    {
        count = 0;
    } /* Endif */

    return count;
}
/*!
 * \brief Allocates a zero-filled private partition block from the partition.
 * 
 * The functions allocate a fixed-size memory block, which the task owns.
 * 
 * \param[in] partition Partition from which to allocate the partition block.
 * 
 * \return Pointer to the partition block (success).
 * \return NULL (failure)
 * 
 * \warning On failure, calls _task_set_error() to set one of the following task 
 * error codes:
 * \li PARTITION_INVALID (Partition does not represent a valid partition.)
 * \li PARTITION_OUT_OF_BLOCKS (All the partition blocks in the partition are 
 * allocated (for static partitions only).)
 * \li PARTITION_BLOCK_INVALID_CHECKSUM (MQX found an incorrect checksum in the 
 * partition block header.)
 * \li Task error code set by _mem_alloc_system() MQX cannot allocate memory for 
 * the partition block (for dynamic partitions only).     
 * 
 * \see _partition_alloc
 * \see _partition_alloc_system
 * \see _partition_alloc_system_zero
 * \see _partition_create
 * \see _task_set_error
 * \see _mem_alloc_system  
 */ 
pointer _partition_alloc_zero
(
    _partition_id partition
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    PARTPOOL_STRUCT_PTR    partpool_ptr = (PARTPOOL_STRUCT_PTR) partition;
    pointer                result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_partition_alloc_zero, partition);

    result = _partition_alloc_internal(partpool_ptr, kernel_data->ACTIVE_PTR);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (result == NULL)
    {
        _KLOGX2(KLOG_partition_alloc_zero, result);
        return (result);
    } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */

    _mem_zero(result, (_mem_size) (partpool_ptr->BLOCK_SIZE - sizeof(INTERNAL_PARTITION_BLOCK_STRUCT)));

    _KLOGX2(KLOG_partition_alloc_zero, result);
    return (result);

} /* Endbody */

#endif /* MQX_USE_PARTITIONS */

/* EOF */
