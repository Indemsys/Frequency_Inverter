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
* $FileName: 29f400.c$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains the block definitions for the
*   flash device.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"

#define AMD29F400_SECTOR_SIZE       (0x10000 * BSP_FLASH_DEVICES)

const FLASHX_BLOCK_INFO_STRUCT _29f400_block_map_32bit[] =  { 
    { 7, 0,                             AMD29F400_SECTOR_SIZE },
    { 1, (0x70000 * BSP_FLASH_DEVICES), (AMD29F400_SECTOR_SIZE/2) },
    { 2, (0x78000 * BSP_FLASH_DEVICES), (AMD29F400_SECTOR_SIZE/8) },
    { 1, (0x7C000 * BSP_FLASH_DEVICES), (AMD29F400_SECTOR_SIZE/4) },
    { 0, 0, 0 }
};

/* EOF*/
