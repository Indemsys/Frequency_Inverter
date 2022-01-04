#ifndef _io_pipe_h_
#define _io_pipe_h_
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
* $FileName: io_pipe.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   The file contains the public functions prototype, defines, 
*   structure definitions to the Pipe IO device
*
*END************************************************************************/

#include "ioctl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


/*
** PIPE IOCTL calls
*/
#define PIPE_IOCTL_GET_SIZE                  _IO(IO_TYPE_PIPE,0x01)
#define PIPE_IOCTL_FULL                      _IO(IO_TYPE_PIPE,0x02)
#define PIPE_IOCTL_EMPTY                     _IO(IO_TYPE_PIPE,0x03)
#define PIPE_IOCTL_RE_INIT                   _IO(IO_TYPE_PIPE,0x04)
#define PIPE_IOCTL_CHAR_AVAIL                _IO(IO_TYPE_PIPE,0x05)
#define PIPE_IOCTL_NUM_CHARS_FULL            _IO(IO_TYPE_PIPE,0x06)
#define PIPE_IOCTL_NUM_CHARS_FREE            _IO(IO_TYPE_PIPE,0x07)



/*----------------------------------------------------------------------*/
/*
**                          EXTERN FUNCTION DEFINITIONS
*/




#ifdef __cplusplus
extern "C" {
#endif

extern uint_32  _io_pipe_install(char_ptr, uint_32, uint_32);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
