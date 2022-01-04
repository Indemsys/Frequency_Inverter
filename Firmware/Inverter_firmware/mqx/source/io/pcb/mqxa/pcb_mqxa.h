#ifndef __pcb_mqxa_h__
#define __pcb_mqxa_h__
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
* $FileName: pcb_mqxa.h$
* $Version : 3.8.4.0$
* $Date    : Oct-11-2011$
*
* Comments:
*
*   This file contains the definitions for the PCB device driver that
*   sends and receives packets over a asynchrnous serial device.  The
*   packets are in MQX IPC async packet format.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/

/*
** Initialization errors
*/
#define IO_PCB_MQXA_DEVICE_ALREADY_OPEN         (IO_PCB_ERROR_BASE|0x90)
#define IO_PCB_MQXA_INCORRECT_SERIAL_DEVICE     (IO_PCB_ERROR_BASE|0x91)

/*
**             PACKET STRUCTURE CONTROL FIELD BIT DEFINITIONS
*/

#define IO_PCB_MQXA_HDR_LITTLE_ENDIAN           (0x40)
#define IO_PCB_MQXA_DATA_LITTLE_ENDIAN          (0x20)

#define IO_PCB_MQXA_HDR_BIG_ENDIAN              (0x00)
#define IO_PCB_MQXA_DATA_BIG_ENDIAN             (0x00)

/*
** ENDIAN
** Indicates in control field of packet the endianness of the packet
*/
#define IO_PCB_MQXA_HDR_ENDIAN_MASK             (0x40)
#define IO_PCB_MQXA_DATA_ENDIAN_MASK            (0x20)

#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
#define IO_PCB_MQXA_HDR_ENDIAN   IO_PCB_MQXA_HDR_LITTLE_ENDIAN
#define IO_PCB_MQXA_DATA_ENDIAN  IO_PCB_MQXA_DATA_LITTLE_ENDIAN
#else
#define IO_PCB_MQXA_HDR_ENDIAN   IO_PCB_MQXA_HDR_BIG_ENDIAN
#define IO_PCB_MQXA_DATA_ENDIAN  IO_PCB_MQXA_DATA_BIG_ENDIAN
#endif

#define IO_PCB_MQX_MUST_CONVERT_HDR_ENDIAN(ctrl) \
   (((ctrl) & IO_PCB_MQXA_HDR_ENDIAN_MASK) != IO_PCB_MQXA_HDR_ENDIAN)

#define IO_PCB_MQX_MUST_CONVERT_DATA_ENDIAN(ctrl) \
   (((ctrl) & IO_PCB_MQXA_DATA_ENDIAN_MASK) != IO_PCB_MQXA_DATA_ENDIAN)

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/

/*
** IO_PCB_MQXA_PACKET_HEADER_STRUCT
** This structure defines what a packet header looks like for this
** MQX protocol.
*/
typedef struct io_pcb_mqxa_packet_header_struct
{
   uint_16 LENGTH;
#ifdef MQX_USE_32BIT_MESSAGE_QIDS
   uint_16 PADD;
   uint_32 ADDR1;
   uint_32 ADDR2;
#else
   uint_16 ADDR;
   uint_16 ADDR2;
#endif
   uchar CONTROL;
#ifdef MQX_USE_32BIT_MESSAGE_QIDS
   uchar RESERVED[3];
#else
   uchar RESERVED[1];
#endif
} IO_PCB_MQXA_PACKET_HEADER_STRUCT, _PTR_ IO_PCB_MQXA_PACKET_HEADER_STRUCT_PTR;

/*
** IO_PCB_MQXA_INIT_STRUCT
** This structure contains the initialization information for the
** async_serial protocol
**
*/
typedef struct io_pcb_mqxa_init_struct
{

   /* The serial interrupt device to use */
   char _PTR_ IO_PORT_NAME;

   /* What baud rate to use */
   uint_32    BAUD_RATE;

   /* Is the IO port polled? */
   boolean    IS_POLLED;

   /* Maximum size of input packet */
   _mem_size  INPUT_MAX_LENGTH;

   /* What priority the tasks are to run at */
   _mqx_uint  INPUT_TASK_PRIORITY;
   _mqx_uint  OUTPUT_TASK_PRIORITY;

} IO_PCB_MQXA_INIT_STRUCT, _PTR_ IO_PCB_MQXA_INIT_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _io_pcb_mqxa_install(char _PTR_, pointer);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
