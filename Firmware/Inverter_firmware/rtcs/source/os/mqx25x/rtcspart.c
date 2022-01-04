/**HEADER********************************************************************
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
* $FileName: rtcspart.c$
* $Version : 3.8.11.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the interface to the RTCS
*   partitions.
*
*END************************************************************************/

#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_mem_alloc
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_mem_alloc(_mem_size size)
{
   if (_RTCS_mem_pool) {
      return _mem_alloc_from(_RTCS_mem_pool, size);
   } else {
      return _mem_alloc(size);
   }
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_mem_alloc_zero
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_mem_alloc_zero(_mem_size size)
{
   if (_RTCS_mem_pool) {
      return _mem_alloc_zero_from(_RTCS_mem_pool, size);
   } else {
      return _mem_alloc_zero(size);
   }
}
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_mem_alloc_system
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_mem_alloc_system(_mem_size size)
{
   if (_RTCS_mem_pool) {
      return _mem_alloc_system_from(_RTCS_mem_pool, size);
   } else {
      return _mem_alloc_system(size);
   }
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_mem_alloc_system_zero
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_mem_alloc_system_zero(_mem_size size)
{
   if (_RTCS_mem_pool) {
      return _mem_alloc_system_zero_from(_RTCS_mem_pool, size);
   } else {
      return _mem_alloc_system_zero(size);
   }
}

#if RTCSCFG_USE_MQX_PARTITIONS
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_create
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

_rtcs_part RTCS_part_create
   (
      uint_32 size,                       /* [IN] size of one block */
      uint_32 init,                       /* [IN] initial number of blocks */
      uint_32 grow,                       /* [IN] number of blocks to grow by */
      uint_32 max,                        /* [IN] maximum number of blocks */
      int_32 (_CODE_PTR_ cons)(pointer),  /* [IN] block constructor */
      int_32 (_CODE_PTR_ dest)(pointer)   /* [IN] block destroyer */
   )
{ /* Body */
   _rtcs_part     part;
   pointer _PTR_  elem;
   uint_32        i;

   /* Allocate partition */
   part = RTCS_mem_alloc_system(sizeof(*part));
   if (!part) {
      return NULL;
   } /* Endif */
   
   _mem_set_type(part, MEM_TYPE_RTCS_PARTITION);

   part->PART = _partition_create(size + sizeof(pointer), init, grow, max);
   if (part->PART == PARTITION_NULL_ID) {
      _mem_free(part);
      return NULL;
   } /* Endif */

   /* Set up partition information */
   part->SIZE   = size;
   part->FREE   = NULL;
   part->CONSTR = cons;
   part->DESTR  = dest;

   /* Pre-construct some blocks and enqueue them */
   for (i = 0; i < init; i++) {
      elem = _partition_alloc_system(part->PART);
      if (!elem) {
         break;
      } /* Endif */
      if (part->CONSTR) {
         if (part->CONSTR(elem+1)) {
            _partition_free(elem);
            break;
         } /* Endif */
      } /* Endif */
      *elem = part;
      RTCS_part_free(elem+1);
   } /* Endfor */

   return part;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_alloc
* Returned Values : A pointer to a partition block if successful, or
*                   NULL if allocation fails.
* Comments        :
*     Allocate a partition block.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_part_alloc
   (
      _rtcs_part part            /* [IN] partition to allocate from */
   )
{ /* Body */
   pointer _PTR_  elem;

   _int_disable();
   if (!part->FREE) {
      _int_enable();

      /* Allocate a new block and construct it */
      elem = _partition_alloc_system(part->PART);
      if (!elem) {
         return NULL;
      } /* Endif */
      if (part->CONSTR) {
         if (part->CONSTR(elem+1)) {
            _partition_free(elem);
            return NULL;
         } /* Endif */
      } /* Endif */

   } else {
      // NOTE: we may have been interrupted here, and part-FREE may have become NULL 
      /* Dequeue a block */
      elem = part->FREE;
      part->FREE = *elem;
      _int_enable();

   } /* Endif */

   /* Point the block back to the partition (for RTCS_part_free()) */
   *elem = part;
   return elem + 1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_alloc_zero
* Returned Values : A pointer to a zeroed partition block if successful,
*                   or NULL if allocation fails.
* Comments        :
*     Allocate a partition block and zero it.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_part_alloc_zero
   (
      _rtcs_part part            /* [IN] partition to allocate from */
   )
{ /* Body */
   pointer elem;

   elem = RTCS_part_alloc(part);
   if (elem) {
      _mem_zero(elem, part->SIZE);
   } /* Endif */

   return elem;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_free
* Returned Values : void
* Comments        :
*     Free a partition block.
*
*END*-----------------------------------------------------------------*/

void RTCS_part_free
   (
      pointer block              /* [IN] block to free */
   )
{ /* Body */
   pointer _PTR_  elem = block;
   _rtcs_part     part;

   part = *--elem;

   _int_disable();
   *elem = part->FREE;
   part->FREE = elem;
   _int_enable();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_destroy
* Returned Values : void
* Comments        :
*     Destroy a partition
*
*END*-----------------------------------------------------------------*/

void RTCS_part_destroy
   (
      _rtcs_part part            /* [IN] partition to destroy */
   )
{ /* Body */
   pointer _PTR_  elem;

   /* Destroy all the elements */
   /* If we have destroy function ->  use it. */
    if (part->DESTR) 
    {
        while (part->FREE) 
        {
            elem = part->FREE;
            part->FREE = *elem;
            part->DESTR(elem+1);
            /*_partition_free(elem);*/
        } /* Endwhile */
    }
    else
    {   /* If not, lets use standart MQX function. */
        while (part->FREE) 
        {
            elem = part->FREE;
            part->FREE = *elem;
            _partition_free(elem);
        }
    }

   /* Free the partition */
    _partition_destroy(part->PART);
    _mem_free(part);

} /* Endbody */


#else

#define RTCS_PART_ALIGN(n) ((n) + (-(n) & 3))

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_grow
* Returned Values : RTCS_OK if successful, or RTCS_ERROR if a
*                   constructor failed
* Comments        :
*     Add blocks to a partition.
*
*END*-----------------------------------------------------------------*/

static RTCS_part_grow
   (
      _rtcs_part  part,          /* [IN] partition to grow */
      pointer     blocks,        /* [IN] array of blocks to add to part */
      uint_32     num_blocks     /* [IN] number of blocks to add to part */
   )
{ /* Body */
   uint_32  i;
   pointer  elem;

   /* Construct all the blocks */
   if (part->CONSTR) {
      elem = blocks;
      elem = (pointer _PTR_)elem + 1;
      for (i = 0; i < num_blocks; i++) {
         if (part->CONSTR(elem)) {
            if (part->DESTR) {
               while (i > 0) {
                  elem = (char_ptr)elem - part->BLOCK_SIZE;
                  part->DESTR(elem);
                  i--;
               } /* Endwhile */
            } /* Endif */
            return RTCS_ERROR;
         } /* Endif */
         elem = (char_ptr)elem + part->BLOCK_SIZE;
      } /* Endfor */
   } /* Endif */

   /* Add the blocks to the free list */
   part->NUM_BLOCKS += num_blocks;
   elem = blocks;
   for (i = 0; i < num_blocks; i++) {
      *(pointer _PTR_)elem = part;
      RTCS_part_free((pointer _PTR_)elem+1);
      elem = (char_ptr)elem + part->BLOCK_SIZE;
   } /* Endfor */

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_create
* Returned Values : An _rtcs_part if successful, or 0 if creation fails.
* Comments        :
*     Create and initialize a partition.
*
*END*-----------------------------------------------------------------*/

_rtcs_part RTCS_part_create
   (
      uint_32 size,                       /* [IN] size of one block */
      uint_32 init,                       /* [IN] initial number of blocks */
      uint_32 grow,                       /* [IN] number of blocks to grow by */
      uint_32 max,                        /* [IN] maximum number of blocks */
      int_32 (_CODE_PTR_ cons)(pointer),  /* [IN] block constructor */
      int_32 (_CODE_PTR_ dest)(pointer)   /* [IN] block destroyer */
   )
{ /* Body */
   _rtcs_part  part;

   /* Allocate partition and initial blocks */
   size = RTCS_PART_ALIGN(sizeof(pointer) + size);
   part = _mem_alloc_system(sizeof(*part) + size * init);
   if (!part) {
      return NULL;
   } /* Endif */

   _mem_set_type(part, MEM_TYPE_RTCS_PARTITION);
 
   /* Set up partition information */
   part->BLOCK_SIZE  = size;
   part->NUM_BLOCKS  = 0;
   part->GROW_BLOCKS = grow;
   part->MAX_BLOCKS  = max;
   part->GROW        = NULL;
   part->FREE        = NULL;
   part->CONSTR      = cons;
   part->DESTR       = dest;

   /* Construct all the blocks and add them to the free list */
   if (RTCS_part_grow(part, part+1, init) == RTCS_ERROR) {
      _mem_free(part);
      return NULL;
   } /* Endif */

   return part;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_alloc
* Returned Values : A pointer to a partition block if successful, or
*                   NULL if allocation fails.
* Comments        :
*     Allocate a partition block.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_part_alloc
   (
      _rtcs_part part            /* [IN] partition to allocate from */
   )
{ /* Body */
   uint_32        new_blocks;
   pointer        grow;
   pointer _PTR_  elem;

   if (!part->FREE && part->GROW_BLOCKS) {
      _int_disable();
      if (!part->MAX_BLOCKS || (part->NUM_BLOCKS < part->MAX_BLOCKS)) {
         new_blocks = part->GROW_BLOCKS;
         if (part->MAX_BLOCKS && (new_blocks > part->MAX_BLOCKS-part->NUM_BLOCKS)) {
            /* Don't grow beyond maximum */
            new_blocks = part->MAX_BLOCKS - part->NUM_BLOCKS;
         } /* Endif */
         _int_enable();

         grow = _mem_alloc_system(sizeof(pointer) + part->BLOCK_SIZE * new_blocks);
         if (grow) {
            _mem_set_type(part, MEM_TYPE_RTCS_PARTITION);

            _int_disable();
            if (!part->MAX_BLOCKS || (part->NUM_BLOCKS+new_blocks <= part->MAX_BLOCKS)) {

               /* Construct all the blocks and add them to the free list */
               elem = grow;
               if (RTCS_part_grow(part, elem+1, new_blocks) == RTCS_ERROR) {
                  _int_enable();
                  _mem_free(grow);
               } else {
                  *elem = part->GROW;
                  part->GROW = elem;
                  _int_enable();
               } /* Endif */

            } else {
               _int_enable();
               _mem_free(grow);
            } /* Endif */

         } /* Endif */
      } else {
         _int_enable();
      } /* Endif */
   } /* Endif */

   _int_disable();
   if (!part->FREE) {
      _int_enable();
      return NULL;
   } /* Endif */

   /* Dequeue a block */
    elem = part->FREE;
   part->FREE = *elem;
   _int_enable();

   /* Point the block back to the partition (for RTCS_part_free()) */
   *elem = part;
   return elem + 1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_alloc_zero
* Returned Values : A pointer to a zeroed partition block if successful,
*                   or NULL if allocation fails.
* Comments        :
*     Allocate a partition block and zero it.
*
*END*-----------------------------------------------------------------*/

pointer RTCS_part_alloc_zero
   (
      _rtcs_part part            /* [IN] partition to allocate from */
   )
{ /* Body */
   pointer elem;

   elem = RTCS_part_alloc(part);
   if (elem) {
      _mem_zero(elem, part->BLOCK_SIZE - sizeof(pointer));
   } /* Endif */

   return elem;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_free
* Returned Values : void
* Comments        :
*     Free a partition block.
*
*END*-----------------------------------------------------------------*/

void RTCS_part_free
   (
      pointer block              /* [IN] block to free */
   )
{ /* Body */
   pointer _PTR_  elem = block;
   _rtcs_part     part;

   part = *--elem;

   _int_disable();
   *elem = part->FREE;
   part->FREE = elem;
   _int_enable();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_part_destroy
* Returned Values : void
* Comments        :
*     Destroy a partition
*
*END*-----------------------------------------------------------------*/

void RTCS_part_destroy
   (
      _rtcs_part part            /* [IN] partition to destroy */
   )
{ /* Body */
   pointer _PTR_  elem;
   pointer _PTR_  nextelem;

   /* Destroy all the elements */
   if (part->DESTR) {
      for (elem = part->FREE; elem; elem = *elem) {
         part->DESTR(elem+1);
      } /* Endwhile */
   } /* Endif */

   /* Free all the grown blocks */
   for (elem = part->GROW; elem; elem = nextelem) {
      nextelem = *elem;
      _mem_free(elem);
   } /* Endfor */

   /* Free the partition and the initial blocks */
   _mem_free(part);

} /* Endbody */

#endif

/* EOF */
