#ifndef _pcflash_h_
#define _pcflash_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: pcflash.h$
* $Version : 3.8.8.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the PC Card flash drivers
*   Revision History:
*   Date          Version        Changes
*   Nov 16, 1998  2.40           Convert to MQX2.40 from MQX2.33
*
*END************************************************************************/

#include "ioctl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** PCFlash IOCTL calls
*/
#define PCFLASH_IOCTL_IS_CARD_INSERTED     _IO(IO_TYPE_PCFLASH,0x01)
#define PCFLASH_IOCTL_GET_NUM_SECTORS      _IO(IO_TYPE_PCFLASH,0x02)
#define PCFLASH_IOCTL_GET_SECTOR_SIZE      _IO(IO_TYPE_PCFLASH,0x03)
#define PCFLASH_IOCTL_GET_DRIVE_PARAMS     _IO(IO_TYPE_PCFLASH,0x04)
#define PCFLASH_IOCTL_SET_BLOCK_MODE       _IO(IO_TYPE_PCFLASH,0x05)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

typedef struct pcflash_drive_info_struct
{
   uint_32  NUMBER_OF_HEADS; 
   uint_32  NUMBER_OF_TRACKS;
   uint_32  SECTORS_PER_TRACK;
} PCFLASH_DRIVE_INFO_STRUCT, _PTR_ PCFLASH_DRIVE_INFO_STRUCT_PTR;
   
    
/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif
 
uint_32 _io_pcflash_install(char_ptr);
uint_32 _io_apcflash_install(char_ptr);

#ifdef __cplusplus
}
#endif


#endif
/* EOF */
