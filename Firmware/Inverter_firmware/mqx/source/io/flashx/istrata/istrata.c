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
* $FileName: istrata.c$
* $Version : 3.8.14.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions to program Intel's StrataFlash 
*   devices
*   While this driver is generic to Intel's StrataFlash family of
*   products it has only been tested on the following parts:
*   28F128J3A
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "istrata.h"
#include "istrataprv.h"

#define LOCK    1
#define UNLOCK  0
#define BYTES_IN_WORD    2
#define BYTES_IN_DWORD   4

const FLASHX_DEVICE_IF_STRUCT _intel_strata_if = {
    _intel_strata_erase,
    _intel_strata_program,
    NULL,
    NULL,
    _intel_strata_init,
    NULL,
    _intel_strata_write_protect,
    _io_intel_strata_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs a sector of flash
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_program
   (  
      /* [IN] device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr,

      /* [IN] where to copy data from */
      char_ptr              from_ptr,
      
      /* [OUT} where to copy data to */
      char_ptr              to_ptr,

      /* [IN] the sector size to copy */
      _mem_size             sector_size
   )
{ /* Body */
   _mqx_uint   total_width;
   _mqx_uint   i = 0;
   boolean     program = FALSE;
   boolean     result;
   _mqx_uint   write_protect = TRUE;
   _mqx_uint_ptr write_protect_ptr = &write_protect;
   uint_32     temp_data = 0xFFFFFFFF;
   uint_8_ptr  temp_data_ptr;
   uint_8      byte_data_counter = 0;
   
   temp_data_ptr = (uint_8_ptr)&temp_data;
   
   total_width = dev_ptr->DEVICES * dev_ptr->WIDTH;

   /* First make sure we actually need to program the sector */
   if (total_width == 1) {
      uint_8_ptr  from8_ptr = (uint_8_ptr)((pointer)from_ptr);
      uint_8_ptr  to8_ptr   = (uint_8_ptr)((pointer)to_ptr);

      while( !program && (i < sector_size) ) {
         program = *from8_ptr++ != *to8_ptr++;
         i++;
      } /* Endwhile */

      i--;
   } else if (total_width == 2) {
      uint_16_ptr  from16_ptr = (uint_16_ptr)((pointer)from_ptr);
      uint_16_ptr  to16_ptr   = (uint_16_ptr)((pointer)to_ptr);

      while( !program && (i < sector_size) ) {
         program = *from16_ptr++ != *to16_ptr++;
         i += 2;
      } /* Endwhile */

      i -= 2;
   } else if (total_width == 4) {
      uint_32_ptr  from32_ptr = (uint_32_ptr)((pointer)from_ptr);
      uint_32_ptr  to32_ptr   = (uint_32_ptr)((pointer)to_ptr);

      while( !program && (i < sector_size) ) {
         program = *from32_ptr++ != *to32_ptr++;
         i += 4;
      } /* Endwhile */

      i -= 4;
   } else {
      /* Unsupported configuration */
      return FALSE;
   } /* Endif */

   if (!program) {
      return !program;
   } /* Endif */

   /* get the flash write protect status */
   _io_intel_strata_ioctl(dev_ptr, FLASH_IOCTL_GET_WRITE_PROTECT, write_protect_ptr); 
   /* if the flash is write protect, return */
   if(write_protect){
      return(FALSE);
   }

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_program_1byte(dev_ptr, from_ptr, to_ptr, sector_size, i);
         break;
      case 2:
         if((uint_32)to_ptr & 0x01){
            to_ptr -= 0x01;
            byte_data_counter = 0x01;
            *temp_data_ptr = *to_ptr;
         
            while(sector_size) {            
               while( byte_data_counter < BYTES_IN_WORD && sector_size ){
                  *(uchar_ptr)(temp_data_ptr+byte_data_counter) = *from_ptr++;
                  byte_data_counter++;
                  sector_size--;
               }
               byte_data_counter = 0;
               
               result = _intel_strata_program_2byte(dev_ptr, (uint_16_ptr)temp_data_ptr,
                  (uint_16_ptr)to_ptr, 2, i);
               if(FALSE == result)
                  return result;
               if(i >> 1)
                  i += 2;
               temp_data = 0xFFFFFFFF;
               to_ptr += 2;
            }
         }
         else if(sector_size & 0x01){
            sector_size++;
            result = _intel_strata_program_2byte(dev_ptr, (uint_16_ptr)from_ptr,
                  (uint_16_ptr)to_ptr, sector_size, i);
         }
         else
            result = _intel_strata_program_2byte(dev_ptr, (uint_16_ptr)from_ptr,
               (uint_16_ptr)to_ptr, sector_size, i);
         break;
      case 4:
         if((uint_32)to_ptr & 0x03){
            to_ptr -= (uint_32)to_ptr & 0x03;
            byte_data_counter = (uint_32)to_ptr & 0x03;
            *temp_data_ptr = *to_ptr;
         
            while(sector_size) {            
               while( byte_data_counter < BYTES_IN_DWORD && sector_size ){
                  *(uchar_ptr)(temp_data_ptr+byte_data_counter) = *from_ptr++;
                  byte_data_counter++;
                  sector_size--;
               }
               byte_data_counter = 0;
               
               result = _intel_strata_program_4byte(dev_ptr, (uint_32_ptr)temp_data_ptr,
                  (uint_32_ptr)to_ptr, 4, i);
               if(FALSE == result)
                  return result;
               if(i >> 1)
                  i += 4;
               temp_data = 0xFFFFFFFF;
               to_ptr += 4;
            }
         }
         else if(sector_size & 0x01){
            sector_size++;
            result = _intel_strata_program_4byte(dev_ptr, (uint_32_ptr)from_ptr,
                  (uint_32_ptr)to_ptr, sector_size, i);
         }
         else
            result = _intel_strata_program_4byte(dev_ptr, (uint_32_ptr)from_ptr,
               (uint_32_ptr)to_ptr, sector_size, i);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases a sector of flash
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_erase
   (  
      /* [IN] device info */
      IO_FLASHX_STRUCT_PTR  dev_ptr,

      /* [IN] the sector to erase */
      char_ptr             input_sect_ptr,
      
      /* [IN] the number of bytes to erase */
      _mem_size            bytes
   )
{ /* Body */
   _mqx_uint  total_width;
   _mqx_uint  i = 0;
   boolean    result = TRUE;
   boolean    erase = FALSE;
   MQX_TICK_STRUCT  tmp_ticks;
   _mqx_uint  write_protect = TRUE;
   _mqx_uint_ptr write_protect_ptr = &write_protect;

#if 0   
   if (dev_ptr->WRITE_PROTECT) {
      (*dev_ptr->WRITE_PROTECT)(dev_ptr, FALSE);
   }/* Endif */
#endif

   /* get the flash write protect status */
   _io_intel_strata_ioctl(dev_ptr, FLASH_IOCTL_GET_WRITE_PROTECT, write_protect_ptr); 
   /* if the flash is write protect, return */
   if(write_protect){
      return(FALSE);
   }

   total_width = dev_ptr->DEVICES * dev_ptr->WIDTH;

   /* First make sure we actually need to erase the sector */
   if (total_width == 1) {
      uint_8_ptr  to8_ptr   = (uint_8_ptr)((pointer)input_sect_ptr);

      while( !erase && (i < bytes) ) {
         erase = 0xff != *to8_ptr++;
         i++;
      } /* Endwhile */

   } else if (total_width == 2) {
      uint_16_ptr  to16_ptr   = (uint_16_ptr)((pointer)input_sect_ptr);

      while( !erase && (i < bytes) ) {
         erase = 0xffff != *to16_ptr++;
         i += 2;
      } /* Endwhile */

   } else if (total_width == 4) {
      uint_32_ptr  to32_ptr   = (uint_32_ptr)((pointer)input_sect_ptr);

      while( !erase && (i < bytes) ) {
         erase = 0xffffffff != *to32_ptr++;
         i += 4;
      } /* Endwhile */

   } else {
      /* Unsupported configuration */
      return FALSE;
   } /* Endif */

   if (!erase) {
      return !erase;
   } /* Endif */

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_erase_1byte(dev_ptr, input_sect_ptr, 
            bytes, &tmp_ticks);
         break;
      case 2:
         result = _intel_strata_erase_2byte(dev_ptr, (uint_16_ptr)input_sect_ptr,
            bytes, &tmp_ticks);
         break;
      case 4:
         result = _intel_strata_erase_4byte(dev_ptr, (uint_32_ptr)input_sect_ptr,
            bytes, &tmp_ticks);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_check_timeout
* Returned Value   : TRUE if timeout occurs
* Comments         : 
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_check_timeout
   (  
      /* [IN] Time operation started in ticks */
      MQX_TICK_STRUCT_PTR   start_tick_ptr,

      /* [IN] The number of ticks the operation is expected to take */
      _mqx_uint             period
   )
{ /* Body */
   MQX_TICK_STRUCT  end_ticks;
   MQX_TICK_STRUCT  current_ticks;
   _mqx_int         result;

   PSP_ADD_TICKS_TO_TICK_STRUCT(start_tick_ptr, period, &end_ticks);
   _time_get_elapsed_ticks(&current_ticks);

   result = PSP_CMP_TICKS(&current_ticks, &end_ticks);

   if (result >= 0) {
      return TRUE;
   } else {
      return FALSE;
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_test_lock_bits
* Returned Value   : TRUE if locked
* Comments         : 
*
*   This function test, if the flash is locked  
*END*----------------------------------------------------------------------*/
static boolean _intel_strata_test_lock_bits
    (
        /* [IN] device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr
    ) 
{
    uint_32 block_size;
    uint_32 num_blocks;
    volatile uchar _PTR_    dest_ptr;
    int i = 0;
    uchar   status;
    
    num_blocks = dev_ptr->HW_BLOCK->NUM_SECTORS; /* sector == block */
    block_size = dev_ptr->HW_BLOCK->SECTOR_SIZE;
    /* test all sectors for lock */ 
    for(i = 0; i < num_blocks; i++) {
        /* set block address */
        dest_ptr = (volatile uchar _PTR_)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
        /* return in status block-bit value 0- unlock, 1- locked */ 
        ISTRATA_READ_LOCK_STATUS(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_READ_ID_MODE_CMD_16X1, ISTRATA_STATUS_BUSY_8X1);
        /* is some sector locked? */
        if(status & 0x01){
            /* yes, some sector is locked */
            return TRUE;
        }
    }
    return FALSE;
}
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_write_protect
* Returned Value   : TRUE if successful
* Comments         : 
*    This function sets or clears the lock bits on all sectors
*    lock = 1 - device is to be write protect, 0 - allow writing device
* 
*END*----------------------------------------------------------------------*/
boolean _intel_strata_write_protect
    (
        /* [IN] the device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr,
      
      /* [IN]  */
      _mqx_uint             lock
    ) 
{
    boolean result = TRUE;
    
    /* set the lock bits */
    if(lock == LOCK){
        result = _intel_strata_set_lock_bits(dev_ptr);   
    }
    /* clear lock bits */   
    else{
        /* is the flash locked? */
        if(_intel_strata_test_lock_bits(dev_ptr)){
            /* unlock */
            result = _intel_strata_clear_lock_bits(dev_ptr);
        }
    }
    return(result);
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_clear_lock_bits
* Returned Value   : TRUE if successful
* Comments         : 
*    This function clears the lock bits on all sectors
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_clear_lock_bits
   (  
      /* [IN] the device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr
   )
{ /* Body */
   boolean          result;
   MQX_TICK_STRUCT  start_ticks;

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_clearlockbits_1byte(dev_ptr, &start_ticks);
         break;
      case 2:
         result = _intel_strata_clearlockbits_2byte(dev_ptr, &start_ticks);
         break;
      case 4:
         result = _intel_strata_clearlockbits_4byte(dev_ptr, &start_ticks);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_set_lock_bits
* Returned Value   : TRUE if successful
* Comments         : 
*    This function sets the lock bits on all sectors
* 
*END*----------------------------------------------------------------------*/

boolean _intel_strata_set_lock_bits
   (  
      /* [IN] the device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr
   )
{ /* Body */
   boolean          result;
   MQX_TICK_STRUCT  start_ticks;

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_setlockbits_1byte(dev_ptr, &start_ticks);
         break;
      case 2:
         result = _intel_strata_setlockbits_2byte(dev_ptr, &start_ticks);
         break;
      case 4:
         result = _intel_strata_setlockbits_4byte(dev_ptr, &start_ticks);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_intel_strata_ioctl
* Returned Value   : TRUE if successful
* Comments         : 
*    flash ioctl function
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_intel_strata_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_FLASHX_STRUCT_PTR   dev_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      pointer                param_ptr
   )
{ /* Body */
    _mqx_uint                       result = MQX_OK;
    switch(cmd) {
        case FLASH_IOCTL_GET_WRITE_PROTECT:
            /* is the flash locked ? */
            if (_intel_strata_test_lock_bits(dev_ptr)) {
                *((boolean _PTR_)param_ptr) = TRUE;
            } else {
                *((boolean _PTR_)param_ptr) = FALSE;
            }
            break;
        default:
            break;
    }
    return result;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_init
* Returned Value   : TRUE if successful
* Comments         : 
*    This function initializes intel strata flash
* 
*END*----------------------------------------------------------------------*/
boolean _intel_strata_init
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    if (_intel_strata_test_lock_bits(dev_ptr))
        return _intel_strata_clear_lock_bits(dev_ptr);

    return TRUE;
}

/* EOF */
