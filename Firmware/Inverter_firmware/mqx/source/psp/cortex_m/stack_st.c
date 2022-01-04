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
* $FileName: stack_st.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the function for obtaining the address of the
*   stack start structure for the task.
*
*END************************************************************************/

#include "mqx_inc.h"

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _psp_get_stack_start
* Returned Value   : 
* Comments         :
*   This function returns the pointer to the stack start structure on
* the stack.
*
*END*-------------------------------------------------------------------------*/

PSP_STACK_START_STRUCT_PTR _psp_get_stack_start
   (
      /* [IN] the task descriptor whose stack start struct address is wanted */
      TD_STRUCT_PTR td_ptr
   )
{ /* Body */

   return (pointer)((uchar _PTR_)td_ptr->STACK_BASE -  sizeof(PSP_STACK_START_STRUCT));

} /* Endbody */

/* EOF */
