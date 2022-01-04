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
* $FileName: kflexcan.c$
* $Version : 3.8.13.0$
* $Date    : Jun-6-2012$
*
* Comments:
*   Kinetis FlexCAN specific code.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Starting FLEXCAN in normal mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_HALT_MASK | CAN_MCR_MDIS_MASK
                            | CAN_MCR_FRZ_MASK);

   /* wait for synchronization */
   while(can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

   return( FLEXCAN_OK );
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

   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

  /* check for low power mode */
  if(can_reg_ptr->MCR & CAN_MCR_LPMACK_MASK)
  {
     /* Enable clock */
     can_reg_ptr->MCR &= (~ CAN_MCR_MDIS_MASK);
     /* wait until enabled */
     while (can_reg_ptr->MCR & CAN_MCR_LPMACK_MASK){}
  }

  /* Reset the FLEXCAN */
  can_reg_ptr->MCR = CAN_MCR_SOFTRST_MASK;

  /* Wait for reset cycle to complete */
  while (can_reg_ptr->MCR & CAN_MCR_SOFTRST_MASK){}

  /* Set Freeze, Halt */
  can_reg_ptr->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);

  /* check for freeze Ack */
  if(
     ( (can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) != CAN_MCR_FRZACK_MASK ) ||
     ( (can_reg_ptr->MCR & CAN_MCR_NOTRDY_MASK) != CAN_MCR_NOTRDY_MASK )
    )
     return (FLEXCAN_SOFTRESET_FAILED);

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

   volatile uchar_ptr      temp_ptr;
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (NULL);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (NULL);
   }

   temp_ptr = (uchar_ptr)can_reg_ptr;
   temp_ptr += FLEXCAN_MSG_BUFADDR_OFFSET;


   /*
   ** Calculate what message object to point to
   */
  temp_ptr += sizeof(can_reg_ptr->MB[0]) * mailbox_number;

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
   volatile CAN_MemMapPtr  can_reg_ptr;
   volatile uint_32        ret_code = FLEXCAN_OK;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   switch(mode)
   {
   case (FLEXCAN_NORMAL_MODE):
      /*
      ** Normal mode, check freeze ACK bit
      ** call start if bit is set
      */
      if( (can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == CAN_MCR_FRZACK_MASK)
      {
         if( FLEXCAN_Start(dev_num) )
            ret_code = FLEXCAN_START_FAILED;
      }
      break;
   case (FLEXCAN_LISTEN_MODE):
      /* Set CTRL1[LOM], listen-only mode */
      can_reg_ptr->CTRL1 |= CAN_CTRL1_LOM_MASK;
      break;
   case (FLEXCAN_TIMESYNC_MODE):
      /* Timer Synchronization mode (Global Network Time) */
      can_reg_ptr->CTRL1 |= CAN_CTRL1_TSYN_MASK;
      break;
   case (FLEXCAN_LOOPBK_MODE):
      /* Self test mode, Loop Back */
      can_reg_ptr->CTRL1 |= CAN_CTRL1_LPB_MASK;
      break;
   case (FLEXCAN_BOFFREC_MODE):
      /* Bus Off Recovery mode (according to CAN 2.0b) */
      can_reg_ptr->CTRL1 &= ~(CAN_CTRL1_BOFFMSK_MASK);
      break;
   case (FLEXCAN_FREEZE_MODE):
      /* Debug mode, Halt and Freeze */
      can_reg_ptr->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
     /* check for freeze Ack */
     if( (can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) != CAN_MCR_FRZACK_MASK)
        ret_code = FLEXCAN_FREEZE_FAILED;
      break;
   case (FLEXCAN_DISABLE_MODE):
      /* Debug mode, Halt and Freeze */
      can_reg_ptr->MCR |= CAN_MCR_MDIS_MASK;
      break;
   default:
      ret_code = FLEXCAN_INVALID_MODE;
   }

   return( ret_code );
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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if(clk == FLEXCAN_IPBUS_CLK)
   /* Internal bus clock (fsys/2) */
      can_reg_ptr->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

   else if (clk == FLEXCAN_OSC_CLK)
      /* External clock */
      can_reg_ptr->CTRL1 &= (~ CAN_CTRL1_CLKSRC_MASK);

   else
      return (FLEXCAN_CLOCK_SOURCE_INVALID);


   return( FLEXCAN_OK );
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : FLEXCAN_Initialize
* Returned Value   : uint_32
* Comments         :
*    This function initializes all mailbox-independant registers of the
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
   /* [IN] MS 16 bits = prescaller value, LS 16 bits = SJW */
   uint_32 bit_timing1,
   /* [IN] Bit rate in Kbps */
   uint_32 frequency,
   /* [IN] clock source */
   uint_32 clk
)
{ /* Body */
   volatile CAN_MemMapPtr  can_reg_ptr;
   vuint_32                PSEG1 =0;
   vuint_32                PSEG2 = 0;
   vuint_32                PROPSEG = 0;
   vuint_32                RJW = 0;
   vuint_32                PRESDIV = 0;
   uint_32                 i;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /*
   ** To access the memory mapped registers
   ** Entre disable mode (hard reset).
   */
   if((can_reg_ptr->MCR & CAN_MCR_MDIS_MASK) == 0x00000000)
   {
      /* clock disable (module) */
      can_reg_ptr->MCR = CAN_MCR_MDIS_MASK;

      /* wait until disable mode acknowledged */
      while (!can_reg_ptr->MCR & CAN_MCR_LPMACK_MASK) {}
   }

   /* Reset FLEXCAN, Halt, freeze mode */
   if(FLEXCAN_Softreset(dev_num))
      return (FLEXCAN_INIT_FAILED);
   
   /*  Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0) {}
   
   /* 
   ** Select the clock source 
   ** Default: internal bus clock (fsys/2).
   */
   FLEXCAN_Select_clk (dev_num, clk);
   
   if (can_reg_ptr->CTRL1 & CAN_CTRL1_CLKSRC_MASK) 
   {
       clk = _cm_get_clock(_cm_get_clock_configuration(), CM_CLOCK_SOURCE_BUS);
   }
   else
   {
       clk = _cm_get_clock(_cm_get_clock_configuration(), CM_CLOCK_SOURCE_OSCER);
   }   

   frequency *= 1000;

   /* bit timing settings */
   if( (bit_timing0 == 0x00) && (bit_timing1 == 0x00) )
   {
       // If BSP_CANPE_CLOCK is defined, then we will calculate the CAN bit timing parameters
       // using the method outlined in AN1798, section 4.1. A maximum time for PROP_SEG will be used,
       // the remaining TQ will be split equally between PSEG1 and PSEG2, provided PSEG2 >=2. RJW is
       // set to the minimum of 4 or PSEG1.
       
       uint_32 TQ_x_Prescaler = clk / frequency;
       uint_32 TQ, lowest_diff, diff, best_TQ, actual_freq;
    
       if (TQ_x_Prescaler < (CAN_MIN_TQ-1)) {
           // We will be off by more than 12.5%
           return FLEXCAN_INVALID_FREQUENCY;
       }
    
       // First, find the best TQ and pre-scaler to use for the desired frequency. If any exact matches
       // is found, we use the match that gives us the lowest pre-scaler and highest TQ, otherwise we pick
       // the TQ and prescaler that generates the closest frequency to the desired frequency.
    
       lowest_diff = frequency;
       best_TQ=0;
       for (TQ=CAN_MAX_TQ;TQ>=CAN_MIN_TQ;TQ--) {
           PRESDIV = TQ_x_Prescaler/TQ;
           if ( PRESDIV <=256){
               if (TQ_x_Prescaler == TQ*PRESDIV) {
                   best_TQ=TQ;
                   break;
               }
               actual_freq = (clk/PRESDIV)/TQ;
               diff = (actual_freq>frequency)?actual_freq-frequency:frequency-actual_freq;
               if (diff<lowest_diff) {
                   lowest_diff=diff;
                   best_TQ=TQ;
               }
           }
       }
       if ((best_TQ>=CAN_MIN_TQ) && ( PRESDIV <=256)) {
           PSEG2 = (best_TQ-CAN_MAX_PROPSEG)/2;
           if (PSEG2 < CAN_MIN_PSEG2) PSEG2=CAN_MIN_PSEG2;
           PSEG1 = (best_TQ == CAN_MIN_TQ)?1:PSEG2;
           PROPSEG = best_TQ - PSEG1 - PSEG2 - 1;
           RJW = (PSEG1<CAN_MAX_RJW)?PSEG1:CAN_MAX_RJW;

           can_reg_ptr->CTRL1 &= (~ (CAN_CTRL1_SMP_MASK | CAN_CTRL1_PROPSEG_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_PRESDIV_MASK));
           can_reg_ptr->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(PROPSEG - 1) | CAN_CTRL1_RJW(RJW - 1)
                                    | CAN_CTRL1_PSEG1(PSEG1 - 1) | CAN_CTRL1_PSEG2(PSEG2 - 1)
                                    | CAN_CTRL1_PRESDIV(PRESDIV - 1));
       } else {
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

      can_reg_ptr->CTRL1 &= (~ (CAN_CTRL1_SMP_MASK | CAN_CTRL1_PROPSEG_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_PRESDIV_MASK));
      can_reg_ptr->CTRL1 |= (0 | CAN_CTRL1_PROPSEG(PROPSEG) | CAN_CTRL1_RJW(RJW)
                                 | CAN_CTRL1_PSEG1(PSEG1) | CAN_CTRL1_PSEG2(PSEG2)
                                 | CAN_CTRL1_PRESDIV(PRESDIV));
   }

   /* Set maximum number of message buffers */
   can_reg_ptr->MCR |= CAN_MCR_MAXMB(0xFFFFFFFF);

   /* Rx global mask */
   can_reg_ptr->RXMGMASK   = CAN_ID_EXT(0xFFFFFFFF);

   /* Rx reg 14 mask */
   can_reg_ptr->RX14MASK  = CAN_ID_EXT(0xFFFFFFFF);

   /* Rx reg 15 mask */
   can_reg_ptr->RX15MASK  = CAN_ID_EXT(0xFFFFFFFF);

   /*
   * Initialize all message buffers as inactive
   */
   for (i = 0; i <= CAN_MCR_MAXMB(0xF); i++)
   {
       can_reg_ptr->MB[i].CS = 0x00000000;
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
   volatile CAN_MemMapPtr  can_reg_ptr;
   vuint_32                temp_msg_config;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( (mailbox_number > CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   if (8 < data_len_code)
   {
      return (FLEXCAN_DATA_SIZE_ERROR);
   }

   /*
   ** TX = 0x80: MB is not ready for transmit (code=0b1000)
   ** RX = 0x00: MB is not active (code=0b0000)
   ** Set code depending on direction and set the number of data bytes to be stored
   */
   if (direction == FLEXCAN_TX)
   {
      temp_msg_config = CAN_CS_CODE(CAN_TX_MSG_BUFFER_NOT_ACTIVE);
   }
   else if (direction == FLEXCAN_RX)
   {
      temp_msg_config = CAN_CS_CODE(CAN_RX_MSG_BUFFER_NOT_ACTIVE);
   }
   else
   {
      return (FLEXCAN_INVALID_DIRECTION);
   }/* Endif */

   /* Set transmit length to data length in bytes */
   temp_msg_config |= (data_len_code << CAN_CS_DLC_SHIFT);
   can_reg_ptr->MB[mailbox_number].CS = temp_msg_config;

   /* Set the ID according the format structure */
   if ( format == FLEXCAN_EXTENDED )
   {
      /* Set IDE */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_IDE_MASK;

      /* Set SRR bit for Tx buffers */
      if(direction == FLEXCAN_TX)
         can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_SRR_MASK;

      /* Start CR# 1730 */
      can_reg_ptr->MB[mailbox_number].ID &= ~(0x1FFFFFFF);
      can_reg_ptr->MB[mailbox_number].ID |= (identifier & 0x1FFFFFFF);
   }
   else if( format == FLEXCAN_STANDARD )
   {
      /* make sure IDE and SRR are not set */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);

      /* ID[28-18] */
      can_reg_ptr->MB[mailbox_number].ID &= ~CAN_ID_STD_MASK;
      can_reg_ptr->MB[mailbox_number].ID |= CAN_ID_STD(identifier);
   }
   else
      return (FLEXCAN_MESSAGE_FORMAT_UNKNOWN);
   /* Endif */

   /* Enable CAN interrupt */
   if(int_enable == FLEXCAN_ENABLE)
   {
      /* Enable interrupt line */
      if( FLEXCAN_Int_enable(dev_num, mailbox_number) )
         return (FLEXCAN_INT_ENABLE_FAILED);
   }
   else
   {
      /* Disable interrupt line */
      if( FLEXCAN_Int_disable(dev_num, mailbox_number) )
         return (FLEXCAN_INT_DISABLE_FAILED);
   }

   return ( FLEXCAN_OK );
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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   /* Set RTR */
   can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_RTR_MASK;

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   /* Reset the code */
   can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

   /* Activating mailbox */
   can_reg_ptr->MB[mailbox_number].CS |= code_val;

   /* // Unlock the mailbox ... ???
   can_reg_ptr = (CAN_MemMapPtr)can_ptr;
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
   volatile CAN_MemMapPtr  can_reg_ptr;
   vuint_32 temp;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return( FLEXCAN_INVALID_MAILBOX );
   }

   /* Lock the mailbox */
   temp = can_reg_ptr->MB[mailbox_number].CS ;

   return( FLEXCAN_OK );

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
   volatile CAN_MemMapPtr  can_reg_ptr;
   vuint_32                temp;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Unlock the mailbox */
   temp = can_reg_ptr->TIMER;

   return( FLEXCAN_OK );

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Set Freeze mode */
   can_reg_ptr->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK | CAN_MCR_NOTRDY_MASK;

   /* Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0){}

   /* MID[28-0] */
   can_reg_ptr->RXMGMASK = CAN_ID_EXT(extmask);

   /* De-assert Freeze Mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_FRZ_MASK);

   /* Wait till exit of freeze mode */
   while( can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Set Freeze mode */
   can_reg_ptr->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK | CAN_MCR_NOTRDY_MASK;

   /* Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0){}

   /* MID[28-0] */
   can_reg_ptr->RX14MASK = CAN_ID_EXT(extmask);

   /* De-assert Freeze Mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_FRZ_MASK);

   /* Wait till exit of freeze mode */
   while( can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Set Freeze mode */
   can_reg_ptr->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK | CAN_MCR_NOTRDY_MASK;

   /* Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0){}

   /* MID[28-0] */
   can_reg_ptr->RX15MASK = CAN_ID_EXT(extmask);

   /* De-assert Freeze Mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_FRZ_MASK);

   /* Wait till exit of freeze mode */
   while( can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Set Freeze mode */
   can_reg_ptr->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK | CAN_MCR_NOTRDY_MASK;

   /* Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0){}

   /* 11 bit standard mask */
   can_reg_ptr->RXMGMASK = CAN_ID_STD(stdmask);

   /* De-assert Freeze Mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_FRZ_MASK);

   /* Wait till exit of freeze mode */
   while( can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Set Freeze mode */
   can_reg_ptr->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK | CAN_MCR_NOTRDY_MASK;

   /* Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0){}

    /* 11 bit standard mask */
   can_reg_ptr->RX14MASK = CAN_ID_STD(stdmask);

   /* De-assert Freeze Mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_FRZ_MASK);

   /* Wait till exit of freeze mode */
   while( can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   /* Set Freeze mode */
   can_reg_ptr->MCR |= CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK | CAN_MCR_NOTRDY_MASK;

   /* Wait for entering the freeze mode */
   while((can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK) == 0){}

    /* 11 bit standard mask */
   can_reg_ptr->RX15MASK = CAN_ID_STD(stdmask);

   /* De-assert Freeze Mode */
   can_reg_ptr->MCR &= ~(CAN_MCR_FRZ_MASK);

   /* Wait till exit of freeze mode */
   while( can_reg_ptr->MCR & CAN_MCR_FRZACK_MASK){}

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   return ((can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK) != 0);

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
   volatile CAN_MemMapPtr  can_reg_ptr;
   volatile uchar_ptr      data_array;
   vuint_32                data_length;
   vuint_32                temp;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   /* Locking mailbox */
   can_reg_ptr->MB[mailbox_number].CS  |= CAN_CS_CODE(CAN_TX_MSG_BUFFER_NOT_ACTIVE);

   data_length = ( (can_reg_ptr->MB[mailbox_number].CS) & CAN_CS_DLC_MASK );

   /* Copy user's buffer into mailbox data area */
   data_array = (uchar_ptr) data;

   can_reg_ptr->MB[mailbox_number].WORD0 = data_array[3] | data_array[2]<<8 | data_array[1]<<16 | data_array[0]<<24;
   can_reg_ptr->MB[mailbox_number].WORD1 = data_array[7] | data_array[6]<<8 | data_array[5]<<16 | data_array[4]<<24;

   /* transmit data in message buffer */
   can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_ONCE) | data_length);

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
   volatile CAN_MemMapPtr  can_reg_ptr;
   volatile uchar_ptr      data_array;
   uint_32                 i;
   uint_32                 return_code = FLEXCAN_OK;
   vuint_32                data_length;
   vuint_32                DLC;
   vuint_32                temp;
   volatile                uchar_ptr mb_data_array;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   data_array = (uchar_ptr)data;

   /* Lock */
   temp = can_reg_ptr->MB[mailbox_number].CS;

   /* Data length */
   data_length = ( temp & CAN_CS_DLC_MASK );
   DLC = data_length >> CAN_CS_DLC_SHIFT;

   /* Check for mailbox activation code */
   if ( (temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY))
   {
      /* No data return */
      return_code = FLEXCAN_NO_MESSAGE;
   }

   if ( (temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_RX_MSG_BUFFER_NOT_ACTIVE) )
   {
      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* mailbox is not active, return */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | data_length;

      return_code = FLEXCAN_NO_MESSAGE;
   }

   /* initialize message buffer data pointer */
   mb_data_array = (uchar_ptr) &can_reg_ptr->MB[mailbox_number].WORD0;

   /*
   ** Copy data to user buffer
   */
   if ( (temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_RX_MSG_BUFFER_BUSY) )
   {
	  /* Wait for copying data */
      while(can_reg_ptr->MB[mailbox_number].CS & CAN_CS_CODE(CAN_RX_MSG_BUFFER_BUSY)) {}

	  /* Copying data */
      for ( i=0; i<DLC; i++ )
      {
         data_array[i] = mb_data_array[3 - (i&0x3) + (i&0x4)];
      } /* Endfor */
	  can_reg_ptr->MB[mailbox_number].WORD0 = 0;
      can_reg_ptr->MB[mailbox_number].WORD1 = 0;

      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | data_length;

      return_code = FLEXCAN_MESSAGE_BUSY;
   }
   else if ( (temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_RX_MSG_BUFFER_FULL) )
   {
      /* Copying data */
      for ( i=0; i<DLC; i++ )
      {
         data_array[i] = mb_data_array[3 - (i&0x3) + (i&0x4)];
      } /* Endfor */

      can_reg_ptr->MB[mailbox_number].WORD0 = 0;
      can_reg_ptr->MB[mailbox_number].WORD1 = 0;

      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | data_length;

      return_code = FLEXCAN_OK;

   } else if ( (temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_RX_MSG_BUFFER_OVERRUN) )
   {
      /* Copying data */
      for ( i=0; i<DLC; i++ )
      {
         data_array[i] = mb_data_array[3 - (i&0x3) + (i&0x4)];
      } /* Endfor */

      can_reg_ptr->MB[mailbox_number].WORD0 = 0;
      can_reg_ptr->MB[mailbox_number].WORD1 = 0;

      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | data_length;

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

   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   can_reg_ptr->MB[mailbox_number].CS &= ~CAN_CS_CODE_MASK;

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
   volatile CAN_MemMapPtr  can_reg_ptr;
   uint_32                 error_code;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return( FLEXCAN_INVALID_MAILBOX );
   }

   /* Set message format to remote */
   error_code = FLEXCAN_Request_mailbox( dev_num, mailbox_number, format );
   if ( error_code != FLEXCAN_OK )
   {
      return( error_code );
   }

   /*
   ** Activate mailbox: code 0b1100
   ** Data field length and data in remote message is ignored.
   ** Mailbox becomes receive mailbox.
   */
   error_code = FLEXCAN_Activate_mailbox( dev_num, mailbox_number, CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_REMOTE) );
   if ( error_code != FLEXCAN_OK )
   {
      return( error_code );
   }

   return( FLEXCAN_OK );

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
   volatile CAN_MemMapPtr  can_reg_ptr;

   vuint_32                code;
   uint_32                 i;
   uint_32                 return_code = FLEXCAN_OK;
   vuint_32                temp;
   vuint_32                DLC;
   vuint_32                ID;
   vuint_32                dlen;
   volatile uchar_ptr      mb_data_array;
   volatile uchar_ptr      data_array;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return( FLEXCAN_INVALID_MAILBOX );
   }

   /* store data len */
   dlen = can_reg_ptr->MB[mailbox_number].CS & CAN_CS_DLC_MASK;
   DLC = dlen >> CAN_CS_DLC_SHIFT;

   /* Reassemble the message ID */
   switch (format)
   {
   case (FLEXCAN_EXTENDED):
      /* Start CR# 1730 */
      ID = (can_reg_ptr->MB[mailbox_number].ID & 0x1FFFFFFF);
      break;
   case (FLEXCAN_STANDARD):
      /* ID[28-18] */
      ID = ((can_reg_ptr->MB[mailbox_number].ID & CAN_ID_STD_MASK) >> CAN_ID_STD_SHIFT);
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
   code = can_reg_ptr->MB[mailbox_number].CS & CAN_CS_CODE_MASK;

   /* initialize message buffer data pointer */
   mb_data_array = (uchar_ptr) &can_reg_ptr->MB[mailbox_number].WORD0;

   if ( code == CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY))
   {
      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | dlen);

      /* No data return */
      return_code = FLEXCAN_NO_MESSAGE;
   }

   if ( code == CAN_CS_CODE(CAN_RX_MSG_BUFFER_NOT_ACTIVE) )
   {
      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | dlen);

      /* No data return */
      return_code = FLEXCAN_NO_MESSAGE;
   }

   if ( code == CAN_CS_CODE(CAN_RX_MSG_BUFFER_FULL) )
   {
      data_array = (uchar_ptr)data;
      for ( i=0 ; i<DLC ; i++ )
      {
         data_array[i] = mb_data_array[3 - (i&0x3) + (i&0x4)];
      } /* Endfor */

      can_reg_ptr->MB[mailbox_number].WORD0 = 0;
      can_reg_ptr->MB[mailbox_number].WORD1 = 0;

      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | dlen);
      return_code = FLEXCAN_OK;
   }
   else if ( code == CAN_CS_CODE(CAN_RX_MSG_BUFFER_OVERRUN) )
   {
      data_array = (uchar_ptr)data;
      for ( i=0 ;i<DLC ; i++ )
      {
         data_array[i] = mb_data_array[3 - (i&0x3) + (i&0x4)];
      } /* Endfor */

      can_reg_ptr->MB[mailbox_number].WORD0 = 0;
      can_reg_ptr->MB[mailbox_number].WORD1 = 0;

      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | dlen);
      return_code = FLEXCAN_NO_MESSAGE;
   }
   else if ( code == CAN_CS_CODE(CAN_RX_MSG_BUFFER_BUSY) )
   {
      data_array = (uchar_ptr)data;
      for ( i=0 ;i<DLC ; i++ )
      {
         data_array[i] = mb_data_array[3 - (i&0x3) + (i&0x4)];
      } /* Endfor */

      can_reg_ptr->MB[mailbox_number].WORD0 = 0;
      can_reg_ptr->MB[mailbox_number].WORD1 = 0;

      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* Set the code */
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_RX_MSG_BUFFER_EMPTY) | dlen);
      return_code =  FLEXCAN_MESSAGE_OVERWRITTEN;
   } /* Endif */

   /* Unlock message buffer. only in polling mode */
   if(int_enable == FLEXCAN_DISABLE)
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

   volatile CAN_MemMapPtr  can_reg_ptr;
   uint_32                 i;
   vuint_32                temp16;
   vuint_32                code;
   uchar_ptr               mb_data_array;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   if (8 < data_len_code)
   {
      return (FLEXCAN_DATA_SIZE_ERROR);
   }

   /* Set the ID according the format structure */
   if ( format == FLEXCAN_EXTENDED )
   {
      /* Set IDE */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_IDE_MASK;

      /* Set SRR bit */
      can_reg_ptr->MB[mailbox_number].CS |= CAN_CS_SRR_MASK;

      /* Start CR# 1730 */
      can_reg_ptr->MB[mailbox_number].ID &= ~(0x1FFFFFFF);
      can_reg_ptr->MB[mailbox_number].ID |= (identifier & 0x1FFFFFFF);
   }
   else if( format == FLEXCAN_STANDARD )
   {
      /* make sure IDE and SRR are not set */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);

      /* ID[28-18] */
      can_reg_ptr->MB[mailbox_number].ID &= ~CAN_ID_STD_MASK;
      can_reg_ptr->MB[mailbox_number].ID |= CAN_ID_STD(identifier);
   }
   else
   {
      return (FLEXCAN_MESSAGE_FORMAT_UNKNOWN);
   }/* Endif */


   mb_data_array = (uchar_ptr) &can_reg_ptr->MB[mailbox_number].WORD0;
   /*
   ** Copy user's buffer into mailbox data area
   */
   for ( i=0 ; i < data_len_code ; i++ )
   {
      mb_data_array[3 - (i&0x3) + (i&0x4)] = ((uchar_ptr)data)[i];
   } /* Endfor */

   data_len_code <<= CAN_CS_DLC_SHIFT;
   /*
   ** Set data up for send
   ** send is automatic when remote message is received
   ** send when message buffer is not active
   */
   code = can_reg_ptr->MB[mailbox_number].CS;
   if( (code & CAN_CS_CODE_MASK ) == CAN_CS_CODE(CAN_TX_MSG_BUFFER_NOT_ACTIVE) )
   {
      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);
      /* Clear the DLC */
      can_reg_ptr->MB[mailbox_number].CS &= ~CAN_CS_DLC_MASK;
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_ONCE) | data_len_code);
   }

   /* Update data to respond to remote request frame */
   if( ((code & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_RESPONED_ONLY)) ||
       ((code & CAN_CS_CODE_MASK) == CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_RESPONED)) )
   {
      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);
      /* Clear the DLC */
      can_reg_ptr->MB[mailbox_number].CS &= ~CAN_CS_DLC_MASK;
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_RESPONED) | data_len_code);
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

   volatile CAN_MemMapPtr  can_reg_ptr;
   uint_32                 read_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   read_ptr = (vuint_32)can_reg_ptr + offset;
   *data_ptr = (uint_32) *(uchar_ptr)read_ptr;

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

   volatile CAN_MemMapPtr  can_reg_ptr;
   vuint_32                write_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
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
   volatile CAN_MemMapPtr  can_reg_ptr;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   *can_status = can_reg_ptr->ESR1;

   return(FLEXCAN_OK);
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
   volatile CAN_MemMapPtr  can_reg_ptr;
   uint_32                 i;
   uchar_ptr               data_array;
   vuint_32                code;
   uchar_ptr               mb_data_array;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (CAN_MCR_MAXMB (0xFFFFFFFF)) )
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   if (8 < data_len_code)
   {
      return (FLEXCAN_DATA_SIZE_ERROR);
   }

   /* check for RTR bit */
   code = can_reg_ptr->MB[mailbox_number].CS;
   if( (code & CAN_CS_RTR_MASK) == CAN_CS_RTR_MASK)
      return FLEXCAN_RTR_NOT_SET;

   /* check if mailbox is set for transmit */
   if( (code & CAN_CS_CODE(CAN_TX_MSG_BUFFER_NOT_ACTIVE)) == CAN_CS_CODE(CAN_TX_MSG_BUFFER_NOT_ACTIVE))
   {
      /* Reset the code */
      can_reg_ptr->MB[mailbox_number].CS &= ~(CAN_CS_CODE_MASK);

      /* store data */
      mb_data_array = (uchar_ptr) &can_reg_ptr->MB[mailbox_number].WORD0;
      data_array = (uchar_ptr) data_ptr;
      for(i=0; i<data_len_code; i++)
      {
         mb_data_array[3 - (i&0x3) + (i&0x4)] = data_array[i];;
      }

      /*
      ** Data frame to be transmitted only as a response to
      ** a remote frame. code 0b1010
      */
      can_reg_ptr->MB[mailbox_number].CS |= (CAN_CS_CODE(CAN_MESSAGE_TRANSMIT_RESPONED) | data_len_code);
    }
    else
        return FLEXCAN_INVALID_MAILBOX;

   return FLEXCAN_OK;

} /* Endbody */
