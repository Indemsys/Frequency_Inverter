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
* $FileName: lh28f016.c$
* $Version : 3.8.4.0$
* $Date    : Oct-4-2011$
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

#define LH28F016_SECTOR_SIZE (0x10000)
#define LH28F016_NUM_SECTORS (32)

const FLASHX_BLOCK_INFO_STRUCT _lh28f016_block_map_8bit[] =  { 
    { LH28F016_NUM_SECTORS, 0, LH28F016_SECTOR_SIZE },
    { 0, 0, 0}
};

const FLASHX_BLOCK_INFO_STRUCT _lh28f016_block_map_16bit[] =  { 
    { LH28F016_NUM_SECTORS, 0, 2*LH28F016_SECTOR_SIZE },
    { 0, 0, 0}
};

const FLASHX_BLOCK_INFO_STRUCT _lh28f016_block_map_32bit[] =  { 
    { LH28F016_NUM_SECTORS, 0, 4*LH28F016_SECTOR_SIZE },
    { 0, 0, 0}
};

/* EOF*/
