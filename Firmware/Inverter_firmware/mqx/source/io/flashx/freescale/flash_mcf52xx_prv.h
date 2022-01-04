#ifndef __INT_FLASH_MCF52XX_PRV_H__
#define __INT_FLASH_MCF52XX_PRV_H__
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
* $FileName: flash_mcf52xx_prv.h$
* $Version : 3.8.1.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions prototype, defines for the internal 
*   flash driver
*
*END************************************************************************/

#define CFM_IPS_FLASH_ADDR              0x44000000

/* address in memory map to write in FLASH */
#define FLASH_START_ADDRESS  CFM_IPS_FLASH_ADDR

/* End address for region of flash */
#define FLASH_END_ADDRESS   (vuint32)&__FLASH_SIZE+0x44000000

typedef struct internal_52xx_struct 
{
    vchar_ptr  cfm_ptr;  /* pointer to CFM structure */
    char_ptr   flash_execute_code_ptr; /* pointer to flash write and erase code */
        
} MCF52XX_FLASH_INTERNAL_STRUCT, _PTR_ MCF52XX_FLASH_INTERNAL_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

void    _mcf52xx_set_cfmclkd();

boolean mcf52xx_flash_init(IO_FLASHX_STRUCT_PTR);
void mcf52xx_flash_deinit(IO_FLASHX_STRUCT_PTR);
boolean mcf52xx_flash_erase_sector(IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size);
boolean mcf52xx_flash_write_sector(IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr, _mem_size);

#ifdef __cplusplus
}
#endif

#endif /* __INT_FLASH_MCF52XX_PRV_H__ */
