/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
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
 **************************************************************************//*!
 *
 * @file usb_dci_kinetis.h
 *
 * @author 
 *
 * @version 
 *
 * @date
 *
 * @brief The file contains Macro's and functions needed by the DCI layer.
 *
 *****************************************************************************/

#ifndef _USB_DCI_H
#define _USB_DCI_H
/******************************************************************************
 * Includes
 *****************************************************************************/

#include "app_cfg.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define BYTES_1024              (1024)
#define BYTES_512               (512)
#define ENDPT_EP_STALL_MASK     (0x02)
#define FRAME_HIGH_BYTE_SHIFT    (8)

#ifdef USB_LOWPOWERMODE
typedef enum _stopmode
{
    STOP_MODE1 = 1, /* STOP MODE 1 */
    STOP_MODE2 = 2, /* STOP MODE 2 */
    STOP_MODE3 = 3, /* STOP MODE 3 */
    STOP_MODE4 = 4  /* STOP MODE 4 */
}STOP_MODE;
#endif

/* USBTRC0 Initialization Parameters */
#define _USBPHYEN           (0x01)       /* Use internal transceiver */
#define _USBPUEN            (0x40)       /* Use internal pull-up resistor */
#define _USBREGEN           (0x04)       /* Use the internal regulator */
#define _USBRESET           (0x80)

#define UCFG_VAL            (_USBPUEN|_USBREGEN) 

#define CTL_RESET_VAL (0)             /* value programmed to the CTL 
                                         register in RESET */

#define EP_CTRL     (0x0C)            /* Cfg Control pipe for this endpoint */
#define EP_OUT      (0x08)            /* Cfg OUT only pipe for this endpoint*/
#define EP_IN       (0x04)            /* Cfg IN only pipe for this endpoint */
#define HSHK_EN     (0x01)            /* Enable handshake packet            */
                                      /* Handshake should be disable for
                                         isochorous transfer                */
#define EP_CTL_DIS  (0x10)

#define EP_DISABLE  (0)                                         
                                 
#define TRANSFER_INDEX(x)  (x>>1)                                       

#define MAX_EP_BUFFER_SIZE  USB_MAX_EP_BUFFER_SIZE /*Max Endpoint Buffer Size*/

/* Macro's to check whether corresponding INT_STAT bit is set */
#define BUS_RESET_FLAG(x)               ((x) & 1)  
#define ERROR_FLAG(x)                   ((x) & 2)
#define SOF_TOKEN_FLAG(x)               ((x) & 4)
#define SLEEP_FLAG(x)                   ((x) & 0x10)
#define RESUME_FLAG(x)                  ((x) & 0x20)
#define STALL_FLAG(x)                   ((x) & 0x80)
#define TOKEN_COMPL_FLAG(x)             ((x) & 8)

/* Setup the controller for Remote Wakeup */
#define USB_DCI_WAKEUP				\
{									\
	USB0_ISTAT |= USB_ISTAT_RESUME_MASK; \
	USB0_INTEN &= ~USB_INTEN_RESUMEEN_MASK; \
	USB0_CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;	\
}

/* control endpoint transfer types */
#define USB_TRF_UNKNOWN      			(0xFF)


#define BDT_MIN_BUFFER_SIZE             (16)  /* space occupied by smallest 
                                                           buffer in BDT */
                                                              
#define BDT_MIN_BUFFER_ADDR_INC         (4)   /* min offset increment  
                                                 correspoding to min buffer 
                                                 size */
                                                 
#define BDT_OFFSET_SHIFT                (4)   /* bdt offset shift */

#define INVALID_BDT_INDEX               (0xff)/* invalid bdt index */  

#define ENDPOINT_NUMBER_SHIFT           (4)   /* endpoint shift & mask to  */
#define ENDPOINT_NUMBER_MASK            (0xf0)/* use in setting and getting 
                                                 status */

#define ENDPOINT_DIRECTION_SHIFT        (3)   /* direction shift & mask to */
#define ENDPOINT_DIRECTION_MASK         (0x08)/* be used for STAT byte 
                                                 in BDT */

#define SEND_CONTROL_ENDPOINT_BDT_INDEX (2)   /* BDT Index for Control Endpoint 
                                                 SEND direction */
#define RECV_CONTROL_ENDPOINT_BDT_INDEX (0)   /* BDT Index for Control Endpoint
                                                 RECV direction */
                               
#define EPCTL_STALL                     (0x02)/* Stall bit in Endpoint Control Reg */
                                 
#define USB_SETUP_TOKEN                 (0x0d)/* Setup Token PID */
#define USB_SETUP_DIRECTION             (0x80)/* Data xfer direction for Setup packet */

#define INT_STAT_CLEAR_ALL              (0xbf)/* Value to clear all Interrupts */
#define ERR_STAT_CLEAR_ALL              (0xbf)/* Value to clear all Errors */
#define ERR_ENB_ENABLE_ALL              (0xbf)/* Value to enable all Error Interrupts */
#define INTENB_BUS_RESET_VAL            (0x9f)/* Value to enable Interrupts in Bus Reset */
#define INTENB_DISABLE_ALL_VAL          (0x00)/* Value to disable all Interrupts */

#define MAX_USB_RAM_BUFFER_INDEX        (14)  /* MAX possible RAM buffer Index */

#define EP_START_BUFFER_ADDR            (0x08)/* First USB_RAM buffer offset*/

#define ASSERT_RESUME_DELAY_COUNT       (40000)/* Delay for assert resume */

#define NO_ERRORS                       (0)   /* Init value for error */

#define USB_RAM_EVEN_BUFFER             (0)
#define USB_RAM_ODD_BUFFER              (1)

/******************************************************************************
 * Types
 *****************************************************************************/
#ifdef LONG_SEND_TRANSACTION
  #define LONG_TRANSACTION
#endif 

#ifdef LONG_RECIEVE_TRANSACTION
  #define LONG_TRANSACTION
#endif 

typedef union {
  INT8U Byte;
  struct {
    INT8U EP_HSHK     :1;                                       /* When set this bet enables an endpoint to perform handshaking during a transaction to this endpoint. This bit will generally be set unless the endpoint is Isochronous */
    INT8U EP_STALL    :1;                                       /* When set this bit indicates that the endpoint is stalled */
    INT8U EP_TX_EN    :1;                                       /* This bit, when set, enables the endpoint for TX transfers */
    INT8U EP_RX_EN    :1;                                       /* This bit, when set, enables the endpoint for RX transfers */
    INT8U EP_CTRL_DIS  :1;                                       /* This bit, when set, disables control (SETUP) transfers. When cleared, control transfers are enabled. This applies if and only if the EP_RX_EN and EP_TX_EN bits are also set */
    INT8U             :1; 
    INT8U RETRY_DIS   :1;                                       /* This is a Host mode only bit and is only present in the control register for endpoint 0 (ENDPT0) */
    INT8U HOST_WO_HUB :1;                                       /* This is a Host mode only bit and is only present in the control register for endpoint 0 (ENDPT0) */
  } Bits;
} ENDPT0STR;

#pragma pack(1)
/* This structure is used to hold endpoint paramaetes and the transaction parameters on the IO's happening on them */
typedef struct   
{
	INT16U            len;                 /* endpoint max buffer len */
	INT32U            addr;                /* endpoint buffer addr in USB_RAM */
#ifdef LONG_TRANSACTION                   
    INT8U_ptr         app_buffer;          /* application buffer pointer */
    INT16U            app_len;             /* application buffer len */
    INT16U            curr_offset;         /* current offset for long transactions */
#endif  
    INT8U             flag;                /* zero termination flag */
    INT8U             ep_cbl_indx;         /* Corresponding to the buffer */
    INT8U             direction;           /* Direction (Send/Receive) */  
    INT8U             type;                /* Type of Endpoint */
} 
T_EP_STATE;
#pragma pack()

 /*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern INT8U USB_Device_Call_Service(
    INT8U                    type, 
    PTR_USB_DEV_EVENT_STRUCT  event 
);


#endif
