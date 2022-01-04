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
* $FileName: sai.h$
* $Version : 3.8.3.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

/*
** TODO:
** 1. Extend driver API, so it supports clock hooking combinations RX0->TX0, 
**    TX0->RX1 etc and bit clock swapping between TX and RX.
** 2. Separate channel selection for TX and RX - Extend init and info struct.
**
*/

#ifndef __SAI_H__
#define __SAI_H__

#include <ioctl.h>

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* Enable floating point operations in I2S */
#ifndef I2S_USE_FLOAT
#define I2S_USE_FLOAT   0
#endif

#define I2S_CHAR_BIT    (0x08)

/*
** SAI Clock sources
*/
#define I2S_CLK_INT     (0x00)
#define I2S_CLK_EXT     (0x01)

/*
** SAI Bus Modes
*/
#define I2S_MODE_MASTER (0x01)
#define I2S_MODE_SLAVE  (0x02)

/*
** SAI I/O Mode
*/
#define I2S_IO_READ     (0x04)
#define I2S_IO_WRITE    (0x08)

/*
** SAI RX/TX clock modes
*/
#define I2S_TX_ASYNCHRONOUS (0x01)
#define I2S_TX_SYNCHRONOUS  (0x02)
#define I2S_RX_ASYNCHRONOUS (0x04)
#define I2S_RX_SYNCHRONOUS  (0x08)

#define I2S_TX_BCLK_NORMAL  (0x10)
#define I2S_TX_BCLK_SWAPPED (0x20)
#define I2S_RX_BCLK_NORMAL  (0x40)
#define I2S_RX_BCLK_SWAPPED (0x80)

#define I2S_TX_MASTER       (I2S_MODE_MASTER << 4)
#define I2S_TX_SLAVE        (I2S_MODE_SLAVE  << 4)

#define I2S_RX_MASTER       (I2S_MODE_MASTER)
#define I2S_RX_SLAVE        (I2S_MODE_SLAVE)

#define I2S_DEBUG       0

#if I2S_DEBUG
#define I2S_LOG(...)  \
        {   \
            printf("\nI2S_LOG: %s: %d", __FILE__, __LINE__);  \
            printf(__VA_ARGS__);    \
        }
#else
#define I2S_LOG(...) {}
#endif

/*
** SAI Error Codes
*/
#define I2S_ERROR_BASE                      (DRIVER_ERROR_BASE | 0x0FF0)
#define I2S_OK                              (0x00)
#define I2S_ERROR_INVALID_PARAMETER         (I2S_ERROR_BASE | 0x01)
#define I2S_ERROR_CHANNEL_INVALID           (I2S_ERROR_BASE | 0x02)
#define I2S_ERROR_MODE_INVALID              (I2S_ERROR_BASE | 0x03)
#define I2S_ERROR_WORD_LENGTH_UNSUPPORTED   (I2S_ERROR_BASE | 0x04)
#define I2S_ERROR_CLK_INVALID               (I2S_ERROR_BASE | 0x05)
#define I2S_ERROR_DIVIDER_VALUE_INVALID     (I2S_ERROR_BASE | 0x06)
#define I2S_ERROR_FREQUENCY_INVALID         (I2S_ERROR_BASE | 0x07)
#define I2S_ERROR_BUFFER_SMALL              (I2S_ERROR_BASE | 0x08)
#define I2S_ERROR_DEVICE_BUSY               (I2S_ERROR_BASE | 0x09)
#define I2S_ERROR_PARAM_OUT_OF_RANGE        (I2S_ERROR_BASE | 0x0A)
#define I2S_ERROR_INVALID_BCLK              (I2S_ERROR_BASE | 0x0B)
#define I2S_ERROR_INVALID_TX_CHANNEL        (I2S_ERROR_BASE | 0x0C)
#define I2S_ERROR_INVALID_RX_CHANNEL        (I2S_ERROR_BASE | 0x0D)

/*
** IOCTL calls specific to I2S
*/
#define IO_TYPE_I2S                                 0x15
#define IO_IOCTL_I2S_SET_MODE_MASTER                _IO(IO_TYPE_I2S, 0x01)
#define IO_IOCTL_I2S_SET_MODE_SLAVE                 _IO(IO_TYPE_I2S, 0x02)
#define IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT           _IO(IO_TYPE_I2S, 0x03)
#define IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT           _IO(IO_TYPE_I2S, 0x04)
#define IO_IOCTL_I2S_SET_DATA_BITS                  _IO(IO_TYPE_I2S, 0x05)
#define IO_IOCTL_I2S_DISABLE_DEVICE                 _IO(IO_TYPE_I2S, 0x06)
#define IO_IOCTL_I2S_ENABLE_DEVICE                  _IO(IO_TYPE_I2S, 0x07)
#define IO_IOCTL_I2S_SET_MCLK_FREQ                  _IO(IO_TYPE_I2S, 0x08)
#define IO_IOCTL_I2S_SET_FS_FREQ                    _IO(IO_TYPE_I2S, 0x09)
#define IO_IOCTL_I2S_TX_DUMMY_ON                    _IO(IO_TYPE_I2S, 0x0A)
#define IO_IOCTL_I2S_TX_DUMMY_OFF                   _IO(IO_TYPE_I2S, 0x0B)
#define IO_IOCTL_I2S_GET_MODE                       _IO(IO_TYPE_I2S, 0x0C)
#define IO_IOCTL_I2S_GET_CLOCK_SOURCE               _IO(IO_TYPE_I2S, 0x0D)
#define IO_IOCTL_I2S_GET_DATA_BITS                  _IO(IO_TYPE_I2S, 0x0E)
#define IO_IOCTL_I2S_GET_MCLK_FREQ                  _IO(IO_TYPE_I2S, 0x0F)
#define IO_IOCTL_I2S_GET_BCLK_FREQ                  _IO(IO_TYPE_I2S, 0x10)
#define IO_IOCTL_I2S_GET_TX_DUMMY                   _IO(IO_TYPE_I2S, 0x11)
#define IO_IOCTL_I2S_GET_FS_FREQ                    _IO(IO_TYPE_I2S, 0x12)
#define IO_IOCTL_I2S_GET_STATISTICS                 _IO(IO_TYPE_I2S, 0x13)
#define IO_IOCTL_I2S_SET_TXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x14)
#define IO_IOCTL_I2S_SET_RXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x15)
#define IO_IOCTL_I2S_GET_TXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x16)
#define IO_IOCTL_I2S_GET_RXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x17)
#define IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_ON      _IO(IO_TYPE_I2S, 0x18)
#define IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_OFF     _IO(IO_TYPE_I2S, 0x19)
#define IO_IOCTL_I2S_GET_CLK_ALWAYS_ENABLED         _IO(IO_TYPE_I2S, 0x1A)
#define IO_IOCTL_I2S_CLEAR_STATISTICS               _IO(IO_TYPE_I2S, 0x1B)
#define IO_IOCTL_I2S_SET_CLOCK_MODE                 _IO(IO_TYPE_I2S, 0x1C)
#define IO_IOCTL_I2S_GET_CLOCK_MODE                 _IO(IO_TYPE_I2S, 0x1D)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** I2S_STATISTICS_STRUCT
** SAI run time statistics structure
*/

typedef struct i2s_statistics_struct
{
    /* Number of SAI interrupts so far */
    uint_32 INTERRUPTS;

    /* FIFO error */
    uint_32 FIFO_ERROR;

    /* FIFO sync error */
    uint_32 BUFFER_ERROR;

    /* Number of valid bytes received (not dummy receives) */
    uint_32 RX_PACKETS;

    /* Number of valid bytes transmitted (not dummy transmits) */
    uint_32 TX_PACKETS;

    /* Number of bytes currently read or wrote */
    uint_32 PACKETS_PROCESSED;

    /* Number of bytes currently buffered */
    uint_32 PACKETS_QUEUED;

    /* Number of bytes requested for reading or writing */
    uint_32 PACKETS_REQUESTED;

} I2S_STATISTICS_STRUCT, _PTR_ I2S_STATISTICS_STRUCT_PTR;

typedef struct sai_init_struct
{
    /* A pointer to a string that identifies the device for fopen */
    char_ptr                ID_PTR;

    _mqx_int (_CODE_PTR_    INIT)(pointer);

    _mqx_int (_CODE_PTR_    DEINIT)(pointer, pointer);

    _mqx_int (_CODE_PTR_    RX)(pointer, char_ptr, uint_32);

    _mqx_int (_CODE_PTR_    TX)(pointer, char_ptr, uint_32);

    /* The ioctl function to call */
    _mqx_int (_CODE_PTR_    IOCTL)(pointer, _mqx_int, pointer);
    
    /* The I/O channel initialization data */
    pointer                 INIT_DATA_PTR;
} SAI_INIT_STRUCT, _PTR_ SAI_INIT_STRUCT_PTR;

typedef const SAI_INIT_STRUCT _PTR_ SAI_INIT_STRUCT_CPTR; 

/*
** FUNCTION PROTOTYPE
*/

#ifdef __cplusplus
extern "C" 
{
#endif

    _mqx_uint _io_sai_int_install(SAI_INIT_STRUCT_CPTR);

#ifdef __cplusplus
}
#endif

#endif /* __SAI_H__ */

/* EOF */
