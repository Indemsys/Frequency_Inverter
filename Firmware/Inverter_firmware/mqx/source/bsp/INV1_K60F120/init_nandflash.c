/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: init_nandflash.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the default initialization record for the NAND
*   flash.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "nfc.h"

extern NANDFLASH_INFO_STRUCT _MT29F2G16_organization_16bit[];

const NANDFLASH_INIT_STRUCT _bsp_nandflash_init =
{
   /* NAME                 */   "nandflash:",
   /* INIT                 */   nfc_init,
   /* DEINIT               */   nfc_deinit,
   /* CHIP_ERASE           */   nfc_erase_flash,
   /* BLOCK_ERASE          */   nfc_erase_block,
   /* PAGE_READ            */   nfc_read_page,
   /* PAGE_PROGRAM         */   nfc_write_page,
   /* WRITE_PROTECT        */   NULL,
   /* IS_BLOCK_BAD         */   nfc_check_block,
   /* MARK_BLOCK_AS_BAD    */   nfc_mark_block_as_bad,
   /* IOCTL                */   nfc_ioctl,
   /* NANDFLASH_INFO_PTR   */   _MT29F2G16_organization_16bit,
   /* VIRTUAL_PAGE_SIZE    */   0,
   /* NUM_VIRTUAL_PAGES    */   0,
   /* PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO */   
                                0,
   /* ECC_SIZE             */   0,
   /* WRITE_VERIFY         */   0,
   /* DEVICE_SPECIFIC_DATA */   0
};

/* EOF */
