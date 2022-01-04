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
* $FileName: flexcan_int.c$
* $Version : 3.8.10.0$
* $Date    : Sep-1-2011$
*
* Comments:
*
*   Revision History:
*   Date             Version  Changes
*   ---------        -------  -------
*   Jan.20/04        2.50     Initial version
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <flexcan.h>

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_int_enable
* Returned Value   : uint_32 
* Comments         :
*    This function enables interrupt for requested mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Int_enable
(
   /* [IN] FlexCAN device number */
   uint_8 dev_num,
   /* [IN] mailbox number */
   uint_32 mailbox_number
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }
   
   if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB (0xFFFFFFFF)) ) 
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }
   
   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BUF, mailbox_number);
   if (0 == index)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }
         
   if (_flexcan_int_init (index, FLEXCAN_MESSBUF_INT_LEVEL, FLEXCAN_MESSBUF_INT_SUBLEVEL, TRUE) != MQX_OK)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }
   
   /* IMASK, unmask the message buffer */
   (can_reg_ptr->IMASK)  |= (0x1 << mailbox_number);

   return( FLEXCAN_OK );
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Error_int_enable
* Returned Value   : uint_32 
* Comments         :
*    This function unmasks (enables) error, wake up & Bus off interrupts.
*
*
*END*********************************************************************/
uint_32 FLEXCAN_Error_int_enable
(
   /* [IN] FlexCAN device number */
   uint_8 dev_num
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_ERROR_INT_LEVEL, FLEXCAN_ERROR_INT_SUBLEVEL, TRUE) != MQX_OK)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_BUSOFF_INT_LEVEL, FLEXCAN_BUSOFF_INT_SUBLEVEL, TRUE) != MQX_OK)
   {
      return (FLEXCAN_INT_ENABLE_FAILED);
   }
                        
    /* BOFFMSK = 0x1, ERRMSK = 0x1 */
   can_reg_ptr->CANCTRL |= (FLEXCAN_CANCTRL_BOFFMSK | FLEXCAN_CANCTRL_ERRMSK);

   return ( FLEXCAN_OK );
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_int_disable
* Returned Value   : uint_32 
* Comments         :
*    This function masks (disables) interrupt for requested mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Int_disable
(
   /* [IN] FlexCAN device number */
   uint_8 dev_num,
   /* [IN] mailbox number */
   uint_32 mailbox_number
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB (0xFFFFFFFF)) ) 
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }

   /* Start CR# 1750 */
   can_reg_ptr->IMASK  &= ~(0x1 << mailbox_number);
   /* End CR# 1750 */

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BUF, mailbox_number);
   if (0 == index)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   /* Disable the interrupt */
   if (_flexcan_int_init (index, FLEXCAN_MESSBUF_INT_LEVEL, FLEXCAN_MESSBUF_INT_SUBLEVEL, FALSE) != MQX_OK)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }
      
   return (FLEXCAN_OK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Error_int_disable
* Returned Value   : uint_32 
* Comments         :
*    This function masks (disables) error, wake up & Bus off interrupts 
*
*END*********************************************************************/
uint_32 FLEXCAN_Error_int_disable
(
   /* [IN] FlexCAN device number */
   uint_8 dev_num
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;

   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

    /* BOFFMSK = 0x1, ERRMSK = 0x1 */
   can_reg_ptr->CANCTRL &= ~(FLEXCAN_CANCTRL_BOFFMSK | FLEXCAN_CANCTRL_ERRMSK);

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_ERROR_INT_LEVEL, FLEXCAN_ERROR_INT_SUBLEVEL, FALSE) != MQX_OK)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   if (_flexcan_int_init (index, FLEXCAN_BUSOFF_INT_LEVEL, FLEXCAN_BUSOFF_INT_SUBLEVEL, FALSE) != MQX_OK)
   {
      return (FLEXCAN_INT_DISABLE_FAILED);
   }

   return ( FLEXCAN_OK );
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_ex
* Returned Value   : uint_32 
* Comments         :
*    This function installs interrupt handler for requested mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Install_isr_ex
(
   /* [IN] FlexCAN device number */
   uint_8       dev_num,
   /* [IN] mailbox number */
   uint_32      mailbox_number,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr,
   /* [IN] Interrupt service routine */
   pointer      isr_data
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   uint_32   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   if ( mailbox_number > (FLEXCAN_CANMCR_MAXMB (0xFFFFFFFF)) ) 
   {
      return (FLEXCAN_INVALID_MAILBOX);
   }
   
   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BUF, mailbox_number);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   /* Install ISR */
   result = _int_install_isr (index, isr, (pointer)isr_data);
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }

   return return_code;
}
/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr
* Returned Value   : uint_32 
* Comments         :
*    This function installs interrupt handler for requested mailbox 
*
*END*********************************************************************/
uint_32 FLEXCAN_Install_isr
(
   /* [IN] FlexCAN device number */
   uint_8       dev_num,
   /* [IN] mailbox number */
   uint_32      mailbox_number,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
  
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   return FLEXCAN_Install_isr_ex(dev_num, mailbox_number, isr, (pointer) can_reg_ptr);
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_err_int
* Returned Value   : uint_32 
* Comments         :
*    This function installs interrupt handler for a flexcan error
*
*END*********************************************************************/
uint_32 FLEXCAN_Install_isr_err_int
(
   /* [IN] FlexCAN device number */
   uint_8       dev_num,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint_32   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }
 
   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_ERR, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   result = _int_install_isr (index, isr, (pointer)can_reg_ptr); 
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }

   return return_code;
}
 
/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_boff_int
* Returned Value   : uint_32 
* Comments         :
*    This function installs interrupt handler for a flexcan bus off
*
*END*********************************************************************/
uint_32 FLEXCAN_Install_isr_boff_int
(
   /* [IN] FlexCAN device number */
   uint_8       dev_num,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint_32   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_BOFF, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   result = _int_install_isr (index, isr, (pointer)can_reg_ptr); 
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }

   return return_code;
}

/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_Install_isr_wake_int
* Returned Value   : uint_32 
* Comments         :
*    This function installs interrupt handler for a flexcan wake-up
*
*END*********************************************************************/
uint_32 FLEXCAN_Install_isr_wake_int
(
   /* [IN] FlexCAN device number */
   uint_8       dev_num,
   /* [IN] Interrupt service routine */
   INT_ISR_FPTR isr
)
{
   uint_32   return_code = FLEXCAN_OK;
   INT_ISR_FPTR result;
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile PSP_INTERRUPT_TABLE_INDEX     index;
  
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   index = _bsp_get_flexcan_vector (dev_num, FLEXCAN_INT_WAKEUP, 0);
   if (0 == index)
   {
      return (FLEXCAN_INT_INSTALL_FAILED);
   }

   result = _int_install_isr (index, isr, (pointer)can_reg_ptr); 
   if(result == (INT_ISR_FPTR)NULL)
   {
      return_code = _task_get_error();
   }
    
   return return_code;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _FLEXCAN_ISR
* Returned Value   : uint_32 
* Comments         :
*    This function services FlexCAN interrupt routine. 
*
*END*********************************************************************/
void _FLEXCAN_ISR
(
   /* [IN] FlexCAN base address */
   pointer can_ptr
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   vuint_32                               tmp_reg;
   vuint_32                               temp;
      
   can_reg_ptr = (FLEXCAN_REG_STRUCT_PTR)can_ptr;

   /* get the interrupt flag */
   tmp_reg = (can_reg_ptr->IFLAG & FLEXCAN_IMASK_VALUE);
   // check Tx/Rx interrupt flag and clear the interrupt
   if(tmp_reg)
   {
      /* clear the interrupt and unlock message buffer */
      /* Start CR# 1751 */
      can_reg_ptr->IFLAG |= tmp_reg;
      /* End CR# 1751 */
      temp = can_reg_ptr->TIMER;
   }/* Endif */

   // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
   tmp_reg = (can_reg_ptr->ERRSTAT & FLEXCAN_ALL_INT);
   if(tmp_reg)
   {
      /* Start CR# 1751 */
      can_reg_ptr->ERRSTAT |= tmp_reg;
      /* End CR# 1751 */
   } /* Endif */
      
   return;
}
   
/*FUNCTION****************************************************************
* 
* Function Name    : FLEXCAN_int_status
* Returned Value   : uint_32 
* Comments         :
*    This function gets the FlexCAN interrupt status. 
*
*END*********************************************************************/
uint_32 FLEXCAN_Int_status
(
   /* [IN] FlexCAN device number */
   uint_8 dev_num
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   vuint_32                               tmp_reg;
   
   can_reg_ptr = _bsp_get_flexcan_base_address (dev_num);
   if (NULL == can_reg_ptr)  
   {
      return (FLEXCAN_INVALID_ADDRESS);
   }

   // check Tx/Rx interrupt flag
   tmp_reg = (can_reg_ptr->IFLAG & FLEXCAN_IMASK_VALUE);
   if(tmp_reg)
   {
      return (FLEXCAN_TX_RX_INT);
   } /*Endif*/

   // check error interrupt
   tmp_reg = (can_reg_ptr->ERRSTAT & FLEXCAN_ERROR_INT);
   if(tmp_reg)
   {
      return (FLEXCAN_ERROR_INT);   
   } /*Endif*/

   // check busoff interrupt
   tmp_reg = (can_reg_ptr->ERRSTAT & FLEXCAN_BUSOFF_INT);
   if(tmp_reg)
   {
      return (FLEXCAN_BUSOFF_INT);   
   } /*Endif*/

   // check busoff interrupt
   tmp_reg = (can_reg_ptr->ERRSTAT & FLEXCAN_WAKEUP_INT);
   if(tmp_reg)
   {
      return (FLEXCAN_WAKEUP_INT);   
   } /*Endif*/
   
   return (FLEXCAN_OK);
}
