#ifndef __io_mem_h__
#define __io_mem_h__
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
* $FileName: io_mem.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the fdv Ramdisk drivers
*
*END************************************************************************/

#include "ioctl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** IO_MEM IOCTL calls
*/
#define IO_MEM_IOCTL_GET_BASE_ADDRESS     _IO(IO_TYPE_MEM,0x01)
#define IO_MEM_IOCTL_GET_TOTAL_SIZE       _IO(IO_TYPE_MEM,0x02)
#define IO_MEM_IOCTL_GET_DEVICE_ERROR     _IO(IO_TYPE_MEM,0x03)

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _io_mem_install(char_ptr, pointer, _file_size);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
