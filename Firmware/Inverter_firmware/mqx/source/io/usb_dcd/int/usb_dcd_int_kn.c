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
* $FileName: usb_dcd_int_kn.c$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains low level functions for the USB DCD interrupt device driver
*   for Kinetis family.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include "usb_dcd.h"
#include "usb_dcd_int_prv.h"
#include "usb_dcd_kn_prv.h"


extern uint_32 _kusb_dcd_polled_init (KUSB_DCD_INIT_STRUCT_PTR, pointer _PTR_, char_ptr);
extern uint_32 _kusb_dcd_polled_ioctl (KUSB_DCD_INFO_STRUCT_PTR, uint_32, uint_32_ptr);

extern uint_32 _kusb_dcd_int_init (IO_USB_DCD_INT_DEVICE_STRUCT_PTR, char_ptr);
extern uint_32 _kusb_dcd_int_deinit (IO_USB_DCD_INT_DEVICE_STRUCT_PTR, KUSB_DCD_INFO_STRUCT_PTR);
extern uint_32 _kusb_dcd_int_read (IO_USB_DCD_INT_DEVICE_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 _kusb_dcd_int_write (IO_USB_DCD_INT_DEVICE_STRUCT_PTR, uchar_ptr, uint_32);
static void    _kusb_dcd_isr (pointer);


/*FUNCTION****************************************************************
*
* Function Name    : _kusb_dcd_int_install
* Returned Value   : MQX error code
* Comments         :
*    Install an USB DCD device.
*
*END*********************************************************************/

uint_32 _kusb_dcd_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr           identifier,

      /* [IN] The I/O init data pointer */
      KUSB_DCD_INIT_STRUCT_CPTR  init_data_ptr
   )
{ /* Body */

   return _io_usb_dcd_int_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer, char_ptr))_kusb_dcd_int_init,
      (uint_32 (_CODE_PTR_)(pointer, pointer))_kusb_dcd_int_deinit,
      (int_32 (_CODE_PTR_)(pointer, char_ptr, int_32))_kusb_dcd_int_read,
      (int_32 (_CODE_PTR_)(pointer, char_ptr, int_32))_kusb_dcd_int_write,
      (int_32 (_CODE_PTR_)(pointer, uint_32, uint_32_ptr))_kusb_dcd_polled_ioctl,
      (pointer)init_data_ptr);

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kusb_dcd_int_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an USB_DCD device.
*
*END*********************************************************************/

uint_32 _kusb_dcd_int_init
   (
      /* [IN] Initialization information for the device being opened */
      IO_USB_DCD_INT_DEVICE_STRUCT_PTR  int_io_dev_ptr,

      /* [IN] The rest of the name of the device opened */
      char_ptr                      open_name_ptr

   )
{ /* Body */
   USBDCD_MemMapPtr                     usb_dcd_ptr;
   KUSB_DCD_INFO_STRUCT_PTR      		 io_info_ptr;
   KUSB_DCD_INIT_STRUCT_PTR		       usb_dcd_init_ptr;
   uint_32                        		 vector, result;

   usb_dcd_init_ptr = (KUSB_DCD_INIT_STRUCT_PTR)(int_io_dev_ptr->DEV_INIT_DATA_PTR);
   result = _kusb_dcd_polled_init (usb_dcd_init_ptr,
                                     &(int_io_dev_ptr->DEV_INFO_PTR),
                                     open_name_ptr);
   if (result)
   {
      return result;
   }

   io_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   io_info_ptr->METHOD = USB_DCD_INT;
   usb_dcd_ptr = io_info_ptr->USB_DCD_PTR;
   vector = _bsp_get_usb_dcd_vector();
   if (0 == vector)
   {
      return USB_DCD_ERROR_INVALID_INT_VECTOR;
   }

   /* Install new vectors and backup the old ones */
   io_info_ptr->OLD_ISR_DATA = _int_get_isr_data (vector);
   io_info_ptr->OLD_ISR = _int_install_isr (vector, _kusb_dcd_isr, io_info_ptr);

   /* Enable USB DCD interrupts */
#ifndef BSP_TWRMCF51JF
   _bsp_int_init((IRQInterruptIndex)_bsp_get_usb_dcd_vector(), io_info_ptr->INIT.LEVEL, 0, TRUE);
   NVICICER2|=(1<<10);
   NVICISER2|=(1<<10);
   NVICICPR2|=(1<<10);
#endif
   usb_dcd_ptr->CONTROL |= USBDCD_CONTROL_IE_MASK;
   return result;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kusb_dcd_int_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an USB DCD device.
*
*END*********************************************************************/

uint_32 _kusb_dcd_int_deinit
   (
      /* [IN] the initialization information for the device being opened */
      IO_USB_DCD_INT_DEVICE_STRUCT_PTR  int_io_dev_ptr,

      /* [IN] the address of the device specific information */
      KUSB_DCD_INFO_STRUCT_PTR   io_info_ptr
   )
{ /* Body */
   USBDCD_MemMapPtr                     usb_dcd_ptr;
   KUSB_DCD_INIT_STRUCT_PTR      usb_dcd_init_ptr;

   if ((NULL == io_info_ptr) || (NULL == int_io_dev_ptr))
   {
      return USB_DCD_ERROR_INVALID_PARAMETER;
   }

   usb_dcd_ptr = io_info_ptr->USB_DCD_PTR;

   /* Disable the USB DCD Interrupt */
   usb_dcd_ptr->CONTROL &= (uint_32)(~USBDCD_CONTROL_SR_MASK);
   /* Disable the USB DCD Module */
	usb_dcd_ptr->CONTROL |= USBDCD_CONTROL_SR_MASK;

   /* Install original vectors */
   usb_dcd_init_ptr = (KUSB_DCD_INIT_STRUCT_PTR)(int_io_dev_ptr->DEV_INIT_DATA_PTR);
   _int_install_isr (_bsp_get_usb_dcd_vector(), io_info_ptr->OLD_ISR, io_info_ptr->OLD_ISR_DATA);

   /* Free buffers and info struct */
   _mem_free (int_io_dev_ptr->DEV_INFO_PTR);
   int_io_dev_ptr->DEV_INFO_PTR = NULL;

   return USB_DCD_OK;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _kusb_dcd_int_read
* Returned Value   : MQX_OK
* Comments         :
*
*
*
*END*********************************************************************/

uint_32 _kusb_dcd_int_read
   (
      /* [IN] the address of the device specific information */
      IO_USB_DCD_INT_DEVICE_STRUCT_PTR   int_io_dev_ptr,

      /* [IN] The array to copy data into */
      uchar_ptr                      buffer,

      /* [IN] number of bytes to read */
      uint_32                        length
   )
{ /* Body */

   return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _kusb_dcd_int_write
* Returned Value   : MQX_OK
* Comments         :
*
*
*
*END*********************************************************************/

uint_32 _kusb_dcd_int_write
   (
      /* [IN] the address of the device specific information */
      IO_USB_DCD_INT_DEVICE_STRUCT_PTR    int_io_dev_ptr,

      /* [IN] The array characters are to be read from */
      uchar_ptr                       buffer,

      /* [IN] number of bytes to output */
      uint_32                         length
   )
{ /* Body */

   return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    :_kusb_dcd_isr
* Returned Value   : none
*
*END*********************************************************************/
static void _kusb_dcd_isr
   (
      pointer                  parameter
   )
{ /* Body */
   KUSB_DCD_INFO_STRUCT_PTR     io_info_ptr = parameter;
   USBDCD_MemMapPtr             usb_dcd_ptr = io_info_ptr->USB_DCD_PTR;
   USB_MemMapPtr				usbotg = USB0_BASE_PTR;

   /* Clear interrupt flag */
   usb_dcd_ptr->CONTROL |= USBDCD_CONTROL_IACK_MASK;
   /* Update DCD state */
   io_info_ptr->STATE = USB_DCD_SEQ_COMPLETE;
   /* Enable pullup register for Non-OTG*/
	if (usb_dcd_ptr->STATUS & USBDCD_STATUS_SEQ_RES_MASK)
	{
		 usbotg->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
	}
} /* Endbody */

/* EOF */
