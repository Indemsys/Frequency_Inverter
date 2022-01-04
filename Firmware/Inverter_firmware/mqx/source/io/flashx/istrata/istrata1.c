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
* $FileName: istrata1.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions to program Intel's StrataFlash 
*   devices
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "istrata.h"
#include "istrataprv.h"

                                          
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_program_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs a sector of flash for 8 bit devices
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_program_1byte
   (  
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR  handle_ptr,

      /* [IN] where to copy data from */
      char_ptr              from_ptr,
      
      /* [OUT} where to copy data to */
      char_ptr              to_ptr,

      /* [IN] the sector size to copy */
      _mem_size             sector_size,

      /* [IN] the offset to start at */
      _mem_size             offset
   )
{ /* Body */
   boolean                 erase;
   boolean                 success;
   boolean                 timeout;
   _mqx_uint               j;
   MQX_TICK_STRUCT         start_ticks;

   /* First make sure we actually need to program the sector */
   erase = FALSE;

   for (j = offset; !erase && (j < sector_size); j++) {
      erase = ISTRATA_ERASE_UNIT(to_ptr[j], from_ptr[j]);
   } /* Endfor */

   if (erase) {
      success = _intel_strata_erase_1byte(handle_ptr, to_ptr, 
         sector_size, &start_ticks);
      if (!success) {
         return success;
      } /* Endif */
      /* 
      ** We erased the whole sector so we must program from the beginning of
      ** the sector
      */
      offset = 0;
   } else {
      success = TRUE;
   } /* Endif */

   if (handle_ptr->DEVICES == 1) {
      volatile uchar _PTR_  dest_ptr = (volatile uchar _PTR_)to_ptr;
      uchar _PTR_           src_ptr  = (uchar _PTR_)from_ptr;
      _mem_size             size = sector_size;
      uchar                 tmp, status;

      dest_ptr = dest_ptr + offset;
      src_ptr  = src_ptr  + offset;

      ISTRATA_PROGRAM(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_WRITE_CMD_8X1, 
         ISTRATA_STATUS_BUSY_8X1);
   } else if (handle_ptr->DEVICES == 2) {
      volatile uint_16 _PTR_  dest_ptr = (volatile uint_16 _PTR_)((pointer)to_ptr);
      uint_16 _PTR_           src_ptr  = (uint_16 _PTR_)((pointer)from_ptr);
      _mem_size               size;
      uint_16                 tmp, status;

      size = sector_size >> 1;
      offset >>= 1;
      dest_ptr = dest_ptr + offset;
      src_ptr  = src_ptr  + offset;

      ISTRATA_PROGRAM(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_WRITE_CMD_8X2, 
         ISTRATA_STATUS_BUSY_8X2);
   } else if (handle_ptr->DEVICES == 4) {
      volatile uint_32 _PTR_  dest_ptr = (volatile uint_32 _PTR_)((pointer)to_ptr);
      uint_32 _PTR_           src_ptr  = (uint_32 _PTR_)((pointer)from_ptr);
      _mem_size               size;
      uint_32                 tmp, status;

      size = sector_size >> 2;
      offset >>= 2;
      dest_ptr = dest_ptr + offset;
      src_ptr  = src_ptr  + offset;

      ISTRATA_PROGRAM(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_WRITE_CMD_8X4, 
         ISTRATA_STATUS_BUSY_8X4);
   } else {
      /* Unsupported configuration */
      return FALSE;
   } /* Endif */

   if (success) {
      /* Verify sector is written correctly */
      for( j = 0; j < sector_size; j++ ) {
         if ( from_ptr[j] != to_ptr[j] ) {
            success = FALSE;
            break;
         } /* Endif */
      } /* Endfor */
   } /* Endif */

   return success;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_erase_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases a sector
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_erase_1byte
   (
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR handle_ptr,

      /* [IN] the sector to erase */
      char_ptr             input_sect_ptr,

      /* [IN] the size of the sector */
      _mem_size            sect_size,

      /* [IN/OUT] place to store start time */
      MQX_TICK_STRUCT_PTR  start_ticks_ptr
   )
{ /* Body */
   boolean timeout = FALSE;
   boolean success = TRUE;

   if (handle_ptr->DEVICES == 1) {
      volatile uchar _PTR_    dest_ptr = (volatile uchar _PTR_)input_sect_ptr;
      uchar                   status;

      ISTRATA_ERASE(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_ERASE_CMD1_8X1, 
         ISTRATA_ERASE_CMD2_8X1, ISTRATA_STATUS_BUSY_8X1, 0xFF);

   } else if (handle_ptr->DEVICES == 2) {
      volatile uint_16 _PTR_  dest_ptr = (volatile uint_16 _PTR_)input_sect_ptr;
      uint_16                 status;

      ISTRATA_ERASE(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_ERASE_CMD1_8X2, 
         ISTRATA_ERASE_CMD2_8X2, ISTRATA_STATUS_BUSY_8X2, 0xFFFF);

   } else if (handle_ptr->DEVICES == 4) {
      volatile uint_32 _PTR_  dest_ptr = (volatile uint_32 _PTR_)input_sect_ptr;
      uint_32                 status;

      ISTRATA_ERASE(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_ERASE_CMD1_8X4, 
         ISTRATA_ERASE_CMD2_8X4, ISTRATA_STATUS_BUSY_8X4, 0xFFFFFFFF);

   } else {
      success = FALSE;
   } /* Endif */

   return success;

} /* Endbody */


/* Start CR 871 */
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_clearlockbits_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function unlocks all sectors
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_clearlockbits_1byte
   (
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR handle_ptr,

      /* [IN/OUT] place to store start time */
      MQX_TICK_STRUCT_PTR  start_ticks_ptr
   )
{ /* Body */
   uint_32 block_size;
   uint_32 num_blocks;
   uint_32 i;
   boolean timeout = FALSE;
   boolean success = TRUE;

   num_blocks = handle_ptr->HW_BLOCK->NUM_SECTORS; /* sector == block */
   block_size = handle_ptr->HW_BLOCK->SECTOR_SIZE;

   /*
   ** All blocks default to the locked state after initial power-up or reset.
   ** The blocks are unlocked individually.
   */
   if (handle_ptr->DEVICES == 1) {
      volatile uchar _PTR_    dest_ptr;
      uchar                   status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uchar _PTR_)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_CLEAR_LOCKBITS(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_CLEAR_CMD1_8X1, 
            ISTRATA_CLEAR_CMD2_8X1, ISTRATA_STATUS_BUSY_8X1);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 2) {
      volatile uint_16 _PTR_  dest_ptr;
      uint_16                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint_16 _PTR_)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_CLEAR_LOCKBITS(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_CLEAR_CMD1_8X2, 
            ISTRATA_CLEAR_CMD2_8X2, ISTRATA_STATUS_BUSY_8X2);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 4) {
      volatile uint_32 _PTR_  dest_ptr;
      uint_32                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint_32 _PTR_)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_CLEAR_LOCKBITS(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_CLEAR_CMD1_8X4, 
            ISTRATA_CLEAR_CMD2_8X4, ISTRATA_STATUS_BUSY_8X4);
      } /* Endfor */
      
   } else {
      success = FALSE;
   } /* Endif */

   return success;

} /* Endbody */
/* End CR 871 */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_setlockbits_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function locks all sectors
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_setlockbits_1byte
   (
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR handle_ptr,

      /* [IN/OUT] place to store start time */
      MQX_TICK_STRUCT_PTR  start_ticks_ptr
   )
{ /* Body */
   uint_32 block_size;
   uint_32 num_blocks;
   uint_32 i;
   boolean timeout = FALSE;
   boolean success = TRUE;

   num_blocks = handle_ptr->HW_BLOCK->NUM_SECTORS; /* sector == block */
   block_size = handle_ptr->HW_BLOCK->SECTOR_SIZE;

   /*
   ** The blocks are locked individually.
   */
   if (handle_ptr->DEVICES == 1) {
      volatile uchar _PTR_    dest_ptr;
      uchar                   status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uchar _PTR_)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_SET_LOCKBITS(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_SET_CMD1_8X1, 
            ISTRATA_SET_CMD2_8X1, ISTRATA_STATUS_BUSY_8X1);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 2) {
      volatile uint_16 _PTR_  dest_ptr;
      uint_16                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint_16 _PTR_)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_SET_LOCKBITS(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_SET_CMD1_8X2, 
            ISTRATA_SET_CMD2_8X2, ISTRATA_STATUS_BUSY_8X2);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 4) {
      volatile uint_32 _PTR_  dest_ptr;
      uint_32                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint_32 _PTR_)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_SET_LOCKBITS(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_SET_CMD1_8X4, 
            ISTRATA_SET_CMD2_8X4, ISTRATA_STATUS_BUSY_8X4);
      } /* Endfor */
      
   } else {
      success = FALSE;
   } /* Endif */

   return success;

} /* Endbody */

/* EOF */
