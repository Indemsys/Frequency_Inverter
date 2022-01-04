/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: debug_prv.h$
* $Version : 3.8.2.0$
* $Date    : Sep-16-2011$
*
* Comments:
*
*   The file contains private functions prototype, defines, structure 
*   definitions private to debug driver.
*
*END************************************************************************/

#ifndef __iodebug_prv_h__
#define __iodebug_prv_h__

#include "iodebug.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*----------------------------------------------------------------------*/
/*
**                          DATATYPE DEFINITIONS
*/

typedef struct {
    char                        FLUSH_CHAR;
    char _PTR_                  BUFFER_PTR;
    uint_32                     BUFFER_POS;
    int_32                      COUNT;
    IODEBUG_INIT_STRUCT_PTR     DEV_INIT_DATA_PTR;
    LWSEM_STRUCT                LWSEM;
} IO_DEBUG_DEVICE_STRUCT, _PTR_ IO_DEBUG_DEVICE_STRUCT_PTR; 


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

/* Internal debug functions */
#ifdef __cplusplus
extern "C" {
#endif

extern void _io_debug_semi_write_char(register char_ptr);
extern char _io_debug_semi_read_char(void);
extern void _io_debug_semi_write_string(register char_ptr);
extern void _io_debug_semi_buffer_flush(IO_DEBUG_DEVICE_STRUCT_PTR);
extern void _io_debug_semi_buffer_write(IO_DEBUG_DEVICE_STRUCT_PTR, char_ptr);

extern boolean _io_debug_itm_write_char(register char_ptr);

extern _mqx_int _io_debug_open(MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int _io_debug_close(MQX_FILE_PTR);
extern _mqx_int _io_debug_read(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_debug_write(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_debug_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
extern void _io_debug_buffer_flush(IO_DEBUG_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
