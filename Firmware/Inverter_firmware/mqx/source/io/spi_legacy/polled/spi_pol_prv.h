#ifndef __spi_pol_prv_h__
#define __spi_pol_prv_h__
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
* $FileName: spi_pol_prv.h$
* $Version : 3.8.8.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file includes the private definitions for the polled SPI
*   I/O drivers.
*
*END************************************************************************/

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*---------------------------------------------------------------------
**
** IO SPI POLLED DEVICE STRUCT
**
** This structure used to store information about an SPI polled io device
** for the IO device table
*/
typedef struct io_spi_polled_device_struct
{

   /* The I/O init function */
   _mqx_uint (_CODE_PTR_ DEV_INIT)(pointer, pointer _PTR_, char _PTR_);

   /* The I/O deinit function */
   _mqx_uint (_CODE_PTR_ DEV_DEINIT)(pointer, pointer);
   
   /* [IN] The READ function */
   _mqx_int (_CODE_PTR_  DEV_READ)(pointer, char_ptr, _mqx_int);
      
   /* [IN] The WRITE function */
   _mqx_int (_CODE_PTR_  DEV_WRITE)(pointer, char_ptr, _mqx_int);
   
   /* The ioctl function, (change bauds etc) */
   _mqx_int (_CODE_PTR_  DEV_IOCTL)(pointer, _mqx_uint, _mqx_uint_ptr, _mqx_uint);

   /* The I/O channel initialization data */
   pointer               DEV_INIT_DATA_PTR;
   
   /* Device specific information */
   pointer               DEV_INFO_PTR;

   /* Open count for number of accessing file descriptors */
   uint_32               COUNT;

   /* Open flags for this channel */
   uint_32               FLAGS;
   
} IO_SPI_POLLED_DEVICE_STRUCT, _PTR_ IO_SPI_POLLED_DEVICE_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_spi_polled_open(MQX_FILE_PTR, char _PTR_, char _PTR_);
extern _mqx_int _io_spi_polled_close(MQX_FILE_PTR);
extern _mqx_int _io_spi_polled_read(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_spi_polled_write(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_spi_polled_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
extern _mqx_int _io_spi_polled_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
