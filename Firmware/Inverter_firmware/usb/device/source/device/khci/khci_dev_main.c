/**HEADER**********************************************************************
*
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved
*
*******************************************************************************
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
* $FileName: khci_dev_main.c$
* $Version : 3.8.25.0$
* $Date    : Sep-26-2012$
*
* Comments:
*
*   This file contains the main usbfs USB Device Controller interface
*   functions.
*
*END************************************************************************/

/******************************************************************************
 *               BUFFER DISCRIPTOR TABLE (BDT) DISCRIPTION                    *
 ******************************************************************************/
 /**
 * The USB-FS implements a Buffer Descriptor Table (BDT) in system memory. The
 * BDT resides on a 512 byte boundary in system memory and is pointed to by the
 * BDT Page Registers. Every endpoint direction requires two eight-byte Buffer
 * Descriptor entries.Therefore, a system with 16 fully bidirectional endpoints
 * would require 512 bytes of system memory to implement the BDT.The two Buffer
 * Descriptor (BD) entries allows for an EVEN BD and ODD BD entry for each
 * endpoint direction. This allows the microprocessor to process one BD while
 * the USB-FS is processing the other BD. Double buffering BDs in this way
 * allows the USB-FS to easily transfer data at the maximum throughput provided
 * by USB.
 *
 * Because the buffers are shared between the microprocessor and the USB-FS a
 * simple semaphore mechanism is used to distinguish who is allowed to update
 * the BDT and buffers in system memory. A semaphore bit, the OWN bit, is
 * cleared to 0 when the BD entry is owned by the microprocessor. The
 * microprocessor is allowed read and write access to the BD entry and the
 * buffer in system memory when the OWN bit is 0.
 * When the OWN bit is set to 1, the BD entry and the buffer in system memory
 * are owned by the USB-FS. The USB-FS now has full read and write access and
 * the microprocessor should not modify the BD or its corresponding data buffer.
 * The BD also contains indirect address pointers to where the actual buffer
 * resides in system memory.
 ******************************************************************************
 *                 BUFFER DISCRIPTOR FORMAT DISCRIPTION
 ******************************************************************************
 * The Buffer Descriptors have different meaning based on who is reading the BD
 * in memory.The USB-FS Controller uses the data stored in BDs to determine:
 *
 *       # Who owns the buffer in system memory
 *       # Data0 or Data1 PID
 *       # Release Own upon packet completion
 *       # No address increment (FIFO Mode)
 *       # Data toggle synchronization enable
 *       # How much data is to be transmitted or received
 *       # Where the buffer resides in system memory
 *
 * While the microprocessor uses the data stored in the BDs to determine:
 *       # Who owns the buffer in system memory
 *       # Data0 or Data1 PID
 *       # The received TOKEN PID
 *       # How much data was transmitted or received
 *       # Where the buffer resides in system memory
 *
 *    ------ ------ ------ ------ -------- ---------- ---------- ---------- ---------- ---------- ----------
 *   |31-26 |25-16 | 15-8 |  7   |   6    |   5      |   4      |   3      |   2      |   1      |   0      |
 *   |RSVD  | BC   | RSVD | OWN  |DATA0/1 |TOK_PID[3]|TOK_PID[2]|TOK_PID[1]|TOK_PID[0]|TOK_PID[n]| reserved |
 *    ------ ------ ------ ------ -------- ---------- ---------- ---------- ---------- ---------- ----------
 *   |                             ADDRESS[31--0]                                                           |
 *   |                                                                                                      |
 *    -------------------------------------------------------------------------------------------------------
 *
 * This Buffer Discriptor table is represented by the variable "BDT_BASE"
 * defined in file usbfs_dev_main.h. Macros such as "BD_ADDR_RX" and
 * "BD_ADDR_TX" is used to manipulate the address field and Macros such as
 * BD_CTRL_RX and BD_CTRL_TX is used to manipulate the control fields.
 */


/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "dev_cnfg.h"
#include "devapi.h"

#include "khci_dev_main.h"

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* TODO: rewrite following defintions to bit mask */
#define ISTAT_CLEAR_ALL         (0xFF) /* Value to clear all pending interrupts */
#define ERRSTAT_CLEAR_ALL       (0xFF) /* Value to clear all errors */
#define ERREN_ENABLE_ALL        (0xFF) /* Value to enable all error interrupts */
#define INTEN_BUS_RESET_VAL     (0x9F) /* Value to enable interrupts in bus reset */

#if defined( __ICCCF__ ) || defined( __ICCARM__ )
    #pragma segment="USB_BDT_Z"
    #pragma data_alignment=512
    __no_init static uint_8 bdt[512] @ "USB_BDT_Z";
#elif defined (__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".usb_bdt"))) static uint_8 bdt[512] = { 1 };
#else
  #pragma define_section usb_bdt ".usb_bdt" RW
  __declspec(usb_bdt) static uint_8 bdt[512] = { 1 };    // DO NOT REMOVE INITIALIZATION !!! bug in CW (cw 7.1) - generate wrong binary code, data
#endif  

boolean g_zero_pkt_send = FALSE;

#ifdef KHCI_DEBUG
struct tk_dne_log {
    uint_32 len;
    uint_8 ep_num;
    uint_8 dir;
    uint_8 pid;
};
volatile static uint_16 tdl_index = 0;
volatile static struct tk_dne_log tdl[1024] = { 0 }; /* note, the array is for 1024 records only */
volatile static uint_16 pkb_index = 0;
volatile static uint_8 pkb[1024][8] = { 0 };
volatile static uint_32 pkbaddr[1024] = { 0 };
#   define KHCI_DEBUG_LOG(a, b, c, d) \
  { \
    tdl[tdl_index].ep_num = a; \
    tdl[tdl_index].dir = b; \
    tdl[tdl_index].len = c; \
    tdl[tdl_index].pid = d; \
    tdl_index++; \
  }
  
#   define KHCI_SAVE_SETUP(source) \
  { \
    pkb[pkb_index][0] = *((uint_8 *)source + 0); \
    pkb[pkb_index][1] = *((uint_8 *)source + 1); \
    pkb[pkb_index][2] = *((uint_8 *)source + 2); \
    pkb[pkb_index][3] = *((uint_8 *)source + 3); \
    pkb[pkb_index][4] = *((uint_8 *)source + 4); \
    pkb[pkb_index][5] = *((uint_8 *)source + 5); \
    pkb[pkb_index][6] = *((uint_8 *)source + 6); \
    pkb[pkb_index][7] = *((uint_8 *)source + 7); \
    pkb_index++; \
  }
  
#   define KHCI_SAVE_BUFADDR(addr) \
  { \
    pkbaddr[pkb_index] = (uint_32)(addr); \
  }
    
#else
#   define KHCI_DEBUG_LOG(a, b, c, d)
#   define KHCI_SAVE_SETUP(source)
#   define KHCI_SAVE_BUFADDR(addr)
#endif

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
void _usb_device_alloc_XD
(
    _usb_device_handle handle,
    /* [IN] the dTD to enqueue */
    XD_STRUCT_PTR *xd_ptr
);

 /*****************************************************************************
 * Local Functions
 *****************************************************************************/
 /*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_alloc_XD
*  Returned Value : void
*  Comments       :
*        Allocates an XD from the free XD ring.
*
*END*-----------------------------------------------------------------*/
void _usb_device_alloc_XD
(
    _usb_device_handle handle,
    /* [IN] the dTD to enqueue */
    XD_STRUCT_PTR *xd_ptr_ptr
)
{   /* Body */
    volatile USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr = handle;
    
    /* This function can be called from any context, and it needs mutual
      exclusion with itself.*/
    USB_lock();
    
    /* Get a transfer descriptor for the specified endpoint 
    ** and direction 
    */
    USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, *xd_ptr_ptr);
   
    usb_dev_ptr->XD_ENTRIES--;

    USB_unlock();
} /* Endbody */

/******************************************************************************
 *
 * @name  _usb_next_setup_token_prep
 *
 * @brief The function prepares for next setup token
 *
 * @param state_ptr:      Device info Structure.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_next_setup_token_prep(USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr)
{
    XD_STRUCT_PTR xd_ptr_temp;

    /* prepare XD queue for RECV CONTROL ENDPOINT*/
    _usb_device_alloc_XD (state_ptr, &xd_ptr_temp);
    xd_ptr_temp->EP_NUM = USB_CONTROL_ENDPOINT;
    xd_ptr_temp->BDIRECTION = USB_RECV;
    xd_ptr_temp->WTOTALLENGTH = ZERO_LENGTH;
    xd_ptr_temp->EP_TYPE = USB_CONTROL_PIPE;
    xd_ptr_temp->DONT_ZERO_TERMINATE = TRUE;
    xd_ptr_temp->WSTARTADDRESS = (uint_8_ptr) LONG_LE_TO_HOST(
        BD_ADDR_RX(USB_CONTROL_ENDPOINT,
        state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].rx_buf_odd));

    USB_XD_QUEUE_ENQUEUE(&state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].xd_queue_recv, xd_ptr_temp);
    
    /* toggle send buffer */
    state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].tx_buf_odd ^= 1;
    
    /* configure data pid for setup token and give control to SEI*/
    state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].rx_data0 = 0;
    BD_CTRL_RX(USB_CONTROL_ENDPOINT,
        state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].rx_buf_odd) =
    LONG_LE_TO_HOST((uint_32)(USB_BD_BC(
        state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].max_packet_size) |
        USB_BD_OWN | USB_BD_DTS | USB_BD_DATA01(state_ptr->EP_INFO[USB_CONTROL_ENDPOINT].rx_data0)));
    /* setup token is always on DATA0 PID */
    return;
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_ep_read
 *
 * @brief The function reads the endpoint buffers
 *
 * @param state_ptr:      Device info Structure.
 * @param ep_num:         endpoint number
 * @param buf_ptr:        buffer to receive in
 * @param buf_num_bytes:  number of bytes to read
 *
 * @return USB_OK         When Successfull
 *****************************************************************************/
static USB_STATUS _usb_device_usbfs_ep_read
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr,
     /*[IN]*/
    uint_8 ep_num,
     /*[OUT]*/
    uint_8_ptr buf_ptr,
     /*[IN]*/
    uint_32 buf_num_bytes
)
{
    volatile USB_MemMapPtr usb_ptr;
    
    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    
    _int_disable();
    
    /* USB data is directly transferred to App Buffer to Avoid one level of
     * memcpy. (i.e from Endpoint buffer to App buffer). So a hack here
     * is been provided. This hack stores the current endpoint buffer
     * address to a variable and programes the buffer address in the BDT
     * as APP buffer. Later when TOKEN_DNE interrupt is received for this Token
     * this buffer address saved is restored. Please note that at present
     * App should not release the buffer passed till he gets a notification from
     * interrupt handler.
     */
    
    state_ptr->EP_INFO[ep_num].save_recv_addr = 
        LONG_LE_TO_HOST(BD_ADDR_RX(ep_num,
            state_ptr->EP_INFO[ep_num].rx_buf_odd));

    BD_ADDR_RX(ep_num, state_ptr->EP_INFO[ep_num].rx_buf_odd) =
        LONG_LE_TO_HOST((uint_32)buf_ptr);

    /* Program number of bytes to be received and give
     * the Control to the SEI
     */
    BD_CTRL_RX(ep_num, state_ptr->EP_INFO[ep_num].rx_buf_odd) =
        LONG_LE_TO_HOST(USB_BD_BC(buf_num_bytes) |
            USB_BD_OWN | USB_BD_DTS | USB_BD_DATA01(state_ptr->EP_INFO[ep_num].rx_data0));
    
    _int_enable ();
    usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_ep_write
 *
 * @brief The function writes the endpoint buffers
 *
 * @param state_ptr:      Device info Structure.
 * @param ep_num:         endpoint number
 * @param buf_ptr:        buffer to send from
 * @param buf_num_bytes:  number of bytes to write
 * @param bytes_written_ptr: buffer that will  contian Number of bytes written
 *                           to device.
 *
 * @return USB_OK         When Successfull
 *****************************************************************************/
static USB_STATUS _usb_device_usbfs_ep_write
(
    /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr, 
    /*[IN]*/
    uint_8 ep_num,
    /*[IN]*/
    uint_8_ptr buf_ptr,
    /*[IN]*/
    uint_32 buf_num_bytes,
     /*[OUT]*/
    uint_32_ptr bytes_written_ptr 
)
{
    volatile USB_MemMapPtr usb_ptr;
    uint_16 max_packet_size;
    
    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* If the number of bytes to be sent is greater than the
     * maximum data that can be sent on the USB bus, then split the
     * transaction, into multiple transaction.
     */
    max_packet_size = state_ptr->EP_INFO[ep_num].max_packet_size;
    
    if (buf_num_bytes > max_packet_size)
    {
        buf_num_bytes = max_packet_size;
    }
    
    *bytes_written_ptr = buf_num_bytes;
    
    /* Program the endpoint buffer address in BDT
     * from where DMA will pick the Data to be sent over USB bus.
     */
    BD_ADDR_TX(ep_num, state_ptr->EP_INFO[ep_num].tx_buf_odd) =
        LONG_LE_TO_HOST((uint_32)buf_ptr);
    /* Program the number of bytes to be sent in BDT and Give
     * the onership to SEI
     */
    BD_CTRL_TX(ep_num, state_ptr->EP_INFO[ep_num].tx_buf_odd) =
        LONG_LE_TO_HOST((uint_32)(USB_BD_BC(buf_num_bytes) |
            USB_BD_OWN | USB_BD_DTS |
            USB_BD_DATA01(state_ptr->EP_INFO[ep_num].tx_data0)));
    
    usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    return USB_OK;
}
    
/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_reset_ep_state
 *
 * @brief This Function Resets all the parameters required for End Point
 *        Initialization
 *
 * @param state_ptr: Device info Structure.
 * @return USB_OK         When Successfull
 *****************************************************************************/
static USB_STATUS _usb_device_usbfs_reset_ep_state
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr
)
{
    volatile USB_MemMapPtr usb_ptr;
    _mqx_int ep;
    
    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Clear all the error Status registor */
    usb_ptr->ERRSTAT = ERRSTAT_CLEAR_ALL;
    
    /*Reset all ODD and Even BDTs to Zero */
    usb_ptr->CTL |= USB_CTL_ODDRST_MASK;
    
    /* Initialize the address as zero in the Address register of USB IP*/
    usb_ptr->ADDR = 0;  
    
    for (ep = 0; ep < USB_MAX_ENDPOINTS; ep++)
    {
        /* Clearing all buffer descriptors for both ODD and even
         * for Both Receive and Transmit direction
         */
        BD_CTRL_RX(ep, EVEN_BUFF) = 0;
        BD_CTRL_RX(ep, ODD_BUFF) = 0;

        BD_CTRL_TX(ep, EVEN_BUFF) = 0;
        BD_CTRL_TX(ep, ODD_BUFF) = 0;

        state_ptr->EP_INFO[ep].rx_buf_odd = EVEN_BUFF;
        state_ptr->EP_INFO[ep].tx_buf_odd = EVEN_BUFF;
        state_ptr->EP_INFO[ep].data_buffer = NULL;
        state_ptr->EP_INFO[ep].save_recv_addr = USB_UNINITIALIZED_VAL_32;
        
        if(ep == USB_CONTROL_ENDPOINT)
        {
            state_ptr->EP_INFO[ep].tx_buf_odd = ODD_BUFF;
        }

        /* Initialize All End Point Control Registers with default value of 0*/
        usb_ptr->ENDPOINT[ep].ENDPT = 0;
    }
    
    g_zero_pkt_send = FALSE;
    
    usb_ptr->CTL &= ~USB_CTL_ODDRST_MASK;
    
    /* initializing device address to default  address
     * value for USB in Device strucutre.
     */
    state_ptr->G.DEVICE_ADDRESS = 0;

    /* Set Default state of USB in Device Structure.*/
    state_ptr->G.USB_STATE = USB_STATE_DEFAULT;

    /* Enable All Error Interrupts */
    usb_ptr->ERREN = ERREN_ENABLE_ALL;

    /* Enable All Interrupts except RESUME */
    usb_ptr->INTEN = INTEN_BUS_RESET_VAL;

    /* SEI if, has suspended packet trasmission resume packet transmission by
     * clearing TXD_SUSPEND in CTL register.
     */
    usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_process_transmit_request
 *
 * @brief : Service TOKEN DONE Interrupt when there is transmission of packet.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 * @param ep_num:    endpoint number
 * @param buffer_ptr:Holds Data that is send to Host.
 * @return USB_STATUS_TRANSFER_PENDING when there is split transaction or
 *                   USB_OK.
 *****************************************************************************/
static USB_STATUS _usb_device_usbfs_process_transmit_request
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr, 
    /*[IN]*/
    uchar ep_num,
     /*[OUT]*/
    uint_8** buffer_ptr
)
{
    volatile XD_STRUCT_PTR xd_ptr;
    
    USB_XD_QUEUE_GET_HEAD(&state_ptr->EP_INFO[ep_num].xd_queue_send,&xd_ptr);

    *buffer_ptr = (uint_8_ptr) LONG_LE_TO_HOST(BD_ADDR_TX(ep_num,
        state_ptr->EP_INFO[ep_num].tx_buf_odd));

    /* set DATA PID and buffer for next send transaction */
    state_ptr->EP_INFO[ep_num].tx_buf_odd ^= 1;
    state_ptr->EP_INFO[ep_num].tx_data0 ^= 1;

    /* If any split transaction is pending initiate that. */
    if((xd_ptr != NULL)&&(xd_ptr->WSOFAR != xd_ptr->WTOTALLENGTH))
    {
        _usb_dci_usbfs_send(state_ptr, xd_ptr);
        return USB_STATUS_TRANSFER_PENDING;
    }

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_process_receive_request
 *
 * @brief : Service TOKEN DONE Interrupt when there is receive packet scenario.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function
 *                   USB_install_isr during ISR installation.
 * @param ep_num:    endpoint number
 * @param stat:      status of last transaction.
 * @param buffer_ptr:buffer holding data received from Host.
 *
 * @return USB_STATUS_TRANSFER_PENDING when there is split transaction else
 *                   USB_OK
 *****************************************************************************/
#if (__CWCC__ >= 4)
    /* Workaround for CodeWarrior 10.x ARM C compiler and optimization level > 1 */
    #pragma optimization_level 1
#endif

static USB_STATUS _usb_device_usbfs_process_receive_request
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr, 
    /*[IN]*/
    uchar ep_num, 
    /*[IN]*/
    uchar stat, 
     /*[OUT]*/
    uint_8** buffer_ptr
)
{
    volatile XD_STRUCT_PTR xd_ptr;
    uchar dir,buf_odd,token_pid;
    uint_32 len;
    
    USB_XD_QUEUE_GET_HEAD(&state_ptr->EP_INFO[ep_num].xd_queue_recv,&xd_ptr);
    
    *buffer_ptr = (uint_8_ptr) LONG_LE_TO_HOST(BD_ADDR_RX(ep_num,
        state_ptr->EP_INFO[ep_num].rx_buf_odd));
    KHCI_SAVE_BUFADDR(*buffer_ptr);
    KHCI_SAVE_SETUP(*buffer_ptr);
    
    dir = (stat & USB_STAT_TX_MASK) ? USB_SEND : USB_RECV;
    buf_odd = (stat & USB_STAT_ODD_MASK) ? ODD_BUFF : EVEN_BUFF;
    
    /* Get length of Data transmited or recevied in the last transaction. */
    len = HOST_TO_LE_LONG(BD_CTRL(ep_num,dir,buf_odd))>>16 & 0x3ff;
    
    token_pid = (uint_8)((HOST_TO_LE_LONG(BD_CTRL(ep_num,dir,buf_odd)) >> 2) & 0x0000000f);
    
    if((ep_num == USB_CONTROL_ENDPOINT) && (token_pid == USB_SETUP_TOKEN))
    {
        /* cancel any pending transfers in SEND QUEUE if present*/
        (void)_usb_device_cancel_transfer((_usb_device_handle)state_ptr,
            ep_num, USB_SEND);
        /* for first In/OUT after setup token DATA PID has to be always one */
        state_ptr->EP_INFO[ep_num].tx_data0 = 1;
        state_ptr->EP_INFO[ep_num].rx_data0 = 1;
        state_ptr->EP_INFO[ep_num].rx_buf_odd ^= 1;
        state_ptr->EP_INFO[ep_num].tx_buf_odd ^= 1;
    }
    else
    {
        /* set DATA PID and buffer for next recv transaction */
        state_ptr->EP_INFO[ep_num].rx_data0 ^= 1;
        state_ptr->EP_INFO[ep_num].rx_buf_odd ^= 1;
        
        if(state_ptr->EP_INFO[ep_num].save_recv_addr != 
            USB_UNINITIALIZED_VAL_32)
        {
            /* save original address */
            BD_ADDR_RX(ep_num, state_ptr->EP_INFO[ep_num].rx_buf_odd^1) =
                HOST_TO_LE_LONG(state_ptr->EP_INFO[ep_num].save_recv_addr);
            state_ptr->EP_INFO[ep_num].save_recv_addr = 
                USB_UNINITIALIZED_VAL_32;
        }
        
        /* if data in recv is queued -- split recv */
        if((xd_ptr)&&((xd_ptr->WSOFAR < xd_ptr->WTOTALLENGTH) && len))
        {
            _usb_dci_usbfs_recv(state_ptr, xd_ptr);
            return USB_STATUS_TRANSFER_PENDING;
        }
    }

    return USB_OK;
}

#if (__CWCC__ >= 4)
    /* End of workaround for CodeWarrior 10.x ARM C compiler and optimization level > 1 */
#pragma optimization_level reset
#endif

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_err_intr
 *
 * @brief : Service Error Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_err_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr 
)
{
    uchar device_error = 0, stat, ep_num;
    volatile USB_MemMapPtr usb_ptr;
    USB_EVENT_STRUCT event;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */

    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }
    
    /* Get the status of previous transaction*/
    stat = usb_ptr->STAT;

    /* Clear the Error Interrupt */
    usb_ptr->ISTAT = USB_ISTAT_ERROR_MASK;
    
    /* Get the Endpoint number on which the transaction occured.
     * This is (7 - 5) [3:0] bit in STAT register.
     */
    ep_num = (uint_8)((stat & ENDPOINT_NUMBER_MASK) >> ENDPOINT_NUMBER_SHIFT);
    
    /* Read the ERRSTAT register to determine the source of the error
     * It is Andded with ERREN register to find out which of the
     * Error was enabled and report only the enabled error to the App layer
     */
    device_error = (uint_8)(usb_ptr->ERRSTAT & usb_ptr->ERREN);
    
    #ifdef _DEV_DEBUG
        printf("USB Err: 0x%x\n", device_error);
    #endif   
    
    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.ep_num = ep_num;
    event.setup = FALSE;
    event.direction = (boolean)(stat >> 3 & 1);
    event.buffer_ptr = (uchar*)&device_error;
    event.len = ZERO_LENGTH;
    
    /* Invoke Service Call */
    (void)_usb_device_call_service(USB_SERVICE_ERROR,&event);
    
    /*clear all errors*/
    usb_ptr->ERRSTAT = ERRSTAT_CLEAR_ALL; 
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_reset_intr
 *
 * @brief : Service reset Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_reset_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr 
)
{
    uint_8 cnt=0;
    uchar stat,ep_num;
    volatile USB_MemMapPtr usb_ptr;
    USB_EVENT_STRUCT event;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */

    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }
    
    /* Clear Reset Interrupt */
    usb_ptr->ISTAT = USB_ISTAT_USBRST_MASK;
    
    /* Get the status of previous transaction*/
    stat = usb_ptr->STAT;
    
    /* Get the Endpoint number on which the transaction occured.
     * This is (7 - 5) [3:0] bit in STAT register.
     */
    ep_num = (uint_8)((stat & ENDPOINT_NUMBER_MASK) >> ENDPOINT_NUMBER_SHIFT);
    
    /* De-Init All the End Point.  */
    for (cnt = 0; cnt < USB_MAX_ENDPOINTS; cnt++)
    {
        _usb_dci_usbfs_deinit_endpoint(state_ptr,cnt,USB_RECV);
        _usb_dci_usbfs_deinit_endpoint(state_ptr,cnt,USB_SEND);
    }

    /* Re-Initialize All the end point */
    _usb_device_usbfs_reset_ep_state((USB_KHCI_DEV_STATE_STRUCT_PTR)state_ptr);
    
    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.ep_num = ep_num;
    event.setup = FALSE;
    event.direction = USB_RECV;
    event.buffer_ptr = (uchar*)NULL;
    event.len = ZERO_LENGTH;
    /* Inform Upper Layer of Reset Signal.
     * Remember Upper layer i.e class driver invokes
     * _usb_dci_usbfs_init_endpoint() as part of reset callback
     */
    (void)_usb_device_call_service(USB_SERVICE_BUS_RESET,&event);
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_tk_dne_intr
 *
 * @brief : Service Error Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. It is the same
 *                   stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_tk_dne_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr 
)
{
    uint_8 ep_num,stat,dir,buf_odd,setup,token_pid;
    uint_32 len = 0;
    USB_STATUS error;
    uint_32 buf_num_bytes;
    volatile USB_MemMapPtr usb_ptr;
    volatile XD_STRUCT_PTR xd_ptr;
    uint_8 * buffer_ptr = NULL;
    USB_EVENT_STRUCT event;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }

    /* Get the status of previous transaction*/
    stat = usb_ptr->STAT;

    /* Clear TOKEN  Done Interrupt. This clearing of TOKEN_DONE
     * should happen after STAT register is read. This is becouse
     * STAT register has four byte fifo and if TOKEN_DONE is cleared
     * in INT_STAT then the content of STAT register is updated with
     * the next value in the STAT fifo.
     */
    usb_ptr->ISTAT = USB_ISTAT_TOKDNE_MASK;

    /* Get the Endpoint number on which the transaction occured.
     * This is (7 - 5) [3:0] bit in STAT register.
     */
    ep_num = (uint_8)((stat & ENDPOINT_NUMBER_MASK) >> ENDPOINT_NUMBER_SHIFT);

    /* Get the Direction of transaction. (i.e the transaction was a receive
     * operation or transaction was a Transmit operation).It is Bit [3] in STAT
     * register.
     */
    dir = (stat & USB_STAT_TX_MASK) ? USB_SEND : USB_RECV;

    /* Get the last buffer transaction was in ODD buffer or Even Buffer*/
    buf_odd = (stat & USB_STAT_ODD_MASK) ? ODD_BUFF : EVEN_BUFF;

    /* Get length of Data transmited or recevied in the last transaction. */
    len = HOST_TO_LE_LONG(BD_CTRL(ep_num,dir,buf_odd))>>16 & 0x3ff;

    /* Get PID for this token */
    token_pid = (uint_8)((HOST_TO_LE_LONG(BD_CTRL(ep_num,dir,buf_odd))>>2) & 0x0000000f);

    /* if token PID is a setup token */
    setup = (token_pid == USB_SETUP_TOKEN) ? TRUE : FALSE;
    /* DEQUEUE the XD queue for previous transaction and perform split
     * transaction if required
     */
    
    KHCI_DEBUG_LOG(ep_num, dir, len, token_pid);
    
    if(dir)
    { /* direction is USB_SEND*/
        /* Get head of the send queue */
        USB_XD_QUEUE_GET_HEAD(&state_ptr->EP_INFO[ep_num].xd_queue_send,&xd_ptr);
        /* updating the WSOFAR field */
        xd_ptr->WSOFAR += len;
        /* total number of bytes send are updated for upper layer */
        event.len = xd_ptr->WSOFAR;
        /* buffer address is updated for upper layer */
        event.buffer_ptr = xd_ptr->WSTARTADDRESS; 
        /* Get the number of Bytes left to be transmitted. If it is not zero
         * its a split transaction and donot delete the XD pointer
         */
        buf_num_bytes = (uint_32)(xd_ptr->WTOTALLENGTH - xd_ptr->WSOFAR);
        
        /* dequeue if all bytes have been send or the last send transaction was
           of length less then the max packet size length configured for
           corresponding endpoint */
        if((buf_num_bytes == 0)||(state_ptr->EP_INFO[ep_num].max_packet_size >
            len))
        {
            xd_ptr->BSTATUS = USB_STATUS_IDLE;
            /* Remove from the EP_Info send queue*/
            USB_XD_QUEUE_DEQUEUE(&state_ptr->EP_INFO[ep_num].xd_queue_send, &xd_ptr);
            /* Add it to the free queue */
            if (xd_ptr != NULL)
            _usb_device_free_XD(state_ptr, xd_ptr);
            //_usb_device_free_XD(xd_ptr);
        }
        /* Process the Transmit token Done. If it was split transaction,
         * start transmit for the remaing data. Below function returns USB_OK
         * if all data is transmitted.
         */
        if(_usb_device_usbfs_process_transmit_request(state_ptr,ep_num,
            &buffer_ptr) != USB_OK)
            return;/*if above call returned USB_STATUS_TRANSFER_PENDING */
        
        /* to send zero bytes data on CONTROL ENDPOINT TO MARK END OF TRANSFER*/
        /* this is needed only when last send transaction was equal to
           max_buffer_size and there is no more data to send*/
        if((len==state_ptr->EP_INFO[ep_num].max_packet_size)&&
            (buf_num_bytes == 0)&&(ep_num==USB_CONTROL_ENDPOINT))
        {
            g_zero_pkt_send = TRUE;
            (void)_usb_device_send_data((_usb_device_handle)state_ptr,ep_num,
                NULL,0);
            return;
        }
    }
    else
    {/* direction is USB_RECV*/
        if(g_zero_pkt_send == TRUE)
        {
            g_zero_pkt_send = FALSE;
        }
        
        /* Get head of the send queue */
        USB_XD_QUEUE_GET_HEAD(&state_ptr->EP_INFO[ep_num].xd_queue_recv,
            &xd_ptr);
        xd_ptr->WSOFAR += len;/* updating the WSOFAR field */
        /* total number of bytes received are updated for upper layer */
        event.len = xd_ptr->WSOFAR;
        /* buffer address is updated for upper layer */
        event.buffer_ptr = xd_ptr->WSTARTADDRESS;
        /* check if number of bytes to be received is zero or if the
         * recevied packet is for Zero size packet.
         */
        buf_num_bytes = (uint_32)(xd_ptr->WTOTALLENGTH - xd_ptr->WSOFAR);


      /* dequeue if all bytes have been received or the last send transaction
         was of length less then the max packet size length configured for
         corresponding endpoint */
        if((!xd_ptr->WTOTALLENGTH)||(buf_num_bytes == 0)||
           (state_ptr->EP_INFO[ep_num].max_packet_size > len))
        {
            xd_ptr->BSTATUS = USB_STATUS_IDLE;
            /* Remove from the EP_Info send queue*/
            USB_XD_QUEUE_DEQUEUE(&state_ptr->EP_INFO[ep_num].xd_queue_recv, &xd_ptr);
            /* Add it to the free queue */
            if (xd_ptr != NULL)
            _usb_device_free_XD(state_ptr, xd_ptr);
        }
        /* Process the receive token Done. If it was split transaction,
         * start receive for the remaing data. Below function returns USB_OK
         * if all data is received.
         */

        if ( _usb_device_usbfs_process_receive_request(state_ptr,ep_num,
            stat,&buffer_ptr)!=USB_OK)
        {
            return;/*if above call returned USB_STATUS_TRANSFER_PENDING */  
        }       
    }

    /* prepare for next setup token if needed*/
    if((ep_num == USB_CONTROL_ENDPOINT)&&(!len)&&(g_zero_pkt_send ==FALSE))
    {
        _usb_next_setup_token_prep(state_ptr);
    }

    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.ep_num = ep_num;
    event.setup = setup;
    event.direction = (boolean)(stat >> 3 & 1);

    /* propagate control to upper layers for processing */
    error = _usb_device_call_service(ep_num,&event);

    usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
 }

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_sleep_intr
 *
 * @brief : Service Sleep Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_sleep_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr
)
{
    volatile USB_MemMapPtr usb_ptr;
    USB_EVENT_STRUCT event;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }
        
    usb_ptr->ISTAT = USB_ISTAT_RESUME_MASK;
    
    /* clear suspend interrupt */
    usb_ptr->ISTAT = USB_ISTAT_SLEEP_MASK;
    
    (void)_usb_device_get_status((_usb_device_handle)state_ptr, 
        USB_STATUS_DEVICE_STATE, &(state_ptr->G.USB_DEV_STATE_B4_SUSPEND));
        
    (void)_usb_device_set_status((_usb_device_handle)state_ptr,
        (uint_8)USB_STATUS_DEVICE_STATE, (uint_16)USB_STATE_SUSPEND);
    
    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.ep_num = (uint_8)USB_UNINITIALIZED_VAL_32;
    event.setup = 0;
    event.direction = 0;
    event.buffer_ptr = (uchar*)NULL;
    event.len = ZERO_LENGTH;

    /* Notify Device Layer of SLEEP Event */
    /* this callback need only handle and type -
       all other arguments are redundant */
    (void)_usb_device_call_service(USB_SERVICE_SLEEP,&event);
    
    /* Enable RESUME Interrupt */
    usb_ptr->INTEN |= USB_INTEN_RESUMEEN_MASK;
}

/************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_resume_intr
 *
 * @brief : Service resume Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_resume_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr
)
{
    volatile USB_MemMapPtr usb_ptr;
    USB_EVENT_STRUCT event;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }
        
    /* clear resume interrupt status bit */
    usb_ptr->ISTAT = USB_ISTAT_RESUME_MASK;
   
    (void)_usb_device_set_status((_usb_device_handle)state_ptr,
        (uint_8)USB_STATUS_DEVICE_STATE, (uint_16)state_ptr->G.USB_DEV_STATE_B4_SUSPEND);
    
    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.ep_num = (uint_8)USB_UNINITIALIZED_VAL_32;
    event.setup = 0;
    event.direction = 0;
    event.buffer_ptr = (uchar*)NULL;
    event.len = ZERO_LENGTH;
    
    /* Notify Device Layer of RESUME Event */
    /* this callback need only handle and type -
       all other arguments are redundant */
    (void)_usb_device_call_service(USB_SERVICE_RESUME,&event);
    
    /* Disable RESUME Interrupt */
    usb_ptr->INTEN &= ~USB_INTEN_RESUMEEN_MASK;
}

/***********************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_sof_token_intr
 *
 * @brief : Service SOF Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_sof_token_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr
)
{
    uchar stat,ep_num;
    volatile USB_MemMapPtr usb_ptr;
    uint_8_ptr buffer_ptr;
    USB_EVENT_STRUCT event;

    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }
        
    /* Clear SOF Interrupt */
    usb_ptr->ISTAT = USB_ISTAT_SOFTOK_MASK;
    
    state_ptr->G.USB_SOF_COUNT = usb_ptr->FRMNUMH;
    state_ptr->G.USB_SOF_COUNT <<= FRAME_HIGH_BYTE_SHIFT;
    state_ptr->G.USB_SOF_COUNT |= usb_ptr->FRMNUML;
    
    /* address of Lower byte of Frame number.
     */
    buffer_ptr = (uint_8_ptr)&(state_ptr->G.USB_SOF_COUNT);
    
    /* clear resume interrupt status bit */
    usb_ptr->ISTAT = USB_ISTAT_RESUME_MASK;
    
    /* Get the status of previous transaction*/
    stat = usb_ptr->STAT;
    
    /* Get the Endpoint number on which the transaction occured.
     * This is (7 - 5) [3:0] bit in STAT register.
     */
    ep_num = (uint_8)((stat & ENDPOINT_NUMBER_MASK) >> ENDPOINT_NUMBER_SHIFT);
    
    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.ep_num = ep_num;
    event.setup = FALSE;
    event.direction =USB_RECV;
    event.buffer_ptr = (uchar*)buffer_ptr;
    event.len = ZERO_LENGTH;
    
    /* Notify Device Layer of SOF Event */
    (void)_usb_device_call_service(USB_SERVICE_SOF,&event);
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_attach_intr
 *
 * @brief : Service attach Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_attach_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr 
)
{
    volatile USB_MemMapPtr usb_ptr;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }

    usb_ptr->ISTAT = USB_ISTAT_ATTACH_MASK;
}

/************************************************************************//*!
 *
 * @name  _usb_device_usbfs_service_stall_intr
 *
 * @brief : Service stall Interrupt.
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_service_stall_intr
(
     /*[IN]*/
    USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr 
)
{
    volatile USB_MemMapPtr usb_ptr;
    USB_EVENT_STRUCT event;
    
    /*any value except USB_STATUS_STALLED to initialize with */
    uint_16 endp_status = USB_STATUS_IDLE;
    
    /* This test is redundant, the condition is already tested in ISR
    if (state_ptr == NULL)
    {
        return;
    } */
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr)state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }
    
    /* Clear Stall Interrupt */
    usb_ptr->ISTAT = USB_ISTAT_STALL_MASK;
    
    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = (_usb_device_handle)state_ptr;
    event.setup = FALSE;
    event.direction = (uint_8)USB_UNINITIALIZED_VAL_32;
    event.buffer_ptr = (uchar*)NULL;
    event.len = ZERO_LENGTH;

    /* check if the stall interrupt received was for CONTROL ENDPOINT */
    (void)_usb_device_get_status(event.handle,
        (uint_8)(USB_CONTROL_ENDPOINT|USB_STATUS_ENDPOINT), &endp_status);
    if(endp_status == USB_STATUS_STALLED)
    {
        event.ep_num = USB_CONTROL_ENDPOINT;
    }
    else
    {
        event.ep_num = (uint_8)USB_UNINITIALIZED_VAL_32;
    }

    /* Notify Device Layer of STALL Event */
    (void)_usb_device_call_service(USB_SERVICE_STALL,&event);

    /* prepare for next setup token if stall was received for Control Endp*/
    if(event.ep_num == USB_CONTROL_ENDPOINT)
    {
        _usb_next_setup_token_prep(state_ptr);
    }
}

/**************************************************************************//*!
 *
 * @name  _usb_device_usbfs_isr
 *
 * @brief : Service all the interrupts in the kirin usb hardware
 *
 * @param state_ptr: Device Structure Pointer Passed to ISR. The is the
 *                   same stracutre which was passed in function USB_install_isr
 *                   During ISR installation.
 *
 * @return NONE
 *****************************************************************************/
static void _usb_device_usbfs_isr(USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr)
{
    uchar error;
    volatile USB_MemMapPtr usb_ptr;
    
    if (state_ptr == NULL)
    {
        return;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return;
    }

    error = (uint_8)(usb_ptr->ISTAT & usb_ptr->INTEN);
    
    /* This interrupt comes when any of the error conditions within
     * the ERRSTAT register occur. The ColdFire core must
     * then read the ERRSTAT register to determine the source of the error.
     */
    if (error & USB_ISTAT_ERROR_MASK)
    {
         _usb_device_usbfs_service_err_intr(state_ptr);
    }
    /* This reset interrupt comes when the USB Module has decoded a valid USB reset.
     * This informs the Microprocessor that it should write 0x00 into the address
     * register and enable endpoint 0. USB_RST bit is set after a USB reset has been
     * detected for 2.5 microseconds. It is not asserted again until the USB reset
     * condition has been removed and then reasserted.
     */
    if (error & USB_ISTAT_USBRST_MASK)
    {
       _usb_device_usbfs_service_reset_intr(state_ptr);
    }
    /* This TOKEN_DONE interrupt comes when the current token being processed
     * has completed. The ColdFire core should immediately read the STAT register
     * to determine the EndPoint and BD used for this token. Clearing this bit
     * (by writing a one) causes the STAT register to be cleared or the
     * STAT holding register to be loaded into the STAT register.
     */
    if (error & USB_ISTAT_TOKDNE_MASK)
    {
        _usb_device_usbfs_service_tk_dne_intr(state_ptr);
    }
    /* This sleep interrupt comes when the USB Module detects a constant idle
     * on the USB bus for 3 milliseconds.
     */
    if (error & USB_ISTAT_SLEEP_MASK)
    {
        _usb_device_usbfs_service_sleep_intr(state_ptr);
    }
    
    /* This interrupt comes depending upon the DP/DM signals, and can be used
     * to signal remote wake-up signaling on the USB bus. When not in suspend
     * mode this interrupt should be disabled
     */
    if (error & USB_ISTAT_RESUME_MASK)
    {
        _usb_device_usbfs_service_resume_intr(state_ptr);
    }
    /* Thisinterrupt comes when the USB Module receives a Start Of Frame
     * (SOF) token.
     */
    if(error & USB_ISTAT_SOFTOK_MASK)
    {
       _usb_device_usbfs_service_sof_token_intr(state_ptr);
    }
    
    /* This interrupt comes when the USB Module detects an attach of a 
     * USB device. This signal is only valid if HOST_MODE_EN is true. 
     * This interrupt signifies that a peripheral is now present and must
     * be configured.
     */
    if(error & USB_ISTAT_ATTACH_MASK)
    {
       _usb_device_usbfs_service_attach_intr(state_ptr);
    }
    
    /* In Target mode this interrupt comes when a STALL handshake is sent 
       by the SIE.*/
    if (error & USB_ISTAT_STALL_MASK)
    {
         _usb_device_usbfs_service_stall_intr(state_ptr);
    }

    return;
}

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
*
* @name        : _usb_dci_usbfs_preinit
* @brief       : Allocates space for the USB device controller.
* @param handle: Handle to USB Device to be filled
* @return   USB_OK on successful.
******************************************************************************/
USB_STATUS _usb_dci_usbfs_preinit
(
    /* [IN] the USB device handle */
    _usb_device_handle _PTR_ handle_ptr
)
{
    USB_KHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR) USB_mem_alloc_zero(sizeof(USB_KHCI_DEV_STATE_STRUCT));
    KHCI_XD_STRUCT_PTR            xd_ptr;
    SCRATCH_STRUCT_PTR            temp_scratch_ptr;
    _mqx_uint j;

    if (NULL != usb_dev_ptr) {
        /* Allocate MAX_XDS_FOR_TR_CALLS */
        usb_dev_ptr->G.XD_BASE = (XD_STRUCT_PTR) USB_mem_alloc_zero(sizeof(KHCI_XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);

        if (usb_dev_ptr->G.XD_BASE == NULL)
        {
            #if DEV_DEBUG
            printf("2 memalloc failed in _usb_device_init\n");
            #endif

            _mem_free(usb_dev_ptr);
            return USBERR_ALLOC;
        } /* Endif */

        /* Allocate memory for internal scratch structure */
        usb_dev_ptr->G.XD_SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR) USB_mem_alloc_zero(sizeof(SCRATCH_STRUCT) * MAX_XDS_FOR_TR_CALLS);

        if (usb_dev_ptr->G.XD_SCRATCH_STRUCT_BASE == NULL)
        {
            #if DEV_DEBUG
            printf("3 memalloc failed in _usb_device_init\n");
            #endif

            _mem_free(usb_dev_ptr->G.XD_BASE);
            _mem_free(usb_dev_ptr);
            return USBERR_ALLOC;
        } /* Endif */

        usb_dev_ptr->G.TEMP_XD_PTR = (XD_STRUCT_PTR)USB_mem_alloc_zero(sizeof(KHCI_XD_STRUCT));
        if(usb_dev_ptr->G.TEMP_XD_PTR == NULL)
        {
            #ifdef _DEV_DEBUG
                printf("4 memalloc failed in _usb_device_init\n");
            #endif  
            _mem_free(usb_dev_ptr->G.XD_SCRATCH_STRUCT_BASE);
            _mem_free(usb_dev_ptr->G.XD_BASE);
            _mem_free(usb_dev_ptr);
            return USBERR_ALLOC;
        }

        usb_dev_ptr->G.XD_HEAD = NULL;
        usb_dev_ptr->G.XD_TAIL = NULL;
        usb_dev_ptr->G.XD_ENTRIES = 0;

        /* Enqueue all the XDs */   
        xd_ptr = (KHCI_XD_STRUCT_PTR) usb_dev_ptr->G.XD_BASE;
        temp_scratch_ptr = usb_dev_ptr->G.XD_SCRATCH_STRUCT_BASE;

        for (j = 0; j < MAX_XDS_FOR_TR_CALLS; j++) 
        {
           xd_ptr->G.SCRATCH_PTR = temp_scratch_ptr;
           xd_ptr->G.SCRATCH_PTR->FREE = _usb_device_free_XD;
           xd_ptr->G.SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
           _usb_device_free_XD(usb_dev_ptr,(pointer)xd_ptr);
           xd_ptr++;
           temp_scratch_ptr++;
        }

        *handle_ptr = (_usb_device_handle) usb_dev_ptr;
    }
    else { //NULL == usb_dev_ptr
        return USBERR_ALLOC;
    }

    return USB_OK;
}	


/**************************************************************************//*!
*
* @name        : _usb_dci_usbfs_init
* @brief       : Initializes the USB device controller.
* @param handle: USB device handle.
* @param init_param: initialization parameters specific for device
* @return   USB_OK on successful.
******************************************************************************/
USB_STATUS _usb_dci_usbfs_init
(
    /* [IN] the USB device handle */
    _usb_device_handle         handle
)
{
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    USB_KHCI_DEV_INIT_STRUCT_PTR       param = (USB_KHCI_DEV_INIT_STRUCT_PTR) usb_dev_ptr->G.INIT_PARAM;
    volatile USB_MemMapPtr             usb_ptr;

    if (usb_dev_ptr == NULL) {
        return USBERR_INVALID_DEVICE_NUM;
    }
    usb_dev_ptr->G.DEV_PTR = param->BASE_PTR;
    usb_dev_ptr->G.DEV_VEC = param->VECTOR;
    usb_dev_ptr->G.USB_STATE = USB_STATE_UNKNOWN;
   
    /* Get the maximum number of endpoints supported by this USB controller */
    usb_dev_ptr->G.MAX_ENDPOINTS = param->MAX_ENDPOINTS;

    /* Get the USB IP base address in the controller */
    usb_ptr = (USB_MemMapPtr) param->BASE_PTR;
    usb_dev_ptr->G.DEV_PTR = (pointer) usb_ptr;
    usb_dev_ptr->G.DEV_VEC = param->VECTOR;
//    usb_dev_ptr->G.PRIORITY = param->PRIORITY;

    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Install the ISR
     * Device Pointer is passed as an Argument when the
     * ISR is invoked.
     */
    if (!(USB_install_isr(usb_dev_ptr->G.DEV_VEC, (void (*)(void *))_usb_device_usbfs_isr, usb_dev_ptr)))
       return USBERR_INSTALL_ISR;

    /* Memory for BD is created by creating a data section "usb_bdt"
     * of 512 bytes in usbfs_dev_main.h. This memory is cleared here.
     */
    USB_mem_zero(bdt, sizeof(bdt));
    
    /* Initialize the end point state*/
    _usb_device_usbfs_reset_ep_state(usb_dev_ptr);
        
    /* Initialize BDT Page register 1,2,3. The Buffer
     * Descriptor Table Page Register 1 2,3 contains an
     * 8-bit value used to compute the address where
     * the current Buffer Descriptor Table (BDT)
     * resides in system memory.
     */
    usb_ptr->BDTPAGE1 = (uint_8)((uint_32)BDT_BASE >> 8);
    usb_ptr->BDTPAGE2 = (uint_8)((uint_32)BDT_BASE >> 16);
    usb_ptr->BDTPAGE3 = (uint_8)((uint_32)BDT_BASE >> 24);
    
    /* Clear all interrupts and bring it to finite
     * state before device starts functioning.
     */
    usb_ptr->ISTAT = ISTAT_CLEAR_ALL;
    
    /* Enable Sleep,Token Done,Error,USB Reset,Stall,
     * Resume and SOF Interrupt.
     */
    usb_ptr->INTEN = INTEN_BUS_RESET_VAL;
    
    /* Enable USB module*/
    usb_ptr->CTL = USB_CTL_USBENSOFEN_MASK;

    /* Remove suspend state */
    usb_ptr->USBCTRL &= ~USB_USBCTRL_SUSP_MASK;
    
    /* Enable D+ pull up register
     * Note, that this D+ external resistor is not applicable for some devices
    */
    usb_ptr->OTGCTL = USB_OTGCTL_DPHIGH_MASK |  USB_OTGCTL_OTGEN_MASK;

    return USB_OK;
}

/**************************************************************************//*!
* @name        : _usb_dci_usbfs_init_endpoint
*
* @brief       : Initialize the Endpoint
* @param handle: Handle to USB Device.
* @param xd_ptr: Transaction Discriptor.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_init_endpoint
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    XD_STRUCT_PTR xd_ptr
)
{
    USB_STATUS error = USB_OK;
    uint_8 ep_ctrl = 0;
    XD_STRUCT_PTR xd_temp_ptr;
    uint_16 max_pkt_size = 0;

    /* Get the device pointer initialized in _usb_dci_usbfs_init*/
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    volatile USB_MemMapPtr usb_ptr;

    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }

    
    if((xd_ptr->EP_TYPE > USB_INTERRUPT_PIPE) || 
        (xd_ptr->BDIRECTION > USB_SEND))
    {
        return USBERR_EP_INIT_FAILED;
    }
    
    if(state_ptr->EP_INFO[xd_ptr->EP_NUM].ep_init_flag[xd_ptr->BDIRECTION]
        == TRUE) 
    {/* If endpoint is already initialized, then deinitialize it before initializing it again */
        (void)_usb_dci_usbfs_deinit_endpoint(handle, xd_ptr->EP_NUM, 
            xd_ptr->BDIRECTION);
    }

    /* mark this endpoint as initialized */
    state_ptr->EP_INFO[xd_ptr->EP_NUM].ep_init_flag[xd_ptr->BDIRECTION] = TRUE;
        
    /*before initializing cancel all transfers on EP as there may be calls
     for endpoint initialization more than once. This will free any allocated
     queue*/

    /*
     * Since this is endpoint initialization section there will
     * not be any pending transfers on this endpoint
     */
    state_ptr->G.PENDING_XD_PTR = NULL;

    /* alloc XD for RECV ONLY and WSOFAR is zero*/
    if((xd_ptr->WSOFAR == 0)&&(!xd_ptr->BDIRECTION))
    {
        _usb_device_alloc_XD (state_ptr, &xd_temp_ptr);
        xd_temp_ptr->EP_NUM = xd_ptr->EP_NUM;
        xd_temp_ptr->BDIRECTION = xd_ptr->BDIRECTION;
        xd_temp_ptr->WTOTALLENGTH =xd_ptr->WTOTALLENGTH;
        xd_temp_ptr->WSOFAR = xd_ptr->WSOFAR;
        xd_temp_ptr->EP_TYPE = xd_ptr->EP_TYPE;
        xd_temp_ptr->DONT_ZERO_TERMINATE = xd_ptr->DONT_ZERO_TERMINATE;
    }

    /* Enable hand shaking at this endpoint if this is not an Isochronous
     * transfer
     */
    ep_ctrl |= (xd_ptr->EP_TYPE != USB_ISOCHRONOUS_PIPE ? USB_ENDPT_EPHSHK_MASK : 0);
        
    /* The end point buffer size will be set as required by app.
     * but if the size crosses MAX_EP_BUFFER_SIZE size then truncate the
     * max packet size.
     */
    max_pkt_size = (uint_16)((xd_ptr->WMAXPACKETSIZE > MAX_EP_BUFFER_SIZE)?
        MAX_EP_BUFFER_SIZE:xd_ptr->WMAXPACKETSIZE);
    
    /* Initialize the End Point Information Structure which is part of device
     * structure. It is done so that it can be used at later point of time
     * like ISR handler, any other function call.
     */
    state_ptr->EP_INFO[xd_ptr->EP_NUM].type = xd_ptr->EP_TYPE;
    state_ptr->EP_INFO[xd_ptr->EP_NUM].direction = xd_ptr->BDIRECTION;
    state_ptr->EP_INFO[xd_ptr->EP_NUM].max_packet_size = max_pkt_size;
    state_ptr->EP_INFO[xd_ptr->EP_NUM].endpoint_status = USB_STATUS_DISABLED;
    state_ptr->EP_INFO[xd_ptr->EP_NUM].stall_flag = FALSE;
    
    /* Write the initialized control values to end point control register*/
    usb_ptr->ENDPOINT[xd_ptr->EP_NUM].ENDPT |=  ep_ctrl;
    
    /*Configure this endpoint for receive or Send direction as reuired by APP*/
    usb_ptr->ENDPOINT[xd_ptr->EP_NUM].ENDPT |= (xd_ptr->BDIRECTION ? USB_ENDPT_EPTXEN_MASK: USB_ENDPT_EPRXEN_MASK);
    
    /* Set the BDT and buffer data pointer for Receive Direction*/
    if(USB_RECV == xd_ptr->BDIRECTION)
    {
        const uint_8 control[2] = { 
            USB_BD_DATA01(0) | USB_BD_DTS | USB_BD_OWN,
            USB_BD_DATA01(1) | USB_BD_DTS
        };

        /* Initialize the DATA PID to DATA0*/
        state_ptr->EP_INFO[xd_ptr->EP_NUM].rx_data0 = 0;
                            
        /* Allocate memory for both endpoint even and odd buffers*/
        state_ptr->EP_INFO[xd_ptr->EP_NUM].data_buffer =
            (uint_8_ptr)USB_mem_alloc_uncached_zero(2 * sizeof(uint_8) * max_pkt_size);
        
        if(state_ptr->EP_INFO[xd_ptr->EP_NUM].data_buffer == NULL)
        {
            #ifdef _DEV_DEBUG
                printf("malloc failed in _usb_dci_usbfs_init_endpoint for even and odd buffers\n");
            #endif  
            return USBERR_ALLOC;
        }
        
        /*Configure Even buff for the end point*/
        BD_ADDR_RX(xd_ptr->EP_NUM, 
            state_ptr->EP_INFO[xd_ptr->EP_NUM].rx_buf_odd) =
                LONG_LE_TO_HOST((uint_32)
                    state_ptr->EP_INFO[xd_ptr->EP_NUM].data_buffer);
        
        /* configure first start address for recv transaction */
        if((xd_ptr->WSOFAR == 0)&&(!xd_ptr->BDIRECTION))
        {
            xd_temp_ptr->WSTARTADDRESS = (uint_8_ptr) LONG_LE_TO_HOST(BD_ADDR_RX(xd_ptr->EP_NUM,
                state_ptr->EP_INFO[xd_ptr->EP_NUM].rx_buf_odd));
                /* Queue the XD taken from free list to End Point Info struncture*/
            USB_XD_QUEUE_ENQUEUE(&state_ptr->EP_INFO[xd_ptr->EP_NUM].xd_queue_recv, xd_temp_ptr);
        }

        BD_CTRL_RX(xd_ptr->EP_NUM, 
            state_ptr->EP_INFO[xd_ptr->EP_NUM].rx_buf_odd) =
                LONG_LE_TO_HOST((uint_32)(USB_BD_BC(max_pkt_size) | control[0]));
        
        /*Configure Even buff for odd end point*/
        BD_ADDR_RX(xd_ptr->EP_NUM, 
            state_ptr->EP_INFO[xd_ptr->EP_NUM].rx_buf_odd ^ 1) =
                LONG_LE_TO_HOST((uint_32)
                (state_ptr->EP_INFO[xd_ptr->EP_NUM].data_buffer + max_pkt_size));
        
        BD_CTRL_RX(xd_ptr->EP_NUM, 
            state_ptr->EP_INFO[xd_ptr->EP_NUM].rx_buf_odd ^ 1) =
                LONG_LE_TO_HOST((uint_32)(USB_BD_BC(max_pkt_size) | control[1]));
    }
    else /* Set the BDT and buffer data pointer for Send direction */
    {
        /* Initialize the DATA PID to DATA0*/
        state_ptr->EP_INFO[xd_ptr->EP_NUM].tx_data0 = 0;
        
        BD_CTRL_TX(xd_ptr->EP_NUM, 
            state_ptr->EP_INFO[xd_ptr->EP_NUM].tx_buf_odd) =
                LONG_LE_TO_HOST((uint_32)(USB_BD_BC(max_pkt_size) |
                    USB_BD_DATA01(0) | USB_BD_DTS));

        BD_CTRL_TX(xd_ptr->EP_NUM, 
            state_ptr->EP_INFO[xd_ptr->EP_NUM].tx_buf_odd ^ 1) =
                LONG_LE_TO_HOST((uint_32)(USB_BD_BC(max_pkt_size) | 
                    USB_BD_DATA01(1) | USB_BD_DTS));
     }
    return error;
}

/**************************************************************************//*!
* @name  : _usb_dci_usbfs_deinit_endpoint
*
* @brief       : De-Initialize the Endpoint
* @param handle: Handle to USB Device.
* @param ep_num: End Point Number.
* @param direction: USB_SEND or USB_RECV.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_deinit_endpoint
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    uint_8 ep_num,
    /*[IN]*/
    uint_8 direction 
)
{
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    volatile USB_MemMapPtr usb_ptr;
    
    if (state_ptr == NULL)
    {
        return USBERR_EP_DEINIT_FAILED;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_EP_DEINIT_FAILED;
    }

    /*before de-initializing cancel all transfers on EP */
    (void)_usb_device_cancel_transfer(handle, ep_num, direction);
    
    /*Disable the trasmit or receive endpoint*/
    usb_ptr->ENDPOINT[ep_num].ENDPT = EP_DISABLE;
    
    /* Free the Memory Allocated for Data buffers in endpoint number ep_num*/
    if ((direction == USB_RECV) && state_ptr->EP_INFO[ep_num].data_buffer)
    {
        USB_mem_free(state_ptr->EP_INFO[ep_num].data_buffer);
        state_ptr->EP_INFO[ep_num].data_buffer = NULL;
    }
    
    /* uninitialise the strucrure for this endpoint */
    BD_ADDR(ep_num,direction,EVEN_BUFF) =
        LONG_LE_TO_HOST_CONST((uint_32)USB_UNINITIALIZED_VAL_32);
    BD_ADDR(ep_num,direction,ODD_BUFF) =
        LONG_LE_TO_HOST_CONST((uint_32)USB_UNINITIALIZED_VAL_32);
    
    state_ptr->EP_INFO[ep_num].max_packet_size =    
        (uint_16)USB_UNINITIALIZED_VAL_32;

    /* mark this endpoint as de-initialized */
    state_ptr->EP_INFO[ep_num].ep_init_flag[direction] = FALSE;
        
    return USB_OK;
}

/**************************************************************************//*!
* @name  _usb_dci_usbfs_send
* @brief       : Sends data. Non-blocking.
* @param handle: Handle to USB Device.
* @param xd_ptr: Transaction Discriptor.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_send
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    XD_STRUCT_PTR xd_ptr 
)
{
    USB_STATUS error = USBERR_TX_FAILED;/* initializing to failed value */
    uint_32 buf_num_bytes = (uint_32)(xd_ptr->WTOTALLENGTH - xd_ptr->WSOFAR);
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    
    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Allocate XD stucutre from Free List maintained by Device strucutre,
     * if it is not any split transaction.
     */
    if (xd_ptr->WSOFAR == 0)
    {
        USB_XD_QUEUE_ENQUEUE(&state_ptr->EP_INFO[xd_ptr->EP_NUM].xd_queue_send, xd_ptr);
    }
    
    /* Upper layer wants to send Zero Packet data
     * Note that this situation will only come when
     * Upper layer wants to send Zero packet data not
     * in case of split transaction
     */
    if (buf_num_bytes == 0)
    {
        error = _usb_device_usbfs_ep_write(state_ptr, xd_ptr->EP_NUM,
            xd_ptr->WSTARTADDRESS, buf_num_bytes, (uint_32*)&buf_num_bytes);
        
        if (error != USB_OK)
        {
            /* If write returns error release the XD pointer
             * from Endpoint send queue and return it to Free list of XD pointers.
             */
            xd_ptr->BSTATUS = USB_STATUS_IDLE;
            USB_XD_QUEUE_DEQUEUE (&state_ptr->EP_INFO[xd_ptr->EP_NUM].xd_queue_send, &xd_ptr);
            if (xd_ptr != NULL)
            _usb_device_free_XD(state_ptr, xd_ptr);
            //_usb_device_free_XD(xd_ptr);
        }
        return error;
    }
    
    /* If not Zero Size packet send the data from here*/
    error = _usb_device_usbfs_ep_write(state_ptr, xd_ptr->EP_NUM,
        (uint_8_ptr)xd_ptr->WSTARTADDRESS + xd_ptr->WSOFAR, buf_num_bytes,
        (uint_32*)&buf_num_bytes);
    
    if (error != USB_OK)
    {
        /* If write returns error release the XD pointer
         * from Endpoint send queue and return it to Free list of XD pointers.
         */
        xd_ptr->BSTATUS = USB_STATUS_IDLE;
        USB_XD_QUEUE_DEQUEUE (&state_ptr->EP_INFO[xd_ptr->EP_NUM].xd_queue_send, &xd_ptr);
        if (xd_ptr != NULL)
            _usb_device_free_XD(state_ptr, xd_ptr);
    }   
    return error;
}

/**************************************************************************//*!
* @name        : _usb_dci_usbfs_recv
* @brief       : Receives data. Non-blocking.
* @param handle: Handle to USB Device.
* @param xd_ptr: Transaction Discriptor.
* @return      : USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_recv
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    XD_STRUCT_PTR xd_ptr 
)
{
    USB_STATUS error = USBERR_RX_FAILED;/* initializing to failed value */
    uint_32 buf_num_bytes;
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    
    if ((state_ptr == NULL) || (xd_ptr == NULL))
    {
        return USBERR_ERROR;
    }
    
    buf_num_bytes = xd_ptr->WTOTALLENGTH - xd_ptr->WSOFAR;
    
    /* Allocate XD stucutre from Free List maintained by Device strucutre,
     * if it is not any split transaction.
     */
    if (xd_ptr->WSOFAR == 0)
    {
        USB_XD_QUEUE_ENQUEUE(&state_ptr->EP_INFO[xd_ptr->EP_NUM].xd_queue_recv, xd_ptr);
    }

    /* If the number of bytes to be received is greater than the
     * maximum data that can be received on the USB bus, then split the
     * transaction into multiple receive transaction.
     */
    if (buf_num_bytes > state_ptr->EP_INFO[xd_ptr->EP_NUM].max_packet_size)
    {
        buf_num_bytes = state_ptr->EP_INFO[xd_ptr->EP_NUM].max_packet_size;
    }
    
    /* Send the receiv command to the device.*/
    error = _usb_device_usbfs_ep_read(handle, xd_ptr->EP_NUM,
        (uint_8_ptr)((uint_32)xd_ptr->WSTARTADDRESS + (uint_32)xd_ptr->WSOFAR),
        buf_num_bytes);
    
    if (error != USB_OK)
    {
        USB_XD_QUEUE_DEQUEUE(&state_ptr->EP_INFO[xd_ptr->EP_NUM].xd_queue_recv, &xd_ptr);
    }
    
    return error;
}

/**************************************************************************//*!*
* @name  : _usb_dci_usbfs_stall_endpoint
* @brief      : Stalls the specified endpoint
* @param handle: Handle to USB Device.
* @param ep_num: End Point Number.
* @param direction: USB_SEND or USB_RECV.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_stall_endpoint
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    uint_8 ep_num, 
    /*[IN]*/
    uint_8 direction
)
{
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    USB_MemMapPtr usb_ptr;

    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr = (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* set  the stall flag in device structure to be true */
    state_ptr->EP_INFO[ep_num].stall_flag = TRUE;
    
    /* retiring pending IRPs on stall detection */
    (void)_usb_device_cancel_transfer(handle,ep_num,direction);
    
    /* If Stall is for Send packet update Send BDT */
    if (direction)
    {/* USB_SEND */
        BD_CTRL_TX(ep_num, state_ptr->EP_INFO[ep_num].tx_buf_odd) =
            LONG_LE_TO_HOST((uint_32)(USB_BD_BC(state_ptr->EP_INFO[ep_num].
                max_packet_size)| USB_BD_OWN | USB_BD_STALL | USB_BD_DTS));
    }
    else
    {/* USB_RECV */
        /* If Stall is for Receive transaction, Update Recevice BDT*/
        BD_CTRL_RX(ep_num, state_ptr->EP_INFO[ep_num].rx_buf_odd) =
            LONG_LE_TO_HOST((uint_32)(USB_BD_BC(state_ptr->EP_INFO[ep_num].
                max_packet_size) | USB_BD_OWN | USB_BD_STALL | USB_BD_DTS));
    }
    
    /* Continue Further processing as the IP stops on receiving
     * Setup Token.
     */
    usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    
    return USB_OK;
}

/**************************************************************************//*!*
* @name  : _usb_dci_usbfs_unstall_endpoint
* @brief       : Unstall the Endpoint in specific direction
* @param handle: Handle to USB Device.
* @param ep_num: End Point Number.
* @param direction: USB_SEND or USB_RECV.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_unstall_endpoint
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    uint_8 ep_num, 
    /*[IN]*/
    uint_8 direction 
)
{
    XD_STRUCT_PTR xd_ptr_temp;
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;
    USB_MemMapPtr usb_ptr;
    
    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr = (USB_MemMapPtr) state_ptr->G.DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* clear  the stall flag in device structure */
    state_ptr->EP_INFO[ep_num].stall_flag = FALSE;
    
    usb_ptr->ENDPOINT[ep_num].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
    if (direction)
    {/* USB_SEND */      
        usb_ptr->ENDPOINT[ep_num].ENDPT |= 
            (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPTXEN_MASK);
        BD_CTRL_TX(ep_num, state_ptr->EP_INFO[ep_num].tx_buf_odd) =
            LONG_LE_TO_HOST((uint_32)(USB_BD_BC(state_ptr->EP_INFO[ep_num].max_packet_size) |
            USB_BD_DTS | USB_BD_DATA01(0)));
    }
    else
    {/* USB_RECV */
        usb_ptr->ENDPOINT[ep_num].ENDPT |= 
            (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPRXEN_MASK);
        /* prepare XD queue for RECV CONTROL ENDPOINT*/
        _usb_device_alloc_XD (state_ptr, &xd_ptr_temp);
        xd_ptr_temp->EP_NUM = ep_num;
        xd_ptr_temp->BDIRECTION = USB_RECV;
        xd_ptr_temp->WTOTALLENGTH = ZERO_LENGTH;
        xd_ptr_temp->EP_TYPE = state_ptr->EP_INFO[ep_num].type;
        xd_ptr_temp->DONT_ZERO_TERMINATE = TRUE;
        
        /* Enqueue the Prepared XD to the receive queue*/
        USB_XD_QUEUE_ENQUEUE(&state_ptr->EP_INFO[ep_num].xd_queue_recv, xd_ptr_temp);
        
        BD_CTRL_RX(ep_num, state_ptr->EP_INFO[ep_num].rx_buf_odd) =
            LONG_LE_TO_HOST((uint_32)(USB_BD_BC(state_ptr->EP_INFO[ep_num].max_packet_size) |
                USB_BD_OWN | USB_BD_DTS | USB_BD_DATA01(0)));
    }

    usb_ptr->CTL &= (~ USB_CTL_TXSUSPENDTOKENBUSY_MASK);
    return USB_OK;
}

/**************************************************************************//*!*
* @name  : _usb_dci_usbfs_cancel
* @brief : Cancels all pending transfers on an endpoint.
* @param handle: Handle to USB Device.
* @param ep_num: End Point Number.
* @param direction: USB_SEND or USB_RECV.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_cancel
(
     /*[IN]*/
    _usb_device_handle handle,
    /*[IN]*/
    uint_8 ep_num, 
    /*[IN]*/
    uint_8 direction 
)
{
    volatile XD_STRUCT_PTR xd_temp_ptr;
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;

    if (state_ptr == NULL)
    {
       return USBERR_ERROR;
    }

    /* Empty the queue and add the XD release stucture to the Free list*/
    do
    {
        if(direction)
        {
            USB_XD_QUEUE_DEQUEUE(&state_ptr->EP_INFO[ep_num].xd_queue_send, &xd_temp_ptr);
            
            if(xd_temp_ptr)
            {
                xd_temp_ptr->BSTATUS = USB_STATUS_IDLE;
                if (xd_temp_ptr != NULL)
                _usb_device_free_XD(state_ptr, xd_temp_ptr);
                //_usb_device_free_XD(xd_temp_ptr);
            }
        }
        else
        {
            USB_XD_QUEUE_DEQUEUE(&state_ptr->EP_INFO[ep_num].xd_queue_recv, &xd_temp_ptr);
            
            if(xd_temp_ptr)
            {
                xd_temp_ptr->BSTATUS = USB_STATUS_IDLE;
                if (xd_temp_ptr != NULL)
                _usb_device_free_XD(state_ptr, xd_temp_ptr);
                //_usb_device_free_XD(xd_temp_ptr);
            }
        }
    }
    while (xd_temp_ptr);

    return USB_OK;
}

/**************************************************************************//*!*
* @name  : _usb_dci_usbfs_set_addr
* @brief       : Set device address.
* @param handle : Device handle.
* @param addr :   Address to be set into Device Address register.
* @return     :   Returns USB_OK or error code.
******************************************************************************/
USB_STATUS _usb_dci_usbfs_set_addr
(
     /*[IN]*/
    _usb_device_handle handle, 
    /*[IN]*/
    uint_8 addr
)
{
    volatile USB_DEV_STATE_STRUCT_PTR state_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    volatile USB_MemMapPtr usb_ptr;

    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }

    if (addr)
    {
        /* Update USB address in Device registers. This 7-bit value
         * defines the USB address that the USB Module decodes
         * in device mode
         */
        usb_ptr->ADDR = addr;
        /* Update the USB device address in Device Info Strucutre for
         * Future Referances.
         */
        state_ptr->DEVICE_ADDRESS = addr;
        /* Set the Device Start as Address Asigned State.*/
        ((USB_DEV_STATE_STRUCT_PTR)handle)->USB_STATE = USB_STATE_ADDRESS;
    }
    return USB_OK;
}

/**************************************************************************//*!*
* @name : _usb_dci_usbfs_shutdown
* @brief      : Shuts down the usbfs Device Controller
*                   Note: There is no function in MQX to uninstall ISR.
*                         Hence ISR is not Uninstalled here.However, installing
*                         ISR twice will not have any negative impact.
* @param handle : Device handle.
* @return     :   Returns USB_OK or error code.
******************************************************************************/
USB_STATUS _usb_dci_usbfs_shutdown
(
     /*[IN]*/
    _usb_device_handle handle 
)
{
    volatile USB_DEV_STATE_STRUCT_PTR state_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    volatile USB_MemMapPtr usb_ptr;

    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }

    /* Reset the Control Register */
    usb_ptr->CTL = 0; /* disables the USB MODULE */

    (void)_usb_device_set_status(handle, (uint_8)USB_STATUS_DEVICE_STATE,
        (uint_16)USB_STATE_UNKNOWN);

    return USB_OK;
}

/**************************************************************************//*!*
* @name : _usb_dci_usbfs_get_setup_data
* @brief       : Reads the Setup data from the 8-byte setup buffer
* @param handle: Handle to USB Device.
* @param ep_num: End Point Number.
* @param buffer_ptr: Buffer Pointer holding output Data..
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_get_setup_data
(
     /*[IN]*/
    _usb_device_handle handle, 
     /*[IN]*/
    uint_8 ep_num,
    /*[OUT]*/
    uchar_ptr buffer_ptr
)
{
    uint_8 loop_index=0;
    volatile uint_8_ptr addr;
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;

    if (state_ptr == NULL)
    {
       return USBERR_ERROR;
    }
    
    if (buffer_ptr == NULL)
    {
       return USBERR_ERROR;
    }
    
    /* address correponding to the endpoint tbuffers */
    addr = (uint_8_ptr) LONG_LE_TO_HOST(
        BD_ADDR_RX(ep_num, state_ptr->EP_INFO[ep_num].rx_buf_odd));
    
    /* copy bdt buffer to application buffer */
    for(loop_index = 0; loop_index < USB_SETUP_PKT_SIZE; loop_index++)
    {
        buffer_ptr[loop_index]=addr[loop_index];
    }
    
    *((uint_16*)&buffer_ptr[2]) = SHORT_LE_TO_HOST(*((uint_16*)&buffer_ptr[2]));
    *((uint_16*)&buffer_ptr[4]) = SHORT_LE_TO_HOST(*((uint_16*)&buffer_ptr[4]));
    *((uint_16*)&buffer_ptr[6]) = SHORT_LE_TO_HOST(*((uint_16*)&buffer_ptr[6]));

    return USB_OK;
}

/**************************************************************************//*!*
* @name  : _usb_dci_usbfs_assert_resume
* @brief : Resume signalling for remote wakeup
* @param handle: Handle to USB Device.
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_assert_resume
(
     /*[IN]*/
    _usb_device_handle handle 
)
{
    uint_16 delay_count;
    volatile USB_MemMapPtr usb_ptr;
    volatile USB_DEV_STATE_STRUCT_PTR state_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    
    if (state_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    
    /* clear resume FLAG*/
    usb_ptr->ISTAT = USB_ISTAT_RESUME_MASK;

    /* Disable RESUME Interrupt */
//    usb_ptr->INTEN &= ~USB_INTEN_RESUME_EN_MASK; //already done in RESUME interrupt

    /* continue processing */
    usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;

    /* Start RESUME signaling and make SUSPEND bit 0*/
    usb_ptr->CTL |= USB_CTL_RESUME_MASK;

    /* Set RESUME line for 1-15 ms*/
    delay_count = ASSERT_RESUME_DELAY_COUNT;
    do
    {
       delay_count--;
    } while(delay_count);

    /* Stop RESUME signalling */
    usb_ptr->CTL &= ~USB_CTL_RESUME_MASK;

    return USB_OK;
}

/**************************************************************************//*!*
* @name : _usb_dci_usbfs_get_endpoint_status
* @brief : Get Endpoint Transfer Status
* @param handle: Handle to USB Device.
* @param component: End Point Number.
* @param endp_status: Variable containig endpint status..
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_get_endpoint_status
(
     /*[IN]*/
    _usb_device_handle handle,
     /*[IN]*/
    uint_8 component,
    /*[OUT]*/
    uint_16_ptr endp_status
)
{
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR)handle;

    if (state_ptr == NULL)
    {
        return USBERR_ERROR;    
    }       

    *endp_status = state_ptr->EP_INFO[component].endpoint_status;
    return USB_OK;
}

/**************************************************************************//*!*
* @name  : _usb_dci_usbfs_set_endpoint_status
* @brief   : Set Endpoint Transfer Status
* @param handle: Handle to USB Device.
* @param component: End Point Number.
* @param setting: Variable containing new settings..
* @return      :  USB_OK or error code
******************************************************************************/
USB_STATUS _usb_dci_usbfs_set_endpoint_status
(
     /*[IN]*/
    _usb_device_handle handle,
     /*[IN]*/
    uint_8 component,
     /*[IN]*/
    uint_16 setting
)
{
    volatile USB_KHCI_DEV_STATE_STRUCT_PTR state_ptr = (USB_KHCI_DEV_STATE_STRUCT_PTR) handle;
    USB_STATUS error = USBERR_ERROR;/* initializing */
    uint_8 ep_num = (uint_8)(component & USB_STATUS_ENDPOINT_NUMBER_MASK);
    /* direction is in most significant bit */
    uint_8 direction = (uint_8)((component>>COMPONENT_PREPARE_SHIFT) & 0x01);

    if (state_ptr == NULL)
    {
        return error;
    }

    if (ep_num > USB_MAX_ENDPOINTS) {
        return error;
    }
    
    state_ptr->EP_INFO[ep_num].endpoint_status = setting;
    
    /* check if the setting was to halt endpoint or unhalt it*/
    if(setting == USB_STATUS_STALLED)
    {   /* stall the endpoint */
        error = _usb_device_stall_endpoint(handle,ep_num,direction);
    }
    else if((setting == USB_STATUS_IDLE) && (state_ptr->EP_INFO[ep_num].stall_flag == TRUE))
    {   /* unstall the endpoint*/
        /* need to update the direction here for unstalling
        (as it was not passed from above layers for unstall because the
         STAT register doesn't get updated in STALL ISR and we need to unstall
         directly from STALL_SERVICE for CONTROL ENDPOINT. Therefore, this
         method of unstall was employed to make the structure generic)*/
        direction = state_ptr->EP_INFO[ep_num].direction;
        error = _usb_device_unstall_endpoint(handle,ep_num,direction);
    }

    return error;
}

/**************************************************************************//*!
 *
 * @name  _usb_dci_usbfs_get_transfer_status
 *
 * @brief The function retrieves the Transfer status of an endpoint
 *
 * @param handle
 * @param ep_num       : endpoint number
 * @param direction    : direction of endpoint
 *
 * @return error
 *         USB_STATUS_IDLE                 : No transfer on endpoint
 *         USB_STATUS_DISABLED             : endpoint is disabled
 *         USB_STATUS_STALLED              : endpoint is stalled
 *         USB_STATUS_TRANSFER_IN_PROGRESS : When SIE has control of BDT
 ******************************************************************************
 *
 * This function retrieves the transfer status of the endpoint by checking the
 * BDT as well as the endpoint control register
 *
 *****************************************************************************/
USB_STATUS _usb_dci_usbfs_get_transfer_status
(
     /*[IN]*/
    _usb_device_handle handle,
     /*[IN]*/
    uint_8 ep_num,
     /*[IN]*/
    uint_8 direction
)
{
    USB_STATUS error = USB_STATUS_DISABLED;
    volatile USB_MemMapPtr usb_ptr;
    volatile USB_DEV_STATE_STRUCT_PTR state_ptr = (USB_DEV_STATE_STRUCT_PTR) handle;

    if (state_ptr == NULL)
    {
        return USBERR_ERROR;    
    }       

    /* Get the USB IP base address in the controller */
    usb_ptr =  (USB_MemMapPtr) state_ptr->DEV_PTR;
    if (usb_ptr == NULL)
    {
        return USBERR_ERROR;
    }

    if(ep_num < USB_MAX_ENDPOINTS)
    {
        uint_8 ep_control = usb_ptr->ENDPOINT[ep_num].ENDPT;

        error = USB_STATUS_IDLE;
        
        /* Check for direction in endpoint control register */
        if((ep_control & (USB_ENDPT_EPTXEN_MASK |
            USB_ENDPT_EPRXEN_MASK)) == EP_DISABLE)
        {
            error = USB_STATUS_DISABLED;
        }
        /* Check for stall bit in endpoint control register */
        else if ((ep_control &  USB_ENDPT_EPSTALL_MASK) ==
            USB_ENDPT_EPSTALL_MASK)
        {
            error = USB_STATUS_STALLED ;
        }
        /* Check whether SIE has control of BDT */
        else if((LONG_LE_TO_HOST(BD_CTRL(ep_num,direction,EVEN_BUFF)) & USB_BD_OWN) ||
            (LONG_LE_TO_HOST(BD_CTRL(ep_num,direction,ODD_BUFF)) & USB_BD_OWN))
        {
            error = USB_STATUS_TRANSFER_IN_PROGRESS;
        }
    }
    return error;
}

/* EOF */
