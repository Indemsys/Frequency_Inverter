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
* $FileName: ehci_dev_main.c$
* $Version : 3.8.14.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*   This file contains the main VUSBHS USB Device Controller interface
*   functions.
*
*END*********************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "dev_cnfg.h"
#include "devapi.h"
#include "dev_main.h"

#include "ehci_dev_main.h"
#include "ehci_usbprv.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
#define USB_TEST_MODE_TEST_PACKET_LENGTH   (53)
#define MCF_USB_USBMODE             (*(volatile uint_32*)(0xFC0B01A8))
#define MCF_USB_USBCMD              (*(volatile uint_32*)(0xFC0B0140))
#define MCF_USB_USBMODE_SLOM        (0x00000008)
#define MCF_USB_USBCMD_ITC(x)       (((x)&0x000000FF)<<16)
#define MCF_USB_USBSTS              (*(volatile uint_32*)(0xFC0B0144))
#define MCF_USB_USBSTS_URI          (0x00000040)
/****************************************************************************
 * Global Variables
 ****************************************************************************/
static uint_8_ptr g_ep_recv_buff_ptr[USB_MAX_ENDPOINTS];

/****************************************************************************
 * Global Functions
 ****************************************************************************/

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
/* Test packet for Test Mode :
   TEST_PACKET. USB 2.0 Specification section 7.1.20 */
uint_8 test_packet[USB_TEST_MODE_TEST_PACKET_LENGTH] =
{
    /* Synch */
    /* DATA 0 PID */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
    0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
    0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
    0xEF, 0xF7, 0xFB, 0xFD, 0x7E
};

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/********************************************************************/
/**************************************************************************//*!
*
* @name        : _usb_dci_usbhs_preinit
* @brief       : Allocates space for the USB device controller.
* @param handle: Handle to USB Device to be filled
* @return   USB_OK on successful.
******************************************************************************/
USB_STATUS _usb_dci_usbhs_preinit
(
    /* [IN] the USB device handle */
    _usb_device_handle _PTR_ handle_ptr
)
{
    USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR) USB_mem_alloc_zero(sizeof(USB_EHCI_DEV_STATE_STRUCT));
    EHCI_XD_STRUCT_PTR            xd_ptr;
    SCRATCH_STRUCT_PTR            temp_scratch_ptr;
    _mqx_uint j;

    if (NULL != usb_dev_ptr) {
        /* Allocate MAX_XDS_FOR_TR_CALLS */
        usb_dev_ptr->G.XD_BASE = (XD_STRUCT_PTR) USB_mem_alloc_zero(sizeof(EHCI_XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);

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

        usb_dev_ptr->G.TEMP_XD_PTR = (XD_STRUCT_PTR) USB_mem_alloc_zero(sizeof(EHCI_XD_STRUCT));
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

    } /* Endif */

    usb_dev_ptr->G.XD_HEAD = NULL;
    usb_dev_ptr->G.XD_TAIL = NULL;
    usb_dev_ptr->G.XD_ENTRIES = 0;

    /* Enqueue all the XDs */   
    xd_ptr = (EHCI_XD_STRUCT_PTR) usb_dev_ptr->G.XD_BASE;
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
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_init
(
    /* [IN] the USB device handle */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    volatile USBHS_REG_STRUCT_PTR      dev_ptr;
    volatile USBHS_REG_STRUCT_PTR      cap_dev_ptr;
    USB_EHCI_DEV_INIT_STRUCT_PTR       param = (USB_EHCI_DEV_INIT_STRUCT_PTR) usb_dev_ptr->G.INIT_PARAM;
    _mqx_uint                          temp;
    _mem_size                          total_memory=0;
    uint_8_ptr                         driver_memory;
    _mqx_uint                          vector;

    if (usb_dev_ptr == NULL) {
        return USBERR_INVALID_DEVICE_NUM;
    }
    usb_dev_ptr->G.DEV_PTR = param->BASE_PTR;
    usb_dev_ptr->G.DEV_VEC = param->VECTOR;
    usb_dev_ptr->G.USB_STATE = USB_STATE_UNKNOWN;
   
    usb_dev_ptr->CAP_BASE_PTR = param->CAP_BASE_PTR;
    cap_dev_ptr = (USBHS_REG_STRUCT_PTR) usb_dev_ptr->CAP_BASE_PTR;

    /* Get the maximum number of endpoints supported by this USB controller */
    usb_dev_ptr->G.MAX_ENDPOINTS =
        (uint_8)((EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.DCC_PARAMS) &
         USBHS_MAX_ENDPTS_SUPPORTED) & 0x000000FF);

    temp = (uint_8)(usb_dev_ptr->G.MAX_ENDPOINTS * 2);

    /****************************************************************
     Consolidated memory allocation
     ****************************************************************/
    total_memory = ((temp * sizeof(USBHS_EP_QUEUE_HEAD_STRUCT)) + 2048) +
                   ((MAX_EP_TR_DESCRS * sizeof(USBHS_EP_TR_STRUCT)) + 32) +
                   (sizeof(SCRATCH_STRUCT)*MAX_EP_TR_DESCRS);

    driver_memory = (uint_8_ptr) USB_mem_alloc_uncached(total_memory);


    if (driver_memory == NULL)
    {
        return USBERR_ALLOC;
    }

    /****************************************************************
     Zero out the memory allocated
    ****************************************************************/
    USB_mem_zero((void *)driver_memory,total_memory);

    #if PSP_HAS_DATA_CACHE
    /****************************************************************
     Flush the zeroed memory if cache is enabled
     ****************************************************************/
        USB_dcache_flush_mlines((pointer)driver_memory,total_memory);
    #endif

    /****************************************************************
     Keep a pointer to driver memory alloctaion
     ****************************************************************/
    usb_dev_ptr->DRIVER_MEMORY = driver_memory;
    usb_dev_ptr->TOTAL_MEMORY = total_memory;
    /****************************************************************
     Assign QH base
     ****************************************************************/
    usb_dev_ptr->EP_QUEUE_HEAD_BASE = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR) driver_memory;
    driver_memory += ((temp * sizeof(USBHS_EP_QUEUE_HEAD_STRUCT)) + 2048);

    /* Align the endpoint queue head to 2K boundary */
    usb_dev_ptr->EP_QUEUE_HEAD_PTR = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        USB_MEM2048_ALIGN((uint_32)usb_dev_ptr->EP_QUEUE_HEAD_BASE);

    /****************************************************************
     Assign DTD base
     ****************************************************************/
    usb_dev_ptr->DTD_BASE_PTR = (USBHS_EP_TR_STRUCT_PTR) driver_memory;
    driver_memory += ((MAX_EP_TR_DESCRS * sizeof(USBHS_EP_TR_STRUCT)) + 32);

    /* Align the dTD base to 32 byte boundary */
    usb_dev_ptr->DTD_ALIGNED_BASE_PTR = (USBHS_EP_TR_STRUCT_PTR)
        USB_MEM32_ALIGN((uint_32)usb_dev_ptr->DTD_BASE_PTR);

    /****************************************************************
     Assign SCRATCH Structure base
     ****************************************************************/
    /* Allocate memory for internal scratch structure */
    usb_dev_ptr->G.SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR) driver_memory;

    /* Install the interrupt service routine */
    if (!(USB_install_isr(usb_dev_ptr->G.DEV_VEC,_usb_dci_usbhs_isr,(pointer)usb_dev_ptr)))
    {
        #ifdef _DEV_DEBUG
            printf("USB_Install_isr failed.\n");
        #endif
        return USBERR_INSTALL_ISR;
    }

    usb_dev_ptr->G.USB_STATE = USB_STATE_UNKNOWN;

    /* Initialize the VUSB_HS controller */
    _usb_dci_usbhs_chip_initialize((_usb_device_handle) usb_dev_ptr);

    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_chip_initialize
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_chip_initialize
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR    usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR             dev_ptr;
    volatile USBHS_REG_STRUCT_PTR             cap_dev_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT_PTR   ep_queue_head_ptr;
    volatile USBHS_EP_TR_STRUCT_PTR           dTD_ptr;
    uint_8                                    temp,i;
    SCRATCH_STRUCT_PTR                        temp_scratch_ptr;
    volatile uint_16_ptr                      pUOCSR;
    USB_EHCI_DEV_INIT_STRUCT_PTR              param;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR) handle;
    if (usb_dev_ptr == NULL) {
        return;
    }
    
    param = (USB_EHCI_DEV_INIT_STRUCT_PTR) usb_dev_ptr->G.INIT_PARAM;

    USB_lock();

    cap_dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->CAP_BASE_PTR;
    /* Modify the base address of the VUSB_HS registers, do not use the old one anymore */
    usb_dev_ptr->G.DEV_PTR = (pointer) ((uint_32)cap_dev_ptr +
        (EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER) &
         EHCI_CAP_LEN_MASK));
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Program the controller to be the USB device controller */
    #if USBCFG_MEMORY_ENDIANNESS==MQX_BIG_ENDIAN
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE,
        ( USBHS_MODE_CTRL_MODE_DEV | USBHS_MODE_BIG_ENDIAN | USBHS_MODE_SETUP_LOCK_DISABLE));
    #else
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE,
        ( USBHS_MODE_CTRL_MODE_DEV | USBHS_MODE_SETUP_LOCK_DISABLE));
    #endif

    temp = (uint_8)(usb_dev_ptr->G.MAX_ENDPOINTS * 2);

    /* Initialize the internal dTD head and tail to NULL */
    usb_dev_ptr->DTD_HEAD = NULL;
    usb_dev_ptr->DTD_TAIL = NULL;

    /* Make sure the 16 MSBs of this register are 0s */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT,0);

    ep_queue_head_ptr = usb_dev_ptr->EP_QUEUE_HEAD_PTR;

    /* Initialize all device queue heads */
    for (i = 0; i < temp; i++)
    {
        /* Interrupt on Setup packet */
        EHCI_MEM_WRITE((ep_queue_head_ptr + i)->MAX_PKT_LENGTH,
            ((uint_32)USB_MAX_CTRL_PAYLOAD <<
                VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS)
            | VUSB_EP_QUEUE_HEAD_IOS);
        EHCI_MEM_WRITE((ep_queue_head_ptr + i)->NEXT_DTD_PTR, VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE);
    }

    #if PSP_HAS_DATA_CACHE
        USB_dcache_flush_mlines((void *)ep_queue_head_ptr, ((sizeof(USBHS_EP_QUEUE_HEAD_STRUCT))*temp));
    #endif

    /* Configure the Endpoint List Address */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR,
        (uint_32)ep_queue_head_ptr);

    dTD_ptr = usb_dev_ptr->DTD_ALIGNED_BASE_PTR;
    usb_dev_ptr->DTD_ENTRIES = 0;

    temp_scratch_ptr = usb_dev_ptr->G.SCRATCH_STRUCT_BASE;

    /* Enqueue all the dTDs */
    for (i=0;i<MAX_EP_TR_DESCRS;i++)
    {
        dTD_ptr->SCRATCH_PTR = temp_scratch_ptr;
        dTD_ptr->SCRATCH_PTR->FREE = _usb_dci_usbhs_free_dTD;
        /* Set the dTD to be invalid */
        EHCI_MEM_WRITE(dTD_ptr->NEXT_TR_ELEM_PTR, USBHS_TD_NEXT_TERMINATE);
        /* Set the Reserved fields to 0 */
        EHCI_MEM_CLEAR_BITS(dTD_ptr->SIZE_IOC_STS,USBHS_TD_RESERVED_FIELDS);
        dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer) usb_dev_ptr;
        _usb_dci_usbhs_free_dTD(usb_dev_ptr, (pointer)dTD_ptr);
        dTD_ptr++;
        temp_scratch_ptr++;
    }

    /*--- Intial Configuration ---*/
    /* Initialize the endpoint 0 properties */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0],
        (EHCI_EPCTRL_TX_DATA_TOGGLE_RST | EHCI_EPCTRL_RX_DATA_TOGGLE_RST));
    EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0],
        0 ); //(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL));

    /* Enable interrupts */
    /* There is no need to enable SOF Interrupt as its generated automatically
       by hardware irrespective of device attachment status */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR,
        (EHCI_INTR_INT_EN | EHCI_INTR_ERR_INT_EN |
        EHCI_INTR_PORT_CHANGE_DETECT_EN | EHCI_INTR_RESET_EN |
        EHCI_INTR_DEVICE_SUSPEND));

    /* Enable the interrupts */
    USB_unlock();

    usb_dev_ptr->G.USB_STATE = USB_STATE_UNKNOWN;

    /* Set the OTG Termination bit: not needed; the device termination is done with USBCMD:RS bit
    ** when B session is active: see below
    */
    //EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.OTGSC,USBHS_OTGSC_OT);

    /* Set the Run bit in the command register */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);
    
    /* Enable a valid B session to allow device to connect to a host*/
    pUOCSR = param->USBOTGCSR_ADDR;
    if (pUOCSR != NULL) {
      *pUOCSR = USB_OTG_UOCSR_BVLD;
    }
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_free_dTD
*  Returned Value : void
*  Comments       :
*        Enqueues a dTD onto the free DTD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_usbhs_free_dTD
(
    _usb_device_handle handle,
    /* [IN] the dTD to enqueue */
    pointer  dTD_ptr
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR) handle;

    /*
    ** This function can be called from any context, and it needs mutual
    ** exclusion with itself.
    */
    USB_lock();

    /*
    ** Add the dTD to the free dTD queue (linked via PRIVATE) and
    ** increment the tail to the next descriptor
    */
    EHCI_DTD_QADD(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL,
        (USBHS_EP_TR_STRUCT_PTR)dTD_ptr);
    usb_dev_ptr->DTD_ENTRIES++;
    USB_unlock();
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_add_dTD
*  Returned Value : USB_OK or error code
*  Comments       :
*        Adds a device transfer desriptor(s) to the queue.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_usbhs_add_dTD
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR  usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR     dev_ptr;
    volatile USBHS_EP_TR_STRUCT_PTR   dTD_ptr, temp_dTD_ptr, first_dTD_ptr = NULL;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT_PTR ep_queue_head_ptr;
    uint_32 curr_pkt_len, remaining_len, curr_offset, bit_pos, addr;
    uint_8 temp;

    /*********************************************************************
     For a optimal implementation, we need to detect the fact that
     we are adding DTD to an empty list. If list is empty, we can
     actually skip several programming steps esp. those for ensuring
     that there is no race condition.The following boolean will be useful
     in skipping some code here.
     *********************************************************************/
     boolean   list_empty = FALSE;
     boolean   read_safe;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    remaining_len = xd_ptr->WTOTALLENGTH;

    curr_offset = 0;
    temp = (uint_8)(2*xd_ptr->EP_NUM + xd_ptr->BDIRECTION);
    bit_pos = (uint_32)(1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));

    ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) +
        temp;

    #if PSP_HAS_DATA_CACHE
        USB_dcache_flush_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
    #endif

   /*********************************************************************
    This loops iterates through the length of the transfer and divides
    the data in to DTDs each handling the a max of 0x4000 bytes of data.
    The first DTD in the list is stored in a pointer called first_dTD_ptr.
    This pointer is later linked in to QH for processing by the hardware.
    *********************************************************************/

    do
    {
        /* Check if we need to split the transfer into multiple dTDs */
        if(remaining_len > VUSB_EP_MAX_LENGTH_TRANSFER)
        {
            curr_pkt_len = VUSB_EP_MAX_LENGTH_TRANSFER;
        }
        else
        {
            curr_pkt_len = remaining_len;
        }

        remaining_len -= curr_pkt_len;

        /* Get a dTD from the queue */
        EHCI_DTD_QGET(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, dTD_ptr);

        if (!dTD_ptr)
        {
            return USBERR_TR_FAILED;
        }

        usb_dev_ptr->DTD_ENTRIES--;

        if (curr_offset == 0)
        {
            first_dTD_ptr = dTD_ptr;
        }

        #if PSP_HAS_DATA_CACHE
            /**************************************************************
             USB Memzero does not bypass the cache and hence we must use
             DTD pointer to update the memory and bypass the cache. If
             your DTD are allocated from an uncached region, you can
             eliminitate this approach and switch back to USB_mem_zero().
             **************************************************************/
            EHCI_MEM_WRITE(dTD_ptr->NEXT_TR_ELEM_PTR, 0);
            EHCI_MEM_WRITE(dTD_ptr->SIZE_IOC_STS, 0);
            EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR0, 0);
            EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR1,0);
            EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR2,0);
            EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR3,0);
            EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR4,0);
        #else
            /* Zero the dTD. Leave the last 4 bytes as that is the
               scratch pointer*/
            USB_mem_zero((void *) dTD_ptr,(sizeof(USBHS_EP_TR_STRUCT) - 4));
        #endif

        /* Initialize the dTD */
        dTD_ptr->SCRATCH_PTR->PRIVATE = handle;

        /* Set the Terminate bit */
        EHCI_MEM_WRITE(dTD_ptr->NEXT_TR_ELEM_PTR, VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE);

        /*************************************************************
         FIX ME: For hig-speed and high-bandwidth ISO IN endpoints,
         we must initialize the multiplied field so that Host can issues
         multiple IN transactions on the endpoint. See the DTD data
         structure for MultiIO field.

         S Garg 11/06/2003
         *************************************************************/

        /* Set the reserved field to 0 */
        EHCI_MEM_CLEAR_BITS(dTD_ptr->SIZE_IOC_STS,USBHS_TD_RESERVED_FIELDS);

        /* 4K apart buffer page pointers */
        
        addr = (uint_32)(xd_ptr->WSTARTADDRESS + curr_offset);
        EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR0, addr);
        EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR1, (addr + 4096));
        EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR2, (addr + (4096*2)));
        EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR3, (addr + (4096*3)));
        EHCI_MEM_WRITE(dTD_ptr->BUFF_PTR4, (addr + (4096*4)));

        curr_offset += curr_pkt_len;
        /* Fill in the transfer size */
        if (!remaining_len)
        {
            EHCI_MEM_WRITE(dTD_ptr->SIZE_IOC_STS,((curr_pkt_len << USBHS_TD_LENGTH_BIT_POS) | USBHS_TD_IOC | USBHS_TD_STATUS_ACTIVE));
        }
        else
        {
            EHCI_MEM_WRITE(dTD_ptr->SIZE_IOC_STS,((curr_pkt_len << USBHS_TD_LENGTH_BIT_POS) | USBHS_TD_STATUS_ACTIVE));
        }
        #if PSP_HAS_DATA_CACHE
            USB_dcache_flush_mlines((void *)dTD_ptr,sizeof(USBHS_EP_TR_STRUCT));
        #endif
        /* Maintain the first and last device transfer descriptor per
           endpoint and direction */
        if (!usb_dev_ptr->EP_DTD_HEADS[temp])
        {
            usb_dev_ptr->EP_DTD_HEADS[temp] = dTD_ptr;
            /***********************************************
             If list does not have a head, it means that list
             is empty. An empty condition is detected.
             ***********************************************/
            list_empty = TRUE;
        }

        /* Check if the transfer is to be queued at the end or beginning */
        temp_dTD_ptr = usb_dev_ptr->EP_DTD_TAILS[temp];

        /* Remember which XD to use for this dTD */
        dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD = (pointer)xd_ptr;

        /* New tail */
        usb_dev_ptr->EP_DTD_TAILS[temp] = dTD_ptr;

        if (temp_dTD_ptr)
        {
            /* Should not do |=. The Terminate bit should be zero */
            EHCI_MEM_WRITE(temp_dTD_ptr->NEXT_TR_ELEM_PTR, (uint_32)dTD_ptr);
        }

    } while (remaining_len); /* EndWhile */


    /**************************************************************
     In the loop above DTD has already been added to the list
     However endpoint has not been primed yet. If list is not empty
     we need safer ways to add DTD to the
     existing list. Else we just skip to adding DTD to QH safely.
     **************************************************************/
    if(list_empty)/* If List is Empty : case 1*/
    {
        /* No other transfers on the queue */
        /* Step 1 of Executing a Transfer Descriptor documentation */
        EHCI_MEM_WRITE(ep_queue_head_ptr->NEXT_DTD_PTR,(uint_32)first_dTD_ptr);
        /* Step 2 of Executing a Transfer Descriptor documentation */
        EHCI_MEM_WRITE(ep_queue_head_ptr->SIZE_IOC_INT_STS,0);

        /* Prime the Endpoint */
        /* Step 3 of Executing a Transfer Descriptor documentation */
        EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME,bit_pos);
    }
    else /* If list is not empty : case 2*/
    {
        #ifdef TRIP_WIRE
            /*********************************************************
             Hardware v3.2+ require the use of semaphore to ensure that
             QH is safely updated.
             *********************************************************/

            /*********************************************************
             Check the prime bit. If set return USB_OK
             *********************************************************/
            if (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) &
                bit_pos)
            {
                return USB_OK;
            }

            read_safe = FALSE;
            while(!read_safe)
            {
                /*********************************************************
                 start with setting the semaphores
                 *********************************************************/
                EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,
                    EHCI_CMD_ATDTW_TRIPWIRE_SET);

                /*********************************************************
                 Read the endpoint status
                 *********************************************************/
                if(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD) &
                    EHCI_CMD_ATDTW_TRIPWIRE_SET)
                {
                    read_safe = TRUE;
                }
            }/*end while loop */

            /*********************************************************
             Clear the semaphore
             *********************************************************/
            EHCI_MEM_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,
                EHCI_CMD_SETUP_TRIPWIRE_CLEAR);
        #else   /*workaround old method */
            /* Start CR 1015 */
            /* Prime the Endpoint */
            EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME, bit_pos);

            if(!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos))
            {
               /* old workaround will be compiled */
               while(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos)
               {
                    /* Wait for the ENDPTPRIME to go to zero */
               }

               if(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)
               {
                    /* The endpoint was not not primed so no other transfers on
                       the queue */
                    return USB_OK;
               }
            }
            else
            {
                return USB_OK;
            }

            #if PSP_HAS_DATA_CACHE
                USB_dcache_invalidate_mlines((void *)first_dTD_ptr, sizeof(USBHS_EP_TR_STRUCT));
            #endif

            /* No other transfers on the queue */
            EHCI_MEM_WRITE(ep_queue_head_ptr->NEXT_DTD_PTR,(uint_32)first_dTD_ptr);
            EHCI_MEM_WRITE(ep_queue_head_ptr->SIZE_IOC_INT_STS, 0);

            #if PSP_HAS_DATA_CACHE
                USB_dcache_flush_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
            #endif

            /* Prime the Endpoint */
            EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME,bit_pos);
        #endif
    }
    return USB_OK;
   /* End CR 1015 */
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data by adding and executing the dTD. Non-blocking.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_send_data
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{   /* Body */
    /* Add and execute the device transfer descriptor */
    #if PSP_HAS_DATA_CACHE
        USB_dcache_flush_mlines((void *)xd_ptr->WSTARTADDRESS, xd_ptr->WTOTALLENGTH);
    #endif

    return(_usb_dci_usbhs_add_dTD(handle, xd_ptr));
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_recv_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Receives data by adding and executing the dTD. Non-blocking.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_recv_data
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] The transfer descriptor address */
    XD_STRUCT_PTR              xd_ptr
)
{   /* Body */
    /* Add and execute the device transfer descriptor */
    #if PSP_HAS_DATA_CACHE
        USB_dcache_invalidate_mlines((void *)xd_ptr->WSTARTADDRESS, xd_ptr->WTOTALLENGTH);
    #endif

    return(_usb_dci_usbhs_add_dTD(handle, xd_ptr));
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_process_tr_complete
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_process_tr_complete
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR  usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR     dev_ptr;
    volatile USBHS_EP_TR_STRUCT_PTR   dTD_ptr;
    volatile USBHS_EP_TR_STRUCT_PTR   temp_dTD_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT_PTR ep_queue_head_ptr;
    uint_8  temp, i, ep_num, direction;
    uint_32  remaining_length = 0;
    uint_32  actual_transfer_length = 0;
    uint_32  errors = 0;
    volatile EHCI_XD_STRUCT_PTR  xd_ptr;
    volatile EHCI_XD_STRUCT_PTR  temp_xd_ptr = NULL;
    uint_8_ptr buff_start_address = NULL;

    USB_EVENT_STRUCT event;
    uint_32 max_packet_size, bit_pos;


    _int_disable();
    USB_mem_zero(&event, sizeof(USB_EVENT_STRUCT)); /* Initialize event structure */

    event.handle = handle;
    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;
    /* We use separate loops for ENDPTSETUPSTAT and ENDPTCOMPLETE because the
       setup packets are to be read ASAP */
    /* Process all Setup packet received interrupts */
    bit_pos = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT);

    if (bit_pos)
    {

        for (i = 0; i < 16; i++)
        {
            if (bit_pos & (1 << i))
            {
                event.ep_num = USB_CONTROL_ENDPOINT;
                event.setup = TRUE;
                event.direction = USB_RECV;
                event.len = USB_SETUP_PKT_SIZE;
                event.buffer_ptr =
                    (uint_8_ptr)USB_mem_alloc_zero(USB_SETUP_PKT_SIZE);
                if(event.buffer_ptr == NULL)
                {
                    #ifdef _DEV_DEBUG
                        printf("1 memalloc failed in _usb_dci_usbhs_process_tr_complete\n");
                    #endif
                    return;
                }

                (void)_usb_dci_usbhs_get_setup_data(handle,
                    USB_CONTROL_ENDPOINT,event.buffer_ptr);

                (void)_usb_device_call_service(i, &event);

                USB_mem_free(event.buffer_ptr);
             } /* Endif */
         } /* Endfor */
    } /* Endif */

   /* Don't clear the endpoint setup status register here. It is cleared as a
      setup packet is read out of the buffer */

    /* Process non-setup transaction complete interrupts */
    bit_pos = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE);

    /* Clear the bits in the register */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE,bit_pos);

    if (bit_pos)
    {
        /* Get the endpoint number and the direction of transfer */
        for (i = 0; i < 32; i++)
        {
            if (bit_pos & (1 << i))
            {
                if (i > 15)
                {
                    ep_num = (uint_8)(i - 16);
                    direction = 1;
                }
                else
                {
                    ep_num = i;
                    direction = 0;
                } /* Endif */

                temp = (uint_8)(2*ep_num + direction);

                /* Get the first dTD */
                dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

                ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
                EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) +
                temp;
                max_packet_size = (EHCI_MEM_READ(ep_queue_head_ptr->MAX_PKT_LENGTH) >> 16) &
                    0x000007FF;

                #if PSP_HAS_DATA_CACHE
                    USB_dcache_invalidate_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
                #endif

                /* Process all the dTDs for respective transfers */
                while (dTD_ptr)
                {

                    if (EHCI_MEM_READ(dTD_ptr->SIZE_IOC_STS) & USBHS_TD_STATUS_ACTIVE)
                    {
                        /*No more dTDs to process. Next one is owned by VUSB*/
                        break;
                    } /* Endif */

                    /* Get the correct internal transfer descriptor */
                    xd_ptr = (EHCI_XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;

                    if (xd_ptr)
                    {
                        buff_start_address = xd_ptr->G.WSTARTADDRESS;
                        actual_transfer_length = xd_ptr->G.WTOTALLENGTH;
                        event.buffer_ptr = xd_ptr->G.WSTARTADDRESS;
                        temp_xd_ptr = xd_ptr;
                    } /* Endif */

                    /* Get the address of the next dTD */
                    temp_dTD_ptr = (USBHS_EP_TR_STRUCT_PTR)
                    (EHCI_MEM_READ(dTD_ptr->NEXT_TR_ELEM_PTR) & USBHS_TD_ADDR_MASK);

                    /* Read the errors */
                    errors = (EHCI_MEM_READ(dTD_ptr->SIZE_IOC_STS) & USBHS_TD_ERROR_MASK);

                    if (!errors)
                    {
                        /* No errors */
                        /* Get the length of transfer from the current dTD */
                        remaining_length += ((EHCI_MEM_READ(dTD_ptr->SIZE_IOC_STS) &
                            VUSB_EP_TR_PACKET_SIZE) >> 16);
                        actual_transfer_length -= remaining_length;
                    }
                    else
                    {
                        if (errors & USBHS_TD_STATUS_HALTED)
                        {
                            /* Clear the errors and Halt condition */
                            EHCI_MEM_CLEAR_BITS(ep_queue_head_ptr->SIZE_IOC_INT_STS,errors);
                        } /* Endif */
                    } /* Endif */

                    /* Retire the processed dTD */
                    (void)_usb_dci_usbhs_cancel_transfer(handle, ep_num,
                        direction);
                    event.ep_num = ep_num;
                    event.setup = FALSE;
                    event.direction = direction;
                    /* if wtotallength is greater tahn max_packet_size,
                       then it marks split transaction */
                    event.len = (xd_ptr->G.WTOTALLENGTH > max_packet_size)?
                        xd_ptr->G.WTOTALLENGTH : actual_transfer_length;

                    if (temp_dTD_ptr)
                    {
                        if((uint_32)temp_dTD_ptr->SCRATCH_PTR->\
                            XD_FOR_THIS_DTD != (uint_32)temp_xd_ptr)
                        {
                            /* Transfer complete. Call the register service
                               function for the endpoint */
                            (void)_usb_device_call_service(ep_num,&event);
                            remaining_length = 0;
                        } /* Endif */
                    }
                    else
                    {
                        /* Transfer complete. Call the register service
                           function for the endpoint */
                        (void)_usb_device_call_service(ep_num,&event);
                    } /* Endif */

                    dTD_ptr = temp_dTD_ptr;
                    errors = 0;
                } /* Endwhile */
            } /* Endif */
        } /* Endfor */
    }/* Endif */
    _int_enable();
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_process_reset
*  Returned Value : None
*  Comments       :
*        Services reset interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_process_reset
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR  usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR     dev_ptr;
    uint_32                    temp;
    uint_8                     cnt=0;

    USB_EVENT_STRUCT           event;

    event.handle = handle;
    event.ep_num = USB_CONTROL_ENDPOINT;
    event.setup = FALSE;
    event.direction = USB_RECV;
    event.buffer_ptr = NULL;
    event.len = ZERO_LENGTH;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* De-Init All the End Point.  */
    for (cnt = 0; cnt < USB_MAX_ENDPOINTS; cnt++)
    {
        _usb_dci_usbhs_deinit_endpoint(handle,cnt,USB_RECV);
        _usb_dci_usbhs_deinit_endpoint(handle,cnt,USB_SEND);
    }

    /* The address bits are past bit 25-31. Set the address */
    EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR,0xFE000000);

    /* Clear all the setup token semaphores */
    temp = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT);
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT ,temp);

    /* Clear all the endpoint complete status bits */
    temp = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE);
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE ,temp);

    while (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & 0xFFFFFFFF)
    {
        /* Wait until all ENDPTPRIME bits cleared */
    } /* Endif */

    /* Write 1s to the Flush register */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH, 0xFFFFFFFF);

    if (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]) &
        EHCI_PORTSCX_PORT_RESET)
    {
        usb_dev_ptr->BUS_RESETTING = TRUE;
        usb_dev_ptr->G.USB_STATE = USB_STATE_POWERED;
    }
    else
    {
        /* re-initialize */
        _usb_dci_usbhs_chip_initialize((pointer)usb_dev_ptr);
    } /* Endif */

    /* Inform the application so that it can cancel all previously queued transfers */
    _usb_device_call_service(USB_SERVICE_BUS_RESET,&event);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_process_suspend
*  Returned Value : None
*  Comments       :
*        Services suspend interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_process_suspend
(
      /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

    USB_EVENT_STRUCT          event;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;

    usb_dev_ptr->USB_STATE = USB_STATE_SUSPEND;

    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = handle;
    event.ep_num = (uint_8)USB_UNINITIALIZED_VAL_32;
    event.setup = 0;
    event.direction = 0;
    event.buffer_ptr = (uint_8*)NULL;
    event.len = ZERO_LENGTH;
    /* Inform the upper layers */
    (void)_usb_device_call_service(USB_SERVICE_SLEEP,&event);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_process_SOF
*  Returned Value : None
*  Comments       :
*        Services SOF interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_process_SOF
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_DEV_STATE_STRUCT_PTR                    usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR                       dev_ptr;

    USB_EVENT_STRUCT      event;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = handle;
    event.ep_num = (uint_8)USB_UNINITIALIZED_VAL_32;
    event.setup = 0;
    event.direction = 0;
    event.len = sizeof(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_FRINDEX);
    event.buffer_ptr = (uint_8_ptr)USB_mem_alloc_zero(event.len);
    if(event.buffer_ptr == NULL)
    {
        #ifdef _DEV_DEBUG
            printf("memalloc failed in _usb_dci_usbhs_process_SOF\n");
        #endif
        return;
    }

    USB_mem_copy((void*)&(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_FRINDEX),
        event.buffer_ptr, event.len);
    /* Inform the upper layer */
    (void)_usb_device_call_service(USB_SERVICE_SOF,&event);
    USB_mem_free(event.buffer_ptr);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_process_port_change
*  Returned Value : None
*  Comments       :
*        Services port change detect interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_process_port_change
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR dev_ptr;

    USB_EVENT_STRUCT      event;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Initialize the event strucutre to be passed to the upper layer*/
    event.handle = handle;
    event.ep_num = (uint_8)USB_UNINITIALIZED_VAL_32;
    event.setup = 0;
    event.direction = 0;

    if (usb_dev_ptr->BUS_RESETTING)
    {
        /* Bus reset operation complete */
        usb_dev_ptr->BUS_RESETTING = FALSE;
    } /* Endif */

    if (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]) &
        EHCI_PORTSCX_PORT_RESET))
    {
        /* Get the speed */
        if (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]) &
            EHCI_PORTSCX_PORT_HIGH_SPEED)
        {
            usb_dev_ptr->G.SPEED = USB_SPEED_HIGH;
        }
        else
        {
            usb_dev_ptr->G.SPEED = USB_SPEED_FULL;
        } /* Endif */

        event.len = sizeof(usb_dev_ptr->G.SPEED);
        event.buffer_ptr = (uint_8_ptr)USB_mem_alloc_zero(event.len);

        if(event.buffer_ptr == NULL)
        {
            #ifdef _DEV_DEBUG
                printf("memalloc failed in _usb_dci_usbhs_process_port_change\n");
            #endif
            return;
        }

        USB_mem_copy(&(usb_dev_ptr->G.SPEED), event.buffer_ptr,event.len);
        /* Inform the upper layers of the speed of operation */
        USB_mem_free(event.buffer_ptr);
    } /* Endif */

    if (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]) &
        EHCI_PORTSCX_PORT_SUSPEND)
    {
        usb_dev_ptr->G.USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->G.USB_STATE;
        usb_dev_ptr->G.USB_STATE = USB_STATE_SUSPEND;

        event.len = ZERO_LENGTH;
        event.buffer_ptr = (uint_8*)NULL;
        /* Inform the upper layers */
        (void)_usb_device_call_service(USB_SERVICE_SUSPEND,&event);
    } /* Endif */

    if(!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]) &
        EHCI_PORTSCX_PORT_SUSPEND) &&
        (usb_dev_ptr->G.USB_STATE == USB_STATE_SUSPEND))
    {
        usb_dev_ptr->G.USB_STATE = usb_dev_ptr->G.USB_DEV_STATE_B4_SUSPEND;

        event.len = ZERO_LENGTH;
        event.buffer_ptr = (uint_8*)NULL;
        /* Inform the upper layers */
        (void)_usb_device_call_service(USB_SERVICE_RESUME,&event);

        return;
    } /* Endif */

    usb_dev_ptr->G.USB_STATE = USB_STATE_DEFAULT;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_process_error
*  Returned Value : None
*  Comments       :
*        Services error interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_process_error
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_DEV_STATE_STRUCT_PTR  usb_dev_ptr;
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    /* Increment the error count */
    usb_dev_ptr->ERRORS++;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_isr
*  Returned Value : None
*  Comments       :
*        Services all the VUSB_HS interrupt sources
*
*END*-----------------------------------------------------------------*/
void _usb_dci_usbhs_isr(_usb_device_handle handle)

{   /* Body */
    volatile USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR      dev_ptr;
    uint_32 error;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    error = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS) &
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR);
        /* Clear all the interrupts occured */
        EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS, error);
        if(error & EHCI_STS_RESET)
        {
            /* Print so that user can be notified of reset in general */
            _usb_dci_usbhs_process_reset((pointer)usb_dev_ptr);
        }

        if (error & EHCI_STS_INT)
        {
            _usb_dci_usbhs_process_tr_complete((pointer)usb_dev_ptr);
        }

        if (error & EHCI_STS_PORT_CHANGE)
        {
            _usb_dci_usbhs_process_port_change((pointer)usb_dev_ptr);
        }

        if (error & EHCI_STS_ERR)
        {
            #ifdef _DEV_DEBUG
                printf("USB Error\n");
            #endif
            _usb_dci_usbhs_process_error((pointer)usb_dev_ptr);
        }

        if (error & EHCI_STS_SOF)
        {
            _usb_dci_usbhs_process_SOF((pointer)usb_dev_ptr);
        }

        if (error & EHCI_STS_SUSPEND)
        {
            _usb_dci_usbhs_process_suspend((pointer)usb_dev_ptr);
            return;
        }
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_set_address
*  Returned Value : None
*  Comments       :
*        Sets the newly assigned device address
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_set_address
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* Address of the device assigned by the host */
    uint_8                     address
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR            dev_ptr;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* The address bits are past bit 25-31. Set the address */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR,
        ((uint_32)address << USBHS_ADDRESS_BIT_SHIFT));

    usb_dev_ptr->G.USB_STATE = USB_STATE_ADDRESS;
    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_get_setup_data
*  Returned Value : None
*  Comments       :
*        Reads the Setup data from the 8-byte setup buffer
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_get_setup_data
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [OUT] address of the buffer to read the setup data into */
    uint_8_ptr                  buffer_ptr
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR              usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR                       dev_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT_PTR             ep_queue_head_ptr;
    boolean  read_safe;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Get the endpoint queue head */
    ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) +
        2*ep_num + USB_RECV;

    /********************************************************************
     CR 1219. Hardware versions 2.3+ have a implementation of tripwire
     semaphore mechanism that requires that we read the contents of
     QH safely by using the semaphore. Read the USBHS document to under
     stand how the code uses the semaphore mechanism. The following are
     the steps in brief

     1. USBCMD Write ‘1’ to Setup Tripwire in register.
     2. Duplicate contents of dQH.StatusBuffer into local software byte
        array.
     3  Read Setup TripWire in register. (if set - continue; if
        cleared goto 1.)
     4. Write '0' to clear Setup Tripwire in register.
     5. Process setup packet using local software byte array copy and
        execute status/handshake phases.
     ********************************************************************/

    /*if semaphore mechanism is used the following code is compiled in*/
    #ifdef TRIP_WIRE
        read_safe = FALSE;
        while(!read_safe)
        {
    #if PSP_HAS_DATA_CACHE
      USB_dcache_invalidate_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
      USB_dcache_invalidate_mlines((void *)ep_queue_head_ptr->SETUP_BUFFER, USB_SETUP_PKT_SIZE);
    #endif

            /*********************************************************
             start with setting the semaphores
             *********************************************************/

            EHCI_MEM_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,
                EHCI_CMD_SETUP_TRIPWIRE_SET);
            /*********************************************************
             Duplicate the contents of SETUP buffer to our buffer
             Note:On 5329 the data returned is little endian so it needs
             to be byte swapped.Here while copying it is been takencare.
             If you are using this as reference code care should be taken
             while copying the setup packet on your chip.
             *********************************************************/
             #if USBCFG_MEMORY_ENDIANNESS==MQX_BIG_ENDIAN
            buffer_ptr[0]= ep_queue_head_ptr->SETUP_BUFFER[3];
            buffer_ptr[1]= ep_queue_head_ptr->SETUP_BUFFER[2];
            buffer_ptr[2]= ep_queue_head_ptr->SETUP_BUFFER[1];
            buffer_ptr[3]= ep_queue_head_ptr->SETUP_BUFFER[0];
            buffer_ptr[4]= ep_queue_head_ptr->SETUP_BUFFER[7];
            buffer_ptr[5]= ep_queue_head_ptr->SETUP_BUFFER[6];
            buffer_ptr[6]= ep_queue_head_ptr->SETUP_BUFFER[5];
            buffer_ptr[7]= ep_queue_head_ptr->SETUP_BUFFER[4];
            #else
            _mem_copy((pointer)ep_queue_head_ptr->SETUP_BUFFER,buffer_ptr, 8);
            #endif
            /*********************************************************
             If setup tripwire semaphore is cleared by hardware it means
             that we have a danger and we need to restart.
             else we can exit out of loop safely.
             *********************************************************/
            if(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD) &
                EHCI_CMD_SETUP_TRIPWIRE_SET)
            {
                /* we can proceed exiting out of loop*/
                read_safe = TRUE;
            }
        }

        /*********************************************************
         Clear the semaphore bit now
         *********************************************************/
        EHCI_MEM_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,
            EHCI_CMD_SETUP_TRIPWIRE_SET);
    #else   /*when semaphore is not used */
        /*********************************************************
         Duplicate the contents of SETUP buffer to our buffer
         Note:On 5329 the data returned is little endian so it needs
         to be byte swapped.Here while copying it is been takencare.
         If you are using this as reference code care should be taken
         while copying the setup packet on your chip.
         *********************************************************/
        buffer_ptr[0]= ep_queue_head_ptr->SETUP_BUFFER[3];
        buffer_ptr[1]= ep_queue_head_ptr->SETUP_BUFFER[2];
        buffer_ptr[2]= ep_queue_head_ptr->SETUP_BUFFER[1];
        buffer_ptr[3]= ep_queue_head_ptr->SETUP_BUFFER[0];
        buffer_ptr[4]= ep_queue_head_ptr->SETUP_BUFFER[7];
        buffer_ptr[5]= ep_queue_head_ptr->SETUP_BUFFER[6];
        buffer_ptr[6]= ep_queue_head_ptr->SETUP_BUFFER[5];
        buffer_ptr[7]= ep_queue_head_ptr->SETUP_BUFFER[4];
    #endif

    /* Clear the bit in the ENDPTSETUPSTAT */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT,
        (uint_32)(1 << ep_num));

    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_init_endpoint
*  Returned Value : None
*  Comments       :
*        Initializes the specified endpoint and the endpoint queue head
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_init_endpoint
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the transaction descriptor address */
    XD_STRUCT_PTR              xd_p
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR       usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR                dev_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT _PTR_    ep_queue_head_ptr;
    uint_32                                      bit_pos;
    EHCI_XD_STRUCT_PTR                           xd_ptr = (EHCI_XD_STRUCT_PTR)xd_p;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    xd_ptr->MAX_PKTS_PER_UFRAME =
      (uint_8)(((uint_8)xd_ptr->G.DONT_ZERO_TERMINATE & USB_MAX_PKTS_PER_UFRAME) >> 1);

    if((xd_ptr->G.EP_TYPE > USB_INTERRUPT_PIPE) ||
        (xd_ptr->G.BDIRECTION > USB_SEND))
    {
        return USBERR_EP_INIT_FAILED;
    }

    /* before initializing cancel all transfers on EP as there may be calls
       for endpoint initialization more than once. This will free any allocated
       queue */
    (void)_usb_device_cancel_transfer(handle, xd_ptr->G.EP_NUM, xd_ptr->G.BDIRECTION);

    /* Get the endpoint queue head address */
    ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) +
        2 * xd_ptr->G.EP_NUM + xd_ptr->G.BDIRECTION;

    bit_pos = (uint_32)(1 << (16 * xd_ptr->G.BDIRECTION + xd_ptr->G.EP_NUM));

    /* Check if the Endpoint is Primed */
    if((!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos))
      &&
      (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)))
    {
        /* Set the max packet length, interrupt on Setup and Mult fields */
        if (xd_ptr->G.EP_TYPE == USB_ISOCHRONOUS_PIPE)
        {
            /* Mult bit should be set for isochronous endpoints */
            EHCI_MEM_WRITE(ep_queue_head_ptr->MAX_PKT_LENGTH,(uint_32)((xd_ptr->G.WMAXPACKETSIZE << 16) |
               ((xd_ptr->MAX_PKTS_PER_UFRAME ? xd_ptr->MAX_PKTS_PER_UFRAME : 1)
                << VUSB_EP_QUEUE_HEAD_MULT_POS)));
            #if PSP_HAS_DATA_CACHE
                USB_dcache_flush_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
            #endif
        }
        else
        {
            if (xd_ptr->G.EP_TYPE != USB_CONTROL_PIPE)
            {
                EHCI_MEM_WRITE(ep_queue_head_ptr->MAX_PKT_LENGTH, (uint_32)
                    ((xd_ptr->G.WMAXPACKETSIZE << 16) |
                     (xd_ptr->G.DONT_ZERO_TERMINATE ?
                     VUSB_EP_QUEUE_HEAD_ZERO_LEN_TER_SEL : 0)));
                if(xd_ptr->G.BDIRECTION == USB_RECV)
                {
                    g_ep_recv_buff_ptr[xd_ptr->G.EP_NUM] =
                        (uint_8*)USB_mem_alloc_uncached_zero(xd_ptr->G.WMAXPACKETSIZE);

                    if(g_ep_recv_buff_ptr[xd_ptr->G.EP_NUM] == NULL)
                    {
                        #ifdef _DEV_DEBUG
                            printf("memalloc failed in _usb_dci_usbhs_init_endpoint\n");
                        #endif
                        return USBERR_ALLOC;
                    }

                    (void)_usb_device_recv_data(handle, xd_ptr->G.EP_NUM, g_ep_recv_buff_ptr[xd_ptr->G.EP_NUM], xd_ptr->G.WMAXPACKETSIZE);
                }
            }
            else
            {
                EHCI_MEM_WRITE(ep_queue_head_ptr->MAX_PKT_LENGTH, (uint_32)
                    ((xd_ptr->G.WMAXPACKETSIZE << 16) | VUSB_EP_QUEUE_HEAD_IOS));
               #if PSP_HAS_DATA_CACHE
                    USB_dcache_flush_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
                #endif
            } /* Endif */
        } /* Endif */

        /* Enable the endpoint for Rx and Tx and set the endpoint type */
        EHCI_MEM_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->G.EP_NUM],
            ((xd_ptr->G.BDIRECTION ? (EHCI_EPCTRL_TX_ENABLE |
            EHCI_EPCTRL_TX_DATA_TOGGLE_RST) :
            (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) |
            (xd_ptr->G.EP_TYPE << (xd_ptr->G.BDIRECTION ?
            EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT))));
    }
    else
    {
        return USBERR_EP_INIT_FAILED;
    } /* Endif */

    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_deinit_endpoint
*  Returned Value : None
*  Comments       :
*        Disables the specified endpoint and the endpoint queue head
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_deinit_endpoint
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR             usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR                      dev_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT _PTR_          ep_queue_head_ptr;
    uint_32                                      bit_pos;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /*before de-initializing cancel all transfers on EP */
    (void)_usb_device_cancel_transfer(handle, ep_num, direction);

    /* Ger the endpoint queue head address */
    ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) +
        (2*ep_num + direction);

    #if PSP_HAS_DATA_CACHE
        USB_dcache_invalidate_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
    #endif

    bit_pos = (uint_32)(1 << (16 * direction + ep_num));

    /* Check if the Endpoint is Primed */
    if((!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos))
      &&
      (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)))
    {
        /* Reset the max packet length and the interrupt on Setup */
        EHCI_MEM_WRITE(ep_queue_head_ptr->MAX_PKT_LENGTH,0);

        /* Disable the endpoint for Rx or Tx and reset the endpoint type */
        EHCI_MEM_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num],
           (direction ? (EHCI_EPCTRL_TX_ENABLE|EHCI_EPCTRL_TX_TYPE):(EHCI_EPCTRL_RX_ENABLE|EHCI_EPCTRL_RX_TYPE)));

        /* TODO: flush buffers. See EHCI EPFLUSH register */
    }
    else
    {
        return USBERR_EP_DEINIT_FAILED;
    }

    if((ep_num != USB_CONTROL_ENDPOINT)&&(direction == USB_RECV))
    {
        USB_mem_free(g_ep_recv_buff_ptr[ep_num]);
    }

    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_get_transfer_status
*  Returned Value : USB_OK or error code
*  Comments       :
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_get_transfer_status
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr;
    volatile USBHS_EP_TR_STRUCT_PTR        dTD_ptr;
    volatile EHCI_XD_STRUCT_PTR            xd_ptr;
    uint_8                                 error;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;

    /* Unlink the dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2*ep_num + direction];

    if (dTD_ptr)
    {
        /* Get the transfer descriptor for the dTD */
        xd_ptr = (EHCI_XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        error = xd_ptr->G.BSTATUS;
    }
    else
    {
        error = USB_STATUS_IDLE;
    }

    return (error);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        Cancels a transfer
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_cancel_transfer
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR    usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR             dev_ptr;
    volatile USBHS_EP_TR_STRUCT_PTR           dTD_ptr, check_dTD_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT_PTR   ep_queue_head_ptr;
    volatile EHCI_XD_STRUCT_PTR               xd_ptr;
    uint_32                                   temp, bit_pos;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    bit_pos = (uint_32)(1 << (16 * direction + ep_num));
    temp = (uint_32)(2*ep_num + direction);

    ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) +
        temp;

    #if PSP_HAS_DATA_CACHE
        USB_dcache_invalidate_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
    #endif

    /* Unlink the dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

    if (dTD_ptr)
    {
        check_dTD_ptr = (USBHS_EP_TR_STRUCT_PTR) \
            ((uint_32)EHCI_MEM_READ(dTD_ptr->NEXT_TR_ELEM_PTR) &
            USBHS_TD_ADDR_MASK);

        if (EHCI_MEM_READ(dTD_ptr->SIZE_IOC_STS & USBHS_TD_STATUS_ACTIVE))
        {
            /* Flushing will halt the pipe */
            /* Write 1 to the Flush register */
            EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH,bit_pos);
            /* Wait until flushing completed */
            while (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH) & bit_pos)
            {
                /* ENDPTFLUSH bit should be cleared to indicate this
                   operation is complete */
            }

            while (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)
            {
                /* Write 1 to the Flush register */
                EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH,bit_pos);

                /* Wait until flushing completed */
                while (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH) & bit_pos)
                {
                    /* ENDPTFLUSH bit should be cleared to indicate this
                       operation is complete */
                }
            }
        }

        /* Retire the current dTD */
        EHCI_MEM_WRITE(dTD_ptr->SIZE_IOC_STS, 0);
        EHCI_MEM_WRITE(dTD_ptr->NEXT_TR_ELEM_PTR, USBHS_TD_NEXT_TERMINATE);

        /* The transfer descriptor for this dTD */
        xd_ptr = (EHCI_XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer) usb_dev_ptr;
        /* Free the dTD */
        _usb_dci_usbhs_free_dTD(usb_dev_ptr, (pointer)dTD_ptr);

        /* Update the dTD head and tail for specific endpoint/direction */
        if (!check_dTD_ptr)
        {
            usb_dev_ptr->EP_DTD_HEADS[temp] = NULL;
            usb_dev_ptr->EP_DTD_TAILS[temp] = NULL;

            if (xd_ptr)
            {
                xd_ptr->G.SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
                /* Free the transfer descriptor */
                _usb_device_free_XD(usb_dev_ptr, (pointer)xd_ptr);
            }

            /* No other transfers on the queue */
            EHCI_MEM_WRITE(ep_queue_head_ptr->NEXT_DTD_PTR, VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE);
            EHCI_MEM_WRITE(ep_queue_head_ptr->SIZE_IOC_INT_STS,0);
        }
        else
        {
            usb_dev_ptr->EP_DTD_HEADS[temp] = check_dTD_ptr;

            if (xd_ptr)
            {
                if ((uint_32)check_dTD_ptr->SCRATCH_PTR->\
                    XD_FOR_THIS_DTD != (uint_32)xd_ptr)
                {
                    xd_ptr->G.SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
                    /* Free the transfer descriptor */
                    _usb_device_free_XD(usb_dev_ptr, (pointer)xd_ptr);
                }
            }

            if (EHCI_MEM_READ(check_dTD_ptr->SIZE_IOC_STS) & USBHS_TD_STATUS_ACTIVE)
            {
                /* Start CR 1015 */
                /* Prime the Endpoint */
                EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME, bit_pos);

                if (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos))
                {
                    while(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos)
                    {
                        /* Wait for the ENDPTPRIME to go to zero */
                    }

                    if (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)
                    {
                        /* The endpoint was not not primed so no other
                        transfers on the queue */
                        return USB_OK;
                    }
                }
                else
                {
                    return USB_OK;
                }

                /* No other transfers on the queue */
                EHCI_MEM_WRITE(ep_queue_head_ptr->NEXT_DTD_PTR, (uint_32)check_dTD_ptr);
                EHCI_MEM_WRITE(ep_queue_head_ptr->SIZE_IOC_INT_STS , 0);

                /* Prime the Endpoint */
                EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME,bit_pos);
            }
        }
    }

    return USB_OK;
    /* End CR 1015 */
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_shutdown
*  Returned Value : None
*  Comments       :
*        Shuts down the VUSB_HS Device
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_shutdown
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR              usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR                       dev_ptr;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Disable interrupts */
    EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR,
        (EHCI_INTR_INT_EN | EHCI_INTR_ERR_INT_EN | EHCI_INTR_PORT_CHANGE_DETECT_EN | EHCI_INTR_RESET_EN));

    /* Reset the Run the bit in the command register to stop VUSB */
    EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);

    /* Reset the controller to get default values */
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD , EHCI_CMD_CTRL_RESET);

    USB_mem_free((pointer)usb_dev_ptr->DRIVER_MEMORY);

    return USB_OK;
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_assert_resume
*  Returned Value : None
*  Comments       :
*        Resume signalling for remote wakeup
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_assert_resume
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR          dev_ptr;
    uint_32                                temp;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Assert the Resume signal */
    temp = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]);
    temp &= ~EHCI_PORTSCX_W1C_BITS;
    temp |= EHCI_PORTSCX_PORT_FORCE_RESUME;
    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] , temp);

    /* Port change interrupt will be asserted at the end of resume
       operation */

   return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_stall_endpoint
*  Returned Value : None
*  Comments       :
*        Stalls the specified endpoint
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_stall_endpoint
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR    usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR             dev_ptr;
    volatile USBHS_EP_QUEUE_HEAD_STRUCT _PTR_ ep_queue_head_ptr;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Get the endpoint queue head address */
    ep_queue_head_ptr = (USBHS_EP_QUEUE_HEAD_STRUCT_PTR)
        EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR) + 2 * ep_num + direction;

    #if PSP_HAS_DATA_CACHE
        USB_dcache_flush_mlines((void *)ep_queue_head_ptr, sizeof(USBHS_EP_QUEUE_HEAD_STRUCT));
    #endif

    /* Stall the endpoint for Rx or Tx and set the endpoint type */
    if (EHCI_MEM_READ(ep_queue_head_ptr->MAX_PKT_LENGTH) & VUSB_EP_QUEUE_HEAD_IOS)
    {
        /* This is a control endpoint so STALL both directions */
        EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num],
            (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL));
    }
    else
    {
        EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num],
            (direction ? EHCI_EPCTRL_TX_EP_STALL:EHCI_EPCTRL_RX_EP_STALL));
    }

    return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_usbhs_unstall_endpoint
*  Returned Value : None
*  Comments       :
*        Unstall the specified endpoint in the specified direction
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_unstall_endpoint
(
    /* [IN] the USB_dev_initialize state structure */
    _usb_device_handle         handle,

    /* [IN] the Endpoint number */
    uint_8                     ep_num,

    /* [IN] direction */
    uint_8                     direction
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR  usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR     dev_ptr;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    /* Enable the endpoint for Rx or Tx and set the endpoint type */
    EHCI_MEM_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num],
        (direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL));

    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_usbhs_get_endpoint_status
* Returned Value : None
* Comments       :
*     Gets the endpoint status
*
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_get_endpoint_status
(
    /* [IN] Handle to the USB device */
    _usb_device_handle   handle,

    /* [IN] Endpoint to get */
    uint_8               ep,
    /* [OUT] Status to get */
    uint_16_ptr error
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR          dev_ptr;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    *error = (uint_16)((EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep]) &
        (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL)) ? 1 : 0);

    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_usbhs_set_test_mode
* Returned Value : None
* Comments       :
*     sets/resets the test mode
*
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_set_test_mode
(
    /* [IN] Handle to the USB device */
    _usb_device_handle handle,

    /* [IN] Test mode */
    uint_16 test_mode
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR       usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR                dev_ptr;
    uint_32                                      temp;

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    temp = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0]);

    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0],
        (temp | EHCI_EPCTRL_TX_DATA_TOGGLE_RST));

    if (test_mode == USB_TEST_MODE_TEST_PACKET)
    {
        _usb_device_send_data(handle, 0, test_packet,
            USB_TEST_MODE_TEST_PACKET_LENGTH);
    } /* Endif */

    temp = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]);
    temp &= ~EHCI_PORTSCX_W1C_BITS;

    EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0], (temp | ((uint_32)test_mode << 8)));

    return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_usbhs_set_endpoint_status
* Returned Value : None
* Comments       :
*     Sets the endpoint registers e.g. to enable TX, RX, control
*
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_dci_usbhs_set_endpoint_status
(
    /* [IN] Handle to the USB device */
    _usb_device_handle   handle,

    /* [IN] Endpoint to set */
    uint_8               component,

    /* [IN] Endpoint characteristics */
    uint_16               setting
)
{   /* Body */
    volatile USB_EHCI_DEV_STATE_STRUCT_PTR  usb_dev_ptr;
    volatile USBHS_REG_STRUCT_PTR     dev_ptr;
    uint_8 error = USBERR_ERROR;
    uint_8 ep_num = (uint_8)(component & USB_STATUS_ENDPOINT_NUMBER_MASK);
    uint_8 direction = (uint_8)((component>>COMPONENT_PREPARE_SHIFT) & 0x01);

    if (ep_num > USB_MAX_ENDPOINTS) {
        return error;
    }

    usb_dev_ptr = (USB_EHCI_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (USBHS_REG_STRUCT_PTR)usb_dev_ptr->G.DEV_PTR;

    if(setting == USB_STATUS_STALLED)
    {
        error = _usb_device_stall_endpoint(handle, ep_num, direction);

    }
    else if(setting == USB_STATUS_IDLE)
    {
        error = _usb_device_unstall_endpoint(handle, ep_num, direction);
    } /* Endif */

    return USB_OK;
} /* EndBody */

/* EOF */
