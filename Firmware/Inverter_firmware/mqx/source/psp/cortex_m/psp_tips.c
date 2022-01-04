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
* $FileName: psp_tips.c$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*   This file contains the functions for converting ticks to picoseconds
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_ticks_to_picoseconds
* Returned Value   : uint_32 - number of picoseconds
* Comments         :
*    This function converts ticks into picoseconds, with rounding or truncating
*
*END*----------------------------------------------------------------------*/

static uint_32 __psp_ticks_to_picoseconds
   (
      /* [IN] Ticks to be converted */
      PSP_TICK_STRUCT_PTR tick_ptr,

      /* [OUT] pointer to where the overflow boolean is to be written */
      boolean _PTR_       overflow_ptr,
      
      /* [IN] boolean to control rounding or truncating */
      boolean             round
   )
{ /* Body */
   PSP_128_BIT_UNION      tmp;
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

#if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   tmp.LLW[1] = tick_ptr->TICKS[0];
   tmp.LLW[0] = 0;
#else
   tmp.LLW[0] = tick_ptr->TICKS[0];
   tmp.LLW[1] = 0;
#endif

   /* Convert ticks to hardware ticks */
   _psp_mul_128_by_32(&tmp, kernel_data->HW_TICKS_PER_TICK, &tmp);

   /* Add in hardware ticks */
   _psp_add_element_to_array(tmp.LW, tick_ptr->HW_TICKS[0], 
      sizeof(PSP_128_BIT_UNION) / sizeof(uint_32), 
      tmp.LW);

   /* 
   ** Convert hardware ticks to ps. (H / (T/S * H/T) * 1000000000000)
   ** Multiply by an extra 10 for rounding purposes.
   */
   _psp_mul_128_by_32(&tmp, 1000000000, &tmp);
   _psp_mul_128_by_32(&tmp, 10000, &tmp);
   _psp_div_128_by_32(&tmp, kernel_data->TICKS_PER_SECOND, &tmp);
   _psp_div_128_by_32(&tmp, kernel_data->HW_TICKS_PER_TICK, &tmp);
      /* Round OR Truncate*/
   if (round)
   {
       _psp_add_element_to_array(tmp.LW, 5, sizeof(PSP_128_BIT_UNION) / sizeof(uint_32), tmp.LW);
   }
   _psp_div_128_by_32(&tmp, 10, &tmp);

#if (PSP_ENDIAN == MQX_BIG_ENDIAN)
   *overflow_ptr = (boolean)(tmp.LLW[0] || tmp.LW[2]);
   return tmp.LW[3];
#else
   *overflow_ptr = (boolean)(tmp.LLW[1] || tmp.LW[1]);
   return tmp.LW[0];
#endif

} /* Endbody */
/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_ticks_to_picoseconds
* Returned Value   : uint_32 - number of picoseconds
* Comments         :
*    This function converts ticks into picoseconds, with rounding
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_ticks_to_picoseconds
   (
      /* [IN] Ticks to be converted */
      PSP_TICK_STRUCT_PTR tick_ptr,

      /* [OUT] pointer to where the overflow boolean is to be written */
      boolean _PTR_       overflow_ptr
   )
{ /* Body */
    return __psp_ticks_to_picoseconds(tick_ptr, overflow_ptr, TRUE);
}

/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_ticks_to_picoseconds
* Returned Value   : uint_32 - number of picoseconds
* Comments         :
*    This function converts ticks into picoseconds, with truncating
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_ticks_to_picoseconds_truncate
   (
      /* [IN] Ticks to be converted */
      PSP_TICK_STRUCT_PTR tick_ptr,

      /* [OUT] pointer to where the overflow boolean is to be written */
      boolean _PTR_       overflow_ptr
   )
{ /* Body */
   return __psp_ticks_to_picoseconds(tick_ptr, overflow_ptr, FALSE);
}
/* EOF */
