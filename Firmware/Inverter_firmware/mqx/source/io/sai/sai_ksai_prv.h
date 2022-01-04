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
* $FileName: sai_ksai_prv.h$
* $Version : 3.8.3.0$
* $Date    : Sep-11-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the SAI/I2S driver
*
*END************************************************************************/
#ifndef __SAI_KSAI_PRV_H__
#define __SAI_KSAI_PRV_H__

#include "sai_ksai.h"
#include "sai_audio.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* Limits for master clock divider */
#define FRACT_MAX 256
#define DIV_MAX 4096

/*
** SAI limits
*/

#define BCLK_DIV_MIN    1
#define BCLK_DIV_MAX    4096

/* Hardware FIFO size */
#if !PSP_MQX_CPU_IS_VYBRID
  #define SIZE_OF_FIFO    8
#else
  #define SIZE_OF_FIFO    32
#endif

#define MEM_TYPE_IO_I2S_INFO_STRUCT     1046
#define MEM_TYPE_IO_I2S_LEFT_BUFFER     1047
#define MEM_TYPE_IO_I2S_RIGHT_BUFFER    1048

/* Number of data channels on both RX and TX */
#if !PSP_MQX_CPU_IS_VYBRID
  #define SAI_DATA_CHANNELS 2
#else
  #define SAI_DATA_CHANNELS 1
#endif

#define DEFAULT_BCLK_DIV 8


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** SAI software buffer structure
*/
typedef struct ksai_buffer
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
    
} KSAI_BUFFER, _PTR_ KSAI_BUFFER_PTR;

typedef struct ksai_dummy_data
{
    /* Pointer to dummy data (sine wave) */
    int_16_ptr DATA;

    /* Length in samples of dummy data */
    uint_32 LENGTH;

    /* Output index */
    uint_32 INDEX;

} KSAI_DUMMY_DATA, _PTR_  KSAI_DUMMY_DATA_PTR;

typedef struct ksai_info_struct
{  
    /* Current initialized values */
    KSAI_INIT_STRUCT INIT;

    I2S_MemMapPtr SAI_PTR;

    /* The previous TX interrupt handler and data */
    void (_CODE_PTR_ OLD_TX_ISR)(pointer);
    pointer  OLD_TX_ISR_DATA;
    
    /* The previous TX interrupt handler and data */
    void (_CODE_PTR_ OLD_RX_ISR)(pointer);
    pointer  OLD_RX_ISR_DATA;
    
    /* Selected SAI HW module */
    uint_8 HW_CHANNEL;
    
    /* I2S TX channel */
    uint_8 TX_CHANNEL;

    /* I2S RX channel */
    uint_8 RX_CHANNEL;
    
    /* Divider of fsys to create SSI clock*/
    uint_8 MCLK_DIV;
    
    /* Master clock source*/
    CM_CLOCK_SOURCE MCLK_SRC;
    
    /* Bit clock divider */
    uint_16 BCLK_DIV;

    /* 
    ** Oversampling clock frequency in Hz - only valid 
    ** when internal clock source is selected, ignored otherwise 
    */
    uint_32 MCLK_FREQ;
    
    /* 
    ** Bit clock frequency in Hz - only valid 
    ** when internal clock source is selected, ignored otherwise 
    */
    uint_32 BCLK_FREQ;

    /* 
    ** Frame sync frequency in Hz - only valid 
    ** when internal clock source is selected, ignored otherwise 
    */
    uint_32 FS_FREQ;
    
    /* I2S module clock source (only affects SAI master mode) */
    uint_8 CLOCK_SOURCE;

    /* I2S mode of operation (master/slave)*/
    uint_8 MODE;

    /* I2S I/O mode (write/read) */
    uint_8 IO_MODE;

    /* Audio data input/output format */
    AUDIO_DATA_FORMAT IO_FORMAT;

    /* Transmit SAI clocks and dummy data when buffer is empty */
    boolean TX_DUMMY;

    /* Number of valid data bits */
    uint_8 DATA_BITS;

    /* Transmission in progress */
    boolean ONTHEWAY;

    /* Pointer to the buffer to use for current data */
    KSAI_BUFFER BUFFER;

    /* Transmission statistics */
    I2S_STATISTICS_STRUCT STATS;

    /* Data for dummy transmission*/
    KSAI_DUMMY_DATA DUMMY;

    /* Pointer to data that are going to be processed in channel*/
    char_ptr DATA;

    /* Event signaling finished read/write operation */
    LWSEM_STRUCT EVENT_IO_FINISHED;
    
    /* Clock state when no tx/rx */
    boolean CLK_ALWAYS_ON;

    /* First Input/Output operation */
    boolean FIRST_IO;
    
    /* Clock setup: sync-async; bitclock: normal-swapped */
    uint_8 CLOCK_MODE;
    
} KSAI_INFO_STRUCT, _PTR_ KSAI_INFO_STRUCT_PTR;

#endif /* __SAI_KSAI_PRV_H__ */

/* EOF */
