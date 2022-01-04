#ifndef _usb_dcd_int_prv_h_
#define _usb_dcd_int_prv_h_ 1
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
* $FileName: usb_dcd_int_prv.h$
* $Version : 3.8.1.0$
* $Date    : Jul-27-2011$
*
* Comments:
*
*   This file includes the private definitions for the interrupt USB DCD
*   I/O drivers.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


/*---------------------------------------------------------------------
**
** IO USB DCD INT DEVICE STRUCT
**
** This structure used to store information about an USB DCD interrupt io device
** for the IO device table
*/
typedef struct io_usb_dcd_int_device_struct
{

   /* The I/O init function */
   _mqx_uint (_CODE_PTR_ DEV_INIT)(pointer, char_ptr);

   /* The I/O deinit function */
   _mqx_uint (_CODE_PTR_ DEV_DEINIT)(pointer, pointer);
   
   /* [IN] The READ function */
   _mqx_int  (_CODE_PTR_ DEV_READ)(pointer, char_ptr, _mqx_int);
      
   /* [IN] The WRITE function */
   _mqx_int  (_CODE_PTR_ DEV_WRITE)(pointer, char_ptr, _mqx_int);
   
   /* The ioctl function, (change bauds etc) */
   _mqx_int  (_CODE_PTR_ DEV_IOCTL)(pointer, _mqx_uint, _mqx_uint_ptr);

   /* The I/O channel initialization data */
   pointer               DEV_INIT_DATA_PTR;
   
   /* Device specific information */
   pointer               DEV_INFO_PTR;

   /* Open count for number of accessing file descriptors */
   _mqx_uint             COUNT;

   /* Open flags for this channel */
   _mqx_uint             FLAGS;

} IO_USB_DCD_INT_DEVICE_STRUCT, _PTR_ IO_USB_DCD_INT_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* I/O prototypes */
extern _mqx_int _io_usb_dcd_int_open(FILE_DEVICE_STRUCT_PTR, char_ptr, char_ptr);
extern _mqx_int _io_usb_dcd_int_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_usb_dcd_int_read(FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_usb_dcd_int_write(FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_usb_dcd_int_ioctl(FILE_DEVICE_STRUCT_PTR, _mqx_uint, pointer);
extern _mqx_int _io_usb_dcd_int_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
