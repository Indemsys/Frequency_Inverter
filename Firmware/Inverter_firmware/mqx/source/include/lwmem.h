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
* $FileName: lwmem.h$
* $Version : 3.8.22.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the structure definitions and constants for an
*   application which will be using MQX.
*   All compiler provided header files must be included before mqx.h.
*
*END************************************************************************/
#ifndef __lwmem_h__
#define __lwmem_h__

#include <mqx_cnfg.h>
#if (! MQX_USE_LWMEM) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error LWMEM component is currently disabled in MQX kernel. Please set MQX_USE_LWMEM to 1 in user_config.h and recompile kernel.
#endif

#ifndef __ASM__
/*--------------------------------------------------------------------------*/
/*
 *                    DATATYPE DECLARATIONS
 */

typedef pointer _lwmem_pool_id;

/* LWMEM POOL STRUCT */
/*!
 * \brief This structure is used to define the information that defines what
 * defines a light weight memory pool.
 */
typedef struct lwmem_pool_struct
{
   /*! \brief Links lightweight memory pools together. */   
   QUEUE_ELEMENT_STRUCT LINK;

   /*! \brief Handle validation stamp. */
   _mqx_uint            VALID;

   /*! \brief The address of the start of Memory Pool blocks. */
   pointer              POOL_ALLOC_START_PTR;

   /*! \brief The address of the end of the Memory Pool. */
   pointer              POOL_ALLOC_END_PTR;

   /*! \brief The address of the head of the memory pool free list. */
   pointer              POOL_FREE_LIST_PTR;

   /*! \brief Pointer used when walking through free list by lwmem_alloc. */
   pointer              POOL_ALLOC_PTR;

   /*! \brief Pointer used when freeing memory by lwmem_free. */
   pointer              POOL_FREE_PTR;

   /*! \brief Pointer used when testing memory by lwmem_test. */
   pointer              POOL_TEST_PTR;

   /*! \brief Pointer used when testing memory by lwmem_test. */
   pointer              POOL_TEST2_PTR;

   /*! \brief Pointer used by lwmem_cleanup_internal. */
   pointer              POOL_DESTROY_PTR;
   
   /*! \brief Pointer to highwater mark. */
   pointer              HIGHWATER;

} LWMEM_POOL_STRUCT, _PTR_ LWMEM_POOL_STRUCT_PTR;


#define POOL_USER_RW_ACCESS     (MPU_UM_RW)
#define POOL_USER_RO_ACCESS     (MPU_UM_R)
#define POOL_USER_NO_ACCESS     (0)
/*--------------------------------------------------------------------------*/
/*
 *                  PROTOTYPES OF FUNCTIONS
 */

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __TAD_COMPILE__

extern pointer          _lwmem_alloc(_mem_size);
extern pointer          _lwmem_alloc_at(_mem_size, pointer);
extern pointer          _lwmem_alloc_align(_mem_size, _mem_size);
extern pointer          _lwmem_alloc_align_from(_lwmem_pool_id, _mem_size, _mem_size);
extern pointer          _lwmem_alloc_zero(_mem_size);
extern pointer          _lwmem_alloc_from(_lwmem_pool_id, _mem_size);
extern pointer          _lwmem_alloc_zero_from(_lwmem_pool_id, _mem_size);
extern _mem_size        _lwmem_get_free(void);
extern _mem_size        _lwmem_get_free_from(_lwmem_pool_id);

extern pointer          _lwmem_alloc_system(_mem_size);
extern pointer          _lwmem_alloc_system_zero(_mem_size);
extern pointer          _lwmem_alloc_system_from(_lwmem_pool_id, _mem_size);
extern pointer          _lwmem_alloc_system_zero_from(_lwmem_pool_id, _mem_size);
extern _lwmem_pool_id   _lwmem_create_pool(LWMEM_POOL_STRUCT_PTR, pointer, _mem_size);
extern _lwmem_pool_id   _lwmem_create_pool_mapped(pointer start, _mem_size  size);
extern _mqx_uint        _lwmem_free(pointer);
extern _mqx_uint        _lwmem_get_size(pointer);
extern _lwmem_pool_id   _lwmem_set_default_pool(_lwmem_pool_id);
extern _mqx_uint        _lwmem_test(_lwmem_pool_id _PTR_, pointer _PTR_);
extern _mqx_uint        _lwmem_transfer(pointer, _task_id, _task_id);

extern _lwmem_pool_id   _lwmem_get_system_pool_id(void);
extern _mem_type        _lwmem_get_type(pointer);   
extern boolean          _lwmem_set_type(pointer,_mem_type);   
extern pointer          _lwmem_get_highwater(void) ;

#if MQX_ENABLE_USER_MODE
extern pointer          _usr_lwmem_alloc(_mem_size);
extern pointer          _usr_lwmem_alloc_from(_lwmem_pool_id, _mem_size);
extern _mqx_uint        _usr_lwmem_free(pointer);
extern _lwmem_pool_id   _usr_lwmem_create_pool(LWMEM_POOL_STRUCT_PTR, pointer, _mem_size);
extern _mqx_uint        _mem_set_pool_access(_lwmem_pool_id, uint_32);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* _ASM_ */
#endif /* __lwmem_h__ */
/* EOF */
