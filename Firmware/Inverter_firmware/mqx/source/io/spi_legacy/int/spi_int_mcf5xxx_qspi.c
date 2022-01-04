/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: spi_int_mcf5xxx_qspi.c$
* $Version : 3.8.18.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   The file contains low level QSPI interrupt driver functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include "spi.h"
#include "spi_pol_prv.h"
#include "spi_int_prv.h"
#include "spi_mcf5xxx_qspi_prv.h"

extern uint_32 _mcf5xxx_qspi_polled_init(MCF5XXX_QSPI_INIT_STRUCT_PTR, pointer _PTR_, char_ptr);
extern uint_32 _mcf5xxx_qspi_polled_ioctl(MCF5XXX_QSPI_INFO_STRUCT_PTR, uint_32, uint_32_ptr, uint_32);

static uint_32 _mcf5xxx_qspi_int_init(IO_SPI_INT_DEVICE_STRUCT_PTR, char_ptr);
static uint_32 _mcf5xxx_qspi_int_deinit(IO_SPI_INT_DEVICE_STRUCT_PTR, MCF5XXX_QSPI_INFO_STRUCT_PTR);
static uint_32 _mcf5xxx_qspi_int_rx(IO_SPI_INT_DEVICE_STRUCT_PTR, uchar_ptr, int_32);
static uint_32 _mcf5xxx_qspi_int_tx(IO_SPI_INT_DEVICE_STRUCT_PTR, uchar_ptr, int_32);
static uint_32 _mcf5xxx_qspi_int_enable(MCF5XXX_QSPI_INFO_STRUCT_PTR io_info_ptr);
static void _mcf5xxx_qspi_int_isr(pointer parameter);
                       
/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_int_install
* Returned Value   : MQX error code
* Comments         :
*    Install an SPI device.
*
*END*********************************************************************/
uint_32 _mcf5xxx_qspi_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr           identifier,
  
      /* [IN] The I/O init data pointer */
      MCF5XXX_QSPI_INIT_STRUCT_CPTR init_data_ptr
   )
{
   return _io_spi_int_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer,char_ptr))_mcf5xxx_qspi_int_init,
      (uint_32 (_CODE_PTR_)(pointer))_mcf5xxx_qspi_int_enable,
      (uint_32 (_CODE_PTR_)(pointer, pointer))_mcf5xxx_qspi_int_deinit,
      (_mqx_int (_CODE_PTR_)(pointer, char_ptr, int_32))_mcf5xxx_qspi_int_rx,
      (_mqx_int (_CODE_PTR_)(pointer, char_ptr, int_32))_mcf5xxx_qspi_int_tx,
      (_mqx_int (_CODE_PTR_)(pointer, uint_32, _mqx_uint_ptr, _mqx_uint))_mcf5xxx_qspi_polled_ioctl, 
      (pointer)init_data_ptr);
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_int_init  
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the SPI module 
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_int_init
  (
     /* [IN] The address of the device specific information */
     IO_SPI_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

     /* [IN] The rest of the name of the device opened */
     char_ptr                     open_name_ptr
  )
{
   MCF5XXX_QSPI_INFO_STRUCT_PTR   io_info_ptr;
   MCF5XXX_QSPI_INIT_STRUCT_PTR   spi_init_ptr;
   uint_32                        result = SPI_OK;

   /* Initialization as in polled mode */
   spi_init_ptr = (MCF5XXX_QSPI_INIT_STRUCT_PTR)(int_io_dev_ptr->DEV_INIT_DATA_PTR);
   result = _mcf5xxx_qspi_polled_init(spi_init_ptr,
                                     &(int_io_dev_ptr->DEV_INFO_PTR),
                                     open_name_ptr);
   if (result)
   {
      return result;
   }
   
   result = _bsp_get_qspi_vector(spi_init_ptr->CHANNEL);
   if (0 == result)
   {
      _mem_free (int_io_dev_ptr->DEV_INFO_PTR);
      int_io_dev_ptr->DEV_INFO_PTR = NULL;
      return MQX_INVALID_VECTORED_INTERRUPT;
   }
   
   io_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   
   /* Allocate buffers */
   io_info_ptr->RX_BUFFER = (pointer)_mem_alloc_system(spi_init_ptr->RX_BUFFER_SIZE & (~ 1));
   if (NULL == io_info_ptr->RX_BUFFER) 
   {
      _mem_free (int_io_dev_ptr->DEV_INFO_PTR);
      int_io_dev_ptr->DEV_INFO_PTR = NULL;
      return MQX_OUT_OF_MEMORY;
   }
   io_info_ptr->TX_BUFFER = (pointer)_mem_alloc_system(spi_init_ptr->TX_BUFFER_SIZE & (~ 1));
   if (NULL == io_info_ptr->TX_BUFFER) 
   {
      _mem_free (int_io_dev_ptr->DEV_INFO_PTR);
      int_io_dev_ptr->DEV_INFO_PTR = NULL;
      _mem_free (io_info_ptr->RX_BUFFER);
      io_info_ptr->RX_BUFFER = NULL;
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type(io_info_ptr->RX_BUFFER,MEM_TYPE_IO_SPI_IN_BUFFER);       
   _mem_set_type(io_info_ptr->TX_BUFFER,MEM_TYPE_IO_SPI_OUT_BUFFER);       
   
   /* Install ISR */ 
   io_info_ptr->OLD_ISR_DATA = _int_get_isr_data(result);
   io_info_ptr->OLD_ISR = _int_install_isr(result, _mcf5xxx_qspi_int_isr, int_io_dev_ptr);
   
   return SPI_OK; 
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_int_deinit  
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes the SPI module 
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_int_deinit
   (
      /* [IN] The address of the device specific information */
      IO_SPI_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] The address of the channel specific information */
      MCF5XXX_QSPI_INFO_STRUCT_PTR io_info_ptr
   )
{
   VMCF5XXX_QSPI_STRUCT_PTR        qspi_ptr;
   MCF5XXX_QSPI_INIT_STRUCT_PTR    qspi_init_ptr;
   uint_32                         index;
   
   if ((NULL == io_info_ptr) || (NULL == int_io_dev_ptr)) 
   {
      return SPI_ERROR_DEINIT_FAILED;
   }
   
   /* Disable and clear SPI */
   qspi_ptr = io_info_ptr->QSPI_PTR;
   qspi_ptr->QIR = MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
   qspi_ptr->QWR = MCF5XXX_QSPI_QWR_HALT | MCF5XXX_QSPI_QWR_CSIV | MCF5XXX_QSPI_QWR_NEWQP(0) | MCF5XXX_QSPI_QWR_ENDQP(0);
   while (qspi_ptr->QDLYR & MCF5XXX_QSPI_QDLYR_SPE) 
      {};
   qspi_ptr->QIR = MCF5XXX_QSPI_QIR_WCEF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_SPIF;
   
   /* Disable all chip selects */
   for (index = 0; index < MCF5XXX_QSPI_CS_COUNT; index++)
   {
      if ((NULL != io_info_ptr->CS_CALLBACK[index]) && (0 != (io_info_ptr->CS_ACTIVE & (1 << index))))
      {
         io_info_ptr->CS_CALLBACK[index] (1 << index, 1, io_info_ptr->CS_USERDATA[index]);
      }                
   }
   io_info_ptr->CS_ACTIVE = 0;
   
   /* Return original interrupt vector */
   qspi_init_ptr = (MCF5XXX_QSPI_INIT_STRUCT_PTR)(int_io_dev_ptr->DEV_INIT_DATA_PTR);
   _int_install_isr(_bsp_get_qspi_vector(qspi_init_ptr->CHANNEL), io_info_ptr->OLD_ISR, io_info_ptr->OLD_ISR_DATA);

   /* Release buffers */
   _mem_free(int_io_dev_ptr->DEV_INFO_PTR);
   int_io_dev_ptr->DEV_INFO_PTR = NULL;
   _mem_free(io_info_ptr->RX_BUFFER);
   io_info_ptr->RX_BUFFER = NULL;
   _mem_free(io_info_ptr->TX_BUFFER);
   io_info_ptr->TX_BUFFER = NULL;
      
   return SPI_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _mcf5xxx_qspi_int_enable
* Returned Value   : MQX error code
* Comments         :
*    This function enables receive and error interrupt.
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_int_enable
   ( 
   /* [IN] The address of the channel specific information */
      MCF5XXX_QSPI_INFO_STRUCT_PTR io_info_ptr
   )
{

   if (NULL == io_info_ptr)   
   {
      return SPI_ERROR_INVALID_PARAMETER;   
   }

   _bsp_int_init((PSP_INTERRUPT_TABLE_INDEX)_bsp_get_qspi_vector(io_info_ptr->INIT.CHANNEL), BSP_QSPI_INT_LEVEL, BSP_QSPI_INT_SUBLEVEL, TRUE);

   return SPI_OK;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_int_rx
* Returned Value   : Returns the number of bytes received
* Comments         : 
*   Reads the data into provided array.
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_int_rx
   (
      /* [IN] The address of the device specific information */
      IO_SPI_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] The array characters are to be read from */
      uchar_ptr                    buffer,
      
      /* [IN] Number of char's to transmit */
      int_32                       size
   )
{
   MCF5XXX_QSPI_INFO_STRUCT_PTR    io_info_ptr;
   VMCF5XXX_QSPI_STRUCT_PTR        qspi_ptr;
   uint_32                         num, index;
   uchar_ptr                       rx_ptr;

   io_info_ptr  = int_io_dev_ptr->DEV_INFO_PTR;
   qspi_ptr = io_info_ptr->QSPI_PTR;

   /* Check whether new data in rx buffer */
   rx_ptr = (uchar_ptr)(io_info_ptr->RX_BUFFER);
   for (num = 0; num < size; num++) 
   {
      index = io_info_ptr->RX_OUT;
      if (index == io_info_ptr->RX_IN) break;
      *buffer++ = rx_ptr[index++];
      if (index >= io_info_ptr->INIT.RX_BUFFER_SIZE) index = 0;
      io_info_ptr->RX_OUT = index;
   }
   index = size - num;

   if (0 != index)
   {
      /* Not enough data, assert chip selects and enable further transfer, if none */
      io_info_ptr->RX_REQUEST = index;
      if (0 == (qspi_ptr->QIR & MCF5XXX_QSPI_QIR_SPIFE))
      {
         if (io_info_ptr->CS ^ io_info_ptr->CS_ACTIVE)
         {
             for (index = 0; index < MCF5XXX_QSPI_CS_COUNT; index++)
             {
                if ((0 != ((io_info_ptr->CS ^ io_info_ptr->CS_ACTIVE) & (1 << index))) && (NULL != io_info_ptr->CS_CALLBACK[index]))
                {
                   io_info_ptr->CS_CALLBACK[index] (1 << index, (io_info_ptr->CS_ACTIVE >> index) & 1, io_info_ptr->CS_USERDATA[index]);
                }
             }
             io_info_ptr->CS_ACTIVE = io_info_ptr->CS;
         }
         _mcf5xxx_qspi_int_isr (int_io_dev_ptr);
         qspi_ptr->QIR |= MCF5XXX_QSPI_QIR_SPIFE;
      }
   }
   return num;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_int_tx
* Returned Value   : return number of byte transmitted
* Comments         : 
*   Writes the provided data into trasmit buffer
*
*END*********************************************************************/
static uint_32 _mcf5xxx_qspi_int_tx
   (
      /* [IN] The address of the device specific information */
      IO_SPI_INT_DEVICE_STRUCT_PTR int_io_dev_ptr,

      /* [IN] The array to store data */
      uchar_ptr                    buffer,
      
      /* [IN] Number of char's to transmit */
      int_32                       size
   )
{
   MCF5XXX_QSPI_INFO_STRUCT_PTR    io_info_ptr;
   VMCF5XXX_QSPI_STRUCT_PTR        qspi_ptr;
   uint_32                         num, index, tmp;
   uchar_ptr                       tx_ptr;
    
   io_info_ptr = int_io_dev_ptr->DEV_INFO_PTR;
   qspi_ptr = io_info_ptr->QSPI_PTR;

   /* Fill new data into transmit buffer */
   tx_ptr = (uchar_ptr)(io_info_ptr->TX_BUFFER);
   index = io_info_ptr->TX_IN;
   for (num = 0; num < size; num++) 
   {
      tmp = index + 1;
      if (tmp >= io_info_ptr->INIT.TX_BUFFER_SIZE) tmp = 0;
      if (tmp == io_info_ptr->TX_OUT) break;
      tx_ptr[index] = *buffer++;
      index = tmp;
   }
   io_info_ptr->TX_IN = index;
   
   if (0 != num) 
   {
      /* At least one byte to transmit, assert chip selects and enable transfer, if none */
      if (0 == (qspi_ptr->QIR & MCF5XXX_QSPI_QIR_SPIFE))
      {
         if (io_info_ptr->CS ^ io_info_ptr->CS_ACTIVE)
         {
             for (index = 0; index < MCF5XXX_QSPI_CS_COUNT; index++)
             {
                if ((0 != ((io_info_ptr->CS ^ io_info_ptr->CS_ACTIVE) & (1 << index))) && (NULL != io_info_ptr->CS_CALLBACK[index]))
                {
                   io_info_ptr->CS_CALLBACK[index] (1 << index, (io_info_ptr->CS_ACTIVE >> index) & 1, io_info_ptr->CS_USERDATA[index]);
                }
             }
             io_info_ptr->CS_ACTIVE = io_info_ptr->CS;
         }
         _mcf5xxx_qspi_int_isr (int_io_dev_ptr);
         qspi_ptr->QIR |= MCF5XXX_QSPI_QIR_SPIFE;
      }
   }
   return num;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _mcf5xxx_qspi_int_isr
* Returned Value   : SPI interrupt routine
* Comments         : 
*   Reads/writes bytes transmitted from/to buffers.
*
*END*********************************************************************/
static void _mcf5xxx_qspi_int_isr
   (
      /* [IN] The address of the device specific information */
      pointer                   parameter
   )
{
   MCF5XXX_QSPI_INFO_STRUCT_PTR io_info_ptr;
   VMCF5XXX_QSPI_STRUCT_PTR     qspi_ptr;
   uint_32                      state;
   uint_16                      index, tmp;
   boolean                      tx_and_rx;
   uint_16                      reg_tmp;
   
   tx_and_rx = (((IO_SPI_INT_DEVICE_STRUCT_PTR)parameter)->FLAGS & SPI_FLAG_FULL_DUPLEX) != 0;
   io_info_ptr = ((IO_SPI_INT_DEVICE_STRUCT_PTR)parameter)->DEV_INFO_PTR;
   qspi_ptr = (VMCF5XXX_QSPI_STRUCT_PTR)(io_info_ptr->QSPI_PTR);
   
   /* Read interrupt state */
   state = qspi_ptr->QIR;
   qspi_ptr->QIR |= MCF5XXX_QSPI_QIR_SPIF | MCF5XXX_QSPI_QIR_ABRT | MCF5XXX_QSPI_QIR_WCEF;
   io_info_ptr->STATS.INTERRUPTS++;

   /* Receive */
   if (state & MCF5XXX_QSPI_QIR_SPIF) 
   {
      qspi_ptr->QAR = 0x0010;
      tmp = qspi_ptr->QDR;
      if (io_info_ptr->ONTHEWAY > 0) io_info_ptr->ONTHEWAY--;

      /* If request for receive and receive allowed (full duplex or first byte skipped already) */
      if ((tx_and_rx) || ((1 == io_info_ptr->RECEIVING) && (io_info_ptr->RX_REQUEST > 0)))
      {
         /* Actual receive into buffer, if overflow, incoming bytes are thrown away */
         index = io_info_ptr->RX_IN + 1;
         if (index >= io_info_ptr->INIT.RX_BUFFER_SIZE) index = 0;
         if (index != io_info_ptr->RX_OUT) 
         {
            if (MCF5XXX_QSPI_QMR_BITS_GET(qspi_ptr->QMR) != 8)
            {
               io_info_ptr->RX_BUFFER[io_info_ptr->RX_IN] = (uint_8)(tmp >> 8);
               io_info_ptr->RX_IN = index++;
               if (index >= io_info_ptr->INIT.RX_BUFFER_SIZE) index = 0;
            }
            io_info_ptr->RX_BUFFER[io_info_ptr->RX_IN] = (uint_8)tmp;
            io_info_ptr->RX_IN = index;
            io_info_ptr->STATS.RX_PACKETS++;
         } else {
            io_info_ptr->STATS.RX_OVERFLOWS++;
         }
         if (io_info_ptr->RX_REQUEST)
         {
            if (--io_info_ptr->RX_REQUEST == 0) 
            {
               io_info_ptr->RECEIVING = 0;
            }
         }
      } 
      /* Skipping bytes sent before receive */
      if (io_info_ptr->RECEIVING > 1) io_info_ptr->RECEIVING--;
   }

   /* Transmit */
   index = io_info_ptr->RX_IN + 1;
   if (index >= io_info_ptr->INIT.RX_BUFFER_SIZE) index = 0;
   if ((tx_and_rx) || (io_info_ptr->RECEIVING == 0)) 
   {
      if (io_info_ptr->TX_OUT != io_info_ptr->TX_IN)
      {
         // Actual transmit
         tmp = io_info_ptr->TX_BUFFER[io_info_ptr->TX_OUT++];
         if (io_info_ptr->TX_OUT >= io_info_ptr->INIT.TX_BUFFER_SIZE) io_info_ptr->TX_OUT = 0;
         if (MCF5XXX_QSPI_QMR_BITS_GET(qspi_ptr->QMR) != 8)
         {
            tmp = (tmp << 8) | io_info_ptr->TX_BUFFER[io_info_ptr->TX_OUT++];
            if (io_info_ptr->TX_OUT >= io_info_ptr->INIT.TX_BUFFER_SIZE) io_info_ptr->TX_OUT = 0;
         }
         qspi_ptr->QAR = 0x0020;
         qspi_ptr->QDR = (MCF5XXX_QSPI_QDR_CONT | MCF5XXX_QSPI_QDR_BITSE | MCF5XXX_QSPI_QDR_CS(~ io_info_ptr->CS)) & 0xFF00;
         qspi_ptr->QAR = 0x0000;
         qspi_ptr->QDR = tmp;
         reg_tmp = qspi_ptr->QWR & (~ (MCF5XXX_QSPI_QWR_NEWQP(0xF) | MCF5XXX_QSPI_QWR_ENDQP(0xF)));
         qspi_ptr->QWR = reg_tmp | MCF5XXX_QSPI_QWR_NEWQP(0) | MCF5XXX_QSPI_QWR_ENDQP(0);
         qspi_ptr->QDLYR |= MCF5XXX_QSPI_QDLYR_SPE;
         /* If keeping CS low is needed between transfers */
         if (io_info_ptr->CSIV_ACTIVE)
         {
             /* CS inactive low for tranfers longer than 16 frames */
             qspi_ptr->QWR &= (~ MCF5XXX_QSPI_QWR_CSIV);
         }
         io_info_ptr->STATS.TX_PACKETS++;
         io_info_ptr->ONTHEWAY++;            
      } 
      else 
      {
         if ((index != io_info_ptr->RX_OUT) && (io_info_ptr->RX_REQUEST > 0))
         {
            // Dummy transmit, setup receive
            qspi_ptr->QAR = 0x0020;
            qspi_ptr->QDR = (MCF5XXX_QSPI_QDR_CONT | MCF5XXX_QSPI_QDR_BITSE | MCF5XXX_QSPI_QDR_CS(~ io_info_ptr->CS)) & 0xFF00;
            qspi_ptr->QAR = 0x0000;
            qspi_ptr->QDR = 0xFFFF;
            reg_tmp = qspi_ptr->QWR & (~ (MCF5XXX_QSPI_QWR_NEWQP(0xF) | MCF5XXX_QSPI_QWR_ENDQP(0xF)));
            qspi_ptr->QWR = reg_tmp | MCF5XXX_QSPI_QWR_NEWQP(0) | MCF5XXX_QSPI_QWR_ENDQP(0);
            qspi_ptr->QDLYR |= MCF5XXX_QSPI_QDLYR_SPE;
            /* If keeping CS low is needed between transfers */
            if (io_info_ptr->CSIV_ACTIVE)
            {
                /* CS inactive low for tranfers longer than 16 frames */
                qspi_ptr->QWR &= (~ MCF5XXX_QSPI_QWR_CSIV);
            }
            io_info_ptr->ONTHEWAY++;
            io_info_ptr->RECEIVING = io_info_ptr->ONTHEWAY;
         } 
         else
         {
            // No reason for transmit, block transmit interrupt
            qspi_ptr->QIR &= (~ MCF5XXX_QSPI_QIR_SPIFE);
         }
      }
   } 
   else 
   {
      if ((index != io_info_ptr->RX_OUT) && (io_info_ptr->RX_REQUEST > io_info_ptr->ONTHEWAY - (io_info_ptr->RECEIVING - 1)))
      {
         // Dummy transmit for receiving
         qspi_ptr->QAR = 0x0020;
         qspi_ptr->QDR = (MCF5XXX_QSPI_QDR_CONT | MCF5XXX_QSPI_QDR_BITSE | MCF5XXX_QSPI_QDR_CS(~ io_info_ptr->CS)) & 0xFF00;
         qspi_ptr->QAR = 0x0000;
         qspi_ptr->QDR = 0xFFFF;
         reg_tmp = qspi_ptr->QWR & (~ (MCF5XXX_QSPI_QWR_NEWQP(0xF) | MCF5XXX_QSPI_QWR_ENDQP(0xF)));
         qspi_ptr->QWR = reg_tmp | MCF5XXX_QSPI_QWR_NEWQP(0) | MCF5XXX_QSPI_QWR_ENDQP(0);
         qspi_ptr->QDLYR |= MCF5XXX_QSPI_QDLYR_SPE;
         /* If keeping CS low is needed between transfers */
         if (io_info_ptr->CSIV_ACTIVE)
         {
             /* CS inactive low for tranfers longer than 16 frames */
             qspi_ptr->QWR &= (~ MCF5XXX_QSPI_QWR_CSIV);
         }
         io_info_ptr->ONTHEWAY++;
      } 
      else
      {
         // No reason for transmit, block transmit interrupt
         qspi_ptr->QIR &= (~ MCF5XXX_QSPI_QIR_SPIFE);
      }
   }
}
/* EOF */
