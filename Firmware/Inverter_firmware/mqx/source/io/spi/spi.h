/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
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
* $FileName: spi.h$
* $Version : 3.8.1.0$
* $Date    : Sep-25-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the SPI driver
*
*END************************************************************************/


#ifndef __spi_h__
#define __spi_h__


/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** IOCTL calls specific to SPI
*/
#define IO_IOCTL_SPI_GET_BAUD              _IO(IO_TYPE_SPI,0x01)
#define IO_IOCTL_SPI_SET_BAUD              _IO(IO_TYPE_SPI,0x02)
#define IO_IOCTL_SPI_GET_MODE              _IO(IO_TYPE_SPI,0x03)
#define IO_IOCTL_SPI_SET_MODE              _IO(IO_TYPE_SPI,0x04)
#define IO_IOCTL_SPI_GET_DUMMY_PATTERN     _IO(IO_TYPE_SPI,0x05)
#define IO_IOCTL_SPI_SET_DUMMY_PATTERN     _IO(IO_TYPE_SPI,0x06)
#define IO_IOCTL_SPI_GET_TRANSFER_MODE     _IO(IO_TYPE_SPI,0x07)
#define IO_IOCTL_SPI_SET_TRANSFER_MODE     _IO(IO_TYPE_SPI,0x08)
#define IO_IOCTL_SPI_GET_ENDIAN            _IO(IO_TYPE_SPI,0x09)
#define IO_IOCTL_SPI_SET_ENDIAN            _IO(IO_TYPE_SPI,0x0A)
#define IO_IOCTL_SPI_DEVICE_ENABLE         _IO(IO_TYPE_SPI,0x0B)
#define IO_IOCTL_SPI_DEVICE_DISABLE        _IO(IO_TYPE_SPI,0x0C)
#define IO_IOCTL_SPI_GET_FLAGS             _IO(IO_TYPE_SPI,0x0D)
#define IO_IOCTL_SPI_SET_FLAGS             _IO(IO_TYPE_SPI,0x0E)
#define IO_IOCTL_SPI_GET_STATS             _IO(IO_TYPE_SPI,0x0F)
#define IO_IOCTL_SPI_CLEAR_STATS           _IO(IO_TYPE_SPI,0x10)
#define IO_IOCTL_SPI_GET_FRAMESIZE         _IO(IO_TYPE_SPI,0x11)
#define IO_IOCTL_SPI_SET_FRAMESIZE         _IO(IO_TYPE_SPI,0x12)
#define IO_IOCTL_SPI_GET_CS                _IO(IO_TYPE_SPI,0x13)
#define IO_IOCTL_SPI_SET_CS                _IO(IO_TYPE_SPI,0x14)
#define IO_IOCTL_SPI_SET_CS_CALLBACK       _IO(IO_TYPE_SPI,0x15)
#define IO_IOCTL_SPI_READ_WRITE            _IO(IO_TYPE_SPI,0x16)
#define IO_IOCTL_SPI_FLUSH_DEASSERT_CS     _IO(IO_TYPE_SPI,0x17)


/*
** SPI clock polarity and phase (IO_IOCTL_SPI_XXX_MODE)
*/
#define SPI_CPHA_MASK                      (0x01)
#define SPI_CPOL_MASK                      (0x02)

#define SPI_CLK_POL_PHA_MODE0              (0x00)                            /* Inactive SPICLK low, sample on leading edge */
#define SPI_CLK_POL_PHA_MODE1              (SPI_CPHA_MASK)                   /* Inactive SPICLK low, sample on trailing edge */
#define SPI_CLK_POL_PHA_MODE2              (SPI_CPOL_MASK)                   /* Inactive SPICLK high, sample on leading edge */
#define SPI_CLK_POL_PHA_MODE3              (SPI_CPOL_MASK | SPI_CPHA_MASK)   /* Inactive SPICLK high, sample on trailing edge */
#define SPI_CLK_POL_PHA_MODE_MASK          (SPI_CPOL_MASK | SPI_CPHA_MASK)


/*
** SPI device modes of operation (IO_IOCTL_SPI_XXX_TRANSFER_MODE)
*/
#define SPI_DEVICE_MASTER_MODE             (0x00)
#define SPI_DEVICE_SLAVE_MODE              (0x01)


/*
** SPI data bits transferring format (IO_IOCTL_SPI_XXX_ENDIAN)
*/
#define SPI_DEVICE_BIG_ENDIAN              (0x0001)
#define SPI_DEVICE_LITTLE_ENDIAN           (0x8000)


/*
** SPI Open Flags
*/
#define SPI_FLAG_HALF_DUPLEX               (0 << IO_O_USER_SHIFT)
#define SPI_FLAG_FULL_DUPLEX               (1 << IO_O_USER_SHIFT)
#define SPI_FLAG_NO_DEASSERT_ON_FLUSH      (2 << IO_O_USER_SHIFT)


/*
** SPI Transfer Attributes
** Public so that it can be used in default parameters in init structures,
** otherwise there is no API to directly manipulate attibutes using these masks, use individual IOCTLs instead.
*/
#define SPI_ATTR_ENDIAN_MASK               0x01UL
#define SPI_ATTR_LITTLE_ENDIAN             SPI_ATTR_ENDIAN_MASK
#define SPI_ATTR_BIG_ENDIAN                0

#define SPI_ATTR_TRANSFER_MODE_MASK        0x02UL
#define SPI_ATTR_SLAVE_MODE                SPI_ATTR_TRANSFER_MODE_MASK
#define SPI_ATTR_MASTER_MODE               0


/* SPI Error Codes */
#define SPI_OK                             (0x00)
#define SPI_ERROR_MODE_INVALID             (SPI_ERROR_BASE | 0x01)
#define SPI_ERROR_TRANSFER_MODE_INVALID    (SPI_ERROR_BASE | 0x02)
#define SPI_ERROR_BAUD_RATE_INVALID        (SPI_ERROR_BASE | 0x03)
#define SPI_ERROR_ENDIAN_INVALID           (SPI_ERROR_BASE | 0x04)
#define SPI_ERROR_CHANNEL_INVALID          (SPI_ERROR_BASE | 0x05)
#define SPI_ERROR_DEINIT_FAILED            (SPI_ERROR_BASE | 0x06)
#define SPI_ERROR_INVALID_PARAMETER        (SPI_ERROR_BASE | 0x07)
#define SPI_ERROR_FRAMESIZE_INVALID        (SPI_ERROR_BASE | 0x08)


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/


/*
** SPI_PARAM_STRUCT
**
** This structure defines parameters of SPI bus transfer.
** The structure is stored in init data and file cotext of upper layer driver and passed to lower layer SPI driver.
*/
typedef struct spi_param_struct
{
    /* Baud rate to use */
    uint_32 BAUDRATE;

    /* The SPI clock phase and polarity mode */
    uint_32 MODE;

    /* Size of single SPI frame */
    uint_32 FRAMESIZE;

    /* CS signal(s) to activate for the transfer */
    uint_32 CS;

    /* Additional attributes for the transfer */
    uint_32 ATTR;

    /* Pattern to transmit during half-duplex rx transfer */
    uint_32 DUMMY_PATTERN;

} SPI_PARAM_STRUCT, _PTR_ SPI_PARAM_STRUCT_PTR;


/*
** Functions implemented by lower layer SPI driver (device interface).
*/
typedef _mqx_int (_CODE_PTR_ SPI_DEVIF_INIT_FPTR)(const void _PTR_ init_data_ptr, pointer *io_info_ptr_ptr);
typedef _mqx_int (_CODE_PTR_ SPI_DEVIF_DEINIT_FPTR)(pointer io_info_ptr);
typedef _mqx_int (_CODE_PTR_ SPI_DEVIF_SETPARAM_FPTR)(pointer io_info_ptr, SPI_PARAM_STRUCT_PTR spi_param_ptr);
typedef _mqx_int (_CODE_PTR_ SPI_DEVIF_TX_RX_FPTR)(pointer io_info_ptr, uint_8_ptr txbuf, uint_8_ptr rxbuf, uint_32 len);
typedef _mqx_int (_CODE_PTR_ SPI_DEVIF_CS_DEASSERT_FPTR)(pointer io_info_ptr);
typedef _mqx_int (_CODE_PTR_ SPI_DEVIF_IOCTL_FPTR)(pointer io_info_ptr, SPI_PARAM_STRUCT_PTR spi_param_ptr, uint_32 cmd, uint_32_ptr cmd_param_ptr);


typedef struct spi_devif_struct
{
    /* Initialization if low level SPI driver */
    SPI_DEVIF_INIT_FPTR     INIT;

    /* Deinitialization of low level SPI driver */
    SPI_DEVIF_DEINIT_FPTR   DEINIT;

    /* Sets up the SPI bus accorting to given parameters */
    SPI_DEVIF_SETPARAM_FPTR SETPARAM;

    /* Data trasfer function */
    SPI_DEVIF_TX_RX_FPTR    TX_RX;

    /* Immediate deassertion of CS signals, if handled by low level driver */
    SPI_DEVIF_CS_DEASSERT_FPTR CS_DEASSERT;

    /* IOCTL commands not fully handled by upper layer are passed to this function */
    SPI_DEVIF_IOCTL_FPTR    IOCTL;

} SPI_DEVIF_STRUCT, _PTR_ SPI_DEVIF_STRUCT_PTR;

typedef const SPI_DEVIF_STRUCT _PTR_ SPI_DEVIF_STRUCT_CPTR;


/*
** SPI_CS_CALLBACK
**
** This callback function is used to provide external CS handling funtionality.
*/
typedef _mqx_int (_CODE_PTR_ SPI_CS_CALLBACK)(uint_32 cs, pointer context_data);


/*
** SPI_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a spi port is initialized.
*/
typedef struct spi_init_struct
{
    /* Pointer to low level driver */
    SPI_DEVIF_STRUCT_CPTR DEVIF;

    /* Pointer to init data specific for low level driver */
    const void _PTR_      DEVIF_INIT;

    /* Default transfer parameters for low level driver */
    SPI_PARAM_STRUCT      PARAMS;

    /* Default callback function for external CS handling */
    SPI_CS_CALLBACK       CS_CALLBACK;

    /* Default context for CS callback function */
    pointer               CS_USERDATA;

} SPI_INIT_STRUCT, _PTR_ SPI_INIT_STRUCT_PTR;

typedef const SPI_INIT_STRUCT _PTR_ SPI_INIT_STRUCT_CPTR;


/*
** SPI_STATISTICS_STRUCT
**
** This is used to get statistics data from SPI device using IO_IOCTL_SPI_GET_STATS.
*/
typedef struct spi_statistics_struct
{
   /* Number of valid frames received (not dummy receives) */
   uint_32          RX_PACKETS;

   /* Number of valid frames transmitted (not dummy transmits) */
   uint_32          TX_PACKETS;

} SPI_STATISTICS_STRUCT, _PTR_ SPI_STATISTICS_STRUCT_PTR;


/*
** SPI_CS_CALLBACK_STRUCT
**
** This structure defines the parameters of the external chip select callback
** when passed to IO_IOCTL_SPI_SET_CS_CALLBACK.
*/

typedef struct spi_cs_callback_struct
{
   /* The SPI callback itself */
   SPI_CS_CALLBACK  CALLBACK;

   /* User data */
   pointer          USERDATA;

} SPI_CS_CALLBACK_STRUCT, _PTR_ SPI_CS_CALLBACK_STRUCT_PTR;


/*
** SPI_READ_WRITE_STRUCT
**
** Structure passed to IOCTL TX/RX command (IO_IOCTL_SPI_READ_WRITE) for true duplex operation.
*/

typedef struct spi_read_write_struct
{
   /* Byte length of each buffer */
   uint_32          BUFFER_LENGTH;

   /* Buffer containing bytes to write */
   char_ptr         WRITE_BUFFER;

   /* Buffer containing bytes read after transfer */
   char_ptr         READ_BUFFER;

} SPI_READ_WRITE_STRUCT, _PTR_ SPI_READ_WRITE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif


extern _mqx_int _io_spi_install(char_ptr identifier, SPI_INIT_STRUCT_CPTR init_data_ptr);


#ifdef __cplusplus
}
#endif


#endif

/* EOF */
