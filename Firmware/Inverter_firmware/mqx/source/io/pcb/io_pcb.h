#ifndef __io_pcb_h__
#define __io_pcb_h__
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
* $FileName: io_pcb.h$
* $Version : 3.6.7.0$
* $Date    : Oct-5-2011$
*
* Comments:
*
*   This file is the header file for the I/O subsystem interface.                        
*
*END************************************************************************/

#include "ioctl.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* An invalid pool id */
#define IO_PCB_NULL_POOL_ID (_io_pcb_pool_id)(0)

/* Error codes */
#define IO_PCB_POOL_INVALID            (IO_PCB_ERROR_BASE|0x10)
#define IO_PCB_INVALID                 (IO_PCB_ERROR_BASE|0x11)
#define IO_PCB_NOT_A_PCB               (IO_PCB_ERROR_BASE|0x12)
#define IO_PCB_NOT_A_PCB_DEVICE        (IO_PCB_ERROR_BASE|0x13)
#define IO_PCB_READ_NOT_AVAILABLE      (IO_PCB_ERROR_BASE|0x14)
#define IO_PCB_WRITE_NOT_AVAILABLE     (IO_PCB_ERROR_BASE|0x15)
#define IO_PCB_DEVICE_DOES_NOT_EXIST   (IO_PCB_ERROR_BASE|0x16)
#define IO_PCB_ALLOC_CALLBACK_FAILED   (IO_PCB_ERROR_BASE|0x17)

/* PCB IOCTL Commands */
#define IO_PCB_IOCTL_READ_CALLBACK_SET _IO(IO_TYPE_PCB,0x01)
#define IO_PCB_IOCTL_SET_INPUT_POOL    _IO(IO_TYPE_PCB,0x02)
#define IO_PCB_IOCTL_ENQUEUE_READQ     _IO(IO_TYPE_PCB,0x03)
#define IO_PCB_IOCTL_START             _IO(IO_TYPE_PCB,0x04)
#define IO_PCB_IOCTL_UNPACKED_ONLY     _IO(IO_TYPE_PCB,0x05)

/*--------------------------------------------------------------------------*/
/*
**                            MACRO DEFINITIONS
*/
#define IO_PCB_FREE(pcb_ptr) \
   (*(pcb_ptr)->FREE_PCB_FUNCTION_PTR)(pcb_ptr)

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPES
*/

/* The type of an IO PCB Pool ID (the address of the pool - 4) */
typedef pointer _io_pcb_pool_id;

typedef struct pcb_queue_element_struct {

   /* next element in queue, MUST BE FIRST FIELD */
   struct pcb_queue_element_struct _PTR_ NEXT;

   /* previous element in queue, MUST BE SECOND FIELD */
   struct pcb_queue_element_struct _PTR_ PREV;
   
} PCB_QUEUE_ELEMENT_STRUCT, _PTR_ PCB_QUEUE_ELEMENT_STRUCT_PTR;

/* 
** IO PCB FRAGMENT STRUCT
** This structure defines the location and size of a memory fragment, used by
** the IO PCB structure.
*/
typedef struct io_pcb_fragment_struct {

   /* The length of the data in bytes */
   _mqx_uint  LENGTH;

   /* The starting address of the data */
   uchar_ptr  FRAGMENT;

} IO_PCB_FRAGMENT_STRUCT, _PTR_ IO_PCB_FRAGMENT_STRUCT_PTR;

/*
** IO PCB STRUCT
** This structure defines what a Packet Control Block (PCB) looks like.
** The PCB is used to define the format of a data packet.  The data packet
** consists of any number of fragments of data located in various memory locations.
** The meaning of each fragment is protocol and application dependent.
*/
typedef struct io_pcb_struct {

   /* MQX queue utility pointers for queueing up PCBs */
   PCB_QUEUE_ELEMENT_STRUCT  QUEUE;

   /* The function to call when freeing this PCB */
   _mqx_uint     (_CODE_PTR_ FREE_PCB_FUNCTION_PTR)(struct io_pcb_struct _PTR_);

   /* PCB Validity field used for validity checking*/
   _mqx_uint                 VALID;

   /* The PCB pool that the PCB was allocated from */
   _io_pcb_pool_id           POOL_ID;

   /* Addresss of private information for used by the protocol/application */
   pointer                   OWNER_PRIVATE;
   pointer                   INSTANTIATOR_PRIVATE;

   /* protocol/application specific data */
   uint_16                   PRIVATE;

   /* The number of fragments in the variable length array */
   uint_16                   NUMBER_OF_FRAGMENTS;

   /* A variable length array of data fragments */
   IO_PCB_FRAGMENT_STRUCT  FRAGMENTS[1];

} IO_PCB_STRUCT, _PTR_ IO_PCB_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                      FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* IO PCB functions */
#ifndef __TAD_COMPILE__
extern _io_pcb_pool_id   _io_pcb_create_pool(_mqx_uint, _mem_size, _mqx_uint, 
   _mqx_uint, _mqx_uint, 
   IO_PCB_STRUCT_PTR (_CODE_PTR_) (IO_PCB_STRUCT_PTR, pointer), pointer,
   IO_PCB_STRUCT_PTR (_CODE_PTR_) (IO_PCB_STRUCT_PTR, pointer), pointer);
extern IO_PCB_STRUCT_PTR _io_pcb_alloc(_io_pcb_pool_id, boolean);
extern _mqx_int  _io_pcb_read(MQX_FILE_PTR, IO_PCB_STRUCT_PTR _PTR_);
extern _mqx_int  _io_pcb_write(MQX_FILE_PTR, IO_PCB_STRUCT _PTR_);
extern _mqx_uint _io_pcb_free(IO_PCB_STRUCT _PTR_);
extern _mqx_uint _io_pcb_free_internal(IO_PCB_STRUCT _PTR_);
extern _mqx_uint _io_pcb_destroy_pool(_io_pcb_pool_id);
extern _mqx_uint _io_pcb_test(pointer _PTR_, pointer _PTR_);
extern _mqx_int  _io_pcb_start(MQX_FILE_PTR fd_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
