#ifndef __sdcard_spi_h__
#define __sdcard_spi_h__
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
* $FileName: sdcard_spi.h$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototypes, defines, structure 
*   definitions private to the SD SPI communication.
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


#define SDCARD_SPI_RETRIES                 (3000)


#define SDCARD_SPI_START                   (0xFE)
#define SDCARD_SPI_MULTI_START             (0xFC)
#define SDCARD_SPI_MULTI_STOP              (0xFD)
#define SDCARD_SPI_IDLE                    (0xFF)
#define SDCARD_SPI_BUSY                    (0x00)
#define SDCARD_SPI_DATA_RESPONSE_MASK      (0x1F)
#define SDCARD_SPI_DATA_RESPONSE_OK        (0x05)
#define SDCARD_SPI_DATA_ERROR_MASK         (0x0F)
#define SDCARD_SPI_END_BIT                 (0x01)

#define SDCARD_SPI_IF_COND_VHS_MASK        (0x00000F00L)
#define SDCARD_SPI_IF_COND_VHS_27_36       (0x00000100L)

#define SDCARD_SPI_OCR_VOLTAGE_3_3V        (0x00300000L)
#define SDCARD_SPI_OCR_CCS                 (0x40000000L)
#define SDCARD_SPI_OCR_POWER_UP_STATUS     (0x80000000L)

#define SDCARD_SPI_ACMD41_HCS              (0x40000000L)


#define SDCARD_SPI_CMD0_GO_IDLE_STATE      ( 0 | 0x40)
#define SDCARD_SPI_CMD8_SEND_IF_COND       ( 8 | 0x40)
#define SDCARD_SPI_CMD9_SEND_CSD           ( 9 | 0x40)
#define SDCARD_SPI_CMD12_STOP              (12 | 0x40)
#define SDCARD_SPI_CMD13_SEND_STATUS       (13 | 0x40)
#define SDCARD_SPI_CMD16_SET_BLOCKLEN      (16 | 0x40)
#define SDCARD_SPI_CMD17_READ_BLOCK        (17 | 0x40)
#define SDCARD_SPI_CMD18_READ_MULTI_BLOCK  (18 | 0x40)
#define SDCARD_SPI_CMD24_WRITE_BLOCK       (24 | 0x40)
#define SDCARD_SPI_CMD25_WRITE_MULTI_BLOCK (25 | 0x40)
#define SDCARD_SPI_ACMD41_SEND_OP_COND     (41 | 0x40)
#define SDCARD_SPI_CMD55_APP_CMD           (55 | 0x40)
#define SDCARD_SPI_CMD58_READ_OCR          (58 | 0x40)
#define SDCARD_SPI_CMD59_CRC_ON            (59 | 0x40)
#define SDCARD_SPI_ACMD13_SEND_STATUS      SDCARD_SPI_CMD13_SEND_STATUS
#define SDCARD_SPI_ACMD22_SEND_NUM_WR_BLOCKS (22 | 0x40)


#define SDCARD_SPI_R1_IDLE_STATE           (1 << 0)
#define SDCARD_SPI_R1_ERASE_RESET          (1 << 1)
#define SDCARD_SPI_R1_ILLEGAL_CMD          (1 << 2)
#define SDCARD_SPI_R1_CRC_ERROR            (1 << 3)
#define SDCARD_SPI_R1_ERASE_SEQ            (1 << 4)
#define SDCARD_SPI_R1_ADDRESS_ERROR        (1 << 5)
#define SDCARD_SPI_R1_PARAM_ERROR          (1 << 6)
#define SDCARD_SPI_R1_START_MASK           (1 << 7)
#define SDCARD_SPI_R1_ERROR_MASK           (0x7E)


/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/* Internal functions to SD SPI communication */
#ifdef __cplusplus
extern "C" {
#endif


extern   boolean _io_sdcard_spi_init(MQX_FILE_PTR);
extern  _mqx_int _io_sdcard_spi_read_block(MQX_FILE_PTR, uchar_ptr, uint_32, uint_32);
extern  _mqx_int _io_sdcard_spi_write_block(MQX_FILE_PTR, uchar_ptr, uint_32, uint_32);


#ifdef __cplusplus
}
#endif


#endif
