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
* $FileName: psp_mstiq.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the function for converting milliseconds to a ticks
*   ignoring the Hardware Ticks
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_msecs_to_ticks_quick
 * Returned Value   : void
 * Comments         :
 *   This function converts milliseconds into ticks without HW ticks
 *
 *END*----------------------------------------------------------------------*/

void _psp_msecs_to_ticks_quick
   (
       /* [IN] The number of milliseconds to convert */
       _mqx_uint           msecs,

       /* [OUT] Pointer to tick structure where the result will be stored */
       PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
   static uint_32         ms_per_tick, heuristic = 0;
   KERNEL_DATA_STRUCT_PTR kernel_data;

   tick_ptr->HW_TICKS[0] = 0;

   if (heuristic == 1) {
      /* Perform fast calculation */
fast: tick_ptr->TICKS[0] = (uint_64)(msecs / ms_per_tick);
      return;
   }/* Endif */

   _GET_KERNEL_DATA(kernel_data);

   if (heuristic == 2) {
slow: tick_ptr->TICKS[0] =
         ((uint_64)msecs * kernel_data->TICKS_PER_SECOND) / 1000;
      return;
   }/* Endif */

   ms_per_tick = 1000 / kernel_data->TICKS_PER_SECOND;
   if ((ms_per_tick * kernel_data->TICKS_PER_SECOND) == 1000) {
      heuristic = 1;
      goto fast;
   }/* Endif */

   heuristic = 2;
   goto slow;

} /* Endbody */
/* EOF */
