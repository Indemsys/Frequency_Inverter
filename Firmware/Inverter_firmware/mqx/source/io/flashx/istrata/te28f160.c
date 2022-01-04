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
* $FileName: te28f160.c$
* $Version : 3.8.5.0$
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

/* Flash device series definition to be used by istrataprv.h */
#define I28F160_SECTOR_SIZE     (0x2000) /* 4KW */
#define I28F160_NUM_SECTORS_1   (8)
#define I28F160_NUM_SECTORS_2   (31)

/* Bottom Boot sector map */
const FLASHX_BLOCK_INFO_STRUCT _i28f160bot_block_map_16bit[] =  {
    { I28F160_NUM_SECTORS_1, (0),       I28F160_SECTOR_SIZE},     /* 4KW sectors */
    { I28F160_NUM_SECTORS_2, (0x10000), ( 8 * I28F160_SECTOR_SIZE)}, /* 32KW sectors */
    { 0, 0, 0 }
};

/* Top Boot sector map */
const FLASHX_BLOCK_INFO_STRUCT _i28f160top_block_map_16bit[] =  {
    { I28F160_NUM_SECTORS_2, (0),        ( 8 * I28F160_SECTOR_SIZE)},  /* 32KW sectors */
    { I28F160_NUM_SECTORS_1, (0x1F0000), (I28F160_SECTOR_SIZE)},    /* 4KW sectors */
{ 0, 0, 0 }
};

/* EOF*/

/* EOF*/
