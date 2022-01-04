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
* $FileName: int_ctrl_mcf5225.c$
* $Version : 3.7.10.0$
* $Date    : Mar-9-2011$
*
* Comments:
*
*   This file contains definitions for the MFC5225 interrupt controller
*   functions.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : compute_icntl_registers
* Returned Value  : uint_32
* Comments        :
*   Compute which registers to touch for the given interrupt number
*
*END*---------------------------------------------------------------------*/

static boolean compute_icntl_registers
   (
      // [IN] Interrupt number
      PSP_INTERRUPT_TABLE_INDEX irq,

      // [OUT] Interrupt control register
      volatile uchar _PTR_ _PTR_    ictlr,

      // [OUT] Interrupt mask register
      volatile uint_32 _PTR_ _PTR_  maskr,

      // [OUT] Interrupt mask register bit
      uint_32_ptr                   bit,

      // [OUT] Interrupt clear bit 0 pointer to register
      volatile uint_32 _PTR_ _PTR_  clear
   )
{ /* Body */

   VMCF52XX_ICTRL0_STRUCT_PTR ictrl0;
   VMCF52XX_ICTRL1_STRUCT_PTR ictrl1;

   if (irq >= PSP_INT_FIRST_EXTERNAL) {
      uint_32 index = irq - PSP_INT_FIRST_EXTERNAL;

      if (index < 32) {
         ictrl0 = PSP_GET_ICTRL0_BASE();
         *ictlr = &ictrl0->ICR[index];
         *maskr = &ictrl0->ICTRL.IMRL;
         *bit = 1 << index;
         *clear = &ictrl0->ICTRL.IMRL;
         return TRUE;
      } /* Endif */

      if (index < 64) {
         ictrl0 = PSP_GET_ICTRL0_BASE();
         *ictlr = &ictrl0->ICR[index];
         *maskr = &ictrl0->ICTRL.IMRH;
         *bit = 1 << (index - 32);
         *clear = &ictrl0->ICTRL.IMRL;
         return TRUE;
      } /* Endif */

      if (index < 96){
         index = irq - MCF5225_ICTRL1_BASEx;
         ictrl1 = PSP_GET_ICTRL1_BASE();
         *ictlr = &ictrl1->ICR[index];
         *maskr = &ictrl1->ICTRL.IMRL;
         *bit = 1 << (index - 64);
         *clear = &ictrl1->ICTRL.IMRL;
         return TRUE;
      } /* Endif */

      if (index < 128){
         index = irq - MCF5225_ICTRL1_BASEx;
         ictrl1 = PSP_GET_ICTRL1_BASE();
         *ictlr = &ictrl1->ICR[index];
         *maskr = &ictrl1->ICTRL.IMRH;
         *bit = 1 << (index - 96);
         *clear = &ictrl1->ICTRL.IMRL;
         return TRUE;
      } /* Endif */

   } /* Endif */

   return FALSE;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _mcf5225_int_init
* Returned Value  : uint_32
* Comments        :
*   Initialize a specific interrupt in the proper interrupt controller
*
*END*---------------------------------------------------------------------*/

uint_32 _mcf5225_int_init
   (
      // [IN} Interrupt number
      PSP_INTERRUPT_TABLE_INDEX irq,

      // [IN} Interrupt priority level
      _int_level                level,

      // [IN} Interrupt sub-priority level within priority
      _int_priority             sublevel,

      // [IN} Unmask the interrupt now?
      boolean                   unmask
   )
{ /* Body */

   volatile uchar _PTR_   ictrl;
   volatile uint_32 _PTR_ maskr, _PTR_ clear;
   uint_32 bit, mask;
   uint_32 temp;

   if (compute_icntl_registers(irq,&ictrl,&maskr,&bit,&clear)) {
      temp = _psp_get_sr();
      _psp_set_sr(temp | 0x0700);
      mask = *maskr;
      if (!unmask) {
         *maskr = mask | bit;
      } /* Endif */
      *ictrl = ((level & 7) << 3) | (sublevel & 7);
      if (unmask) {
         *maskr = mask & ~bit;
         *clear = *clear & 0xFFFFFFFE;
      } /* Endif */
      _psp_set_sr(temp);
      return MQX_OK;
   } /* Endif */

   return MQX_INVALID_PARAMETER;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _mcf5225_int_unmask
* Returned Value  : uint_32
* Comments        :
*   Clear the mask bit that inhibits the interrupt
*
*END*---------------------------------------------------------------------*/

uint_32 _mcf5225_int_unmask
   (
      // [IN} Interrupt number
      PSP_INTERRUPT_TABLE_INDEX irq
   )
{ /* Body */

   volatile uchar _PTR_   ictrl;
   volatile uint_32 _PTR_ maskr, _PTR_ clear;
   uint_32 bit;

   if (compute_icntl_registers(irq,&ictrl,&maskr,&bit,&clear)) {
      *maskr = *maskr & ~bit;
      *clear = *clear & 0xFFFFFFFE;
      return MQX_OK;
   } /* Endif */

   return MQX_INVALID_PARAMETER;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _mcf5225_int_mask
* Returned Value  : uint_32
* Comments        :
*   Set the mask bit that inhibits the interrupt
*
*END*---------------------------------------------------------------------*/

uint_32 _mcf5225_int_mask
   (
      // [IN} Interrupt number
      PSP_INTERRUPT_TABLE_INDEX irq
   )
{ /* Body */

   volatile uchar _PTR_   ictrl;
   volatile uint_32 _PTR_ maskr, _PTR_ clear;
   uint_32 bit;

   if (compute_icntl_registers(irq,&ictrl,&maskr,&bit,&clear)) {
      *maskr = *maskr | bit;
      return MQX_OK;
   } /* Endif */

   return MQX_INVALID_PARAMETER;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _mcf5225_int_mask_all
* Returned Value  : void
* Comments        :
*   Mask all external interrupts
*
*END*---------------------------------------------------------------------*/

void  _mcf5225_int_mask_all
   (
      void
   )
{ /* Body */

   PSP_GET_ICTRL0_BASE()->ICTRL.IMRL = MCF52XX_ICTR_IMRL_MASKALL;
   PSP_GET_ICTRL1_BASE()->ICTRL.IMRL = MCF52XX_ICTR_IMRL_MASKALL;

} /* Endbody */

/* EOF */
