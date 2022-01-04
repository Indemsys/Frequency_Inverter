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
* $FileName: debug.h$
* $Version : 3.8.3.0$
* $Date    : Sep-16-2011$
*
* Comments:
*
*   The file contains public functions prototype, defines, structure 
*   definitions private to the debug driver
*
*END************************************************************************/
#ifndef __iodebug_h__
#define __iodebug_h__


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/
/* IOCLT flag - change flush character */
#define IO_IOCTL_IODEBUG_SET_FLUSH_CHAR         _IO(IO_TYPE_IODEBUG,0x01)
/* No flush character */
#define IODEBUG_NOFLUSH_CHAR                    '\0' 
#define IODEBUG_MODE_SEMIHOST                   0
#define IODEBUG_MODE_ITM                        1

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

typedef struct { 
    uint_32 MODE;
    uint_32 DATA_LENGTH;
    char    FLUSH_CHAR;
} IODEBUG_INIT_STRUCT, _PTR_ IODEBUG_INIT_STRUCT_PTR;

typedef const IODEBUG_INIT_STRUCT _PTR_ IODEBUG_INIT_STRUCT_CPTR;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_debug_install(char_ptr, IODEBUG_INIT_STRUCT_CPTR);
extern _mqx_int _io_debug_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */

