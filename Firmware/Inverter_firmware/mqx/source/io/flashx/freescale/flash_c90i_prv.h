/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: flash_c90i_prv.h$
* $Version : 3.8.2.0$
* $Date    : Sep-17-2012$
*
* Comments:
*
*   The file contains function prototypes and defines for the internal 
*   flash driver.
*
*END************************************************************************/
#ifndef __flash_c90i_prv_h__
#define __flash_c90i_prv_h__


#include "flashx.h"
#include "flashxprv.h"


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

// SPECIAL_TAG definitions
#define C90_ASPACE_MASK     0x0000000f
#define C90_CONTROLLER_MASK 0x000000f0
#define C90_WIDTH_MASK      0x0000ff00
#define C90_ASPACE_SHIFT        (0)
#define C90_CONTROLLER_SHIFT    (4)
#define C90_WIDTH_SHIFT         (8)

#define C90_CONTROLLER_0    0x00000010
#define C90_CONTROLLER_1    0x00000020
#define C90_INTERLEAVED     (C90_CONTROLLER_0|C90_CONTROLLER_1)

#define C90_WIDTH_32        (32<<C90_WIDTH_SHIFT)
#define C90_WIDTH_64        (64<<C90_WIDTH_SHIFT)
#define C90_WIDTH_128       (128<<C90_WIDTH_SHIFT)

#define C90_ASPACE(x)       ((x&C90_ASPACE_MASK)>>C90_ASPACE_SHIFT)
#define C90_WIDTH(x)        ((x&C90_WIDTH_MASK)>>C90_WIDTH_SHIFT)

/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

typedef struct c90i_flash_internal_struct 
{
    VC90_REG_STRUCT_PTR reg0_ptr;
    VC90_REG_STRUCT_PTR reg1_ptr;
   void (_CODE_PTR_ flash_exec)(VC90_REG_STRUCT_PTR);
   uint_32 aspace_sectors[C90_ASPACE_COUNT];
} C90I_FLASH_INTERNAL_STRUCT, _PTR_ C90I_FLASH_INTERNAL_STRUCT_PTR;


/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

boolean c90i_flash_init (IO_FLASHX_STRUCT_PTR);
void    c90i_flash_deinit (IO_FLASHX_STRUCT_PTR);
boolean c90i_flash_sector_erase (IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size);
boolean c90i_flash_sector_program (IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr, _mem_size);
boolean c90i_flash_chip_erase (IO_FLASHX_STRUCT_PTR);
boolean c90i_flash_write_protect(IO_FLASHX_STRUCT_PTR, _mqx_uint);

#ifdef __cplusplus
}
#endif

#endif
