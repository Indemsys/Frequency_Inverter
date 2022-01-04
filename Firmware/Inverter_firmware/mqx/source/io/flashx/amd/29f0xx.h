#ifndef _29f0xx_h_
#define _29f0xx_h_
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
* $FileName: 29f0xx.h$
* $Version : 3.8.4.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the 29f0xx devices
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/* 
**              DEFINED VARIABLES
*/
extern const FLASHX_BLOCK_INFO_STRUCT _29f010_block_map_8bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f010_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f010_block_map_32bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f040_block_map_8bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f040_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f040_block_map_32bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f080_block_map_8bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f080_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f080_block_map_32bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f016_block_map_8bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f016_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29f016_block_map_32bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29lv640_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29lv160_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29lv320_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _29pl160_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _at49bv1614a_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _at49bv6416_block_map_16bit[];

extern const FLASHX_DEVICE_IF_STRUCT _29f0xx_if;


#endif
/* EOF */
