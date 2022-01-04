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
* $FileName: psp_tiol.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*   This file contains the functions for converting from time to ticks.
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name    : _psp_time_to_ticks
* Returned Value   : void
* Comments         :
*    This function converts the time struct format into ticks
*
*END*----------------------------------------------------------------------*/

void _psp_time_to_ticks
   (
      TIME_STRUCT_PTR     time_ptr,
      PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
   PSP_TICK_STRUCT tick1;
   PSP_TICK_STRUCT tick2;

   _psp_seconds_to_ticks(time_ptr->SECONDS, &tick1);
   _psp_msecs_to_ticks(time_ptr->MILLISECONDS, &tick2);

   _psp_add_ticks(&tick1, &tick2, tick_ptr);

} /* Endbody */

/* EOF */
