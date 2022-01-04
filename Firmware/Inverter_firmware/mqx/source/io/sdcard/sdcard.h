#ifndef __sdcard_h__
#define __sdcard_h__
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
* $FileName: sdcard.h$
* $Version : 3.8.7.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the SD card.
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#define IO_SDCARD_BLOCK_SIZE_POWER   (9)
#define IO_SDCARD_BLOCK_SIZE         (1 << IO_SDCARD_BLOCK_SIZE_POWER)


/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/*
** SDCARD_INIT STRUCT
**
** The address of this structure is used to maintain sd card init
** information.
*/
typedef struct sdcard_init_struct
{
   /* The function to call to initialize sd card */
   boolean  (_CODE_PTR_ INIT_FUNC)(MQX_FILE_PTR);

   /* The function to call to read blocks from sd card */
   _mqx_int (_CODE_PTR_ READ_FUNC)(MQX_FILE_PTR, uchar_ptr, uint_32, uint_32);

   /* The function to call to write blocks to sd card */
   _mqx_int (_CODE_PTR_ WRITE_FUNC)(MQX_FILE_PTR, uchar_ptr, uint_32, uint_32);
   
   /* Signal specification */
   uint_32             SIGNALS;

} SDCARD_INIT_STRUCT, _PTR_ SDCARD_INIT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_sdcard_install(char_ptr,SDCARD_INIT_STRUCT_PTR,MQX_FILE_PTR);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
