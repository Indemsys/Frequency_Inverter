/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: nandflash.c$
* $Version : 3.8.13.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains NAND flash driver functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "nandflash.h"
#include "nandflashprv.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a NAND flash driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_nandflash_install
   (
      /* [IN] The initialization structure for the device */
      NANDFLASH_INIT_STRUCT_CPTR  init_ptr
   )
{ /* Body */
   IO_NANDFLASH_STRUCT_PTR     dev_ptr;
  
   dev_ptr = (IO_NANDFLASH_STRUCT_PTR)_mem_alloc_system_zero(
      (_mem_size)sizeof(IO_NANDFLASH_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
#endif

   dev_ptr->INIT                 = init_ptr->INIT;
   dev_ptr->DEINIT               = init_ptr->DEINIT;
   dev_ptr->CHIP_ERASE           = init_ptr->CHIP_ERASE;
   dev_ptr->BLOCK_ERASE          = init_ptr->BLOCK_ERASE;
   dev_ptr->PAGE_READ            = init_ptr->PAGE_READ;
   dev_ptr->PAGE_PROGRAM         = init_ptr->PAGE_PROGRAM;
   dev_ptr->WRITE_PROTECT        = init_ptr->WRITE_PROTECT;
   dev_ptr->IS_BLOCK_BAD         = init_ptr->IS_BLOCK_BAD;
   dev_ptr->MARK_BLOCK_AS_BAD    = init_ptr->MARK_BLOCK_AS_BAD;
   dev_ptr->IOCTL                = init_ptr->IOCTL;
   dev_ptr->NANDFLASH_INFO_PTR   = init_ptr->NANDFLASH_INFO_PTR;
   dev_ptr->VIRTUAL_PAGE_SIZE    = BSP_VIRTUAL_PAGE_SIZE;
   dev_ptr->NUM_VIRTUAL_PAGES    = (init_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE / BSP_VIRTUAL_PAGE_SIZE) * init_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS;
   dev_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO    
                                 = (init_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE / BSP_VIRTUAL_PAGE_SIZE);
   dev_ptr->ECC_SIZE             = BSP_ECC_SIZE;
   dev_ptr->WRITE_VERIFY         = init_ptr->WRITE_VERIFY;
   dev_ptr->DEVICE_SPECIFIC_DATA = init_ptr->DEVICE_SPECIFIC_DATA;

   _lwsem_create(&dev_ptr->LWSEM, 1);
    
   return (_io_dev_install_ext(
      init_ptr->ID_PTR,
      _io_nandflash_open,
      _io_nandflash_close,
      _io_nandflash_read,
      _io_nandflash_write,
      _io_nandflash_ioctl,
      _io_nandflash_uninstall, 
      (pointer)dev_ptr)); 
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Uninstalls a NAND flash driver.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
   IO_NANDFLASH_STRUCT_PTR handle_ptr = 
      (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   if (handle_ptr->COUNT == 0) {
      _mem_free((pointer)handle_ptr);
      io_dev_ptr->DRIVER_INIT_PTR = NULL;
      return IO_OK;
   } else {
      return IO_ERROR_DEVICE_BUSY;
   } /* Endif */  
   
}  /* Endbody */    

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_open
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         : Opens and initializes NAND flash driver.
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,
      
      /* [IN] the remaining portion of the name of the device */
      char_ptr   open_name_ptr,

      /* [IN] the flags to be used during operation: */
      char_ptr   flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR      io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR   handle_ptr = 
      (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   uint_32                   chip_initialized = NANDFLASHERR_NO_ERROR;
   uint_32                   num_ecc_bytes;
   
   if (handle_ptr->COUNT) {
      /* Device is already opened */
      fd_ptr->FLAGS = handle_ptr->FLAGS;
      handle_ptr->COUNT++;
      return MQX_OK;
   } /* Endif */
      
   if (handle_ptr->INIT) {
      chip_initialized = (*handle_ptr->INIT)(handle_ptr);
   }/* Endif */

#if MQX_CHECK_ERRORS
   if (chip_initialized != NANDFLASHERR_NO_ERROR) {
      return(IO_ERROR);
   } /* Endif */
#endif   
 
   /* Check if the sum of virtual page size (incl. ECC bytes) per one physical page 
      is not greater than the physical page size plus the number of physical spare bytes. */
   num_ecc_bytes = NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(handle_ptr->ECC_SIZE);
#if !(BSP_M5329EVB || BSP_TWRMPC5125)
   if(((handle_ptr->VIRTUAL_PAGE_SIZE + num_ecc_bytes) * handle_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO) > 
      (handle_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE + handle_ptr->NANDFLASH_INFO_PTR->SPARE_AREA_SIZE))
   {
      return ((_mqx_int)NANDFLASHERR_IMPROPER_ECC_SIZE);
   }
#endif
  
   handle_ptr->COUNT++;   
      
   return MQX_OK;  

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_close
* Returned Value   : MQX_OK
* Comments         : Closes NAND flash driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR handle_ptr = 
      (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   
   if (--handle_ptr->COUNT == 0) {
      if (handle_ptr->DEINIT) {
         (*handle_ptr->DEINIT)(handle_ptr);
      } /* Endif */
   } /* Endif */

   return MQX_OK;  
   
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_read
* Returned Value   : number of pages read
* Comments         : Reads data from NAND flash memory device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] where the data is to be stored */
      char_ptr   data_ptr,
   
      /* [IN] the number of pages to input */
      _mqx_int   num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR handle_ptr  = 
      (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   uint_32 error;

   fd_ptr->ERROR = IO_OK;
   if (NULL == handle_ptr->PAGE_READ)
   {
      fd_ptr->ERROR = IO_ERROR_READ;
      return IO_ERROR;
   } /* Endif */
   
   if ( (num + fd_ptr->LOCATION) > handle_ptr->NUM_VIRTUAL_PAGES ) 
   {
      fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
      num = (_mqx_int)(handle_ptr->NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
   } /* Endif */
   
   
   _lwsem_wait(&handle_ptr->LWSEM);
   
   error = (*handle_ptr->PAGE_READ)(handle_ptr, (uchar_ptr)data_ptr, fd_ptr->LOCATION, num);

   if (error == NANDFLASHERR_NO_ERROR)
   {
      fd_ptr->LOCATION += num;
   }
   else if (error == NANDFLASHERR_ECC_CORRECTED)
   {
      fd_ptr->LOCATION += num;
   }
   else
   {
      fd_ptr->ERROR = IO_ERROR_READ;
      num = IO_ERROR;
   } /* Endif */

   _lwsem_post(&handle_ptr->LWSEM);

   return num;
      
} /* Endbody */

 
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_write
* Returned Value   : number of pages written
* Comments         : Writes data to the NAND flash memory device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] where the data is stored */
      char_ptr   data_ptr,
   
      /* [IN] the number of pages to output */
      _mqx_int   num
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR handle_ptr  = 
      (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   fd_ptr->ERROR = IO_OK;
   if (NULL == handle_ptr->PAGE_PROGRAM)
   {
      fd_ptr->ERROR = IO_ERROR_WRITE;
      return IO_ERROR;
   } /* Endif */
   
   if (fd_ptr->FLAGS & IO_O_RDONLY)
   {
      fd_ptr->ERROR = IO_ERROR_WRITE_PROTECTED;
      return IO_ERROR;
   } /* Endif */
   
   if ( (num + fd_ptr->LOCATION) > handle_ptr->NUM_VIRTUAL_PAGES ) 
   {
      fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
      num = (_mqx_int)(handle_ptr->NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
   } /* Endif */
   
   
   _lwsem_wait(&handle_ptr->LWSEM);
   
   if (((*handle_ptr->PAGE_PROGRAM)(handle_ptr, (uchar_ptr)data_ptr, fd_ptr->LOCATION, num)) == NANDFLASHERR_NO_ERROR)
   {
      fd_ptr->LOCATION += num;
   }
   else
   {
      fd_ptr->ERROR = IO_ERROR_WRITE;
      num = IO_ERROR;
   } /* Endif */

   _lwsem_post(&handle_ptr->LWSEM);

   return num;
      
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_nandflash_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_nandflash_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,
   
      /* [IN] the ioctl command */
      _mqx_uint  cmd,
   
      /* [IN/OUT] the ioctl parameters */
      pointer    param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR               io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR            handle_ptr = 
      (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   _mqx_int                           result = MQX_OK;
   _mqx_uint                          i;
   _mqx_uint_ptr                      uparam_ptr;
   
   switch (cmd) {

      case NANDFLASH_IOCTL_GET_PHY_PAGE_SIZE:
         uparam_ptr = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE;
         break;

      case NANDFLASH_IOCTL_GET_SPARE_AREA_SIZE:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->NANDFLASH_INFO_PTR->SPARE_AREA_SIZE;
         break;

      case NANDFLASH_IOCTL_GET_BLOCK_SIZE:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE;
         break;

      case NANDFLASH_IOCTL_GET_NUM_BLOCKS:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS;
         break;

      case NANDFLASH_IOCTL_GET_WIDTH:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->NANDFLASH_INFO_PTR->WIDTH;
         break;

      case NANDFLASH_IOCTL_GET_NUM_VIRT_PAGES:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->NUM_VIRTUAL_PAGES;
         break;

      case NANDFLASH_IOCTL_GET_VIRT_PAGE_SIZE:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = handle_ptr->VIRTUAL_PAGE_SIZE;
         break;

      case NANDFLASH_IOCTL_ERASE_BLOCK:
         if (handle_ptr->BLOCK_ERASE) {
            if (NANDFLASHERR_NO_ERROR == (*handle_ptr->BLOCK_ERASE)(handle_ptr, (uint_32)param_ptr,FALSE)) {
               result = MQX_OK;
            } else {
               result = IO_ERROR;
            } /* Endif */
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case NANDFLASH_IOCTL_ERASE_BLOCK_FORCE:
         if (handle_ptr->BLOCK_ERASE) {
            if (NANDFLASHERR_NO_ERROR == (*handle_ptr->BLOCK_ERASE)(handle_ptr, (uint_32)param_ptr,TRUE)) {
               result = MQX_OK;
            } else {
               result = IO_ERROR;
            } /* Endif */
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case NANDFLASH_IOCTL_ERASE_CHIP:
         if (handle_ptr->CHIP_ERASE) {
            if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CHIP_ERASE)(handle_ptr)) {
               result = MQX_OK;
            } else {
               result = IO_ERROR;
            } /* Endif */
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case NANDFLASH_IOCTL_WRITE_PROTECT:
         if (handle_ptr->WRITE_PROTECT) {
            if (NANDFLASHERR_NO_ERROR == (*handle_ptr->WRITE_PROTECT)(handle_ptr, *(_mqx_uint_ptr)param_ptr)) {
               result = MQX_OK;
            } else {
               result = IO_ERROR;
            } /* Endif */
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case NANDFLASH_IOCTL_CHECK_BLOCK:
         if (handle_ptr->IS_BLOCK_BAD) {
            result = (*handle_ptr->IS_BLOCK_BAD)(handle_ptr, (uint_32)param_ptr);
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case NANDFLASH_IOCTL_MARK_BLOCK_AS_BAD:
         if (handle_ptr->MARK_BLOCK_AS_BAD) {
            if (NANDFLASHERR_NO_ERROR == (*handle_ptr->MARK_BLOCK_AS_BAD)(handle_ptr, (uint_32)param_ptr)) {
               result = MQX_OK;
            } else {
               result = IO_ERROR;
            } /* Endif */
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case NANDFLASH_IOCTL_GET_BAD_BLOCK_TABLE:
         for (i=0; i<handle_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS; i++)
         {
            if(NANDFLASHERR_BLOCK_NOT_BAD == (*handle_ptr->IS_BLOCK_BAD)(handle_ptr, (uint_32)i))
            {
                *(uint_8*)param_ptr = 1;    /* block is good */
            }
            else
            {
                *(uint_8*)param_ptr = 0;    /* block is bad */
            }
            param_ptr = (pointer)((uint_8*)param_ptr + sizeof(uint_8));
         }
         break;

      case NANDFLASH_IOCTL_GET_ID:
         if (handle_ptr->IOCTL) {
            if (NANDFLASHERR_NO_ERROR == (*handle_ptr->IOCTL)(handle_ptr,NANDFLASH_IOCTL_GET_ID,param_ptr)) {
               result = MQX_OK;
            } else {
               result = IO_ERROR;
            } /* Endif */
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;
         } /* Endif */   
         break;

      case IO_IOCTL_DEVICE_IDENTIFY:
         uparam_ptr  = (uint_32_ptr)param_ptr;
         uparam_ptr[IO_IOCTL_ID_PHY_ELEMENT]  = IO_DEV_TYPE_PHYS_NANDFLASH;
         uparam_ptr[IO_IOCTL_ID_LOG_ELEMENT]  = IO_DEV_TYPE_LOGICAL_MFS;
         uparam_ptr[IO_IOCTL_ID_ATTR_ELEMENT] = IO_NANDFLASH_ATTRIBS;
         /*if (fd_ptr->FLAGS & IO_O_RDONLY)
         {
             uparam_ptr[IO_IOCTL_ID_ATTR_ELEMENT] &= (~ IO_DEV_ATTR_WRITE);
         }*/
         break;

      default:
         if(handle_ptr->IOCTL != NULL){
            result = (*handle_ptr->IOCTL)(handle_ptr, cmd, param_ptr);
         } else {
            result = IO_ERROR_INVALID_IOCTL_CMD;    
         }         
         break;
   } /* Endswitch */
   return result;

} /* Endbody */

/* EOF */
