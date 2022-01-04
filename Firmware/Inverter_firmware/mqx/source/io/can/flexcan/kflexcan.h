#ifndef _kFlexCAN_h_
#define _kFlexCAN_h_ 1
/**HEADER********************************************************************
* 
* Copyright (c) 2008-2010 Freescale Semiconductor;
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
* $FileName: kflexcan.h$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This include file is used to provide constant and structure definitions
*   specific to the FlexCAN Serial Communications Controller
*   Revision History:
*   Apr 21, 2003   2.50          Initial version
*
*END************************************************************************/

#include <mqx.h>
#include <psp.h>


// CAN constants

#define CAN_MIN_TQ  8
#define CAN_MAX_TQ  25

#define CAN_MAX_PROPSEG 8
#define CAN_MAX_RJW 4
#define CAN_MIN_PSEG2 2


// concrete HW settings:
    
#define FLEXCAN_MESSBUF_INT_LEVEL       (3)
#define FLEXCAN_MESSBUF_INT_SUBLEVEL    (4)

#define FLEXCAN_ERROR_INT_LEVEL         (3)
#define FLEXCAN_ERROR_INT_SUBLEVEL      (2)

#define FLEXCAN_BUSOFF_INT_LEVEL        (3)
#define FLEXCAN_BUSOFF_INT_SUBLEVEL     (3)

#define FLEXCAN_WAKEUP_INT_LEVEL        (3)
#define FLEXCAN_WAKEUP_INT_SUBLEVEL     (1)

#define FLEXCAN_MSG_BUFADDR_OFFSET      (128)
#define FLEXCAN_MSG_BUFF_MAXDATA_SIZE   (8)

#ifndef FLEXCAN_IPBUS_CLK
#define FLEXCAN_IPBUS_CLK               (BSP_BUS_CLOCK) /* Indicates internal clock source */
#endif
#ifndef FLEXCAN_OSC_CLK
#define FLEXCAN_OSC_CLK                 (0) /* Must be set to external clock frequency, different to FLEXCAN_IPBUS_CLK! */ 
#endif

// iterface provided:

/*
** CAN interrupt types enum
*/
#define FLEXCAN_INT_BUF                 (0)
#define FLEXCAN_INT_ERR                 (1)
#define FLEXCAN_INT_BOFF                (2)
#define FLEXCAN_INT_WAKEUP              (3)

/*
** CAN commands
*/
#define FLEXCAN_TX                      (0xFF)
#define FLEXCAN_RX                      (0x00)
#define FLEXCAN_EXTENDED                (0xFF)
#define FLEXCAN_STANDARD                (0x00)
#define FLEXCAN_ENABLE                  (0xFF)
#define FLEXCAN_DISABLE                 (0x00)

/*
** Module mode
*/
#define FLEXCAN_NORMAL_MODE             (0)
#define FLEXCAN_LISTEN_MODE             (1)
#define FLEXCAN_TIMESYNC_MODE           (2)
#define FLEXCAN_LOOPBK_MODE             (3)
#define FLEXCAN_BOFFREC_MODE            (4)
#define FLEXCAN_FREEZE_MODE             (5)
#define FLEXCAN_DISABLE_MODE            (6)

/*
** FLEXCAN error codes
*/
#define FLEXCAN_OK                       (0x00)
#define FLEXCAN_UNDEF_ERROR              (FLEXCAN_ERROR_BASE | 0x01)
#define FLEXCAN_MESSAGE14_TX             (FLEXCAN_ERROR_BASE | 0x02)
#define FLEXCAN_MESSAGE15_TX             (FLEXCAN_ERROR_BASE | 0x03)
#define FLEXCAN_MESSAGE_OVERWRITTEN      (FLEXCAN_ERROR_BASE | 0x04)
#define FLEXCAN_NO_MESSAGE               (FLEXCAN_ERROR_BASE | 0x05)
#define FLEXCAN_MESSAGE_LOST             (FLEXCAN_ERROR_BASE | 0x06)
#define FLEXCAN_MESSAGE_BUSY             (FLEXCAN_ERROR_BASE | 0x07)
#define FLEXCAN_MESSAGE_ID_MISSMATCH     (FLEXCAN_ERROR_BASE | 0x08)
#define FLEXCAN_MESSAGE14_START          (FLEXCAN_ERROR_BASE | 0x09)
#define FLEXCAN_MESSAGE15_START          (FLEXCAN_ERROR_BASE | 0x0A)
#define FLEXCAN_INVALID_ADDRESS          (FLEXCAN_ERROR_BASE | 0x0B)
#define FLEXCAN_INVALID_MAILBOX          (FLEXCAN_ERROR_BASE | 0x0C)
#define FLEXCAN_TIMEOUT                  (FLEXCAN_ERROR_BASE | 0x0D)
#define FLEXCAN_INVALID_FREQUENCY        (FLEXCAN_ERROR_BASE | 0x0E)
#define FLEXCAN_INT_ENABLE_FAILED        (FLEXCAN_ERROR_BASE | 0x0F)
#define FLEXCAN_INT_DISABLE_FAILED       (FLEXCAN_ERROR_BASE | 0x10)
#define FLEXCAN_INT_INSTALL_FAILED       (FLEXCAN_ERROR_BASE | 0x11)
#define FLEXCAN_REQ_MAILBOX_FAILED       (FLEXCAN_ERROR_BASE | 0x12)
#define FLEXCAN_DATA_SIZE_ERROR          (FLEXCAN_ERROR_BASE | 0x13)
#define FLEXCAN_MESSAGE_FORMAT_UNKNOWN   (FLEXCAN_ERROR_BASE | 0x14)
#define FLEXCAN_INVALID_DIRECTION        (FLEXCAN_ERROR_BASE | 0x15)
#define FLEXCAN_RTR_NOT_SET              (FLEXCAN_ERROR_BASE | 0x16)
#define FLEXCAN_SOFTRESET_FAILED         (FLEXCAN_ERROR_BASE | 0x17)
#define FLEXCAN_INVALID_MODE             (FLEXCAN_ERROR_BASE | 0x18)
#define FLEXCAN_START_FAILED             (FLEXCAN_ERROR_BASE | 0x19)
#define FLEXCAN_CLOCK_SOURCE_INVALID     (FLEXCAN_ERROR_BASE | 0x1A)
#define FLEXCAN_INIT_FAILED              (FLEXCAN_ERROR_BASE | 0x1B)
#define FLEXCAN_ERROR_INT_ENABLE_FAILED  (FLEXCAN_ERROR_BASE | 0x1C)
#define FLEXCAN_ERROR_INT_DISABLE_FAILED (FLEXCAN_ERROR_BASE | 0x1D)
#define FLEXCAN_FREEZE_FAILED            (FLEXCAN_ERROR_BASE | 0x1E)



#ifdef __cplusplus
extern "C" {
#endif

extern pointer _bsp_get_flexcan_base_address(uint_8);
extern IRQInterruptIndex _bsp_get_flexcan_vector (uint_8,uint_8,uint_32);
extern uint_32 FLEXCAN_Softreset(uint_8);
extern uint_32 FLEXCAN_Start(uint_8);
extern pointer FLEXCAN_Get_msg_object(uint_8,uint_32);
extern uint_32 FLEXCAN_Select_mode(uint_8,uint_32);
extern uint_32 FLEXCAN_Select_clk(uint_8,uint_32);
extern uint_32 FLEXCAN_Initialize(uint_8,uint_32,uint_32,uint_32,uint_32);
extern uint_32 FLEXCAN_Initialize_mailbox(uint_8,uint_32,uint_32, uint_32,uint_32,uint_32,uint_32);
extern uint_32 FLEXCAN_Request_mailbox(uint_8,uint_32,uint_32);
extern uint_32 FLEXCAN_Activate_mailbox(uint_8,uint_32,uint_32);
extern uint_32 FLEXCAN_Lock_mailbox(uint_8,uint_32);
extern uint_32 FLEXCAN_Unlock_mailbox(uint_8);
extern uint_32 FLEXCAN_Set_global_extmask(uint_8,uint_32);
extern uint_32 FLEXCAN_Set_buf14_extmask(uint_8,uint_32);
extern uint_32 FLEXCAN_Set_buf15_extmask(uint_8,uint_32);
extern uint_32 FLEXCAN_Set_global_stdmask(uint_8,uint_32);
extern uint_32 FLEXCAN_Set_buf14_stdmask(uint_8,uint_32);
extern uint_32 FLEXCAN_Set_buf15_stdmask(uint_8,uint_32);
extern boolean FLEXCAN_Tx_successful(uint_8);
extern uint_32 FLEXCAN_Tx_mailbox(uint_8,uint_32,pointer);
extern uint_32 FLEXCAN_Rx_mailbox(uint_8,uint_32,pointer);
extern uint_32 FLEXCAN_Disable_mailbox(uint_8,uint_32);
extern uint_32 FLEXCAN_Request_message(uint_8,uint_32,uint_32);
extern uint_32 FLEXCAN_Rx_message(uint_8,uint_32,uint_32_ptr,uint_32,uint_32_ptr,pointer,uint_32);
extern uint_32 FLEXCAN_Tx_message(uint_8,uint_32,uint_32,uint_32,uint_32,pointer);
extern uint_32 FLEXCAN_Read(uint_8,uint_32,uint_32_ptr);
extern uint_32 FLEXCAN_Write(uint_8,uint_32,uint_32);
extern uint_32 FLEXCAN_Get_status(uint_8,uint_32_ptr);
extern uint_32 FLEXCAN_Update_message(uint_8,pointer,uint_32,uint_32,uint_32);
extern uint_32 FLEXCAN_Int_enable(uint_8,uint_32);
extern uint_32 FLEXCAN_Error_int_enable(uint_8);
extern uint_32 FLEXCAN_Int_disable(uint_8,uint_32);
extern uint_32 FLEXCAN_Error_int_disable(uint_8);
extern uint_32 FLEXCAN_Install_isr(uint_8,uint_32,INT_ISR_FPTR);
extern uint_32 FLEXCAN_Install_isr_err_int(uint_8,INT_ISR_FPTR);
extern uint_32 FLEXCAN_Install_isr_boff_int(uint_8,INT_ISR_FPTR);
extern uint_32 FLEXCAN_Install_isr_wake_int(uint_8,INT_ISR_FPTR);
extern  void   _FLEXCAN_ISR(pointer);
extern uint_32 FLEXCAN_Int_status(uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
