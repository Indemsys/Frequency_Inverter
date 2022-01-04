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
* $FileName: 29LV800BT.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains the block definitions for the AM29LV800BT
*   flash device.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"

/* Reference sector size */
#define AMD29LV800BT_SECTOR_SIZE     (0x4000 * BSP_FLASH_DEVICES)

#define AMD29LV800BT_NUM_SECTORS_0 (15) /* block 0 */
#define AMD29LV800BT_NUM_SECTORS_1 (1)  /* block 1 */
#define AMD29LV800BT_NUM_SECTORS_2 (2)  /* block 2 */
#define AMD29LV800BT_NUM_SECTORS_3 (1)  /* block 3 */

const FLASHX_BLOCK_INFO_STRUCT _29lv800bt_block_map_16bit[] =  {
    /* sectors per block        , offset                     ,  sector size */
    { AMD29LV800BT_NUM_SECTORS_0, (0),                          (4*AMD29LV800BT_SECTOR_SIZE)},/* 64K */
    { AMD29LV800BT_NUM_SECTORS_1, (0xF0000*BSP_FLASH_DEVICES),  (2*AMD29LV800BT_SECTOR_SIZE)},/* 32K */
    { AMD29LV800BT_NUM_SECTORS_2, (0xF8000*BSP_FLASH_DEVICES),  (AMD29LV800BT_SECTOR_SIZE/2)},/* 8K  */
    { AMD29LV800BT_NUM_SECTORS_3, (0xFC000*BSP_FLASH_DEVICES),  (AMD29LV800BT_SECTOR_SIZE)},  /* 16K */
    { 0, 0, 0 }
};

/* EOF*/
