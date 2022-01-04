/**HEADER***********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
********************************************************************************
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
********************************************************************************
*
* $FileName: nfc.c$
* $Version : 3.8.16.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions to interface NAND Flash Controller module.
*
*END***************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "nandflash.h"
#include "nandflashprv.h"
#include "nfc.h"

/* NFC ECC status word */
#define NFC_ECC_STATUS_CORFAIL       (0x80)
#define NFC_ECC_STATUS_ERROR_COUNT   (0x3F)
#define NFC_ECC_STATUS_OFFSET        (0x0C)

uint_32 nfc_swap_addr1, nfc_swap_addr2;

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : nfc_init
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_IMPROPER_ECC_SIZE
*                    NANDFLASHERR_INFO_STRUC_MISSING
*                    MQX_OUT_OF_MEMORY
* Comments         :
*    This function initializes the driver.
*
*END*-------------------------------------------------------------------------*/

uint_32 nfc_init
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr
   )
{ /* Body */

   uint_32 result = NANDFLASHERR_NO_ERROR;
   NFC_MemMapPtr nfc_ptr;
   uint_32 nand_id, block_size_kB, density, num_ecc_bytes;
   NANDFLASH_INFO_STRUCT_PTR nand_info_ptr;

   /* Initialize NFC I/O Pins */
   nfc_io_init();

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   /* Create NANDFLASH_INFO_STRUCT based on NAND ID read from the NAND Flash,
      if not defined before manually */
   if((nandflash_ptr->NANDFLASH_INFO_PTR) == NULL)
   {
       nand_info_ptr = (NANDFLASH_INFO_STRUCT_PTR)_mem_alloc_system_zero(
                       (_mem_size)sizeof(NANDFLASH_INFO_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
       if(nand_info_ptr == NULL)
       {
          return((uint_32)MQX_OUT_OF_MEMORY);
       } /* Endif */
#endif

       if (((*nandflash_ptr->IOCTL)(nandflash_ptr,NANDFLASH_IOCTL_GET_ID,&nand_id)) == NANDFLASHERR_NO_ERROR)
       {
           nand_info_ptr->PHY_PAGE_SIZE =
             (_mem_size)(((nand_id & NANDFLASH_ID_PAGE_SIZE_MASK) + 1)*1024);

           if((nand_id & NANDFLASH_ID_SPARE_BYTES_NUM_MASK)>>2 == 1)
           {
               nand_info_ptr->SPARE_AREA_SIZE = (_mem_size)64;
           }

           if(nand_id & NANDFLASH_ID_BLOCK_SIZE_MASK)
           {
               nand_info_ptr->BLOCK_SIZE =
                 (_mem_size)((nand_id & NANDFLASH_ID_BLOCK_SIZE_MASK)*8192);
           }
           else
           {
               nand_info_ptr->BLOCK_SIZE = 65536;
           }

           block_size_kB = (nand_info_ptr->BLOCK_SIZE)/1024;
           density = (nand_id & NANDFLASH_ID_DENSITY_MASK)>>16;
           switch(density)
           {
               case NANDFLASH_DENSITY_1Gb:
                   /* nand_info_ptr->NUM_BLOCKS = 1024/8*1024/block_size_kB; */
                   nand_info_ptr->NUM_BLOCKS = 131072/block_size_kB;
                   break;

               case NANDFLASH_DENSITY_2Gb:
                   nand_info_ptr->NUM_BLOCKS = 2*131072/block_size_kB;
                   break;

               case NANDFLASH_DENSITY_4Gb:
                   nand_info_ptr->NUM_BLOCKS = 4*131072/block_size_kB;
                   break;

               case NANDFLASH_DENSITY_8Gb:
                   nand_info_ptr->NUM_BLOCKS = 8*131072/block_size_kB;
                   break;

               case NANDFLASH_DENSITY_16Gb:
                   nand_info_ptr->NUM_BLOCKS = 16*131072/block_size_kB;
                   break;

               default:
                   break;
           }

           nand_info_ptr->WIDTH =
             (_mqx_uint)(8 + ((nand_id & NANDFLASH_ID_WIDTH_MASK)>>3));

           nandflash_ptr->NANDFLASH_INFO_PTR = nand_info_ptr;
           nandflash_ptr->NUM_VIRTUAL_PAGES  =
             ((nand_info_ptr->BLOCK_SIZE)/(nandflash_ptr->VIRTUAL_PAGE_SIZE))*(nand_info_ptr->NUM_BLOCKS);
           nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO =
             (nand_info_ptr->PHY_PAGE_SIZE)/(nandflash_ptr->VIRTUAL_PAGE_SIZE);
       }
       else
       {
           return ((uint_32)NANDFLASHERR_INFO_STRUC_MISSING);
       }
   }

   /* Set the CFG register */
   nfc_ptr->CFG  = NFC_CFG_STOPWERR_MASK |
                   NFC_CFG_ECCAD((nandflash_ptr->VIRTUAL_PAGE_SIZE / 8) + 1) |
                   NFC_CFG_ECCSRAM_MASK |
                   NFC_CFG_IDCNT(5) |
                   NFC_CFG_TIMEOUT(6) |
                   NFC_CFG_PAGECNT(1);

   if((nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH) == 16)
       nfc_ptr->CFG |= NFC_CFG_BITWIDTH_MASK;
   else if((nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH) == 8)
       nfc_ptr->CFG &= ~NFC_CFG_BITWIDTH_MASK;

   switch(nandflash_ptr->ECC_SIZE)
   {
       case 0:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(0);
          break;
       case 4:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(1);
          break;
       case 6:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(2);
          break;
       case 8:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(3);
          break;
       case 12:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(4);
          break;
       case 16:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(5);
          break;
       case 24:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(6);
          break;
       case 32:
          nfc_ptr->CFG |= NFC_CFG_ECCMODE(7);
          break;
       default:
          result = NANDFLASHERR_IMPROPER_ECC_SIZE;
          break;
   }

   /* Auto-increment of row address is not enabled */
   nfc_ptr->RAI  = 0;

   /* Set both addresses in the SWAP register to the same value equal to
      the possition of the bad block marker within the physical page, mapped
      to the virtual page */
   num_ecc_bytes = NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(nandflash_ptr->ECC_SIZE);
   nfc_swap_addr1 = ((nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE)-
                     ((nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO - 1) *
                     (nandflash_ptr->VIRTUAL_PAGE_SIZE + num_ecc_bytes)))/8;
   nfc_swap_addr2 = (nandflash_ptr->VIRTUAL_PAGE_SIZE)/8;

   return(result);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_deinit
* Returned Value   :
* Comments         :
*    This function de-initializes the driver.
*
*END*----------------------------------------------------------------------*/

void nfc_deinit
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr
   )
{ /* Body */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_erase_flash
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_ERASE_FAILED
* Comments         :
*    This function erases the whole NAND Flash using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_erase_flash
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr
   )
{ /* Body */

   uint_32 count;

   for (count = 0; count < (nandflash_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS); count++)
   {
       if (NANDFLASHERR_NO_ERROR != nfc_erase_block(nandflash_ptr, count, FALSE))
           return NANDFLASHERR_ERASE_FAILED;
   }
   return NANDFLASHERR_NO_ERROR;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_erase_block
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_ERASE_FAILED or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function erases the defined block of NAND Flash
*    using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_erase_block
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr,

      /* [IN] the block to erase */
      uint_32                 block_number,

      /* [IN] TRUE = force block erase in case the block is marked as bad */
      boolean                 force_flag
   )
{ /* Body */
   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count;

   NFC_MemMapPtr  nfc_ptr;

   /* Do not erase block if force_flag is zero and the block is marked as bad */
   if((!force_flag) &&
      (NANDFLASHERR_BLOCK_NOT_BAD != nfc_check_block(nandflash_ptr, block_number)))
   {
       return NANDFLASHERR_ERASE_FAILED;
   }

   if (nandflash_ptr->WRITE_PROTECT) {
      (*nandflash_ptr->WRITE_PROTECT)(nandflash_ptr, FALSE);
   }/* Endif */

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   nfc_ptr->CMD1  = NFC_CMD1_BYTE2(NANDFLASH_CMD_BLOCK_ERASE_CYCLE2) |
                    NFC_CMD1_BYTE3(NANDFLASH_CMD_READ_STATUS);
   nfc_ptr->CMD2  = NFC_CMD2_BYTE1(NANDFLASH_CMD_BLOCK_ERASE_CYCLE1) |
                    NFC_CMD2_CODE(0x4ED8);
   nfc_ptr->RAR   = NFC_RAR_CS0_MASK |
                    NFC_RAR_RB0_MASK |
                    ((nandflash_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE /
                      nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE * block_number) & 0xFFFFFF);
   nfc_ptr->ISR  |= (NFC_ISR_IDLECLR_MASK | NFC_ISR_IDLEEN_MASK |
                     NFC_ISR_DONECLR_MASK | NFC_ISR_DONEEN_MASK);

   /* Start command execution */
   nfc_ptr->CMD2 |= NFC_CMD2_BUSY_START_MASK;

   for (count = 0; count <= MAX_WAIT_COMMAND; count++)
   {
       if (nfc_ptr->ISR & NFC_ISR_DONE_MASK)
       {
           if (nfc_ptr->SR2 & NANDFLASH_STATUS_ERR)
               result = NANDFLASHERR_ERASE_FAILED;
           else
               result = NANDFLASHERR_NO_ERROR;
           break;
       }
   }

   if (nandflash_ptr->WRITE_PROTECT) {
      (*nandflash_ptr->WRITE_PROTECT)(nandflash_ptr, TRUE);
   }/* Endif */
   return(result);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_read_page
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_ECC_FAILED or
*                    NANDFLASHERR_ECC_CORRECTED or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function reads one or several pages of NAND Flash
*    using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_read_page
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr,

      /* [OUT} where to copy data to */
      uchar_ptr               to_ptr,

      /* [IN] the page to read */
      uint_32                 page_number,

      /* [IN] the amount of pages to read */
      uint_32                 page_count
   )
{ /* Body */
   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count1, count2, count3;
   uint_32 output_offset = 0;
   boolean ecc_corrected = FALSE, swap = FALSE;
   uint_32 row, col, num_ecc_bytes;

   NFC_MemMapPtr  nfc_ptr;

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   for (count1 = page_number; count1 < (page_number + page_count); count1++)
   {
     num_ecc_bytes = NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(nandflash_ptr->ECC_SIZE);
     row = count1/nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO;
     col = (count1 - (row * nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO))*
           (nandflash_ptr->VIRTUAL_PAGE_SIZE + num_ecc_bytes);
     /* Is the bad block byte(s) about to be re-written? */
     count2 = count1 % ((nandflash_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE / nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE) * nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO);
     swap = ((count2 == (nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO - 1)) ||
             (count2 == (2*nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO - 1)));

     if(nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16)
        col = col/2;

     nfc_ptr->CMD1 = NFC_CMD1_BYTE2(NANDFLASH_CMD_PAGE_READ_CYCLE2);
     nfc_ptr->CMD2 = NFC_CMD2_BYTE1(NANDFLASH_CMD_PAGE_READ_CYCLE1) |
                     NFC_CMD2_CODE(0x7EE0) |
                     NFC_CMD2_BUFNO(1);
     nfc_ptr->CAR  = col & 0xFFFF;
     nfc_ptr->RAR  = NFC_RAR_CS0_MASK |
                     NFC_RAR_RB0_MASK |
                     (row & 0xFFFFFF);
     nfc_ptr->SECSZ  = (nandflash_ptr->VIRTUAL_PAGE_SIZE)+num_ecc_bytes;

     /* For 16-bit data width flash devices, only odd SIZE is supported */
     if((nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16) && !(nfc_ptr->SECSZ % 2))
       nfc_ptr->SECSZ  += 1;

     /* If the last virtual page of the first or the second physical page
        is about to be read the virtual page needs to be enlarged
        by 8 bytes and swapping switched on due to the bad block marking */
     if(swap)
     {
       nfc_ptr->SWAP  = NFC_SWAP_ADDR1(nfc_swap_addr1) |
                        NFC_SWAP_ADDR2(nfc_swap_addr2);
       nfc_ptr->SECSZ  += 8;
     }
     else
     {
       nfc_ptr->SWAP  = NFC_SWAP_ADDR1(0x7FF) |
                        NFC_SWAP_ADDR2(0x7FF);
     }

     nfc_ptr->ISR  |= (NFC_ISR_DONECLR_MASK |
                       NFC_ISR_DONEEN_MASK |
                       NFC_ISR_IDLECLR_MASK |
                       NFC_ISR_IDLEEN_MASK);

     /* Start command execution */
     nfc_ptr->CMD2 |= NFC_CMD2_BUSY_START_MASK;

     for (count2 = 0; count2 <= MAX_WAIT_COMMAND; count2++)
     {
       if (nfc_ptr->ISR & NFC_ISR_IDLE_MASK)
       {

         if ( nandflash_ptr->ECC_SIZE == 0)
         {
           if (nfc_ptr->SR2 & NANDFLASH_STATUS_ERR)
           {
             return(NANDFLASHERR_ECC_FAILED);
           }
         }
         else
         {
           /* Check ECC status word */
           if (*(uint_32*)&NFC_SRAM_B1_REG(nfc_ptr, nandflash_ptr->VIRTUAL_PAGE_SIZE + NFC_ECC_STATUS_OFFSET) & NFC_ECC_STATUS_CORFAIL)
           {
             return(NANDFLASHERR_ECC_FAILED);
           }
           else
           {
             if (*(uint_32*)&NFC_SRAM_B1_REG(nfc_ptr, nandflash_ptr->VIRTUAL_PAGE_SIZE + NFC_ECC_STATUS_OFFSET) & NFC_ECC_STATUS_ERROR_COUNT)
             {
               ecc_corrected = TRUE;
             }
           }
         }

         for(count3 = 0; count3 < (nandflash_ptr->VIRTUAL_PAGE_SIZE); count3++)
         {
            *(to_ptr + output_offset) = NFC_SRAM_B1_REG(nfc_ptr, count3);
            output_offset++;
         }
         result = NANDFLASHERR_NO_ERROR;
         break;
       }
     }
   }
   if(ecc_corrected)
      result = NANDFLASHERR_ECC_CORRECTED;
   return(result);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_write_page
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_WRITE_FAILED or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function writes one or several pages of NAND Flash
*    using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_write_page
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr,

      /* [IN] where to copy data from */
      uchar_ptr               from_ptr,

      /* [IN] the first page to write */
      uint_32                 page_number,

      /* [IN] the amount of pages to write */
      uint_32                 page_count
   )
{ /* Body */
   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count1, count2;
   uint_32 row, col, num_ecc_bytes;
   boolean swap = FALSE;

   NFC_MemMapPtr  nfc_ptr;

   if (nandflash_ptr->WRITE_PROTECT) {
      (*nandflash_ptr->WRITE_PROTECT)(nandflash_ptr, FALSE);
   }/* Endif */

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   for (count1 = page_number; count1 < (page_number + page_count); count1++)
   {
     num_ecc_bytes = NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(nandflash_ptr->ECC_SIZE);
     row = count1/nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO;
     col = (count1 - (row * nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO))*
           (nandflash_ptr->VIRTUAL_PAGE_SIZE + num_ecc_bytes);

     /* If the last virtual page of the first or the second physical page
        is about to be written the swapping needs to be switched on due
        to the bad block marking */
     count2 = count1 % ((nandflash_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE / nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE) * nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO);
     swap = ((count2 == (nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO - 1)) ||
             (count2 == (2*nandflash_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO - 1)));
     if(swap)
     {
       nfc_ptr->SWAP  = NFC_SWAP_ADDR1(nfc_swap_addr1) |
                        NFC_SWAP_ADDR2(nfc_swap_addr2);
     }
     else
     {
       nfc_ptr->SWAP  = NFC_SWAP_ADDR1(0x7FF) |
                        NFC_SWAP_ADDR2(0x7FF);
     }

     if(nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16)
        col = col/2;

     /* Copy one virtual page into the SRAM buffer #0 */
     for ( count2 = 0; count2 < (nandflash_ptr->VIRTUAL_PAGE_SIZE); count2++ )
     {
        NFC_SRAM_B0_REG(nfc_ptr, count2) = *(from_ptr + count2);
     }

     nfc_ptr->CMD1 = NFC_CMD1_BYTE2(NANDFLASH_CMD_PAGE_PROGRAM_CYCLE2) |
                     NFC_CMD1_BYTE3(NANDFLASH_CMD_READ_STATUS);
     nfc_ptr->CMD2 = NFC_CMD2_BYTE1(NANDFLASH_CMD_PAGE_PROGRAM_CYCLE1) |
                     NFC_CMD2_CODE(0x7FD8) |
                     NFC_CMD2_BUFNO(0);
     nfc_ptr->CAR  = col & 0xFFFF;
     nfc_ptr->RAR  = NFC_RAR_CS0_MASK |
                     NFC_RAR_RB0_MASK |
                     (row & 0xFFFFFF);
     nfc_ptr->SECSZ  = (nandflash_ptr->VIRTUAL_PAGE_SIZE)+num_ecc_bytes;

     /* For 16-bit data width flash devices, only odd SIZE is supported */
     if((nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16) && !(nfc_ptr->SECSZ % 2))
       nfc_ptr->SECSZ  += 1;

     /* If the last virtual page of the first or the second physical page
        is about to be written the bad block marking (negative) has to be
        inserted and the virtual page needs to be enlarged by 8 bytes */
     if(swap)
     {
       *(uint_32*)&NFC_SRAM_B0_REG(nfc_ptr, (nfc_swap_addr2 * 8)) = 0xFFFF0000;
       nfc_ptr->SECSZ  += 8;
     }

     nfc_ptr->ISR  |= (NFC_ISR_DONECLR_MASK |
                       NFC_ISR_DONEEN_MASK  |
                       NFC_ISR_IDLECLR_MASK |
                       NFC_ISR_IDLEEN_MASK);

     /* Start command execution */
     nfc_ptr->CMD2 |= NFC_CMD2_BUSY_START_MASK;

     for (count2 = 0; count2 <= MAX_WAIT_COMMAND; count2++)
     {
       if (nfc_ptr->ISR & NFC_ISR_IDLE_MASK)
       {
          if (nfc_ptr->SR2 & NANDFLASH_STATUS_ERR)
          {
             result = NANDFLASHERR_WRITE_FAILED;
             goto exit;
          }
          else
          {
             result = NANDFLASHERR_NO_ERROR;
          }
          break;
       }
     }
     from_ptr += nandflash_ptr->VIRTUAL_PAGE_SIZE;
   }

exit:
   if (nandflash_ptr->WRITE_PROTECT) {
      (*nandflash_ptr->WRITE_PROTECT)(nandflash_ptr, TRUE);
   }/* Endif */
   return(result);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_reset
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function resets the NAND Flash using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_reset
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr
   )
{ /* Body */

   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count;
   NFC_MemMapPtr  nfc_ptr;

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   nfc_ptr->CMD2  =  NFC_CMD2_BYTE1(NANDFLASH_CMD_RESET) |
                     NFC_CMD2_CODE(0x4002);
   nfc_ptr->ISR  |= (NFC_ISR_IDLECLR_MASK | NFC_ISR_IDLEEN_MASK |
                     NFC_ISR_DONECLR_MASK | NFC_ISR_DONEEN_MASK);

   /* Start command execution */
   nfc_ptr->CMD2 |=  NFC_CMD2_BUSY_START_MASK;

   for (count = 0; count <= MAX_WAIT_COMMAND; count++)
   {
       if (nfc_ptr->ISR & NFC_ISR_DONE_MASK)
       {
           result = NANDFLASHERR_NO_ERROR;
           break;
       }
   }
   return result;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_check_block
* Returned Value   : NANDFLASHERR_BLOCK_NOT_BAD or
*                    NANDFLASHERR_BLOCK_BAD or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function checks if the defined NAND Flash block is bad or not
*    using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_check_block
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr,

      /* [IN] block to check */
      uint_32                 block_number
   )
{ /* Body */
   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count, cfg_bck, col;

   NFC_MemMapPtr  nfc_ptr;

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   /* Set the ECCMODE to 0 - ECC bypass */
   cfg_bck = nfc_ptr->CFG;
   nfc_ptr->CFG &= ~(NFC_CFG_ECCMODE(7));

   /* Reset the swap register */
   nfc_ptr->SWAP  = NFC_SWAP_ADDR1(0x7FF) |
                    NFC_SWAP_ADDR2(0x7FF);

   col = nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE;
   if(nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16)
      col = col/2;

   nfc_ptr->CMD1 = NFC_CMD1_BYTE2(NANDFLASH_CMD_PAGE_READ_CYCLE2);
   nfc_ptr->CMD2 = NFC_CMD2_BYTE1(NANDFLASH_CMD_PAGE_READ_CYCLE1) |
                   NFC_CMD2_CODE(0x7EE0) |
                   NFC_CMD2_BUFNO(1);
   nfc_ptr->CAR  = col & 0xFFFF;
   nfc_ptr->RAR  = NFC_RAR_CS0_MASK |
                   NFC_RAR_RB0_MASK |
                   ((nandflash_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE /
                     nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE * block_number) & 0xFFFFFF);
   nfc_ptr->SECSZ  = 3;
   nfc_ptr->ISR  |= (NFC_ISR_DONECLR_MASK |
                     NFC_ISR_DONEEN_MASK  |
                     NFC_ISR_IDLECLR_MASK |
                     NFC_ISR_IDLEEN_MASK);

   /* Start command execution */
   nfc_ptr->CMD2 |= NFC_CMD2_BUSY_START_MASK;

   for (count = 0; count <= MAX_WAIT_COMMAND; count++)
   {
     if (nfc_ptr->ISR & NFC_ISR_IDLE_MASK)
     {
       if(!( (*(uint_32 *)&(NFC_SRAM_B1_REG(nfc_ptr, 0)) & 0xFF000000)))
       {
         result = NANDFLASHERR_BLOCK_BAD;
       }
       else
       {
         if(  (nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16)
            &&(!(*(uint_32 *)&NFC_SRAM_B1_REG(nfc_ptr, 0) & 0x00FF0000))
         )
         {
           result = NANDFLASHERR_BLOCK_BAD;
         }
         else
         {
           result = NANDFLASHERR_BLOCK_NOT_BAD;
         }
       }
       break;
     }
   }

   /* Set the ECCMODE back */
   nfc_ptr->CFG = cfg_bck;

   return(result);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_mark_block_as_bad
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_WRITE_FAILED or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function marks the defined NAND Flash block as bad
*    using the NFC module (writing 0x00 to the first spare area byte).
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_mark_block_as_bad
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr,

      /* [IN] block to mark as bad */
      uint_32                 block_number
   )
{ /* Body */

   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count, cfg_bck, col;

   NFC_MemMapPtr  nfc_ptr;

   nfc_erase_block(nandflash_ptr, block_number, FALSE);

   if (nandflash_ptr->WRITE_PROTECT) {
      (*nandflash_ptr->WRITE_PROTECT)(nandflash_ptr, FALSE);
   }/* Endif */

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   /* Set the ECCMODE to 0 - ECC bypass */
   cfg_bck = nfc_ptr->CFG;
   nfc_ptr->CFG &= ~(NFC_CFG_ECCMODE(7));

   col = nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE;
   if(nandflash_ptr->NANDFLASH_INFO_PTR->WIDTH == 16)
      col = col/2;

   /* Reset the swap register */
   nfc_ptr->SWAP  = NFC_SWAP_ADDR1(0x7FF) |
                    NFC_SWAP_ADDR2(0x7FF);

   /* Prepare bad block bytes in the SRAM buffer #0,
      no matter if the NAND Flash width is 8 or 16 */
   *(uint_32 *)&NFC_SRAM_B0_REG(nfc_ptr, 0) = 0;

   nfc_ptr->CMD1 = NFC_CMD1_BYTE2(NANDFLASH_CMD_PAGE_PROGRAM_CYCLE2) |
                   NFC_CMD1_BYTE3(NANDFLASH_CMD_READ_STATUS);
   nfc_ptr->CMD2 = NFC_CMD2_BYTE1(NANDFLASH_CMD_PAGE_PROGRAM_CYCLE1) |
                   NFC_CMD2_CODE(0x7FD8) |
                   NFC_CMD2_BUFNO(0);
   nfc_ptr->CAR  = col & 0xFFFF;
   nfc_ptr->RAR  = NFC_RAR_CS0_MASK |
                   NFC_RAR_RB0_MASK |
                   ((nandflash_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE /
                     nandflash_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE * block_number) & 0xFFFFFF);
   nfc_ptr->SECSZ  = 3;

   nfc_ptr->ISR  |= (NFC_ISR_DONECLR_MASK |
                     NFC_ISR_DONEEN_MASK  |
                     NFC_ISR_IDLECLR_MASK |
                     NFC_ISR_IDLEEN_MASK);

   /* Start command execution */
   nfc_ptr->CMD2 |= NFC_CMD2_BUSY_START_MASK;

   for (count = 0; count <= MAX_WAIT_COMMAND; count++)
   {
     if (nfc_ptr->ISR & NFC_ISR_IDLE_MASK)
     {
        if (nfc_ptr->SR2 & NANDFLASH_STATUS_ERR)
        {
           result = NANDFLASHERR_WRITE_FAILED;
        }
        else
        {
           result = NANDFLASHERR_NO_ERROR;
        }
        break;
     }
   }

   if (nandflash_ptr->WRITE_PROTECT) {
      (*nandflash_ptr->WRITE_PROTECT)(nandflash_ptr, TRUE);
   }/* Endif */

   /* Set the ECCMODE back */
   nfc_ptr->CFG = cfg_bck;

   return(result);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_read_ID
* Returned Value   : NANDFLASHERR_NO_ERROR or
*                    NANDFLASHERR_TIMEOUT
* Comments         :
*    This function reads the ID from the NAND Flash
*    using the NFC module.
*
*END*----------------------------------------------------------------------*/

uint_32 nfc_read_ID
   (
      /* [IN] the NAND flash information */
      IO_NANDFLASH_STRUCT_PTR nandflash_ptr,

      /* [IN] where to store ID */
      uchar_ptr               to_ptr,

      /* [IN] the amount of ID bytes */
      _mem_size               size
   )
{ /* Body */

   uint_32 result = NANDFLASHERR_TIMEOUT;
   uint_32 count1, count2;
   NFC_MemMapPtr  nfc_ptr;

   /* Get the pointer to nfc registers structure */
   nfc_ptr = _bsp_get_nfc_address();

   nfc_ptr->CMD2  =  NFC_CMD2_BYTE1(NANDFLASH_CMD_READ_ID) |
                     NFC_CMD2_CODE(0x4804);
   nfc_ptr->ISR  |= (NFC_ISR_IDLECLR_MASK | NFC_ISR_IDLEEN_MASK |
                     NFC_ISR_DONECLR_MASK | NFC_ISR_DONEEN_MASK);

   /* Start command execution */
   nfc_ptr->CMD2 |=  NFC_CMD2_BUSY_START_MASK;

   for (count1 = 0; count1 <= MAX_WAIT_COMMAND; count1++)
   {
       if (nfc_ptr->ISR & NFC_ISR_DONE_MASK)
       {
           for (count2 = 0; count2 < size; count2++)
           {
             to_ptr[count2] = *(uchar_ptr)((uint_32)&nfc_ptr->SR1 + count2);
           }
           result = NANDFLASHERR_NO_ERROR;
           break;
       }
   }
   return result;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : nfc_ioctl
* Returned Value   : TRUE if successful
* Comments         :
*    nfc ioctl function
*
*END*----------------------------------------------------------------------*/
_mqx_int nfc_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_NANDFLASH_STRUCT_PTR handle_ptr,

      /* [IN] the ioctl command */
      _mqx_uint               cmd,

      /* [IN] the ioctl parameters */
      pointer                 param_ptr
   )
{ /* Body */
    _mqx_uint result = MQX_OK;
    switch(cmd) {
        case NANDFLASH_IOCTL_GET_ID:
            /* Get the NAND Flash ID */
            if ((nfc_read_ID(handle_ptr, param_ptr, 4)) == NANDFLASHERR_NO_ERROR)
            {
                result = MQX_OK;
            }
            else
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            }
            break;
        default:
            break;
    }
    return result;
}

/* EOF */
