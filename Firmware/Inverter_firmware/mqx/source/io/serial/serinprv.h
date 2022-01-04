#ifndef __serinprv_h__
#define __serinprv_h__
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
* $FileName: serinprv.h$
* $Version : 3.8.8.0$
* $Date    : Aug-29-2011$
*
* Comments:
*
*   This file includes the private definitions for the interrupt
*   driven serial I/O drivers.
*
*END************************************************************************/

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** Xon/Xoff protocol characters
*/
#define CNTL_S   ((char) 0x13)  /* Control S == XOFF.   */
#define CNTL_Q   ((char) 0x11)  /* Control Q == XON.    */

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*---------------------------------------------------------------------
**
** IO SERIAL INT DEVICE STRUCT
**
** This structure used to store information about a interrupt serial io device
** for the IO device table
*/
typedef struct io_serial_int_device_struct
{

   /* The I/O init function */
   _mqx_uint (_CODE_PTR_ DEV_INIT)(pointer, char _PTR_);

   /* The enable interrupts function */
   _mqx_uint (_CODE_PTR_ DEV_ENABLE_INTS)(pointer);

   /* The I/O deinit function */
   _mqx_uint (_CODE_PTR_ DEV_DEINIT)(pointer, pointer);

   /* The output function, used to write out the first character */
   void    (_CODE_PTR_ DEV_PUTC)(pointer, char);

   /* The ioctl function, (change bauds etc) */
   _mqx_uint (_CODE_PTR_ DEV_IOCTL)(pointer, _mqx_uint, pointer);

   /* The I/O channel initialization data */
   pointer             DEV_INIT_DATA_PTR;
   
   /* Device specific information */
   pointer             DEV_INFO_PTR;

   /* The queue size to use */
   _mqx_uint             QUEUE_SIZE;
   
   /* Open count for number of accessing file descriptors */
   _mqx_uint             COUNT;

   /* Open flags for this channel */
   _mqx_uint             FLAGS;

   /* The input queue */
   CHARQ_STRUCT_PTR    IN_QUEUE;

   /* The input waiting tasks */
   pointer             IN_WAITING_TASKS;

   /* The output queue */
   CHARQ_STRUCT_PTR    OUT_QUEUE;

   /* The output waiting tasks */
   pointer             OUT_WAITING_TASKS;

   /* Has output been started */
   boolean             OUTPUT_ENABLED;

   /* Protocol flag information */
   _mqx_uint             HAVE_STOPPED_OUTPUT;
   _mqx_uint             HAVE_STOPPED_INPUT;
   _mqx_uint             MUST_STOP_INPUT;
   _mqx_uint             MUST_START_INPUT;
   _mqx_uint             INPUT_HIGH_WATER_MARK;
   _mqx_uint             INPUT_LOW_WATER_MARK;
   _mqx_uint             MUST_STOP_OUTPUT;
   
#if MQX_ENABLE_LOW_POWER
   
   /* Low power related state information */
   IO_SERIAL_LPM_STRUCT  LPM_INFO;
   
#endif

} IO_SERIAL_INT_DEVICE_STRUCT, _PTR_ IO_SERIAL_INT_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Interrupt I/O prototypes */
extern _mqx_int  _io_serial_int_open(FILE_DEVICE_STRUCT_PTR, char _PTR_, 
   char _PTR_);
extern _mqx_int  _io_serial_int_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int  _io_serial_int_read(FILE_DEVICE_STRUCT_PTR, char _PTR_, _mqx_int);
extern _mqx_int  _io_serial_int_write(FILE_DEVICE_STRUCT_PTR, char _PTR_, _mqx_int);
extern _mqx_int  _io_serial_int_ioctl(FILE_DEVICE_STRUCT_PTR, _mqx_uint, 
   pointer);
extern _mqx_int _io_serial_int_uninstall(IO_DEVICE_STRUCT_PTR);

/* Callback Functions called by lower level interrupt I/O interrupt handlers */
extern boolean _io_serial_int_addc(IO_SERIAL_INT_DEVICE_STRUCT_PTR, char);
extern _mqx_int  _io_serial_int_nextc(IO_SERIAL_INT_DEVICE_STRUCT_PTR);

/* Internal helper functions */
extern boolean    _io_serial_int_putc_internal(IO_SERIAL_INT_DEVICE_STRUCT_PTR, 
   char, _mqx_uint);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
