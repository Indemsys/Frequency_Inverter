/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2011 Embedded Access Inc.;
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
* $FileName: serl_linflexd.h$
* $Version : 3.8.3.0$
* $Date    : Oct-5-2011$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the linflexd drivers for the px56xx
*
*END************************************************************************/

#ifndef _serial_linflexd_h_
#define _serial_linflexd_h_

#ifdef __cplusplus
extern "C" {
#endif


#define LINFLEXD_BUFFER_MODE    0
#define LINFLEXD_FIFO_MODE      0x80000000 /* used as CONTROL_BITS flag */

#define LINFLEXD_NUM_VECTORS 3
#define LINFLEXD_RX_VECTOR_INDEX 0
#define LINFLEXD_TX_VECTOR_INDEX 1
#define LINFLEXD_ERR_VECTOR_INDEX 2


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** LINFLEXD_SERIAL_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct linflexd_serial_init_struct
{
    // The device to initialize
    uint_32     DEVICE;

    // The serial I/O queue size to use to buffer Rx/Tx data.
    _mqx_uint   QUEUE_SIZE;

    // The input clock speed
    uint_32     CLOCK_SPEED;

    // The baud rate for the channel
    uint_32     BAUD_RATE;

    // The number of bits in a character
    _mqx_uint   BITS_PER_CHARACTER;

    // The parity to initialize the channel to
    _mqx_uint   PARITY;

    // The number of stop bits
    _mqx_uint   STOP_BITS;

    // Interrupt priority */
    uint_32    PRIORITY;
} LINFLEXD_SERIAL_INIT_STRUCT, _PTR_ LINFLEXD_SERIAL_INIT_STRUCT_PTR;

typedef const LINFLEXD_SERIAL_INIT_STRUCT _PTR_ LINFLEXD_SERIAL_INIT_STRUCT_CPTR;


/*
** LINFLEXD_STATS_STRUCT
** Run time state information for each serial channel
*/
typedef struct linflexd_serial_stats_struct {
    /* Statistical information */
    uint_32                        INTERRUPTS;
    uint_32                        RX_DROPPED_INPUT;
    uint_32                        RX_GOOD_CHARS;
    uint_32                        RX_CHARS;
    uint_32                        TX_CHARS;
    uint_32                        RX_PARITY_ERRORS;
    uint_32                        RX_FRAMING_ERRORS;
    uint_32                        RX_OVERRUNS;
    uint_32                        RX_BREAKS;
    uint_32                        RX_NOISE_ERRORS;
} LINFLEXD_STATS_STRUCT, _PTR_ LINFLEXD_STATS_STRUCT_PTR;


/*
** LINFLEXD_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct linflexd_serial_info_struct
{
    /* The current init values for this port */
    LINFLEXD_SERIAL_INIT_STRUCT    INIT;

    /* The device registers */
    VLINFLEXD_REG_STRUCT_PTR       LINFLEXD_PTR;

    /* Internal driver interrupt state */
    uint_32                        NUM_VECTORS;
    const uint_32 _PTR_            VECTORS;
    /* The previous interrupt handler and data */
    void               (_CODE_PTR_ OLD_ISR[LINFLEXD_NUM_VECTORS])(pointer);
    pointer                        OLD_ISR_DATA[LINFLEXD_NUM_VECTORS];

    uint_32                        CONTROL_BITS;

    /* Statistical information */
    LINFLEXD_STATS_STRUCT          STATS;
} LINFLEXD_SERIAL_INFO_STRUCT, _PTR_ LINFLEXD_SERIAL_INFO_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

/* Polled driver functions */
extern uint_32 _linflexd_serial_polled_install(char_ptr, LINFLEXD_SERIAL_INIT_STRUCT_CPTR);

/* Polled driver functions */
extern uint_32 _linflexd_serial_int_install(char_ptr, LINFLEXD_SERIAL_INIT_STRUCT_CPTR);

extern VLINFLEXD_REG_STRUCT_PTR _bsp_get_linflexd_base_address(uint_32 channel);
extern boolean _bsp_linflexd_enable_access(uint_32 device);
extern uint_32 _bsp_get_linflexd_vectors(uint_32 device, const uint_32 _PTR_ _PTR_ vectors_ptr);

#ifdef __cplusplus
}
#endif

#endif
