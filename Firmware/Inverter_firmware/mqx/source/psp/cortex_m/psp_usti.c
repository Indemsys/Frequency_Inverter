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
* $FileName: psp_usti.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the function for converting microseconds to a ticks
*
*END************************************************************************/
    
#include "mqx_inc.h"

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_usecs_to_ticks
 * Returned Value   : void
 * Comments         :
 *   This function converts microseconds into ticks
 *
 *END*----------------------------------------------------------------------*/

void _psp_usecs_to_ticks
   (
       /* [IN] The number of microseconds to convert */
       _mqx_uint           usecs,

       /* [OUT] Pointer to tick structure where the result will be stored */
       PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
   uint_64                tmp;
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   tmp = (uint_64)usecs * kernel_data->TICKS_PER_SECOND;
   tick_ptr->TICKS[0] = tmp / (1000 * 1000);

   /* Calculate the remaining microticks */
   tmp %= (1000 * 1000);

   /* Convert to hardware ticks */
   tmp = (tmp * kernel_data->HW_TICKS_PER_TICK + 500000) / (1000 * 1000);

   tick_ptr->HW_TICKS[0] = (uint_32)tmp;

} /* Endbody */

/* EOF */
