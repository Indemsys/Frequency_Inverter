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
* $FileName: psp_tiad.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the function for adding two tick structs
*
*END************************************************************************/

#include "mqx_inc.h"

#if MQX_HAS_TICK

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_add_ticks
 * Returned Value   : none
 * Comments         : Adds two PSP tick structure together, including hardware
 *   ticks
 *
 *END*----------------------------------------------------------------------*/

void _psp_add_ticks
   (
       /* [IN] The two structures to add - both must be normalized */
       PSP_TICK_STRUCT_PTR a_ptr,
       PSP_TICK_STRUCT_PTR b_ptr,

       /* [OUT] The result of the addition */
       PSP_TICK_STRUCT_PTR r_ptr
   )
{ /* Body */
   register uint_32       a_hw_ticks;
   register uint_32       b_hw_ticks;
   register uint_32       hwtpt;

   r_ptr->TICKS[0] = a_ptr->TICKS[0] + b_ptr->TICKS[0];

   a_hw_ticks  = a_ptr->HW_TICKS[0];
   b_hw_ticks  = b_ptr->HW_TICKS[0];

   hwtpt = _mqx_kernel_data->HW_TICKS_PER_TICK;

   if ( a_hw_ticks >= (hwtpt - b_hw_ticks)) {
      r_ptr->TICKS[0]++;
      r_ptr->HW_TICKS[0] = a_hw_ticks + (b_hw_ticks - hwtpt);
   } else {
      r_ptr->HW_TICKS[0] = a_hw_ticks + b_hw_ticks;
   } /* Endif */

} /* Endbody */

#endif

/* EOF */

