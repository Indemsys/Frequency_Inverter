#ifndef __tfs_prv_h__
#define __tfs_prv_h__
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
* $FileName: tfs_prv.h$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the structure definitions and constants
*   that are internal to the Embedded MS-DOS File System (TFS)
*
*END************************************************************************/


#include <tfs.h>


/* configuration data for the TFS */
typedef struct tfs_drive_struct
{
   char_ptr                 IDENTIFIER;
   const TFS_DIR_ENTRY      *ROOT;
  
} TFS_DRIVE_STRUCT, _PTR_ TFS_DRIVE_STRUCT_PTR;


/*
** extern statements for TFS
*/
#ifdef __cplusplus
extern "C" {
#endif

extern char_ptr TFS_Parse_Out_Device_Name (char_ptr);
extern int_32 TFS_Cmp (char_ptr, char_ptr);
extern pointer TFS_Open_File (TFS_DRIVE_STRUCT_PTR, char_ptr, uint_32_ptr);
extern uint_32 TFS_Read (MQX_FILE_PTR, uint_32, char_ptr, uint_32_ptr);
extern uint_32 TFS_Write (MQX_FILE_PTR, uint_32, char_ptr, uint_32_ptr);
extern uint_32 TFS_Move_File_Pointer (MQX_FILE_PTR, uint_32_ptr);
extern int_32 TFS_Close_File (MQX_FILE_PTR);
    
#ifdef __cplusplus
}
#endif

    
#endif
/* EOF */
