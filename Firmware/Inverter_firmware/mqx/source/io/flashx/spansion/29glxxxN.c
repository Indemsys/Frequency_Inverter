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
* $FileName: 29glxxxN.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions to program the Spansion
*   S29GL128N flash devices.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "29glxxxN.h"
#include "29glxxxN_prv.h"
                                                      
#define FLASH_READY 1
#define FLASH_ERASE_SUSPENDED 2
#define FLASH_TIMEOUT 3
#define FLASH_BUSY 4
#define FLASH_ERROR 5

extern uint_32 _29glxxxN_status(IO_FLASHX_STRUCT_PTR, pointer);


const FLASHX_DEVICE_IF_STRUCT _flashx_29glxxxN_if = {
    _29glxxxN_erase,
    _29glxxxN_program,
    _29glxxxN_chip_erase,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs the prom devices listed below:
*       29GL128N, 29GL256N and 29GL512N
* 
*END*----------------------------------------------------------------------*/

boolean _29glxxxN_program
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
   volatile uint_16 _PTR_  dest16_ptr;
   uint_16_ptr             src16_ptr;
   volatile uchar _PTR_    base8_ptr;
   volatile uchar _PTR_    dest8_ptr;
   uchar_ptr               src8_ptr;
   uint_32                 tmp1;
   uint_32                 result = FLASH_READY;
   _mem_size               sector_addr;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   _io_flashx_phys_to_sector(flashx_ptr, (_mem_size)to_ptr, NULL, &sector_addr, NULL);

   if (flashx_ptr->WIDTH == 16) {
      src16_ptr  = (uint_16_ptr)from_ptr;         
      dest16_ptr = (volatile uint_16 _PTR_)to_ptr;
      base16_ptr = (volatile uint_16 _PTR_)sector_addr;
      while (size) {
         tmp1 = (uint_32)*src16_ptr++;
         if (tmp1 != 0xFFFF) {
            _int_disable();
            base16_ptr[0x555] = 0xAAAA;
            base16_ptr[0x2AA] = 0x5555;
            base16_ptr[0x555] = 0xA0A0;        /* Write command */            
            *dest16_ptr       = (uint_16)tmp1; /* write value */
           _int_enable();
            while ((result = _29glxxxN_status(flashx_ptr,
               (pointer)dest16_ptr)) == FLASH_BUSY)
            {
            }/* Endwhile */
            if (result != FLASH_READY) {
               break;
            } /* Endif */
         }/* Endif */
         size -= 2;
         dest16_ptr++;
      }/* Endwhile */
      base16_ptr[0] = 0xF0F0;

   } else if (flashx_ptr->WIDTH == 8) {
      src8_ptr  = (uchar_ptr)from_ptr;         
      dest8_ptr = (volatile uchar _PTR_)to_ptr;
      base8_ptr = (volatile uchar _PTR_)sector_addr;
      while (size) {
         tmp1 = (uint_32)*src8_ptr++;
         if (tmp1 != 0xFF) {
            _int_disable();
            base8_ptr[0xAAA] = 0xAA;
            base8_ptr[0x555] = 0x55;
            base8_ptr[0xAAA] = 0xA0;        /* Write command */           
            *dest8_ptr       = (uchar)tmp1; /* write value */
           _int_enable();
            while ((result = _29glxxxN_status(flashx_ptr,
               (pointer)dest8_ptr)) == FLASH_BUSY)
            {
            }/* Endwhile */
            if (result != FLASH_READY) {
               break;
            } /* Endif */
         }/* Endif */
         size -= 1;
         dest8_ptr++;
      }/* Endwhile */
      base8_ptr[0] = 0xF0;

   } /* Endif */
   
   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */

   return result == FLASH_READY;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases (sector) the prom devices listed below:
*       29GL128N, 29GL256N and 29GL512N
* 
*END*----------------------------------------------------------------------*/

boolean _29glxxxN_erase
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
   volatile uchar _PTR_   sect8_ptr;
   uint_32                result;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint_16 _PTR_)input_sect_ptr;
      _int_disable();     
      sect16_ptr[0x555] = 0xAAAA;
      sect16_ptr[0x2AA] = 0x5555;
      sect16_ptr[0x555] = 0x8080;
      sect16_ptr[0x555] = 0xAAAA;
      sect16_ptr[0x2AA] = 0x5555;
      sect16_ptr[0]     = 0x3030;    /* Sector erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (pointer)sect16_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect16_ptr[0] = 0xF0F0;

   } else if (flashx_ptr->WIDTH == 8) {
      sect8_ptr = (volatile uchar _PTR_)input_sect_ptr;
      _int_disable();
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0xAAA] = 0x80;
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0]     = 0x30;    /* Sector erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (pointer)sect8_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect8_ptr[0] = 0xF0;

   }/* Endif */

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_chip_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases all the prom devices. listed below:
*       29GL128N, 29GL256N and 29GL512N
* 
*END*----------------------------------------------------------------------*/

boolean _29glxxxN_chip_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr
   )
{ /* Body */
   volatile uint_16 _PTR_ sect16_ptr;
   volatile uchar _PTR_   sect8_ptr;
   uint_32                result;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint_16 _PTR_)flashx_ptr->BASE_ADDR;
      _int_disable();
      sect16_ptr[0xAAA] = 0xAAAA;
      sect16_ptr[0x555] = 0x5555;
      sect16_ptr[0xAAA] = 0x8080;
      sect16_ptr[0xAAA] = 0xAAAA;
      sect16_ptr[0x555] = 0x5555;
      sect16_ptr[0xAAA] = 0x1010;    /* chip erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (pointer)sect16_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect16_ptr[0] = 0xF0F0;

   } else if (flashx_ptr->WIDTH == 8) {
      sect8_ptr = (volatile uchar _PTR_)flashx_ptr->BASE_ADDR;
      _int_disable();
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0xAAA] = 0x80;
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0xAAA] = 0x10;    /* chip erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (pointer)sect8_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect8_ptr[0] = 0xF0;

   }/* Endif */

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_status
* Returned Value   : uint_32 current status of operation
* Comments         : 
*
*END*----------------------------------------------------------------------*/

uint_32 _29glxxxN_status
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr,
      
      /* [IN] address */
      pointer              addr
   )
{ /* Body */
   volatile uint_16 _PTR_ sect16_ptr;
   volatile uchar _PTR_   sect8_ptr;
   uint_32                tmp1;
   uint_32                tmp2;
   uint_32                toggle;
   uint_32                retry = 2;
   
   sect16_ptr = (volatile uint_16 _PTR_)addr;
   sect8_ptr  = (volatile uchar _PTR_)addr;

   while (retry--) {
      if (flashx_ptr->WIDTH == 16) {
         tmp1 = *sect16_ptr;
         tmp2 = *sect16_ptr;
         toggle = tmp1 ^ tmp2;   /* See what bits changed */
      } else if (flashx_ptr->WIDTH == 8) {
         tmp1 = *sect8_ptr;
         tmp2 = *sect8_ptr;
         toggle = tmp1 ^ tmp2;   /* See what bits changed */
      }/* Endif */
   
      if (toggle == 0) {
        return FLASH_READY;
      } /* Endif */

      if (toggle & 0x40) {
         if (tmp1 & 0x20) {
            if (retry) {
               continue;
            }/* Endif */
            return FLASH_TIMEOUT;
         }/* Endif */
         return FLASH_BUSY;
      } else {
         if ((toggle & 0x04) && (tmp1 & 0x80)) {
            if (retry) {
               continue;
            }/* Endif */
            return FLASH_ERASE_SUSPENDED;
         } /* Endif */
      }/* Endif */

   } /* Endwhile */

   return FLASH_ERROR;
   
} /* Endbody */
      
/* EOF */
