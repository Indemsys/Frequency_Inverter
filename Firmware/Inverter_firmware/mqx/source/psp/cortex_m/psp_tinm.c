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
* $FileName: psp_tinm.c$
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
 * Function Name    : _psp_normalize_ticks
 * Returned Value   : none
 * Comments         : Normalizes ticks and partial ticks in a tick structure
 *
 *END*----------------------------------------------------------------------*/

void _psp_normalize_ticks
   (
       /* [IN/OUT] Tick structure to be normalized */
       PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR  kernel_data;
   register uint_32        ticks_per_tick;

   _GET_KERNEL_DATA(kernel_data);

   ticks_per_tick = kernel_data->HW_TICKS_PER_TICK;

   if (tick_ptr->HW_TICKS[0] >= ticks_per_tick) {
      register uint_32 t = tick_ptr->HW_TICKS[0] / ticks_per_tick;
      tick_ptr->TICKS[0] += t;
      tick_ptr->HW_TICKS[0] -= t * ticks_per_tick;
   } /* Endif */

} /* Endbody */
#endif

/* EOF */
