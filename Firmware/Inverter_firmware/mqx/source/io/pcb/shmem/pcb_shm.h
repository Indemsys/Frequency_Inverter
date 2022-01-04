#ifndef __pcb_shm_h__
#define __pcb_shm_h__
/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: pcb_shm.h$
* $Version : 3.8.6.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains the definitions for the PCB device driver that
*   sends and receives packets over a asynchrnous serial device.  The
*   packets are in MQX IPC async packet format.
*
*END************************************************************************/

#include "bsp.h"

/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/

/*
** Initialization errors
*/
#define IO_PCB_SHM_DEVICE_ALREADY_OPEN         (IO_PCB_ERROR_BASE|0x90)
#define IO_PCB_SHM_INCORRECT_SERIAL_DEVICE     (IO_PCB_ERROR_BASE|0x91)

#if 0
/*
**             PACKET STRUCTURE CONTROL FIELD BIT DEFINITIONS
*/

#define IO_PCB_SHM_HDR_LITTLE_ENDIAN           (0x40)
#define IO_PCB_SHM_DATA_LITTLE_ENDIAN          (0x20)

#define IO_PCB_SHM_HDR_BIG_ENDIAN              (0x00)
#define IO_PCB_SHM_DATA_BIG_ENDIAN             (0x00)
#endif

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/

/*
** IO_PCB_SHM_INIT_STRUCT
** This structure contains the initialization information for the
** async shared memory protocol
**
*/
typedef struct io_pcb_shm_init_struct
{

   /* Shared memory base address */
   pointer    TX_BD_ADDR;

   /* TX ring limit */
   pointer    TX_LIMIT_ADDR;

   /* Shared memory base address */
   pointer    RX_BD_ADDR;

   /* RX ring limit */
   pointer    RX_LIMIT_ADDR;

   /* Maximum size of input packet */
   _mem_size  INPUT_MAX_LENGTH;

   /* interrupt vector */
   uint_32    RX_VECTOR;
   uint_32    TX_VECTOR;
   uint_32    REMOTE_RX_VECTOR;
   uint_32    REMOTE_TX_VECTOR;

   /* The address of function to trigger interrupts */
   void (_CODE_PTR_ INT_TRIGGER)(uint_32);

} IO_PCB_SHM_INIT_STRUCT, _PTR_ IO_PCB_SHM_INIT_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern pointer _bsp_vtop(pointer ptr);
extern pointer _bsp_ptov(pointer ptr);

extern _mqx_uint _io_pcb_shm_install(char _PTR_, pointer);
#if BSPCFG_IO_PCB_SHM_SUPPORT
extern void _bsp_io_pcb_shm_int_trigger(uint_32 vector);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
