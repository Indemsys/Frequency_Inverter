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
* $FileName: 29wsxxxJ.c$
* $Version : 3.8.5.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions to program the Spansion
*   S29ws128N flash devices.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "29wsxxxJ.h"
#include "29wsxxxJ_prv.h"
                                                      
#define FLASH_READY 1
#define FLASH_ERASE_SUSPENDED 2
#define FLASH_TIMEOUT 3
#define FLASH_BUSY 4
#define FLASH_ERROR 5
#define FLASH_UNSUPPORTED_BUS_SIZE 6
#define FLASH_WRITE_PROTECT 7

#define FLASH_RESET_CMD 0xF0

#define DQ0 0x01
#define DQ1 0x02
#define DQ2 0x04
#define DQ3 0x08
#define DQ4 0x10
#define DQ5 0x20
#define DQ6 0x40
#define DQ7 0x80

#define  UNLOCKED   0
#define  LOCKED     1

#define WS_BYTES_IN_WORD 2

uint_32 _29wsxxxJ_test_erase_cmd(pointer, uint_16);
uint_32 _29wsxxxJ_test_program_cmd(pointer, uint_16);

const FLASHX_DEVICE_IF_STRUCT _flashx_29wsxxxJ_if = {
    _29wsxxxJ_erase,
    _29wsxxxJ_program,
    _29wsxxxJ_chip_erase,
    NULL,
    NULL,
    NULL,
    _29wsxxxJ_write_protect,
    _29wsxxxJ_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs the prom devices listed below:
*       29ws128N, 29ws256N and 29ws512N
* 
*END*----------------------------------------------------------------------*/

boolean _29wsxxxJ_program
   (  
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] where to copy data from */
      char_ptr             from_ptr,
      
      /* [OUT] where to copy data to */
      char_ptr             to_ptr,

      /* [IN] the size to copy in bytes */
      _mem_size            size
   )
{ /* Body */
   volatile uint_16 _PTR_  base16_ptr;
   uint_32                 result = FLASH_READY;
   
   uint_16                 temp_data = 0xFFFF;
   uchar_ptr               temp_data_ptr;
   uchar                   byte_data_counter = 0;
   /* get the offset in write word */
   uint_16 offset = (uint_16)to_ptr & 0x0001;

    /* test for flash write protect */
    if (_29wsxxxJ_test_lock_bits(flashx_ptr) == LOCKED) {
        return  FLASH_WRITE_PROTECT;     
    }/* Endif */

   if (flashx_ptr->WIDTH == 16) {

      (uint_16_ptr)temp_data_ptr =  &temp_data;
            
      /* if the start address !=  doesn't correspond with hardware, prepare
           variables for 1st word write */
      if(offset){
          /* Align pointer to writable address */
          to_ptr -= offset;
          /* jump over old data */
          byte_data_counter = offset;
      }
       
      /* set the flash base address */
      base16_ptr = (uint_16_ptr)flashx_ptr->BASE_ADDR; 

      /* we have data to write */
      while (size) {
         /* read old data to write word */
          temp_data = *(uint_16_ptr)to_ptr;
         /* move data to write word byte by byte */
         while( byte_data_counter < WS_BYTES_IN_WORD && size ){
            *(uchar_ptr)(temp_data_ptr+byte_data_counter) = *from_ptr++;
            byte_data_counter++;
            size--;
         }
        /* flash program */
        _int_disable();
        /* unlock flash */
        *( base16_ptr + 0x555 ) = 0x00AA;
        *( base16_ptr + 0x2AA ) = 0x0055;
        /* Write command */            
        *( base16_ptr + 0x555 ) = 0x00A0;
        /* write value */        
        *(uint_16 _PTR_)to_ptr = temp_data;     
        _int_enable();

        /* test if the write operation complete */
        result = _29wsxxxJ_test_program_cmd(to_ptr, temp_data);
        /* if write command failed, exit write command */ 
        if(result != FLASH_READY){
            break;
        }
         byte_data_counter = 0;
         temp_data = 0xFFFF;
         to_ptr += WS_BYTES_IN_WORD;
      }/* Endwhile */

   }
   else 
       result = FLASH_UNSUPPORTED_BUS_SIZE;

   return result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases (sector) the prom devices listed below:
*       29ws128N, 29ws256N and 29ws512N
* 
*END*----------------------------------------------------------------------*/

boolean _29wsxxxJ_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] the sector to erase */
      char           _PTR_ input_sect_ptr,

      /* [IN] the sector size */
      _mem_size            sector_size
   )
{ /* Body */
   volatile uint_16 _PTR_ sect16_ptr;
   uint_32                result = FLASH_READY;
   
   uint_32 tmp1;
   uint_32 time0;

   /* test for flash write protect */
    if (_29wsxxxJ_test_lock_bits(flashx_ptr) == LOCKED) {
        return  FLASH_WRITE_PROTECT;     
    }/* Endif */

   if (flashx_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint_16 _PTR_)input_sect_ptr;
      _int_disable();
      /* unlock flash */     
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr + 0x2AA ) = 0x0055;
      /* setup command */
      *( sect16_ptr + 0x555 ) = 0x0080;
      /* unlock */
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr +0x2AA ) = 0x0055;
      /* Sector erase */
      *( sect16_ptr )    = 0x0030;    
      _int_enable();

      /* erase check */ 
      tmp1 = *sect16_ptr; 
      /* wait, until DQ3 = 0 or tSEA=50us */  
      time0 = _time_get_microseconds(); 
      while(!(tmp1 & DQ3) && (_time_get_microseconds()-time0)<50){      
          tmp1 = *sect16_ptr;   
      }
      _io_flashx_wait_us(4);
      result = _29wsxxxJ_test_erase_cmd(input_sect_ptr, (uint_16)0xFFFF);  
   } 
   else {
      result = FLASH_UNSUPPORTED_BUS_SIZE;
   }

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_chip_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases all the prom devices. listed below:
*       29ws128N, 29ws256N and 29ws512N
* 
*END*----------------------------------------------------------------------*/

boolean _29wsxxxJ_chip_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr
   )
{ /* Body */
   volatile uint_16 _PTR_ sect16_ptr;
   uint_32                result  = FLASH_READY;

   /* test for flash write protect */
    if (_29wsxxxJ_test_lock_bits(flashx_ptr) == LOCKED) {
        return  FLASH_WRITE_PROTECT;     
    }/* Endif */

   if (flashx_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint_16 _PTR_)flashx_ptr->BASE_ADDR;
      _int_disable();
      /* unlock */
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr + 0x2AA ) = 0x0055;
      /* setup command */
      *( sect16_ptr + 0x555 ) = 0x0080;
      /* unlock */
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr + 0x2AA ) = 0x0055;
      /* chip erase */
      *( sect16_ptr + 0x555 ) = 0x0010;    
      _int_enable();
     
      /* chip erase test */
      result = _29wsxxxJ_test_erase_cmd((uchar_ptr)sect16_ptr, (uint_16)0xFFFF);     
   }
   else {
      result = FLASH_UNSUPPORTED_BUS_SIZE; 
   }

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : 29wsxxxJ_test_erase_cmd
* Returned Value   : uint_32 current status of operation
* Comments         : test, if the write flash command operation finished
*
*END*----------------------------------------------------------------------*/
uint_32 _29wsxxxJ_test_erase_cmd
   (
      /* [IN] address */
      pointer              to_ptr,
      /* written data */
      uint_16              write_data
   ) 
{
    uint_16     test_data, test_data1;
    uchar       error_count = 0;
    uint_32     result =  FLASH_BUSY;   

    while(result == FLASH_BUSY){
        /* Read 1 */
        test_data = *(uint_16 _PTR_)to_ptr;
        /* DQ7 contain valid data? */
        if((test_data & DQ7) == (write_data & DQ7)){
            /* Read 2 */
            test_data1 = *(uint_16 _PTR_)to_ptr;
            /* Read 3 */
            test_data = *(uint_16 _PTR_)to_ptr;
            /* DQ6 toggling, device error */
            if((test_data & DQ6) != (test_data1 & DQ6) ){
                /* software flash reset */
                *(uint_16 _PTR_)to_ptr = FLASH_RESET_CMD;
                result = FLASH_ERROR;
            }
            /* Erase operation complete */
            if((test_data & DQ2) == (test_data1 & DQ2)){
                result = FLASH_READY;
            }
        }
        /* test for operation internal timeout */
        else {
            /* read 1 DQ5 = 1? */
            if(test_data & DQ5){
                /* read 2 */
                test_data1 = *(uint_16 _PTR_)to_ptr;
                /* read 3 */
                test_data = *(uint_16 _PTR_)to_ptr;
                /* DQ6 togling? Timeout occured */  
                if(test_data1 & DQ6 != test_data & DQ6){
                    result = FLASH_TIMEOUT;
                    /* software flash reset */
                    *(uint_16 _PTR_)to_ptr = FLASH_RESET_CMD;
                }
            }
        }
    }
    
    return result == FLASH_READY;       
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : 29wsxxxJ_test_program_cmd
* Returned Value   : uint_32 current status of operation
* Comments         : test, if the write flash command operation finished
*
*END*----------------------------------------------------------------------*/
uint_32 _29wsxxxJ_test_program_cmd
   (
      /* [IN] address */
      pointer              to_ptr,
      /* written data */
      uint_16              write_data
   ) 
{
    uint_16     test_data, test_data1;
    uint_16     cycle_counter = 50;
    uchar       error_count = 0;
    uint_32     result =  FLASH_BUSY;   

    while(result == FLASH_BUSY){
        /* Read 1 */
        test_data = *(uint_16 _PTR_)to_ptr;
        /* DQ7 contain valid data? */
        if((test_data & DQ7) == (write_data & DQ7)){
            /* Read 2 */
            test_data1 = *(uint_16 _PTR_)to_ptr;
            /* Read 3 */
            test_data = *(uint_16 _PTR_)to_ptr;
            /* Read 3 valid data => Write operation complete */
            if(test_data == write_data){
                result = FLASH_READY;
            }
            /* Read 3 doesn't contain valid data, operation failed */
            else {                  
                if(error_count++>2){
                    result = FLASH_ERROR;
                    /* software flash reset */
                    *(uint_16 _PTR_)to_ptr = FLASH_RESET_CMD;
                }
            }
        }
        /* test for operation internal timeout */
        else {
            if(!(test_data & DQ5)){
                test_data1 = *(uint_16 _PTR_)to_ptr;
                test_data = *(uint_16 _PTR_)to_ptr;
                /* DQ6 togling? Timeout occured */  
                if(test_data1 & DQ6 != test_data & DQ6){
                    result = FLASH_TIMEOUT;
                    /* software flash reset */
                    *(uint_16 _PTR_)to_ptr = FLASH_RESET_CMD;
                }
            }
        }
        /* timeout test */
        if(!cycle_counter--){
            result = FLASH_TIMEOUT;
        }
    }
    
    return result;      
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_test_lock_bits
* Returned Value   : uint_32 current status of operation
* Comments         : test, if flash is write protect
*
*END*----------------------------------------------------------------------*/
static boolean _29wsxxxJ_test_lock_bits
    (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr
    ) 
{
    uint_16_ptr base_address_ptr = (uint_16_ptr)flashx_ptr->BASE_ADDR;
    uint_16_ptr sector_address_ptr; 
    int i;
    int sector_count;
    uint_16 data = 0;
        
    /* go over all blocks */
    for ( i = 0; flashx_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++ ) {
        /* set the start block address */
        sector_address_ptr = (uint_16_ptr)flashx_ptr->HW_BLOCK[i].START_ADDR; 
        /* go over all sectors in block */
        for ( sector_count = 0; sector_count < flashx_ptr->HW_BLOCK[i].NUM_SECTORS; sector_count ++){
            /* enter DYB mode */
            _int_disable();
            *( base_address_ptr + 0x555 ) = 0x00AA; /* write unlock cycle 1 */
            *( base_address_ptr + 0x2AA ) = 0x0055; /* write unlock cycle 2 */
            *( sector_address_ptr + 0x555 ) = 0x0058; /* write command entry */
            /* read sector lock bit */
            data = *sector_address_ptr;             
            /* exit DYB mode */
            *( base_address_ptr + 0x555 ) = 0x00AA; /* write unlock cycle 1 */
            *( base_address_ptr + 0x2AA ) = 0x0055; /* write unlock cycle 2 */
            *( sector_address_ptr + 0x555 ) = 0x0090; /* write command entry */
            *( sector_address_ptr) = 0x0;
            _int_enable();
            /* is sector locked? 1 == locked */
            if( (data & DQ0) ){ 
                /* yes - exit locked */
                return LOCKED;
            }               
            /* no - incrememnt sector address */
            (uchar_ptr)sector_address_ptr += (flashx_ptr->HW_BLOCK[i].SECTOR_SIZE); 
        }       
    }
    /* exit flash unlocked */
    return UNLOCKED;    
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_test_lock_bits
* Returned Value   : uint_32 current status of operation
* Comments         : test, if flash is write protect
*
*END*----------------------------------------------------------------------*/
static uint_32 _29wsxxxJ_check_ID
    (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr
    ) 
{
    uint_16_ptr base_address_ptr = (uint_16_ptr)flashx_ptr->BASE_ADDR;
    uint_16_ptr sector_address_ptr; 
    uint_16 manu_id;
    
    *(base_address_ptr + 0x555) = 0x00AA;/* write unlock cycle 1 */
    *(base_address_ptr + 0x2AA) = 0x0055;/* write unlock cycle 2 */
    *(base_address_ptr + 0x555) = 0x0090; /* write command entry */
    manu_id = *(base_address_ptr +1) ;
    
    *base_address_ptr = 0xf0;
    
    /* exit flash unlocked */
    return manu_id; 
}
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_write_protect
* Returned Value   : uint_32 current status of operation
* Comments         : lock/unlock flash for write
*
*END*----------------------------------------------------------------------*/
boolean _29wsxxxJ_write_protect
    (
        /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR  flashx_ptr,
      
      /* [IN]  */
      _mqx_uint             lock
    ) 
{
    volatile uint_16_ptr source_address_ptr;
    uint_16_ptr sector_address_ptr; 
    uint_16_ptr base_address_ptr = (uint_16_ptr)flashx_ptr->BASE_ADDR;
    int i;
    int sector_count;
    uint_16 command = 0, data;
        
    /* set the lock/unlock command */
    if (lock == 1 ){
        /* lock sector command */
        command = 1;
    } 
    else {
        /* unlock sector command */
        command = 0;
    }
    /* go over all blocks */
    for ( i = 0; flashx_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++ ) {
        /* set the start block address */
        sector_address_ptr = (uint_16_ptr)flashx_ptr->HW_BLOCK[i].START_ADDR; 
        /* go over all sectors in block */
        for ( sector_count = 0; sector_count < flashx_ptr->HW_BLOCK[i].NUM_SECTORS; sector_count ++){
            /* enter DYB mode */
            _int_disable(); 
            *( base_address_ptr + 0x555 ) = 0x00AA; /* write unlock cycle 1 */
            *( base_address_ptr + 0x2AA ) = 0x0055; /* write unlock cycle 2 */
            *( base_address_ptr + 0x555 ) = 0x0048; /* write command */
            *( sector_address_ptr ) = command; 
            data = *sector_address_ptr;
            _int_enable();                      
            /* increment sector */
            (uchar_ptr)sector_address_ptr += (flashx_ptr->HW_BLOCK[i].SECTOR_SIZE); 
        }       
    }
    /* reset device */
    * base_address_ptr = FLASH_RESET_CMD;
    
    return MQX_OK;  
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxJ_ioctl
* Returned Value   : TRUE if successful
* Comments         : 
*    flash ioctl function
* 
*END*----------------------------------------------------------------------*/
_mqx_int _29wsxxxJ_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_FLASHX_STRUCT_PTR handle_ptr,

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
            if (_29wsxxxJ_test_lock_bits(handle_ptr) ) {
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


/* EOF */
