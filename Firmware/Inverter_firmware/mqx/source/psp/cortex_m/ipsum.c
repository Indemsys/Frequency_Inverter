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
* $FileName: ipsum.c$
* $Version : 3.7.4.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the implementation for a one's complement checksum.
*   The function can handle any alignment of the memory area 
*   being checksum'd.
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : _mem_sum_ip
* Returned Value  : one's complement checksum
* Comments        :
*     Returns a one's complement checksum over a block of memory, as
*     used for packets in Internet protocols (see RFC791 for definition).
*
*     Note:  This function returns 0 iff all summands are 0.
*
*END*-----------------------------------------------------------------*/

uint_32 _mem_sum_ip
   (
      register uint_32   sum,
      register uint_32   length,
      register pointer   loc
   )
{ /* Body */
   register uint_32   total = sum;
   register uint_32   temp;
   register uchar_ptr buf = (uchar_ptr)loc;

   if (length & 1) {
      length--;
      total += buf[length] << 8;
   } /* Endif */

   length >>= 1;
   while (length--) {
      temp   = (uint_16)(*buf);
      total += temp<<8 | buf[1];
      buf   += 2;
   } /* Endwhile */

   sum = ((total >> 16) & 0xFFFF);
   if (sum) {
      total = (total & 0xFFFF) + sum;
      sum = ((total >> 16) & 0xFFFF);
      if (sum) {
         total = (total & 0xFFFF) + sum;
      } /* Endif */
   } /* Endif */

   return total;

}
