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
* $FileName: int_vtab.c$
* $Version : 3.8.5.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This file contains the functions used to access the vector
*   table locations.
*
*END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_INTERRUPTS
/*!
 * \brief Changes the location of the interrupt vector table.
 * 
 * \param[in] addr Address of the new interrupt vector table.
 * 
 * \return Address of the previous vector table.
 * 
 * \warning Behavior depends on the BSP and the PSP.
 * 
 * \see _int_get_vector_table
 * \see _int_get_previous_vector_table
 */          
_psp_code_addr _int_set_vector_table
(
    _psp_code_addr addr
)
{ /* Body */
   _psp_code_addr         old;
   /* TODO check why we need this part of code */
#if MQX_EXIT_ENABLED
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);
   kernel_data->USERS_VBR = addr;
#endif

   old = _PSP_GET_VTOR();
   _PSP_SET_VTOR(addr);

   return (old);

} /* Endbody */

/*!
 * \brief Gets the address of the current interrupt vector table. The function 
 * depends on the PSP.
 * 
 * \return Address of the current interrupt vector table.
 * 
 * \see _int_set_vector_table
 * \see _int_get_previous_vector_table
 */      
_psp_code_addr _int_get_vector_table
(
    void
)
{
    return _PSP_GET_VTOR();
}

#endif
