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
* $FileName: sai_ksai.h$
* $Version : 3.8.4.0$
* $Date    : Sep-11-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the SAI driver
*
*END************************************************************************/

#ifndef __SAI_KSAI_H__
#define __SAI_KSAI_H__

#include "sai_audio.h"

#if !PSP_MQX_CPU_IS_VYBRID
#include <cm_kinetis.h>
#else
#include <cm_vybrid.h>
#endif
/*
** Constants
*/

/* Maximum value  that can be stored in 16 bit long variable */
#define BIT16_MAX 32767

/*
** Types declaration
*/ 
#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct ki2s_init_struct
{
    /* Selected SAI HW module */
    uint_8 HW_CHANNEL;
        
    /* The SAI TX channel to initialize */
    uint_8 TX_CHANNEL;
    
    /* The SAI RX channel to initialize */
    uint_8 RX_CHANNEL;
    
    /* Clock setup: sync-async; bitclock: normal-swapped */
    uint_8 CLOCK_MODE;

    /* Default operating mode */
    uint_8 MODE;

    /* Number of valid data bits*/
    uint_8 DATA_BITS;

    /* I2S master clock source*/
    uint_8 CLOCK_SOURCE;

    /* Transmit dummy data */
    boolean TX_DUMMY;

    /* Interrupt level to use */
    _mqx_uint LEVEL;

    /* Tx buffer size */
    uint_32 BUFFER_SIZE;
    
    /* Internal master clock source */
    CM_CLOCK_SOURCE MCLK_SRC;
    
    /* I/O data format */
    AUDIO_DATA_FORMAT const * IO_FORMAT;

} KSAI_INIT_STRUCT, _PTR_ KSAI_INIT_STRUCT_PTR;

typedef const KSAI_INIT_STRUCT _PTR_ KSAI_INIT_STRUCT_CPTR;

/*
** Global functions
*/

_mqx_int _ksai_int_init(pointer);
_mqx_int _ksai_int_deinit(pointer, pointer);    
_mqx_int _ksai_int_tx(pointer, char_ptr, uint_32);
_mqx_int _ksai_int_rx(pointer, char_ptr, uint_32);
_mqx_int _ksai_int_ioctl(pointer, _mqx_int, pointer);

pointer _bsp_get_sai_base_address(uint_8);
uint_32 _bsp_get_sai_tx_vector(uint_8);
uint_32 _bsp_get_sai_rx_vector(uint_8);
    
#ifdef __cplusplus
}
#endif

#endif /* __SAI_KSAI_H__ */

/* EOF */
