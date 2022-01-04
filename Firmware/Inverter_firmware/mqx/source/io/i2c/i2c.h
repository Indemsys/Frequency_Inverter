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
* $FileName: i2c.h$
* $Version : 3.8.13.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the I2C I/O functions.
*
*END************************************************************************/

#ifndef _i2c_h_
#define _i2c_h_ 1

#include <ioctl.h>


/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* 
** I2C internal states
*/
enum 
{
   I2C_STATE_READY = 0,
   I2C_STATE_REPEATED_START,
   I2C_STATE_TRANSMIT,
   I2C_STATE_RECEIVE,
   I2C_STATE_ADDRESSED_AS_SLAVE_RX,
   I2C_STATE_ADDRESSED_AS_SLAVE_TX,
   I2C_STATE_LOST_ARBITRATION,
   I2C_STATE_FINISHED
};

/* 
** IOCTL calls specific to I2C 
*/
#define IO_IOCTL_I2C_SET_BAUD                         _IO(IO_TYPE_I2C,0x01)
#define IO_IOCTL_I2C_GET_BAUD                         _IO(IO_TYPE_I2C,0x02)
#define IO_IOCTL_I2C_SET_MASTER_MODE                  _IO(IO_TYPE_I2C,0x03)
#define IO_IOCTL_I2C_SET_SLAVE_MODE                   _IO(IO_TYPE_I2C,0x04)
#define IO_IOCTL_I2C_GET_MODE                         _IO(IO_TYPE_I2C,0x05)
#define IO_IOCTL_I2C_SET_STATION_ADDRESS              _IO(IO_TYPE_I2C,0x06)
#define IO_IOCTL_I2C_GET_STATION_ADDRESS              _IO(IO_TYPE_I2C,0x07)
#define IO_IOCTL_I2C_SET_DESTINATION_ADDRESS          _IO(IO_TYPE_I2C,0x08)
#define IO_IOCTL_I2C_GET_DESTINATION_ADDRESS          _IO(IO_TYPE_I2C,0x09)
#define IO_IOCTL_I2C_SET_RX_REQUEST                   _IO(IO_TYPE_I2C,0x0A)
#define IO_IOCTL_I2C_REPEATED_START                   _IO(IO_TYPE_I2C,0x0B)
#define IO_IOCTL_I2C_STOP                             _IO(IO_TYPE_I2C,0x0C)
#define IO_IOCTL_I2C_GET_STATE                        _IO(IO_TYPE_I2C,0x0D)
#define IO_IOCTL_I2C_GET_STATISTICS                   _IO(IO_TYPE_I2C,0x0E)
#define IO_IOCTL_I2C_CLEAR_STATISTICS                 _IO(IO_TYPE_I2C,0x0F)
#define IO_IOCTL_I2C_DISABLE_DEVICE                   _IO(IO_TYPE_I2C,0x10)
#define IO_IOCTL_I2C_ENABLE_DEVICE                    _IO(IO_TYPE_I2C,0x11)
#define IO_IOCTL_I2C_GET_BUS_AVAILABILITY             _IO(IO_TYPE_I2C,0x12)

/* 
** I2C Bus Modes
*/
#define I2C_MODE_MASTER                               (0x00)
#define I2C_MODE_SLAVE                                (0x01)

/* 
** I2C Bus Operations
*/
#define I2C_OPERATION_WRITE                           (0x00)
#define I2C_OPERATION_READ                            (0x01)
#define I2C_OPERATION_STARTED                         (0x02)

/* 
** I2C Bus Availability
*/
#define I2C_BUS_IDLE                                  (0x00)
#define I2C_BUS_BUSY                                  (0x01)

/* 
** I2C Error Codes
*/
#define I2C_OK                                        (0x00)
#define I2C_ERROR_DEVICE_BUSY                         (I2C_ERROR_BASE | 0x01)
#define I2C_ERROR_CHANNEL_INVALID                     (I2C_ERROR_BASE | 0x02)
#define I2C_ERROR_INVALID_PARAMETER                   (I2C_ERROR_BASE | 0x03)
#define I2C_ERROR_LINE_TIMEOUT                        (I2C_ERROR_BASE | 0x04)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
 
typedef struct i2c_statistics_struct 
{
   /* Number of I2C interrupts so far */
   uint_32               INTERRUPTS;

   /* Number of valid bytes received (not dummy receives) */
   uint_32               RX_PACKETS;

   /* Number of valid bytes transmitted (not dummy transmits) */
   uint_32               TX_PACKETS;

   /* Number of times master lost arbitration */
   uint_32               TX_LOST_ARBITRATIONS;

   /* Number of times master was addressed as slave */
   uint_32               TX_ADDRESSED_AS_SLAVE;

   /* Number of not acknowledged (interrupted) transmits */
   uint_32               TX_NAKS;
   
} I2C_STATISTICS_STRUCT, _PTR_ I2C_STATISTICS_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _io_i2c_polled_install(
      char_ptr,
      _mqx_uint (_CODE_PTR_)(pointer, pointer _PTR_, char_ptr),
      _mqx_uint (_CODE_PTR_)(pointer, pointer),
      _mqx_int (_CODE_PTR_)(pointer, char_ptr, _mqx_int),
      _mqx_int (_CODE_PTR_)(pointer, char_ptr, _mqx_int),
      _mqx_int (_CODE_PTR_)(pointer, _mqx_uint, _mqx_uint_ptr),
      pointer);

extern _mqx_uint _io_i2c_int_install(
      char_ptr,
      _mqx_uint (_CODE_PTR_)(pointer, char_ptr),
      _mqx_uint (_CODE_PTR_)(pointer, pointer),
      _mqx_int (_CODE_PTR_)(pointer, char_ptr, _mqx_int),
      _mqx_int (_CODE_PTR_)(pointer, char_ptr, _mqx_int),
      _mqx_int (_CODE_PTR_)(pointer, _mqx_uint, _mqx_uint_ptr),
      pointer);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
