/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor
* All Rights Reserved                       

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
* $FileName: psp_tkti.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the functions for converting between ticks and
*   the TIME_STRUCT format.
*
*END************************************************************************/

#include "mqx_inc.h"


/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_ticks_to_time
* Returned Value   : boolean - Returns FALSE is overflow occurs
* Comments         :
*    This function converts ticks into the time struct format
*
*END*----------------------------------------------------------------------*/

boolean _psp_ticks_to_time
   (
      /* [IN] Pointer to the tick struct to store the results in */
      PSP_TICK_STRUCT_PTR tick_ptr,

      /* [OUT] Pointer to the time struct to convert */
      TIME_STRUCT_PTR     time_ptr
   )
{ /* Body */
   uint_64                tmp;
   uint_32                tps;
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   tps = kernel_data->TICKS_PER_SECOND;

   /* Saturate if ticks go out of range of time struct */
   if ( (tick_ptr->TICKS[0] / tps) > MAX_UINT_32) {
      time_ptr->SECONDS      = MAX_UINT_32;
      time_ptr->MILLISECONDS = 999;
      return FALSE;
   } /* Endif */

   /* Recompute TICKS to milliseconds, together with HW_TICKS */
   tmp = (tick_ptr->TICKS[0] * 1000) + (tick_ptr->HW_TICKS[0] * 1000 / kernel_data->HW_TICKS_PER_TICK);
   tmp = tmp / tps;
 
   /* Compute seconds and remainder from milliseconds value */
   time_ptr->SECONDS      = tmp / 1000;
   time_ptr->MILLISECONDS = tmp - time_ptr->SECONDS * 1000;

   return TRUE;

} /* Endbody */