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
* $FileName: psp_psti.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the function for converting picoseconds to a ticks
*
*END************************************************************************/

#include "mqx_inc.h"
#if MQX_HAS_TICK

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_psecs_to_ticks
 * Returned Value   : void
 * Comments         :
 *   This function converts picoseconds into ticks. Note, there is no way to
 * represent MAX_UINT_16 picoseconds in terms of ticks.
 *
 *END*----------------------------------------------------------------------*/

void _psp_psecs_to_ticks
   (
       /* [IN] The number of picoseconds to convert */
       _mqx_uint           psecs,

       /* [OUT] Pointer to tick structure where the result will be stored */
       PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
   PSP_128_BIT_UNION      tmp;
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   tmp.LLW[1] = 0;
   tmp.LLW[0] = (uint_64)psecs * kernel_data->TICKS_PER_SECOND;
   tick_ptr->TICKS[0] = tmp.LLW[0] / (1000ULL * 1000 * 1000 * 1000);

   /* Calculate the remaining picoticks */
   tmp.LLW[0] %= (1000ULL * 1000 * 1000 * 1000);
#else
   tmp.LLW[0] = 0;
   tmp.LLW[1] = (uint_64)psecs * kernel_data->TICKS_PER_SECOND;
   tick_ptr->TICKS[0] = tmp.LLW[1] / (1000ULL * 1000 * 1000 * 1000);

   /* Calculate the remaining picoticks */
   tmp.LLW[1] %= (1000ULL * 1000 * 1000 * 1000);
#endif

   /* Convert to hardware ticks */

   _psp_mul_128_by_32(&tmp, kernel_data->HW_TICKS_PER_TICK, &tmp);
   
   _psp_div_128_by_32(&tmp, 1000UL * 1000 * 1000, &tmp);
   _psp_div_128_by_32(&tmp, 1000, &tmp);

#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   tick_ptr->HW_TICKS[0] = tmp.LW[0];
#else
   tick_ptr->HW_TICKS[0] = tmp.LW[3];
#endif

} /* Endbody */
#endif

/* EOF */
