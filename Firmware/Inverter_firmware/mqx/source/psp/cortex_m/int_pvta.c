/*HEADER*********************************************************************
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
* $FileName: int_pvta.c$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains the function for returning the previous vector
*   table field in the kernel data structure.
*
*END************************************************************************/


#include "mqx_inc.h"

#if MQX_EXIT_ENABLED && MQX_USE_INTERRUPTS
/*!
 * \brief Gets the address of the interrupt vector table that MQX might have 
 * created when it started.
 * 
 * The function is useful if you are installing third-party debuggers or monitors.
 * 
 * \return Address of the interrupt vector table that MQX creates when it starts.
 * 
 * \see _int_get_vector_table
 * \see _int_set_vector_table
 */       
_psp_code_addr _int_get_previous_vector_table
   (
      void
   )
{ /* Body */
   register KERNEL_DATA_STRUCT_PTR  kernel_data;

   _GET_KERNEL_DATA(kernel_data);
   return( (_psp_code_addr)kernel_data->USERS_VBR );

} /* Endbody */
#endif

/* EOF */
