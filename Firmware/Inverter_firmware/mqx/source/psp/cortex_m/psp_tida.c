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
* $FileName: psp_tida.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the functions for converting ticks to days
*
*END************************************************************************/

#include "mqx_inc.h"


#if MQX_HAS_TICK

/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_ticks_to_days
* Returned Value   : uint_32 - number of days
* Comments         :
*    This function converts ticks into days
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_ticks_to_days
   (
      PSP_TICK_STRUCT_PTR tick_ptr,
      boolean _PTR_       overflow_ptr
   )
{ /* Body */
   uint_64                tmp;
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   tmp = tick_ptr->TICKS[0];

   if ((tmp != MAX_UINT_64) && 
      (tick_ptr->HW_TICKS[0] > (kernel_data->HW_TICKS_PER_TICK/2)))
   {
      tmp++;
   } /* Endif */

   tmp = (tmp / kernel_data->TICKS_PER_SECOND) /
      (SECS_IN_MINUTE * MINUTES_IN_HOUR * HOURS_IN_DAY);

   *overflow_ptr = (boolean)(tmp > MAX_UINT_32);

   return (uint_32)tmp;

} /* Endbody */

#endif

/* EOF */
