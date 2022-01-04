#ifndef _nandflashprv_h_
#define _nandflashprv_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: nandflashprv.h$
* $Version : 3.6.2.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the NAND flash driver.
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


/* Properties of device */
#define IO_NANDFLASH_ATTRIBS  (IO_DEV_ATTR_READ  | IO_DEV_ATTR_SEEK | \
                               IO_DEV_ATTR_WRITE | IO_DEV_ATTR_BLOCK_MODE)


/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/



/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* These are from nandflash.c */
extern _mqx_int  _io_nandflash_open(MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int  _io_nandflash_close(MQX_FILE_PTR);
extern _mqx_int  _io_nandflash_read (MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int  _io_nandflash_write(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int  _io_nandflash_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
