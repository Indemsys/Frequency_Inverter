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
* $FileName: psp_tisu.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the function for subtracting two tick structs
*
*END************************************************************************/

#include "mqx_inc.h"
#if MQX_HAS_TICK

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_subtract_ticks
 * Returned Value   : 
 * Comments         : r = a - b
 *
 *END*----------------------------------------------------------------------*/

void _psp_subtract_ticks
   (
       /* [IN] The two structures to subtract - both must be normalized */
       PSP_TICK_STRUCT_PTR a_ptr,
       PSP_TICK_STRUCT_PTR b_ptr,

       /* [OUT] The result of the subtraction */
       PSP_TICK_STRUCT_PTR r_ptr
   )
{ /* Body */
   register uint_32       a_hw_ticks;
   register uint_32       b_hw_ticks;

   r_ptr->TICKS[0] = a_ptr->TICKS[0] - b_ptr->TICKS[0];

   a_hw_ticks  = a_ptr->HW_TICKS[0];
   b_hw_ticks  = b_ptr->HW_TICKS[0];

   if ( a_hw_ticks < b_hw_ticks) {
      a_hw_ticks += _mqx_kernel_data->HW_TICKS_PER_TICK;
      r_ptr->TICKS[0]--;
   } /* Endif */

   r_ptr->HW_TICKS[0] = a_hw_ticks - b_hw_ticks;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_subtract_ticks_int32
 * Returned Value   : 
 * Comments         : r = a - b, clamp into range <-(MAX_INT_32 + 1), MAX_INT_32>
 *
 *END*----------------------------------------------------------------------*/

int_32 _psp_subtract_ticks_int32
   (
       /* [IN] The two structures to subtract - both must be normalized */
       PSP_TICK_STRUCT_PTR a_ptr,
       PSP_TICK_STRUCT_PTR b_ptr,

       /* [OUT] The result of the subtraction */
       boolean _PTR_       o_ptr
   )
{ /* Body */
   register uint_32       a_hw_ticks;
   register uint_32       b_hw_ticks;
   PSP_TICK_STRUCT        r;

   r.TICKS[0] = a_ptr->TICKS[0] - b_ptr->TICKS[0];

   a_hw_ticks  = a_ptr->HW_TICKS[0];
   b_hw_ticks  = b_ptr->HW_TICKS[0];

   if ( a_hw_ticks < b_hw_ticks) 
   {
      a_hw_ticks += _mqx_kernel_data->HW_TICKS_PER_TICK;
      r.TICKS[0]--;
   } /* Endif */

   /* exchange sign bits between the 32bit halves of 64bit signed difference */
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   b_hw_ticks = ((uint_32_ptr)r.TICKS)[1];
#else
   b_hw_ticks = ((uint_32_ptr)r.TICKS)[0];
#endif
   a_hw_ticks = (b_hw_ticks & (MAX_UINT_32 ^ MAX_INT_32)) | (((uint_32)(r.TICKS[0])) & MAX_INT_32);
   b_hw_ticks = (((uint_32)(r.TICKS[0])) & (MAX_UINT_32 ^ MAX_INT_32)) | (b_hw_ticks & MAX_INT_32);
   if (o_ptr != NULL) *o_ptr = FALSE;
   
   /* a_hw_ticks contains 32bit signed result, b_hw_ticks must contain all zeroes or all ones according to sign */
   if ((a_hw_ticks >> 31) * MAX_UINT_32 != b_hw_ticks)
   {
      a_hw_ticks = MAX_INT_32 + (a_hw_ticks >> 31);
      if (o_ptr != NULL) *o_ptr = TRUE;
   }

   return (int_32)a_hw_ticks;

} /* Endbody */
#endif

/* EOF */
