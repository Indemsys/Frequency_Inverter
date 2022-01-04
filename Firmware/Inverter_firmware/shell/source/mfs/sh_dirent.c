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
* $FileName: sh_dirent.c$
* $Version : 3.8.12.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the source for an MFS shell function.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <shell.h>
#include <ctype.h>
#include <mfs.h>
#include <sh_mfs.h>

#define SECTOR_SIZE MFS_DEFAULT_SECTOR_SIZE
#define DIRENTS_PER_SECTOR (SECTOR_SIZE/sizeof(MFS_DIR_ENTRY))

static boolean is_zero(uchar_ptr buffer, uint_32 size) 
{
   uint_32 i;

   for (i=0;i<size;i++) {
      if (buffer[i]) {
         return FALSE;
      }
   }
   return TRUE;
}
   


static void print_dirent(MFS_DIR_ENTRY_PTR dirent)
{
   boolean     skip;
   uint_32     i;
   uchar_ptr   lfn = (uchar_ptr) dirent;
   
   skip = FALSE;

   if (dirent->ATTRIBUTE[0] == MFS_ATTR_LFN) {
      
      printf("\nLFN: %02d%c ",dirent->NAME[0]&0x3f, (dirent->NAME[0]&0x40)?'*':'+');
      for (i=0;i<5;i++) {
         printf("%c",lfn[i*2+1]!=0xff?lfn[i*2+1]:' ');
      }
      for (i=0;i<6;i++) {
         printf("%c",lfn[i*2+15]!=0xff?lfn[i*2+14]:' ');
      }
      for (i=0;i<2;i++) {
         printf("%c",lfn[i*2+29]!=0xff?lfn[i*2+28]:' ');
      }
   } else {
      printf("\nSFN: ");
      if (dirent->NAME[0] == 0) {
         printf(" Unused             ");
      } else if ((uchar)dirent->NAME[0] == 0xe5) {
         printf("Deleted             ");
      } else {
         for (i=0;i<8;i++) {
            printf("%c",(isgraph(dirent->NAME[i])?dirent->NAME[i]:' '));
         }
         printf(".");
         for (i=0;i<3;i++) {
            printf("%c",(dirent->TYPE[i]?dirent->TYPE[i]:' '));
         }

         printf(" %c%c%c%c%c%c ",
            (dirent->ATTRIBUTE[0] & MFS_ATTR_READ_ONLY)?'R':' ',
            (dirent->ATTRIBUTE[0] & MFS_ATTR_HIDDEN_FILE)?'H':' ',
            (dirent->ATTRIBUTE[0] & MFS_ATTR_SYSTEM_FILE)?'S':' ',
            (dirent->ATTRIBUTE[0] & MFS_ATTR_VOLUME_NAME)?'V':' ',
            (dirent->ATTRIBUTE[0] & MFS_ATTR_DIR_NAME)?'D':' ',
            (dirent->ATTRIBUTE[0] & MFS_ATTR_ARCHIVE)?'A':' ');
         printf("FClust %02x%02x%02x%02x",dirent->HFIRST_CLUSTER[1] ,dirent->HFIRST_CLUSTER[0],
            dirent->LFIRST_CLUSTER[1] ,dirent->LFIRST_CLUSTER[0] );
         printf(", Size %02x%02x%02x%02x",dirent->FILE_SIZE[3] ,dirent->FILE_SIZE[2], dirent->FILE_SIZE[1] ,dirent->FILE_SIZE[0] );
         skip = TRUE;
      }
   }

   if (!skip) {
      for (i=0;i<sizeof(MFS_DIR_ENTRY);i++) {
         printf("%02x ",(uint_32) lfn[i]);
      }
   }
}



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_dirent
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_dirent(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   uint_32           sector,num_sectors;
   int_32            offset;
   MQX_FILE_PTR      fd, fs;
   uint_32           e;
   uchar_ptr         buffer;
   MFS_DIR_ENTRY_PTR dirents;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2) || (argc > 4)) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else if ( !Shell_parse_uint_32(argv[1], (uint_32_ptr) &offset ))  {
         printf("Error, invalid length\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         num_sectors = 1;
         
         if (argc >= 3) {
            if ( !Shell_parse_uint_32(argv[2], (uint_32_ptr) &num_sectors )) {
               num_sectors = 1;
            }
         }      
         if (argc == 4) {
            fd = fopen(argv[3], "r");
            if (!fd) { 
               printf("Error, unable to open file %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            }
         } else {
            fs = Shell_get_current_filesystem(argv);
            _io_ioctl(fs, IO_IOCTL_GET_DEVICE_HANDLE, &fd);
         }

         if (fd)  {
            buffer = _mem_alloc(SECTOR_SIZE);
            if (buffer) {
               for(sector=0;sector<num_sectors;sector++) {
                  if (fseek(fd, offset+sector, IO_SEEK_SET) == IO_ERROR)  {
                     printf("Error, unable to seek to sector %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  } else  if (_io_read(fd, (char_ptr) buffer, 1) !=1) {
                     printf("Error, unable to read sector %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  } else if (!is_zero(buffer, SECTOR_SIZE)) {
                     printf("\nEntry # %d",offset+sector);
                     dirents = (MFS_DIR_ENTRY_PTR) buffer;

                     for (e=0;e<DIRENTS_PER_SECTOR;e++)  {
                        print_dirent(&dirents[e]);
                     }
                     printf("\n");
                  }
               }
               _mem_free(buffer);
            } else {
               printf("Error, unable to allocate sector buffer.\n" );
               return_code = SHELL_EXIT_ERROR;
            }
            if (argc >= 4) {
               fclose(fd);
            }
         }
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

/* EOF*/
