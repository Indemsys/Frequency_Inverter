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
* $FileName: s29ws64.c$
* $Version : 3.8.3.0$
* $Date    : Oct-7-2011$
*
* Comments:
*
*   The file contains the block definitions for the
*   flash device.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"

#define S28WS64_SECTOR_SIZE_8K      (0x2000)
#define S28WS64_NUM_SECTORS_1       (8)
#define S28WS64_SECTOR_SIZE_64K     (0x10000)
#define S28WS64_NUM_SECTORS_2       (126)
#define S28WS64_NUM_SECTORS_3       (8)
#define S28WS64_BANK0_START         (0)
#define S28WS64_BANK1_START         (S28WS64_NUM_SECTORS_1 * S28WS64_SECTOR_SIZE_8K)
#define S28WS64_BANK2_START         ((S28WS64_NUM_SECTORS_1 * S28WS64_SECTOR_SIZE_8K) + (S28WS64_NUM_SECTORS_2 * S28WS64_SECTOR_SIZE_64K))

const FLASHX_BLOCK_INFO_STRUCT _S28WS64_block_map_16bit[] =  {
{ S28WS64_NUM_SECTORS_1, 0, S28WS64_SECTOR_SIZE_8K},
{ S28WS64_NUM_SECTORS_2, S28WS64_BANK1_START,S28WS64_SECTOR_SIZE_64K},
{ S28WS64_NUM_SECTORS_3, S28WS64_BANK2_START,S28WS64_SECTOR_SIZE_8K},
{ 0, 0, 0 }
};

/* EOF*/
