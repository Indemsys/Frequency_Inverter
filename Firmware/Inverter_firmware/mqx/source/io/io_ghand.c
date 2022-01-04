/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: io_ghand.c$
* $Version : 3.5.4.0$
* $Date    : Dec-8-2009$
*
* Comments:
*
*   This file contains the function for returning the current I/O handler
*   in the kernel data structure, or in a task.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "io.h"

#if MQX_USE_IO
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_get_handle
* Returned Value   : pointer
* Comments         : 
*
*    This function returns the address of a default standard I/O FILE.
*    If an incorrect type is given, or the file_ptr has not been specified,
*    the function will return NULL.
* 
*END*----------------------------------------------------------------------*/

pointer _io_get_handle
   (
      /* [IN] which I/O handle to return */
      _mqx_uint stdio_type
   )
{ /* Body */
   register KERNEL_DATA_STRUCT_PTR  kernel_data;

   _GET_KERNEL_DATA(kernel_data);
   switch ( (uint_16)stdio_type ) {

      case IO_PROC_STDIN:
         return kernel_data->PROCESSOR_STDIN;

      case IO_PROC_STDOUT:
         return kernel_data->PROCESSOR_STDOUT;

      case IO_PROC_STDERR:
         return kernel_data->PROCESSOR_STDERR;

      case IO_STDIN:
         return kernel_data->ACTIVE_PTR->STDIN_STREAM;

      case IO_STDOUT:
         return kernel_data->ACTIVE_PTR->STDOUT_STREAM;

      case IO_STDERR:
         return kernel_data->ACTIVE_PTR->STDERR_STREAM;

      default:
         return (pointer) NULL;

   } /* Endswitch */

} /* Endbody */
#endif
/* EOF */
