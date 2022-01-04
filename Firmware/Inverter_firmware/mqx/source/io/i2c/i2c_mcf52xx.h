#ifndef _i2c_mcf52xx_h
#define _i2c_mcf52xx_h 1
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
* $FileName: i2c_mcf52xx.h$
* $Version : 3.5.7.0$
* $Date    : Nov-26-2009$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the I2C drivers for the MCF52XX.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** MCF52XX_I2C_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct mcf52xx_i2c_init_struct
{
   /* The I2C channel to initialize */
   uint_8                 CHANNEL;

   /* Default operating mode */
   uint_8                 MODE;

   /* The I2C station address for the channel */
   uint_8                 STATION_ADDRESS;

   /* Desired baud rate */
   uint_32                BAUD_RATE;

   /* Interrupt level to use if interrupt driven */
   _int_level             LEVEL;

   /* Sub level within the interrupt level to use if interrupt driven */
   _int_priority          SUBLEVEL;

   /* Tx buffer size (interrupt only) */
   uint_32                TX_BUFFER_SIZE;

   /* Rx buffer size (interrupt only) */
   uint_32                RX_BUFFER_SIZE;

} MCF52XX_I2C_INIT_STRUCT, _PTR_ MCF52XX_I2C_INIT_STRUCT_PTR;

typedef const MCF52XX_I2C_INIT_STRUCT _PTR_ MCF52XX_I2C_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _mcf52xx_i2c_polled_install (char_ptr, MCF52XX_I2C_INIT_STRUCT_CPTR);
extern uint_32 _mcf52xx_i2c_int_install (char_ptr, MCF52XX_I2C_INIT_STRUCT_CPTR);
extern pointer _bsp_get_i2c_base_address (uint_8);
extern uint_32 _bsp_get_i2c_vector (uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
