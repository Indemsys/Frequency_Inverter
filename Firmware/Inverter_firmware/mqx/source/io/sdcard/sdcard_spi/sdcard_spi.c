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
* $FileName: sdcard_spi.c$
* $Version : 3.8.16.0$
* $Date    : Sep-25-2012$
*
* Comments:
*
*   This file contains the SD card driver functions.
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <sdcard.h>
#include <sdcard_prv.h>
#include <sdcard_spi.h>
#include <spi.h>


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_crc7
* Returned Value   : CRC value
* Comments         :
*    Optimized CRC7 code calculation.
*
*END*----------------------------------------------------------------------*/
static uint_32 _io_sdcard_crc7
(
    /* [IN] Initial value, maybe previous partial result */
    uint_32 crc,

    /* [IN] Input bit stream (big endian) for CRC calculation */
    uint_8 input[],

    /* [IN] Length of the bit stream in bytes */
    uint_32 len
)
{
    /* pre-calculated table for CRC7 polynomial 0x89 */
    static const uint_8 crc_table[] = {
        0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F,
        0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77
    };

    int index;

    while (len) {
        index = ((crc>>3) & 0x0f) ^ ((*input)>>4);
        crc = (crc<<4) ^ crc_table[index];

        index = ((crc>>3) & 0x0f) ^ ((*input)&0x0f);
        crc = (crc<<4) ^ crc_table[index];

        input++;
        len--;
    }

    return (crc & 0x7f);
}



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_crc16
* Returned Value   : CRC value
* Comments         :
*    Optimized CRC16 code calculation.
*
*END*----------------------------------------------------------------------*/

/* Optional faster CRC16 caclulation may be used - larger table footprint */
#ifndef SDCARD_SPI_FAST_CRC
#define SDCARD_SPI_FAST_CRC 0
#endif

#if SDCARD_SPI_FAST_CRC
static uint_32 _io_sdcard_crc16
(
    /* [IN] Initial value, maybe previous partial result */
    uint_32 crc,

    /* [IN] Input bit stream (big endian) for CRC calculation */
    uint_8 input[],

    /* [IN] Length of the bit stream in bytes */
    uint_32 len
)
{
    /* pre-calculated table for CRC16 polynomial 0x11021 */
    static const uint_16 crc_table[] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
        0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
        0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
        0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
        0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
        0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
        0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
        0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
        0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
        0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
        0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
        0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
        0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
        0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
        0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
        0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
        0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
        0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
        0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
        0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
        0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
        0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
    };

    int index;

    while (len > 0) {
        index = ((crc>>8) & 0xff) ^ (*input);
        crc = (crc<<8) ^ crc_table[index];

        input++;
        len--;
    }

    return (crc & 0xffff);
}

#else /* !SDCARD_SPI_FAST_CRC */

static uint_32 _io_sdcard_crc16
(
    /* [IN] Initial value, maybe previous partial result */
    uint_32 crc,

    /* [IN] Input bit stream (big endian) for CRC calculation */
    uint_8 input[],

    /* [IN] Length of the bit stream in bytes */
    uint_32 len
)
{
    /* pre-calculated table for CRC16 polynomial 0x11021 */
    static const uint_16 crc_table[] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
    };

    int index;

    while (len > 0) {
        index = ((crc>>12) & 0x0f) ^ ((*input)>>4);
        crc = (crc<<4) ^ crc_table[index];

        index = ((crc>>12) & 0x0f) ^ ((*input)&0x0f);
        crc = (crc<<4) ^ crc_table[index];

        input++;
        len--;
    }

    return (crc & 0xffff);
}
#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_read
* Returned Value   : number of bytes read
* Comments         :
*    Read wrapper that works on top of both polled and interrupt SPI drivers.
*
*END*----------------------------------------------------------------------*/
static uint_32 _io_sdcard_spi_read
(
    /* [OUT] Input buffer pointer */
    uchar_ptr    buffer,

    /* [IN] Size of buffer elements */
    uint_32      size,

    /* [IN] Number of elements to read */
    uint_32      number,

    /* [IN] Opened SPI driver handle */
    MQX_FILE_PTR spifd
)
{
    uint_32 count = 0;
    do
    {
        count += fread(buffer+count, size, number-count, spifd);
    } while (count < number);
    return count;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_write
* Returned Value   : number of bytes written
* Comments         :
*    Write wrapper that works on top of both polled and interrupt SPI drivers.
*
*END*----------------------------------------------------------------------*/
static uint_32 _io_sdcard_spi_write
(
    /* [IN] Output buffer pointer */
    uchar_ptr    buffer,

    /* [IN] Size of buffer elements */
    uint_32      size,

    /* [IN] Number of elements to write */
    uint_32      number,

    /* [IN] Opened SPI driver handle */
    MQX_FILE_PTR spifd
)
{
    uint_32 count = 0;
    do
    {
        count += fwrite (buffer + count, size, number - count, spifd);
    } while (count < number);
    return count;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_wait
* Returned Value   : success or failure
* Comments         :
*    Reads bytes until receiving a one not matching the given pattern.
*    Writes first non-matching byte to response buffer.
*
*END*----------------------------------------------------------------------*/
static boolean _io_sdcard_spi_wait
(
    /* [IN] SD card info */
    SDCARD_STRUCT_PTR sdcard_ptr,

    /* [IN] Pattern to skip, typically SDCARD_SPI_IDLE or SDCARD_SPI_BUSY */
    uint_8 skip,

    /* [OUT] First non-idle byte received */
    uint_8 *response
)
{
    uint_8  sdi;
    uint_32 timeout;

    /* wait for a non-matching byte */
    for (timeout = sdcard_ptr->TIMEOUT; timeout; timeout--)
    {
        sdi = SDCARD_SPI_IDLE;

        if (1 != _io_sdcard_spi_read(&sdi, 1, 1, sdcard_ptr->COM_DEVICE))
            return FALSE;

        if (sdi != skip)
            break;
    }

    /* check for timeout */
    if (0 == timeout)
        return FALSE;

    if (response)
        *response=sdi;

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_command
* Returned Value   : Success or failure
* Comments         :
*    Sends command, reads R1 response and optional extended response
*
*END*----------------------------------------------------------------------*/
static boolean _io_sdcard_spi_command
(
    /* [IN] SD card info */
    SDCARD_STRUCT_PTR sdcard_ptr,

    /* [OUT] command to send */
    uint_8 command,

    /* [OUT] parameter to the command */
    uint_32 param,

    /* [OUT] common response to all commands (R1) */
    uint_8  *r1,

    /* [OUT] extended response pointer, may be NULL */
    uint_32 *response,

    /* [IN] length of extended response in bytes (up to 4) */
    uint_32 resplen
)
{
    int i;
    uint_8  buffer[7];

    if (!_io_sdcard_spi_wait(sdcard_ptr, SDCARD_SPI_BUSY , NULL))
        return FALSE;

    buffer[0] = command;
    buffer[1] = (param >> 24) & 0xFF;
    buffer[2] = (param >> 16) & 0xFF;
    buffer[3] = (param >>  8) & 0xFF;
    buffer[4] =  param        & 0xFF;
    buffer[5] = (_io_sdcard_crc7(0, buffer, 5) << 1) | SDCARD_SPI_END_BIT;

    /* dummy byte, Ncr is min 1, speed up, also simplifies STOP command handling */
    buffer[6] = SDCARD_SPI_IDLE;

    if (sizeof(buffer) != _io_sdcard_spi_write(buffer, 1, sizeof(buffer), sdcard_ptr->COM_DEVICE))
        return FALSE;

    /* wait for first byte of the response (R1) */
    if (!_io_sdcard_spi_wait(sdcard_ptr, SDCARD_SPI_IDLE, r1))
        return FALSE;

    /* is the response valid? */
    if (*r1 & SDCARD_SPI_R1_START_MASK)
        return FALSE;

    /* is extended response is expected at all?*/
    if (0 == resplen)
        return TRUE;

    /* read the extended response to buffer */
    buffer[0] = SDCARD_SPI_IDLE;
    buffer[1] = SDCARD_SPI_IDLE;
    buffer[2] = SDCARD_SPI_IDLE;
    buffer[3] = SDCARD_SPI_IDLE;

    if (resplen != _io_sdcard_spi_read(buffer, 1, resplen, sdcard_ptr->COM_DEVICE))
        return FALSE;

    /* is extended response data required by the caller? */
    if (response == NULL)
        return TRUE;

    /* return the extended response as a single 32-bit value */
    *response = 0;
    for (i=0; i<resplen; i++)
    {
        *response <<= 8;
        *response |= buffer[i];
    }

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_read_data
* Returned Value   : success or failure
* Comments         :
*    Waits for data token and reads it into given buffer.
*
*END*----------------------------------------------------------------------*/
static boolean _io_sdcard_spi_read_data
(
    /* [IN] SD card info */
    SDCARD_STRUCT_PTR sdcard_ptr,

    /* [IN] Data to read*/
    uint_8 data[],

    /* [IN] Length of data */
    uint_32 data_len
)
{
    uint_8  tmp[2];
    uint_16 crc;

    if (!_io_sdcard_spi_wait(sdcard_ptr, SDCARD_SPI_IDLE, data))
        return FALSE;

    if (SDCARD_SPI_START != data[0])
        return FALSE;

    memset(data, SDCARD_SPI_IDLE, data_len);
    if (data_len != _io_sdcard_spi_read(data, 1, data_len, sdcard_ptr->COM_DEVICE))
        return FALSE;

    tmp[0] = tmp[1] = SDCARD_SPI_IDLE;
    if (2 != _io_sdcard_spi_read(tmp, 1, 2, sdcard_ptr->COM_DEVICE))
        return FALSE;

    crc = (tmp[0]<<8) | tmp[1];
    if (crc != _io_sdcard_crc16(0, data, data_len))
        return FALSE;

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_write_data
* Returned Value   : success or failure
* Comments         :
*    Sends data token, reads response and  and waits while the card is busy.
*
*END*----------------------------------------------------------------------*/
static boolean _io_sdcard_spi_write_data
(
    /* [IN] SD card info */
    SDCARD_STRUCT_PTR sdcard_ptr,

    /* [IN] Data to write*/
    uint_8 data[],

    /* [IN] Length of data */
    uint_32 data_len,

    /* [IN] Token header to be sent before the data  */
    uint_8  token,

    /* [OUT] Response to data token sent by the card */
    uint_8  *response
)
{
    uint_8  tmp[2];
    uint_16 crc;

    /* Calculate CRC */
    crc = _io_sdcard_crc16 (0, data, data_len);
    tmp[0] = crc>>8;
    tmp[1] = crc&0xff;

    /* Send token header */
    if (1 != _io_sdcard_spi_write(&token, 1, 1, sdcard_ptr->COM_DEVICE))
        return FALSE;

    /* Send the data */
    if (data_len != _io_sdcard_spi_write(data, 1, data_len, sdcard_ptr->COM_DEVICE))
        return FALSE;

    /* Send CRC */
    if (2 != _io_sdcard_spi_write(tmp, 1, 2, sdcard_ptr->COM_DEVICE))
        return FALSE;

    /* Wait for response and read it */
    if (!_io_sdcard_spi_wait(sdcard_ptr, SDCARD_SPI_IDLE, response))
        return FALSE;

    /* Wait while the card busy writing the data */
    if (!_io_sdcard_spi_wait(sdcard_ptr, SDCARD_SPI_BUSY, NULL))
        return FALSE;

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_write_stop
* Returned Value   : success or failure
* Comments         :
*    Sends data stop token and waits while the card is busy.
*
*END*----------------------------------------------------------------------*/
static boolean _io_sdcard_spi_write_stop
(
    /* [IN] SD card info */
    SDCARD_STRUCT_PTR sdcard_ptr
)
{
    uint_8  tmp[2];

    tmp[0] = SDCARD_SPI_MULTI_STOP;
    tmp[1] = SDCARD_SPI_IDLE; /* 8 dummy clocks */

    /* Send stop token and 8 dummy clocks */
    if (2 != _io_sdcard_spi_write(tmp, 1, 2, sdcard_ptr->COM_DEVICE))
        return FALSE;

    /* Wait while the card busy writing the data */
    if (!_io_sdcard_spi_wait(sdcard_ptr, SDCARD_SPI_BUSY, NULL))
        return FALSE;

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_init
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*    Initializes SPI communication, SD card itself and reads its parameters.
*
*END*----------------------------------------------------------------------*/
boolean _io_sdcard_spi_init
(
    /* [IN/OUT] SD card info */
    MQX_FILE_PTR fd_ptr
)
{
    uint_8               r1;
    uint_32              ifc;
    uint_32              ocr;
    uint_8               csd[16];

    uint_32              param;
    uint_32              baudrate;

    int                  i;

    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

    if ((NULL == sdcard_ptr) || (NULL == sdcard_ptr->COM_DEVICE) || (NULL == sdcard_ptr->INIT))
        return FALSE;

    sdcard_ptr->TIMEOUT = 0;
    sdcard_ptr->NUM_BLOCKS = 0;
    sdcard_ptr->ADDRESS = 0;
    sdcard_ptr->SDHC = FALSE;
    sdcard_ptr->VERSION2 = FALSE;

    /* communication setup */
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_CS, &sdcard_ptr->INIT->SIGNALS))
        return FALSE;

    /* Revert to default baudrate of the com device */
    ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_BAUD, NULL);

    /* Get the baudrate */
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_GET_BAUD, &baudrate))
        return FALSE;

    param = 8;
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_FRAMESIZE, &param))
        return FALSE;

    param = SPI_DEVICE_BIG_ENDIAN;
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_ENDIAN, &param))
        return FALSE;

    param = SPI_CLK_POL_PHA_MODE0;
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_MODE, &param))
        return FALSE;

    param = SPI_DEVICE_MASTER_MODE;
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
        return FALSE;

    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_DEVICE_ENABLE, NULL))
        return FALSE;


    /* set 400 kbps for max compatibility */
    param = 400000;
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_BAUD, &param))
        return FALSE;

    /* max timeout = 250 ms = 1 sec / (4 * 10 bits in transfer at baudrate) */
    sdcard_ptr->TIMEOUT = param / 40;


    /* disable CS for a while */
    param = 0;
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_CS, &param))
        return FALSE;

    /* send 80 dummy clocks without CS, CSD buffer used as temp */
    memset(csd, SDCARD_SPI_IDLE, 10);
    if (10 != _io_sdcard_spi_write(csd, 1, 10, sdcard_ptr->COM_DEVICE))
    {
        fflush (sdcard_ptr->COM_DEVICE);
        return FALSE;
    }

    /* return original CS signal */
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_CS, &sdcard_ptr->INIT->SIGNALS))
        return FALSE;


    /* send CMD0 = go idle */
    for (i = 0; i < SDCARD_SPI_RETRIES; i++)
    {
        if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD0_GO_IDLE_STATE, 0, &r1, NULL, 0))
            return FALSE;

        if (r1 == SDCARD_SPI_R1_IDLE_STATE)
            break;
    }

    if (i >= SDCARD_SPI_RETRIES)
        return FALSE;


    /* send CMD8 = test version */
    if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD8_SEND_IF_COND, SDCARD_SPI_IF_COND_VHS_27_36 | 0xAA, &r1, &ifc, 4))
        return FALSE;

    if (0 == (r1 & SDCARD_SPI_R1_ILLEGAL_CMD))
    {
        /* command supported, v2 card */
        sdcard_ptr->VERSION2 = TRUE;

        if (((ifc & 0xFF) != 0xAA) || ((ifc & SDCARD_SPI_IF_COND_VHS_MASK) != SDCARD_SPI_IF_COND_VHS_27_36))
            return FALSE;
    }


    /* send CMD58 = read OCR ... 3.3V */
    if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD58_READ_OCR, 0, &r1, &ocr, 4))
        return FALSE;

    if (0 == (r1 & SDCARD_SPI_R1_ILLEGAL_CMD))
    {
        if (r1 & SDCARD_SPI_R1_ERROR_MASK)
            return FALSE;

        if (SDCARD_SPI_OCR_VOLTAGE_3_3V != (ocr & SDCARD_SPI_OCR_VOLTAGE_3_3V))
            return FALSE;
    }


    /* send CMD55 + ACMD41 = initialize */
    for (i = 0; i < SDCARD_SPI_RETRIES; i++)
    {
        if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD55_APP_CMD, 0, &r1, NULL, 0))
            return FALSE;

        if (r1 & SDCARD_SPI_R1_ERROR_MASK)
            return FALSE;

        if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_ACMD41_SEND_OP_COND, sdcard_ptr->VERSION2 ? SDCARD_SPI_ACMD41_HCS : 0, &r1, NULL, 0))
            return FALSE;

        if (r1 & SDCARD_SPI_R1_ERROR_MASK)
            return FALSE;

        if (0 == (r1 & SDCARD_SPI_R1_IDLE_STATE))
            break;
    }

    if (i >= SDCARD_SPI_RETRIES)
        return FALSE;


    /* version 2 or later card */
    if (sdcard_ptr->VERSION2)
    {
        /* send CMD58 = read OCR to check CCS */
        if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD58_READ_OCR, 0, &r1, &ocr, 4))
            return FALSE;

        if (r1 & SDCARD_SPI_R1_ERROR_MASK)
            return FALSE;

        if (0 == (ocr & SDCARD_SPI_OCR_POWER_UP_STATUS))
            return FALSE;

        if (ocr & SDCARD_SPI_OCR_CCS)
            sdcard_ptr->SDHC = TRUE;
    }


    /* send CMD9 = get CSD */
    if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD9_SEND_CSD, 0, &r1, NULL, 0))
        return FALSE;

    if (r1 & SDCARD_SPI_R1_ERROR_MASK)
        return FALSE;

    if (!_io_sdcard_spi_read_data(sdcard_ptr, csd, 16))
        return FALSE;

    sdcard_ptr->NUM_BLOCKS = _io_sdcard_csd_capacity(csd);

    /* send CMD59 = CRC ON */
    if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD59_CRC_ON, 0x01, &r1, NULL, 0))
        return FALSE;

    if (r1 & SDCARD_SPI_R1_ERROR_MASK)
        return FALSE;


    /* send CMD16 = set block length to 512 */
    if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD16_SET_BLOCKLEN, 512, &r1, NULL, 0))
        return FALSE;

    if (r1 & SDCARD_SPI_R1_ERROR_MASK)
        return FALSE;


    param = _io_sdcard_csd_baudrate(csd);
    if (param > baudrate) param = baudrate;

    /* set baudrate */
    if (SPI_OK != ioctl (sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_BAUD, &param))
        return FALSE;

    /* max timeout = 250 ms = 1 sec / (4 * 10 bits in transfer at baudrate) */
    sdcard_ptr->TIMEOUT = param / 40;


    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_read_block
* Returned Value   : Number of sectors successfully read or negative error code
* Comments         :
*    Reads sectors from SD card starting with given index into given buffer.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_spi_read_block
(
    /* [IN] SD card info */
    MQX_FILE_PTR fd_ptr,

    /* [OUT] Buffer to fill with read 512 bytes */
    uchar_ptr buffer,

    /* [IN] Index of sector to read */
    uint_32   index,

    /* [IN] Number of sectrors to read */
    uint_32   count
)
{
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

    uint_8               command;
    uint_8               r1;

    _mqx_int             result;

    /* Sanity checks */
    if ((NULL == sdcard_ptr) || (NULL == sdcard_ptr->COM_DEVICE) || (NULL == sdcard_ptr->INIT) || (NULL == buffer) || (0 == count))
        return IO_ERROR;

    /* Set proper SPI chip select */
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_CS, &sdcard_ptr->INIT->SIGNALS))
        return IO_ERROR;

    /* Non-SDHC cards are byte addressed */
    if (!sdcard_ptr->SDHC)
        index <<= IO_SDCARD_BLOCK_SIZE_POWER;

    /* Select command according to requested number of blocks */
    if (count>1)
    {
        command = SDCARD_SPI_CMD18_READ_MULTI_BLOCK;
    }
    else
    {
        command = SDCARD_SPI_CMD17_READ_BLOCK;
    }

    /* Send command and check the response */
    if (!_io_sdcard_spi_command(sdcard_ptr, command, index, &r1, NULL, 0) || (r1 & SDCARD_SPI_R1_ERROR_MASK))
    {
        fflush (sdcard_ptr->COM_DEVICE);
        return IO_ERROR;
    }

    result = 0;
    while (result < count)
    {
        /* Read data */
        if (!_io_sdcard_spi_read_data(sdcard_ptr, buffer, IO_SDCARD_BLOCK_SIZE))
            break;

        buffer+=IO_SDCARD_BLOCK_SIZE;
        result++;
    }

    /* Send stop command if reading multiple sectors */
    if (count>1)
    {
        _io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD12_STOP, 0, &r1, NULL, 0);
    }

    /* Deassert CS */
    fflush (sdcard_ptr->COM_DEVICE);
    return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_spi_write_block
* Returned Value   : Number of sectors successfully written or negative error code
* Comments         :
*    Writes sectors starting with given index to SD card from given buffer.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_spi_write_block
(
    /* [IN] SD card info */
    MQX_FILE_PTR fd_ptr,

    /* [IN] Buffer with 512 bytes to write */
    uchar_ptr buffer,

    /* [IN] Index of sector to write */
    uint_32   index,

    /* [IN] Number of sectrors to write */
    uint_32   count
)
{
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

    uint_8               command;
    uint_8               token;
    uint_8               response;
    uint_8               r1;
    uint_32              r2;
    uint_8               tmp[4];

    _mqx_int             result;

    /* Sanity checks */
    if ((NULL == sdcard_ptr) || (NULL == sdcard_ptr->COM_DEVICE) || (NULL == sdcard_ptr->INIT) || (NULL == buffer) || (0 == count))
        return IO_ERROR;

    /* Set proper SPI chip select */
    if (SPI_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_SPI_SET_CS, &sdcard_ptr->INIT->SIGNALS))
        return IO_ERROR;

    /* Non-SDHC cards are byte addressed */
    if (!sdcard_ptr->SDHC)
        index <<= IO_SDCARD_BLOCK_SIZE_POWER;

    if (count>1)
    {
        command = SDCARD_SPI_CMD25_WRITE_MULTI_BLOCK;
        token = SDCARD_SPI_MULTI_START;
    }
    else
    {
        command = SDCARD_SPI_CMD24_WRITE_BLOCK;
        token = SDCARD_SPI_START;
    }

    /* Send command and check the response. Second response byte is dummy, just to generate clocks. */
    if (!_io_sdcard_spi_command(sdcard_ptr, command, index, &r1, NULL, 1) || (r1 & SDCARD_SPI_R1_ERROR_MASK))
    {
        fflush(sdcard_ptr->COM_DEVICE);
        return IO_ERROR;
    }

    result = 0;
    while (result < count)
    {
        /* Write data */
        if (!_io_sdcard_spi_write_data(sdcard_ptr, buffer, IO_SDCARD_BLOCK_SIZE, token, &response))
        {
            fflush(sdcard_ptr->COM_DEVICE);
            return IO_ERROR;
        }

        /* Check the response */
        if ((response & SDCARD_SPI_DATA_RESPONSE_MASK) != SDCARD_SPI_DATA_RESPONSE_OK)
        {
            result = IO_ERROR;
            break;
        }

        buffer+=IO_SDCARD_BLOCK_SIZE;
        result++;
    }

    /* Send stop token if writing multiple sectors */
    if (count>1)
    {
        if (!_io_sdcard_spi_write_stop(sdcard_ptr))
        {
            fflush(sdcard_ptr->COM_DEVICE);
            return IO_ERROR;
        }
    }

    /* Fetch status from card */
    if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD13_SEND_STATUS, 0, &r1, &r2, 1))
    {
        fflush(sdcard_ptr->COM_DEVICE);
        return IO_ERROR;
    }

    /* Check whether the operation was successful */
    if ((r1 & SDCARD_SPI_R1_ERROR_MASK) || r2)
    {
        if (count>1)
        {
            /* Retrieve number of successfully written blocks */
            if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_CMD55_APP_CMD, 0, &r1, NULL, 0) || (r1 & SDCARD_SPI_R1_ERROR_MASK))
            {
                fflush(sdcard_ptr->COM_DEVICE);
                return IO_ERROR;
            }

            if (!_io_sdcard_spi_command(sdcard_ptr, SDCARD_SPI_ACMD22_SEND_NUM_WR_BLOCKS, 0, &r1, NULL, 0) || (r1 & SDCARD_SPI_R1_ERROR_MASK))
            {
                fflush(sdcard_ptr->COM_DEVICE);
                return IO_ERROR;
            }

            if (!_io_sdcard_spi_read_data(sdcard_ptr, tmp, 4))
            {
                fflush(sdcard_ptr->COM_DEVICE);
                return IO_ERROR;
            }

            result = (tmp[0]<<24) | (tmp[1]<<16) | (tmp[2]<<8) | (tmp[3]);
            if (result > count) result = IO_ERROR; /* Received invalid number */
        }
        else
        {
            result = 0;
        }
    }

    /* Deassert CS */
    fflush(sdcard_ptr->COM_DEVICE);
    return result;
}

/* EOF */
