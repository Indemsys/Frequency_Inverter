#ifndef __iomemprv_h__
#define __iomemprv_h__
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
* $FileName: iomemprv.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the ram disk.
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/
/* Start CR 810 */
/* These are used to determine how memory was allocated for the RAM drive */
#define MEM_TYPE_DYNAMIC   (1)
#define MEM_TYPE_STATIC    (2)

/* Define the block size when driver is opened in block mode */
/* Defines size as a power of 2 */
#define IO_MEM_BLOCK_SIZE_POWER   (9)
#define IO_MEM_BLOCK_SIZE         (1 << IO_MEM_BLOCK_SIZE_POWER)

/* Properties of device */
#define IO_MEM_ATTRIBS  (IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE | \
   IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE)
/* End   CR 810 */

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/*
** IO_MEM STRUCT
**
** The address of this structure is used to maintain ramdisk specific 
** information.
*/
typedef struct io_mem_struct
{
   /* Type of memory, dynamic or static */
   _mqx_uint        TYPE;

   /* Address of the fdv_ram device */
   uchar_ptr        BASE_ADDR;

   /* The total size of memory in the ram disk */
   _file_size       SIZE;

   /* Light weight semaphore struct */
   LWSEM_STRUCT     LWSEM;
   
   /* The current error code for the device */
   _mqx_uint        ERROR_CODE;

   /* Start CR 810 */
   /* The current mode for the device */
   boolean          BLOCK_MODE;

   /* The number of blocks for the device */
   _file_size       NUM_BLOCKS;
   /* End   CR 810 */

} IO_MEM_STRUCT, _PTR_ IO_MEM_STRUCT_PTR;


/* Internal functions to IO_MEM */
#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_mem_open(FILE_DEVICE_STRUCT_PTR, char_ptr, char_ptr);
extern _mqx_int _io_mem_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_mem_write(FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_mem_read (FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_mem_ioctl(FILE_DEVICE_STRUCT_PTR, _mqx_uint, pointer);
extern _mqx_int _io_mem_uninstall(IO_DEVICE_STRUCT_PTR);

/* Start CR 810 */
extern _mqx_int _io_mem_write_blocks(FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
extern _mqx_int _io_mem_read_blocks (FILE_DEVICE_STRUCT_PTR, char_ptr, _mqx_int);
/* End   CR 810 */

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
