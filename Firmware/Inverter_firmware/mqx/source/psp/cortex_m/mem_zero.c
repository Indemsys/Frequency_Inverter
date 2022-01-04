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
* $FileName: mem_zero.c$
* $Version : 3.8.4.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the functions for zeroing memory
*
*END************************************************************************/

#include "mqx_inc.h"


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _mem_zero
* Returned Value   : none
* Comments         :
*   This function zeros the specified number of bytes at the specified
* location.
*   The zeroing is optimized to avoid alignment problems, and attempts
* to zero 32bit numbers optimally
*
*END*----------------------------------------------------------------------*/

#if MQX_USE_MEM_ZERO

void _mem_zero
   (
      /* [IN] the address to start zeroing memory from */
      register pointer from_ptr,

      /* [IN] the number of bytes to zero */
      register _mem_size number_of_bytes
   )
{ /* Body */
#if MQX_USE_SMALL_MEM_ZERO
   register uint_8_ptr from8_ptr = (uint_8_ptr) from_ptr;

   if (number_of_bytes) {
      while (number_of_bytes--) {
         *from8_ptr++ = 0;
      } /* Endwhile */      
   } /* Endif */
#else
   uint_8_ptr from8_ptr = (uint_8_ptr) from_ptr;
   uint_16_ptr from16_ptr = (uint_16_ptr) from_ptr;
   register uint_32_ptr from32_ptr = (uint_32_ptr) from_ptr;
   register uint_32 loops;

   if (number_of_bytes > 3) {

      /* Try to align source on word */
      if ((uint_32)from_ptr & 1) {
         from8_ptr = (uint_8_ptr) from_ptr;
         *from8_ptr++ = 0;

         from_ptr = from8_ptr;
         --number_of_bytes;
      } /* Endif */

      /* Try to align source on longword */
      if ((uint_32)from_ptr & 2) {
         from16_ptr = (uint_16_ptr) from_ptr;

         *from16_ptr++ = 0;

         from_ptr = from16_ptr;
         number_of_bytes -= 2;
      } /* Endif */

      from32_ptr = (uint_32_ptr) from_ptr;
#if MQX_USE_BLOCK_MEM_ZERO
      /* 
      ** so lets copy longwords...
      ** to increase performance, we will do 64 bytes (16 * longwords) at once and after that jumping
      ** This consumes more RAM, more flash, but gets less instruction cycles.
      */
      for (loops = number_of_bytes >> 6; loops != 0; loops--) {
         /* copy 16 longwords */
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
         *from32_ptr++ = 0;
      } /* Endwhile */

      /* Now, write the rest of bytes */
      switch ((number_of_bytes >> 2) & 0xF) {
         case 15: *from32_ptr++ = 0;
         case 14: *from32_ptr++ = 0;
         case 13: *from32_ptr++ = 0;
         case 12: *from32_ptr++ = 0;
         case 11: *from32_ptr++ = 0;
         case 10: *from32_ptr++ = 0;
         case 9:  *from32_ptr++ = 0;
         case 8:  *from32_ptr++ = 0;
         case 7:  *from32_ptr++ = 0;
         case 6:  *from32_ptr++ = 0;
         case 5:  *from32_ptr++ = 0;
         case 4:  *from32_ptr++ = 0;
         case 3:  *from32_ptr++ = 0;
         case 2:  *from32_ptr++ = 0;
         case 1:  *from32_ptr++ = 0;
      } /* Endswitch */

#else /* MQX_USE_BLOCK_MEM_ZERO */
      for (loops = number_of_bytes >> 2; loops != 0; loops--) {
         *from32_ptr++ = 0;
      }
#endif /* MQX_USE_BLOCK_MEM_ZERO */
      from_ptr = from32_ptr;
   } /* Endif */

   /* Copy all remaining bytes */
   if (number_of_bytes & 2) {
      from16_ptr = (uint_16_ptr) from_ptr;

      *from16_ptr++ = 0;

      from_ptr = from16_ptr;
   } /* Endif */
   if (number_of_bytes & 1) {
      * (uint_8_ptr) from_ptr = 0;
   } /* Endif */
#endif
} /* Endbody */

#endif

/* EOF */

