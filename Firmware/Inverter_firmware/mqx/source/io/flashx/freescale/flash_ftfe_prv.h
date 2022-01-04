#ifndef __flash_ftfe_prv_h__
#define __flash_ftfe_prv_h__
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
* $FileName: flash_ftfe_prv.h$
* $Version : 3.8.2.0$
* $Date    : Sep-17-2012$
*
* Comments:
*
*   The file contains function prototypes and defines for the internal 
*   flash driver.
*
*END************************************************************************/

#include "flashx.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* FTFE commands */
#define FTFE_VERIFY_BLOCK              0x00
#define FTFE_VERIFY_SECTION            0x01
#define FTFE_PROGRAM_CHECK             0x02
#define FTFE_READ_RESOURCE             0x03
#define FTFE_PROGRAM_LONGWORD          0x06
#define FTFE_PROGRAM_PHRASE            0x07
#define FTFE_ERASE_BLOCK               0x08
#define FTFE_ERASE_SECTOR              0x09
#define FTFE_PROGRAM_SECTION           0x0B
#define FTFE_VERIFY_ALL_BLOCK          0x40
#define FTFE_READ_ONCE                 0x41
#define FTFE_PROGRAM_ONCE              0x43
#define FTFE_ERASE_ALL_BLOCK           0x44
#define FTFE_SECURITY_BY_PASS          0x45
#define FTFE_PROGRAM_PARTITION         0x80
#define FTFE_SET_EERAM                 0x81

/* FTFE margin read settings */
#define FTFE_MARGIN_NORMAL             0x0000
#define FTFE_USER_MARGIN_LEVEL1        0x0001
#define FTFE_USER_MARGIN_LEVEL0        0x0002
#define FTFE_FACTORY_MARGIN_LEVEL0     0x0003
#define FTFE_FACTORY_MARGIN_LEVEL1     0x0004

/* FTFE sizes */
#define FTFE_WORD_SIZE                 0x0002
#define FTFE_PHRASE_SIZE               0x0008

/* FTFE error codes */
#define FTFE_OK                        0x0000
#define FTFE_ERR_SIZE                  0x0001
#define FTFE_ERR_RANGE                 0x0002
#define FTFE_ERR_ACCERR                0x0004
#define FTFE_ERR_PVIOL                 0x0008
#define FTFE_ERR_MGSTAT0               0x0010
#define FTFE_ERR_CHANGEPROT            0x0020
#define FTFE_ERR_EEESIZE               0x0040
#define FTFE_ERR_EFLASHSIZE            0x0080
#define FTFE_ERR_ADDR                  0x0100
#define FTFE_ERR_NOEEE                 0x0200
#define FTFE_ERR_EFLASHONLY            0x0400
#define FTFE_ERR_DFLASHONLY            0x0800
#define FTFE_ERR_RDCOLERR              0x1000
#define FTFE_ERR_RAMRDY                0x2000

/* Start and End Address of PFlash IFR and DFlash IFR */
#define PFLASH_IFR_START_ADDRESS       0x000000
#define PFLASH_IFR_END_ADDRESS         0x0000FF
#define DFLASH_IFR_START_ADDRESS       0x800000
#define DFLASH_IFR_END_ADDRESS         0x8000FF
#define DFLASH_IFR_LONGWORD_ADDRESS    0x8000FC

#define FLASHX_INVALIDATE_CACHE_ALL    0xFFFFFFFF
#define FLASHX_INVALIDATE_CACHE_BLOCK0 0x00000001
#define FLASHX_INVALIDATE_CACHE_BLOCK1 0x00000002
#define FLASHX_INVALIDATE_CACHE_BLOCK2 0x00000004
#define FLASHX_INVALIDATE_CACHE_BLOCK3 0x00000008

/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

typedef struct ftfe_flash_internal_struct 
{
    vchar_ptr  ftfe_ptr;
    char_ptr   flash_execute_code_ptr;
        
} FTFE_FLASH_INTERNAL_STRUCT, _PTR_ FTFE_FLASH_INTERNAL_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

boolean ftfe_flash_init(IO_FLASHX_STRUCT_PTR);
void    ftfe_flash_deinit(IO_FLASHX_STRUCT_PTR);
boolean ftfe_flash_erase_sector(IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size);
boolean ftfe_flash_write_sector(IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr, _mem_size);

#ifdef __cplusplus
}
#endif

#endif //__flash_ftfe_prv_h__
