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
* $FileName: psp_miti.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the function for converting minutes to ticks
*
*END************************************************************************/

#include "mqx_inc.h"
#if MQX_HAS_TICK

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_minutes_to_ticks
 * Returned Value   : void
 * Comments         :
 *   This function converts minutes into ticks
 *
 *END*----------------------------------------------------------------------*/

void _psp_minutes_to_ticks
   (
       /* [IN] The number of minutes to convert */
       _mqx_uint           minutes,

       /* [OUT] Pointer to tick structure where the result will be stored */
       PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* body */
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   tick_ptr->HW_TICKS[0] = 0;
   tick_ptr->TICKS[0]    = (uint_64)minutes * kernel_data->TICKS_PER_SECOND 
       * 60;
      
} /* Endbody */
#endif

/* EOF */
