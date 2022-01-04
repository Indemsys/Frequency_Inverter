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
* $FileName: i2c_pol_ki2c.c$
* $Version : 3.8.11.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains low level functions for the I2C polled device driver
*   for Kinetis family.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include "i2c.h"
#include "i2c_pol_prv.h"
#include "i2c_ki2c_prv.h"

static uint_32 _ki2c_set_baudrate (uint_32, uint_32);
extern uint_32 _ki2c_polled_init (KI2C_INIT_STRUCT_PTR, pointer _PTR_, char_ptr);
extern uint_32 _ki2c_polled_deinit (IO_I2C_POLLED_DEVICE_STRUCT_PTR, VKI2C_INFO_STRUCT_PTR);
extern uint_32 _ki2c_polled_rx (IO_I2C_POLLED_DEVICE_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 _ki2c_polled_tx (IO_I2C_POLLED_DEVICE_STRUCT_PTR, uchar_ptr, uint_32);
extern uint_32 _ki2c_polled_ioctl (VKI2C_INFO_STRUCT_PTR, uint_32, uint_32_ptr);
static uint_32 _ki2c_polled_rx_tx (I2C_MemMapPtr, VKI2C_INFO_STRUCT_PTR, uchar_ptr, uint_32);


static const uint_32 BAUDRATE_MULT[] = { 1, 2, 4 };
static const uint_32 BAUDRATE_ICR[]= {  20, 22, 24, 26, 28, 30, 34, 40, 28, 32, 
    36, 40, 44, 48, 56, 68, 48, 56, 64, 72, 80, 88, 104, 128, 80, 96, 112, 128,
    144, 160, 192, 240, 160, 192, 224, 256, 288, 320, 384, 480, 320, 384, 448, 
    512, 576, 640, 768, 960, 640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 
    1536, 1792, 2048, 2304, 2560, 3072, 2840 };

/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_set_baudrate
* Returned Value   : uint_32 divider register setting
* Comments         :
*    Find closest setting of divider register for given baudrate.
*
*END*********************************************************************/

static uint_32 _ki2c_set_baudrate 
    (
        /* [IN] Module input clock in Hz */
        uint_32 clock, 
        
        /* [IN] Desired baudrate in Hz */
        uint_32 baudrate
    ) 
{
    #define MAX_ICR 64
    uint_32 mult, icr, min, minmult = 0, minicr = 0;
    int_32  val;
    
    min = (uint_32)-1;
    /* We will find the nearest smallest diference in desired and real baudrate.
    ** This is transformed to find smallest clock diference.
    ** IIC baud rate = bus speed (Hz)/(mul * SCL divider) =>
    ** IIC baud rate * mul * SCL divider <= clock
    */
    for (mult = 0; mult < 3; mult++) 
    {
        for (icr = 0; icr < MAX_ICR; icr++) 
        {
            val = BAUDRATE_MULT[mult] * BAUDRATE_ICR[icr] * baudrate - clock;
            if (val < 0) val = -val;
            if (min > val) 
            {
                min = val;
                minmult = mult;
                minicr = icr;
            }
        }
    }
    return I2C_F_MULT(minmult) | I2C_F_ICR(minicr);
}


/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_install
* Returned Value   : MQX error code
* Comments         :
*    Install an I2C device.
*
*END*********************************************************************/

uint_32 _ki2c_polled_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr              identifier,
  
      /* [IN] The I/O init data pointer */
      KI2C_INIT_STRUCT_CPTR init_data_ptr
   )
{ /* Body */

   
#if PE_LDD_VERSION
   if (PE_PeripheralUsed((uint_32)_bsp_get_i2c_base_address(init_data_ptr->CHANNEL)))
   {
      /* IO Device used by PE Component*/
      return IO_DEVICE_EXISTS;
   }
#endif

   
   return _io_i2c_polled_install(identifier,
      (uint_32 (_CODE_PTR_)(pointer, pointer _PTR_, char_ptr))_ki2c_polled_init,
      (uint_32 (_CODE_PTR_)(pointer, pointer))_ki2c_polled_deinit,
      (int_32 (_CODE_PTR_)(pointer, char_ptr, int_32))_ki2c_polled_rx,
      (int_32 (_CODE_PTR_)(pointer, char_ptr, int_32))_ki2c_polled_tx,
      (int_32 (_CODE_PTR_)(pointer, uint_32, uint_32_ptr))_ki2c_polled_ioctl, 
      (pointer)init_data_ptr);

} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an I2C device.
*
*END*********************************************************************/

uint_32 _ki2c_polled_init
   (
      /* [IN] Initialization information for the device being opened */
      KI2C_INIT_STRUCT_PTR io_init_ptr,

      /* [OUT] Address to store device specific information */
      pointer _PTR_        io_info_ptr_ptr,
       
      /* [IN] The rest of the name of the device opened */
      char_ptr             open_name_ptr

   )
{ /* Body */
   I2C_MemMapPtr           i2c_ptr;
   VKI2C_INFO_STRUCT_PTR   io_info_ptr;

   i2c_ptr = _bsp_get_i2c_base_address (io_init_ptr->CHANNEL);
   if (NULL == i2c_ptr)
   {
        return I2C_ERROR_CHANNEL_INVALID;
   }
  
   io_info_ptr = (VKI2C_INFO_STRUCT_PTR)_mem_alloc_system_zero ((uint_32)sizeof (KI2C_INFO_STRUCT));
   if (NULL == io_info_ptr) 
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type ((pointer)io_info_ptr, MEM_TYPE_IO_I2C_INFO_STRUCT);

   /* I2C board specific initialization */
   _bsp_i2c_io_init (io_init_ptr->CHANNEL);

   *io_info_ptr_ptr                              = (pointer)io_info_ptr;
   io_info_ptr->INIT                             = *io_init_ptr;
   io_info_ptr->I2C_PTR                          = i2c_ptr;
   io_info_ptr->OLD_ISR                          = NULL;
   io_info_ptr->OLD_ISR_DATA                     = NULL;
   io_info_ptr->VECTOR                           = 0;
   io_info_ptr->MODE                             = io_init_ptr->MODE;
   io_info_ptr->STATE                            = I2C_STATE_READY;
   io_info_ptr->ADDRESSEE                        = 0;
   io_info_ptr->OPERATION                        = 0;
   io_info_ptr->RX_REQUEST                       = 0;
   io_info_ptr->RX_BUFFER                        = NULL;
   io_info_ptr->RX_IN                            = 0;
   io_info_ptr->RX_OUT                           = 0;
   io_info_ptr->TX_BUFFER                        = NULL;
   io_info_ptr->TX_IN                            = 0;
   io_info_ptr->TX_OUT                           = 0;
   io_info_ptr->STATISTICS.INTERRUPTS            = 0;
   io_info_ptr->STATISTICS.RX_PACKETS            = 0;
   io_info_ptr->STATISTICS.TX_PACKETS            = 0;
   io_info_ptr->STATISTICS.TX_LOST_ARBITRATIONS  = 0;
   io_info_ptr->STATISTICS.TX_ADDRESSED_AS_SLAVE = 0;
   io_info_ptr->STATISTICS.TX_NAKS               = 0;

   /* Disable and clear I2C before initializing it */
   i2c_ptr->C1 = 0x00;
  
   /* Clear out all I2C events */
   i2c_ptr->S = 0x00;

   /* Set the station address for SLAVE receive operations */      
   i2c_ptr->A1 = io_init_ptr->STATION_ADDRESS << 1;
   
   /* Set the frequency divider for the nearest found baud rate */
   i2c_ptr->F = _ki2c_set_baudrate( BSP_I2C_CLOCK, io_init_ptr->BAUD_RATE ); 
   
   /* Enable I2C */
   i2c_ptr->C1 |= I2C_C1_IICEN_MASK;

   return I2C_OK;
   
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an I2C device.
*
*END*********************************************************************/

uint_32 _ki2c_polled_deinit
   (
      /* [IN] the initialization information for the device being opened */
      IO_I2C_POLLED_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] the address of the device specific information */
      VKI2C_INFO_STRUCT_PTR           io_info_ptr
   )
{ /* Body */
   I2C_MemMapPtr                      i2c_ptr;

   if ((NULL == io_info_ptr) || (NULL == pol_io_dev_ptr)) 
   {
      return I2C_ERROR_INVALID_PARAMETER;
   }
      
   i2c_ptr = io_info_ptr->I2C_PTR;
   if (i2c_ptr->S & I2C_S_BUSY_MASK) 
   {
      return I2C_ERROR_DEVICE_BUSY;
   }
   
   /* Disable the I2C */
   i2c_ptr->C1 = 0x00;

   /* Clear the I2C events */
   i2c_ptr->S = 0xFF; 
 
   /* Free info struct */
   _mem_free (pol_io_dev_ptr->DEV_INFO_PTR);
   pol_io_dev_ptr->DEV_INFO_PTR = NULL;

   return MQX_OK;

} /* Endbody */

 
/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_ioctl
* Returned Value   : MQX error code.
* Comments         : 
*    This function performs miscellaneous services for
*    the I2C I/O device.  
*
*END*********************************************************************/

uint_32 _ki2c_polled_ioctl
   (
      /* [IN] the address of the device specific information */
      VKI2C_INFO_STRUCT_PTR io_info_ptr,

      /* [IN] The command to perform */
      uint_32               cmd,

      /* [IN] Parameters for the command */
      uint_32_ptr           param_ptr
   )
{ /* Body */
   I2C_MemMapPtr            i2c_ptr;
   uint_32                  result = MQX_OK;
   volatile uint_8          tmp, state;
   uint_32                  icr, mult;
  
   i2c_ptr = io_info_ptr->I2C_PTR;
   
   switch (cmd) 
   {
      case IO_IOCTL_FLUSH_OUTPUT:
         do 
         {
            for (tmp = 0; tmp < 0xFF; tmp++) 
               { };
            tmp = i2c_ptr->S;
         } while ((i2c_ptr->C1 & I2C_C1_TX_MASK) && ((I2C_S_BUSY_MASK == (tmp & (I2C_S_TCF_MASK | I2C_S_BUSY_MASK))) || (io_info_ptr->TX_IN != io_info_ptr->TX_OUT)));
         io_info_ptr->TX_IN = io_info_ptr->TX_OUT;
         if ((io_info_ptr->OPERATION & I2C_OPERATION_STARTED) && (0 == (tmp & I2C_S_BUSY_MASK)))
         {
             io_info_ptr->OPERATION = 0;
             io_info_ptr->RX_REQUEST = 0;
             io_info_ptr->STATE = I2C_STATE_FINISHED;
         }
         if (NULL != param_ptr) 
         {
            *param_ptr = tmp & I2C_S_RXAK_MASK;
         }
         break;
      case IO_IOCTL_I2C_REPEATED_START:
         result = I2C_ERROR_DEVICE_BUSY;
         if (i2c_ptr->C1 & I2C_C1_MST_MASK)
         {
            if (((I2C_STATE_TRANSMIT == io_info_ptr->STATE) && (io_info_ptr->TX_IN == io_info_ptr->TX_OUT))
               || ((I2C_STATE_RECEIVE == io_info_ptr->STATE) && (0 == io_info_ptr->RX_REQUEST))
               || (I2C_STATE_FINISHED == io_info_ptr->STATE))
            {
               io_info_ptr->STATE = I2C_STATE_REPEATED_START;
               result = MQX_OK;
            }
         }
         break;
      case IO_IOCTL_I2C_STOP:
         i2c_ptr->C1 &= (~ (I2C_C1_TX_MASK | I2C_C1_TXAK_MASK));
         i2c_ptr->S |= (I2C_S_ARBL_MASK | I2C_S_IICIF_MASK);
         io_info_ptr->RX_REQUEST = 0;
         io_info_ptr->OPERATION = 0;
         io_info_ptr->STATE = I2C_STATE_READY;
         io_info_ptr->RX_OUT = io_info_ptr->RX_IN;

         if (! (i2c_ptr->C1 & I2C_C1_MST_MASK))
         {
            /* As slave, release the bus */
            tmp = (uint_8)i2c_ptr->D;
         }
         else
         {
            /* As master, send STOP condition */ 
            i2c_ptr->C1 &= (~ I2C_C1_MST_MASK);           
         }
         for (tmp = 0; tmp < 0xFF; tmp++) 
            { };

         /* Wait for bus idle */
         tmp = i2c_ptr->S;
         while (tmp & I2C_S_BUSY_MASK)
         { 
            if (tmp & I2C_S_IICIF_MASK)
            {
               i2c_ptr->S |= I2C_S_IICIF_MASK;
               tmp = (uint_8)i2c_ptr->D;
            }
            tmp = i2c_ptr->S;
         }
         break;
      case IO_IOCTL_I2C_SET_RX_REQUEST:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            io_info_ptr->RX_REQUEST = *param_ptr;
         }
         break;
      case IO_IOCTL_I2C_DISABLE_DEVICE:
         i2c_ptr->C1 &= (~ I2C_C1_IICEN_MASK);
         break;
      case IO_IOCTL_I2C_ENABLE_DEVICE:
         i2c_ptr->C1 |= I2C_C1_IICEN_MASK;
         break;
      case IO_IOCTL_I2C_SET_MASTER_MODE:
         if (I2C_STATE_READY != io_info_ptr->STATE) 
         {
            result = I2C_ERROR_DEVICE_BUSY;
         } 
         else 
         {
            io_info_ptr->MODE = I2C_MODE_MASTER;
         }
         break;
      case IO_IOCTL_I2C_SET_SLAVE_MODE:
         if (I2C_STATE_READY != io_info_ptr->STATE) 
         {
            result = I2C_ERROR_DEVICE_BUSY;
         } 
         else 
         {
            io_info_ptr->MODE = I2C_MODE_SLAVE;
         }
         break;
      case IO_IOCTL_I2C_GET_MODE:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            *param_ptr = io_info_ptr->MODE;
         }
         break;
      case IO_IOCTL_I2C_SET_BAUD:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else if (i2c_ptr->S & I2C_S_BUSY_MASK) 
         {
            result = I2C_ERROR_DEVICE_BUSY;
         } 
         else 
         {
           i2c_ptr->F = _ki2c_set_baudrate( BSP_I2C_CLOCK, *param_ptr );
         }
         break;
      case IO_IOCTL_I2C_GET_BAUD:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            mult = BAUDRATE_MULT[(( i2c_ptr->F & I2C_F_MULT_MASK ) >> I2C_F_MULT_SHIFT)];
            icr = BAUDRATE_ICR[( i2c_ptr->F & I2C_F_ICR_MASK )];
            *param_ptr = (uint_32)((BSP_I2C_CLOCK) / (mult * icr));
         }
         break;
      case IO_IOCTL_I2C_SET_DESTINATION_ADDRESS:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            io_info_ptr->ADDRESSEE = *param_ptr;
         }
         break;
      case IO_IOCTL_I2C_GET_DESTINATION_ADDRESS:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            *param_ptr = io_info_ptr->ADDRESSEE;
         }
         break;
      case IO_IOCTL_I2C_SET_STATION_ADDRESS:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            i2c_ptr->A1 = I2C_A1_AD(*param_ptr);
         }
         break;
      case IO_IOCTL_I2C_GET_STATION_ADDRESS:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            *param_ptr = ( i2c_ptr->A1 & I2C_A1_AD_MASK ) >> I2C_A1_AD_SHIFT;
         }
         break;
      case IO_IOCTL_I2C_GET_STATE:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            *param_ptr = io_info_ptr->STATE;
         }
         break;
      case IO_IOCTL_I2C_GET_STATISTICS:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            *((I2C_STATISTICS_STRUCT_PTR)param_ptr) = io_info_ptr->STATISTICS;
         }
         break;         
      case IO_IOCTL_I2C_CLEAR_STATISTICS:
         io_info_ptr->STATISTICS.INTERRUPTS = 0;
         io_info_ptr->STATISTICS.RX_PACKETS = 0;
         io_info_ptr->STATISTICS.TX_PACKETS = 0;
         io_info_ptr->STATISTICS.TX_LOST_ARBITRATIONS = 0;
         io_info_ptr->STATISTICS.TX_ADDRESSED_AS_SLAVE = 0;
         io_info_ptr->STATISTICS.TX_NAKS = 0;
         break;
      case IO_IOCTL_I2C_GET_BUS_AVAILABILITY:
         if (NULL == param_ptr) 
         {
            result = I2C_ERROR_INVALID_PARAMETER;
         } 
         else 
         {
            if (i2c_ptr->S & I2C_S_BUSY_MASK) 
            {
                *param_ptr = I2C_BUS_BUSY;
            } 
            else 
            {
                *param_ptr = I2C_BUS_IDLE;
            }
         }
         break;
      default:
         break;
   }
   return result;
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_rx
* Returned Value   : number of bytes read
* Comments         : 
*   Returns the number of bytes received.
*   Reads the data into provided array when data is available.
*
*END*********************************************************************/

uint_32 _ki2c_polled_rx
   (
      /* [IN] the address of the device specific information */
      IO_I2C_POLLED_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] The array to copy data into */
      uchar_ptr                       buffer,
      
      /* [IN] number of bytes to read */
      uint_32                         length
   )
{ /* Body */
   VKI2C_INFO_STRUCT_PTR              io_info_ptr;
   I2C_MemMapPtr                      i2c_ptr;   
   uint_8                             tmp;
         
   io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
   i2c_ptr = io_info_ptr->I2C_PTR;

   /* If beginning of transmission, set state and send address (master only) */
   io_info_ptr->OPERATION |= I2C_OPERATION_READ;
   tmp = io_info_ptr->STATE;
   if ((I2C_STATE_READY == tmp) || (I2C_STATE_REPEATED_START == tmp))
   {
      io_info_ptr->STATE = I2C_STATE_RECEIVE;
      if (I2C_MODE_MASTER == io_info_ptr->MODE)
      {
         i2c_ptr->C1 |= I2C_C1_TX_MASK;
         i2c_ptr->S |= I2C_S_IICIF_MASK;
         if (I2C_STATE_REPEATED_START == tmp)
         {
            i2c_ptr->C1 |= I2C_C1_RSTA_MASK;
         }
         else
         {
            i2c_ptr->C1 |= I2C_C1_MST_MASK;
         }
         io_info_ptr->OPERATION |= I2C_OPERATION_STARTED;
         i2c_ptr->D = (io_info_ptr->ADDRESSEE << 1) | I2C_OPERATION_READ;
         io_info_ptr->STATISTICS.TX_PACKETS++;
         while (0 == (i2c_ptr->S & I2C_S_IICIF_MASK))
            { };   /* wait for address transferred */
      }
   }

   return _ki2c_polled_rx_tx (i2c_ptr, io_info_ptr, buffer, length);
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_tx
* Returned Value   : number of bytes transmitted
* Comments         : 
*   Writes the provided data buffer and loops until transmission complete.
*
*END*********************************************************************/

uint_32 _ki2c_polled_tx
   (
      /* [IN] the address of the device specific information */
      IO_I2C_POLLED_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] The array characters are to be read from */
      uchar_ptr                       buffer,
      
      /* [IN] number of bytes to output */
      uint_32                         length
   )
{ /* Body */
   VKI2C_INFO_STRUCT_PTR              io_info_ptr;
   I2C_MemMapPtr                      i2c_ptr;   
   uint_8                             tmp;
   
   io_info_ptr  = pol_io_dev_ptr->DEV_INFO_PTR;
   i2c_ptr = io_info_ptr->I2C_PTR;

   /* If beginning of transmission, set state and send address (master only) */
   io_info_ptr->OPERATION &= (~ I2C_OPERATION_READ);
   tmp = io_info_ptr->STATE;
   if ((I2C_STATE_READY == tmp) || (I2C_STATE_REPEATED_START == tmp))
   {
      io_info_ptr->STATE = I2C_STATE_TRANSMIT;
      if (I2C_MODE_MASTER == io_info_ptr->MODE)
      {
         i2c_ptr->C1 |= I2C_C1_TX_MASK;
         i2c_ptr->S |= I2C_S_IICIF_MASK;
         if (I2C_STATE_REPEATED_START == tmp)
         {
            i2c_ptr->C1 |= I2C_C1_RSTA_MASK;
         }
         else
         {
            i2c_ptr->C1 |= I2C_C1_MST_MASK;
         }
         io_info_ptr->OPERATION |= I2C_OPERATION_STARTED;
         i2c_ptr->D = (io_info_ptr->ADDRESSEE << 1) | I2C_OPERATION_WRITE;
         io_info_ptr->STATISTICS.TX_PACKETS++;
         while (0 == (i2c_ptr->S & I2C_S_IICIF_MASK))
            { };   /* wait for address transferred */
      }
   }

   return _ki2c_polled_rx_tx (i2c_ptr, io_info_ptr, buffer, length);
} /* Endbody */


/*FUNCTION****************************************************************
* 
* Function Name    : _ki2c_polled_rx_tx
* Returned Value   : number of bytes processed
* Comments         : 
*   Actual data transfer on I2C bus.
*
*END*********************************************************************/

static uint_32 _ki2c_polled_rx_tx
   (
      /* [IN] I2C register structure */
      I2C_MemMapPtr         i2c_ptr,

      /* [IN] I2C state structure */
      VKI2C_INFO_STRUCT_PTR io_info_ptr,
      
      /* [IN] The buffer for IO operation */
      uchar_ptr             buffer,
      
      /* [IN] Number of bytes in buffer */
      uint_32               length
   )
{ /* Body */
   uint_32                  i;
   uint_8                   i2csr;
   volatile uint_8          tmp;

   for (i = 0; i <= length; i++)
   {
      do 
      {
         i2csr = i2c_ptr->S;
         if (0 == (i2c_ptr->C1 & I2C_C1_MST_MASK))
         {
            if (i == length)
            {
               return i;
            }
            if ((io_info_ptr->OPERATION & I2C_OPERATION_STARTED) && (0 == (i2csr & I2C_S_BUSY_MASK)))
            {
               io_info_ptr->OPERATION = 0;
               io_info_ptr->RX_REQUEST = 0;
               io_info_ptr->STATE = I2C_STATE_FINISHED;
               return i;
            }
         }
      } while (0 == (i2csr & I2C_S_IICIF_MASK));

      io_info_ptr->OPERATION |= I2C_OPERATION_STARTED;
   
      /* Master */
      if (i2c_ptr->C1 & I2C_C1_MST_MASK)
      {
         /* Transmit */
         if (i2c_ptr->C1 & I2C_C1_TX_MASK)
         {
            /* Not ack */
            if (i2csr & I2C_S_RXAK_MASK)
            {
               i2c_ptr->S |= I2C_S_IICIF_MASK;
               io_info_ptr->STATE = I2C_STATE_FINISHED;
               io_info_ptr->STATISTICS.TX_NAKS++;
               length = i;
            }
            /* Ack */
            else
            {
               /* Transmit requested */
               if (I2C_STATE_TRANSMIT == io_info_ptr->STATE)
               {
                  if (0 == (io_info_ptr->OPERATION & I2C_OPERATION_READ))
                  {
                     /* Anything to transmit? */
                     if (i < length)
                     {
                        i2c_ptr->S |= I2C_S_IICIF_MASK;
                        i2c_ptr->D = *buffer++;   /* transmit data */
                        io_info_ptr->STATISTICS.TX_PACKETS++;
                     }
                  }
                  else
                  {
                     length = i;
                  }
               }
               /* Receive requested */
               else if (I2C_STATE_RECEIVE == io_info_ptr->STATE)
               {
                  if (0 != (io_info_ptr->OPERATION & I2C_OPERATION_READ))
                  {
                     i2c_ptr->C1 &= (~ I2C_C1_TX_MASK);
                     if (1 == io_info_ptr->RX_REQUEST)
                     {
                        i2c_ptr->C1 |= I2C_C1_TXAK_MASK;
                     }
                     else
                     {
                        i2c_ptr->C1 &= (~ I2C_C1_TXAK_MASK);
                     }
                     i2c_ptr->S |= I2C_S_IICIF_MASK;
                     if (0 == io_info_ptr->RX_REQUEST)
                     {
                        io_info_ptr->STATE = I2C_STATE_FINISHED;
                        length = i;
                     }
                     else
                     {
                        tmp = i2c_ptr->D;   /* dummy read to clock in 1st byte */
                        i--;
                     }
                  }
                  else
                  {
                     length = i;
                  }
               }
               /* Others are unwanted/not handled states */
               else
               {
                  length = i;
               }
            }
         }
         /* Receive */
         else
         {
            /* Receive requested */
            if (I2C_STATE_RECEIVE == io_info_ptr->STATE)
            {
               if ((0 != (io_info_ptr->OPERATION & I2C_OPERATION_READ)) && (0 != io_info_ptr->RX_REQUEST))
               {
                  if (i < length)
                  {
                     i2c_ptr->S |= I2C_S_IICIF_MASK;
                     io_info_ptr->RX_REQUEST--;
                     if (1 == io_info_ptr->RX_REQUEST)
                     {
                        i2c_ptr->C1 |= I2C_C1_TXAK_MASK;
                     }
                     else
                     {
                        i2c_ptr->C1 &= (~ I2C_C1_TXAK_MASK);
                     }
                     if (0 == io_info_ptr->RX_REQUEST)
                     {
                        i2c_ptr->C1 |= I2C_C1_TX_MASK;   /* no more reading */
                     }
                     *buffer++ = i2c_ptr->D;   /* receive data */
                     if (0 == io_info_ptr->RX_REQUEST)
                     {
                        i2c_ptr->C1 &= (~ I2C_C1_TX_MASK);
                        io_info_ptr->STATE = I2C_STATE_FINISHED;
                        length = i + 1;
                        i = length;
                     }
                     io_info_ptr->STATISTICS.RX_PACKETS++;
                  }
               }
               else
               {
                  length = i;
               }
            }
            /* Others are unwanted/not handled states */
            else 
            {
               length = i;
            }
         }
      }
      /* Slave */
      else
      {
         /* Master arbitration lost */
         if (i2csr & I2C_S_ARBL_MASK)
         {
            i2c_ptr->S |=  I2C_S_ARBL_MASK;
            io_info_ptr->STATE = I2C_STATE_LOST_ARBITRATION;
            io_info_ptr->STATISTICS.TX_LOST_ARBITRATIONS++;
            length = i;
         }
         /* Addressed as slave */
         if (i2csr & I2C_S_IAAS_MASK)
         {
            if (I2C_MODE_MASTER == io_info_ptr->MODE)
            {
               io_info_ptr->STATISTICS.TX_ADDRESSED_AS_SLAVE++;
            }
            /* Transmit requested */
            if (i2csr & I2C_S_SRW_MASK)
            {  
               io_info_ptr->STATE = I2C_STATE_ADDRESSED_AS_SLAVE_TX;
               if (0 == (io_info_ptr->OPERATION & I2C_OPERATION_READ))
               {
                  if (i < length)
                  {
                     i2c_ptr->C1 |= I2C_C1_TX_MASK;
                     i2c_ptr->S |= I2C_S_IICIF_MASK;
                     i2c_ptr->D = *buffer++;   /* transmit data */
                     io_info_ptr->STATISTICS.TX_PACKETS++;
                  }
               }
               else
               {
                  length = i;
               }
            }
            /* Receive requested */
            else
            {
               io_info_ptr->STATE = I2C_STATE_ADDRESSED_AS_SLAVE_RX;
               if ((0 != (io_info_ptr->OPERATION & I2C_OPERATION_READ)) && (i == 0) && (0 != io_info_ptr->RX_REQUEST))
               {
                  i2c_ptr->C1 &= (~ I2C_C1_TX_MASK);
                  i2c_ptr->S |= I2C_S_IICIF_MASK;
                  if (1 == io_info_ptr->RX_REQUEST)
                  {
                     i2c_ptr->C1 |= I2C_C1_TXAK_MASK;
                  }
                  else
                  {
                     i2c_ptr->C1 &= (~ I2C_C1_TXAK_MASK);
                  }
                  tmp = i2c_ptr->D;   /* dummy read to release bus */
                  i--;
               }
               else
               {
                  length = i;
               }
            }
         }
         /* Normal slave operation */
         else
         {
            /* No master arbitration lost */
            if (! (i2csr & I2C_S_ARBL_MASK))
            {
               /* Transmit */
               if (i2c_ptr->C1 & I2C_C1_TX_MASK)
               {
                  /* Not ack */
                  if (i2csr & I2C_S_RXAK_MASK)
                  {
                     i2c_ptr->S |= I2C_S_IICIF_MASK;
                     io_info_ptr->STATE = I2C_STATE_FINISHED;
                     io_info_ptr->STATISTICS.TX_NAKS++;
                     length = i;
                  }
                  /* Ack */
                  else
                  {
                     /* Transmit requested */
                     if (((I2C_STATE_TRANSMIT == io_info_ptr->STATE) || (I2C_STATE_ADDRESSED_AS_SLAVE_TX == io_info_ptr->STATE))
                        && (0 == (io_info_ptr->OPERATION & I2C_OPERATION_READ)))
                     {
                        if (i < length)
                        {
                           i2c_ptr->S |= I2C_S_IICIF_MASK;
                           i2c_ptr->D = *buffer++;   /* transmit data */
                           io_info_ptr->STATISTICS.TX_PACKETS++;
                        }
                     }
                     else
                     {
                        length = i;
                     }
                  }
               }
               /* Receive */
               else
               {
                  /* Receive requested */
                  if (((I2C_STATE_RECEIVE == io_info_ptr->STATE) || (I2C_STATE_ADDRESSED_AS_SLAVE_RX == io_info_ptr->STATE))
                     && (0 != (io_info_ptr->OPERATION & I2C_OPERATION_READ)) && (0 != io_info_ptr->RX_REQUEST))
                  {
                     if (i < length)
                     {
                        i2c_ptr->S |= I2C_S_IICIF_MASK;
                        io_info_ptr->RX_REQUEST--;
                        if (1 == io_info_ptr->RX_REQUEST)
                        {
                           i2c_ptr->C1 |= I2C_C1_TXAK_MASK;
                        }
                        else
                        {
                           i2c_ptr->C1 &= (~ I2C_C1_TXAK_MASK);
                        }
                        if (0 == io_info_ptr->RX_REQUEST)
                        {
                           io_info_ptr->STATE = I2C_STATE_FINISHED;
                        }
                        *buffer++ = i2c_ptr->D;   /* receive data */
                        io_info_ptr->STATISTICS.RX_PACKETS++;
                     }
                  }
                  else
                  {
                     length = i;
                  }
               }
            }
            else
            {
               length = i;
            }
         }
      }
   }
   return length;
} /* Endbody */

/* EOF */
