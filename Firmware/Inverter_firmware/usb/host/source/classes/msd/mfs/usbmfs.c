/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: usbmfs.c$
* $Version : 3.8.34.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the USB MFS mass storage link driver.
*
*END************************************************************************/

#include <mqx.h>
#include "usb.h"
#include "usb_prv.h"
#include "host_common.h"

#include "host_dev_list.h" //see below: workaround 

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif

#include "../usb_host_msd_bo.h"
#include "../usb_host_msd_ufi.h"
#include "usbmfs.h"
#include "usbmfspr.h"

/* Function declaration */
static void _io_usb_mfs_callback
(
    USB_STATUS,  /*status of this command*/
    pointer,     /*pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
    pointer,     /*pointer to the command object*/
    uint_32      /* length of the data transfered if any */
);


static int_32 _io_usb_mfs_open_internal(IO_USB_MFS_STRUCT_PTR info_ptr, CLASS_CALL_STRUCT_PTR ccs_ptr);
static _mqx_int _io_usb_mfs_ioctl_stop(IO_USB_MFS_STRUCT_PTR info_ptr);
static _mqx_int _io_usb_mfs_read_sector_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint_32 sector, uint_16 how_many_sectors, uchar_ptr data_ptr);
static _mqx_int _io_usb_mfs_read_sector_long_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint_32 sector, uint_32 how_many_sectors, uchar_ptr data_ptr);
static _mqx_int _io_usb_mfs_write_sector_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint_32 sector, uint_16 how_many_sectors, uchar_ptr data_ptr);
static _mqx_int _io_usb_mfs_write_sector_long_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint_32 sector, uint_32 how_many_sectors, uchar_ptr data_ptr);
static _mqx_int _io_usb_mfs_read_write_sectors_internal(IO_USB_MFS_STRUCT_PTR info_ptr, char_ptr data_ptr, uint_32 num, boolean write, MQX_FILE_PTR fd_ptr);
static void _io_usb_mfs_callback(USB_STATUS command_status, pointer p1, pointer p2, uint_32 len);
static void _io_usb_ctrl_callback(pointer, pointer, uchar_ptr, uint_32, USB_STATUS);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_install
* Returned Value   : uint_32 - a task error code or MQX_OK
* Comments         :
*    Install a  USB-MFS mass storage device driver.
*
*END*----------------------------------------------------------------------*/

uint_32 _io_usb_mfs_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr                      identifier,

      /* [IN] Logical unit number which driver need to install */
      uint_8                        logical_unit,

      /* [IN] Interface Handle */
      CLASS_CALL_STRUCT_PTR         ccs_ptr

   )
{ /* Body */
   IO_USB_MFS_STRUCT_PTR            info_ptr;
   USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr = NULL;
   USB_STATUS                       error = USBERR_ERROR;
   uint_8                           *block_ptr, offset;

   /* Pointer validity-checking, clear memory, init header */
   USB_lock();
   if (usb_host_class_intf_validate(ccs_ptr)) {
      intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
      if(intf_ptr != NULL) {
         error = usb_hostdev_validate (intf_ptr->G.dev_handle);
      }
   } /* Endif */

   if (USB_OK != error) { /* Device was already detached or intf_ptr is NULL */
      USB_unlock();
      return MQX_OUT_OF_MEMORY;
   }

   if (USB_OK != _usb_hostdev_get_buffer(intf_ptr->G.dev_handle, sizeof(IO_USB_MFS_STRUCT), (pointer *) &info_ptr)) {
      USB_unlock();
      return MQX_OUT_OF_MEMORY;
   }

   USB_unlock();
   _mem_zero(info_ptr, sizeof(IO_USB_MFS_STRUCT));

   /* Fill in the state structure with the info we know */
   info_ptr->LUN         = logical_unit;
   info_ptr->BLENGTH     = USB_MFS_DEFAULT_SECTOR_SIZE;
   info_ptr->ERROR_CODE  = IO_OK;
   info_ptr->BLOCK_MODE  = TRUE;

   info_ptr->COMMAND.CBW_PTR = (CBW_STRUCT_PTR) &info_ptr->CBW;
   info_ptr->COMMAND.CSW_PTR = (CSW_STRUCT_PTR) &info_ptr->CSW;
   info_ptr->COMMAND.CALL_PTR = ccs_ptr;
   info_ptr->COMMAND.LUN = logical_unit;
   info_ptr->COMMAND.CALLBACK = _io_usb_mfs_callback;
   info_ptr->COMMAND_STATUS = MQX_OK;
   _lwsem_create(&info_ptr->LWSEM, 1);
   _lwsem_create(&info_ptr->COMMAND_DONE, 0);

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (uint_8 *)info_ptr;
   offset = *(block_ptr - 1);
   block_ptr = block_ptr - offset - MEM_HEADER_LEN;

   _mem_set_type(block_ptr, MEM_TYPE_USB_MFS_STRUCT);
   return (_io_dev_install_ext(identifier,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr,  char_ptr))_io_usb_mfs_open,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR)                     )_io_usb_mfs_close,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr,  int_32)  )_io_usb_mfs_read,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr,  int_32)  )_io_usb_mfs_write,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, _mqx_uint, pointer) ) _io_usb_mfs_ioctl,
      _io_usb_mfs_uninstall,
      (pointer)block_ptr /* instead of info_ptr */
      ));

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_uninstall_internal
* Returned Value   : uint_32 error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/
int_32 _io_usb_mfs_uninstall
   (
       /* [IN] The device to uninstall */            
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ 
   DEV_MEMORY_PTR block_ptr;
   IO_USB_MFS_STRUCT_PTR info_ptr;

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   _lwsem_destroy(&info_ptr->COMMAND_DONE);
   _lwsem_destroy(&info_ptr->LWSEM);
   /* The info_ptr will be disposed after device detach */
   //USB_mem_free(info_ptr);
   return MQX_OK;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_open_internal
* Returned Value   : An error code or MQX_OK
* Comments         : Opens and initializes a USB-MFS mass storage device driver.
*                    Mass storage device should be initialized to prior to this
*                    this call.
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_open_internal
   (
   IO_USB_MFS_STRUCT_PTR            info_ptr,
   CLASS_CALL_STRUCT_PTR            ccs_ptr 
   )
{ /* Body */
   uint_32     i;
   USB_STATUS error;

   error = usb_class_mass_getmaxlun_bulkonly(info_ptr->COMMAND.CALL_PTR, (uchar_ptr) &info_ptr->INQUIRY_DATA, _io_usb_ctrl_callback, info_ptr);
   if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
      return IO_ERROR;
   } /* Endif */
   if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
      /* Cancel the command */
      usb_mass_ufi_cancel(&info_ptr->COMMAND);
      return IO_ERROR_TIMEOUT;
   } /* Endif */

   error = usb_mass_ufi_inquiry(&info_ptr->COMMAND, (uchar_ptr) &info_ptr->INQUIRY_DATA, sizeof(INQUIRY_DATA_FORMAT));
   if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
      return IO_ERROR_INQUIRE;
   } /* Endif */
   if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
      /* Cancel the command */
      usb_mass_ufi_cancel(&info_ptr->COMMAND);
      return IO_ERROR_TIMEOUT;
   } /* Endif */


   if (info_ptr->COMMAND_STATUS != MQX_OK) {
      return IO_ERROR_INQUIRE;
   } /* Endif */

   for (i = 0; i < USBCFG_MFS_MAX_RETRIES; i++) {
      if (i) {
         /* Delay for a user-specified amount of time */
         _time_delay_ticks(USBCFG_MFS_OPEN_READ_CAPACITY_RETRY_DELAY);
      }
      
      /* Send the call now */
      error = usb_mass_ufi_read_capacity(&info_ptr->COMMAND, (uchar_ptr) &info_ptr->CAPACITY,
         sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));

      if ((error!=MQX_OK) && (error!=USB_STATUS_TRANSFER_QUEUED)) {
         return IO_ERROR_READ;
      } /* Endif */

      if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
         usb_mass_ufi_cancel(&info_ptr->COMMAND);
         return IO_ERROR_TIMEOUT;
      } /* Endif */
      
      if (info_ptr->COMMAND_STATUS == MQX_OK)
         break;
      
      
      error = usb_mass_ufi_request_sense(&info_ptr->COMMAND, &info_ptr->SENSE, sizeof(REQ_SENSE_DATA_FORMAT));

      if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
         return IO_ERROR_READ;
      }

      if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
         usb_mass_ufi_cancel(&info_ptr->COMMAND);
         return IO_ERROR_TIMEOUT;
      }
   }
   
   if (info_ptr->COMMAND_STATUS == MQX_OK) {
      info_ptr->BCOUNT = LONG_BE_TO_HOST(*(uint_32*)&info_ptr->CAPACITY.BLLBA);
      info_ptr->BLENGTH = LONG_BE_TO_HOST(*(uint_32*)&info_ptr->CAPACITY.BLENGTH);

      /* should read this to support  low level format */
      info_ptr->NUMBER_OF_HEADS   = 0;
      info_ptr->NUMBER_OF_TRACKS  = 0;
      info_ptr->SECTORS_PER_TRACK = 0;

      info_ptr->SIZE_BYTES = info_ptr->BLENGTH * info_ptr->BCOUNT;
   } /* Endif */
   
   return (_mqx_int)info_ptr->COMMAND_STATUS;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_open
* Returned Value   : An error code or MQX_OK
* Comments         : Opens and initializes a USB-MFS mass storage device driver.
*                    Mass storage device should be initialized to prior to this
*                    this call.
*END*----------------------------------------------------------------------*/
_mqx_int _io_usb_mfs_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR       fd_ptr,

      /* [IN] the remaining portion of the name of the device */
      char_ptr           open_name_ptr,

      /* [IN] USB mass storage class handle */
      char_ptr           flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR             io_dev_ptr = fd_ptr->DEV_PTR;
   CLASS_CALL_STRUCT_PTR            ccs_ptr = (CLASS_CALL_STRUCT_PTR)flags;
   _mqx_int                         io_error;
   DEV_MEMORY_PTR                   block_ptr;
   IO_USB_MFS_STRUCT_PTR            info_ptr;

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   /* Save the mass storage class handle. */
   info_ptr->MSC_STREAM = (pointer) ccs_ptr;

   /* Send the call now */
   _lwsem_wait(&info_ptr->LWSEM);
   io_error = _io_usb_mfs_open_internal(info_ptr, ccs_ptr);
   _lwsem_post(&info_ptr->LWSEM);

   return io_error;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_close
* Returned Value   : ERROR CODE
* Comments         : Closes the USB mass storage link driver
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_usb_mfs_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
   return(MQX_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read
* Returned Value   : number of bytes read
* Comments         : Reads data from the USB mass storage device
*
*END*----------------------------------------------------------------------*/
_mqx_int _io_usb_mfs_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR         fd_ptr,
      /* [IN] where the outgoing data is store */
      char_ptr             data_ptr,
      /* [IN] the number of bytes to output */
      int_32               num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   io_dev_ptr = fd_ptr->DEV_PTR;
   _mqx_int               io_result;
   DEV_MEMORY_PTR         block_ptr;
   IO_USB_MFS_STRUCT_PTR  info_ptr;

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   _lwsem_wait(&info_ptr->LWSEM);
   io_result = _io_usb_mfs_read_write_sectors_internal(info_ptr, data_ptr, (uint_32)num, FALSE, fd_ptr);
   _lwsem_post(&info_ptr->LWSEM);
   return io_result;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_write
* Returned Value   : number of bytes written
* Comments         : Writes data to the USB mass storage device
*
*END*----------------------------------------------------------------------*/
int_32 _io_usb_mfs_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR         fd_ptr,
      /* [IN] where the outgoing data is store */
      char_ptr             data_ptr,
      /* [IN] the number of bytes to output */
      int_32               num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   io_dev_ptr = fd_ptr->DEV_PTR;
   int_32                  results;
   DEV_MEMORY_PTR         block_ptr;
   IO_USB_MFS_STRUCT_PTR  info_ptr;

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   _lwsem_wait(&info_ptr->LWSEM);

    results = _io_usb_mfs_read_write_sectors_internal(info_ptr, data_ptr, (uint_32)num, TRUE, fd_ptr);

   _lwsem_post(&info_ptr->LWSEM);
   return(results);
} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*-------------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_ioctl_stop
   (
   IO_USB_MFS_STRUCT_PTR       info_ptr 
   )
{ /* Body */
   MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO  start_stop;
   USB_STATUS  error;
   _mqx_int    io_result = IO_ERROR;

   start_stop.START = 0;
   start_stop.LOEJ  = 0;
   error = usb_mass_ufi_start_stop(&info_ptr->COMMAND, start_stop.LOEJ, start_stop.START);
   if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
      if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
         if (info_ptr->COMMAND_STATUS == MQX_OK) {
            io_result =  MQX_OK;
         } /* Endif */
      } else {
         usb_mass_ufi_cancel(&info_ptr->COMMAND);
      } /* Endif */
   } /* Endif */
   return(io_result);

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*-------------------------------------------------------------------------*/
int_32 _io_usb_mfs_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      int_32       command,

      /* [IN] the ioctl parameters */
      pointer      input_param_ptr
   )
{ /* Body */
   USB_MFS_DRIVE_INFO_STRUCT_PTR drive_info_ptr;
   IO_DEVICE_STRUCT_PTR          io_dev_ptr = fd_ptr->DEV_PTR;
   int_32                        result = MQX_OK;
   uint_32_ptr                   param_ptr = input_param_ptr;
   DEV_MEMORY_PTR                block_ptr;
   IO_USB_MFS_STRUCT_PTR         info_ptr;

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   switch (command) {
      case IO_IOCTL_GET_NUM_SECTORS:
      case USB_MFS_IOCTL_GET_NUM_SECTORS:
         *param_ptr = info_ptr->BCOUNT;
         break;

      case IO_IOCTL_GET_BLOCK_SIZE:
         *param_ptr = info_ptr->BLENGTH;
         break;

      case IO_IOCTL_DEVICE_IDENTIFY:
         param_ptr[0] = IO_DEV_TYPE_PHYS_USB_MFS;
         param_ptr[1] = IO_DEV_TYPE_LOGICAL_MFS;
         param_ptr[2] = IO_DEV_ATTR_ERASE | IO_DEV_ATTR_POLL
                         | IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE
                         | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE;
         if (info_ptr->BLOCK_MODE) {
            param_ptr[2] |= IO_DEV_ATTR_BLOCK_MODE;
         } /* Endif */
         break;

      case USB_MFS_IOCTL_GET_DRIVE_PARAMS:
         drive_info_ptr = (USB_MFS_DRIVE_INFO_STRUCT_PTR)((pointer)param_ptr);
         drive_info_ptr->NUMBER_OF_HEADS   = info_ptr->NUMBER_OF_HEADS;
         drive_info_ptr->NUMBER_OF_TRACKS  = info_ptr->NUMBER_OF_TRACKS;
         drive_info_ptr->SECTORS_PER_TRACK = info_ptr->SECTORS_PER_TRACK;
         break;

      case IO_IOCTL_GET_VENDOR_INFO:
         *param_ptr = (uint_32)&(info_ptr->INQUIRY_DATA.BVID);
         break;

      case IO_IOCTL_GET_PRODUCT_ID:
         *param_ptr = (uint_32)&(info_ptr->INQUIRY_DATA.BPID);
         break;

      case IO_IOCTL_GET_PRODUCT_REV:
         *param_ptr = (uint_32)&(info_ptr->INQUIRY_DATA.BPRODUCT_REV);
         break;

      case USB_MFS_IOCTL_DEVICE_STOP:
         /* Send the call now */
         _lwsem_wait(&info_ptr->LWSEM);
         

         result = _io_usb_mfs_ioctl_stop(info_ptr);

            /* Wait for the completion*/
         _lwsem_post(&info_ptr->LWSEM);


         break;

         /* Start CR 812 */
      case IO_IOCTL_SET_BLOCK_MODE:
      case USB_MFS_IOCTL_SET_BLOCK_MODE:
         info_ptr->BLOCK_MODE = TRUE;
         break;
         /* End   CR 812 */

      default:
         result = IO_ERROR_INVALID_IOCTL_CMD;
         break;
   } /* Endswitch */
   return(result);

} /* Endbody */


/* Private functions */
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read_sector
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_read_sector_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to read */
      uint_32                sector,

      /* [IN] How many sectors should read */
      uint_16                how_many_sectors,


      /* [IN] Location to read data into */
      uchar_ptr              data_ptr
   )
{ /* Body */
   uint_32        read_size;
   USB_STATUS     error;
   _mqx_int       io_result = IO_ERROR;
   uint_32        tries;

   /* UFI_READ10 command  */
   read_size = info_ptr->BLENGTH * how_many_sectors;

   /* Send the call now */
   for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
      error = usb_mass_ufi_read_10(
         &info_ptr->COMMAND, sector, data_ptr, read_size, how_many_sectors);
      if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
         if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
            if (info_ptr->COMMAND_STATUS == MQX_OK) {
               io_result = (_mqx_int)how_many_sectors;
               break;
            } /* Endif */
         } else {
            /* Wait for the completion*/
            usb_mass_ufi_cancel(&info_ptr->COMMAND);
         } /* Endif */
      }
      else 
      {
#ifdef _HOST_DEBUG_
          printf("\nUSBMFS retrying read sector");
#endif
      } /* Endif */

   } /* Endif */

   return(io_result);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read_sector_long
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_read_sector_long_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to read */
      uint_32                sector,

      /* [IN] How many sectors should read */
      uint_32                how_many_sectors,

      /* [IN] Location to read data into */
      uchar_ptr              data_ptr
   )
{ /* Body */
   uint_32        read_size;
   USB_STATUS     error;
   _mqx_int       io_result = IO_ERROR;
   uint_32        tries;

   read_size = info_ptr->BLENGTH * how_many_sectors;

   /* Send the call now */
   for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
      error = usb_mass_ufi_read_12(
         &info_ptr->COMMAND, sector, data_ptr, read_size, how_many_sectors);

      if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
         if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
            if (info_ptr->COMMAND_STATUS == MQX_OK) {
               io_result = (_mqx_int) how_many_sectors;
               break;
   } /* Endif */
         } else {
            usb_mass_ufi_cancel(&info_ptr->COMMAND);
         } /* Endif */
      } /* Endif */
      //printf("\nUSBMFS retrying read sector long");
   } /* Endif */

   return (io_result);
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_write_sector
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_write_sector_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to write */
      uint_32                sector,

      /* [IN] How many sectors should read */
      uint_16                how_many_sectors,

      /* [IN] Source data location */
      uchar_ptr              data_ptr
   )
{ /* Body */
   uint_32        write_size;
   uint_32        tries;
   USB_STATUS     error;
   _mqx_int       io_result = IO_ERROR;

   if (sector <= info_ptr->BCOUNT) {

      write_size = info_ptr->BLENGTH * how_many_sectors;
      for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
         error = usb_mass_ufi_write_10(&info_ptr->COMMAND,
               sector, data_ptr, write_size, how_many_sectors);
         if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
            if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
               if (info_ptr->COMMAND_STATUS == MQX_OK) {
                  io_result = (_mqx_int) how_many_sectors;
                  break;
               } /* Endif */
            } else {

               /* Send the call now */

               /* Wait for the completion*/
               usb_mass_ufi_cancel(&info_ptr->COMMAND);
            } /* Endif */
         } /* Endif */
         //printf("\nUSBMFS retrying write sector");
      }
   } /* Endif */

   return (io_result);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_write_sector_long
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_write_sector_long_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to write */
      uint_32                sector,

      /* [IN] How many sectors should read */
      uint_32                how_many_sectors,

      /* [IN] Source data location */
      uchar_ptr              data_ptr
   )
{ /* Body */
   uint_32        write_size;
   USB_STATUS     error;
   _mqx_int       io_result = IO_ERROR;
   uint_32        tries;

   if (sector <= info_ptr->BCOUNT) {

      write_size = info_ptr->BLENGTH * how_many_sectors;
      for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
         error = usb_mass_ufi_write_12(&info_ptr->COMMAND,
            sector, data_ptr, write_size, how_many_sectors);
         if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
            if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
               if (info_ptr->COMMAND_STATUS == MQX_OK) {
                  io_result = (_mqx_int) how_many_sectors;
                  break;
               } /* Endif */
            } else {

               /* Send the call now */
               usb_mass_ufi_cancel(&info_ptr->COMMAND);
            }
            /* Wait for the completion*/
         } /* Endif */
      }
   } /* Endif */

   return (io_result);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read_write_blocks
* Returned Value   : error or number of blocks read/written
* Comments         : This function is used to read/write blocks when driver
*   is in block mode
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_read_write_sectors_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR    info_ptr,

      /* [IN] where the newly-read bytes are to be stored */
      char_ptr                 data_ptr,

      /* [IN] the number of sectors to read */
      uint_32                  num,

      /* [IN] Read/write mode */
      boolean                  write,
      
      /* [IN] File pointer for checking file LOCATION and setting ERROR */
      MQX_FILE_PTR             fd_ptr
   )
{ /* Body */
   _mqx_int (_CODE_PTR_ rw_func_long)(IO_USB_MFS_STRUCT_PTR, uint_32, uint_32, uchar_ptr);
   _mqx_int (_CODE_PTR_ rw_func)(IO_USB_MFS_STRUCT_PTR, uint_32, uint_16, uchar_ptr);
   _mqx_int             io_result = IO_ERROR;
   _mqx_uint            io_error; 

   if (write) {
      io_error = IO_ERROR_WRITE_ACCESS;
      rw_func        = _io_usb_mfs_write_sector_internal;
      rw_func_long   = _io_usb_mfs_write_sector_long_internal;
   } else {
      io_error = IO_ERROR_READ_ACCESS;
      rw_func        = _io_usb_mfs_read_sector_internal;
      rw_func_long   = _io_usb_mfs_read_sector_long_internal;
   } /* Endif */


   if (fd_ptr->LOCATION >= info_ptr->BCOUNT) {
      fd_ptr->ERROR = io_error;
   } else {
      if ((num + fd_ptr->LOCATION) >= info_ptr->BCOUNT) {
         fd_ptr->ERROR = io_error;
         num = (uint_32)(info_ptr->BCOUNT - fd_ptr->LOCATION + 1);
      } /* Endif */

      /* Read all the sectors, one at a time */
      if (num > 0xFFFE) {
         /* Read all the sectors at once */
          io_result = rw_func_long(info_ptr, fd_ptr->LOCATION,
            num, (uchar_ptr)data_ptr);
      } else {
          io_result = rw_func(info_ptr, fd_ptr->LOCATION,
            (uint_16)num, (uchar_ptr)data_ptr);
      } /* Endif */
      if (io_result == IO_ERROR ) {
         fd_ptr->ERROR = io_error;
      } else {
         fd_ptr->LOCATION += num;
      } /* Endif */
   } /* Endif */
   return io_result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_callback
* Returned Value   : None
* Comments         : This function is called by lowlevel drivers
when a command call back is set
*
*END*----------------------------------------------------------------------*/
static void _io_usb_mfs_callback
   (
      USB_STATUS command_status,      /*status of this command*/
      pointer p1,     /* pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      pointer p2,     /* pointer to the command object*/
      uint_32 len     /* length of the data transfered if any */
   )
{ /* Body */
   IO_USB_MFS_STRUCT_PTR info_ptr = (IO_USB_MFS_STRUCT_PTR)p2;

   if (command_status == (USB_STATUS)USB_MASS_FAILED_IN_DATA)
      info_ptr->COMMAND.CSW_PTR->BCSWSTATUS = CSW_STATUS_FAILED;
   if (info_ptr->COMMAND.CSW_PTR->BCSWSTATUS == CSW_STATUS_GOOD) {
      info_ptr->COMMAND_STATUS = command_status;   /* set the status */
   } else {
      info_ptr->COMMAND_STATUS = (uint_32)IO_ERROR;
   } 

   _lwsem_post(&info_ptr->COMMAND_DONE);
   
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_ctrl_callback
* Returned Value   : None
* Comments         : This function is called by lowlevel drivers
* when a control pipe replies 
*
*END*----------------------------------------------------------------------*/
static void _io_usb_ctrl_callback(pointer pipe_handle, pointer user_param, uchar_ptr buffer, uint_32 size, USB_STATUS status)
{
   IO_USB_MFS_STRUCT_PTR info_ptr = (IO_USB_MFS_STRUCT_PTR)user_param;
   
   _lwsem_post(&info_ptr->COMMAND_DONE);
}

/* EOF */
