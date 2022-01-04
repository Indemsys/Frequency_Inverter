#ifndef _usbmfspr_h_
#define _usbmfspr_h_
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
* $FileName: usbmfspr.h$
* $Version : 3.8.9.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the USB mass storage link driver
*   to MFS.
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* Error codes from lower layers */
#define USB_MFS_DEFAULT_SECTOR_SIZE  (512)

/*----------------------------------------------------------------------*/
/*
**                          ERROR CODES
*/

/* Error codes from lower layers */
//#define USB_MFS_NO_ERROR             MQX_OK
//#define USB_MFS_READ_ERROR           IO_ERROR_READ
//#define USB_MFS_WRITE_ERROR          IO_ERROR_WRITE
//#define USB_MFS_INVALID_SECTOR       IO_ERROR_SEEK

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/*
** USB_MFS_INFO_STRUCT
** Run time state information for each USB mass storage device
*/
typedef struct io_usb_mfs_info_struct
{
   COMMAND_OBJECT_STRUCT      COMMAND;    // MUST BE FIRST
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER1[CACHE_LINE_FILLER(sizeof(COMMAND_OBJECT_STRUCT))];
#endif
   CBW_STRUCT                 CBW;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER2[CACHE_LINE_FILLER(sizeof(CBW_STRUCT))];
#endif
   CSW_STRUCT                 CSW;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER3[CACHE_LINE_FILLER(sizeof(CSW_STRUCT))];
#endif
   INQUIRY_DATA_FORMAT        INQUIRY_DATA;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER4[CACHE_LINE_FILLER(sizeof(INQUIRY_DATA_FORMAT))];
#endif
  MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO   CAPACITY;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER5[CACHE_LINE_FILLER(sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO))];
#endif
   REQ_SENSE_DATA_FORMAT      SENSE;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER6[CACHE_LINE_FILLER(sizeof(REQ_SENSE_DATA_FORMAT))];
#endif
   /* Handle for mass storage class  calls */
   pointer       MSC_STREAM;
   
   /* 8 bytes of ASCI Data identifying the vendor of the product */
   
   /* 16 bytes of ASCI Data defined by the vendor */
  
   /* 4 bytes of ASCI Data defined by the vendor */
   
   /* CBW tag used for commands */
   uint_32       CBW_TAG;         
   
   /* Drive number to associate with this slot */
   uint_8        LUN;
   
   uint_32       BLENGTH;   // logic block length
   uint_32       BCOUNT;    // logic blocks count

   /* Total size of Drive in bytes */
   uint_32       SIZE_BYTES; 

   /* The number of heads as reported  */
   uint_32       NUMBER_OF_HEADS;          

   /* The number of tracks as reported  */
   uint_32       NUMBER_OF_TRACKS;

   /* The number of sectos per cylinder as reported */
   uint_32       SECTORS_PER_TRACK;

   /* Light weight semaphore struct */
   LWSEM_STRUCT  LWSEM;
   
   /* The address of temp buffer */

   /* The current error code for the device */
   uint_32       ERROR_CODE;
   
   /* Start CR 812 */
   /* Indicates if the device is running in block mode or character mode */
   boolean       BLOCK_MODE;
   /* End   CR 812 */

   uint_32       COMMAND_STATUS;
   LWSEM_STRUCT  COMMAND_DONE;
} IO_USB_MFS_STRUCT, _PTR_ IO_USB_MFS_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_usb_mfs_open(MQX_FILE_PTR, char _PTR_, char _PTR_);
extern _mqx_int _io_usb_mfs_close(MQX_FILE_PTR);
extern _mqx_int _io_usb_mfs_read (MQX_FILE_PTR, char_ptr, int_32);
extern _mqx_int _io_usb_mfs_write(MQX_FILE_PTR, char_ptr, int_32);
extern _mqx_int _io_usb_mfs_ioctl(MQX_FILE_PTR, int_32, pointer);
extern int_32 _io_usb_mfs_uninstall(IO_DEVICE_STRUCT_PTR  io_dev_ptr);
   
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
