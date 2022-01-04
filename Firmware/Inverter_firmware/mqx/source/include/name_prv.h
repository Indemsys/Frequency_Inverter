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
* $FileName: name_prv.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   name component.
*
*END************************************************************************/
#ifndef __name_prv_h__
#define __name_prv_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Queue FIFO */
#define NAME_TASK_QUEUE_POLICY  (0)

#define NAME_VALID              (_mqx_uint)(0x6e616d65)   /* "name" */

/*--------------------------------------------------------------------------*/
/*                      DATA STRUCTURE DEFINITIONS                          */

/* NAME STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 *   
 * \brief An individual pair of name/number.
 */
typedef struct name_struct
{
   /*! \brief The number associated with the name. */
   _mqx_max_type NUMBER;

   /*! \brief The string name stored in the table, includes null. */
   char      NAME[NAME_MAX_NAME_SIZE];

} NAME_STRUCT, _PTR_ NAME_STRUCT_PTR;
/*! \endcond */

/* NAME COMPONENT STRUCT */
/*!
 *\cond DOXYGEN_PRIVATE 
 * 
 * \brief This structure is used to store information required for name retrieval.
 * 
 * An initial structure is created and when pool growth is required, duplicate 
 * copies of this structure are created and linked via the NEXT_TABLE field.
 */
typedef struct name_component_struct
{

   /*! \brief The maximum number of names allowed in the entire pool. */
   _mqx_uint      MAX_NUMBER;

   /*! \brief The total number of names allocated in the entire pool so far. */
   _mqx_uint      TOTAL_NUMBER;

   /*! \brief The number of names allowed in this block. */   
   _mqx_uint      NUMBER_IN_BLOCK;

   /*! \brief The number of names allowed to be created in the next block. */
   _mqx_uint      GROW_NUMBER;

   /*! \brief The number of names used in the name component. */
   _mqx_uint      NUMBER;

   /*! \brief Lightweight semaphore for protecting the name component. */
   LWSEM_STRUCT   SEM;

   /*! \brief A validation stamp to verify handle correctness. */
   _mqx_uint      VALID;

   /*! \brief The address of the next block of names. */
   struct  name_component_struct _PTR_ NEXT_TABLE;

   /*! \brief A variable sized array of name/number pairs. */
   NAME_STRUCT    NAMES[1];
   
} NAME_COMPONENT_STRUCT, _PTR_ NAME_COMPONENT_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _name_add_internal(pointer, char_ptr, _mqx_max_type);
extern _mqx_uint _name_add_internal_by_index(pointer, char_ptr, _mqx_max_type, 
   _mqx_uint);
extern _mqx_uint _name_create_handle_internal(pointer _PTR_, _mqx_uint, _mqx_uint, 
   _mqx_uint, _mqx_uint);
extern _mqx_uint _name_destroy_handle_internal(pointer);
extern _mqx_uint _name_delete_internal(pointer, char_ptr);
extern _mqx_uint _name_delete_internal_by_index(pointer, _mqx_uint);
extern _mqx_uint _name_find_internal(pointer, char_ptr, _mqx_max_type_ptr);
extern _mqx_uint _name_find_name_internal(pointer, _mqx_max_type, char_ptr);
extern _mqx_uint _name_find_internal_by_index(pointer, _mqx_uint, _mqx_max_type_ptr);
extern _mqx_uint _name_init_internal(pointer _PTR_, _mqx_uint, _mqx_uint, _mqx_uint,
   _mqx_uint);
extern _mqx_uint _name_test_internal(NAME_COMPONENT_STRUCT_PTR, pointer _PTR_, 
   pointer _PTR_);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
