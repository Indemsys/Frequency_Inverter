/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: mem_copy.c$
*
* Comments:
*
*   This file contains the functions for copying memory.
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _mem_copy
* Returned Value   : none
* Comments         :
*   This function copies the specified number of bytes from the
* source address to the destination address.  No attempt is made
* to handle overlapping copies to prevent loss of data.
*   The copying is optimized to avoid alignment problems, and attempts
* to copy 32bit numbers optimally
*
*END*----------------------------------------------------------------------*/

#if MQXCFG_MEM_COPY

void _mem_copy
   (
      /* [IN] address to copy data from */
      pointer  from_ptr,

      /* [IN] address to copy data to */
      pointer  to_ptr,

      /* [IN] number of bytes to copy */
      register _mem_size number_of_bytes
   )
{ /* Body */
#if MQX_USE_SMALL_MEM_COPY
   register uint_8_ptr from8_ptr = (uint_8_ptr) from_ptr;
   register uint_8_ptr to8_ptr = (uint_8_ptr) to_ptr;

   if (number_of_bytes) {
      while (number_of_bytes--) {
         *to8_ptr++ = *from8_ptr++;
      } /* Endwhile */      
   } /* Endif */
#else
   uint_8_ptr from8_ptr = (uint_8_ptr) from_ptr;
   uint_8_ptr to8_ptr = (uint_8_ptr) to_ptr;
   uint_16_ptr from16_ptr = (uint_16_ptr) from_ptr;
   uint_16_ptr to16_ptr = (uint_16_ptr) to_ptr;
   register uint_32_ptr from32_ptr = (uint_32_ptr) from_ptr;
   register uint_32_ptr to32_ptr = (uint_32_ptr) to_ptr;
   
   /* Target and source have the same offset of alignment address */
   if (number_of_bytes > 3 && (((uint_32)from_ptr & 0x03) == ((uint_32)to_ptr & 0x03))) {
      /* Try to align source on word */
      if ((uint_32)from_ptr & 1) {
         from8_ptr = (uint_8_ptr) from_ptr;
         to8_ptr = (uint_8_ptr) to_ptr;

         *to8_ptr++ = *from8_ptr++;

         from_ptr = from8_ptr;
         to_ptr = to8_ptr;
         --number_of_bytes;
      } /* Endif */

      /* Try to align source on longword */
      if ((uint_32)from_ptr & 2) {
         from16_ptr = (uint_16_ptr) from_ptr;
         to16_ptr = (uint_16_ptr) to_ptr;

         *to16_ptr++ = *from16_ptr++;

         from_ptr = from16_ptr;
         to_ptr = to16_ptr;
         number_of_bytes -= 2;
      } /* Endif */

      from32_ptr = (uint_32_ptr) from_ptr;
      to32_ptr = (uint_32_ptr) to_ptr;
#if MQX_USE_BLOCK_MEM_COPY
      /* 
      ** To increase performance a bit, we will copy 64 bytes (16 * longwords) sequentially
      ** This consumes more RAM, more flash, but gets less instruction cycles.
      */
      for (; number_of_bytes >= 0x40; number_of_bytes -= 0x40) {
         /* copy 16 longwords */
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
         *to32_ptr++ = *from32_ptr++;
      } /* Endwhile */
      
      /* Calculate number of remains 4B alignment bytes and write them */
      switch (number_of_bytes & 0x3c) {
         case (15 << 2): *to32_ptr++ = *from32_ptr++;
         case (14 << 2): *to32_ptr++ = *from32_ptr++;
         case (13 << 2): *to32_ptr++ = *from32_ptr++;
         case (12 << 2): *to32_ptr++ = *from32_ptr++;
         case (11 << 2): *to32_ptr++ = *from32_ptr++;
         case (10 << 2): *to32_ptr++ = *from32_ptr++;
         case (9  << 2): *to32_ptr++ = *from32_ptr++;
         case (8  << 2): *to32_ptr++ = *from32_ptr++;
         case (7  << 2): *to32_ptr++ = *from32_ptr++;
         case (6  << 2): *to32_ptr++ = *from32_ptr++;
         case (5  << 2): *to32_ptr++ = *from32_ptr++;
         case (4  << 2): *to32_ptr++ = *from32_ptr++;
         case (3  << 2): *to32_ptr++ = *from32_ptr++;
         case (2  << 2): *to32_ptr++ = *from32_ptr++;
         case (1  << 2): *to32_ptr++ = *from32_ptr++;
      } /* Endswitch */
      
      number_of_bytes = number_of_bytes & (0x03); /* Now get number of unaligned bytes */
#else /* MQX_USE_BLOCK_MEM_COPY */
      for (; number_of_bytes >= 0x4; number_of_bytes -= 0x4) {
         *to32_ptr++ = *from32_ptr++;
      }
#endif /* MQX_USE_BLOCK_MEM_COPY */
      from_ptr = from32_ptr;
      to_ptr = to32_ptr;
   } /* Endif */
   
   /* copy remains */
   if (number_of_bytes) {
      from8_ptr = (uint_8_ptr)from_ptr;
      to8_ptr = (uint_8_ptr)to_ptr;
      while (number_of_bytes--) {
         *to8_ptr++ = *from8_ptr++;
      }
   }
#endif
} /* Endbody */

#endif

/* EOF */
