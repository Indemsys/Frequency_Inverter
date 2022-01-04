#ifndef _i2s_ki2s_h
#define _i2s_ki2s_h 1
/*
**************************************************************************
*
* $FileName: i2s_ki2s.h$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the I2S drivers for the MCF54XX.
*
*END************************************************************************/

#include "i2s_audio.h"

#define BIT16_MAX 32767
/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** MCF54XX_I2S_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct ki2s_init_struct
{
   /* The I2S channel to initialize */
   uint_8 CHANNEL;
   
   /* Default operating mode */
   uint_8 MODE;
   
   /* Number of valid data bits*/
   uint_8 DATA_BITS;
   
   /* SSI_CLOCK source*/
   uint_8 CLOCK_SOURCE;
   
   /* Data are stereo */
   boolean STEREO;
   
   /* Transmit dummy data */
   boolean TX_DUMMY;
   
   /* Interrupt level to use */
   _int_level LEVEL;
   
   /* Tx buffer size */
   uint_32 BUFFER_SIZE;
   
   /* I/O data format */
   AUDIO_DATA_FORMAT const * IO_FORMAT;

} KI2S_INIT_STRUCT, _PTR_ KI2S_INIT_STRUCT_PTR;
typedef const KI2S_INIT_STRUCT _PTR_ KI2S_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _ki2s_int_install (char_ptr, KI2S_INIT_STRUCT_CPTR);
extern pointer _bsp_get_i2s_base_address (uint_8);
extern uint_32 _bsp_get_i2s_vector (uint_8);
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
