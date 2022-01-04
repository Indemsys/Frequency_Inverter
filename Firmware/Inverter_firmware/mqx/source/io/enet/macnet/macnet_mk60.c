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
* $FileName: macnet_mk60.c$
* $Version : 3.8.8.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   Processor family specific file needed for enet module.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <enet.h>
#include <enetprv.h>
#include <macnet_prv.h>


const uint_32 MACNET_vectors[MACNET_DEVICE_COUNT][ENET_NUM_INTS] = {
    {
        INT_ENET_Transmit,
        INT_ENET_Transmit,
        INT_ENET_Error,
        INT_ENET_Error,
        INT_ENET_Receive,
        INT_ENET_Receive,
        INT_ENET_Error,
        INT_ENET_Error,
        INT_ENET_Error,
        INT_ENET_Error,
        INT_ENET_Error,
        INT_ENET_Error,
#if (MK60_REV_1_0 || MK60_REV_1_1 || MK60_REV_1_2)
        /* e2670: ENET: IEEE 1588 TS_AVAIL interrupt is incorrectly mapped to NVIC vector 94 */
        INT_ENET_Error,
#else
        INT_ENET_1588_Timer,
#endif
        INT_ENET_Error,
        INT_ENET_Error,
        INT_ENET_1588_Timer
    }
};

#if ENETCFG_SUPPORT_PTP
extern uint_64  MACNET_PTP_seconds;
#endif /* ENETCFG_SUPPORT_PTP */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MACNET_get_base_address  
* Returned Value   : Pointer to desired enem device or NULL if not present
* Comments         :
*    This function returns pointer to base address of address space of the 
*    desired enet device. Returns NULL otherwise.
*
*END*----------------------------------------------------------------------*/

pointer MACNET_get_base_address(uint_32 device) {
    pointer addr = NULL;
    
    switch (device) {
    case 0:
        addr = (pointer)ENET_BASE_PTR;
        break;
    }
    
    return addr;
}


 
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MACNET_io_init
* Returned Value   : none
* Comments         :
*    This function performs BSP-specific initialization related to ENET
*
*END*----------------------------------------------------------------------*/

void MACNET_io_init( uint_32	device )
{

   if (device >= MACNET_DEVICE_COUNT) 
      return;

   _bsp_enet_io_init(device);
}

#if ENETCFG_SUPPORT_PTP
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MACNET_ptp_install_ts_avail_isr
* Returned Value   : none
* Comments         :
*    This function initializes the TS_AVAIL interrupt
*
*END*----------------------------------------------------------------------*/

boolean MACNET_ptp_install_ts_avail_isr( ENET_CONTEXT_STRUCT_PTR enet_ptr, MACNET_INIT_STRUCT const * enet_init_ptr)
{
   return (MACNET_install_isr(enet_ptr, 4, ENET_INT_TS_AVAIL, MACNET_ptp_store_txstamp, enet_init_ptr->ERX_LEVEL,enet_init_ptr->ERX_SUBLEVEL)) ;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MACNET_ptp_install_ts_timer_isr
* Returned Value   : none
* Comments         :
*    This function initializes the TS_TIMER interrupt
*
*END*----------------------------------------------------------------------*/

boolean MACNET_ptp_install_ts_timer_isr( ENET_CONTEXT_STRUCT_PTR enet_ptr, MACNET_INIT_STRUCT const * enet_init_ptr)
{
   return (MACNET_install_isr(enet_ptr, 5, ENET_INT_TS_TIMER, MACNET_ptp_increment_seconds, enet_init_ptr->ETX_LEVEL - 1, 0)) ;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_ptp_increment_seconds
*  Returned Value :  
*  Comments       : 
*    This function is the TS_TIMER interrupt service routine
*
*END*-----------------------------------------------------------------*/
void MACNET_ptp_increment_seconds
   (
         /* [IN] the Ethernet state structure */
      pointer  enet
   )
{ /* Body */
   ENET_CONTEXT_STRUCT_PTR    enet_ptr = (ENET_CONTEXT_STRUCT_PTR)enet;
   MACNET_CONTEXT_STRUCT_PTR  macnet_context_ptr = (MACNET_CONTEXT_STRUCT_PTR) enet_ptr->MAC_CONTEXT_PTR;
   ENET_MemMapPtr             macnet_ptr= macnet_context_ptr->MACNET_ADDRESS;

   if (macnet_ptr == NULL) return;

   /* Load the output compare buffer */
   macnet_ptr->CHANNEL[MACNET_PTP_TIMER].TCCR = MACNET_1588_ATPER_VALUE - MACNET_1588_CLOCK_INC;
   
   /* Clear ENET_TCSRn[TF] flag by writing a logic one */
   macnet_ptr->CHANNEL[MACNET_PTP_TIMER].TCSR |= ENET_TCSR_TF_MASK;
   
   /* Clear ENET_TGSRn respective channel flag */
   macnet_ptr->TGSR |= 1<<MACNET_PTP_TIMER;
   
   /* Increment seconds */
   MACNET_PTP_seconds++;
}

#endif /* ENETCFG_SUPPORT_PTP */
