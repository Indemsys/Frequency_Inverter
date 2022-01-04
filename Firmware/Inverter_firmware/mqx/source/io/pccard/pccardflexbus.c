/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: pccardflexbus.c$
* $Version : 3.8.14.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   PC Card FlexBus device driver functions.
*   Uses memory mapped access to PC Card.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "fio.h"
#include "io.h"
#include "io_prv.h"
#include "pccardflexbus.h"
#include "pccardflexbuspr.h"


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pccard_install
* Returned Value   : uint_32 - a task error code or MQX_OK 
* Comments         : Install the PC Card device driver
*
*END*-------------------------------------------------------------------------*/

uint_32 _io_pccardflexbus_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr                    identifier,

      /* [IN] Pointer to initialization info. */
      PCCARDFLEXBUS_INIT_STRUCT_CPTR  init_ptr
   )
{ /* Body */
  IO_PCCARDFLEXBUS_STRUCT_PTR      info_ptr;

   /* Allocate memory for the PCCARD state structure */
  info_ptr = (IO_PCCARDFLEXBUS_STRUCT_PTR)_mem_alloc_system_zero((uint_32)sizeof(IO_PCCARDFLEXBUS_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (!info_ptr) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
#endif
   
   info_ptr->INIT_DATA = *init_ptr;

   return (_io_dev_install(identifier,
      _io_pccardflexbus_open,
      _io_pccardflexbus_close,
      NULL,
      NULL,
      _io_pccardflexbus_ioctl,
      (pointer)info_ptr));
} /* Endbody */   

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pccardflexbus_open
* Returned Value   : 
* Comments         :
*
*END*-------------------------------------------------------------------------*/

_mqx_int _io_pccardflexbus_open
   (
      /* [IN] the file handle for the device being opened */
      FILE_DEVICE_STRUCT_PTR   fd_ptr,
      
      /* [IN] the remaining portion of the name of the device */
      char _PTR_               open_name_ptr,

      /* [IN] the flags to be used during operation (not used) */
      char _PTR_               flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR          io_dev_ptr = fd_ptr->DEV_PTR;
   IO_PCCARDFLEXBUS_STRUCT_PTR   info_ptr   = (IO_PCCARDFLEXBUS_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   
   /* Initialize mini flexbus */
   _bsp_flexbus_pccard_setup((uint_32)info_ptr->INIT_DATA.PCMCIA_BASE);   

  /* 
   ** Allocate driver a PCMCIA CS for attribute space. This is needed for
   ** functions that access the CIS. Slot A has offset 0 while slot B has 
   ** the next offset PCCARDFLEXBUS_ATTRIB_SIZE to ensure they don't overlap.
   */
   info_ptr->PCMCIA_BASE = (volatile uchar _PTR_)info_ptr->INIT_DATA.PCMCIA_BASE;
   info_ptr->ATTRIB_PTR  = (volatile uchar _PTR_)((uint_32)info_ptr->PCMCIA_BASE | PCCARDFLEXBUS_REGISTER_MASK);

    /* Detect if card is present in slot */
   if (_io_pccardflexbus_card_detect(info_ptr)) 
   {
      _time_get_elapsed(&info_ptr->START_TIME);
      info_ptr->STATE   = PCCARDFLEXBUS_CARD_INSERTED;
      info_ptr->CARD_IN = TRUE;
   } 
   else 
   {
      info_ptr->STATE   = PCCARDFLEXBUS_CARD_REMOVED;
      info_ptr->CARD_IN = FALSE;
   } 

   return MQX_OK;

} /* Endbody */

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pccardflexbus_close
* Returned Value   : 
* Comments         :
*
*END*-------------------------------------------------------------------------*/

_mqx_int _io_pccardflexbus_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR fd_ptr
   )
{ /* Body */
   /* Nothing to be done here */
   return MQX_OK;
} /* Endbody */

/*FUNCTION*---------------------------------------------------------------------
* 
* Function Name    : _io_pccardflexbus_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*-------------------------------------------------------------------------*/

_mqx_int _io_pccardflexbus_ioctl
   (
      /* [IN] the file handle for the device */
      FILE_DEVICE_STRUCT_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      pointer                input_param_ptr
   )
{ /* Body */
   APCCARD_ADDR_INFO_STRUCT_PTR   addr_ptr;
   IO_DEVICE_STRUCT_PTR           io_dev_ptr = fd_ptr->DEV_PTR;
   IO_PCCARDFLEXBUS_STRUCT_PTR    info_ptr = (IO_PCCARDFLEXBUS_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   int_32                         result   = MQX_OK;
   uint_32_ptr                    param_ptr = input_param_ptr;
   
   switch (cmd) {
      case APCCARD_IOCTL_IS_CARD_INSERTED:
         *param_ptr = _io_pccardflexbus_card_detect(info_ptr);
         break;
      case APCCARD_IOCTL_GET_ADDR_SPACE:
         addr_ptr = (APCCARD_ADDR_INFO_STRUCT_PTR)((pointer)param_ptr);
         addr_ptr->ADDRESS = (pointer)(((uint_32)info_ptr->PCMCIA_BASE | PCCARDFLEXBUS_COMMON_MEM_MASK) + addr_ptr->OFFSET);
         break;

      case APCCARD_IOCTL_FREE_ADDR_SPACE:
         break;
      case APCCARD_IOCTL_POWERDOWN_CARD:
         break;
      case APCCARD_IOCTL_CARD_READY:
         *param_ptr = info_ptr->STATE == PCCARDFLEXBUS_ACCESS_ALLOWED;
         break;
      case APCCARD_IOCTL_WAIT_TILL_READY:
         _io_pccardflexbus_card_wait_till_ready(info_ptr);
         break;
      case APCCARD_IOCTL_READ_TUPLE:
         _io_pccardflexbus_read_tuple(info_ptr->ATTRIB_PTR,
            *param_ptr, (uchar_ptr)(param_ptr + 1));
            break;
      case IO_IOCTL_DEVICE_IDENTIFY:
         param_ptr[0] = IO_DEV_TYPE_PHYS_ADV_PCCARD;
         param_ptr[1] = 0;
         param_ptr[2] = 0;
         break;
// folowing io-controls can not be supported by Flexbus CF driver 
//       case APCCARD_IOCTL_SET_CRD_INSERT_CALLBACK:
//       case APCCARD_IOCTL_SET_CRD_REMOVE_CALLBACK:
//       case APCCARD_IOCTL_VCC_ENABLE:
//       case APCCARD_IOCTL_VPP_ENABLE:
//       case APCCARD_IOCTL_SENSE_VOLTAGE:
//       case APCCARD_IOCTL_RESET:
//       case APCCARD_IOCTL_POWERUP_CARD:

      default:
         result = IO_ERROR_INVALID_IOCTL_CMD;
         break;
   } /* Endswitch */

   return result;

} /* Endbody */



/* Private functions */
/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_pccardflexbus_card_detect
* Returned Value   : TRUE is card is in, FALSE otherwise
* Comments         :
*
*END*-------------------------------------------------------------------------*/

boolean _io_pccardflexbus_card_detect    
   (
      /* [IN] The info driver */
      IO_PCCARDFLEXBUS_STRUCT_PTR  info_ptr
   )
{ /* Body */
   uint_16 card_state; 
   
#if (BSP_PCCARDFLEXBUS_WIDTH == 2) 
   card_state = *(volatile uint_16 *)((uint_32)info_ptr->PCMCIA_BASE | PCCARDFLEXBUS_CARD_PRESENT_MASK);
#else
   card_state = *(volatile uchar *)((uint_32)info_ptr->PCMCIA_BASE | PCCARDFLEXBUS_CARD_PRESENT_MASK);
#endif
   
   if(card_state == BSP_PCCARDFLEXBUS_CARD_PRESENT) // card is in the slot
      return (TRUE);
   else if (card_state == BSP_PCCARDFLEXBUS_SLOT_EMPTY) // card is not present in the slot
      return (FALSE);
   else // wrong CPLD code 
      return (FALSE);
} /* Endbody */



/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pccardflexbus_card_wait_till_ready
* Returned Value  : none
* Comments        : waits until the card has had a chance to powerup 
*   completely.
*   
*
*END*---------------------------------------------------------------------*/

void _io_pccardflexbus_card_wait_till_ready
   (
      /* [IN] The info for this slot */
      IO_PCCARDFLEXBUS_STRUCT_PTR  info_ptr
   )
{ /* Body */

   /* Wait until the card is ready */
   if (!info_ptr->CARD_IN) {
      return;
   } /* Endif */

   _io_pccardflexbus_wait(info_ptr, PCCARDFLEXBUS_RESET_ALLOWED_TIME);

} /* Endbody */



/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_pccardflexbus_card_wait
* Returned Value  : none
* Comments        : 
*   
*
*END*---------------------------------------------------------------------*/

void _io_pccardflexbus_wait
   (
      /* [IN] The info for this slot */
      IO_PCCARDFLEXBUS_STRUCT_PTR  info_ptr,

      /* [IN] The time to wait in ms */
      uint_32                    wait_ms
   )
{ /* Body */
   TIME_STRUCT   now, diff;
   uint_32       diff_ms = 0;

   while (diff_ms < wait_ms) {
      _time_get_elapsed(&now);
      _time_diff(&info_ptr->START_TIME, &now, &diff);
      diff_ms = diff.MILLISECONDS + diff.SECONDS * 1000;
   } /* Endwhile */
      
} /* Endbody */



/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _io_apccardflexbus_read_tuple
* Returned Value  : uint_32 - actual size of tuple info data
* Comments        :
*   This function finds and reads the specified tuple
*
*END*---------------------------------------------------------------------*/

uint_32 _io_pccardflexbus_read_tuple
   (
      /* [IN] Pointer to the start of the CIS */
      volatile uchar _PTR_  cis_ptr,

      /* [IN] The tuple to search for */
      uint_32               tuple_code,

      /* [IN] Address to store the tuple data */
      uchar_ptr             dest_ptr

   )
{ /* Body */
   int                        i;
   int                        tuple_size;
   volatile uchar             cis_data;
   volatile uchar _PTR_       cis_end;
   
#if (PSP_ENDIAN == MQX_BIG_ENDIAN) && (BSP_PCCARDFLEXBUS_WIDTH == 2)
   /* if the bus is 16 bits wide and platform is big endian,
      then use odd addresses to read lower half of data bus */
   cis_ptr = (uchar _PTR_)((uint_32)cis_ptr | 0x01);
#endif

   cis_end = cis_ptr + (0x1000<<BSP_PCCARDFLEXBUS_ADDR_SHIFT);
   
   /* search CIS for tuple_code */
   while ((cis_data = *cis_ptr) != tuple_code) {   

      if (cis_data == IO_APCCARD_TUPLE_END_OF_CIS)
        return 0; /* Reached end of CIS */

      /* Handle null tuple */
      if (cis_data == IO_APCCARD_TUPLE_CISTPL_NULL) {
        /* Just advance next position */
        tuple_size = 0;      	
      }
      else {
        /* Get link field */
        cis_ptr += IO_APCCARD_TUPLE_LINK_CIS_OFFSET<<BSP_PCCARDFLEXBUS_ADDR_SHIFT;
        if(cis_ptr >= cis_end)
          return 0; // tuple not found
        tuple_size = *cis_ptr;
      }      

      /* Calculate the location of the next tuple  */
      cis_ptr += ((tuple_size + 1) * 2)<<BSP_PCCARDFLEXBUS_ADDR_SHIFT;
      if(cis_ptr >= cis_end)
        return 0; // tuple not found
        
   } /* Endwhile */

   /* Get link field */
   tuple_size = cis_ptr[IO_APCCARD_TUPLE_LINK_CIS_OFFSET<<BSP_PCCARDFLEXBUS_ADDR_SHIFT];

   /* Advance to tuple data */
   cis_ptr += IO_APCCARD_TUPLE_DATA_CIS_OFFSET<<BSP_PCCARDFLEXBUS_ADDR_SHIFT;

   for (i = 0; i < tuple_size; i++) {
      *dest_ptr = *cis_ptr;
      dest_ptr++;
      cis_ptr += 2<<BSP_PCCARDFLEXBUS_ADDR_SHIFT;
   } /* Endfor */

   return tuple_size;

} /* Endbody */
