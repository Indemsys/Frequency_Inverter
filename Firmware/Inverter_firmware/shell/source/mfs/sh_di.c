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
* $FileName: sh_di.c$
* $Version : 3.8.10.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*   This file contains the source for an MFS shell function.
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <mfs.h>
#include <shell.h>
#include <sh_mfs.h>

#if SHELLCFG_USES_MFS

#define SECTOR_SIZE MFS_DEFAULT_SECTOR_SIZE

typedef struct 
{
   uchar    JMP[3];
   uchar    OEMNAME[8];
   uchar    BPB_SECTOR_SIZE[2];
   uchar    SECTORS_PER_CLUSTER[1];
   uchar    RESERVED_SECTORS[2];
   uchar    NUMBER_OF_FAT[1];
   uchar    ROOT_ENTRIES[2];
   uchar    NUMBER_SECTORS[2];
   uchar    MEDIA_TYPE[1];
   uchar    SECTORS_PER_FAT[2];
   uchar    SECTORS_PER_TRACK[2];
   uchar    NUM_HEADS[2];
   uchar    HIDDEN_SECTORS[4];
   uchar    MEGA_SECTORS[4];

   uchar    FAT_SIZE[4];
   uchar    EXT_FLAGS[2];
   uchar    FS_VER[2];
   uchar    ROOT_CLUSTER[4];
   uchar    FS_INFO[2];
   uchar    BK_BOOT_SEC[2];
   uchar    RESERVED[12];
   uchar    DRVNUM[1];
   uchar    RESERVED1[1];
   uchar    BOOTSIG[1];
   uchar    VOLID[4];
   uchar    VOLLAB[11];
   uchar    FILSYSTYPE[8];
} BIOS_PARAM_STRUCT_DISK, _PTR_ BIOS_PARAM_STRUCT_DISK_PTR;

/* The file system info struct. For FAT32 only */
typedef struct filesystem_info_disk  {
   uchar    LEAD_SIG[4];
   uchar    RESERVED1[480];
   uchar    STRUCT_SIG[4];
   uchar    FREE_COUNT[4];
   uchar    NEXT_FREE[4];
   uchar    RESERVED2[12];
   uchar    TRAIL_SIG[4];  
} FILESYSTEM_INFO_DISK, _PTR_ FILESYSTEM_INFO_DISK_PTR;



static uint_32 print_bpb(pointer bpb) {
   BIOS_PARAM_STRUCT_DISK_PTR bpb_ptr = (BIOS_PARAM_STRUCT_DISK_PTR) bpb;
   uint_32 i,backup=0;


   printf("OEMNAME            = ");
   for(i=0;i<8;i++) printf("%c", bpb_ptr->OEMNAME[i]);  printf("\n");
   printf("SECTOR_SIZE        = %02x%02x\n",bpb_ptr->BPB_SECTOR_SIZE[1] ,bpb_ptr->BPB_SECTOR_SIZE[0] );
   printf("SECTORS_PER_CLUSTER= %02x%\n",(uint_32) bpb_ptr->SECTORS_PER_CLUSTER[0]  );
   printf("RESERVED_SECTORS   = %02x%02x\n",bpb_ptr->RESERVED_SECTORS[1] ,bpb_ptr->RESERVED_SECTORS[0] );
   printf("NUMBER_OF_FAT      = %02x\n",(uint_32) bpb_ptr->NUMBER_OF_FAT[0]  );
   printf("ROOT_ENTRIES       = %02x%02x\n",bpb_ptr->ROOT_ENTRIES[1] ,bpb_ptr->ROOT_ENTRIES[0] );
   printf("NUMBER_SECTORS     = %02x%02x\n",bpb_ptr->NUMBER_SECTORS[1] ,bpb_ptr->NUMBER_SECTORS[0] );
   printf("MEDIA_TYPE         = %02x\n",(uint_32) bpb_ptr->MEDIA_TYPE[0] );
   printf("SECTORS_PER_FAT    = %02x%02x\n",bpb_ptr->SECTORS_PER_FAT[1] ,bpb_ptr->SECTORS_PER_FAT[0] );
   printf("SECTORS_PER_TRACK  = %02x%02x\n",bpb_ptr->SECTORS_PER_TRACK[1] ,bpb_ptr->SECTORS_PER_TRACK[0] );
   printf("NUM_HEADS          = %02x%02x\n",bpb_ptr->NUM_HEADS[1] ,bpb_ptr->NUM_HEADS[0] );
   printf("HIDDEN_SECTORS     = %02x%02x%02x%02x\n",bpb_ptr->HIDDEN_SECTORS[3] ,bpb_ptr->HIDDEN_SECTORS[2],bpb_ptr->HIDDEN_SECTORS[1] ,bpb_ptr->HIDDEN_SECTORS[0] );
   printf("MEGA_SECTORS       = %02x%02x%02x%02x\n",bpb_ptr->MEGA_SECTORS[3] ,bpb_ptr->MEGA_SECTORS[2],bpb_ptr->MEGA_SECTORS[1] ,bpb_ptr->MEGA_SECTORS[0] );

   if ( bpb_ptr->SECTORS_PER_FAT[1]|| bpb_ptr->SECTORS_PER_FAT[0] ) {
      // FAT 12/16

   } else {
   
      // FAT 32
      printf("FAT_SIZE           = %02x%02x%02x%02x\n",bpb_ptr->FAT_SIZE[3] ,bpb_ptr->FAT_SIZE[2],bpb_ptr->FAT_SIZE[1] ,bpb_ptr->FAT_SIZE[0] );
      printf("EXT_FLAGS          = %02x%02x\n",bpb_ptr->EXT_FLAGS[1] ,bpb_ptr->EXT_FLAGS[0] );
      printf("FS_VER             = %02x%02x\n",bpb_ptr->FS_VER[1] ,bpb_ptr->FS_VER[0] );

      printf("ROOT_CLUSTER       = %02x%02x%02x%02x\n",bpb_ptr->ROOT_CLUSTER[3] ,bpb_ptr->ROOT_CLUSTER[2],bpb_ptr->ROOT_CLUSTER[1] ,bpb_ptr->ROOT_CLUSTER[0] );
      printf("FS_INFO            = %02x%02x\n",bpb_ptr->FS_INFO[1] ,bpb_ptr->FS_INFO[0] );
      printf("BK_BOOT_SEC        = %02x%02x\n",bpb_ptr->BK_BOOT_SEC[1] ,bpb_ptr->BK_BOOT_SEC[0] );

      backup = (bpb_ptr->BK_BOOT_SEC[1] << 8) | bpb_ptr->BK_BOOT_SEC[0];
      printf("DRVNUM             = %02x\n",(uint_32) bpb_ptr->DRVNUM[0] );
      printf("BOOTSIG            = %02x\n",(uint_32) bpb_ptr->BOOTSIG[0] );
      printf("VOLID              = %02x%02x%02x%02x\n",bpb_ptr->VOLID[3] ,bpb_ptr->VOLID[2],bpb_ptr->VOLID[1] ,bpb_ptr->VOLID[0] );
      printf("VOLLAB             = ");
      for(i=0;i<11;i++) printf("%c", bpb_ptr->VOLLAB[i]);  printf("\n");
      printf("FILSYSTYPE         = ");
      for(i=0;i<8;i++) printf("%c", bpb_ptr->FILSYSTYPE[i]);  printf("\n");

   }
   return backup;
}


static void print_fsi(pointer bpb) 
{
   FILESYSTEM_INFO_DISK_PTR bpb_ptr = (FILESYSTEM_INFO_DISK_PTR) bpb;
   
   printf("LEAD_SIG   = %02x%02x%02x%02x\n",bpb_ptr->LEAD_SIG[3],  bpb_ptr->LEAD_SIG[2],  bpb_ptr->LEAD_SIG[1],  bpb_ptr->LEAD_SIG[0] );
   printf("STRUCT_SIG = %02x%02x%02x%02x\n",bpb_ptr->STRUCT_SIG[3],bpb_ptr->STRUCT_SIG[2],bpb_ptr->STRUCT_SIG[1],bpb_ptr->STRUCT_SIG[0] );
   printf("FREE_COUNT = %02x%02x%02x%02x\n",bpb_ptr->FREE_COUNT[3],bpb_ptr->FREE_COUNT[2],bpb_ptr->FREE_COUNT[1],bpb_ptr->FREE_COUNT[0] );
   printf("NEXT_FREE  = %02x%02x%02x%02x\n",bpb_ptr->NEXT_FREE[3], bpb_ptr->NEXT_FREE[2], bpb_ptr->NEXT_FREE[1], bpb_ptr->NEXT_FREE[0] );
   printf("TRAIL_SIG  = %02x%02x%02x%02x\n",bpb_ptr->TRAIL_SIG[3], bpb_ptr->TRAIL_SIG[2], bpb_ptr->TRAIL_SIG[1], bpb_ptr->TRAIL_SIG[0] );
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_di
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_di(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   int_32            offset;

   MQX_FILE_PTR      fd, fs;

   uint_32           backup=0;
   uchar_ptr         buffer=NULL;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2 ) || (argc > 3)) {
         printf("Invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else if ( !Shell_parse_uint_32(argv[1], (uint_32_ptr) &offset ))  {
         printf("Error, invalid length\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         buffer = _mem_alloc(SECTOR_SIZE);
         if (buffer==NULL) {
            printf("Error, unable to allocate sector buffer\n");
            return  SHELL_EXIT_ERROR;
         }

         if (argc==3) {
            fd = fopen(argv[2], "b");
         } else {   
            fs = Shell_get_current_filesystem(argv);
            _io_ioctl(fs, IO_IOCTL_GET_DEVICE_HANDLE, &fd);
         }
              

         if (fd)  {
            if (fseek(fd, offset, IO_SEEK_SET) == IO_ERROR)  {
               printf("Error, unable to seek to sector %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            } else if (_io_read(fd, (char_ptr) buffer, 1) != 1) {
               printf("Error, unable to read sector %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            }
            
            if (return_code == SHELL_EXIT_SUCCESS) {
               printf("\n");
               backup = print_bpb(buffer);
               if (backup) {
                  if (fseek(fd, backup, IO_SEEK_SET) == IO_ERROR)  {
                     printf("Error, unable to seek to sector %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  } else if (_io_read(fd, (char_ptr) buffer, 1) != 1) {
                     printf("Error, unable to read sector %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  }  
                  if (return_code == SHELL_EXIT_SUCCESS) {
                     printf("\n");
                     print_bpb(buffer);
                  }
               }
            }
            if (fseek(fd, 1, IO_SEEK_SET) == IO_ERROR)  {
               printf("Error, unable to seek to sector %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            } else if (_io_read(fd, (char_ptr) buffer, 1) != 1) {
               printf("Error, unable to read sector %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            }  
            if (return_code == SHELL_EXIT_SUCCESS) {
               printf("\n");
               print_fsi(buffer);
            }
            if (argc==3) {
               fclose(fd);
            }
         }
         _mem_free(buffer);
      }
   }
   
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <sector> [<device>]\n", argv[0]);
      } else  {
         printf("Usage: %s <sector> [<device>]\n", argv[0]);
         printf("   <sector>     = sector number\n");
         printf("   <device>     = low level device\n");
      }
   }
   return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS
