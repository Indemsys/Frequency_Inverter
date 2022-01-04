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
* $FileName: io_init.c$
* $Version : 3.5.5.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the function that initializes the I/O 
*   sub-system.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

#if MQX_USE_IO

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_init
* Returned Value   : _mqx_uint MQX_OK or a MQX error code
* Comments         :
*   Initializes the kernel I/O subsystem
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_init(void)
{ 
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);
   _QUEUE_INIT(&kernel_data->IO_DEVICES, 0);
   return (_lwsem_create((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM, 1));

} 

#endif
/* EOF */
