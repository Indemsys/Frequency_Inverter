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
* $FileName: MX29LV640E_TB.c$
* $Version : 3.8.4.0$
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
#include "flashx.h"

#define MX29LV640E_LARGE_SECTOR_SIZE (0x8000)
#define MX29LV640E_NUM_LARGE_SECTORS (127)
#define MX29LV640E_SMALL_SECTOR_SIZE (0x1000)
#define MX29LV640E_NUM_SMALL_SECTORS (8)


const FLASHX_BLOCK_INFO_STRUCT _mx29lv640e_block_map_16bit[] =  { 
    { MX29LV640E_NUM_LARGE_SECTORS, 0, MX29LV640E_LARGE_SECTOR_SIZE * 2 },
    { MX29LV640E_NUM_SMALL_SECTORS, MX29LV640E_NUM_LARGE_SECTORS * MX29LV640E_LARGE_SECTOR_SIZE * 2, MX29LV640E_SMALL_SECTOR_SIZE * 2 },
    { 0, 0, 0 }
};
