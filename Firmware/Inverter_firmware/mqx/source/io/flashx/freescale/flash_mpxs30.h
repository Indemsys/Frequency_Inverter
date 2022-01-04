/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: flash_mpxs30.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the default initialization record for the
*   flash device.
*
*END************************************************************************/
#ifndef __flash_mpxs30_h__
#define __flash_mpxs30_h__


#include "flashx.h"

#define MPXS30_CFLASH_LAS_SIZE      (256*1024)
#define MPXS30_CFLASH_MAS_SIZE      (256*1024)
#define MPXS30_CFLASH_HAS_SIZE      (512*1024)
#define MPXS30_CFLASH_SHADOW_SIZE   (16*1024)
#define MPXS30_DFLASH_LAS_SIZE      (64*1024)

#define MPXS30_CFLASH0_LAS_ADDR     (0x000000)
#define MPXS30_CFLASH1_LAS_ADDR     (0x040000)
#define MPXS30_CFLASH_MAS_ADDR      (0x080000)
#define MPXS30_CFLASH_HAS_ADDR      (0x100000)
#define MPXS30_CFLASH0_SHADOW_ADDR  (0x200000)
#define MPXS30_CFLASH1_SHADOW_ADDR  (0x280000)

/*----------------------------------------------------------------------*/
/*
**              DEFINED VARIABLES
*/

extern const FLASHX_BLOCK_INFO_STRUCT _mpxs30_data_flash_block_map[];
extern const FLASHX_BLOCK_INFO_STRUCT _mpxs30_code_flash_block_map[];

extern const FLASHX_DEVICE_IF_STRUCT _flashx_c90i_code_if;

#endif
