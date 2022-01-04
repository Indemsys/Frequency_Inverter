/*HEADER*********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: tad.c$
* $Version : 3.8.10.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   Serial version of Task Aware Debugging.
*
*END************************************************************************/


#include <mqx.h>
#include <psp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwmem.h>
#include <lwmem_prv.h>
#include <fio.h>
#include <string.h>
#include "tad.h"


#define MEM_TYPE_MAX_STRING_LENGTH 32


const char_ptr MQX_mem_type_str[] = 
{
   NULL,
   "SYSTEM_STACK",
   "INTERRUPT_STACK",
   "INTERRUPT_TABLE",
   "TASK",
   "READYQ",
   "PARTITION_COMPONENT",
   "PARTITION",
   "PARTITION_BLOCKS",
   "MESSAGE_COMPONENT",
   "MESSAGE_QUEUES",
   "MESSAGE_POOLS",
   "MESSAGE_POOL_BLOCKS",
   "LWLOG_COMPONENT",
   "LWLOG",
   "IO_DEVICE",
   "MQX_FILE_PTR",
   "TASK_Q",
   "EDS_BUFFER",
   "EVENT_COMPONENT",
   "EVENT_CONNECTION",
   "EVENT",
   "LOG_COMPONENT",
   "LOG",
   "MUTEX_COMPONENT",
   "NAME_COMPONENT",
   "SEMAPHORE_COMPONENT",
   "SEMAPHORE_CONNECTION",
   "SEMAPHORE",
   "TIMER_COMPONENT",
   "TIMER_ENTRY",
   "WATCHDOG_COMPONENT",
   "WATCHDOG_ALARM",
   "PSP_SUPPORT_STRUCT",
   "FP_CONTEXT",
   "INTERRUPT_VECTOR"
};


const char_ptr MQX_IO_mem_type_str[] = 
{
   NULL,
   "IO_PCFLASH",
   "IO_SECTOR_BUFFER",
   "IO_CHAN_STRUCT",
   "IO_ENET_CFG_STRUCT",
   "IO_ENET_BUFFERS",
   "IO_ENET_MCB",
   "IO_ENET_ECB",
   NULL,
   NULL,
   "IO_DUN",
   "IO_DUN_DEV",
   "IO_GPIO_DEV_DATA",
   "IO_GPIO_PIN_MAP",
   "IO_MEM_STRUCT",
   "IO_MEM_DATA",
   "IO_SERIAL_INT_DEVICE_STRUCT",
   "IO_SERIAL_POLLED_DEVICE_STRUCT",
   "IO_SERIAL_IN_QUEUE",
   "IO_SERIAL_OUT_QUEUE",
   "IO_SERIAL_CHARQ",
   "IO_SERIAL_INFO_STRUCT",
   "IO_TFS_DRIVE_STRUCT",
   "IO_SPI_INT_DEVICE_STRUCT",
   "IO_SPI_POLLED_DEVICE_STRUCT",
   "IO_SPI_IN_BUFFER",
   "IO_SPI_OUT_BUFFER",
   "IO_SPI_INFO_STRUCT",
   "IO_FLASHX_RAM_FUNCT",
   "IO_FLASHX_SECTOR_CACHE",
   "IO_FLASHX_SECTOR_MAP",
   "IO_I2C_INT_DEVICE_STRUCT",
   "IO_I2C_POLLED_DEVICE_STRUCT",
   "IO_I2C_IN_BUFFER",
   "IO_I2C_OUT_BUFFER",
   "IO_I2C_INFO_STRUCT",
   "IO_PCB_PTR",
   "IO_PCB_STRUCT",
   "IO_BD_STRUCT",
   "IO_ENET_MAC_CONTEXT_STRUCT",
   "IO_SDCARD_STRUCT",
   "IO_ESDHC_DEVICE_STRUCT",
   "IO_ESDHC_INFO_STRUCT"
};

  
const char_ptr RTCS_mem_type_str[] = 
{
   NULL,
   "RTCS_DATA",
   "ARP_CFG",
   "ARP_ENTRY",
   "DHCP_CLNT_STRUCT",
   "DHCPSRV_OPTIONS_STRUCT",
   "FTPd_SESSION_PTR",
   "FTPd_DATA_BUFFER",
   "ICMP_CFG_STRUCT",
   "ICMP_DATA",
   "IP_CFG_STRUCT",
   "IP_DATA",
   "ICB",
   "IP_IF",
   "SOCKET_CONFIG_STRUCT",
   "RTCS_PARTITION",
   "IO_SOCKET",
   "SOCKET_OWNER_STRUCT",
   "TCP_CFG_STRUCT",
   "TCB",
   "TCP_TX_WINDOW",
   "TCP_RX_WINDOW",
   "TCP_SEND_CLOCK",
   "UDP_CFG_STRUCT",
   "UCB_STRUCT ",
   "UDP_BUFFER",
   "IO_TELNET",
   "SbufNode",
   "Rchunk",
   "FTPc_CONTEXT_STRUCT",
   "DHCPSRV_ADDR_STRUCT",
   "DHCPSRV_CID",
   "DNS_CONTROL_STRUCT",
   "DNS_UDP_MESSAGE",
   "FTPc_RX_BUFFER",
   "FTPc_COMMAND_BUFFER",
   "HOSTENT_STRUCT",
   "TFTP_PACKET",
   "TFTP_TRANS_STRUCT",
   NULL,
   "MEM_TYPE_HTTPD_STRUCT",
   "HTTPD_PARAMS",
   "HTTPD_SESSION_STRUCT",
   "HTTPD_URI",
   "HTTPD_AUTH",
   "HTTPD_LINE",               
};

/*!
 * \brief Gets string representation of given memory type.
 * 
 * \param[in] type_str String representation of memory type.
 * \param[in] mem_type Memory type.
 * 
 * \return String representation of memory type. 
 */ 
static char_ptr mem_type_str 
   (
      char      type_str[MEM_TYPE_MAX_STRING_LENGTH], 
      _mem_type mem_type
   )
{
   char_ptr  str = NULL;
   _mem_type component = mem_type >> MEM_TYPE_COMPONENT_SHIFT;  
   uint_32   index = 0xfff & mem_type; 

   switch (component) 
   {
      case 0:
         if (index < 1000)
         {
            if (index < sizeof (MQX_mem_type_str) / sizeof (char_ptr))
            {
               str = MQX_mem_type_str[index];
            }
         } 
         else 
         {
            index -= 1000;
            if (index < sizeof (MQX_IO_mem_type_str) / sizeof (char_ptr))
            {
               str = MQX_IO_mem_type_str[index];
            }
         }
         break;

      case IO_RTCS_COMPONENT:
         if (index < sizeof (RTCS_mem_type_str) / sizeof (char_ptr))
         {
            str = RTCS_mem_type_str[index];
         }
         break;

      case IO_MFS_COMPONENT:
         str = "MFS";
         break;

      case IO_USB_COMPONENT:
         str = "USB";
         break;

      case IO_SHELL_COMPONENT:
         str = "SHELL";
         break;
      default:
         break;
   }
   if (NULL != str) 
   {
      strncpy (type_str, str, MEM_TYPE_MAX_STRING_LENGTH);
   } 
   else 
   {
      sprintf (type_str, "Unknown mem type 0x%x", mem_type);
   }
   return type_str;
}   

/*!
 * \brief Prints intormation of memory blocks. 
 */ 
void _tad_lightweight_memory_blocks 
   (
       void
   ) 
{
   uint_32                size, next_free;
   char                   owner[8], mem_type[MEM_TYPE_MAX_STRING_LENGTH];
   LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) _lwmem_get_system_pool_id();
   uint_32                start_ptr = (uint_32) mem_pool_ptr->POOL_ALLOC_START_PTR;
   uint_32                end_ptr = (uint_32) mem_pool_ptr->POOL_ALLOC_END_PTR;
   LWMEM_BLOCK_STRUCT_PTR mem_ptr = (LWMEM_BLOCK_STRUCT_PTR) start_ptr;

   printf ("\nLightweight Memory Blocks:\n");
   printf ("Address   Size(hex)   Size(dec)   Owner   Type\n");
   
   while ((uint_32) mem_ptr < end_ptr)
   {
      size = mem_ptr->BLOCKSIZE;
      next_free = (uint_32)mem_ptr->U.NEXTBLOCK;

      if ((next_free >= start_ptr) && (next_free <= end_ptr) || (next_free == 0)) 
      {
         sprintf (owner, "Free");
         mem_type[0] = 0;
      } 
      else 
      {
         mem_type_str (mem_type, mem_ptr->U.S.MEM_TYPE);
         if (mem_ptr->U.S.TASK_NUMBER == TASK_NUMBER_FROM_TASKID(MQX_DEFAULT_TASK_ID))
         {
            sprintf (owner,"System");
         } 
         else 
         {
            sprintf (owner,"0x1%04x", mem_ptr->U.S.TASK_NUMBER);
         }
      }
      printf ("0x%lx   0x%x   %d   %s   %s\n", (uint_32)mem_ptr, size, size, owner, mem_type);
      mem_ptr = (LWMEM_BLOCK_STRUCT_PTR)(((uint_32)mem_ptr) + size); 
      if (((uint_32)mem_ptr < start_ptr) || ((uint_32)mem_ptr > end_ptr)) 
      {
         printf ("Block out of range!\n");
         break;
      }
   }
}

/*!
 * \brief Prints stacks usage. 
 */ 
void _tad_stack_usage 
   (
       void
   ) 
{
#if MQX_MONITOR_STACK && MQX_TD_HAS_STACK_LIMIT
   _mqx_uint_ptr            stack_used, stack_limit;
   _mqx_int                 percent, size;
   TD_STRUCT_PTR            td_ptr;
   TASK_TEMPLATE_STRUCT_PTR template_ptr;
   KERNEL_DATA_STRUCT_PTR   kernel_data_ptr = _mqx_get_kernel_data ();
   char_ptr                 task_name;

   printf ("\nStack Usage:\n");
   printf ("Task   Stack Base   Stack Limit   Stack Used   %% Used   Overflow?\n");


   size = _QUEUE_GET_SIZE(&kernel_data_ptr->TD_LIST);
   td_ptr = (TD_STRUCT_PTR)((uchar_ptr)kernel_data_ptr->TD_LIST.NEXT - FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
   while ((0 != size) && (NULL != td_ptr))
   {
#if MQX_TD_HAS_TASK_TEMPLATE_PTR
      template_ptr = td_ptr->TASK_TEMPLATE_PTR;
      task_name = template_ptr->TASK_NAME;
#else
      char  name[12];
      sprintf(name, "0x%x", td_ptr->TASK_ID);
      task_name = name;
#endif        
      stack_used = td_ptr->STACK_LIMIT;
      stack_used++;
      while (*stack_used == 0x7374616B) 
      {
         stack_used++;
      }
      if ((stack_used == td_ptr->STACK_LIMIT) || (stack_used == (_mqx_uint_ptr)td_ptr->STACK_LIMIT + 1)) 
      {
         percent=100;
      }
      else 
      {
         percent = ((_mqx_uint_ptr)td_ptr->STACK_BASE - stack_used) * 100 / ((_mqx_uint_ptr)td_ptr->STACK_BASE - (_mqx_uint_ptr)td_ptr->STACK_LIMIT);
      }
      printf ("%s   0x%lx   0x%lx   0x%lx   %ld %%   %s\n", task_name, td_ptr->STACK_BASE, td_ptr->STACK_LIMIT, stack_used, percent,percent>=100?"Yes":"No");
        
      size--;
      td_ptr = (TD_STRUCT_PTR)((uchar_ptr)(td_ptr->TD_LIST_INFO.NEXT) - FIELD_OFFSET(TD_STRUCT,TD_LIST_INFO));
   }
    
   stack_used = stack_limit = (_mqx_uint_ptr)(((_mqx_uint)(kernel_data_ptr->INTERRUPT_STACK_PTR)) - kernel_data_ptr->INIT.INTERRUPT_STACK_SIZE);
   stack_used++;
   while (*stack_used == 0x7374616B) 
   {
      stack_used++;
   }
   if ((stack_used == td_ptr->STACK_LIMIT) || (stack_used == (_mqx_uint_ptr)td_ptr->STACK_LIMIT + 1)) 
   {
      printf ("Interrupt   0x%lx   0x%lx   0x%lx   100 %%   Yes", kernel_data_ptr->INTERRUPT_STACK_PTR, stack_limit, stack_used);
   }
   else 
   {
      percent = ((_mqx_uint_ptr)kernel_data_ptr->INTERRUPT_STACK_PTR - stack_used) * 100 / ((_mqx_uint_ptr)kernel_data_ptr->INTERRUPT_STACK_PTR - stack_limit);
      printf ("Interrupt   0x%lx   0x%lx   0x%lx   %ld %%   No", kernel_data_ptr->INTERRUPT_STACK_PTR, stack_limit, stack_used, percent);
   }
   printf ("\n");
#endif
}
