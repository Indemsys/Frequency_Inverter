#ifndef _nfc_h_
#define _nfc_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: nfc.h$
* $Version : 3.8.11.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains defines and functions prototype for the  
*   NAND Flash aController (NFC) low level driver.
*
*END************************************************************************/

#include "nandflash.h"

#define MAX_WAIT_COMMAND                       0x00100000


/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

pointer _bsp_get_nfc_address(void);

extern void    nfc_io_init(void);

extern uint_32 nfc_init( IO_NANDFLASH_STRUCT_PTR );

extern void    nfc_deinit( IO_NANDFLASH_STRUCT_PTR );

extern uint_32 nfc_erase_flash( IO_NANDFLASH_STRUCT_PTR );

extern uint_32 nfc_erase_block( IO_NANDFLASH_STRUCT_PTR, uint_32, boolean );

extern uint_32 nfc_read_page( IO_NANDFLASH_STRUCT_PTR, uchar_ptr, 
    uint_32, uint_32 );

extern uint_32 nfc_write_page( IO_NANDFLASH_STRUCT_PTR, uchar_ptr, 
    uint_32, uint_32 );
    
extern uint_32 nfc_reset( IO_NANDFLASH_STRUCT_PTR );

extern uint_32 nfc_check_block( IO_NANDFLASH_STRUCT_PTR, uint_32 );
    
extern uint_32 nfc_mark_block_as_bad( IO_NANDFLASH_STRUCT_PTR, uint_32);
    
extern uint_32 nfc_read_ID( IO_NANDFLASH_STRUCT_PTR, uchar_ptr, _mem_size );
    
extern _mqx_int nfc_ioctl(IO_NANDFLASH_STRUCT_PTR, _mqx_uint, pointer);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
