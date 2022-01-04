#ifndef __io_dun_h__
#define __io_dun_h__
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
* $FileName: io_dun.h$
* $Version : 3.8.11.0$
* $Date    : Sep-26-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the DUN and RAS drivers
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                    TYPE DEFINITIONS
*/

/*
** RAS DEVICE STRUCT
*/
typedef struct {
   char_ptr     RECV;
   char_ptr     SEND;
   _mqx_uint    RECV_SIZE;
   _mqx_uint    SEND_SIZE;
} IODUN_DEV_STRUCT, _PTR_ IODUN_DEV_STRUCT_PTR;

/*
** RAS FILE STRUCT
*/
typedef struct {
   MQX_FILE_PTR   F_PTR;
   _mqx_uint      STATE;
   LWSEM_STRUCT   LWSEM;
   char_ptr       PARSE;
   char           F_CHAR;
} IODUN_STRUCT, _PTR_ IODUN_STRUCT_PTR;


/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif
 
 
_mqx_uint _io_ras_install(char_ptr);
_mqx_uint _io_dun_install(char_ptr);

extern _mqx_int _io_dun_open      (MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int _io_dun_close     (MQX_FILE_PTR);
extern _mqx_int _io_dun_write     (MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_dun_read      (MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_dun_ioctl     (MQX_FILE_PTR, _mqx_uint, pointer);
extern char     _io_dun_read_char (MQX_FILE_PTR);

#ifdef __cplusplus
}
#endif


#endif
/* EOF */
