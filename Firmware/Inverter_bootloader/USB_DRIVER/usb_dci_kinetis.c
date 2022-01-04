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
**************************************************************************/ /*!
  *
  * @file usb_dci_kinetis.c
  *
  * @author
  *
  * @version
  *
  * @date
  *
  * @brief The file contains Kinetis USB stack controller layer implementation.
  *
  *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <string.h>
#include "app_cfg.h"
#include "usb_dciapi.h" /* USB DCI API Header File */
#include "usb_devapi.h" /* USB Device API Header File */
#include "usb_dci_kinetis.h"    /* USB DCI Header File */
#include "usb_bdt_kinetis.h"    /* USB BDT Structure Header File */
/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* location for BDT Table and buff */
#pragma data_alignment = 512

static INT8U g_Mem[BYTES_1024];
/* Pointer to BDT Map Structure */
T_USB_BDT    *g_BDT = NULL;

static T_EP_STATE  g_ep_state_tbl[MAX_EP_CBL_INDEX >> 1]; // Таблица состояний конечных точек отдельно для принимающих и передающих

static INT8U  g_dci_controller_Id; /* stores Controller ID */

static INT32U g_bdt_address;     /* Start BDT buffer Address */

static INT8U  g_setup_direction = USB_TRF_UNKNOWN;  // Направление передачи задаваемое транзакцией SETUP 
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void USB_Bus_Reset_Handler(void);
static INT8U USB_DCI_Get_EP_CBL_Index(INT8U ep_num, INT8U direction, BOOLEAN odd);
static INT8U USB_DCI_Get_EP_CBL_Valid_Index(INT8U ep_num, INT8U direction, BOOLEAN odd);
#ifdef LONG_SEND_TRANSACTION
static void USB_DCI_Prepare_Send_Data(T_EP_CBL *p_ep_cbl, T_EP_STATE *p_ep_state);
#endif
static void USB_Packet_RXTX_Handler(INT8U stat, USB_DEV_EVENT_STRUCT *msg);
#ifdef USB_LOWPOWERMODE
static void Enter_StopMode(STOP_MODE stop_mode);
#endif
/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
extern void Watchdog_Reset(void);
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/**************************************************************************/ /*!
  *
  * @name  USB_Bus_Reset_Handler
  *
  * @brief The function handles Bus Reset Interrupt
  *
  * @param  None
  *
  * @return None
  *
  ******************************************************************************
  * This functions is called when USB Bus Reset event is received on USB Bus.
  * This function clears all the errors conditions and reinit Global data
  * structures. Also resets USB device controller.
  *****************************************************************************/
static void USB_Bus_Reset_Handler(void)
{
  USB0_ERRSTAT = ERR_STAT_CLEAR_ALL;  /* clear USB error flag */
  USB0_CTL |= USB_CTL_ODDRST_MASK;                /* Reset to Even buffer */
  USB0_ADDR = 0;                       /* reset to default address */
  /* Select System Clock and Disable Weak Pull Downs */
  USB0_USBCTRL = 0x00;

  /* Clear bdt elem structure */
  Clear_Mem((INT8U_ptr)(g_ep_state_tbl), (sizeof(T_EP_STATE) * (MAX_EP_CBL_INDEX >> 1)), (INT8U)UNINITIALISED_VAL);

  /* Clear Memory for BDT and buffer Data */
  Clear_Mem((INT8U_ptr)g_BDT, BYTES_1024, (INT8U)NULL);

  /* Initialize BDT buffer address */
  g_bdt_address = ((INT32U)g_BDT + BYTES_512);

  g_setup_direction = USB_TRF_UNKNOWN;

  USB0_CTL &= ~USB_CTL_ODDRST_MASK;
  USB0_USBTRC0 |= 0x40;             /* attach CFv1 core to USB bus */

  USB0_ERREN = ERR_ENB_ENABLE_ALL;   /* Enable All Error Interrupts */
  USB0_INTEN = INTENB_BUS_RESET_VAL; /* Enable All Interrupts except RESUME */
  USB0_CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;

  return;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Get_BDT_Index
  *
  * @brief The function maps endpoint number and direction to bdt index
  *
  * @param  ep_num   : Endpoint Number
  * @param  direction: Endpoint direction
  * @param  odd      : Odd or even buffer
  *
  * @return bdt index         : Mapped bdt index
  *         INVALID_BDT_INDEX : Incase of error
  *
  ******************************************************************************
  * This function returns BDT Index from Endpoint number, direction,
  * odd/even buffer
  *****************************************************************************/
static INT8U USB_DCI_Get_EP_CBL_Index(INT8U ep_num, INT8U direction,  BOOLEAN odd)
{
  INT8U ep_cbl_indx = INVALID_BDT_INDEX;

  if ( ep_num < MAX_SUPPORTED_ENDPOINTS )
  {
    /* per endpoint 4 ep_cbl_indx -- 2 for recv 2 for send */
    ep_cbl_indx = (INT8U)((ep_num * 4) + (INT8U)odd);

    if ( direction == USB_SEND )
    {
      ep_cbl_indx += 2;
    }
  }
  return ep_cbl_indx;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Validate_Param
  *
  * @brief The function validates endpoint number & direction parameters
  *        and returns bdt index.
  *
  * @param  ep_num   : Endpoint Number
  * @param  direction: Endpoint direction
  * @param  odd      : odd or even buffer
  *
  * @return bdt index         : mapped bdt index
  *         INVALID_BDT_INDEX : incase of error
  *
  ******************************************************************************
  * This function validates endpoint parameters and returns bdt index
  *****************************************************************************/
static INT8U USB_DCI_Get_EP_CBL_Valid_Index(INT8U ep_num,INT8U direction,BOOLEAN odd)
{
  /* Get bdt index mapped to endpoint number-direction and odd/even buffer */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Index(ep_num, direction, odd);

  if ( (ep_cbl_indx  != INVALID_BDT_INDEX) && (g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)].len == (INT16U)UNINITIALISED_VAL) )
  {
    /* Incase length in bdt_elem is uninitialised return invalid index */
    ep_cbl_indx = INVALID_BDT_INDEX;
  }
  return ep_cbl_indx;
}

#ifdef LONG_SEND_TRANSACTION
/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Prepare_Send_Data
  *
  * @brief The function sets up the BDT for Send
  *
  * @param  buffer_dsc   : Pointer to buffer descriptor element in USB_RAM
  * @param  bdt_elem     : Pointer to per endpoint/direction structure
  *
  * @return None
  *
  ******************************************************************************
  * This functions configures Buffer Descriptor (Address and Count)
  *****************************************************************************/
static void USB_DCI_Prepare_Send_Data(
                                      T_EP_CBL *p_ep_cbl,  /* [OUT] Pointer to buffer descriptor
                                                                  element in USB_RAM */
                                      T_EP_STATE *p_ep_state     /* [IN] Pointer to per endpoint/direction
                                                                  structure */
                                      )
{
  INT8U_ptr buff_ptr = p_ep_state->app_buffer + p_ep_state->curr_offset;
  INT16U current_count = 0;

  /* adjust size based on the input at the init endpoint */
  if ( (p_ep_state->app_len - p_ep_state->curr_offset) > p_ep_state->len )
  {
    /* If size of packet is greater than endpoint buffer size */
    current_count = p_ep_state->len;
  }
  else
  {
    /* If size of packet is smaller than endpoint buffer size */
    current_count = (INT16U)(p_ep_state->app_len - p_ep_state->curr_offset);
  }

  p_ep_cbl->cnt = current_count;

  p_ep_cbl->addr = (INT32U)buff_ptr;
}
#endif

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Init
  *
  * @brief The function initializes the Controller layer
  *
  * @param controller_ID : Controller ID
  *
  * @return status
  *         USB_OK    : Always
  ******************************************************************************
  * Initializes the USB controller
  *****************************************************************************/
INT8U USB_DCI_Init(
                   INT8U    controller_ID   /* [IN] Controller ID */
                   )
{
  /* Select System Clock and Disable Weak Pull Downs */
  USB0_USBCTRL = 0x00;

  /* save the controller_ID for future use*/
  g_dci_controller_Id = controller_ID;

  /* Clear bdt elem structure */
  Clear_Mem((INT8U_ptr)(g_ep_state_tbl), (sizeof(T_EP_STATE) * (MAX_EP_CBL_INDEX >> 1)), (INT8U)UNINITIALISED_VAL);
  g_BDT = (T_USB_BDT *)((INT32U)g_Mem);

  /* Clear Memory for BDT and buffer Data */
  Clear_Mem((INT8U_ptr)g_BDT, BYTES_1024, (INT8U)NULL);

#ifndef OTG_BUILD
  /* Hard Reset to the USB Module */
  USB0_USBTRC0 |= USB_USBTRC0_USBRESET_MASK;

  /* loop till the Reset bit clears  */
  while ((USB0_USBTRC0 & USB_USBTRC0_USBRESET_MASK))
  {};

#endif

  g_setup_direction = USB_TRF_UNKNOWN;

  /* attach CFv1 core to USB bus*/
  //USB0_USBTRC0 |= 0x40;


  /* Set the BDT Table address, Need to be on 512 byte boundry */
  /* D8 Bit is masked in BDT_PAGE_01 */
  USB0_BDTPAGE1 = (INT8U)(((INT32U)g_BDT >> 8) & 0xFE);
  USB0_BDTPAGE2 = (INT8U)((INT32U)g_BDT >> 16);
  USB0_BDTPAGE3 = (INT8U)((INT32U)g_BDT >> 24);

  /* Initialized BDT buffer address */
  g_bdt_address = ((INT32U)g_BDT + BYTES_512);

#ifndef OTG_BUILD
  /* Pull Up configuration */
  USB0_CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;
#endif


  USB0_CTL = USB_CTL_USBENSOFEN_MASK; /* Enable USB module */
  USB0_ISTAT = INT_STAT_CLEAR_ALL;      /* Clear USB interrupts*/

  /* Remove suspend state */
  USB0_USBCTRL &= ~USB_USBCTRL_SUSP_MASK;

  /* Enable USB RESET Interrupt */
  USB0_INTEN |= USB_INTEN_USBRSTEN_MASK;

  USB0_OTGCTL = USB_OTGCTL_DPHIGH_MASK | USB_OTGCTL_OTGEN_MASK;

  Set_VIC_chanel(INT_USB0, 2, 0);

  return USB_OK;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_DeInit
  *
  * @brief The function de-initializes the Controller layer
  *
  * @param controller_ID : Controller ID
  *
  * @return status
  *         USB_OK    : Always
  ******************************************************************************
  * Initializes the USB controller
  *****************************************************************************/
INT8U USB_DCI_DeInit(void)
{
  /* Detach CFv1 core to USB bus*/
  USB0_USBTRC0 &= ~0x40;

  /* Clear USB interrupts*/
  USB0_ISTAT = INT_STAT_CLEAR_ALL;

  /* Disable USB RESET Interrupt */
  USB0_INTEN &= ~USB_INTEN_USBRSTEN_MASK;

  /* Disable USB module */
  USB0_CTL &= ~USB_CTL_USBENSOFEN_MASK;

  return USB_OK;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Init_EndPoint
  *
  * @brief The function initializes an endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_ptr        : Pointer to EndPoint Structures
  * @param flag          : Zero Termination
  *
  * @return status
  *         USB_OK                    : When Successfull
  *         USBERR_EP_INIT_FAILED     : When Error
  ******************************************************************************
  *
  * This function initializes an endpoint and the Bufffer Descriptor Table
  * entry associated with it. Incase the input parameters are invalid it will
  * return USBERR_EP_INIT_FAILED error.
  *
  *****************************************************************************/
INT8U USB_DCI_Init_EndPoint(
                            INT8U                controller_ID, /* [IN] Controller ID */
                            USB_EP_STRUCT_PTR    ep_ptr,       /* [IN] Pointer to Endpoint structure,
                                                                       (endpoint number,
                                                                        endpoint type,
                                                                        endpoint direction,
                                                                        max packet size) */
                            BOOLEAN              flag          /* [IN] Zero Termination */
                            )
{
  INT8U        bdtmap_index;
  INT8U        bdtelem_index;
  INT8U        ep_num     = ep_ptr->ep_num;
  INT8U        direction  = ep_ptr->direction;
  INT32U       ep_ctrl[2] = { EP_OUT, EP_IN };
  T_EP_CBL     *p_tmp_ep_cbl;
  T_EP_STATE   *p_ep_state;

  /* if the max packet size is greater than the max buffer size */
  if ( ep_ptr->size > MAX_EP_BUFFER_SIZE )
  {
    ep_ptr->size = MAX_EP_BUFFER_SIZE;
  }

  /* Get the bdt index correspoding to the endpoint */
  bdtmap_index = USB_DCI_Get_EP_CBL_Index(ep_num, direction, USB_RAM_EVEN_BUFFER);
  bdtelem_index = (INT8U)TRANSFER_INDEX(bdtmap_index);

  /*
     incase the bdtmap_index is invalid
     or already initialised return with an error
  */
  if ( (bdtmap_index  == INVALID_BDT_INDEX) ||
       (g_ep_state_tbl[bdtelem_index].len != (INT16U)UNINITIALISED_VAL) ||
       (g_bdt_address >= ((INT32U)g_Mem + BYTES_1024)) ||
       (ep_ptr->type > USB_INTERRUPT_PIPE) ||
       (ep_ptr->direction > USB_SEND) )
  {
    return USBERR_EP_INIT_FAILED;
  }

  p_ep_state = &g_ep_state_tbl[bdtelem_index];
  /* Reset Handler resets bdtmap_index to UNINITIALISED_VAL */
  if ( p_ep_state->ep_cbl_indx == (INT8U)UNINITIALISED_VAL )
  {
    p_ep_state->ep_cbl_indx = 0;
  }

  /* update bdt element structure */
  p_ep_state->len = (INT16U)ep_ptr->size;
  p_ep_state->flag = flag;
  /* preserving even/odd buffer bit */
  p_ep_state->ep_cbl_indx &= 0x01;
  p_ep_state->ep_cbl_indx |= ((direction << 1) | (ep_num << 2));
  p_ep_state->addr = g_bdt_address;
  p_ep_state->type = ep_ptr->type;
  p_ep_state->direction = direction;

  p_tmp_ep_cbl = &g_BDT->ep_cbl[p_ep_state->ep_cbl_indx];

  /* Update BDTMAP for endpoint's EVEN Buffer */
  p_tmp_ep_cbl->cnt = (INT16U)ep_ptr->size;
  p_tmp_ep_cbl->addr = g_bdt_address;
  p_tmp_ep_cbl->bits.all = (_CPU | _DATA0 | _DTS);

  /* Update BDTMAP for endpoint's ODD Buffer */
  p_tmp_ep_cbl = &g_BDT->ep_cbl[p_ep_state->ep_cbl_indx ^ 1];

  p_tmp_ep_cbl->cnt = (INT16U)ep_ptr->size;
  p_tmp_ep_cbl->addr = g_bdt_address;
  p_tmp_ep_cbl->bits.all = (_CPU | _DATA1 | _DTS);

  /* update the buffer address for the next endpoint */
  g_bdt_address += ep_ptr->size;

  if ( direction == USB_RECV )
  {
    /*
       For Recv Endpoints
       Give SIE Control to DATA0
    */
    p_tmp_ep_cbl = &g_BDT->ep_cbl[p_ep_state->ep_cbl_indx];
    p_tmp_ep_cbl->bits.all |= _OWN;
  }

  /* enable handshake for Non-Isochronous Endpoints */
  ep_ctrl[direction] |= ((ep_ptr->type != USB_ISOCHRONOUS_PIPE) ? HSHK_EN : 0x00);
  /* set the EndPoint Control MCU Register*/
  *((&USB0_ENDPT0) + (4 * ep_num)) |= ep_ctrl[direction];

  return USB_OK;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Cancel_Transfer
  *
  * @brief The function cancels any pending Transfers which ahve not been sent
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param direction     : Endpoint direction
  *
  * @return status
  *         USBERR_NOT_SUPPORTED : Always
  ******************************************************************************
  * This function just returns Error Code not supported
  *****************************************************************************/
INT8U USB_DCI_Cancel_Transfer(
                              INT8U    controller_ID,   /* [IN] Controller ID */
                              INT8U    ep_num,          /* [IN] Endpoint number */
                              INT8U    direction        /* [IN] Endpoint direction */
                              )
{
#ifdef LONG_TRANSACTION
  INT8U status = USBERR_UNKNOWN_ERROR;

  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, direction, USB_RAM_EVEN_BUFFER);
  INT8U bdtelem_index = (INT8U)TRANSFER_INDEX(ep_cbl_indx);

  /* Check for valid bdt index */
  if ( ep_cbl_indx != INVALID_BDT_INDEX )
  {
    T_EP_STATE *p_ep_state   = &g_ep_state_tbl[bdtelem_index];
    T_EP_CBL   *p_ep_cbl     = &g_BDT->ep_cbl[p_ep_state->ep_cbl_indx];
    T_EP_CBL   *p_ep_cbl_alt = &g_BDT->ep_cbl[p_ep_state->ep_cbl_indx ^ 1];


    /* Clear SIE Control Bit for both buffers*/
    p_ep_cbl->bits.all &= ~_OWN;
    p_ep_cbl_alt->bits.all &= ~_OWN;
    p_ep_state->app_len = (INT16U)UNINITIALISED_VAL;

    status = USB_OK;
  }
  return status;
#else
  return USBERR_NOT_SUPPORTED;
#endif
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Deinit_EndPoint
  *
  * @brief The function de initializes an endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param direction     : Endpoint direction
  *
  * @return status
  *         USB_OK                   : When successfull
  *         USBERR_EP_DEINIT_FAILED  : When unsuccessfull
  ******************************************************************************
  *
  * This function un-intializes the endpoint by clearing the corresponding
  * endpoint control register and then clearing the bdt elem.
  *
  *****************************************************************************/
INT8U USB_DCI_Deinit_EndPoint(
                              INT8U    controller_ID,   /* [IN] Controller ID */
                              INT8U    ep_num,          /* [IN] Endpoint number */
                              INT8U    direction        /* [IN] Endpoint direction */
                              )
{

  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, direction, USB_RAM_EVEN_BUFFER);
  INT8U bdtelem_index = (INT8U)TRANSFER_INDEX(ep_cbl_indx);

  /* incase the ep_cbl_indx is invalid*/
  if ( ep_cbl_indx  == INVALID_BDT_INDEX )
  {
    return USBERR_EP_DEINIT_FAILED;
  }
  USB_DCI_Cancel_Transfer(controller_ID, ep_num, direction);
  /* delete buffer space for both even and odd buffers */
  g_bdt_address -= (g_ep_state_tbl[bdtelem_index].len);

  /* Disable endpoint */
  *((&USB0_ENDPT0) + (4 * ep_num)) = EP_DISABLE;

  /* un-initialize the bdt_elem structure for this endpoint */
  g_ep_state_tbl[bdtelem_index].len = (INT16U)UNINITIALISED_VAL;
  g_ep_state_tbl[bdtelem_index].addr = (INT32U)UNINITIALISED_VAL;

  return USB_OK;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Stall_EndPoint
  *
  * @brief The function stalls an endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param direction     : Endpoint direction
  *
  * @return None
  *
  ******************************************************************************
  * This function stalls the endpoint by setting Endpoint BDT
  *****************************************************************************/
void USB_DCI_Stall_EndPoint(
                            INT8U    controller_ID,   /* [IN] Controller ID */
                            INT8U    ep_num,          /* [IN] Endpoint number */
                            INT8U    direction        /* [IN] Endpoint direction */
                            )
{

  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, direction, USB_RAM_EVEN_BUFFER);

  T_EP_STATE   *p_ep_state = &g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)];
  ep_cbl_indx = p_ep_state->ep_cbl_indx;

  /* Check for valid bdt index */
  if ( ep_cbl_indx != INVALID_BDT_INDEX )
  {
    (void)USB_DCI_Cancel_Transfer(controller_ID, ep_num, direction);

    /* Stall endpoint */
    g_BDT->ep_cbl[ep_cbl_indx].bits.all |= (_OWN | _BDTSTALL);
  }

  return;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Unstall_EndPoint
  *
  * @brief The function unstalls an endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param direction     : Endpoint direction
  *
  * @return None
  *
  ******************************************************************************
  * This function unstalls the endpoint by clearing Endpoint Control Register
  * and BDT
  *****************************************************************************/
void USB_DCI_Unstall_EndPoint(
                              INT8U    controller_ID,   /* [IN] Controller ID */
                              INT8U    ep_num,          /* [IN] Endpoint number */
                              INT8U    direction        /* [IN] Endpoint direction */
                              )
{

  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, direction, USB_RAM_EVEN_BUFFER);

  T_EP_STATE *p_ep_state = &g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)];

  ep_cbl_indx = p_ep_state->ep_cbl_indx;

  /* Check for valid bdt index */
  if ( ep_cbl_indx != INVALID_BDT_INDEX )
  {
    ENDPT0STR *endpoint = (ENDPT0STR *)(&USB0_ENDPT0 + (4 * ep_num));

    /* Enable ENDPTx for non control endpoints */
    /* For Control Endpoint the default Value 0x0D */
    if ( ep_num != CONTROL_ENDPOINT )
    {
      INT8U endpt;
      /* Enable handshaking for non isochronous endpoint */
      endpt = (INT8U)((p_ep_state->type != USB_ISOCHRONOUS_PIPE) ? HSHK_EN : 0);
      /*
         Enable the endpoint in the specified direction and disable
         control tranfers (valid only in case the endpoint is
         bidirectional)
      */
      endpt |= (INT8U)(EP_CTL_DIS | ((direction == USB_SEND) ? EP_IN : EP_OUT));
      endpoint->Byte |= endpt;
    }
    /* Clear Endpoint Stall bit is endpoint control register */
    endpoint->Bits.EP_STALL = 0;

    /* Unstall endpoint by clearing stall bit in BDT  */
    g_BDT->ep_cbl[ep_cbl_indx].bits.all &= ~(_OWN | _BDTSTALL);
    /* We Require DATA0 PID to be zero on unstall */
    g_BDT->ep_cbl[ep_cbl_indx].bits.all = _DATA0;
    if ( direction == USB_RECV )
    {
      /* Initiate Next receive Transfer */
      USB_DCI_Recv_Data(controller_ID, ep_num, NULL, 0);
    }
  }
  return;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Get_Setup_Data
  *
  * @brief The function copies Setup Packet from USB RAM to application buffer
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param buff_ptr      : Application buffer pointer
  *
  * @return None
  *
  ******************************************************************************
  * Copies setup packet from USB RAM to Application Buffer
  *****************************************************************************/
void USB_DCI_Get_Setup_Data(
                            INT8U      controller_ID,  /* [IN] Controller ID */
                            INT8U      ep_num,         /* [IN] Endpoint number */
                            INT8U_ptr  buff_ptr        /* [IN] Application buffer pointer */
                            )
{


  INT8U_ptr addr;
  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, USB_RECV, USB_RAM_EVEN_BUFFER);

  T_EP_STATE *p_ep_state = &g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)];
  ep_cbl_indx = p_ep_state->ep_cbl_indx;

  /* address correponding to the endpoint */
  addr = (INT8U_ptr)g_BDT->ep_cbl[ep_cbl_indx].addr;

  /* copy bdt buffer to application buffer */
  (void)memcpy(buff_ptr, addr, USB_SETUP_PKT_SIZE);
  return;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Get_Transfer_Status
  *
  * @brief The function retrieves the Transfer status of an endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param direction     : Endpoint direction
  *
  * @return status
  *         USBERR_TR_FAILED                : When unsuccessfull
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
INT8U USB_DCI_Get_Transfer_Status(
                                  INT8U    controller_ID,   /* [IN] Controller ID */
                                  INT8U    ep_num,          /* [IN] Endpoint number */
                                  INT8U    direction        /* [IN] Endpoint direction */
                                  )
{

  INT8U status = USB_STATUS_DISABLED;

  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, direction, USB_RAM_EVEN_BUFFER);

  /* Check for valid bdt index */
  if ( ep_cbl_indx != INVALID_BDT_INDEX )
  {
    INT8U ep_control = (INT8U)(*((&USB0_ENDPT0) + 4 * ep_num));

    status = USB_STATUS_IDLE;

    /* Check for direction in endpoint control register */
    if ( (ep_control & (EP_IN | EP_OUT)) == EP_DISABLE )
    {
      status = USB_STATUS_DISABLED;
    }
    /* Check for stall bit in endpoint control register */
    else if ( (ep_control &  EPCTL_STALL) == EPCTL_STALL )
    {
      status = USB_STATUS_STALLED;
    }
    /* Check whether SIE has control of BDT */
    else if ( (g_BDT->ep_cbl[ep_cbl_indx].bits.cbits.own == 1) || (g_BDT->ep_cbl[ep_cbl_indx ^ 1].bits.cbits.own == 1) )
    {
      status = USB_STATUS_TRANSFER_IN_PROGRESS;
    }
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Recv_Data
  *
  * @brief The function retrieves data recieved on an RECV endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param buff_ptr      : Application buffer pointer
  * @param size          : Size of the buffer
  *
  * @return status
  *         USB_OK                          : When successfull
  *         USBERR_RX_FAILED                : When unsuccessfull
  ******************************************************************************
  * This function retrieves data received data on a RECV endpoint by copying it
  * from USB RAM to application buffer
  *****************************************************************************/
INT8U USB_DCI_Recv_Data(INT8U controller_ID,INT8U ep_num,INT8U_ptr buff_ptr,INT16U size)
{

  INT8U status = USBERR_RX_FAILED;
  INT8U ep_cbl_indx;
  T_EP_STATE *p_ep_state;
  //uint_8_ptr addr = NULL;

  ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, USB_RECV, USB_RAM_EVEN_BUFFER);
  p_ep_state  = &g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)];
  ep_cbl_indx = p_ep_state->ep_cbl_indx;

  if ( ep_cbl_indx != INVALID_BDT_INDEX )
  {
    T_EP_CBL *p_ep_cbl     = &g_BDT->ep_cbl[ep_cbl_indx ^ 1];
    T_EP_CBL *p_ep_cbl_alt = NULL;


    if ( p_ep_state->len != (INT16U)UNINITIALISED_VAL )
    {
      // Обрабатываем пакет только если контрольный блок конечной точки разблокирован движком USB
      if ( p_ep_cbl->bits.cbits.own == 0 )
      {
        if ( size == 0 )
        {
          /* Give control to the other buffer to recv the next packet  */
          p_ep_cbl_alt = &g_BDT->ep_cbl[ep_cbl_indx];
          p_ep_cbl_alt->cnt = p_ep_state->len;
          p_ep_cbl_alt->addr = p_ep_state->addr;

          /* Give the ownership to SIE and TOGGLE DATA BIT */
          p_ep_cbl_alt->bits.all = (INT8U)((p_ep_cbl_alt->bits.cbits.data << 6) | _OWN | _DTS);
          return USB_OK;
        }

        /* adjust size based on the input at the init endpoint */
#ifdef LONG_RECIEVE_TRANSACTION
        /* Initialise transfer */
        p_ep_state->app_len = size;
        p_ep_state->app_buffer = buff_ptr;
#endif
        if ( size > p_ep_state->len )
        {
          size = p_ep_state->len;
        }

#ifdef LONG_RECIEVE_TRANSACTION
        p_ep_state->curr_offset = 0;
#endif
        p_ep_cbl_alt           = &g_BDT->ep_cbl[ep_cbl_indx];
        p_ep_cbl_alt->cnt      = size;
        p_ep_cbl_alt->addr     = (INT32U)buff_ptr;
        p_ep_cbl_alt->bits.all = (INT8U)((p_ep_cbl_alt->bits.cbits.data << 6) | _OWN | _DTS);
        status = USB_OK;
      }
    }
  }
  return status;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Send_Data
  *
  * @brief The function configures Controller to send data on an SEND endpoint
  *
  * @param controller_ID : Controller ID
  * @param ep_num        : Endpoint number
  * @param buff_ptr      : Application buffer pointer
  * @param size          : Size of the buffer
  *
  * @return status
  *         USB_OK           : When successfull
  *         USBERR_TX_FAILED : When unsuccessfull
  ******************************************************************************
  * This function configures Controller to send data on an SEND endpoint by
  * setting the BDT to send data.
  *****************************************************************************/
INT8U USB_DCI_Send_Data(
                        INT8U          controller_ID,  /* [IN] Controller ID */
                        INT8U          ep_num,         /* [IN] Endpoint number */
                        INT8U_ptr       buff_ptr,       /* [IN] Application buffer pointer */
                        INT16U size            /* [IN] Size of the buffer */
                        )
{

  INT8U       status = USBERR_TX_FAILED;
  T_EP_CBL   *p_ep_cbl;

  //Led_control(3, 1);

  /* validate params and get the bdt index */
  INT8U ep_cbl_indx = USB_DCI_Get_EP_CBL_Valid_Index(ep_num, USB_SEND, USB_RAM_EVEN_BUFFER);

  T_EP_STATE *p_ep_state = &g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)];

  if ( ep_cbl_indx == INVALID_BDT_INDEX ) return USBERR_TX_FAILED;

  /* Send Data After Toggling Buffer */
  ep_cbl_indx = (INT8U)p_ep_state->ep_cbl_indx;

  p_ep_cbl = &g_BDT->ep_cbl[ep_cbl_indx];
  /* Does MCU owns it and it is not stalled */
  if ( !((p_ep_cbl->bits.cbits.own) || (*(&USB0_ENDPT0 + (ep_num * 4)) & ENDPT_EP_STALL_MASK)) )
  {
    /* Now configure buffer_dsc->addr and buffer_dsc->cnt */
#ifdef LONG_SEND_TRANSACTION
    /* Initialise transfer */
    p_ep_state->app_len = size;
    p_ep_state->app_buffer = buff_ptr;
    p_ep_state->curr_offset = 0;

    /* prepare for send */
    USB_DCI_Prepare_Send_Data(p_ep_cbl, p_ep_state);
#else
    p_ep_cbl->addr = (INT32U)buff_ptr;

    /* adjust size based on the input at the init endpoint */
    if ( (INT16U)size > p_ep_state->len )
    {
      p_ep_cbl->cnt = p_ep_state->len;
    }
    else
    {
      p_ep_cbl->cnt = (INT16U)size;
    }
#endif
    if ( ep_num == CONTROL_ENDPOINT )
    {
      /* Set up the control endpoint bdt for next packet */
      p_ep_cbl->bits.all = (_OWN | _DATA1 | _DTS);
    }
    else
    {
      p_ep_cbl->bits.all = (INT8U)((p_ep_cbl->bits.cbits.data << 6) | _OWN | _DTS);
    }

    status = USB_OK;
  } /* Does MCU own IN BDT */

  //Led_control(3, 0);

  return status;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Set_Address
  *
  * @brief The function configures Controller to send data on an SEND endpoint
  *
  * @param controller_ID : Controller ID
  * @param address       : Controller Address
  *
  * @return None
  *
  ******************************************************************************
  * Assigns the Address to the Controller
  *****************************************************************************/
void  USB_DCI_Set_Address(
                          INT8U    controller_ID,    /* [IN] Controller ID */
                          INT8U    address           /* [IN] Controller Address */
                          )
{

  /* set the address */
  USB0_ADDR = address;

  USB_Device_Set_Status(g_dci_controller_Id, USB_STATUS_DEVICE_STATE, USB_STATE_ADDRESS);
  return;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Shutdown
  *
  * @brief The function shuts down the controller
  *
  * @param controller_ID : Controller ID
  *
  * @return None
  *
  ******************************************************************************
  * Resets USB Device Controller
  *****************************************************************************/
void USB_DCI_Shutdown(
                      INT8U    controller_ID     /* [IN] Controller ID */
                      )
{

  /* Reset the Control Register */
  USB0_CTL = 0;
  /* Initiate Reset in the USB Control0 Register */
#ifndef OTG_BUILD

  USB0_USBTRC0 = _USBRESET;
#endif

  USB_Device_Set_Status(g_dci_controller_Id, USB_STATUS_DEVICE_STATE, USB_STATE_UNKNOWN);
  return;
}

/**************************************************************************/ /*!
  *
  * @name  USB_DCI_Assert_Resume
  *
  * @brief The function makes the Controller start USB RESUME signaling
  *
  * @param controller_ID: Controller ID
  *
  * @return None
  *
  ******************************************************************************
  *
  * This function starts RESUME signalling and then stops it after some delay.
  * In this delay make sure that COP is reset.
  *
  *****************************************************************************/
void USB_DCI_Assert_Resume(
                           INT8U    controller_ID     /* [IN] Controller ID */
                           )
{

  INT16U delay_count;

#ifdef USB_LOWPOWERMODE
  /* Clear SUSP Bit from USB_CTRL */
  USB_CTRL_SUSP = 0;
#endif

  /* Reset Low Power RESUME enable */
  USB0_USBTRC0 &= ~USB_USBTRC0_USBRESMEN_MASK;

  USB_DCI_WAKEUP

    USB0_CTL |= USB_CTL_RESUME_MASK;   /* Start RESUME signaling and make SUSPEND bit 0*/

  delay_count = ASSERT_RESUME_DELAY_COUNT; /* Set RESUME line for 1-15 ms*/

  do
  {
    delay_count--;
    Watchdog_refresh();    /* Reset the COP */
  }
  while (delay_count);

  USB0_CTL &= ~USB_CTL_RESUME_MASK;          /* Stop RESUME signalling */

  return;
}

//-------------------------------------------------------------------------------------------------------
//  Функция обрабатывающая из прерываний событие окончания приема или передачи пакета
//
//  usb_stat - содержимое регистра USB0_STAT
//  msg      - указатель на структуру с данными о полученном или тправленном пакете. Данная структура модифицируется в этой функции
//-------------------------------------------------------------------------------------------------------
void USB_Packet_RXTX_Handler(INT8U usb_stat,USB_DEV_EVENT_STRUCT *msg)
{
  INT8U          ep_cbl_indx   = 0;
  T_EP_CBL       *p_ep_cbl     = NULL;
  T_EP_CBL       *p_ep_cbl_alt = NULL;
  T_EP_STATE     *p_ep_state   = NULL;
  BOOLEAN        odd = (BOOLEAN)((usb_stat & 0x04) >> 2);


  odd = (usb_stat >> 2) & 1;                    // Определяем четный или не четный управляющий блок был использован. 1 означает нечетный
  msg->ep_num = (usb_stat >> 4) & 0x0F;         // Узнаем номер конечной точки
  msg->direction = (usb_stat >> 3) & 1;         // Читаем бит TX(3) в регистре USB->STAT. 1 - означает, что данные были переданы, 0 - приняты

  ep_cbl_indx = USB_DCI_Get_EP_CBL_Index(msg->ep_num, msg->direction, odd); // Вычисляем индекс управляющего блока конечной точки

  p_ep_cbl     = &g_BDT->ep_cbl[ep_cbl_indx];    // Получаем указатель на управляющий блок конечной точки
  p_ep_cbl_alt = &g_BDT->ep_cbl[ep_cbl_indx ^ 1];// Получаем указатель на следующий управляющий блок этой же конечной точки


  // Перехватываем транзакцию SETUP с командой DFU_DNLOAD
  //if ( p_ep_cbl->bits.pack_pid.pid == 0x0D )
  //{
  //  
  //  if (( ((INT8U*)(p_ep_cbl->addr))[0]==0x21 ) && ( ((INT8U*)(p_ep_cbl->addr))[1]==0x01 ))
  //  {
  //    __no_operation();
  //  }
  //}

  // Перехватываем транзакцию OUT с данными для программирования и первым байтом равным 0xFF
  //if ( p_ep_cbl->bits.pack_pid.pid == 0x01 )
  //{
  //  
  //  if ( ((INT8U*)(p_ep_cbl->addr))[0]==0xFF )
  //  {
  //    __no_operation();
  //  }
  //}



  p_ep_state = &g_ep_state_tbl[TRANSFER_INDEX(ep_cbl_indx)];// Получаем указатель на структуру параметров конечной точки

  msg->buffer_ptr = (INT8U_ptr)p_ep_cbl->addr;  // В сообщение записываем адрес и длину пакета
  msg->len = p_ep_cbl->cnt;

  p_ep_state->ep_cbl_indx = (INT8U)(p_ep_state->ep_cbl_indx ^ 1);// В структуре параметров конечной точки обновляем индекс на следующий управляющий блок 
                                                                 
  p_ep_cbl_alt->bits.all = (INT8U)((p_ep_cbl->bits.cbits.data ^ 1) << 6); // Переключаем бит data для следующей транзакции этой конечной точки

  if ( msg->direction == USB_SEND )
  {

    // Здесь если была завершена транзакция отправки данных хосту IN .................................................

    if ( msg->ep_num == CONTROL_ENDPOINT )
    {
      /* for Control Endpoint */
      /* For Transfer Direction Host to Device */
      if ( g_setup_direction == USB_RECV )
      {
        // Здесь оказываемся если последняя транзакция SETUP что-то записывала в устройство и устройство это подтвердило выдав транзакцию IN 
        // Это могут быть запросы SetAddress, SetConfiguration ...

        g_setup_direction = USB_TRF_UNKNOWN;
        USB_DCI_Cancel_Transfer(msg->controller_ID, CONTROL_ENDPOINT, USB_RECV); // Прекратить обработку транзакций конечной точкой  
        p_ep_cbl_alt->bits.all = _DATA0;                                         // Подготовить конечную точку к началу обработки новой передачи
        USB_DCI_Recv_Data(msg->controller_ID, CONTROL_ENDPOINT, NULL, 0);

      }
    } /* ep_num is CONTROL ENDPOINT */

#ifdef LONG_SEND_TRANSACTION
    if ( (g_setup_direction == USB_SEND) || (msg->ep_num != CONTROL_ENDPOINT) )
    {
      /* update the request */
      p_ep_state->curr_offset += msg->len;
      /*
         Initiate next USB SEND if:
         1. More Data is still pending OR
         2. Send Data == Endpoint Size AND
         3. Zero Termination Flag is TRUE
      */
      if ( (p_ep_state->app_len > p_ep_state->curr_offset) || ((INT8U)msg->len == p_ep_state->len) && (p_ep_state->flag == TRUE) )
      {
        /* send next Req */
        USB_DCI_Prepare_Send_Data(p_ep_cbl_alt, p_ep_state);

        /* give the ownership to SIE and TOGGLE DATA BIT */
        p_ep_cbl_alt->bits.all = (INT8U)(((p_ep_cbl_alt->bits.cbits.data) << 6) | _OWN | _DTS);
        ;
        return;
      }
      else
      {
        msg->buffer_ptr = p_ep_state->app_buffer;
        msg->len = p_ep_state->curr_offset;
      }
    }
#endif
  }
  else
  {
    // Здесь если была завершена транзакция приема данных OUT или SETUP .................................................

    if ( msg->ep_num == CONTROL_ENDPOINT )
    {
      /* for Control Endpoint */
      if ( p_ep_cbl->bits.pack_pid.pid == USB_SETUP_TOKEN )
      {
        // Обработана транзакция SETUP        
        msg->setup = TRUE;
        g_setup_direction = (INT8U)((INT8U)(msg->buffer_ptr[0]) >> 7);
      }
      else if ( g_setup_direction == USB_SEND )
      {
        // Мы здесь когда конечная приняла данные в запросе от хоста на чтение данных
        // Это прием конечной транзакции OUT, подтверждающей окончание процесса обработки запроса чтения данных хостом  
        g_setup_direction = USB_TRF_UNKNOWN;
        USB_DCI_Cancel_Transfer(msg->controller_ID, CONTROL_ENDPOINT, USB_SEND);
        p_ep_cbl_alt->bits.all = _DATA0;
        USB_DCI_Recv_Data(msg->controller_ID, CONTROL_ENDPOINT, NULL, 0);
      }
    }   

#ifdef LONG_RECIEVE_TRANSACTION
    /* For NON CONTROL ENDPOINT only */
    if ( p_ep_state->app_len != (INT16U)UNINITIALISED_VAL )
    {
      /* on control endpoint the data is only 8 bytes */
      INT16U size = msg->len;
      p_ep_state->curr_offset += size;

      /*
         Initiate next USB RECV if:
         1. More Data is still pending OR
         2. Received Data == Endpoint Size AND
         3. Zero Termination Flag is TRUE
      */
      if ( ((size == p_ep_state->len) && (p_ep_state->app_len > p_ep_state->curr_offset)) || 
           ((p_ep_state->app_len) && (!(p_ep_state->app_len % p_ep_state->len)) && (p_ep_state->flag == TRUE)) )
      {
        /* send next IO */
        INT16U cnt;
        if ( (p_ep_state->app_len - p_ep_state->curr_offset) > p_ep_state->len )
        {
          cnt = p_ep_state->len; 
        }
        else
        {
          cnt = p_ep_state->app_len - p_ep_state->curr_offset;
        }
        
        if ( cnt == 0 )
        {
          /* For Zero byte Packet Receive */
          p_ep_cbl_alt->addr = p_ep_state->addr;
          p_ep_cbl_alt->cnt = 0;
        }
        else
        {
          p_ep_cbl_alt->addr = (INT32U) & p_ep_state->app_buffer[p_ep_state->curr_offset];
          p_ep_cbl_alt->cnt = cnt;
        }

        /* give the ownership to SIE and Toggle DATA bit*/
        p_ep_cbl_alt->bits.all = (INT8U)(((p_ep_cbl_alt->bits.cbits.data) << 6) | _OWN | _DTS);
        return;
      }
      else /* request completed */
      {
        /* populate buffer structure */
        msg->buffer_ptr = p_ep_state->app_buffer;
        msg->len = p_ep_state->curr_offset;
        p_ep_state->app_len = (INT16U)UNINITIALISED_VAL;
      }
    }
#endif
  } /* End of RECV loop */

  /* Notify Device Layer of Data Recieved or Sent Event */
  (void)USB_Device_Call_Service(msg->ep_num, msg);

  return;
}
/**************************************************************************/ /*!
  *
  * @name  USB_ISR
  *
  * @brief The function handles USB interrupts on the bus.
  *
  * @param None
  *
  * @return None
  *
  ******************************************************************************
  * This function is hooked onto interrupt 69 and handles the USB interrupts.
  * After handling the interrupt it calls the Device Layer to notify it about
  * the event.
  *****************************************************************************/

void USB_OTG_IRQHandler(void)
{

  /* Which interrupt occured and also was enabled */
  INT8U                  intr_stat = (INT8U)(USB0_ISTAT & USB0_INTEN);
  INT8U                  usb_stat      = (INT8U)USB0_STAT;
  USB_DEV_EVENT_STRUCT   msg;
  INT16U                 dev_state = USB_STATUS_UNKNOWN;


  /* initialise msg structure */
  msg.controller_ID = g_dci_controller_Id;
  msg.setup         = FALSE;
  msg.buffer_ptr    = NULL;
  msg.len           = 0;
  msg.direction     = USB_RECV;
  msg.errors        = NO_ERRORS;

  msg.ep_num = (INT8U)UNINITIALISED_VAL;

  /* Get the device state from the Device Layer */
  (void)USB_Device_Get_Status(g_dci_controller_Id, USB_STATUS_DEVICE_STATE, &dev_state);

  /* if current devie state is SUSPEND and Low Powere Resume Flag set */
  if ( (USB0_USBTRC0 & USB_USBTRC0_USB_RESUME_INT_MASK) && (dev_state == USB_STATE_SUSPEND) )
  {
#ifdef USB_LOWPOWERMODE
    /* Clear SUSP Bit from USB_CTRL */
    USB_CTRL_SUSP = 0;
#endif
    /* Reset Low Power RESUME enable */
    USB0_USBTRC0 &= ~USB_USBTRC0_USBRESMEN_MASK;
  }

  if ( SOF_TOKEN_FLAG(intr_stat) )
  {
    INT16U sof_count;

    /* Clear SOF Interrupt */
    USB0_ISTAT = USB_ISTAT_SOFTOK_MASK;
    sof_count = (INT16U)((USB0_FRMNUMH << FRAME_HIGH_BYTE_SHIFT) | USB0_FRMNUML);
    /*address of Lower byte of Frame number*/
    msg.buffer_ptr = (INT8U_ptr)(&sof_count);
    /* Notify Device Layer of SOF Event */
    (void)USB_Device_Call_Service(USB_SERVICE_SOF, &msg);
  }

  if ( BUS_RESET_FLAG(intr_stat) )
  {

    /* Clear Reset Flag */
    USB0_ISTAT = USB_ISTAT_USBRST_MASK;

    /* Handle RESET Interrupt */
    USB_Bus_Reset_Handler();

    /* Notify Device Layer of RESET Event */
    (void)USB_Device_Call_Service(USB_SERVICE_BUS_RESET, &msg);

    /* Clearing this bit allows the SIE to continue token processing
       and clear suspend condition */
    USB0_CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;

    /* No need to process other interrupts */
    return;
  }

  if ( TOKEN_COMPL_FLAG(intr_stat) )
  {
    // Реакция на прерывание по биту TOKDNE

    /* Clear TOKEN Interrupt */
    USB0_ISTAT = USB_ISTAT_TOKDNE_MASK;

    USB_Packet_RXTX_Handler(usb_stat, &msg);

    /* Clearing this bit allows the SIE to continue token processing and clear suspend condition */
    USB0_CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
  }

  if ( ERROR_FLAG(intr_stat) )
  {

    /* Clear ERROR Interrupt */
    USB0_ISTAT = USB_ISTAT_ERROR_MASK;

    msg.errors = (INT8U)(USB0_ERRSTAT & USB0_ERREN);

    /* Notify Device Layer of ERROR Event to error service */
    (void)USB_Device_Call_Service(USB_SERVICE_ERROR, &msg);

    USB0_ERRSTAT = ERR_STAT_CLEAR_ALL;  /*clear all errors*/
    /* Clearing this bit allows the SIE to continue token processing
       and clear suspend condition */
    USB0_CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
  }

  if ( SLEEP_FLAG(intr_stat) )
  {
    // This bit is set when the USB Module detects a constant idle on the USB bus for 3 milliseconds.
    // The sleep timer is reset by activity on the USB bus.


    /* Clear RESUME Interrupt if Pending */
    USB0_ISTAT = USB_ISTAT_RESUME_MASK;

    /* Clear SLEEP Interrupt */
    USB0_ISTAT = USB_ISTAT_SLEEP_MASK;

    /* Notify Device Layer of SLEEP Event */
    (void)USB_Device_Call_Service(USB_SERVICE_SLEEP, &msg);

    /* Set Low Power RESUME enable */
    USB0_USBTRC0 |= USB_USBTRC0_USBRESMEN_MASK;

#ifdef USB_LOWPOWERMODE
    /* Set SUSP Bit in USB_CTRL */
    USB_CTRL_SUSP = 1;
#endif
    /* Enable RESUME Interrupt */
    USB0_INTEN |= USB_INTEN_RESUMEEN_MASK;
#ifdef USB_LOWPOWERMODE
    /* Enter Stop3 Mode*/
    Enter_StopMode(STOP_MODE3);
#endif
  }

  if ( RESUME_FLAG(intr_stat) )
  {
    // This bit is set depending upon the DP/DM signals, and can be used to signal remote wake-up signaling on  the USB bus.
    // When not in suspend mode this interrupt should be disabled.

    /* Clear RESUME Interrupt */
    USB0_ISTAT = USB_ISTAT_RESUME_MASK;

    /* Notify Device Layer of RESUME Event */
    (void)USB_Device_Call_Service(USB_SERVICE_RESUME, &msg);

    /* Disable RESUME Interrupt */
    USB0_INTEN &= ~USB_INTEN_RESUMEEN_MASK;
  }

  if ( STALL_FLAG(intr_stat) )
  {
    INT16U endp_status;


    msg.ep_num = (INT8U)UNINITIALISED_VAL;

    /* If Control Endpoint is stalled then unstall it.
    For other endpoints host issues clear endpoint feature request
    to unstall them */

    /* Get Control Endpoint Status*/
    (void)USB_Device_Get_Status(msg.controller_ID, (USB_STATUS_ENDPOINT | CONTROL_ENDPOINT), &endp_status);
    if ( endp_status == USB_STATUS_STALLED )
    {
      msg.ep_num = CONTROL_ENDPOINT;
      msg.direction = USB_SEND;
    }

    /* Clear STALL Interrupt */
    USB0_ISTAT = USB_ISTAT_STALL_MASK;

    /* Notify Device Layer of STALL Event */
    (void)USB_Device_Call_Service(USB_SERVICE_STALL, &msg);

    /* Clearing this bit allows the SIE to continue token processing
       and clear suspend condition */
    USB0_CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
  }

  return;
}

/**************************************************************************/ /*!
  *
  * @name  Clear_Mem
  *
  * @brief The function clears memory starting from start_addr till count bytes
  *
  * @param start_addr : Buffer Start address
  * @param count      : Count of Bytes
  * @param val        : Value to be set
  *
  * @return None
  ******************************************************************************
  * This function is an implementation of memset
  *****************************************************************************/
void Clear_Mem(
               INT8U_ptr start_addr,  /* [OUT] Buffer Start address */
               INT32U count,          /* [IN] Count of Bytes */
               INT8U val              /* [IN] Value to be set */
               )
{
  (void)memset(start_addr, val, count);
  return;
}

#ifdef USB_LOWPOWERMODE
/**************************************************************************/ /*!
  *
  * @name  Enter_StopMode
  *
  * @brief The function configures STOP Mode
  *
  * @param stop_mode : STOP MODE to be entered
  *
  * @return None
  ******************************************************************************
  * This function configures different STOP MODES defined by the controller.
  * Used to put controller into low power mode. Only STOP MODE 3 is implemented
  *****************************************************************************/
static void Enter_StopMode(STOP_MODE stop_mode)
{
  switch (stop_mode)
  {
  case STOP_MODE1:
    /*
       We enter Default Stop Mode
    */
    break;
  case STOP_MODE2:
    /* Save IO Pin Status in a global variable
        IO Pin Status is to be restored at POR.
        Check if PPDC
    */
    /* Set PPDC */
    break;
  case STOP_MODE3:
    /* Clear PPDC */
    SPMSC2_PPDC = 0;
    /* Disable Low Voltage Detect */
    SPMSC1_LVDSE = 0;
    break;
  case STOP_MODE4:
    break;
  }
  /* Enter STOP Mode*/
  _Stop;
}

#endif
