#ifndef _i2s_ki2s_prv_h
#define _i2s_ki2s_prv_h 
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
* $FileName: i2s_ki2s_prv.h$
* $Version : 3.8.4.0$
* $Date    : Aug-20-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the I2S drivers for 54XX Coldfire family.
*
*END************************************************************************/
#include "i2s_ki2s.h"
#include "i2s_audio.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** I2S defaults
*/
#define I2S_DEFAULT_BUFFER_SIZE 64
#define I2S_DEFAULT_DATA_BITS   16
#define I2S_DEFAULT_CHANNEL     0
#define I2S_DEFAULT_MODE        I2S_MODE_MASTER
#define I2S_DEFAULT_CLK_SOURCE  I2S_CLK_INT
/*
** I2S Registers
*/
#define MCF54XX_CCM_MISCCR_SSI0SRC (0x10)
#define MCF54XX_CCM_MISCCR_SSI1SRC (0x20)
#define MCF54XX_CCM_MISCCR_CDRH_SSI0DIV(x) ((x) << 8)
#define MCF54XX_CCM_MISCCR_CDRH_SSI1DIV(x) (x)
/*
** I2S limits
*/
#define SSIDIV_MIN 3
#define SSIDIV_MAX 100
#define BCLK_DIV_MIN 1
#define BCLK_DIV_MAX 4096
#define FIFO_WATERMARK_MAX 15
#define SSI0_SLOT 47
#define SSI1_SLOT 50

//#define I2S_DEBUG 1
#define MEM_TYPE_IO_I2S_INFO_STRUCT 1046
#define MEM_TYPE_IO_I2S_LEFT_BUFFER 1047
#define MEM_TYPE_IO_I2S_RIGHT_BUFFER 1048

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** I2S software buffer structure
*/
typedef struct ki2s_buffer
{
	/* Pointer to data */
	uint_32_ptr DATA;
	
	/* Buffer size */
	uint_32 SIZE;
	
	/* Data input index */
	uint_32 IN;
	
	/* Data output index */
	uint_32 OUT;
	
	/* Free space in buffer */
	uint_32 SPACE;
		
} KI2S_BUFFER, _PTR_ KI2S_BUFFER_PTR;

/*
**
*/
typedef struct ki2s_dummy_data
{
   /* Pointer to dummy data (sine wave) */
   int_16_ptr DATA;
   
   /* Length in samples of dummy data */
   uint_32 LENGTH;
   
   /* Output index left */
   uint_32 INDEX_L;
   
   /* Output index right */
   uint_32 INDEX_R;
   
} KI2S_DUMMY_DATA, _PTR_  KI2S_DUMMY_DATA_PTR;

/*
** I2S_STATISTICS_STRUCT
** I2S run time statistics structure
*/
typedef struct i2s_statistics_struct_prv
{
    /* Number of I2S interrupts so far */
    uint_32 INTERRUPTS;

    /* FIFO0 underruns */
    uint_32 UNDERRUNS_L;

    /* FIFO1 underruns */
    uint_32 UNDERRUNS_R;

    /* FIFO0 overruns */
    uint_32 OVERRUNS_L;

    /* FIFO1 underruns */
    uint_32 OVERRUNS_R;

    /* Number of valid bytes received (not dummy receives) */
    uint_32 RX_PACKETS;

    /* Number of valid bytes transmitted (not dummy transmits) */
    uint_32 TX_PACKETS;

    /* Number of bytes currently read or wrote - left */
    uint_32 PACKETS_PROCESSED_L;

    /* Number of bytes currently buffered - left */
    uint_32 PACKETS_QUEUED_L;

    /* Number of bytes requested for reading or writing - left */
    uint_32 PACKETS_REQUESTED_L;

    /* Number of bytes currently read or wrote - right */
    uint_32 PACKETS_PROCESSED_R;

    /* Number of bytes currently buffered - right */
    uint_32 PACKETS_QUEUED_R;

    /* Number of bytes requested for reading or writing - right */
    uint_32 PACKETS_REQUESTED_R;

} I2S_STATISTICS_STRUCT_PRV, _PTR_ I2S_STATISTICS_STRUCT_PRV_PTR;

/*
** KI2S_INFO_STRUCT
** I2S run time state information
*/
typedef struct ki2s_info_struct
{  
   /* Current initialized values */
   KI2S_INIT_STRUCT INIT;
   
   I2S_MemMapPtr I2S_PTR;
   
   /* The previous interrupt handler and data */
   void (_CODE_PTR_ OLD_ISR)(pointer);
   pointer  OLD_ISR_DATA;

   /* SSI channel */
   uint_8 CHANNEL;
   
   /* Divider of fsys to create SSI clock*/
   uint_8 MCLK_DIV;
   
   /* Bit clock divider */
   uint_16 BCLK_DIV;
   
   /* Oversampling clock frequency in Hz - only valid when internal clock source
	is selected, ignored otherwise */
   uint_32 MCLK_FREQ;
	
   /* Bit clock frequency in Hz - only valid when internal clock source
	is selected, ignored otherwise */
   uint_32 BCLK_FREQ;

   /* Frame sync frequency in Hz - only valid when internal clock source
	is selected, ignored otherwise */
   uint_32 FS_FREQ;
	
   /* SSI module clock source (only affects I2S master mode) */
   uint_8 CLOCK_SOURCE;
   
   /* I2S mode of operation (master/slave)*/
   uint_8 MODE;
   
   /* I2S I/O mode (write/read) */
   uint_8 IO_MODE;
   
   /* Audio data input/output format */
   AUDIO_DATA_FORMAT IO_FORMAT;
   
   /* Transmit I2S clocks and dummy data when buffer is empty */
   boolean TX_DUMMY;
   
   /* Number of valid data bits */
   uint_8 DATA_BITS;
   
   /* Transmission in progress */
   boolean ONTHEWAY;

   /* Pointer to the buffer to use for current data - left */
   KI2S_BUFFER BUFFER_L;
   
   /* Pointer to the buffer to use for current data - right */
   KI2S_BUFFER BUFFER_R;
   
   /* Transmission statistics */
   I2S_STATISTICS_STRUCT_PRV STATS;
   
   /* Data for dummy transmission*/
   KI2S_DUMMY_DATA DUMMY;
   
   /* Pointer to data that are going to be processed in left channel*/
   char_ptr DATA_L;
   
   /* Pointer to data that are going to be processed in right channel*/ 
   char_ptr DATA_R;
   
   /* Event signaling finished read/write operation */
   LWSEM_STRUCT EVENT_IO_FINISHED;
   
   /* Clock state when no tx/rx */
   boolean CLK_ALWAYS_ON;
   
   /* First Input/Output operation */
   boolean FIRST_IO;
   
   /* Mask for receive interrupts enabling/disabling*/
   uint_32 RX_INT_MASK;
   
   /* Mask for transmitt interrupts enabling/disabling*/
   uint_32 TX_INT_MASK;
   
} KI2S_INFO_STRUCT, _PTR_ KI2S_INFO_STRUCT_PTR;

#endif
/* EOF */
