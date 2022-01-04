#ifndef __sdcard_prv_h__
#define __sdcard_prv_h__
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
* $FileName: sdcard_prv.h$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the SD card.
*
*END************************************************************************/


#include "sdcard.h"


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


/* Properties of device */
#define IO_SDCARD_ATTRIBS  (IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE | IO_DEV_ATTR_BLOCK_MODE)


/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/*
** IO_SDCARD STRUCT
**
** The address of this structure is used to maintain SD card specific 
** information.
*/
typedef struct io_sdcard_struct
{
   /* Init struct pointer */
   SDCARD_INIT_STRUCT_PTR INIT;

   /* The low level SD communication file descriptor */
   MQX_FILE_PTR           COM_DEVICE;

   /* Light weight semaphore struct */
   LWSEM_STRUCT           LWSEM;

   /* The low level response timeout >= 250 ms */
   uint_32                TIMEOUT;

   /* The number of blocks for the device */
   uint_32                NUM_BLOCKS;

   /* High capacity = block addressing */
   boolean                SDHC;

   /* Specification 2 or later card = different CSD register */
   boolean                VERSION2;

   /* Card address */
   uint_32                ADDRESS;

} SDCARD_STRUCT, _PTR_ SDCARD_STRUCT_PTR;


/* Internal functions to SD */
#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_sdcard_open(MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int _io_sdcard_close(MQX_FILE_PTR);
extern _mqx_int _io_sdcard_write_blocks(MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_sdcard_read_blocks (MQX_FILE_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_sdcard_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);

extern uint_32  _io_sdcard_csd_capacity(uint_8 *);
extern uint_32  _io_sdcard_csd_baudrate(uint_8 *);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
