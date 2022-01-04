#ifndef _pipe_prv_h_
#define _pipe_prv_h_
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
* $FileName: pipe_prv.h$
* $Version : 3.6.5.0$
* $Date    : Aug-27-2010$
*
* Comments:
*
*   The file contains the private internal functions prototype, defines, 
*   structure   definitions private to the Pipe device
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          STRUCTURE DEFINITIONS
*/


/*
** IO_PIPE_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a Pipe is initialized.
*/

typedef struct io_pipe_init_struct
{
  
   /* The pipe character queue size */
   uint_32 QUEUE_SIZE;

   /* Initialization parameters for the pipe, currently not used*/
   uint_32 FLAGS;

 
} IO_PIPE_INIT_STRUCT, _PTR_ IO_PIPE_INIT_STRUCT_PTR;


/*
** IO_PIPE_INFO_STRUCT
**
** This structure defines the current parameters and status for a Pipe  
**
*/

typedef struct io_pipe_info_struct
{

   CHARQ_STRUCT_PTR     QUEUE;
  
   /* The serial pipe queue size */
   uint_32    QUEUE_SIZE;

   /* Mutext to protect against simulateous reads from the Pipe */ 
   MUTEX_STRUCT READ_MUTEX;

   /* Mutext to protect against simulateous writes to the Pipe */ 
   MUTEX_STRUCT WRITE_MUTEX;

   /* Mutext to protect against access to the Pipe data structures */ 
   MUTEX_STRUCT ACCESS_MUTEX;

   /* Semaphore used to block when pipe is full  */
   LWSEM_STRUCT FULL_SEM; 

   /* Semaphore used to block when pipe is empty */
   LWSEM_STRUCT EMPTY_SEM; 

   /* Flags to define options for the Pipe, currently not used */
   uint_32 FLAGS;

   /* Abort out of pending pipe reads, cleared by next pipe read */
   boolean KM_ABORT_READ;
   
   /* Abort out of pending pipe write, cleared by next pipe write */
   boolean KM_ABORT_WRITE;
 
} IO_PIPE_INFO_STRUCT, _PTR_ IO_PIPE_INFO_STRUCT_PTR;


/*----------------------------------------------------------------------*/
/*
**                          EXTERNAL FUCTION DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_pipe_open(FILE_DEVICE_STRUCT_PTR, char _PTR_, char_ptr);
extern _mqx_int _io_pipe_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_pipe_read(FILE_DEVICE_STRUCT_PTR, char _PTR_, _mqx_int);
extern _mqx_int _io_pipe_write(FILE_DEVICE_STRUCT_PTR, char _PTR_, _mqx_int);
extern _mqx_int _io_pipe_ioctl(FILE_DEVICE_STRUCT_PTR, uint_32, pointer);
extern _mqx_int _io_pipe_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
