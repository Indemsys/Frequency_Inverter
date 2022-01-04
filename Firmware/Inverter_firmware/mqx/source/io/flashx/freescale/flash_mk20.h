/**HEADER********************************************************************
* 
* Copyright (c) 2010-2011 Freescale Semiconductor;
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
* $FileName: flash_mk20.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines for the internal 
*   flash driver.
*
*END************************************************************************/

#ifndef __flash_mk20_h__
#define __flash_mk20_h__

/* FlexNVM partition code values */
#define FLEXNVM_PART_CODE_DATA32_EE0        (0)
#define FLEXNVM_PART_CODE_DATA24_EE8        (1)
#define FLEXNVM_PART_CODE_DATA16_EE16       (2)
#define FLEXNVM_PART_CODE_DATA8_EE24        (9)
#define FLEXNVM_PART_CODE_DATA0_EE32        (3)
#define FLEXNVM_PART_CODE_NOPART            (0x0f)

/* FlexNVM EEPROM size values */
#define FLEXNVM_EE_SIZE_2048                (3)
#define FLEXNVM_EE_SIZE_1024                (4)
#define FLEXNVM_EE_SIZE_512                 (5)
#define FLEXNVM_EE_SIZE_256                 (6)
#define FLEXNVM_EE_SIZE_128                 (7)
#define FLEXNVM_EE_SIZE_64                  (8)
#define FLEXNVM_EE_SIZE_32                  (9)
#define FLEXNVM_EE_SIZE_0                   (0x0f)

/* K20D5: FlexNVM EEPROM Data Size Code 5:4 bits (FCCOB4[5:4]) */
#define FLEXNVM_EE_SPLIT                    (0x30)

/* K20D7: FlexNVM split values */
#define FLEXNVM_EE_SPLIT_1_7                (0)
#define FLEXNVM_EE_SPLIT_1_3                (0x10)
#define FLEXNVM_EE_SPLIT_1_1                (0x30)

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

pointer _bsp_get_ftfl_address(void);
void kinetis_flash_invalidate_cache(uint_32);

#ifdef __cplusplus
}
#endif

#endif
