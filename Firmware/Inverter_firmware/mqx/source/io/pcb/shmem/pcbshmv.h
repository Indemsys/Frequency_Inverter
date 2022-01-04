#ifndef __pcbshmv_h__
#define __pcbshmv_h__
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
* $FileName: pcbshmv.h$
* $Version : 3.0.4.0$
* $Date    : Oct-5-2011$
*
* Comments:
*
*   This file contains the private definitions for the MQX packet format
*   PCB packet drivers operating on shared memory drivers.
*
*END************************************************************************/

#include "pcb_shm.h"

/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/

#define IO_PCB_mqxa_STACK_SIZE   (750 * sizeof(_mem_size))

/* SHARED MEMORY PROTOCOL DEFINITIONS */

/*  Interrupts bit definitions */
#define IO_PCB_SHM_ALL_INTS                           (0)

/* Buffer descriptors bits definition */
#define IO_PCB_SHM_BUFFER_OWN                (0x00000001)
#define IO_PCB_SHM_BUFFER_ALOCATED           (0x00000002)
#define IO_PCB_SHM_BUFFER_RELEASED           (0x00000004)

/* Shared memory Errors */
#define MQX_IO_PCB_SHM_INSTALL_ISR_FAILLED   (0x00000001)

#define NEXT_BD(x,y)         (x+y)
#define NEXT_INDEX(x,y)      ((++x == y)? x : 0)

/* Macro for aligning the Ring start address */
#define SHM_DESCR_ALIGN(n)   ((n) + (-(n) & 31))

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/


/*
** IO_PCB_SHM_BUFFER_STRUCT
** This structure contains the initialization information for the
** shared memory buffers
**
*/
typedef struct io_pcb_shm_buffer_struct
{
   /* pcb_ptr pointer */
   pointer     PACKET_PTR;

   /* Control bits */
   uint_32     CONTROL;

   /* Cache alignment so bd is 32 bytes long */
   uint_32     RESERVED[6];

} IO_PCB_SHM_BUFFER_STRUCT, _PTR_ IO_PCB_SHM_BUFFER_STRUCT_PTR;

/*
** IO_PCB_SHM_INFO_STRUCT
** This structure contains standard Bspio protocol information
**
*/
typedef struct io_pcb_shm_info_struct
{
   /*  INPUT DEFINITIONS */
   LWSEM_STRUCT           READ_LWSEM;
   QUEUE_STRUCT           READ_QUEUE;
   void       (_CODE_PTR_ READ_CALLBACK_FUNCTION)(FILE_DEVICE_STRUCT_PTR,  IO_PCB_STRUCT_PTR);
   _io_pcb_pool_id        READ_PCB_POOL;
   IO_PCB_SHM_BUFFER_STRUCT_PTR  RX_RING_PTR;
   FILE_DEVICE_STRUCT_PTR FD;
   uint_32                RXENTRIES;
   uint_32                RXNEXT;
   uint_32                RXLAST;
   uint_32                RX_LENGTH;

   /* OUTPUT DEFINITIONS */
   LWSEM_STRUCT           WRITE_LWSEM;
   QUEUE_STRUCT           WRITE_QUEUE;
   IO_PCB_SHM_BUFFER_STRUCT_PTR  TX_RING_PTR;
   uint_32                TXENTRIES;
   uint_32                TXNEXT;
   uint_32                TXLAST;
   uint_32                TX_LENGTH;

   /* STATISTICAL INFORMATION */
   _mqx_uint              RX_PACKETS;
   _mqx_uint              TX_PACKETS;
   _mqx_uint              TX_BD_RUNOVER;
   _mqx_uint              RX_BD_RUNOVER;

   /* A copy of the initialization info */
   IO_PCB_SHM_INIT_STRUCT INIT;

   /* Interrupts definitions */
   void      (_CODE_PTR_ RX_OLDISR_PTR)(pointer);
   pointer               RX_OLDISR_DATA;
   void      (_CODE_PTR_ TX_OLDISR_PTR)(pointer);
   pointer               TX_OLDISR_DATA;

} IO_PCB_SHM_INFO_STRUCT, _PTR_ IO_PCB_SHM_INFO_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_pcb_shm_open(FILE_DEVICE_STRUCT_PTR, char _PTR_, char _PTR_);
extern _mqx_int _io_pcb_shm_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_pcb_shm_read(FILE_DEVICE_STRUCT_PTR, IO_PCB_STRUCT_PTR _PTR_);
extern _mqx_int _io_pcb_shm_write(FILE_DEVICE_STRUCT_PTR, IO_PCB_STRUCT_PTR);
extern _mqx_int _io_pcb_shm_ioctl(FILE_DEVICE_STRUCT_PTR, _mqx_uint, pointer);
extern _mqx_int _io_pcb_shm_uninstall(IO_PCB_DEVICE_STRUCT_PTR);
extern void     _io_pcb_shm_tx(pointer);
extern void     _io_pcb_shm_rx_isr(pointer);
extern void     _io_pcb_shm_tx_isr(pointer);

#if BSPCFG_IO_PCB_SHM_SUPPORT
extern _mqx_uint _bsp_io_pcb_shm_int_install(IO_PCB_SHM_INIT_STRUCT_PTR init_ptr,IO_PCB_SHM_INFO_STRUCT_PTR info_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
