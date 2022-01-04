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
* $FileName: sh_disect.c$
* $Version : 3.8.9.0$
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
#include <mfs.h>
#include <sh_mfs.h>

#define SECTOR_SIZE 512


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_disect
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/


int_32  Shell_disect(int_32 argc, char_ptr argv[] )
{ /* Body */
   int_32            return_code = SHELL_EXIT_SUCCESS;
   boolean           print_usage, shorthelp = FALSE;
   uint_32           sector,num_sectors;
   int_32            offset=0;
   MQX_FILE_PTR      fd, fs;
   uint_32           e,i;
   uchar_ptr         buffer;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2) || (argc > 5)) {
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
         if (argc >= 4) {
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
                  } else if (_io_read(fd, (char_ptr) buffer, 1) != 1) {
                     printf("Error, unable to read sector %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  } else { 
                     printf("\nSector # %d\n",offset+sector);
                     for (e=0;e<16;e++)  {
                        for (i=0;i<32;i++) {
                           printf("%02x ",(uint_32) buffer[e*32+i]);
                        }
                        printf("\n");
                     }
                  }
               }
               _mem_free(buffer);
            }
         }
         printf("\n");
         if (argc >= 4) {
            fclose(fd);
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
