#ifndef _lh28fxxx_h_
#define _lh28fxxx_h_
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
* $FileName: lh28fxxx.h$
* $Version : 3.8.4.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the LH28FXXX devices
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/* 
**              DEFINED VARIABLES
*/
extern const FLASHX_BLOCK_INFO_STRUCT _lh28f016_block_map_8bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _lh28f016_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _lh28f016_block_map_32bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _lh28f128_block_map_8bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _lh28f128_block_map_16bit[];

const FLASHX_DEVICE_IF_STRUCT _lh28fxxx_if;

#endif
/* EOF */
