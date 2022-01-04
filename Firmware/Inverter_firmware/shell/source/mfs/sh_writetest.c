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
* $FileName: sh_writetest.c$
* $Version : 3.8.18.0$
* $Date    : Jun-7-2012$
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
#include "sh_prv.h"
#if SHELLCFG_USES_MFS
extern const char MFS_File_test_data[] ;
extern const uint_32 MFS_File_test_data_size;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_write_test
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/
#ifndef min
   #define min(a,b) ((a)<(b)?(a):(b))
#endif

int_32  Shell_write_test(int_32 argc, char_ptr argv[] )
{
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR      fd = NULL;
   char_ptr          read_buffer = NULL;
   uint_32           test_size = MFS_File_test_data_size;
   uint_32           write_size = 1,num,size,bytes_written;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   char_ptr             abs_path = NULL, filename = NULL;
   int_32               error = 0;


   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
         read_buffer = _mem_alloc(test_size + 1);
         _mem_zero (read_buffer, test_size + 1);

         if (read_buffer==NULL)  {
            printf("Error, unable to allocate read buffer\n");
            return_code = SHELL_EXIT_ERROR;
         } else {
            if (MFS_alloc_2paths(&abs_path,&filename) != MFS_NO_ERROR) {
               printf("Error, unable to allocate memory for paths\n" );
               return_code = SHELL_EXIT_ERROR;
            }
            else
            {
               for (write_size=8;write_size<test_size;write_size=write_size<<1) {
                  printf("Test Iteration #%d ", write_size);
                  sprintf(filename,"%s_%d",argv[1],write_size);
                  error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) filename,PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
                  if(!error)
                  {
                     fd = fopen(abs_path, "w");
                  }
                  if (fd == NULL || error) {
                     printf("Error, unable to open file to write\n");
                     return_code = SHELL_EXIT_ERROR;
                     break;
                  }
                  fseek(fd,0,IO_SEEK_SET);
                  bytes_written = 0;
                  while ( bytes_written<test_size) {
                     size = min(write_size,test_size-bytes_written);
                     num = write(fd, (pointer) &MFS_File_test_data[bytes_written],size);
                     if (num) {
                        bytes_written+=num;
                     } else {
                        printf("Write failed, offset = %d", bytes_written);
                       break;
                     }
                  }
                  fclose(fd);
                  fd = fopen(abs_path, "r");
                  if (fd == NULL) {
                     printf("Error, unable to open file to read\n");
                     return_code = SHELL_EXIT_ERROR;
                     break;
                  }
                  if ( read(fd,read_buffer,test_size) != test_size) {
                     printf("Error, unable to read all test data\n");
                     return_code = SHELL_EXIT_ERROR;
                     break;
                  }

                  if (strcmp(MFS_File_test_data,read_buffer)==0) {
                     printf("Passed\n");
                  } else {
                     printf("Failed\n");
                     break;
                  }
                  fclose(fd);
               }
               _mem_free(read_buffer);
               MFS_free_path(abs_path);
               MFS_free_path(filename);
            }
         }
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filename> \n", argv[0]);
      } else  {
         printf("Usage: %s <filename> \n", argv[0]);
         printf("   <filename>   = filename to verify\n");
      }
   }
   return return_code;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_write_test
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_append_test(int_32 argc, char_ptr argv[] )
{
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR      fd;
   char              filename[SHELL_MAX_FILELEN];
   char_ptr          read_buffer = NULL;
   uint_32           test_size = MFS_File_test_data_size;
   uint_32           write_size = 1,num,size,bytes_written;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   char                 abs_path[SHELL_PATHNAME_LEN];
   int_32               error = 0;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
         read_buffer = _mem_alloc(test_size);

         if (read_buffer==NULL)  {
            printf("Error, unable to allocate read buffer\n");
            return_code = SHELL_EXIT_ERROR;
         } else {
            for (write_size=512;write_size<test_size;write_size=write_size*2) {
               printf("\nTest Iteration #%d", write_size);
               sprintf(filename,"%s_%d",argv[1],write_size);
               error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) filename,PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
               bytes_written = 0;
               while ( bytes_written<test_size) {
                  fd = fopen(abs_path, "a");
                  if (fd == NULL || error) {
                     printf("Error, unable to open file to write\n");
                     return_code = SHELL_EXIT_ERROR;
                     break;
                  }
                  fseek(fd,bytes_written,IO_SEEK_SET);
                  size = min(write_size,test_size-bytes_written);
                  num = write(fd, (pointer) &MFS_File_test_data[bytes_written],size);
                  fclose(fd);
                  if (num) {
                     bytes_written+=num;
                  } else {
                     printf(" read failed, offset = %d", bytes_written);
                    break;
                  }
               }
               fd = fopen(abs_path, "r");
               if (fd == NULL || error) {
                  printf("Error, unable to open file to read\n");
                  return_code = SHELL_EXIT_ERROR;
                  break;
               }
               if ( read(fd,read_buffer,test_size) != test_size) {
                  printf("Error, unable to read all test data\n");
                  return_code = SHELL_EXIT_ERROR;
                  break;
               }

               if (strcmp(MFS_File_test_data,read_buffer)==0) {
                  printf("Passed");
               } else {
                  printf("Failed");
                  break;
               }
               fclose(fd);
            }
            _mem_free(read_buffer);
         }
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filename> \n", argv[0]);
      } else  {
         printf("Usage: %s <filename> \n", argv[0]);
         printf("   <filename>   = filename to verify\n");
      }
   }
   return return_code;
} /* Endbody */
#endif //SHELLCFG_USES_MFS
