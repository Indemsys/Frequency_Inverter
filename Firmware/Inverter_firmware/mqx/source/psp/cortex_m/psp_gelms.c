/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: psp_gelms.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the functions for obtaining the elapsed milliseconds
*
*END************************************************************************/

#include "mqx_inc.h"


/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_get_milliseconds_elapsed
* Returned Value   : uint_32 - number of milliseconds
* Comments         :
*    This function obtains current elapsed milliseconds
*
*END*----------------------------------------------------------------------*/

uint_32 _psp_get_elapsed_milliseconds
   (
      void
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   uint_64                ticks;
   uint_32                ms_per_tick;
   uint_32                ms;

   _GET_KERNEL_DATA(kernel_data);

   ticks = ((PSP_TICK_STRUCT_PTR)&kernel_data->TIME)->TICKS[0];
   ms_per_tick = 1000 / kernel_data->TICKS_PER_SECOND;
   if ((ms_per_tick * kernel_data->TICKS_PER_SECOND) == 1000) {
      ms = (uint_32)(ticks * ms_per_tick);
   } else {
      ms = (uint_32)((ticks * 1000) / kernel_data->TICKS_PER_SECOND);
   } /* Endif */
   return(ms);
   
} /* Endbody */

/* EOF */
