#ifndef __ionulprv_h__
#define __ionulprv_h__
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
* $FileName: ionulprv.h$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the null disk.
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/* Internal functions to IO_NULL */
#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_null_open(MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int _io_null_close(MQX_FILE_PTR);
extern _mqx_int _io_null_read (MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_null_write(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_null_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
