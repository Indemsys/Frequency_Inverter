/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: flexcan.c$
* $Version : 3.8.25.0$
* $Date    : Jun-6-2012$
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <flexcan.h>

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Start  
* Returned Value   : uint_32 
* Comments         :
*    This function starts the specified FlexCAN device
*
*END*********************************************************************/
uint_32 FLEXCAN_Start
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* Starting FLEXCAN in normal mode */
  can_reg_ptr->CANMCR &= ~(FLEXCAN_CANMCR_HALT | FLEXCAN_CANMCR_MDIS
                           | FLEXCAN_CANMCR_FRZ);

  /* wait for synchronization */
  while (can_reg_ptr->CANMCR & FLEXCAN_CANMCR_FRZACK)
  {}

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Softreset
* Returned Value   : uint_32 
* Comments         :
*    This function resets the specific CAN device
*
*END*********************************************************************/
uint_32 FLEXCAN_Softreset
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num
 )
{ /* Body */

  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* check for low power mode */
  if ( can_reg_ptr->CANMCR & FLEXCAN_CANMCR_LPMACK )
  {
    /* Enable clock */
    can_reg_ptr->CANMCR &= (~FLEXCAN_CANMCR_MDIS);
    /* wait until enabled */
    while (can_reg_ptr->CANMCR & FLEXCAN_CANMCR_LPMACK)
    {}
  }

  /* Reset the FLEXCAN */
  can_reg_ptr->CANMCR = FLEXCAN_CANMCR_SOFTRST;

  /* Wait for reset cycle to complete */
  while (can_reg_ptr->CANMCR & FLEXCAN_CANMCR_SOFTRST)
  {}

  /* Set Freeze, Halt */
  can_reg_ptr->CANMCR |= (FLEXCAN_CANMCR_FRZ | FLEXCAN_CANMCR_HALT);

  /* check for freeze Ack */
  if (
       ((can_reg_ptr->CANMCR & FLEXCAN_CANMCR_FRZACK) != FLEXCAN_CANMCR_FRZACK) ||
       ((can_reg_ptr->CANMCR & FLEXCAN_CANMCR_NOTRDY) != FLEXCAN_CANMCR_NOTRDY)
       ) return (FLEXCAN_SOFTRESET_FAILED);

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Get_msg_object  
* Returned Value   : pointer 
* Comments         :
*    This function returns a pointer to the mailbox object specified
*    by the mailbox number.
*
*END*********************************************************************/
pointer FLEXCAN_Get_msg_object
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number
 )
{ /* Body */

  volatile uchar_ptr temp_ptr;
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (NULL);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (NULL);
  }

  temp_ptr = (uchar_ptr)can_reg_ptr;
  temp_ptr += FLEXCAN_MSG_BUFADDR_OFFSET;


  /*
  ** Calculate what message object to point to
  */
  temp_ptr += sizeof(FLEXCAN_MSG_OBJECT_STRUCT) * mailbox_number;

  return ((pointer)temp_ptr);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Select_mode  
* Returned Value   : uint_32 
* Comments         :
*    This function sets FlexCAN module operation mode
*
*END*********************************************************************/
uint_32 FLEXCAN_Select_mode
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] operation Mode */
 uint_32 mode
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR can_reg_ptr;
  volatile uint_32                ret_code = FLEXCAN_OK;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  switch (mode)
  {
  case (FLEXCAN_NORMAL_MODE):
    /* 
    ** Normal mode, check freeze ACK bit
    ** call start if bit is set 
    */
    if ( (can_reg_ptr->CANMCR & FLEXCAN_CANMCR_FRZACK) == FLEXCAN_CANMCR_FRZACK )
    {
      if ( FLEXCAN_Start(dev_num) ) ret_code = FLEXCAN_START_FAILED;
    }
    break;
  case (FLEXCAN_LISTEN_MODE):
    /* Set CANCTRL[LOM], listen-only mode */
    can_reg_ptr->CANCTRL |= FLEXCAN_CANCTRL_LOM;
    break;
  case (FLEXCAN_TIMESYNC_MODE):
    /* Timer Synchronization mode (Global Network Time) */
    can_reg_ptr->CANCTRL |= FLEXCAN_CANCTRL_TSYNC;
    break;
  case (FLEXCAN_LOOPBK_MODE):
    /* Self test mode, Loop Back */
    can_reg_ptr->CANCTRL |= FLEXCAN_CANCTRL_LPB;
    break;
  case (FLEXCAN_BOFFREC_MODE):
    /* Bus Off Recovery mode (according to CAN 2.0b) */
    can_reg_ptr->CANCTRL &= ~(FLEXCAN_CANCTRL_BOFFREC);
    break;
  case (FLEXCAN_FREEZE_MODE):
    /* Debug mode, Halt and Freeze */
    can_reg_ptr->CANMCR |= (FLEXCAN_CANMCR_FRZ | FLEXCAN_CANMCR_HALT);
    /* check for freeze Ack */
    if ( (can_reg_ptr->CANMCR & FLEXCAN_CANMCR_FRZACK) != FLEXCAN_CANMCR_FRZACK ) ret_code = FLEXCAN_FREEZE_FAILED;
    break;
  case (FLEXCAN_DISABLE_MODE):
    /* Debug mode, Halt and Freeze */
    can_reg_ptr->CANMCR |= FLEXCAN_CANMCR_MDIS;
    break;
  default:
    ret_code = FLEXCAN_INVALID_MODE;
  }

  return (ret_code);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Select_CLK  
* Returned Value   : uint_32 
* Comments         :
*    This function sets FlexCAN module operation mode
*
*END*********************************************************************/
uint_32 FLEXCAN_Select_clk
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] FlexCAN clock source */
 uint_32 clk
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( clk == FLEXCAN_IPBUS_CLK )
    /* CANPE Clock */
    can_reg_ptr->CANCTRL |= FLEXCAN_CANCTRL_CLK_SRC;

  else if ( clk == FLEXCAN_OSC_CLK )
    /* External clock */
    can_reg_ptr->CANCTRL &= (~FLEXCAN_CANCTRL_CLK_SRC);

  else return (FLEXCAN_CLOCK_SOURCE_INVALID);


  return (FLEXCAN_OK);
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Initialize  
* Returned Value   : uint_32 
* Comments         :
*    This function initializes all mailbox-independent registers of the 
*    FLEXCAN chip.
*    NOTE:
*         TimeSeg1 = PROP_SEG + PHASE_SEG1
*         TimeSeg2 = PHASE_SEG2
*       Ex. PHASE_SEG1 = 0xF and PROP_SEG = 0x7 => bit_timing0 = 0x000F0007
*
*END*********************************************************************/
uint_32 FLEXCAN_Initialize
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] MS 16 bits = TimeSeg1, LS 16 bits = TimeSeg2 */
 uint_32 bit_timing0,
 /* [IN] MS 16 bits = pre-scaler value, LS 16 bits = SJW */
 uint_32 bit_timing1,
 /* [IN] Bit rate in Kbps */
 uint_32 frequency,
 /* [IN] clock frequency (needs to match either FLEXCAN_IPBUS_CLK or FLEXCAN_OSC_CLK */
 uint_32 clk
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR can_bufstruct_ptr;
  volatile uchar_ptr                     temp_ptr;
  vuint_32                               PSEG1 = 0;
  vuint_32                               PSEG2 = 0;
  vuint_32                               PROPSEG = 0;
  vuint_32                               RJW = 0;
  vuint_32                               PRESDIV = 0;
  uint_32                                i;
  uint_32                                result;

#if PSP_HAS_DEVICE_PROTECTION
  if ( !_bsp_flexcan_enable_access(dev_num) )
  {
    return FLEXCAN_INVALID_DEVICE;
  }
#endif

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  temp_ptr = (uchar_ptr)can_reg_ptr;
  temp_ptr += FLEXCAN_MSG_BUFADDR_OFFSET;

  can_bufstruct_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)temp_ptr;

  /* 
  ** To access the memory mapped registers 
  ** Enter disable mode (hard reset).
  */
  if ( (can_reg_ptr->CANMCR & FLEXCAN_CANMCR_MDIS) == 0x00000000 )
  {
    /* clock disable (module) */
    can_reg_ptr->CANMCR = FLEXCAN_CANMCR_MDIS;

    /* wait until disable mode acknowledged */
    while (!can_reg_ptr->CANMCR & FLEXCAN_CANMCR_LPMACK)
    {}
  }

  /* Reset FLEXCAN, Halt, freeze mode */
  result = FLEXCAN_Softreset(dev_num);
  if ( FLEXCAN_OK != result )
  {
    return (result);
  }

  /* 
  ** Select the clock source 
  ** Default: internal bus clock (fsys/2).
  */
  result = FLEXCAN_Select_clk(dev_num, clk);
  if ( FLEXCAN_OK != result )
  {
    return (result);
  }

  frequency *= 1000;

  /* bit timing settings */
  if ( (bit_timing0 == 0x00) && (bit_timing1 == 0x00) )
  {
    // If BSP_CANPE_CLOCK is defined, then we will calculate the CAN bit timing parameters
    // using the method outlined in AN1798, section 4.1. A maximum time for PROP_SEG will be used,
    // the remaining TQ will be split equally between PSEG1 and PSEG2, provided PSEG2 >=2. RJW is
    // set to the minimum of 4 or PSEG1.

    uint_32 TQ_x_Prescaler = clk / frequency;
    uint_32 TQ, lowest_diff, diff, best_TQ, actual_freq;

    if ( TQ_x_Prescaler < (CAN_MIN_TQ - 1) )
    {
      // We will be off by more than 12.5%
      return FLEXCAN_INVALID_FREQUENCY;
    }

    // First, find the best TQ and pre-scaler to use for the desired frequency. If any exact matches
    // is found, we use the match that gives us the lowest pre-scaler and highest TQ, otherwise we pick
    // the TQ and prescaler that generates the closest frequency to the desired frequency.

    lowest_diff = frequency;
    best_TQ = 0;
    for (TQ = CAN_MAX_TQ; TQ >= CAN_MIN_TQ; TQ--)
    {
      PRESDIV = TQ_x_Prescaler / TQ;
      if ( PRESDIV <= 256 )
      {
        if ( TQ_x_Prescaler == TQ * PRESDIV )
        {
          best_TQ = TQ;
          break;
        }
        actual_freq = (clk / PRESDIV) / TQ;
        diff = (actual_freq > frequency) ? actual_freq - frequency : frequency - actual_freq;
        if ( diff < lowest_diff )
        {
          lowest_diff = diff;
          best_TQ = TQ;
        }
      }
    }
    if ( (best_TQ >= CAN_MIN_TQ) && (PRESDIV <= 256) )
    {
      PSEG2 = (best_TQ - CAN_MAX_PROPSEG) / 2;
      if ( PSEG2 < CAN_MIN_PSEG2 ) PSEG2 = CAN_MIN_PSEG2;
      PSEG1 = (best_TQ == CAN_MIN_TQ) ? 1 : PSEG2;
      PROPSEG = best_TQ - PSEG1 - PSEG2 - 1;
      RJW = (PSEG1 < CAN_MAX_RJW) ? PSEG1 : CAN_MAX_RJW;

      can_reg_ptr->CANCTRL &= (~(FLEXCAN_CANCTRL_SAMP | FLEXCAN_CANCTRL_PROPSEG(0xFFFFFFFF) | FLEXCAN_CANCTRL_RJW(0xFFFFFFFF) | FLEXCAN_CANCTRL_PSEG1(0xFFFFFFFF) | FLEXCAN_CANCTRL_PSEG2(0xFFFFFFFF) | FLEXCAN_CANCTRL_PRESDIV(0xFFFFFFFF)));
      can_reg_ptr->CANCTRL |= (0 | FLEXCAN_CANCTRL_PROPSEG(PROPSEG - 1) | FLEXCAN_CANCTRL_RJW(RJW - 1) | FLEXCAN_CANCTRL_PSEG1(PSEG1 - 1) | FLEXCAN_CANCTRL_PSEG2(PSEG2 - 1) | FLEXCAN_CANCTRL_PRESDIV(PRESDIV - 1));
    }
    else
    {
      return FLEXCAN_INVALID_FREQUENCY;
    }
  }
  else
  {
    PSEG1 = (bit_timing0 & 0x00070000) >> 16;
    PROPSEG = bit_timing0 & 0x7;
    PSEG2 = (bit_timing1 & 0x00070000) >> 16;
    RJW = (bit_timing1 & 0x00000300) >> 8;
    PRESDIV = bit_timing1 & 0x000000FF;

    can_reg_ptr->CANCTRL &= (~(FLEXCAN_CANCTRL_SAMP | FLEXCAN_CANCTRL_PROPSEG(0xFFFFFFFF) | FLEXCAN_CANCTRL_RJW(0xFFFFFFFF)| FLEXCAN_CANCTRL_PSEG1(0xFFFFFFFF) | FLEXCAN_CANCTRL_PSEG2(0xFFFFFFFF)| FLEXCAN_CANCTRL_PRESDIV(0xFFFFFFFF)));
    can_reg_ptr->CANCTRL |= (0 | FLEXCAN_CANCTRL_PROPSEG(PROPSEG) | FLEXCAN_CANCTRL_RJW(RJW)| FLEXCAN_CANCTRL_PSEG1(PSEG1) | FLEXCAN_CANCTRL_PSEG2(PSEG2)| FLEXCAN_CANCTRL_PRESDIV(PRESDIV));
  }

  /* Set maximum number of message buffers */
  can_reg_ptr->CANMCR |= FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF);

  /* Rx global mask */
  can_reg_ptr->RXGMASK   = FLEXCAN_EXTENDED_ID_MASK(0xFFFFFFFF);

  /* Rx reg 14 mask */
  can_reg_ptr->RX14MASK  = FLEXCAN_EXTENDED_ID_MASK(0xFFFFFFFF);

  /* Rx reg 15 mask */
  can_reg_ptr->RX15MASK  = FLEXCAN_EXTENDED_ID_MASK(0xFFFFFFFF);

  /* 
  * Initialize all message buffers as inactive 
  */
  for (i = 0; i <= FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF); i++)
  {
    can_bufstruct_ptr[i].CONTROL  = 0x00000000;
  }

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Initialize_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function initializes the specified FlexCAN mailbox (Message Buffer)
*
*END*********************************************************************/
uint_32 FLEXCAN_Initialize_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number,
 /* [IN] message ID */
 uint_32 identifier,
 /* [IN] number of bytes to write to the mailbox (0 to 8) */
 uint_32 data_len_code,
 /* [IN] transmission or reception direction (CAN_TX or CAN_RX) */
 uint_32 direction,
 /* [IN] mailbox format (CAN_STANDARD OR CAN_EXTENDED) */
 uint_32 format,
 /* [IN] enable/disable interrupt for the mailbox */
 uint_32 int_enable
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR msg_obj_ptr;
  vuint_32                               temp_msg_config;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( (mailbox_number > FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  if ( 8 < data_len_code )
  {
    return (FLEXCAN_DATA_SIZE_ERROR);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);
  /*
  ** TX = 0x80: MB is not ready for transmit (code=0b1000)
  ** RX = 0x00: MB is not active (code=0b0000)
  ** Set code depending on direction and set the number of data bytes to be stored 
  */
  if ( direction == FLEXCAN_TX )
  {
    temp_msg_config = FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE;
  }
  else if ( direction == FLEXCAN_RX )
  {
    temp_msg_config = FLEXCAN_RX_MSG_BUFFER_NOT_ACTIVE;
  }
  else
  {
    return (FLEXCAN_INVALID_DIRECTION);
  } /* Endif */

  /* Set transmit length to data length in bytes */
  temp_msg_config |= (data_len_code << 16);
  msg_obj_ptr->CONTROL = temp_msg_config;

  /* Set the ID according the format structure */
  if ( format == FLEXCAN_EXTENDED )
  {
    /* Set IDE */
    msg_obj_ptr->CONTROL |= FLEXCAN_MSGCTRL_IDE;

    /* Set SRR bit for Tx buffers */
    if ( direction == FLEXCAN_TX ) msg_obj_ptr->CONTROL |= FLEXCAN_MSGCTRL_SRR;

    /* Start CR# 1730 */
    msg_obj_ptr->ID = FLEXCAN_EXTENDED_ID_MASK(identifier);
  }
  else if ( format == FLEXCAN_STANDARD )
  {
    /* make sure IDE and SRR are not set */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCTRL_IDE | FLEXCAN_MSGCTRL_SRR);

    /* ID[28-18] */
    msg_obj_ptr->ID = FLEXCAN_STANDARD_ID_MASK(identifier);
  }
  else return (FLEXCAN_MESSAGE_FORMAT_UNKNOWN);
  /* Endif */

  /* Enable CAN interrupt */
  if ( int_enable == FLEXCAN_ENABLE )
  {
    /* Enable interrupt line */
    if ( FLEXCAN_Int_enable(dev_num, mailbox_number) ) return (FLEXCAN_INT_ENABLE_FAILED);
  }
  else
  {
    /* Disable interrupt line */
    if ( FLEXCAN_Int_disable(dev_num, mailbox_number) ) return (FLEXCAN_INT_DISABLE_FAILED);
  }

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Request_mailbox 
* Returned Value   : uint_32 
* Comments         :
*    This function sets the RTR bit (reomte frame) for the 
*    specified mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Request_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number,
 /* [IN] mailbox format (CAN_STANDARD OR CAN_EXTENDED) */
 uint_32 format
 )
{ /* Body */
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR msg_obj_ptr;
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(
                                                                      dev_num, mailbox_number);
  /* Set RTR */
  msg_obj_ptr->CONTROL |= FLEXCAN_MSGCTRL_RTR;

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Activate_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function activates the specified mailbox with the activation code
*    provided.
*
*END*********************************************************************/
uint_32 FLEXCAN_Activate_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number,
 /* [IN] Code value for CONTROL/STATUS field */
 uint_32 code_val
 )
{ /* Body */
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num,
                                                                      mailbox_number);
  /* Reset the code */
  msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

  /* Activating mailbox */
  msg_obj_ptr->CONTROL |= code_val;

  /* // Unlock the mailbox ... ???
  can_reg_ptr = (FLEXCAN_REG_STRUCT_PTR)can_ptr;
  code_val = can_reg_ptr->TIMER;
  */

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Lock_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function locks the specified mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Lock_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  vuint_32 temp;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  /* Lock the mailbox */
  temp = msg_obj_ptr->CONTROL;

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_unlock_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function unlocks flexcan mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Unlock_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  vuint_32                                 temp;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* Unlock the mailbox */
  temp = can_reg_ptr->TIMER;

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Set_global_extmask  
* Returned Value   : uint_32 
* Comments         :
*    This function sets the FLEXCAN chip global extended mask.
*
*END*********************************************************************/
uint_32 FLEXCAN_Set_global_extmask
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] Mask */
 uint_32 extmask
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* MID[28-0] */
  can_reg_ptr->RXGMASK = FLEXCAN_EXTENDED_ID_MASK(extmask);

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Set_Buf14_extmask  
* Returned Value   : uint_32 
* Comments         :
*    This function sets the FlexCAN buf14 extended mask. 
*
*END*********************************************************************/
uint_32 FLEXCAN_Set_buf14_extmask
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] Mask */
 uint_32 extmask
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* MID[28-0] */
  can_reg_ptr->RX14MASK = FLEXCAN_EXTENDED_ID_MASK(extmask);

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Set_Buf15_extmask  
* Returned Value   : uint_32 
* Comments         :
*    This function sets the FlexCAN buf15 extended mask. 
*
*END*********************************************************************/
uint_32 FLEXCAN_Set_buf15_extmask
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] Mask */
 uint_32 extmask
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* MID[28-0] */
  can_reg_ptr->RX15MASK = FLEXCAN_EXTENDED_ID_MASK(extmask);

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Set_global_stdmask  
* Returned Value   : uint_32 
* Comments         :
*    This function sets the CAN chip global standard mask.
*
*END*********************************************************************/
uint_32 FLEXCAN_Set_global_stdmask
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] Mask */
 uint_32 stdmask
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  /* 11 bit standard mask */
  can_reg_ptr->RXGMASK = FLEXCAN_STANDARD_ID_MASK(stdmask);

  return (FLEXCAN_OK);

} /* Endbody */
/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Set_Buf14_stdmask  
* Returned Value   : uint_32 
* Comments         :
*    This function sets the FlexCAN buf14 standard mask. 
*
*END*********************************************************************/
uint_32 FLEXCAN_Set_buf14_stdmask
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] Mask */
 uint_32 stdmask
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  can_reg_ptr->RX14MASK = FLEXCAN_STANDARD_ID_MASK(stdmask);

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Set_Buf15_stdmask  
* Returned Value   : uint_32 
* Comments         :
*    This function sets the FlexCAN buf15 standard mask. 
*
*END*********************************************************************/
uint_32 FLEXCAN_Set_buf15_stdmask
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] Mask */
 uint_32 stdmask
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  can_reg_ptr->RX15MASK = FLEXCAN_STANDARD_ID_MASK(stdmask);

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Tx_successful  
* Returned Value   : boolean 
* Comments         :
*    This function returns TRUE or FALSE depending on whether or not 
*    the message has been placed in the mailbox. 
*
*END*********************************************************************/
boolean FLEXCAN_Tx_successful
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num
 )
{  /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR  can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  return ((can_reg_ptr->IFLAG & FLEXCAN_IMASK_VALUE) != 0);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Tx_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function writes a message to the specified mailbox and sends once
*
*END*********************************************************************/
uint_32 FLEXCAN_Tx_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number,
 /* [IN] Data to be written to the mailbox */
 pointer data
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  volatile uchar_ptr                       data_array;
  uint_32                                  i;
  vuint_32                                 data_length;
  vuint_32                                 temp;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  /* Locking mailbox */
  msg_obj_ptr->CONTROL |= FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE;

  data_length = ((msg_obj_ptr->CONTROL) & FLEXCAN_MSG_CTRL_DLEN);

  /* Copy user's buffer into mailbox data area */
  data_array = (uchar_ptr)data;
  for (i = 0; i < FLEXCAN_MSG_BUFF_MAXDATA_SIZE; i++)
  {
    msg_obj_ptr->DATA[i] = data_array[i];
  } /* Endfor */

  /* transmit data in message buffer */
  msg_obj_ptr->CONTROL |= (FLEXCAN_MESSAGE_TRANSMIT_ONCE | data_length);

  /* Unlock message buffer */
  temp = can_reg_ptr->TIMER;

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Rx_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function reads a message from the specified mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Rx_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32  mailbox_number,
 /* [IN] Location to store the data from the mailbox */
 pointer  data
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  uchar_ptr  data_array;
  uint_32  i;
  uint_32  return_code = FLEXCAN_OK;
  vuint_32 data_length;
  vuint_32 DLC;
  vuint_32 temp;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  data_array = (uchar_ptr)data;

  /* Lock */
  temp = msg_obj_ptr->CONTROL;

  /* Data length */
  data_length = (temp & FLEXCAN_MSG_CTRL_DLEN);
  DLC = data_length >> 16;

  /* Check for mailbox activation code */
  if ( (temp & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_RX_MSG_BUFFER_EMPTY )
  {
    /* No data return */
    return_code = FLEXCAN_NO_MESSAGE;
  }

  if ( (temp & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_RX_MSG_BUFFER_NOT_ACTIVE )
  {
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* mailbox is not active, return */
    msg_obj_ptr->CONTROL |= FLEXCAN_RX_MSG_BUFFER_EMPTY | data_length;

    return_code = FLEXCAN_NO_MESSAGE;
  }
  /*
  ** Copy data to user buffer
  */
  if ( (temp & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_RX_MSG_BUFFER_BUSY )
  {
    // Wait for copying data
    while (msg_obj_ptr->CONTROL & FLEXCAN_RX_MSG_BUFFER_BUSY)
    {}

    // Copying data
    for (i = 0; i < DLC; i++)
    {
      data_array[i] = msg_obj_ptr->DATA[i];
      msg_obj_ptr->DATA[i] = 0;
    } /* Endfor */

    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= FLEXCAN_RX_MSG_BUFFER_EMPTY | data_length;

    return_code = FLEXCAN_MESSAGE_BUSY;
  }
  else if ( (temp & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_RX_MSG_BUFFER_FULL )
  {
    // Copying data
    for (i = 0; i < DLC; i++)
    {
      data_array[i] = msg_obj_ptr->DATA[i];
      msg_obj_ptr->DATA[i] = 0;
    } /* Endfor */

    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= FLEXCAN_RX_MSG_BUFFER_EMPTY | data_length;

    return_code = FLEXCAN_OK;

  }
  else if ( (temp & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_RX_MSG_BUFFER_OVERRUN )
  {
    // Copying data
    for (i = 0; i < DLC; i++)
    {
      data_array[i] = msg_obj_ptr->DATA[i];
      msg_obj_ptr->DATA[i] = 0;
    } /* Endfor */

    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= FLEXCAN_RX_MSG_BUFFER_EMPTY | data_length;

    return_code = FLEXCAN_MESSAGE_LOST;
  }

  /* Unlock message buffer */
  temp = can_reg_ptr->TIMER;

  return return_code;

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Disable_mailbox  
* Returned Value   : uint_32 
* Comments         :
*    This function disables the specified mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Disable_mailbox
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number
 )
{ /* Body */

  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  msg_obj_ptr->CONTROL &= FLEXCAN_MSG_DISABLE;

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Request_message  
* Returned Value   : uint_32 
* Comments         :
*    This function requests remote messages as specified by the
*    mailbox number and format parameters. Sets the message buffer's code 
*
*END*********************************************************************/
uint_32 FLEXCAN_Request_message
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number,
 /* [IN] mailbox format (CAN_STANDARD OR CAN_EXTENDED) */
 uint_32 format
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  uint_32                                  error_code;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  /* Set message format to remote */
  error_code = FLEXCAN_Request_mailbox(dev_num, mailbox_number, format);
  if ( error_code != FLEXCAN_OK )
  {
    return (error_code);
  }

  /* 
  ** Activate mailbox: code 0b1100 
  ** Data field length and data in remote message is ignored.
  ** Mailbox becomes receive mailbox.
  */
  error_code = FLEXCAN_Activate_mailbox(dev_num, mailbox_number, FLEXCAN_MESSAGE_TRANSMIT_REMOTE);
  if ( error_code != FLEXCAN_OK )
  {
    return (error_code);
  }

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Rx_message  
* Returned Value   : uint_32 
* Comments         :
*    This function receives a message from the specified masked mailbox .
*   
*
*END*********************************************************************/
uint_32 FLEXCAN_Rx_message
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32     mailbox_number,
 /* [IN/OUT] message ID */
 uint_32_ptr identifier,
 /* [IN] mailbox format (CAN_STANDARD OR CAN_EXTENDED) */
 uint_32     format,
 /* [IN/OUT] number of bytes to write to the mailbox (0 to 8) */
 uint_32_ptr data_len_code,
 /* [IN/OUT] Location to store the data from the mailbox */
 pointer     data,
 /* [IN] enable/disable interrupt for the mailbox */
 uint_32     int_enable
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;

  vuint_32                code;
  uint_32                 i;
  uint_32                 return_code = FLEXCAN_OK;
  vuint_32                temp;
  vuint_32                DLC;
  vuint_32                ID;
  vuint_32                dlen;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  /* store data len */
  dlen = msg_obj_ptr->CONTROL & FLEXCAN_MSG_CTRL_DLEN;
  DLC = dlen >> 16;

  /* Reassemble the message ID */
  switch (format)
  {
  case (FLEXCAN_EXTENDED):
    /* Start CR# 1730 */
    ID = (msg_obj_ptr->ID & 0x1FFFFFFF);
    break;
  case (FLEXCAN_STANDARD):
    /* ID[28-18] */
    ID = ((msg_obj_ptr->ID & 0x1FFC0000) >> 18);
    break;
  default:
    return (FLEXCAN_MESSAGE_FORMAT_UNKNOWN);
  } /* Endswitch */

  /*
  ** if "code" value for specified Rx message buffer is full
  ** copy data to the buffer and set the code to empty and return. 
  ** if "code" value is set to not active, set the code to empty and
  ** return message buffer empty.
  ** if "code" value is set to overrun, copy data to the buffer and
  ** set the code to empty then return overrun.
  ** if "code" value is set to busy, copy data to the buffer and
  ** set the code to empty then return busy.
  */
  code = msg_obj_ptr->CONTROL & FLEXCAN_MSGCRTL_CODE;
  if ( code == FLEXCAN_RX_MSG_BUFFER_EMPTY )
  {
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= (FLEXCAN_RX_MSG_BUFFER_EMPTY | dlen);

    /* No data return */
    return_code = FLEXCAN_NO_MESSAGE;
  }

  if ( code == FLEXCAN_RX_MSG_BUFFER_NOT_ACTIVE )
  {
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= (FLEXCAN_RX_MSG_BUFFER_EMPTY | dlen);

    /* No data return */
    return_code = FLEXCAN_NO_MESSAGE;
  }

  if ( code == FLEXCAN_RX_MSG_BUFFER_FULL )
  {
    for (i = 0; i < DLC; i++)
    {
      ((uchar_ptr)data)[i] = msg_obj_ptr->DATA[i];
      msg_obj_ptr->DATA[i] = 0;
    } /* Endfor */

    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= (FLEXCAN_RX_MSG_BUFFER_EMPTY | dlen);
    return_code = FLEXCAN_OK;
  }
  else if ( code == FLEXCAN_RX_MSG_BUFFER_OVERRUN )
  {
    for (i = 0; i < DLC; i++)
    {
      ((uchar_ptr)data)[i] = msg_obj_ptr->DATA[i];
      msg_obj_ptr->DATA[i] = 0;
    } /* Endfor */

    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= (FLEXCAN_RX_MSG_BUFFER_EMPTY | dlen);
    return_code = FLEXCAN_MESSAGE_OVERWRITTEN;
  }
  else if ( code == FLEXCAN_RX_MSG_BUFFER_BUSY )
  {
    for (i = 0; i < DLC; i++)
    {
      ((uchar_ptr)data)[i] = msg_obj_ptr->DATA[i];
      msg_obj_ptr->DATA[i] = 0;
    } /* Endfor */
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* Set the code */
    msg_obj_ptr->CONTROL |= (FLEXCAN_RX_MSG_BUFFER_EMPTY | dlen);
    return_code =  FLEXCAN_MESSAGE_OVERWRITTEN;
  } /* Endif */

  /* Unlock message buffer. only in polling mode */
  if ( int_enable == FLEXCAN_DISABLE )
  {
    temp = can_reg_ptr->TIMER;
  }

  /* Store data length */
  *data_len_code = DLC;

  /* Store identifier */
  *identifier = ID;

  return return_code;
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Tx_message  
* Returned Value   : uint_32 
* Comments         :
*    This function sends a message from the specified mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Tx_message
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] mailbox number */
 uint_32 mailbox_number,
 /* [IN] message ID */
 uint_32 identifier,
 /* [IN] mailbox format (CAN_STANDARD OR CAN_EXTENDED) */
 uint_32 format,
 /* [IN] number of bytes to write to the mailbox (0 to 8) */
 uint_32 data_len_code,
 /* [IN] Location to store the data from the mailbox */
 pointer data
 )
{ /* Body */

  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  uint_32                 i;
  vuint_32                temp16;
  vuint_32                code;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  if ( 8 < data_len_code )
  {
    return (FLEXCAN_DATA_SIZE_ERROR);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  /* Set the ID according the format structure */
  if ( format == FLEXCAN_EXTENDED )
  {
    /* Set IDE */
    msg_obj_ptr->CONTROL |= FLEXCAN_MSGCTRL_IDE;

    /* Set SRR bit */
    msg_obj_ptr->CONTROL |= FLEXCAN_MSGCTRL_SRR;

    /* Start CR# 1730 */
    msg_obj_ptr->ID = FLEXCAN_EXTENDED_ID_MASK(identifier);
  }
  else if ( format == FLEXCAN_STANDARD )
  {
    /* make sure IDE and SRR are not set */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCTRL_IDE | FLEXCAN_MSGCTRL_SRR);

    /* ID[28-18] */
    msg_obj_ptr->ID = FLEXCAN_STANDARD_ID_MASK(identifier);
  }
  else
  {
    return (FLEXCAN_MESSAGE_FORMAT_UNKNOWN);
  } /* Endif */

  /*
  ** Copy user's buffer into mailbox data area
  */
  for (i = 0; i < data_len_code; i++)
  {
    msg_obj_ptr->DATA[i] = ((uchar_ptr)data)[i];
  } /* Endfor */

  data_len_code <<= 16;
  /*
  ** Set data up for send 
  ** send is automatic when remote message is received
  ** send when message buffer is not active
  */
  code = msg_obj_ptr->CONTROL;
  if ( (code & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE )
  {
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);
    /* Clear the DLC */
    msg_obj_ptr->CONTROL &= ~FLEXCAN_MSG_CTRL_DLEN;
    msg_obj_ptr->CONTROL |= (FLEXCAN_MESSAGE_TRANSMIT_ONCE | data_len_code);
  }

  /* Update data to respond to remote request frame */
  if ( ((code & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_MESSAGE_TRANSMIT_RESPONED_ONLY) ||
       ((code & FLEXCAN_MSGCRTL_CODE) == FLEXCAN_MESSAGE_TRANSMIT_RESPONED) )
  {
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);
    /* Clear the DLC */
    msg_obj_ptr->CONTROL &= ~FLEXCAN_MSG_CTRL_DLEN;
    msg_obj_ptr->CONTROL |= (FLEXCAN_MESSAGE_TRANSMIT_RESPONED | data_len_code);
  }

  /* Unlock message buffer */
  temp16 = can_reg_ptr->TIMER;

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Read  
* Returned Value   : uint_32 
* Comments         :
*    This function reads the contents of the specified CAN memory.
*
*END*********************************************************************/
uint_32 FLEXCAN_Read
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] offset from the base address of the device */
 uint_32         offset,
 /* [IN/OUT] pointer to location of where the data is to be read to */
 uint_32_ptr     data_ptr
 )
{ /* Body */

  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  uint_32                                  read_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  read_ptr = (vuint_32)can_reg_ptr + offset;
  *data_ptr = (uint_32) * (uchar_ptr)read_ptr;

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Write  
* Returned Value   : uint_32 
* Comments         :
*    This function writes the specified value to the specified CAN 
*    memory.
*
*END*********************************************************************/
uint_32 FLEXCAN_Write
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] offset from the base address of the device */
 uint_32     offset,
 /* Data to be written at the start of the offset */
 uint_32     value
 )
{ /* Body */

  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  vuint_32                                  write_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  write_ptr = (uint_32)can_reg_ptr + offset;
  *(uchar_ptr)write_ptr = (uchar)value;

  return (FLEXCAN_OK);

} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Get_status  
* Returned Value   : uint_32 
* Comments         :
*    This function gets the CAN chip in the specified state.
*
*END*********************************************************************/
uint_32 FLEXCAN_Get_status
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN/OUT] pointer to Error/Status registry */
 uint_32_ptr can_status
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  *can_status = can_reg_ptr->ERRSTAT;

  return (FLEXCAN_OK);
} /* Endbody */

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_update_message
* Returned Value   : uint_32 
* Comments         :
*    This function updates data as a response to a remote frame.
*
*END*********************************************************************/
uint_32 FLEXCAN_Update_message
(
 /* [IN] FlexCAN device number */
 uint_8 dev_num,
 /* [IN] pointer to data */
 pointer data_ptr,
 /* [IN] number of bytes to write to the mailbox (0 to 8) */
 uint_32 data_len_code,
 /* [IN] mailbox format (CAN_STANDARD OR CAN_EXTENDED) */
 uint_32 format,
 /* [IN] mailbox number */
 uint_32 mailbox_number
 )
{ /* Body */
  volatile FLEXCAN_REG_STRUCT_PTR          can_reg_ptr;
  volatile FLEXCAN_MSG_OBJECT_STRUCT_PTR   msg_obj_ptr;
  uint_32   i;
  uchar_ptr data_array;
  vuint_32  code;

  can_reg_ptr = _bsp_get_flexcan_base_address(dev_num);
  if ( NULL == can_reg_ptr )
  {
    return (FLEXCAN_INVALID_ADDRESS);
  }

  if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB(0xFFFFFFFF)) )
  {
    return (FLEXCAN_INVALID_MAILBOX);
  }

  if ( 8 < data_len_code )
  {
    return (FLEXCAN_DATA_SIZE_ERROR);
  }

  msg_obj_ptr = (FLEXCAN_MSG_OBJECT_STRUCT_PTR)FLEXCAN_Get_msg_object(dev_num, mailbox_number);

  /* check for RTR bit */
  code = msg_obj_ptr->CONTROL;
  if ( (code & FLEXCAN_MSGCTRL_RTR) == FLEXCAN_MSGCTRL_RTR ) return FLEXCAN_RTR_NOT_SET;

  /* check if mailbox is set for transmit */
  if ( (code & FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE) == FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE )
  {
    /* Reset the code */
    msg_obj_ptr->CONTROL &= ~(FLEXCAN_MSGCRTL_CODE);

    /* store data */
    data_array = (uchar_ptr)data_ptr;
    for (i = 0; i < data_len_code; i++)
    {
      msg_obj_ptr->DATA[i] = data_array[i];
    }

    /* 
    ** Data frame to be transmitted only as a response to 
    ** a remote frame. code 0b1010
    */
    msg_obj_ptr->CONTROL |= (FLEXCAN_MESSAGE_TRANSMIT_RESPONED | data_len_code);
  }
  else return FLEXCAN_INVALID_MAILBOX;

  return FLEXCAN_OK;

} /* Endbody */
