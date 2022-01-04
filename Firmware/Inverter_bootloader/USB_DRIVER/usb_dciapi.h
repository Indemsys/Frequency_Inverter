/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
******************************************************************************
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
**************************************************************************/ /*!
  *
  * @file usb_dciapi.h
  *
  * @author 
  *
  * @version 
  *
  * @date Jun-05-2009
  *
  * @brief The file contains DCI api function definetions .
  *
  *****************************************************************************/

#ifndef _USB_DCIAPI_H
  #define _USB_DCIAPI_H

/******************************************************************************
 * Includes
 *****************************************************************************/
  
  #include "usb_devapi.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
  #define MAX_SUPPORTED_ENDPOINTS (USB_SERVICE_MAX_EP + 1)
/* Maximum endpoints supported */
  #define MIN_SUPPORTED_ENDPOINTS (1)        /* Minimum endpoints supported */
  #define DOUBLE_BUFFERED_ENPOINT_NUMBER (0) /* First double buffered endpoint */

/******************************************************************************
 * Types - None
 *****************************************************************************/

/*****************************************************************************
* Global Functions
*****************************************************************************/
extern INT8U USB_DCI_Init(
                           INT8U    controller_ID
                           );


extern INT8U USB_DCI_Init_EndPoint(
                                    INT8U              controller_ID,
                                    USB_EP_STRUCT_PTR   ep_ptr,
                                    BOOLEAN             double_buffered
                                    );

extern INT8U USB_DCI_Init_EndPoint(
                                    INT8U               controller_ID,
                                    USB_EP_STRUCT_PTR    ep_ptr,
                                    BOOLEAN              flag
                                    );

extern INT8U USB_DCI_Cancel_Transfer(
                                      INT8U    controller_ID,
                                      INT8U    ep_num,
                                      INT8U    direction
                                      );

extern INT8U USB_DCI_Deinit_EndPoint(
                                      INT8U    controller_ID,
                                      INT8U    ep_num,
                                      INT8U    direction
                                      );

extern void USB_DCI_Stall_EndPoint(
                                   INT8U    controller_ID,
                                   INT8U    ep_num,
                                   INT8U    direction
                                   );

extern void USB_DCI_Unstall_EndPoint(
                                     INT8U    controller_ID,
                                     INT8U    ep_num,
                                     INT8U    direction
                                     );

extern void USB_DCI_Get_Setup_Data(
                                   INT8U       controller_ID,
                                   INT8U       ep_num,
                                   INT8U_ptr    buff_ptr
                                   ); 

extern INT8U USB_DCI_Get_Transfer_Status(
                                          INT8U    controller_ID,
                                          INT8U    ep_num,
                                          INT8U    direction
                                          );

extern INT8U USB_DCI_Recv_Data(
                                INT8U           controller_ID,
                                INT8U           ep_num,
                                INT8U_ptr        buff_ptr,
                                INT16U  size
                                );

extern INT8U USB_DCI_Send_Data(
                                INT8U           controller_ID,
                                INT8U           ep_num,
                                INT8U_ptr        buff_ptr,
                                INT16U  size
                                );

extern void  USB_DCI_Set_Address(
                                 INT8U    controller_ID,
                                 INT8U    address
                                 );

extern void USB_DCI_Shutdown(
                             INT8U    controller_ID
                             );

extern void USB_DCI_Assert_Resume(
                                  INT8U    controller_ID
                                  );

extern void Clear_Mem(INT8U *start_addr, INT32U count, INT8U val);

  #define USB_DCI_Cancel_Transfer USB_Device_Cancel_Transfer

  #define USB_DCI_Recv_Data USB_Device_Recv_Data

  #define USB_DCI_Send_Data USB_Device_Send_Data

  #define USB_DCI_Shutdown USB_Device_Shutdown

  #define USB_DCI_Stall_EndPoint USB_Device_Stall_EndPoint

  #define USB_DCI_Unstall_EndPoint USB_Device_Unstall_EndPoint

  #define USB_DCI_Get_Transfer_Status USB_Device_Get_Transfer_Status

  #define USB_DCI_Get_Setup_Data USB_Device_Read_Setup_Data

  #define USB_DCI_Set_Address USB_Device_Set_Address

  #define USB_DCI_Assert_Resume USB_Device_Assert_Resume

#endif


