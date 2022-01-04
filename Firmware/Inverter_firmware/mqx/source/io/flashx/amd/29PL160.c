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
* $FileName: 29PL160.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains the block definitions for the AM29PL160
*   flash device.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"

#define AMD29PL160_SECTOR_SIZE   (0x4000 * BSP_FLASH_DEVICES)
#define AMD29PL160_NUM_SECTORS_1 (1)
#define AMD29PL160_NUM_SECTORS_2 (2)
#define AMD29PL160_NUM_SECTORS_3 (1)
#define AMD29PL160_NUM_SECTORS_4 (7)


const FLASHX_BLOCK_INFO_STRUCT _29pl160_block_map_16bit[] =  {
    { AMD29PL160_NUM_SECTORS_1, (0),                            (AMD29PL160_SECTOR_SIZE)},
    { AMD29PL160_NUM_SECTORS_2, (0x4000*BSP_FLASH_DEVICES),     (AMD29PL160_SECTOR_SIZE/2)},
    { AMD29PL160_NUM_SECTORS_3, (0x8000*BSP_FLASH_DEVICES),     (14*AMD29PL160_SECTOR_SIZE)},
    { AMD29PL160_NUM_SECTORS_4, (0x40000*BSP_FLASH_DEVICES),    (16*AMD29PL160_SECTOR_SIZE)},
    { 0, 0, 0 }
};

/* EOF*/
