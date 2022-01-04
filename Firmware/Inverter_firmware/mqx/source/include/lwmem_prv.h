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
* $FileName: lwmem_prv.h$
* $Version : 3.8.17.0$
* $Date    : Jun-6-2012$
* $Version : 3.8.17.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains definitions private to the light weight
*   memory manger.
*
*END************************************************************************/
#ifndef __lwmem_prv_h__
#define __lwmem_prv_h__

/*--------------------------------------------------------------------------*/
/*
 *                    CONSTANT DEFINITIONS
 */

/* The correct value for the light weight memory pool VALID field */
#define LWMEM_POOL_VALID   (_mqx_uint)(0x6C6D6570)    /* "lmep" */

/* The smallest amount of memory that is allocated */
#define LWMEM_MIN_MEMORY_STORAGE_SIZE \
   ((_mem_size)(sizeof(LWMEM_BLOCK_STRUCT) + PSP_MEMORY_ALIGNMENT) & \
   PSP_MEMORY_ALIGNMENT_MASK)


/*--------------------------------------------------------------------------*/
/*
 *                      MACROS DEFINITIONS
 */

/*
 * get the location of the block pointer, given the address as provided
 * to the application by _lwmem_alloc.
 */
#define GET_LWMEMBLOCK_PTR(addr) \
   (LWMEM_BLOCK_STRUCT_PTR)((pointer)((uchar_ptr)(addr) - \
      sizeof(LWMEM_BLOCK_STRUCT)))

/*--------------------------------------------------------------------------*/
/*
 *                    DATATYPE DECLARATIONS
 */

/* LWMEM BLOCK STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This structure is used to define the storage blocks used by the memory 
 * manager in MQX.
 */
typedef struct lwmem_block_struct
{
   /*! \brief The size of the block. */
   _mem_size      BLOCKSIZE;

   /*! \brief The pool the block came from. */
   _lwmem_pool_id POOL;

   /*!
    * \brief For an allocated block, this is the task ID of the owning task.
    * When on the free list, this points to the next block on the free list.
    */
   union {
      pointer     NEXTBLOCK;
      struct {
         _task_number    TASK_NUMBER;
         _mem_type       MEM_TYPE;
      } S;
   } U;

} LWMEM_BLOCK_STRUCT, _PTR_ LWMEM_BLOCK_STRUCT_PTR;
/*! \endcond */

#define _GET_LWMEMBLOCK_TYPE(ptr)      (((LWMEM_BLOCK_STRUCT_PTR)GET_LWMEMBLOCK_PTR(ptr))->U.S.MEM_TYPE)

/*--------------------------------------------------------------------------*/
/*
 *                  PROTOTYPES OF FUNCTIONS
 */

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __TAD_COMPILE__

extern pointer   _lwmem_alloc_internal(_mem_size, TD_STRUCT_PTR, _lwmem_pool_id, boolean);
extern pointer   _lwmem_alloc_at_internal(_mem_size, pointer, TD_STRUCT_PTR, _lwmem_pool_id, boolean);
extern pointer   _lwmem_alloc_align_internal(_mem_size, _mem_size, TD_STRUCT_PTR, _lwmem_pool_id, boolean);
extern _mem_size _lwmem_get_free_internal(_lwmem_pool_id);
extern void      _lwmem_cleanup_internal(TD_STRUCT_PTR);
extern void      _lwmem_transfer_internal(pointer, TD_STRUCT_PTR);
extern _mqx_uint _lwmem_transfer_td_internal(pointer, TD_STRUCT_PTR,
   TD_STRUCT_PTR);
extern _mqx_uint _lwmem_init_internal(void);
extern pointer   _lwmem_get_next_block_internal(TD_STRUCT_PTR,pointer);
extern _lwmem_pool_id _lwmem_create_pool_mapped(pointer, _mem_size);

#if MQX_ENABLE_USER_MODE
pointer          _usr_lwmem_alloc_internal(_mem_size);
_mqx_uint        _usr_lwmem_free_internal(pointer);
_lwmem_pool_id   _usr_lwmem_create_pool_internal(LWMEM_POOL_STRUCT_PTR, pointer, _mem_size);
#endif

#endif
#ifdef __cplusplus
}
#endif

#endif /* __lwmem_prv_h__ */
/* EOF */
