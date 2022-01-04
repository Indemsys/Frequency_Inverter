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
* $FileName: usb_msc.h$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
* @brief The file contains USB stack MSC class layer api header function.
*
*****************************************************************************/

#ifndef _USB_MSC_H
#define _USB_MSC_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "devapi.h"  
#include "usb_class.h"
#include "usb_framework.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define USB_MSC_DEVICE_READ_REQUEST     (0x81)
#define USB_MSC_DEVICE_WRITE_REQUEST    (0x82) 
#define USB_MSC_DEVICE_FORMAT_COMPLETE  (0x83)
#define USB_MSC_DEVICE_REMOVAL_REQUEST  (0x84)
#define USB_MSC_DEVICE_GET_INFO         (0x85)
#define USB_MSC_START_STOP_EJECT_MEDIA  (0x86) 

/* Class specific request Codes */
#define BULK_ONLY_MASS_STORAGE_RESET          (0xFF)
#define GET_MAX_LUN                           (0xFE)
#define PUT_REQUESTS                          (0xFD)
#define GET_REQUESTS                          (0xFC)

/* Events to the Application */ /* 0 to 4 are reserved for class events */

/* other macros */
#define  USB_DCBWSIGNATURE       LONG_BE_TO_HOST_CONST(0x55534243)
#define  USB_DCSWSIGNATURE       LONG_BE_TO_HOST_CONST(0x55534253)
#define  USB_CBW_DIRECTION_BIT   (0x80)
#define  USB_CBW_DIRECTION_SHIFT (7)
#define  MSC_CBW_LENGTH          (31)
#define  MSC_CSW_LENGTH          (13)

#define COMMAND_PASSED                (0x00)
#define COMMAND_FAILED                (0x01)
#define PHASE_ERROR                   (0x02)
/* macros for queuing */
 #define MSD_MAX_QUEUE_ELEMS  (4)

/* MACROS FOR COMMANDS SUPPORTED */
 #define INQUIRY_COMMAND                    (0x12)
 #define READ_10_COMMAND                    (0x28)
 #define READ_12_COMMAND                    (0xA8)
 #define REQUEST_SENSE_COMMAND              (0x03)
 #define TEST_UNIT_READY_COMMAND            (0x00)
 #define WRITE_10_COMMAND                   (0x2A)
 #define WRITE_12_COMMAND                   (0xAA)
 #define PREVENT_ALLOW_MEDIUM_REM_COMMAND   (0x1E)
 #define FORMAT_UNIT_COMMAND                (0x04)
 #define READ_CAPACITY_10_COMMAND           (0x25)
 #define READ_FORMAT_CAPACITIES_COMMAND     (0x23)
 #define MODE_SENSE_10_COMMAND              (0x5A) 
 #define MODE_SENSE_6_COMMAND               (0x1A)
 #define MODE_SELECT_10_COMMAND             (0x55)
 #define MODE_SELECT_6_COMMAND              (0x15)
 #define SEND_DIAGNOSTIC_COMMAND            (0x1D)
 #define VERIFY_COMMAND                     (0x2F)
 #define START_STOP_UNIT_COMMAND            (0x1B)
  
 #define MAX_MSC_DEVICE                     (0x01)
 
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
void USB_Service_Bulk_In(PTR_USB_EVENT_STRUCT event,pointer arg);
void USB_Service_Bulk_Out(PTR_USB_EVENT_STRUCT event,pointer arg);
void USB_Class_MSC_Event(uint_8 event, void* val,pointer arg);
/******************************************************************************
 * Types
 *****************************************************************************/
 
typedef uint_32 MSD_HANDLE;

typedef struct _app_data_struct 
{
    uint_8_ptr      data_ptr;         /* pointer to buffer       */            
    uint_32 data_size;                /* buffer size of endpoint */
}APP_DATA_STRUCT;
 
/* structure to hold a request in the endpoint queue */
typedef struct _usb_class_msc_queue 
{
    _usb_device_handle  handle;
    uint_8 channel;       
    APP_DATA_STRUCT app_data;  
}USB_CLASS_MSC_QUEUE, *PTR_USB_CLASS_MSC_QUEUE;
 
/* USB class msc endpoint data */
typedef struct _usb_class_msc_endpoint 
{
    uint_8 endpoint; /* endpoint num */                    
    uint_8 type;     /* type of endpoint (interrupt, bulk or isochronous) */   
    uint_8 bin_consumer;/* the num of queued elements */
    uint_8 bin_producer;/* the num of de-queued elements */
    USB_CLASS_MSC_QUEUE queue[MSD_MAX_QUEUE_ELEMS]; /* queue data */  
}USB_CLASS_MSC_ENDPOINT;

typedef struct _usb_msc_cbw   /* Command Block Wrapper -- 31 bytes */
{
    uint_32 signature;        /*0-3  : dCBWSignature*/
    uint_32 tag;              /*4-7  : dCBWTag*/
    uint_32 data_length;      /*8-11 : dCBWDataTransferLength*/
    uint_8  flag;             /*12   : bmCBWFlags*/
    uint_8  lun;              /*13   : bCBWLUN(bits 3 to 0)*/
    uint_8  cb_length;        /*14   : bCBWCBLength*/
    uint_8  command_block[16];/*15-30 : CBWCB*/
}CBW, *PTR_CBW ;

typedef struct _usb_msc_csw   /* Command Status Wrapper -- 13 bytes */
{
    uint_32 signature; /*0-3  : dCSWSignature*/
    uint_32 tag;       /*4-7 : dCSWTag*/
    uint_32 residue;   /*8-11 : dCSWDataResidue*/
    uint_8 csw_status; /*12 : bCSWStatus*/
}CSW, *PTR_CSW;

typedef struct _lba_info_struct
{
    uint_32 starting_lba;/* LBA to start transfering with */
    uint_32 lba_transfer_num;/* number of LBAs to transfer */    
}LBA_INFO_STRUCT, * PTR_LBA_INFO_STRUCT;

typedef struct _lba_app_struct
{
    uint_32 offset;
    uint_32 size;
    uint_8_ptr buff_ptr;
}LBA_APP_STRUCT, * PTR_LBA_APP_STRUCT;

typedef struct _device_lba_info_struct
{
    uint_32 total_lba_device_supports;/* lab : LOGICAL ADDRESS BLOCK */ 
    uint_32 length_of_each_lab_of_device;
    uint_8 num_lun_supported; 
}DEVICE_LBA_INFO_STRUCT, * PTR_DEVICE_LBA_INFO_STRUCT;

typedef struct _msc_thirteen_case_check
{
    _usb_device_handle handle;
    uint_32 host_expected_data_len;
    uint_8 host_expected_direction;
    uint_32 device_expected_data_len;
    uint_8 device_expected_direction;
    uint_8_ptr csw_status_ptr;
    uint_32_ptr csw_residue_ptr;
    uint_8_ptr buffer_ptr;
    boolean lba_txrx_select;
    LBA_INFO_STRUCT lba_info;
}MSC_THIRTEEN_CASE_STRUCT, *PTR_MSC_THIRTEEN_CASE_STRUCT;

typedef struct _msd_buffers_info
{
     uint_8_ptr msc_lba_send_ptr;
     uint_8_ptr msc_lba_recv_ptr;
     uint_32 msc_lba_send_buff_size;
     uint_32 msc_lba_recv_buff_size;
}MSD_BUFF_INFO, *PTR_MSD_BUFF_INFO;
/* MSD Device Structure */
typedef struct _msc_variable_struct
{
    _usb_device_handle controller_handle;
    MSD_HANDLE msc_handle;
    USB_CLASS_HANDLE class_handle;
    USB_ENDPOINTS *ep_desc_data;
    USB_CLASS_CALLBACK_STRUCT msc_callback;
    USB_REQ_CALLBACK_STRUCT       vendor_callback;            
    USB_CLASS_CALLBACK_STRUCT param_callback;
    uint_8            bulk_in_endpoint; 
    uint_32           bulk_in_endpoint_packet_size;
    uint_8            bulk_out_endpoint;
    uint_32           usb_max_suported_interfaces;
     /* contains the endpoint info */
    /* Memory Allocation for endpoint done at App layer. Only App
       Knows how many endpoints to allocate.
     */
     void *  scsi_object_ptr;
     
    USB_CLASS_MSC_ENDPOINT *ep;
     /* macro configuired by user*/
     /* LUN can have value only from 0 to 15 decimal */
    uint_8 lun;
    /* flag to track bulk out data processing after CBW if needed*/
    boolean out_flag; 
    /* flag to track bulk in data processing before CSW if needed*/
    boolean in_flag; 
    /* flag to track if there is need to stall BULK IN ENDPOINT 
       because of BULK COMMAND*/
    boolean in_stall_flag; 
    /* flag to track if there is need to stall BULK OUT ENDPOINT
       because of BULK COMMAND */
    boolean out_stall_flag; 
    /* flag to validate CBW */
    boolean cbw_valid_flag; 
    /* global structure for command status wrapper */
    PTR_CSW csw_ptr; 
    /* global structure for command block wrapper */
    PTR_CBW cbw_ptr; 
    boolean re_stall_flag;
    DEVICE_LBA_INFO_STRUCT device_info;
    MSD_BUFF_INFO msd_buff;
    /* following two macros are used to manage transfers involving read/write 
       on APPLICATION MASS STORAGE DEVICE */
    uint_32 transfer_remaining;
    uint_32 current_offset;     
}MSC_DEVICE_STRUCT, _PTR_ MSC_DEVICE_STRUCT_PTR; 

/* MSD Configuration structure to be passed by APP*/
typedef struct _usb_msd_config 
{
    /* SCSI related initialization data. To be moved to SCSI layer.*/
     DEVICE_LBA_INFO_STRUCT device_info;
     boolean implementing_disk_drive;
     uint_32 usb_max_suported_interfaces;
     /*****************************************/
     uint_8  bulk_in_endpoint; 
     uint_32 bulk_in_endpoint_packet_size;
     uint_8  bulk_out_endpoint;
     uint_32 desc_endpoint_cnt;
     MSD_BUFF_INFO msd_buff;
     USB_ENDPOINTS *ep_desc_data;
     USB_CLASS_MSC_ENDPOINT *ep;
     USB_CLASS_CALLBACK_STRUCT_PTR msc_class_callback;
     USB_REQ_CALLBACK_STRUCT_PTR   vendor_req_callback;
     USB_CLASS_CALLBACK_STRUCT_PTR param_callback; 
     DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr; 
}USB_MSD_CONFIG_STRUCT, _PTR_ USB_MSD_CONFIG_STRUCT_PTR;
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param msd_config_ptr    : Configuration paramemter strucutre pointer
 *                            passed by APP.
 * @return status       
 *         MSD Handle           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the MSC Class layer and layers it is dependednt on 
 ******************************************************************************/
MSD_HANDLE USB_Class_MSC_Init
(
   USB_MSD_CONFIG_STRUCT_PTR msd_config_ptr /*[IN]*/
); 

/**************************************************************************//*!
 *
 * @name  USB_Class_MSC_Send_Data
 *
 * @brief 
 *
 * @param msc_handle   :   handle returned from USB_Class_MSC_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
extern uint_8 USB_Class_MSC_Send_Data
(
    MSD_HANDLE          msc_handle,
    uint_8              ep_num,
    uint_8_ptr           buff_ptr,      /* [IN] buffer to send */      
    uint_32             size           /* [IN] length of the transfer */
);

/**************************************************************************//*!
 *
 * @name  USB_MSC_LBA_Transfer
 *
 * @brief 
 *
 * @param MSC_DEVICE_STRUCT_PTR   
 * @param direction       :   transfer direction
 * @param lba_info_ptr    :   buffer to send
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
extern uint_8 USB_MSC_LBA_Transfer
(
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
    boolean direction,
    PTR_LBA_INFO_STRUCT lba_info_ptr
);

#define USB_MSC_Bulk_Send_Data(a,b,c)  USB_Class_MSC_Send_Data(a,msc_obj_ptr->bulk_in_endpoint,b,c)
#define USB_MSC_Bulk_Recv_Data(a,b,c)  _usb_device_recv_data(a,msc_obj_ptr->bulk_out_endpoint,b,c)

#define USB_MSC_Periodic_Task USB_Class_Periodic_Task 

/* Sub Class Functions */

 /**************************************************************************//*!
 *
 * @name  USB_MSC_SCSI_Init
 *
 * @brief The funtion initializes the SCSI parameters and callbacks
 *
 * @param msc_obj_ptr         MSD class object pointer.
 * @param cb:                 event callback
 * @param storage_disk        Ramdisk Memory pointer.
 * @param lb                  logical block
 * @param lab_len             lenght of each logical blocks
 *
 * @return status
 *         USB_OK           : When Successfull
 *         Others           : Errors
 *
 *****************************************************************************/
extern uint_8 USB_MSC_SCSI_Init
(
     MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
     USB_CLASS_CALLBACK_STRUCT_PTR cb,
     PTR_DEVICE_LBA_INFO_STRUCT device_info_ptr,
     uint_32 implementing_disk_drive
);
                                       

/**************************************************************************//*!
 *
 * @name  msc_inquiry_command
 *
 * @brief It requests that information regarding parameters of the Device be
 *        sent to the Host Computer
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/

extern uint_8 msc_inquiry_command (MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                   PTR_CBW cbw_ptr, 
                                   uint_32* csw_residue_ptr, 
                                   uint_8* csw_status_ptr);
/**************************************************************************//*!
 *
 * @name  msc_read_command
 *
 * @brief command requests that device transfer data to the host computer
 *        (read (10) command)
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_read_command (MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                PTR_CBW cbw_ptr, 
                                uint_32* csw_residue_ptr, 
                                uint_8* csw_status_ptr); 
 /**************************************************************************//*!
 *
 * @name  msc_request_sense_command
 *
 * @brief command instructs the Device to transfer sense data to host computer
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return NONE
 *
 *****************************************************************************/
extern uint_8 msc_request_sense_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                        PTR_CBW cbw_ptr, 
                                        uint_32* csw_residue_ptr, 
                                        uint_8* csw_status_ptr); 
/**************************************************************************//*!
 *
 * @name  msc_test_unit_ready_command
 *
 * @brief It provides a means to check if the device is ready
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_test_unit_ready_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                          PTR_CBW cbw_ptr, 
                                          uint_32* csw_residue_ptr, 
                                          uint_8* csw_status_ptr); 
/**************************************************************************//*!
 *
 * @name  msc_verify_command
 *
 * @brief requests that device verifies the data on medium
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_verify_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                 PTR_CBW cbw_ptr,
                                 uint_32* csw_residue_ptr,
                                 uint_8* csw_status_ptr);                                         
/**************************************************************************//*!
 *
 * @name  msc_mode_sense_command
 *
 * @brief command provides a means for a Device to report parameters to Host
 *        Computer.It is a complementary command to the MODE SELECT command.
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_mode_sense_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                     PTR_CBW cbw_ptr, 
                                     uint_32* csw_residue_ptr, 
                                     uint_8* csw_status_ptr);
/**************************************************************************//*!
 *
 * @name  msc_mode_select_command
 *
 * @brief command provides a means for a Device to report parameters to Host
 *        Computer.It is a complementary command to the MODE SENSE command.
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_mode_select_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                      PTR_CBW cbw_ptr, 
                                      uint_32* csw_residue_ptr, 
                                      uint_8* csw_status_ptr);                                     
/**************************************************************************//*!
 *
 * @name  msc_read_capacity_command
 *
 * @brief command provides a means for the host computer to request information
 *        regarding the capacity of the installed medium of the device.
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_read_capacity_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                        PTR_CBW cbw_ptr, 
                                        uint_32* csw_residue_ptr, 
                                        uint_8* csw_status_ptr); 
/**************************************************************************//*!
 *
 * @name  msc_format_unit_command
 *
 * @brief host sends the FORMAT UNIT command to physically format a diskette
 *        according to selected options
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_format_unit_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                      PTR_CBW cbw_ptr, 
                                      uint_32* csw_residue_ptr, 
                                      uint_8* csw_status_ptr); 
/**************************************************************************//*!
 *
 * @name  msc_write_command
 *
 * @brief command requests that the Device write the data transferred by the
 *        Host Computer to the medium.
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_write_command (MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                 PTR_CBW cbw_ptr, 
                                 uint_32* csw_residue_ptr, 
                                 uint_8* csw_status_ptr); 
/**************************************************************************//*!
 *
 * @name  msc_start_stop_unit_command
 *
 * @brief command instructs device to enable or disable media access operations
 *
 * @param controller_ID:        To identify the controller 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_start_stop_unit_command
(
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint_32* csw_residue_ptr,
    uint_8* csw_status_ptr
);
                                 
/**************************************************************************//*!
 *
 * @name  msc_prevent_allow_medium_removal
 *
 * @brief command tells the UFI device to enable or disable the removal of the
 *        medium in the logical unit.
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_prevent_allow_medium_removal(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                               PTR_CBW cbw_ptr, 
                                               uint_32* csw_residue_ptr, 
                                               uint_8* csw_status_ptr); 
/**************************************************************************//*!
 *
 * @name  msc_read_format_capacity_command
 *
 * @brief allows the host to request a list of the possible capacities that
 *        can be formatted on the currently installed medium
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_read_format_capacity_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                               PTR_CBW cbw_ptr, 
                                               uint_32* csw_residue_ptr, 
                                               uint_8* csw_status_ptr);  
                                               
/**************************************************************************//*!
 *
 * @name  msc_send_diagnostic_command
 *
 * @brief requests the device to perform self test
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_send_diagnostic_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                          PTR_CBW cbw_ptr, 
                                          uint_32* csw_residue_ptr, 
                                          uint_8* csw_status_ptr);     
                                                                                  
/**************************************************************************//*!
 *
 * @name  msc_unsupported_command
 *
 * @brief Responds appropriately to unsupported commands
 *
 * @param msc_obj_ptr 
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
extern uint_8 msc_unsupported_command(MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
                                      PTR_CBW cbw_ptr, 
                                      uint_32* csw_residue_ptr, 
                                      uint_8* csw_status_ptr);

/* EOF */
#endif

/* EOF */
