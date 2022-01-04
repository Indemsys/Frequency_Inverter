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
* $FileName: io_sinit.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the function that initializes the kernel
*   default serial I/O.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "serial.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_default_init
* Returned Value   : none
* Comments         :
*   Initializes the kernel default serial I/O
*
*END*----------------------------------------------------------------------*/

void _io_serial_default_init
(
 void
 )
{ /* Body */
  KERNEL_DATA_STRUCT_PTR kernel_data;

  _GET_KERNEL_DATA(kernel_data);

  if ( kernel_data->INIT.IO_CHANNEL )
  {
    kernel_data->PROCESSOR_STDIN = _io_fopen((char _PTR_)kernel_data->INIT.IO_CHANNEL,(char _PTR_)kernel_data->INIT.IO_OPEN_MODE);
    kernel_data->PROCESSOR_STDOUT = kernel_data->PROCESSOR_STDIN;
    kernel_data->PROCESSOR_STDERR = kernel_data->PROCESSOR_STDIN;
  } /* Endif */

} /* Endbody */

/* EOF */
