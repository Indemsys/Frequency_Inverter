#ifndef __flash_ftfl_h__
#define __flash_ftfl_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: flash_ftfe.h$
* $Version : 3.8.2.0$
* $Date    : Oct-5-2011$
*
* Comments:
*
*   The file contains function prototypes and defines for the internal 
*   flash driver.
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/* 
**              DEFINED VARIABLES
*/

extern const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisN_block_map[];
extern const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisX_block_map[];
extern const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf51xx_plus_block_map[];

extern const FLASHX_DEVICE_IF_STRUCT _flashx_ftfe_if;

#endif //__flash_ftfl_h__
