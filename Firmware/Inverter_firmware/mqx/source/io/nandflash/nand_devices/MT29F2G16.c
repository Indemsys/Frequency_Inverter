/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: MT29F2G16.c$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains the definitions for the MT29F2G16 flash device.
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "nandflash.h"


/* MT29F2G16AABWP organization */
#define MT29F2G16_PHYSICAL_PAGE_SIZE      2048
#define MT29F2G16_SPARE_AREA_SIZE         64
#define MT29F2G16_BLOCK_SIZE              131072 /* 128kB */
#define MT29F2G16_NUM_BLOCKS              2048
#define MT29F2G16_WIDTH                   16


NANDFLASH_INFO_STRUCT _MT29F2G16_organization_16bit[] =  {
  MT29F2G16_PHYSICAL_PAGE_SIZE,
  MT29F2G16_SPARE_AREA_SIZE, 
  MT29F2G16_BLOCK_SIZE,
  MT29F2G16_NUM_BLOCKS,
  MT29F2G16_WIDTH
};

/* EOF*/
