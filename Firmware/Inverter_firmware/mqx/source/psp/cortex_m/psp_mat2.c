/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor
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
* $FileName: psp_mat2.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the math functions.
*
*END************************************************************************/

#include "mqx_inc.h"


/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_div_128_by_32
* Returned Value   : Returns the remainder from the calculation
* Comments         : This function divides a 4 * 32 bit quantity by a
*    32 bit quantity.
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_div_128_by_32
   (
      /* [IN] Pointer to the numerator array (4 long words) */
      PSP_128_BIT_UNION_PTR  n_ptr,

      /* [IN] The divisor */
      uint_32                div,

      /* [OUT] Pointer to a 4 long word array where the result will be stored */
      PSP_128_BIT_UNION_PTR  r_ptr

   )
{ /* Body */
   PSP_64_BIT_UNION      d;
   PSP_128_BIT_UNION     tmp;
   uint_64               w, r;
   _mqx_int              i;

   if (!div) {
      return MAX_UINT_32;
   } /* Endif */
   
   if (div == 1) {
      *r_ptr = *n_ptr;
      return 0;
   } /* Endif */

   if (n_ptr->LLW[0] || n_ptr->LLW[1]) {
      tmp.LLW[0] = tmp.LLW[1] = 0;
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
      for ( i = 2, r = n_ptr->LW[3]; i >= 0; i-- ) {
         w     = (r << 32) + n_ptr->LW[i];
         r     = w % div;
         d.LLW = w / div;
         tmp.LW[i+1] += d.LW[1];
         tmp.LW[i]   += d.LW[0];
      } /* Endfor */
#else
      for ( i = 1, r = n_ptr->LW[0]; i <= 3; i++ ) {
         w     = (r << 32) + n_ptr->LW[i];
         r     = w % div;
         d.LLW = w / div;
         tmp.LW[i-1] += d.LW[0];
         tmp.LW[i]   += d.LW[1];
      } /* Endfor */
#endif
      *r_ptr = tmp;
      return (uint_32)r;
   } /* Endif */

   *r_ptr = *n_ptr;

   return 0;

} /* Endbody */


/* EOF */
