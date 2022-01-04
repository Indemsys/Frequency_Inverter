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
* $FileName: psp_mat3.c$
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
* Function Name    : _psp_add_element_to_array
* Returned Value   : uint_32 - 1 if overflow, 0 otherwise
* Comments         :
*    This function adds a single element to an array. r = a[] + val
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_add_element_to_array
   (
      /* [IN] Pointer to the array to be added to */
      uint_32_ptr s1_ptr,

      /* [IN] The value to add the array */
      uint_32     val,

      /* [IN] The size of the array to add in long words */
      uint_32     size,

      /* [OUT] Pointer to where the result is to be stored */
      uint_32_ptr res_ptr

   )
{ /* Body */
   register uint_32 x, y, z, cy;
   register int_32  j;

#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   x  = s1_ptr[0];
   y  = x + val;
   cy = (y < val);
   res_ptr[0] = y;
   for ( j = 1; j < size; j++) {
      z = s1_ptr[j];
      x = z + cy;
      res_ptr[j] = x;
      cy = (x < z);
   } /* Endfor */
#else
   x  = s1_ptr[size-1];
   y  = x + val;
   cy = (y < val);
   res_ptr[size-1] = y;
   for ( j = (size-2); j >= 0; j--) {
      z = s1_ptr[j];
      x = z + cy;
      res_ptr[j] = x;
      cy = (x < z);
   } /* Endfor */
#endif
   return cy;

} /* Endbody */

/* EOF */
