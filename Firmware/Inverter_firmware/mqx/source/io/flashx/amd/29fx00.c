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
* $FileName: 29fx00.c$
* $Version : 3.8.4.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions to program any AMD 29FX00
*   flash device.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "29fx00.h"
#include "29fx00_prv.h"

const FLASHX_DEVICE_IF_STRUCT _29fx00_if = {
    _29fx00_sector_erase,
    _29fx00_program,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29fx00_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs the prom device listed below:
* 
*END*----------------------------------------------------------------------*/

boolean _29fx00_program
   (  
      /* [IN] the flash information */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] where to copy data from */
      char_ptr             from_ptr,
      
      /* [OUT} where to copy data to */
      char_ptr             to_ptr,

      /* [IN] the amount to program */
      _mem_size            bytes
   )
{ /* Body */
   boolean result = TRUE;
   uint_32 count = bytes;
   uint_32 timeout;
   uint_32 tmp1;
   uint_32 tmp2;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 32){
      volatile uint_32 _PTR_  base32_ptr;
      volatile uint_32 _PTR_  dest32_ptr;
      uint_32_ptr             src32_ptr;

      src32_ptr  = (uint_32_ptr)from_ptr;         
      dest32_ptr = (volatile uint_32 _PTR_)to_ptr;
      base32_ptr = (volatile uint_32 _PTR_)to_ptr;

      *base32_ptr = 0xFFFFFFFF; /* Set read mode */

      count = (count+3) & ~3;   /* Round count up, multiple of 4 */
      while (count > 0) {
         tmp1 = *src32_ptr++;
         if (tmp1 != 0xFFFFFFFF) {
            base32_ptr[0x0555] = 0xAAAAAAAA; 
            base32_ptr[0x02AA] = 0x55555555;
            base32_ptr[0x0555] = 0xA0A0A0A0;       /* Write command */
            dest32_ptr[0]      = tmp1;
            timeout = 100;
            while (timeout) {
               tmp2 = dest32_ptr[0];
               /* Check timeout or data bit of byte being programmed */
               if ((tmp2 & 0x80808080) != (tmp1 & 0x80808080)) {
                  /* Check for timeout error */
                  if ((tmp2 & 0x20202020) == 0x20202020) {
                     tmp2 = dest32_ptr[0];
                     if ((tmp2 & 0x80808080) != (tmp1 & 0x80808080)) {
                        result = FALSE;
                        *base32_ptr = 0xFFFFFFFF; /* Set read mode */
                        goto done;
                     } /* Endif */
                     break;
                  } /* Endif */
               } else  {
                  break;      /* done: byte programmed */
               } /* Endif */
               _io_flashx_wait_us(10);
               timeout--;
            } /* Endwhile */
            if (timeout==0) {
               result = FALSE;
               *base32_ptr = 0xFFFFFFFF; /* Set read mode */
               goto done;
            } /* Endif */
         } /* Endif */
         dest32_ptr++;
         count -= 4;
      } /* Endwhile */

      *base32_ptr = 0xFFFFFFFF; /* Set read mode */

   } else if (flashx_ptr->WIDTH == 16){
      volatile uint_16 _PTR_  base16_ptr;
      volatile uint_16 _PTR_  dest16_ptr;
      uint_16_ptr             src16_ptr;

      src16_ptr  = (uint_16_ptr)from_ptr;         
      dest16_ptr = (volatile uint_16 _PTR_)to_ptr;
      base16_ptr = (volatile uint_16 _PTR_)to_ptr;

      *base16_ptr = 0xFFFF; /* Set read mode */

      count = (count+1) & ~1;   /* Round count up, multiple of 2 */
      while (count > 0) {
         tmp1 = *src16_ptr++;
         if (tmp1 != 0xFFFF) {
            base16_ptr[0x0555] = 0xAAAA; 
            base16_ptr[0x02AA] = 0x5555;
            base16_ptr[0x0555] = 0xA0A0;       /* Write command */
            dest16_ptr[0]      = tmp1;
            timeout = 100;
            while (timeout) {
               tmp2 = dest16_ptr[0];
               /* Check timeout or data bit of byte being programmed */
               if ((tmp2 & 0x8080) != (tmp1 & 0x8080)) {
                  /* Check for timeout error */
                  if ((tmp2 & 0x2020) == 0x2020) {
                     tmp2 = dest16_ptr[0];
                     if ((tmp2 & 0x8080) != (tmp1 & 0x8080)) {
                        result = FALSE;
                        *base16_ptr = 0xFFFF; /* Set read mode */
                        goto done;  /*** */
                     } /* Endif */
                     break;
                  } /* Endif */
               } else  {
                  break;      /* done: byte programmed */
               } /* Endif */
               _io_flashx_wait_us(10);
               timeout--;
            } /* Endwhile */
            if (timeout==0) {
               result = FALSE;
               *base16_ptr = 0xFFFF; /* Set read mode */
               goto done;  /*** */
            } /* Endif */
         } /* Endif */
         dest16_ptr++;
         count -= 2;
      } /* Endwhile */

      *base16_ptr = 0xFFFF; /* Set read mode */

   } else if (flashx_ptr->WIDTH == 8){
      volatile uint_8  _PTR_  base8_ptr;
      volatile uint_8  _PTR_  dest8_ptr;
      uint_8_ptr              src8_ptr;

      src8_ptr  = (uint_8_ptr)from_ptr;         
      dest8_ptr = (volatile uint_8 _PTR_)to_ptr;
      base8_ptr = (volatile uint_8 _PTR_)to_ptr;

      *base8_ptr = 0xFF; /* Set read mode */

      while (count > 0) {
         tmp1 = *src8_ptr++;
         if (tmp1 != 0xFFFF) {
            base8_ptr[0x0555] = 0xAA; 
            base8_ptr[0x02AA] = 0x55;
            base8_ptr[0x0555] = 0xA0;       /* Write command */
            dest8_ptr[0]      = tmp1;
            timeout = 100;
            while (timeout) {
               tmp2 = dest8_ptr[0];
               /* Check timeout or data bit of byte being programmed */
               if ((tmp2 & 0x80) != (tmp1 & 0x80)) {
                  /* Check for timeout error */
                  if ((tmp2 & 0x20) == 0x20) {
                     tmp2 = dest8_ptr[0];
                     if ((tmp2 & 0x80) != (tmp1 & 0x80)) {
                        result = FALSE;
                        *base8_ptr = 0xFF; /* Set read mode */
                        goto done;  /****/
                     } /* Endif */
                     break;
                  } /* Endif */
               } else  {
                  break;      /* done: byte programmed */
               } /* Endif */
               _io_flashx_wait_us(10);
               timeout--;
            } /* Endwhile */
            if (timeout==0) {
               result = FALSE;
               *base8_ptr = 0xFF; /* Set read mode */
               goto done;  /****/
            } /* Endif */
         } /* Endif */
         dest8_ptr++;
         count -= 1;
      } /* Endwhile */

      *base8_ptr = 0xFF; /* Set read mode */

   }/* Endif */

   if (flashx_ptr->WRITE_VERIFY) {
      /* Verify sector(s) is(are) written correctly */
      uint_32_ptr  src32_ptr  = (uint_32_ptr)from_ptr;
      uint_32_ptr  dest32_ptr =  (uint_32_ptr)to_ptr;
      count = bytes/4 + 1;
      while (--count) {
         if (*src32_ptr++ != *dest32_ptr++) {
            result = FALSE;
            break;
         } /* Endif */
      } /* Endwhile */
   } /* Endif */   

done:
   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */
   return(result);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29fx00_sector_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases all 29fx00 devices.
* 
*END*----------------------------------------------------------------------*/

boolean _29fx00_sector_erase
   (
      /* [IN] the flash information */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] the sector to erase */
      char_ptr             input_sect_ptr,

      /* [IN] the number of bytes to erase */
      _mem_size            bytes
   )
{ /* Body */
   boolean result = TRUE;
   uint_32 timeout;
   uint_32 tmp1;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 32){
      volatile uint_32 _PTR_  base32_ptr;
   
      base32_ptr = (volatile uint_32 _PTR_)input_sect_ptr;

      /* Erase a sector */
      base32_ptr[0x0555] = 0xAAAAAAAA;
      base32_ptr[0x02AA] = 0x55555555;
      base32_ptr[0x0555] = 0x80808080; /* Erase command */
      base32_ptr[0x0555] = 0xAAAAAAAA;
      base32_ptr[0x02AA] = 0x55555555;

      base32_ptr[0]      = 0x30303030;

      timeout = 9000000;
      while (timeout) {
         tmp1 = base32_ptr[0];
         if ((tmp1 & 0x80808080) == 0x80808080) {
            break;
         } /* Endif */
         if ((tmp1 & 0x20202020) == 0x20202020) {
            tmp1 = base32_ptr[0];
            if ( (tmp1 & 0x80808080) != 0x80808080) {
               base32_ptr[0] = 0xFFFFFFFF; /* Set read mode */
               /* Reset FLASH */
               base32_ptr[0x555] = 0xF0F0F0F0;  /* Reset command */
               result = FALSE;
               break;
            } /* Endif */
            break;
         } /* Endif */
         _io_flashx_wait_us(10);
         timeout--;
      } /* Endwhile */
      tmp1 = base32_ptr[0];
      if (timeout == 0) {
         result = FALSE;
      } /* Endif */
      base32_ptr[0] = 0xFFFFFFFF; /* Set read mode */

   } else if (flashx_ptr->WIDTH == 16){
      volatile uint_16 _PTR_  base16_ptr;

      base16_ptr = (volatile uint_16 _PTR_)input_sect_ptr;

      /* Erase a sector */
      base16_ptr[0x0555] = 0xAAAA;
      base16_ptr[0x02AA] = 0x5555;
      base16_ptr[0x0555] = 0x8080; /* Erase command */
      base16_ptr[0x0555] = 0xAAAA;
      base16_ptr[0x02AA] = 0x5555;

      base16_ptr[0]      = 0x3030;

      timeout = 9000000;
      while (timeout) {
         tmp1 = base16_ptr[0];
         if ((tmp1 & 0x8080) == 0x8080) {
            break;
         } /* Endif */
         if ((tmp1 & 0x2020) == 0x2020) {
            tmp1 = base16_ptr[0];
            if ( (tmp1 & 0x8080) != 0x8080) {
               base16_ptr[0] = 0xFFFF; /* Set read mode */
               /* Reset FLASH */
               base16_ptr[0x555] = 0xF0F0;  /* Reset command */
               result = FALSE;
               break;
            } /* Endif */
            break;
         } /* Endif */
         _io_flashx_wait_us(10);
         timeout--;
      } /* Endwhile */
      tmp1 = base16_ptr[0];
      if (timeout == 0) {
         result = FALSE;
      } /* Endif */
      base16_ptr[0] = 0xFFFF; /* Set read mode */

   } else if (flashx_ptr->WIDTH == 8){
      volatile uint_8  _PTR_  base8_ptr;

      base8_ptr = (volatile uint_8 _PTR_)input_sect_ptr;
      /* Erase a sector */
      base8_ptr[0x0555] = 0xAA;
      base8_ptr[0x02AA] = 0x55;
      base8_ptr[0x0555] = 0x80; /* Erase command */
      base8_ptr[0x0555] = 0xAA;
      base8_ptr[0x02AA] = 0x55;

      base8_ptr[0]      = 0x30;

      timeout = 9000000;
      while (timeout) {
         tmp1 = base8_ptr[0];
         if ((tmp1 & 0x80) == 0x80) {
            break;
         } /* Endif */
         if ((tmp1 & 0x20) == 0x20) {
            tmp1 = base8_ptr[0];
            if ( (tmp1 & 0x80) != 0x80) {
               base8_ptr[0] = 0xFF; /* Set read mode */
               /* Reset FLASH */
               base8_ptr[0x555] = 0xF0;  /* Reset command */
               result = FALSE;
            } /* Endif */
            break;
         } /* Endif */
         _io_flashx_wait_us(10);
         timeout--;
      } /* Endwhile */
      tmp1 = base8_ptr[0];
      if (timeout == 0) {
         result = FALSE;
      } /* Endif */
      base8_ptr[0] = 0xFF; /* Set read mode */

   }/* Endif */

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */
   return(result);

} /* Endbody */

/* EOF */
