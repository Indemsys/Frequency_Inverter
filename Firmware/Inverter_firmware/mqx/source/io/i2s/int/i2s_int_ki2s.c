/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: i2s_int_ki2s.c$
* $Version : 3.8.8.0$
* $Date    : Aug-20-2012$
*
* Comments:
*
*   This file contains low level functions for the I2S interrupt device driver
*   for Kinetis family.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "i2s.h"
#include "i2s_int_prv.h"
#include "i2s_ki2s_prv.h"

uint_32 _ki2s_int_init (IO_I2S_DEVICE_STRUCT_PTR, char_ptr);
uint_32 _ki2s_int_deinit (IO_I2S_DEVICE_STRUCT_PTR, KI2S_INFO_STRUCT_PTR);
uint_32 _ki2s_int_rx (IO_I2S_DEVICE_STRUCT_PTR, char_ptr, uint_32);
uint_32 _ki2s_int_tx (IO_I2S_DEVICE_STRUCT_PTR, char_ptr, uint_32);
uint_32 _ki2s_int_ioctl (KI2S_INFO_STRUCT_PTR io_info_ptr, uint_32 cmd, pointer param_ptr);
static void _ki2s_int_isr (pointer);
static uint_32 _ki2s_format_in_data(char_ptr input, AUDIO_DATA_FORMAT_PTR format);
static void _ki2s_format_out_data(uint_32 input, char_ptr output, AUDIO_DATA_FORMAT_PTR format);
static uint_32 _ki2s_sine_wave(uint_32 fs, uint_32 freq, int_16_ptr* output);
static _mqx_int _ki2s_hw_init(KI2S_INIT_STRUCT_PTR, I2S_MemMapPtr);
static uint_32 _ki2s_get_mclk_value(KI2S_INFO_STRUCT_PTR io_info_ptr);
static _mqx_int _ki2s_set_mclk_value(KI2S_INFO_STRUCT_PTR io_info_ptr, uint_32 frequency);
static _mqx_int _ki2s_set_fs_value(KI2S_INFO_STRUCT_PTR io_info_ptr, uint_32 frequency);
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_int_install
* Returned Value   : MQX error code
* Comments         :
*    Install an I2S device.
*
*END*********************************************************************/

uint_32 _ki2s_int_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr           identifier,

      /* [IN] The I/O init data pointer */
      KI2S_INIT_STRUCT_CPTR  init_data_ptr
   )
{ /* Body */

   return _io_i2s_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer, char_ptr))_ki2s_int_init,
      (uint_32 (_CODE_PTR_)(pointer, pointer))_ki2s_int_deinit,
      (int_32 (_CODE_PTR_)(pointer, char_ptr, int_32))_ki2s_int_rx,
      (int_32 (_CODE_PTR_)(pointer, char_ptr, int_32))_ki2s_int_tx,
      (int_32 (_CODE_PTR_)(pointer, uint_32, uint_32_ptr))_ki2s_int_ioctl,
      (pointer)init_data_ptr);

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_int_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an I2S device.
*
*END*********************************************************************/

uint_32 _ki2s_int_init
   (
      /* [IN] Initialization information for the device being opened */
      IO_I2S_DEVICE_STRUCT_PTR  io_i2s_dev_ptr,

      /* [IN] The rest of the name of the device opened */
      char_ptr                      open_name_ptr

   )
{ /* Body */
   I2S_MemMapPtr        i2s_ptr;
   KI2S_INIT_STRUCT_PTR i2s_init_ptr;
   KI2S_INFO_STRUCT_PTR io_info_ptr;
   uint_8               channel, mode, data_bits, clk_source, endianity, aligment, bits, size, channels;
   _mqx_uint            vector, buffer_size;
   _mqx_uint            result = 0;

   /*Variables initialization*/
   i2s_init_ptr = (KI2S_INIT_STRUCT_PTR)(io_i2s_dev_ptr->DEV_INIT_DATA_PTR);
   if (i2s_init_ptr == NULL)
   {
      #ifdef I2S_DEBUG
       printf("_i2s_init: Error - Invalid init struct pointer.\n");
      #endif
      return(I2S_ERROR_INVALID_PARAMETER);
   }
   else
   {
      channel = i2s_init_ptr->CHANNEL;
      mode = i2s_init_ptr->MODE;
      data_bits = i2s_init_ptr->DATA_BITS;
      clk_source = i2s_init_ptr->CLOCK_SOURCE;
      buffer_size = i2s_init_ptr->BUFFER_SIZE;
      endianity = i2s_init_ptr->IO_FORMAT->ENDIAN;
      aligment = i2s_init_ptr->IO_FORMAT->ALIGNMENT;
      bits = i2s_init_ptr->IO_FORMAT->BITS;
      size = i2s_init_ptr->IO_FORMAT->SIZE;
      channels = i2s_init_ptr->IO_FORMAT->CHANNELS;
   }

   if ((data_bits < 8) || (data_bits > 24) || (data_bits == 14) || (data_bits % 2))
   {
      #ifdef I2S_DEBUG
       printf("_i2s_init: Error - Unsupported data word length.\n");
      #endif
      return (I2S_ERROR_WORD_LENGTH_UNSUPPORTED);
   }

   if ((channel > 1) || (channel < 0))
   {
      #ifdef I2S_DEBUG
       printf("_i2s_init: Error - Invalid I2S channel.\n");
      #endif
      return(I2S_ERROR_CHANNEL_INVALID);
   }

   if (mode > I2S_MODE_SLAVE)
   {
      #ifdef I2S_DEBUG
       printf("_i2s_init: Error - Invalid I2S mode.\n");
      #endif
      return(I2S_ERROR_MODE_INVALID);
   }
   if (clk_source > I2S_CLK_EXT)
   {
      #ifdef I2S_DEBUG
       printf("_i2s_init: Error - Invalid I2S clock source.\n");
      #endif
      return(I2S_ERROR_CLK_INVALID);
   }
   if (buffer_size < 2)
   {
      #ifdef I2S_DEBUG
       printf("_i2s_init: Error - I2S transmit buffer size too small.\n");
      #endif
      return(I2S_ERROR_BUFFER_SMALL);
   }
   if ((aligment > AUDIO_ALIGNMENT_LEFT) || (bits < AUDIO_BIT_SIZE_MIN) ||
      (bits > AUDIO_BIT_SIZE_MAX) || (endianity > AUDIO_LITTLE_ENDIAN) ||
      (size * I2S_CHAR_BIT < bits) || (channels > 2) || (channels == 0))
   {
      #ifdef I2S_DEBUG
        printf("_i2s_init: Error - Invalid I/O data format.\n");
      #endif
      return(AUDIO_ERROR_INVALID_IO_FORMAT);
   }
   i2s_ptr = _bsp_get_i2s_base_address(channel);
   io_info_ptr = (KI2S_INFO_STRUCT_PTR) _mem_alloc_system_zero ((uint_32)sizeof (KI2S_INFO_STRUCT));
   if (NULL == io_info_ptr)
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type (io_info_ptr, MEM_TYPE_IO_I2S_INFO_STRUCT);

   /* I2S board specific IO pins initialization */
   if (_bsp_i2s_io_init(channel) != MQX_OK)
   {
     #ifdef I2S_DEBUG
      printf("_i2s_init: Error - I/O init failed.\n");
     #endif
     return(I2S_ERROR_IO_INIT_FAILED);
   }
   /* I2S board specific registers initialization */
   _ki2s_hw_init(i2s_init_ptr, i2s_ptr);

   /*Info struct setup*/
   io_info_ptr->INIT = *i2s_init_ptr;
   io_info_ptr->I2S_PTR = i2s_ptr;
   io_info_ptr->MODE = i2s_init_ptr->MODE;
   io_info_ptr->DATA_BITS = i2s_init_ptr->DATA_BITS;
   io_info_ptr->CLOCK_SOURCE = i2s_init_ptr->CLOCK_SOURCE;
   io_info_ptr->IO_FORMAT.ENDIAN = endianity;
   io_info_ptr->IO_FORMAT.BITS = bits;
   io_info_ptr->IO_FORMAT.ALIGNMENT = aligment;
   io_info_ptr->IO_FORMAT.SIZE = size;
   io_info_ptr->IO_FORMAT.CHANNELS = channels;
   
   if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
   {
      io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr);
      io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / 4;
      /* DIV2 = 1 in default and 2 is inner division */
      io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * io_info_ptr->IO_FORMAT.CHANNELS);
   }
   else
   {
      io_info_ptr->MCLK_FREQ = 0;
      io_info_ptr->BCLK_FREQ = 0;
      io_info_ptr->FS_FREQ = 0;
   }
   io_info_ptr->MCLK_DIV = SSIDIV_MIN;
   io_info_ptr->BCLK_DIV = 4;
   io_info_ptr->TX_DUMMY = i2s_init_ptr->TX_DUMMY;
   io_info_ptr->ONTHEWAY = FALSE;
   io_info_ptr->DATA_L = NULL;
   io_info_ptr->DATA_R = NULL;
   /* Set up interrupts mask */
   if (channels == 1)
   {
       io_info_ptr->TX_INT_MASK = (I2S_IER_TFE0EN_MASK);
       io_info_ptr->RX_INT_MASK = (I2S_IER_RFF0EN_MASK);
   }
   else
   {
       io_info_ptr->TX_INT_MASK = (I2S_IER_TFE0EN_MASK | I2S_IER_TFE1EN_MASK);
       io_info_ptr->RX_INT_MASK = (I2S_IER_RFF0EN_MASK | I2S_IER_RFF1EN_MASK);
   }
   result = _lwsem_create(&(io_info_ptr->EVENT_IO_FINISHED), 0);
   if (result != MQX_OK) {
      #ifdef I2S_DEBUG
      printf("i2s_init - Error: Unable to create event_int_done.\n");
      #endif
      _task_set_error(result);
   }
   io_info_ptr->CLK_ALWAYS_ON = TRUE;
   io_info_ptr->FIRST_IO = TRUE;

   /* Buffers allocation */
   io_info_ptr->BUFFER_L.DATA = (uint_32_ptr) _mem_alloc_system(buffer_size * sizeof(uint_32));
   if (io_info_ptr->BUFFER_L.DATA == NULL)
   {
      #ifdef I2S_DEBUG
      printf("i2s_init - Error: Unable to allocate buffer memory.\n");
      #endif
      return MQX_OUT_OF_MEMORY;
   }
   io_info_ptr->BUFFER_L.SPACE = buffer_size;
   io_info_ptr->BUFFER_L.SIZE = buffer_size;

   io_info_ptr->BUFFER_R.DATA = (uint_32_ptr) _mem_alloc_system(buffer_size * sizeof(uint_32));
   if (io_info_ptr->BUFFER_R.DATA == NULL)
   {
      uint_32 errcode = _mem_free(io_info_ptr->BUFFER_L.DATA);
      if (errcode != MQX_OK)
      {
         return errcode;
      }
      io_info_ptr->BUFFER_L.DATA = NULL;
      io_info_ptr->BUFFER_L.SPACE = 0;
      io_info_ptr->BUFFER_L.SIZE = 0;
      #ifdef I2S_DEBUG
      printf("i2s_init - Error: Unable to allocate buffer memory.\n");
      #endif
      return MQX_OUT_OF_MEMORY;
   }
   io_info_ptr->BUFFER_R.SPACE = buffer_size;
   io_info_ptr->BUFFER_R.SIZE = buffer_size;

   _mem_set_type (io_info_ptr->BUFFER_L.DATA, MEM_TYPE_IO_I2S_LEFT_BUFFER);
   _mem_set_type (io_info_ptr->BUFFER_R.DATA, MEM_TYPE_IO_I2S_RIGHT_BUFFER);

   io_i2s_dev_ptr->DEV_INFO_PTR = io_info_ptr;

   /*Interrupt vector setup*/
   vector = _bsp_get_i2s_vector(channel);
   io_info_ptr->OLD_ISR_DATA = _int_get_isr_data (vector);
   io_info_ptr->OLD_ISR = _int_install_isr (vector, _ki2s_int_isr, io_info_ptr);
   _bsp_int_init((IRQInterruptIndex)_bsp_get_i2s_vector(channel), io_info_ptr->INIT.LEVEL, 0, TRUE);

   return(I2S_OK);
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_int_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an I2S device.
*
*END*********************************************************************/

uint_32 _ki2s_int_deinit
   (
      /* [IN] the initialization information for the device being opened */
      IO_I2S_DEVICE_STRUCT_PTR  io_i2s_dev_ptr,

      /* [IN] the address of the device specific information */
      KI2S_INFO_STRUCT_PTR   io_info_ptr
   )
{ /* Body */
   I2S_MemMapPtr          i2s_ptr;
   KI2S_INIT_STRUCT_PTR   i2s_init_ptr;
   _mqx_uint              errcode;
   _mqx_uint              result = 0;

   if ((NULL == io_info_ptr) || (NULL == io_i2s_dev_ptr))
   {
      return I2S_ERROR_INVALID_PARAMETER;
   }

   i2s_ptr = io_info_ptr->I2S_PTR;

   /*SSI disable*/
   i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK);

   /* Clear the SSI events */
   i2s_ptr->ISR = 0x00;

   /* Install original vectors */
   i2s_init_ptr = (KI2S_INIT_STRUCT_PTR)(io_i2s_dev_ptr->DEV_INIT_DATA_PTR);
   _bsp_int_init((IRQInterruptIndex)_bsp_get_i2s_vector(i2s_init_ptr->CHANNEL), i2s_init_ptr->LEVEL, 0, TRUE);
   _int_install_isr (_bsp_get_i2s_vector (i2s_init_ptr->CHANNEL), io_info_ptr->OLD_ISR, io_info_ptr->OLD_ISR_DATA);
   /* Destroy event */
   result = _lwsem_destroy(&(io_info_ptr->EVENT_IO_FINISHED));
   if (result != MQX_OK)
   {
      #ifdef I2S_DEBUG
      printf("i2s_rx - Error: unable to destroy event_int_done.\n");
      #endif
      _task_set_error(result);
   }
   /* Free buffers and info struct */
   errcode = _mem_free (io_info_ptr->BUFFER_L.DATA);
   if (errcode != MQX_OK)
   {
      return errcode;
   }
   io_info_ptr->BUFFER_L.IN = 0;
   io_info_ptr->BUFFER_L.OUT = 0;
   io_info_ptr->BUFFER_L.DATA = NULL;
   io_info_ptr->BUFFER_L.SPACE = 0;
   io_info_ptr->BUFFER_L.SIZE = 0;

   errcode = _mem_free (io_info_ptr->BUFFER_R.DATA);
   if (errcode != MQX_OK)
   {
      return errcode;
   }
   io_info_ptr->BUFFER_R.IN = 0;
   io_info_ptr->BUFFER_R.OUT = 0;
   io_info_ptr->BUFFER_R.DATA = NULL;
   io_info_ptr->BUFFER_R.SPACE = 0;
   io_info_ptr->BUFFER_R.SIZE = 0;

   if (io_info_ptr->TX_DUMMY)
   {
      errcode = _mem_free (io_info_ptr->DUMMY.DATA);
      if (errcode != MQX_OK)
      {
         return errcode;
      }
      io_info_ptr->DUMMY.DATA = NULL;
   }

   errcode = _mem_free (io_i2s_dev_ptr->DEV_INFO_PTR);
   if (errcode != MQX_OK)
   {
      return errcode;
   }
   io_i2s_dev_ptr->DEV_INFO_PTR = NULL;
   return I2S_OK;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_int_rx
* Returned Value   : number of bytes read
* Comments         :
*   Returns the number of bytes received.
*   Reads the data into provided array when data is available.
*
*END*********************************************************************/

uint_32 _ki2s_int_rx
   (
      /* [IN] the address of the device specific information */
      IO_I2S_DEVICE_STRUCT_PTR   io_i2s_dev_ptr,

      /* [IN] The array to copy data into */
      char_ptr                      buffer,

    /* [IN] number of bytes to read */
      uint_32                       length
   )
{ /* Body */
   KI2S_INFO_STRUCT_PTR             io_info_ptr   = io_i2s_dev_ptr->DEV_INFO_PTR;
   I2S_MemMapPtr                    i2s_ptr = io_info_ptr->I2S_PTR;
   KI2S_BUFFER_PTR                  buffer_l = &(io_info_ptr->BUFFER_L);
   KI2S_BUFFER_PTR                  buffer_r = &(io_info_ptr->BUFFER_R);
   I2S_STATISTICS_STRUCT_PRV_PTR    stats = &(io_info_ptr->STATS);
   AUDIO_DATA_FORMAT_PTR            io_format = &(io_info_ptr->IO_FORMAT);
   LWSEM_STRUCT_PTR                 event_int_done = &(io_info_ptr->EVENT_IO_FINISHED);
   _mqx_uint                        result = MQX_OK;
   uint_32                          i = 0;

   /* disable interrupts */
   i2s_ptr->IER &= ~(
      I2S_IER_TUE0EN_MASK | /* Transmit underrun error 0*/
      I2S_IER_TFE0EN_MASK | /* Transmit FIFO0 empty */
      I2S_IER_TUE1EN_MASK | /* Transmit underrun error 1*/
      I2S_IER_TFE1EN_MASK   /* Transmit FIFO1 empty */
      );

   io_info_ptr->IO_MODE = I2S_IO_READ;
   /* Init stats and buffers to default values */
    stats->PACKETS_REQUESTED_L += length/(io_format->CHANNELS * io_format->SIZE);
    switch (io_format->CHANNELS)
   {
      case 1:
         io_info_ptr->DATA_L = buffer;
         io_info_ptr->DATA_R = buffer;
         /* One channel only - enable only interrupts of FIFO0 */
         i2s_ptr->IER |= (I2S_IER_RFF0EN_MASK | I2S_IER_ROE0EN_MASK);
      break;

      default:
          io_info_ptr->DATA_L = buffer;
          io_info_ptr->DATA_R = buffer + io_format->SIZE;
          i2s_ptr->IER |= (
              I2S_IER_ROE0EN_MASK | /* Receive overrun error 0*/
              I2S_IER_RFF0EN_MASK | /* Receive FIFO0 full */
              I2S_IER_ROE1EN_MASK | /* Receive overrun error 1*/
              I2S_IER_RFF1EN_MASK   /* Receive FIFO1 full */
              );
          stats->PACKETS_REQUESTED_R += length/(io_format->CHANNELS * io_format->SIZE);
      break;
   }

   io_info_ptr->ONTHEWAY = TRUE;

   while(io_info_ptr->ONTHEWAY)
   {
      if (io_info_ptr->FIRST_IO) /* enable receiver and interrupts in a case of first run to get some data */
      {
         io_info_ptr->FIRST_IO = FALSE;
         i2s_ptr->IER |= (I2S_IER_RIE_MASK); /* Receive interrupts enable */
         i2s_ptr->CR |= (I2S_CR_RE_MASK); /* Enable receiver */
      }
      /* Wait for a receive completition */
      result = _lwsem_wait(event_int_done);
      if (result != MQX_OK)
      {
         #ifdef I2S_DEBUG
          printf("i2s_rx - Error: Wait for event_int_done failed.\n");
         #endif
         _task_set_error(result);
      }
      /* All data received, disable receiver */
      if ((stats->PACKETS_QUEUED_L >= stats->PACKETS_REQUESTED_L) &&
          (stats->PACKETS_QUEUED_R >= stats->PACKETS_REQUESTED_R))
      {
          io_info_ptr->ONTHEWAY = FALSE;
      }
      while ((stats->PACKETS_PROCESSED_L < stats->PACKETS_QUEUED_L) &&
             (stats->PACKETS_PROCESSED_L < stats->PACKETS_REQUESTED_L))
      {
         /* Critical Section - disable interrupts with access to buffer */
         i2s_ptr->IER &= ~(io_info_ptr->RX_INT_MASK);
         _ki2s_format_out_data(buffer_l->DATA[buffer_l->OUT], io_info_ptr->DATA_L, io_format);
         stats->PACKETS_PROCESSED_L++;
         buffer_l->SPACE++;
         (++buffer_l->OUT >= buffer_l->SIZE) ? (buffer_l->OUT = 0) : (0);
         /* End of critical section*/
         i2s_ptr->IER |= (io_info_ptr->RX_INT_MASK);
         io_info_ptr->DATA_L += io_format->CHANNELS * io_format->SIZE;
      }

      while ((stats->PACKETS_PROCESSED_R < stats->PACKETS_QUEUED_R) &&
             (stats->PACKETS_PROCESSED_R < stats->PACKETS_REQUESTED_R))
      {
         /* Critical Section - disable interrupts with access to buffer */
         i2s_ptr->IER &= ~(io_info_ptr->RX_INT_MASK);
         *(io_info_ptr->DATA_R) = buffer_r->DATA[buffer_r->OUT];
         _ki2s_format_out_data(buffer_r->DATA[buffer_r->OUT], io_info_ptr->DATA_R, io_format);
         stats->PACKETS_PROCESSED_R++;
         buffer_r->SPACE++;
         (++buffer_r->OUT >= buffer_r->SIZE) ? (buffer_r->OUT = 0) : (0);
         /* End of critical section*/
         i2s_ptr->IER |= (io_info_ptr->RX_INT_MASK);
         io_info_ptr->DATA_R += io_format->CHANNELS * io_format->SIZE;
      }
   }
   return(length);
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_int_tx
* Returned Value   : number of bytes written
* Comments         :
*   Returns the number of bytes written.
*   Writes the data provided into transmition buffer if available.
*
*END*********************************************************************/

uint_32 _ki2s_int_tx
   (
      /* [IN] the address of the device specific information */
      IO_I2S_DEVICE_STRUCT_PTR    io_i2s_dev_ptr,

      /* [IN] The array to be read from */
      char_ptr                       buffer,

      /* [IN] number of packets to output */
      uint_32                         length
   )
{ /* Body */
   KI2S_INFO_STRUCT_PTR          io_info_ptr = io_i2s_dev_ptr->DEV_INFO_PTR;
   I2S_MemMapPtr                 i2s_ptr = io_info_ptr->I2S_PTR;
   KI2S_BUFFER_PTR               buffer_l = &(io_info_ptr->BUFFER_L);
   KI2S_BUFFER_PTR               buffer_r = &(io_info_ptr->BUFFER_R);
   I2S_STATISTICS_STRUCT_PRV_PTR stats = &(io_info_ptr->STATS);
   AUDIO_DATA_FORMAT_PTR         io_format = &(io_info_ptr->IO_FORMAT);
   LWSEM_STRUCT_PTR              event_int_done = &(io_info_ptr->EVENT_IO_FINISHED);
   _mqx_uint                     result = MQX_OK;

   

   i2s_ptr->IER &= ~(
         I2S_IER_ROE0EN_MASK | /* Receive overrun error 0*/
         I2S_IER_RFF0EN_MASK | /* Receive FIFO0 full */
         I2S_IER_ROE1EN_MASK | /* Receive overrun error 0*/
         I2S_IER_RFF1EN_MASK   /* Receive FIFO0 full */
         );
   
   stats->PACKETS_REQUESTED_L += length/(io_format->CHANNELS * io_format->SIZE);
   
   if ((buffer == NULL) && !(io_info_ptr->TX_DUMMY))
   {
      _task_set_error(I2S_ERROR_INVALID_PARAMETER);
      return (0);
   }
   else
   {
      switch (io_format->CHANNELS)
      {
         case 1:
            io_info_ptr->DATA_L = buffer;
            io_info_ptr->DATA_R = buffer;
            /* One channel only - enable only interrupts of FIFO0 */
            i2s_ptr->IER |= (I2S_IER_TUE0EN_MASK | I2S_IER_TFE0EN_MASK);
         break;

         default:
            io_info_ptr->DATA_L = buffer;
            io_info_ptr->DATA_R = buffer + io_format->SIZE;
            i2s_ptr->IER |= (
               I2S_IER_TUE0EN_MASK | /* Transmit underrun error 0*/
               I2S_IER_TFE0EN_MASK | /* Transmit FIFO0 empty */
               I2S_IER_TUE1EN_MASK | /* Transmit underrun error 0*/
               I2S_IER_TFE1EN_MASK   /* Transmit FIFO0 empty */
               );
            stats->PACKETS_REQUESTED_R += length/(io_format->CHANNELS * io_format->SIZE);
         break;
      }
   }

   io_info_ptr->IO_MODE = I2S_IO_WRITE;
   
   if (io_info_ptr->TX_DUMMY)
   {
      stats->PACKETS_QUEUED_L = stats->PACKETS_REQUESTED_L;
      stats->PACKETS_QUEUED_R = stats->PACKETS_REQUESTED_R;
   }

   io_info_ptr->ONTHEWAY = TRUE;
    
   while(io_info_ptr->ONTHEWAY)
   {
      if (!(io_info_ptr->TX_DUMMY))
      {
         /* Copy data from input to left channel buffer */
         while ((buffer_l->SPACE > 0) && (stats->PACKETS_QUEUED_L < stats->PACKETS_REQUESTED_L))
         {
            /* buffer access, critical section - disable interrupts with buffer access */
            i2s_ptr->IER &= ~(io_info_ptr->TX_INT_MASK);
            buffer_l->DATA[buffer_l->IN] = _ki2s_format_in_data(io_info_ptr->DATA_L, io_format);
            buffer_l->SPACE--;
            stats->PACKETS_QUEUED_L++;
            /* buffer only even samples */
            io_info_ptr->DATA_L += io_format->CHANNELS * io_format->SIZE;
            /* increase and check input index of left cyclic buffer */
            (++buffer_l->IN >= buffer_l->SIZE) ? (buffer_l->IN = 0) : (0);
            /* critical section end - enable interrupt */
            i2s_ptr->IER |= (io_info_ptr->TX_INT_MASK);
         }

         /* Copy data from input to right channel buffer */
         while ((buffer_r->SPACE > 0) && (stats->PACKETS_QUEUED_R < stats->PACKETS_REQUESTED_R))
         {
            /* buffer access, critical section - disable interrupts with buffer access */
            i2s_ptr->IER &= ~(io_info_ptr->TX_INT_MASK);
            buffer_r->DATA[buffer_r->IN] = _ki2s_format_in_data(io_info_ptr->DATA_R, io_format);
            buffer_r->SPACE--;
            stats->PACKETS_QUEUED_R++;
            /* buffer only odd samples */
            io_info_ptr->DATA_R += io_format->CHANNELS * io_format->SIZE;
            /* increase and check input index of right cyclic buffer */
            (++buffer_r->IN >= buffer_r->SIZE) ? (buffer_r->IN = 0) : (0);
            /* critical section end - enable interrupt */
            i2s_ptr->IER |= (io_info_ptr->TX_INT_MASK);
         }
      }
      if (io_info_ptr->FIRST_IO)
      {
         io_info_ptr->FIRST_IO = FALSE;
         i2s_ptr->IER |= (I2S_IER_TIE_MASK); /* Transmit interrupts enable*/
         i2s_ptr->CR |= (I2S_CR_TE_MASK); /* Enable transmitter */
      }

      /* If all requested packets are queued, break and return from function */
      if ((stats->PACKETS_QUEUED_L == stats->PACKETS_REQUESTED_L) && (stats->PACKETS_QUEUED_R == stats->PACKETS_REQUESTED_R))
      {
         break;
      }

      /* Wait for transmit finish and make current task blocked */
      result = _lwsem_wait(event_int_done);

      if (result != MQX_OK)
      {
         #ifdef I2S_DEBUG
          printf("i2s_tx - Error: Wait for event_int_done failed.\n");
         #endif
         _task_set_error(result);
      }
   }
   /* return number of processed data */
   return(length);
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    :_ki2s_int_isr
* Returned Value   : none
*
*END*********************************************************************/
static void _ki2s_int_isr
   (
      pointer                  parameter
   )
{ /* Body */
   KI2S_INFO_STRUCT_PTR          io_info_ptr = parameter;
   I2S_MemMapPtr                 i2s_ptr = io_info_ptr->I2S_PTR;
   KI2S_BUFFER_PTR               buffer_l = &(io_info_ptr->BUFFER_L);
   KI2S_BUFFER_PTR               buffer_r = &(io_info_ptr->BUFFER_R);
   I2S_STATISTICS_STRUCT_PRV_PTR stats = &(io_info_ptr->STATS);
   KI2S_DUMMY_DATA_PTR           dummy = &(io_info_ptr->DUMMY);

   stats->INTERRUPTS++;

   if (io_info_ptr->IO_MODE == I2S_IO_READ)
   {
      /* One of receive FIFOs full */
      if ((i2s_ptr->ISR & I2S_ISR_RFF0_MASK) || (i2s_ptr->ISR & I2S_ISR_RFF1_MASK))
      {
         uint_8 i, data_n;
         /* Read value of Receive FIFO Counter0 */
         data_n = ((i2s_ptr->FCSR >> 12) & 0xF);
         /* calculate quantity of data to read */
         if (data_n > buffer_l->SPACE)
         {
            data_n = buffer_l->SPACE;
         }
         /* Copy data from left channel FIFO to left buffer */
         for (i = 0; i < data_n; i++)
         {
            buffer_l->DATA[buffer_l->IN] = i2s_ptr->RX0;
            (++buffer_l->IN >= buffer_l->SIZE) ? (buffer_l->IN = 0) : (0);
         }
         stats->RX_PACKETS += data_n;
         stats->PACKETS_QUEUED_L += data_n;
         buffer_l->SPACE -= data_n;
         
         /* Read value of Receive FIFO Counter1 */
         data_n = ((i2s_ptr->FCSR >> 28) & 0xF);
         
         if (data_n > buffer_r->SPACE)
         {
            data_n = buffer_r->SPACE;
         }
         /* Copy data from right channel FIFO to buffer */
         for (i = 0; i < data_n; i++)
         {      
            buffer_r->DATA[buffer_r->IN] = i2s_ptr->RX1;
            (++buffer_r->IN >= buffer_r->SIZE) ? (buffer_r->IN = 0) : (0);
         }
         stats->RX_PACKETS += data_n;
         stats->PACKETS_QUEUED_R += data_n;
         buffer_r->SPACE -= data_n;
      }

      /* Check receiver overrun error 0 */
      if ((i2s_ptr->IER & I2S_IER_ROE0EN_MASK) && (i2s_ptr->ISR & I2S_ISR_ROE0_MASK))
      {
         stats->OVERRUNS_L++;
         /* Clear error flag */
         i2s_ptr->ISR |= (I2S_ISR_ROE0_MASK);
      }

      /* Check receiver overrun error 1 */
      if ((i2s_ptr->IER & I2S_IER_ROE1EN_MASK) && (i2s_ptr->ISR & I2S_ISR_ROE1_MASK))
      {
         stats->OVERRUNS_R++;
         /* Clear error flag */
         i2s_ptr->ISR |= (I2S_ISR_ROE1_MASK);
      }
   }
   else
   {
      /* All data transmitted, disable transmitter */
      if ((stats->PACKETS_PROCESSED_L >= stats->PACKETS_REQUESTED_L) &&
          (stats->PACKETS_PROCESSED_R >= stats->PACKETS_REQUESTED_R) &&
          (i2s_ptr->ISR & I2S_ISR_TFE0_MASK) &&
          (i2s_ptr->ISR & I2S_ISR_TFE1_MASK))
      {
         i2s_ptr->CR &= ~(I2S_CR_TE_MASK);
         /* disable transmitter interrupts */
         i2s_ptr->IER &= ~(I2S_IER_TIE_MASK);
         io_info_ptr->ONTHEWAY = FALSE;
         io_info_ptr->FIRST_IO = TRUE;
      }

      /* Transmit FIFO empty - need more data */
      if ((i2s_ptr->ISR & I2S_ISR_TFE0_MASK) || (i2s_ptr->ISR & I2S_ISR_TFE1_MASK))
      {
         uint_8 i, space;
         /* Read value of Transmit FIFO Counter0 */
         space = FIFO_WATERMARK_MAX - ((i2s_ptr->FCSR >> I2S_CHAR_BIT) & 0xF);
         /* Copy data from buffer to left channel FIFO */
         for (i = 0; ((i < space) && (stats->PACKETS_PROCESSED_L < stats->PACKETS_QUEUED_L)); i++)
         {
            if (io_info_ptr->TX_DUMMY)
            {
               i2s_ptr->TX0 = (*(dummy->DATA + dummy->INDEX_L)) & 0xFFFFFFFF;
               (++dummy->INDEX_L >= dummy->LENGTH) ? (dummy->INDEX_L = 0) : (0);
            }
            else
            {
               i2s_ptr->TX0 = buffer_l->DATA[buffer_l->OUT];
               (++buffer_l->OUT >=  buffer_l->SIZE) ? (buffer_l->OUT = 0) : (0);
            }
            stats->PACKETS_PROCESSED_L++;
         }
         stats->TX_PACKETS += i;
         buffer_l->SPACE += i;

         /* Read value of Transmit FIFO Counter1 */
         space = FIFO_WATERMARK_MAX - ((i2s_ptr->FCSR >> (I2S_CHAR_BIT * 3)) & 0xF);

         /* Copy data from buffer to right channel FIFO */
         for (i = 0; ((i < space) && (stats->PACKETS_PROCESSED_R < stats->PACKETS_QUEUED_R)); i++)
         {
            if (io_info_ptr->TX_DUMMY)
            {
               i2s_ptr->TX1 = (*(dummy->DATA + dummy->INDEX_R)) & 0xFFFFFFFF;
               (++dummy->INDEX_R >= dummy->LENGTH) ? (dummy->INDEX_R = 0) : (0);
            }
            else
            {
               i2s_ptr->TX1 = buffer_r->DATA[buffer_r->OUT];
              (++buffer_r->OUT >=  buffer_r->SIZE) ? (buffer_r->OUT = 0) : (0); 
            }
            stats->PACKETS_PROCESSED_R++;
         }
         stats->TX_PACKETS += i;
         buffer_r->SPACE += i;
      }
      
      if (stats->PACKETS_PROCESSED_L >= stats->PACKETS_REQUESTED_L)
      {
          i2s_ptr->IER &= ~(I2S_IER_TFE0EN_MASK);
      }
      if (stats->PACKETS_PROCESSED_R >= stats->PACKETS_REQUESTED_R)
      {
          i2s_ptr->IER &= ~(I2S_IER_TFE1EN_MASK);
      }
      
      /* Check for transmit underrun error 0 */
      if ((i2s_ptr->IER & I2S_IER_TUE0EN_MASK) && (i2s_ptr->ISR & I2S_ISR_TUE0_MASK))
      {
         stats->UNDERRUNS_L++;
         /* Clear error flag */
         i2s_ptr->ISR |= (I2S_ISR_TUE0_MASK);
      }

      /* Check for transmit underrun error 1 */
      if ((i2s_ptr->IER & I2S_IER_TUE1EN_MASK) && (i2s_ptr->ISR & I2S_ISR_TUE1_MASK))
      {
         stats->UNDERRUNS_R++;
         /* Clear error flag */
         i2s_ptr->ISR |= (I2S_ISR_TUE1_MASK);
      }
   }
   if (_lwsem_post(&(io_info_ptr->EVENT_IO_FINISHED)) != MQX_OK)
   {
      #ifdef I2S_DEBUG
      printf("i2s_isr - Error: Unable to set EVENT_IO_FINISHED.\n");
      #endif
   }
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_int_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the I2S I/O device.
*
*END*********************************************************************/

uint_32 _ki2s_int_ioctl
   (
      /* [IN] the address of the device specific information */
      KI2S_INFO_STRUCT_PTR io_info_ptr,

      /* [IN] The command to perform */
      uint_32                     cmd,

      /* [IN] Parameters for the command */
      pointer                 param_ptr
   )
{ /* Body */
   uint_32              result = MQX_OK;
   I2S_MemMapPtr        i2s_ptr = io_info_ptr->I2S_PTR;
   uint_8               watermark = 0;
   KI2S_DUMMY_DATA_PTR  dummy = &io_info_ptr->DUMMY;
   SIM_MemMapPtr        sim = SIM_BASE_PTR;

   switch(cmd)
   {
      case IO_IOCTL_I2S_SET_MODE_MASTER:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_MODE_MASTER: Error - Cannot chnage I2S mode while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK);
         i2s_ptr->CR &= ~(I2S_CR_I2SMODE_MASK);
         i2s_ptr->CR |= (I2S_CR_I2SMODE(0x01)); /* Master mode */

         if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_EXT)
         {
            i2s_ptr->CR &= ~(I2S_CR_SYSCLKEN_MASK); /* Internal master clock disable */
         }
         else if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
         {
            i2s_ptr->CR |= (I2S_CR_SYSCLKEN_MASK); /* Master clock enable */
            io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr);
            io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
            /* The word length is fixed to 32 in I2S master mode and DC is 1
            ** thus frame sync frequency is always BCLK_FREQ/64
            */
            io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / 64;
         }

         i2s_ptr->TCR |= (
            I2S_TCR_TXDIR_MASK | /* Transmit bit clock generated internally */
            I2S_TCR_TFDIR_MASK    /* Transmit frame sync generated internally */
            );
         i2s_ptr->CR |= (I2S_CR_I2SEN_MASK);
         io_info_ptr->MODE = I2S_MODE_MASTER;
         break;

      case IO_IOCTL_I2S_SET_MODE_SLAVE:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_MODE_SLAVE: Error - Cannot chnage I2S mode while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK);
         i2s_ptr->CR &= ~(I2S_CR_I2SMODE_MASK);
         i2s_ptr->CR |= (I2S_CR_I2SMODE(0x02)); /* I2S Slave mode */
         i2s_ptr->CR &= ~(I2S_CR_SYSCLKEN_MASK);  /* Master clock disabled */
         i2s_ptr->TCR &= ~(
            I2S_TCR_TXDIR_MASK |  /* Transmit bit clock generated externally */
            I2S_TCR_TFDIR_MASK    /* Transmit frame sync generated externally */
            );
         i2s_ptr->CR |= (I2S_CR_I2SEN_MASK);
         io_info_ptr->MODE = I2S_MODE_SLAVE;
         break;

      case IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT: Error - Cannot chnage I2S clock source while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         /* You should not change clock source, when in slave mode */
         if (io_info_ptr->MODE == I2S_MODE_SLAVE)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT: Error - Cannot chnage I2S clock source while in slave mode.\n");
            #endif
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
         }

         i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK); /* disable SSI - see table at top of this file */
         i2s_ptr->CR |= (I2S_CR_SYSCLKEN_MASK); /* Internal master clock enable */
         i2s_ptr->CR |= (I2S_CR_I2SEN_MASK);

         if (io_info_ptr->CHANNEL == 0)
         {
        	 SIM_SOPT2 |= SIM_SOPT2_I2SSRC(0);
         }
         io_info_ptr->CLOCK_SOURCE = I2S_CLK_INT;
         io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr);
         io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
         io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * io_info_ptr->IO_FORMAT.CHANNELS);
         break;

      case IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT: Error - Cannot chnage I2S clock source while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         if (io_info_ptr->MODE == I2S_MODE_SLAVE)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT: Error - Cannot change I2S clock source while in slave mode.\n");
            #endif
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
         }
         /*enable pin to receive external CLK signal*/
         i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK); /* disable I2S - see table at top of this file */
         i2s_ptr->CR &= ~(I2S_CR_SYSCLKEN_MASK); /* Internal master clock disable */
         i2s_ptr->CR |= (I2S_CR_I2SEN_MASK);

         if (io_info_ptr->CHANNEL == 0)
         {
        	 SIM_SOPT2 |= SIM_SOPT2_I2SSRC(3); //I2S0 clock source = I2S_CLKIN pin
         }
         io_info_ptr->CLOCK_SOURCE = I2S_CLK_EXT;
         break;

      case IO_IOCTL_I2S_SET_DATA_BITS:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_DATA_BITS: Error - Cannot chnage I2S data bit length while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
         }
         else
         {
            uint_8 data_bits =  *((uint_8_ptr) param_ptr);
            if ((data_bits < 8) || (data_bits > 24) || (data_bits == 14) || (data_bits % 2))
            {
               #ifdef I2S_DEBUG
                printf("_i2s_ioctl - IO_IOCTL_I2S_SET_DATA_BITS: Error - Unsupported data word length.\n");
               #endif
               result = I2S_ERROR_WORD_LENGTH_UNSUPPORTED;
               break;
            }
            data_bits = (data_bits/2) - 1;
            i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK);
            i2s_ptr->TCCR &= ~(I2S_TCCR_WL_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_WL((uint_32) data_bits));
            i2s_ptr->RCCR &= ~(I2S_RCCR_WL_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_WL((uint_32) data_bits));
            i2s_ptr->CR |= (I2S_CR_I2SEN_MASK);
            io_info_ptr->DATA_BITS = *((uint_8_ptr) param_ptr);
            io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * io_info_ptr->IO_FORMAT.CHANNELS);
         }
         break;

      case IO_IOCTL_I2S_DISABLE_DEVICE:
         i2s_ptr->IER &= ~(
              I2S_IER_TUE0EN_MASK | /* Transmit underrun error 0*/
              I2S_IER_TFE0EN_MASK | /* Transmit FIFO0 empty */
              I2S_IER_ROE0EN_MASK | /* Receive overrun error 0*/
              I2S_IER_RFF0EN_MASK | /* Receive FIFO0 full */
              I2S_IER_TUE1EN_MASK | /* Transmit underrun error 1*/
              I2S_IER_TFE1EN_MASK | /* Transmit FIFO1 empty */
              I2S_IER_ROE1EN_MASK | /* Receive overrun error 1*/
              I2S_IER_RFF1EN_MASK | /* Receive FIFO1 full */
              I2S_IER_TIE_MASK    | /* Transmit interrupts disable */
              I2S_IER_RIE_MASK      /* Receive interrupts disable */
              );
         i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK); /*I2S disable*/
         break;

      case IO_IOCTL_I2S_ENABLE_DEVICE:
         i2s_ptr->IER |= (
                          I2S_IER_TIE_MASK | /* Transmit interrupts enable */
                          I2S_IER_RIE_MASK   /* Receive interrupts enable */
                          );
         i2s_ptr->CR |= (I2S_CR_I2SEN_MASK); /*I2S enable*/
         break;

      case IO_IOCTL_I2S_SET_MCLK_FREQ:
         if (io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_MCLK_FREQ: Error - Cannot chnage I2S master clock frequency while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         _ki2s_set_mclk_value(io_info_ptr, *((uint_32_ptr) param_ptr));
         break;

      case IO_IOCTL_I2S_SET_FS_FREQ:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_FS_FREQ: Error - Cannot chnage I2S sampling frequency while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         _ki2s_set_fs_value(io_info_ptr, *((uint_32_ptr) param_ptr));
         break;

      case IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT:
         if(io_info_ptr->ONTHEWAY)
         {
            #ifdef I2S_DEBUG
             printf("_i2s_ioctl - IO_IOCTL_I2S_SET_IO_DATA_FORMAT: Error - Cannot change I/O data format while I/O actions are in progress.\n");
            #endif
            result = I2S_ERROR_DEVICE_BUSY;
            break;
         }
         if (param_ptr == NULL)
         {
            result = I2S_ERROR_INVALID_PARAMETER;
            break;
         }
         else
         {
            uint_8 aligment, endianity, bits, channels, size;
            endianity = ((AUDIO_DATA_FORMAT_PTR) param_ptr)->ENDIAN;
            bits = ((AUDIO_DATA_FORMAT_PTR) param_ptr)->BITS;
            aligment = ((AUDIO_DATA_FORMAT_PTR) param_ptr)->ALIGNMENT;
            channels = ((AUDIO_DATA_FORMAT_PTR) param_ptr)->CHANNELS;
            size = ((AUDIO_DATA_FORMAT_PTR) param_ptr)->SIZE;
            if ((aligment > AUDIO_ALIGNMENT_LEFT) || (bits < AUDIO_BIT_SIZE_MIN) ||
               (bits > AUDIO_BIT_SIZE_MAX) || (endianity > AUDIO_LITTLE_ENDIAN) ||
               (channels > 2) || (channels == 0))
            {
               #ifdef I2S_DEBUG
                printf("_i2s_ioctl - IO_IOCTL_I2S_SET_IO_DATA_FORMAT: Error - Invalid I/O data format.\n");
               #endif
               result = AUDIO_ERROR_INVALID_IO_FORMAT;
               break;
            }
            io_info_ptr->IO_FORMAT.ENDIAN = endianity;
            io_info_ptr->IO_FORMAT.BITS = bits;
            io_info_ptr->IO_FORMAT.ALIGNMENT = aligment;
            io_info_ptr->IO_FORMAT.CHANNELS = channels;
            io_info_ptr->IO_FORMAT.SIZE = size;
          
            /* If only one channel is going to be used disable FIFO1 and its interrupts */
            if (channels == 1)
            {
                io_info_ptr->TX_INT_MASK = (I2S_IER_TFE0EN_MASK);
                io_info_ptr->RX_INT_MASK = (I2S_IER_RFF0EN_MASK);
                i2s_ptr->RCR &= ~(I2S_RCR_RFEN1_MASK);
                i2s_ptr->TCR &= ~(I2S_TCR_TFEN1_MASK);
            }
            else
            {
                io_info_ptr->TX_INT_MASK = (I2S_IER_TFE0EN_MASK | I2S_IER_TFE1EN_MASK);
                io_info_ptr->RX_INT_MASK = (I2S_IER_RFF0EN_MASK | I2S_IER_RFF1EN_MASK);
                i2s_ptr->RCR |= (I2S_RCR_RFEN1_MASK);
                i2s_ptr->TCR |= (I2S_TCR_TFEN1_MASK);
            }
         }
         break;

      case IO_IOCTL_I2S_SET_TXFIFO_WATERMARK:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         watermark = *((uint_8_ptr) param_ptr);
         if (watermark > FIFO_WATERMARK_MAX)
         {
            result = I2S_ERROR_PARAM_OUT_OF_RANGE;
            break;
         }
         i2s_ptr->FCSR &= ~(
            I2S_FCSR_TFWM0_MASK |
            I2S_FCSR_TFWM1_MASK
            );
         i2s_ptr->FCSR |= (
            I2S_FCSR_TFWM0(watermark) | /* Set transmit FIFO0 watermark */
            I2S_FCSR_TFWM1(watermark)  /* Set transmit FIFO1 watermark */
         );
         break;

      case IO_IOCTL_I2S_SET_RXFIFO_WATERMARK:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         watermark = *((uint_8_ptr) param_ptr);
         if (watermark > FIFO_WATERMARK_MAX)
         {
            result = I2S_ERROR_PARAM_OUT_OF_RANGE;
            break;
         }
         i2s_ptr->FCSR &= ~(
            I2S_FCSR_RFWM0_MASK |
            I2S_FCSR_RFWM1_MASK
            );
         i2s_ptr->FCSR |= (
            I2S_FCSR_RFWM0(watermark) | /* Set receive FIFO0 watermark */
            I2S_FCSR_RFWM1(watermark)   /* Set reveive FIFO1 watermark */
            );
         break;

      case IO_IOCTL_I2S_TX_DUMMY_ON:
         if (io_info_ptr->TX_DUMMY == FALSE)
         {
            io_info_ptr->TX_DUMMY = TRUE;
            dummy->LENGTH = _ki2s_sine_wave(io_info_ptr->FS_FREQ, 440, &(dummy->DATA));
            dummy->INDEX_L = 0;
            dummy->INDEX_R = 0;
         }
         break;

      case IO_IOCTL_I2S_TX_DUMMY_OFF:
         if (io_info_ptr->TX_DUMMY == TRUE)
         {
            io_info_ptr->TX_DUMMY = FALSE;
            result = _mem_free(dummy->DATA);
            if (result == MQX_OK)
            {
               dummy->LENGTH = 0;
               dummy->DATA = NULL;
            }
         dummy->INDEX_L = 0;
         dummy->INDEX_R = 0;
         }
         break;

      case IO_IOCTL_I2S_GET_MODE:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((uint_8_ptr) param_ptr) = io_info_ptr->MODE;
         break;

      case IO_IOCTL_I2S_GET_CLOCK_SOURCE:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((uint_8_ptr) param_ptr) = io_info_ptr->CLOCK_SOURCE;
          break;

      case IO_IOCTL_I2S_GET_DATA_BITS:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((uint_8_ptr) param_ptr) = io_info_ptr->DATA_BITS;
         break;

      case IO_IOCTL_I2S_GET_MCLK_FREQ:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((uint_32_ptr) param_ptr) = io_info_ptr->MCLK_FREQ;
         break;

      case IO_IOCTL_I2S_GET_BCLK_FREQ:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((uint_32_ptr) param_ptr) = io_info_ptr->BCLK_FREQ;
         break;

      case IO_IOCTL_I2S_GET_TX_DUMMY:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((boolean*) param_ptr) = io_info_ptr->TX_DUMMY;
         break;

      case IO_IOCTL_I2S_GET_FS_FREQ:
         if (param_ptr == NULL)
         {
              result = I2S_ERROR_INVALID_PARAMETER;
              break;
         }
         *((uint_32_ptr) param_ptr) = io_info_ptr->FS_FREQ;
         break;

      case IO_IOCTL_I2S_GET_STATISTICS:
         if (param_ptr == NULL)
         {
            result = I2S_ERROR_INVALID_PARAMETER;
            break;
         }
         else
         {
             I2S_STATISTICS_STRUCT_PTR stats_out = (I2S_STATISTICS_STRUCT_PTR) param_ptr;
             I2S_STATISTICS_STRUCT_PRV_PTR stats_in = &(io_info_ptr->STATS);
             
             stats_out->INTERRUPTS = stats_in->INTERRUPTS;
             stats_out->FIFO_ERROR = stats_in->UNDERRUNS_L + stats_in->UNDERRUNS_R + stats_in->OVERRUNS_L +stats_in->OVERRUNS_R;
             stats_out->BUFFER_ERROR = 0;
             stats_out->RX_PACKETS = stats_in->RX_PACKETS;
             stats_out->TX_PACKETS = stats_in->TX_PACKETS;
             stats_out->PACKETS_PROCESSED = stats_in->PACKETS_PROCESSED_L + stats_in->PACKETS_PROCESSED_R;
             stats_out->PACKETS_QUEUED = stats_in->PACKETS_QUEUED_L + stats_in->PACKETS_QUEUED_R;
             stats_out->PACKETS_REQUESTED = stats_in->PACKETS_REQUESTED_L + stats_in->PACKETS_REQUESTED_R;
         }
         break;

      case IO_IOCTL_AUDIO_GET_IO_DATA_FORMAT:
         if (param_ptr == NULL)
         {
            result = I2S_ERROR_INVALID_PARAMETER;
            break;
         }
         ((AUDIO_DATA_FORMAT_PTR) param_ptr)->ENDIAN = io_info_ptr->IO_FORMAT.ENDIAN;
         ((AUDIO_DATA_FORMAT_PTR) param_ptr)->BITS = io_info_ptr->IO_FORMAT.BITS;
         ((AUDIO_DATA_FORMAT_PTR) param_ptr)->ALIGNMENT = io_info_ptr->IO_FORMAT.ALIGNMENT;
         ((AUDIO_DATA_FORMAT_PTR) param_ptr)->CHANNELS = io_info_ptr->IO_FORMAT.CHANNELS;
         break;

      case IO_IOCTL_I2S_GET_TXFIFO_WATERMARK:
         if (param_ptr == NULL)
         {
            result = I2S_ERROR_INVALID_PARAMETER;
            break;
         }
         *((uint_8_ptr) param_ptr) = i2s_ptr->FCSR & I2S_FCSR_TFWM0_MASK;
         break;

      case IO_IOCTL_I2S_GET_RXFIFO_WATERMARK:
         if (param_ptr == NULL)
         {
            result = I2S_ERROR_INVALID_PARAMETER;
            break;
         }
         *((uint_8_ptr) param_ptr) = (i2s_ptr->FCSR & I2S_FCSR_RFWM0_MASK) >> 8;
         break;

      case IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_ON:
         i2s_ptr->CR &= ~(I2S_CR_RFRCLKDIS_MASK); /* Enable receiving clocks after receiver is disabled */
         i2s_ptr->CR &= ~(I2S_CR_TFRCLKDIS_MASK); /* Enable transmitting clocks after transmitter is disabled */
         io_info_ptr->CLK_ALWAYS_ON = TRUE;
         break;

      case IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_OFF:
         i2s_ptr->CR |= (I2S_CR_RFRCLKDIS_MASK); /* Disable receiving clocks after receiver is disabled */
         i2s_ptr->CR |= (I2S_CR_TFRCLKDIS_MASK); /* Disable transmitting clocks after transmitter is disabled */
         io_info_ptr->CLK_ALWAYS_ON = FALSE;
         break;

      case IO_IOCTL_I2S_GET_CLK_ALWAYS_ENABLED:
         if (param_ptr == NULL)
         {
            result = I2S_ERROR_INVALID_PARAMETER;
            break;
         }
         *((boolean*) param_ptr) = io_info_ptr->CLK_ALWAYS_ON;
         break;

      case IO_IOCTL_I2S_CLEAR_STATISTICS:
         _mem_zero(&(io_info_ptr->STATS), sizeof(io_info_ptr->STATS));
         break;
         
      case IO_IOCTL_FLUSH_OUTPUT:
          while (io_info_ptr->ONTHEWAY)
          {
              uint_32 time = 0;
              time = ((io_info_ptr->BUFFER_L.SIZE - io_info_ptr->BUFFER_L.SPACE) +
                      (io_info_ptr->BUFFER_R.SIZE - io_info_ptr->BUFFER_R.SPACE)) *
                      (1000 / io_info_ptr->FS_FREQ) + 1;
              _time_delay(time);
          }
          break;
      
      default:
         result = IO_ERROR_INVALID_IOCTL_CMD;
         break;
   }
   return result;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_format_in_data
* Returned Value   : formatted input data
* Comments         : Function performs I2S input data formatting
*
*END*********************************************************************/
static uint_32 _ki2s_format_in_data
   (
    /* [IN] data to format */
    char_ptr input,

    /* [IN] Requested format */
    AUDIO_DATA_FORMAT_PTR format
    )
{/* Body */
   uint_32 retval = 0;
   uint_8 i;
   
   if (format->ENDIAN == AUDIO_BIG_ENDIAN)
   {
      for (i = 0; i < format->SIZE; i++)
      {
          retval |= ((((uint_32) *(input + i)) << (format->SIZE - 1 - i) * 8) & (0xFF) << (format->SIZE - 1 - i) * 8);
      }
      if ((format->ALIGNMENT == AUDIO_ALIGNMENT_RIGHT) && (format->BITS < (format->SIZE * 8)))
      {
         retval >>= (format->SIZE * I2S_CHAR_BIT) - format->BITS;
      }
   }
   else
   {
      for (i = 0; i < format->SIZE; i++)
      {
          retval |= ((uint_32) *(input + i) << i * 8);
      }
   }
   return(~retval);
}/* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_format_out_data
* Returned Value   : none
* Comments         : Function performs I2S output data formatting
*
*END*********************************************************************/
static void _ki2s_format_out_data
   (
    /* [IN] data to format */
    uint_32 input,

    /* [OUT] location for output */
    char_ptr output,

    /* [IN] Requested output data format */
    AUDIO_DATA_FORMAT_PTR format
    )
{
   uint_8 i;
   input = ~input;
   
   if (format->ENDIAN == AUDIO_BIG_ENDIAN)
   {
     for(i = 0; i < format->SIZE; i++)
     {
       output[i] = (input >> I2S_CHAR_BIT * (format->SIZE - i - 1) & 0xFF);
     }
     if ((format->ALIGNMENT == AUDIO_ALIGNMENT_LEFT) && (format->BITS < (format->SIZE * 8)))
     {
       input <<= (format->SIZE * I2S_CHAR_BIT) - format->BITS;
     }
   }
   else
   {
     for(i = 0; i < format->SIZE; i++)
     {
       output[i] = (char) (input  >> (i * I2S_CHAR_BIT)); 
     }
   }

   return;
}

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_sine_wave
* Returned Value   : samples number if successful, 0 otherwise
* Comments         :
*    This function generate a sine wave.
*
*END*********************************************************************/

uint_32 _ki2s_sine_wave(uint_32 fs, uint_32 freq, int_16_ptr* output)
{
    uint_32 samples, i;
    boolean over180 = 0;
    ieee_single period, step, angle = 0.0;
    period = 1 / (ieee_single) freq;
    samples = fs * (uint_32)period;
    if (samples == 0)
    {
      return(0);
    }
    *output = (int_16_ptr) _mem_alloc(sizeof(int_16) * samples);
    if (*output == NULL)
    {
       return(0);
    }
    step = 360.0 / (ieee_single) samples;
    for(i = 0; i < samples; i++)
    {
        if (angle > 180)
        {
            angle = angle - 180;
            over180 = TRUE;
        }
        *((*output) + i) =  BIT16_MAX * (4*(uint_32)angle*(180 - (uint_32)angle))/(40500 - (uint_32)angle * (180 - (uint_32)angle));
        if (over180)
        {
           *((*output) + i) = -(*((*output) + i));
        }
        angle += step;
    }
    return(samples);
}


/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_hw_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an I2S hardware control registers.
*
*END*********************************************************************/

static _mqx_int _ki2s_hw_init
(
   KI2S_INIT_STRUCT_PTR i2s_init_ptr,
   I2S_MemMapPtr i2s_ptr
)
{
   /* variables */
   uint_8 data_bits = i2s_init_ptr->DATA_BITS;
   uint_8 i2s_mode = i2s_init_ptr->MODE;
   uint_8 clk_source = i2s_init_ptr->CLOCK_SOURCE;
   uint_8 channel = i2s_init_ptr->CHANNEL;
   uint_8 channels = i2s_init_ptr->IO_FORMAT->CHANNELS;
   boolean tx_empty = i2s_init_ptr->TX_DUMMY;
   SIM_MemMapPtr   sim = SIM_BASE_PTR;

   data_bits = (data_bits/2) - 1;
   i2s_ptr->CR &= ~(I2S_CR_I2SEN_MASK); /* I2S disable */
   i2s_ptr->CR |= (
      I2S_CR_SYN_MASK   |    /* Set synchronous mode */
      I2S_CR_TCHEN_MASK      /* Turn dual channel mode ON */
      );
   i2s_ptr->TCR &= ~(I2S_TCR_TSHFD_MASK); /* Data transmitted MSB first */
   i2s_ptr->RCR &= ~(I2S_RCR_RSHFD_MASK); /* Data received MSB first */
   i2s_ptr->TCR |= (
      I2S_TCR_TSCKP_MASK | /* Data clocked out on falling edge of bit clock */
      I2S_TCR_TFSI_MASK  | /* Transmit frame sync is active low */
      I2S_TCR_TEFS_MASK  | /* Transmit frame sync is initiated one bit before the data transmits */
      I2S_TCR_TFEN0_MASK | /* Transmit FIFO0 enable */
      I2S_TCR_TFEN1_MASK   /* Transmit FIFO1 enable */
      );
   i2s_ptr->FCSR &= ~(
      I2S_FCSR_TFWM0_MASK |
      I2S_FCSR_TFWM1_MASK | 
      I2S_FCSR_RFWM0_MASK |
      I2S_FCSR_RFWM1_MASK 
      );
   i2s_ptr->FCSR |= (
      I2S_FCSR_TFWM0(0xA) | /* Set transmit FIFO0 watermark to 10 */
      I2S_FCSR_TFWM1(0xA) | /* Set transmit FIFO1 watermark to 10 */
      I2S_FCSR_RFWM0(0x3) | /* Set receive FIFO0 watermark to 3   */
      I2S_FCSR_RFWM1(0x3)   /* Set receive FIFO1 watermark to 3   */
      );
   i2s_ptr->RCR |= (
      I2S_RCR_RSCKP_MASK | /* Data latched on rising edge of bit clock */
      I2S_RCR_RFSI_MASK  | /* Receive frame sync is active low */
      I2S_RCR_REFS_MASK  | /* Receive frame sync is initiated one bit before the data is received */
      I2S_RCR_RFEN0_MASK | /* Receive FIFO0 enable */
      I2S_RCR_RFEN1_MASK   /* Receive FIFO1 enable */
      );
   /* Interrupts setup */
   /* If only one channel is going to be used disable FIFO1 and its interrupts */
   if (channels == 1)
   {
       i2s_ptr->RCR &= ~(I2S_RCR_RFEN1_MASK);
       i2s_ptr->TCR &= ~(I2S_TCR_TFEN1_MASK);
       i2s_ptr->IER |= (
          I2S_IER_ROE0EN_MASK | /* Receive overrun error 0 */
          I2S_IER_RFF0EN_MASK | /* Receive FIFO1 full */
          I2S_IER_TUE0EN_MASK | /* Transmit underrun error 0 */
          I2S_IER_TFE0EN_MASK   /* Transmit FIFO0 empty */
          );
   }
   else
   {
       i2s_ptr->IER |= (
          I2S_IER_TUE0EN_MASK | /* Transmit underrun error 0*/
          I2S_IER_TFE0EN_MASK | /* Transmit FIFO0 empty */
          I2S_IER_ROE0EN_MASK | /* Receive overrun error 0*/
          I2S_IER_RFF0EN_MASK | /* Receive FIFO0 full */
          I2S_IER_TUE1EN_MASK | /* Transmit underrun error 1*/
          I2S_IER_TFE1EN_MASK | /* Transmit FIFO1 empty */
          I2S_IER_ROE1EN_MASK | /* Receive overrun error 1*/
          I2S_IER_RFF1EN_MASK   /* Receive FIFO1 full */
          );
   }
   
   i2s_ptr->IER &= ~(
      I2S_IER_RDR0EN_MASK | /* Receiver data ready 0 */
      I2S_IER_RDR1EN_MASK | /* Receiver data ready 1 */
      I2S_IER_TDE0EN_MASK | /* Transmit data empty 0 */
      I2S_IER_TDE1EN_MASK | /* Transmit data empty 1 */
      I2S_IER_RIE_MASK    | /* Receiver disable interrupts */
      I2S_IER_TIE_MASK      /* Transmitter disable interrupts */
      );
   
   /* clock source specific */
   switch (clk_source)
   {
      case I2S_CLK_INT:
         if (channel == 0)
         {
             /* At first Disable system clock to the I2S module */
             sim->SCGC6 &= ~(SIM_SCGC6_I2S_MASK);
             /* I2S clock source is system clock */
             sim->SOPT2 &= ~(SIM_SOPT2_I2SSRC_MASK);
             sim->SOPT2 |= SIM_SOPT2_I2SSRC(0);
             /* Divide to get the 12.2880 MHz from 96MHz (96* (16/125)) */
             sim->CLKDIV2 &= ~(SIM_CLKDIV2_I2SDIV_MASK | SIM_CLKDIV2_I2SFRAC_MASK);
             sim->CLKDIV2 |= SIM_CLKDIV2_I2SDIV(124) | SIM_CLKDIV2_I2SFRAC(15);
             sim->SCGC6 |= SIM_SCGC6_I2S_MASK;
         }
         break;
      case I2S_CLK_EXT:
         if (channel == 0)
         {
            /* At first Disable system clock to the I2S module */
            sim->SCGC6 &= ~(SIM_SCGC6_I2S_MASK);
            sim->SOPT2 &= ~(SIM_SOPT2_I2SSRC_MASK);
            sim->SOPT2 |= SIM_SOPT2_I2SSRC(3);
            /* Divide to get the 12.2880 MHz from 96MHz (96* (16/125)) */
            sim->CLKDIV2 &= ~(SIM_CLKDIV2_I2SDIV_MASK | SIM_CLKDIV2_I2SFRAC_MASK);
            sim->CLKDIV2 |= SIM_CLKDIV2_I2SDIV(124) | SIM_CLKDIV2_I2SFRAC(15);
            sim->SCGC6 |= SIM_SCGC6_I2S_MASK;
         }
         break;
      default:
         break;
   }
   /* mode specific */
   switch(i2s_mode)
   {
      case I2S_MODE_MASTER:
         i2s_ptr->CR &= ~(I2S_CR_I2SMODE_MASK);
         i2s_ptr->CR |= (I2S_CR_I2SMODE(0x01));
         if (clk_source == I2S_CLK_EXT)
         {
            i2s_ptr->CR &= ~(I2S_CR_SYSCLKEN_MASK); /* Master clock disable */
         }
         else
         {
            i2s_ptr->CR |= (I2S_CR_SYSCLKEN_MASK);  /* Master clock enable */
         }
         i2s_ptr->TCR |= (
            I2S_TCR_TXDIR_MASK | /* Transmit bit clock generated internally */
            I2S_TCR_TFDIR_MASK   /* Transmit frame sync generated internally */
            );
         break;
      case I2S_MODE_SLAVE:
         i2s_ptr->CR &= ~(I2S_CR_I2SMODE_MASK);
         i2s_ptr->CR |= (I2S_CR_I2SMODE(0x02));
         i2s_ptr->CR &= ~(I2S_CR_SYSCLKEN_MASK); /* Master clock disabled */
         i2s_ptr->TCR &= ~(
            I2S_TCR_TXDIR_MASK | /* Transmit bit clock generated externally */
            I2S_TCR_TFDIR_MASK   /* Transmit frame sync generated externally */
            );
         break;
      default:
         break;
   }

   /* Set word length (valid data bits) */
   i2s_ptr->TCCR = (I2S_TCCR_WL((uint_32) data_bits));
   /* set frame rate 48 kHz*/
   i2s_ptr->TCCR &= ~(I2S_TCCR_DC_MASK);
   i2s_ptr->TCCR |= (I2S_TCCR_DC(0x01));
   i2s_ptr->TCCR &= ~(I2S_TCCR_PM_MASK);
   i2s_ptr->TCCR |= (I2S_TCCR_PM(0x80));
   /* Set word length (valid data bits) */
   i2s_ptr->RCCR = (I2S_RCCR_WL((uint_32) data_bits));
   i2s_ptr->RCCR &= ~(I2S_RCCR_DC_MASK);
   i2s_ptr->RCCR |= (I2S_RCCR_DC(0x01));
   i2s_ptr->RCCR &= ~(I2S_RCCR_PM_MASK);
   i2s_ptr->RCCR |= (I2S_RCCR_PM(0x80));

   /* Enable receiving clocks after receiver is disabled */
   i2s_ptr->CR &= ~(I2S_CR_RFRCLKDIS_MASK);
   /* Enable transmitting clocks after transmitter is disabled */
   i2s_ptr->CR &= ~(I2S_CR_TFRCLKDIS_MASK);
   /* I2S enable */
   i2s_ptr->CR |= (I2S_CR_I2SEN_MASK);

   return (MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_get_mclk_value
* Returned Value   : Master clock frequency
* Comments         :
*    This function computes master clock value.
*
*END*********************************************************************/

static uint_32 _ki2s_get_mclk_value
(
   KI2S_INFO_STRUCT_PTR io_info_ptr
)
{
    SIM_MemMapPtr   sim = SIM_BASE_PTR;
    uint_32 fract = 0;
    uint_32 div = 0;
    fract = (sim->CLKDIV2 & SIM_CLKDIV2_I2SFRAC_MASK) >> SIM_CLKDIV2_I2SFRAC_SHIFT;
    div = (sim->CLKDIV2 & SIM_CLKDIV2_I2SDIV_MASK) >> SIM_CLKDIV2_I2SDIV_SHIFT;
    return ((BSP_SYSTEM_CLOCK / (div + 1)) * (fract + 1));
}

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_mclk_value
* Returned Value   : MQX_OK if everything is OK, error macro value in case 
*                    of error.
* Comments         :
*    This function sets up master clock (called from ioctl)
*
*END*********************************************************************/
_mqx_int _ki2s_set_mclk_value
(
    KI2S_INFO_STRUCT_PTR io_info_ptr,
    uint_32 frequency
)
{
    uint_8 divider;
    boolean found;
    _mqx_int result = MQX_OK;
    SIM_MemMapPtr sim = SIM_BASE_PTR;
    
    if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
    {
        uint_32 fract, temp_fract, divide, temp_div, closest_mclk, iter_mclk;
        found = FALSE;
        closest_mclk = 0;
        iter_mclk = 0;
        divider =(uint_8) (BSP_SYSTEM_CLOCK / frequency);
        
        if ((divider < SSIDIV_MIN) || (divider > SSIDIV_MAX))
        {
            #ifdef I2S_DEBUG
            printf("_i2s_ioctl - IO_IOCTL_I2S_SET_MCLK_FREQ: Error - System clock divider value out of range.\n");
            #endif
            result = I2S_ERROR_DIVIDER_VALUE_INVALID;
            return (result);
        }
        for (fract = 1; fract < 256; fract++)
        {
            for(divide = 1; divide < 4096; divide++)
            {
                iter_mclk = (BSP_SYSTEM_CLOCK / divide) * fract;
                if (iter_mclk < frequency) break;
                if ((iter_mclk - frequency) == 0)
                {
                    found = TRUE;
                    break;
                }              
                else if ((closest_mclk == 0) || ((iter_mclk % frequency) < (closest_mclk % frequency)))
                {
                    temp_fract = fract;
                    temp_div = divide;
                    closest_mclk = iter_mclk;
                }
            }
            if (found) break;
        }
        if (found == FALSE)
        {
            frequency = closest_mclk;
            fract = temp_fract;
            divide = temp_div;
        }
        if (io_info_ptr->CHANNEL == 0)
        {
            sim->SCGC6 &= ~(SIM_SCGC6_I2S_MASK);
            sim->CLKDIV2 &= ~SIM_CLKDIV2_I2SDIV_MASK;
            sim->CLKDIV2 &= ~SIM_CLKDIV2_I2SFRAC_MASK;
            sim->CLKDIV2 |= SIM_CLKDIV2_I2SDIV(divide-1) | SIM_CLKDIV2_I2SFRAC(fract-1);
            sim->SCGC6 |= SIM_SCGC6_I2S_MASK;
        }
        io_info_ptr->MCLK_FREQ = frequency;
        io_info_ptr->MCLK_DIV = divider;
    }
    else
    {
        io_info_ptr->MCLK_FREQ = frequency;
    }
    if (io_info_ptr->MODE == I2S_MODE_MASTER)
    {
        io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / 64;
    }
    else
    {
        io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * io_info_ptr->IO_FORMAT.CHANNELS);
    }
    io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
    return(MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_fs_value
* Returned Value   : MQX_OK if everything is OK, error macro value in case 
*                    of error.
* Comments         :
*    This function sets up frame sync (sampling) frequency (called from ioctl)
*
*END*********************************************************************/
_mqx_int _ki2s_set_fs_value
(
    KI2S_INFO_STRUCT_PTR io_info_ptr,
    uint_32 frequency
)
{
    uint_32 bclk_frequency;
    uint_16 divider;
    _mqx_int result = MQX_OK;
    I2S_MemMapPtr i2s_ptr;
    
    i2s_ptr = io_info_ptr->I2S_PTR;
    if (io_info_ptr->MODE == I2S_MODE_SLAVE)
    {
        bclk_frequency = frequency * io_info_ptr->DATA_BITS * 2;
        if ((bclk_frequency > (io_info_ptr->MCLK_FREQ / 2)) || (bclk_frequency > (BSP_BUS_CLOCK / 5)))
        {
            #ifdef I2S_DEBUG
            printf("_i2s_ioctl - IO_IOCTL_I2S_SET_FS_FREQ: Error - BCLK_FREQ > MCLK_FREQ/2 or BCLK_FREQ > BUS_CLOCK/5.\n");
            #endif
            result = I2S_ERROR_FREQUENCY_INVALID;
            return(result);
        }
        io_info_ptr->BCLK_FREQ = bclk_frequency;
        io_info_ptr->FS_FREQ = frequency;
        return(result);
    }

    bclk_frequency = frequency * 64;
    divider = io_info_ptr->MCLK_FREQ / (bclk_frequency * 2);
    /*
    ** valid divider values are:
    ** 1-256 in step of 1
    ** 257-512 in step of 2
    ** 513-2048 in step of 8
    ** 2049-4096 in step of 16
    */
    if ((divider < BCLK_DIV_MIN) || (divider > BCLK_DIV_MAX)   ||
        (divider > 256) && (divider < 513) && (divider % 2)    ||
        (divider > 512) && (divider < 2049) && (divider % 8)   ||
        (divider > 2048) && (divider < BCLK_DIV_MAX+1) && (divider % 16))
    {
        #ifdef I2S_DEBUG
         printf("_i2s_ioctl - IO_IOCTL_I2S_SET_FS_FREQ: Error - Invalid BCLK divider value.\n");
        #endif
        result = I2S_ERROR_DIVIDER_VALUE_INVALID;
        return(result);
    }
    else
    {
        if (divider < 257)
        {
            /* According to reference manual values of DIV2, PSR and PM must
            ** not be changed simultaneously */
            i2s_ptr->TCCR &= ~(I2S_TCCR_PM_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_PM(divider - 1));
            i2s_ptr->TCCR &= ~(I2S_TCCR_PSR_MASK);
            i2s_ptr->TCCR &= ~(I2S_TCCR_DIV2_MASK);
            /* Set receiver clock */
            i2s_ptr->RCCR &= ~(I2S_RCCR_PM_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_PM(divider - 1));
            i2s_ptr->RCCR &= ~(I2S_RCCR_PSR_MASK);
            i2s_ptr->RCCR &= ~(I2S_RCCR_DIV2_MASK);
        }
        else if ((divider > 256) && (divider < 513))
        {
            divider = divider/2;
            i2s_ptr->TCCR &= ~(I2S_TCCR_PM_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_PM(divider - 1));
            i2s_ptr->TCCR &= ~(I2S_TCCR_PSR_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_DIV2_MASK);
            /* Set receiver clock */
            i2s_ptr->RCCR &= ~(I2S_RCCR_PM_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_PM(divider - 1));
            i2s_ptr->RCCR &= ~(I2S_RCCR_PSR_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_DIV2_MASK);
        }
        else if ((divider > 512) && (divider < 2049))
        {
            divider = divider/8;
            i2s_ptr->TCCR &= ~(I2S_TCCR_PM_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_PM(divider - 1));
            i2s_ptr->TCCR |= (I2S_TCCR_PSR_MASK);
            i2s_ptr->TCCR &= ~(I2S_TCCR_DIV2_MASK);
            /* Set receiver clock */
            i2s_ptr->RCCR &= ~(I2S_RCCR_PM_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_PM(divider - 1));
            i2s_ptr->RCCR |= (I2S_RCCR_PSR_MASK);
            i2s_ptr->RCCR &= ~(I2S_RCCR_DIV2_MASK);
         }
         else if (divider > 2048)
         {
            divider = divider/16;
            i2s_ptr->TCCR &= ~(I2S_TCCR_PM_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_PM(divider - 1));
            i2s_ptr->TCCR |= (I2S_TCCR_PSR_MASK);
            i2s_ptr->TCCR |= (I2S_TCCR_DIV2_MASK);
            /* Set receiver clock */
            i2s_ptr->RCCR &= ~(I2S_RCCR_PM_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_PM(divider - 1));
            i2s_ptr->RCCR |= (I2S_RCCR_PSR_MASK);
            i2s_ptr->RCCR |= (I2S_RCCR_DIV2_MASK);
         }
         io_info_ptr->BCLK_DIV = divider * 2;
         io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
         /* The word length is fixed to 32 in I2S master mode and DC is 1
         ** thus frame sync frequency is always BCLK_FREQ/64
         */
         io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / 64;
    }
    return(MQX_OK);
}
/* EOF */
