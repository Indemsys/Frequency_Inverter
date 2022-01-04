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
* $FileName: psp_mat1.c$
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
* Function Name    : _psp_mul_128_by_32
* Returned Value   : uint_32 - overflow
* Comments         :
*    This function multiplies a 128 bit quantity by a 32 bit quantity and 
* stores the result in a 128 bit quantity
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_mul_128_by_32
   (
      /* 
      ** [IN] Pointer to a 4 long word array in which the value to be multiplied
      ** is stored 
      */
      PSP_128_BIT_UNION_PTR   m_ptr,

      /* [IN] The multiplier */
      uint_32                 mul,

      /* [OUT] Pointer to a 4 long word array where the result will be stored */
      PSP_128_BIT_UNION_PTR   r_ptr

   )
{ /* Body */
   PSP_128_BIT_UNION tmp;
   uint_64           w,r;
   uint_32           w0;
   _mqx_uint         i;

#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   tmp.LLW[0] = 0;
   r = 0;
   if (!mul || (!m_ptr->LLW[0] && !m_ptr->LLW[1])) {
      tmp.LLW[1] = 0;
   } else if (mul == 1) {
      *r_ptr = *m_ptr;
      return r;
   } else {
      for ( i = 0; i < 3; i++ ) {
         w  = (uint_64)mul * (uint_64)m_ptr->LW[i];
         w0 = (uint_32)w;
         tmp.LW[i] += w0;
         tmp.LW[i+1] = (w >> 32) + (tmp.LW[i] < w0);
      } /* Endfor */

      w = (uint_64)mul * (uint_64)m_ptr->LW[3];
      w0 = (uint_32)w;
      tmp.LW[3] += w0;
      r = (w >> 32) + (tmp.LW[3] < w0);
   } /* Endif */
   
   *r_ptr = tmp;
   return r;
#else
   tmp.LLW[1] = 0;
   r = 0;
   if (!mul || (!m_ptr->LLW[0] && !m_ptr->LLW[1])) {
      tmp.LLW[0] = 0;
   } else if (mul == 1) {
      *r_ptr = *m_ptr;
      return r;
   } else {
      for ( i = 3; i > 0; i-- ) {
         w  = (uint_64)mul * (uint_64)m_ptr->LW[i];
         w0 = (uint_32)w;
         tmp.LW[i] += w0;
         tmp.LW[i-1] = (w >> 32) + (tmp.LW[i] < w0);
      } /* Endfor */

      w = (uint_64)mul * (uint_64)m_ptr->LW[0];
      w0 = (uint_32)w;
      tmp.LW[0] += w0;
      r = (w >> 32) + (tmp.LW[0] < w0);
   } /* Endif */
   
   *r_ptr = tmp;
   return r;
#endif

} /* Endbody */

/* EOF */
