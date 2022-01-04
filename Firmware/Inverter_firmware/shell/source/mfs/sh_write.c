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
* $FileName: sh_write.c$
* $Version : 3.8.15.0$
* $Date    : Aug-6-2012$
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
#include <sh_prv.h>

#if SHELLCFG_USES_MFS

#define MAX_BUFF_SIZE 8192

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_write
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_write(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean           print_usage, shorthelp = FALSE;
   int_32            return_code = SHELL_EXIT_SUCCESS;
   uint_32           count=0;
   int_32            offset=0;
   int_32            seek_mode=0;
   char              c;
   MQX_FILE_PTR      fd = NULL;
   char_ptr          abs_path;   
   boolean           cache_enabled=TRUE;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   char_ptr             buf = NULL;
   int_32               writesize = MAX_BUFF_SIZE;
   int_32               error = 0;
   _mqx_int             result;
   _mqx_int             bi;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2) || (argc > 5)) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } 
      /* check if filesystem is mounted */ 
      else if (NULL == Shell_get_current_filesystem(argv))  
      {
         printf("Error, file system not mounted\n");
         return_code = SHELL_EXIT_ERROR;
      }
      else  {
         count = 0;
         offset = 0;
         seek_mode = IO_SEEK_CUR; 
         if (argc >= 3)  {
            if (! Shell_parse_uint_32(argv[2], &count ))  {
               printf("Error, invalid length\n");
               return_code = SHELL_EXIT_ERROR;
               print_usage=TRUE;
            } else  {
               if (argc >= 5)  {
                  if (strcmp(argv[3], "begin") == 0) {
                     seek_mode = IO_SEEK_SET;
                  } else if (strcmp(argv[3], "end") == 0) { 
                     seek_mode = IO_SEEK_END;
                  } else if (strcmp(argv[3], "current") == 0) { 
                     seek_mode = IO_SEEK_CUR;
                  } else { 
                     printf("Error, invalid seek type\n");
                     return_code = SHELL_EXIT_ERROR;
                     print_usage=TRUE;
                  }
                  
                  if (return_code == SHELL_EXIT_SUCCESS)  {
                     if (! Shell_parse_int_32(argv[4], &offset ))  {
                        printf("Error, invalid seek value\n");
                        return_code = SHELL_EXIT_ERROR;
                        print_usage=TRUE;
                     }
                  }
               }
            }
         }
      }
     
      if (return_code == SHELL_EXIT_SUCCESS)  {
         if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
            printf("Error, unable to allocate memory for paths\n" );
            return_code = SHELL_EXIT_ERROR;
         } else {
            error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
             
            do {
               buf = _mem_alloc(writesize);
               if (buf != NULL) break;
               else writesize >>= 1;
            } while (writesize > 0);
             
            if (buf != NULL)
            {   
               if(!error)
               {
                  fd = fopen(abs_path, "a");
               }
               if (fd && !error) {
                  if (fseek(fd, offset, seek_mode ) != IO_ERROR) {
                   
                     c = '0';
                     while (count) {

                         /* generate data to buf */
                         for (bi = 0; (bi < count) && (bi < writesize); bi++) {
                             buf[bi] = c;
                             c = (c == 'z') ? '0' : c+1;
                         }

                         result = write(fd, buf, bi);

                         if (result != bi) {
                             /* incomplete write */
                             printf("Error writing file %s.\n", argv[1] );
                             error = ferror(fd);
                             if (error == MFS_DISK_FULL)
                                 printf("Disk full.\n" );
                             return_code = SHELL_EXIT_ERROR;
                             break;
                         }

                         count -= result;
                     }                    
                     
                  } else {
                     printf("Error, unable to seek file %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  }

                  fclose(fd);

               } else  {
                  printf("Error, unable to open file %s.\n", argv[1] );
                  return_code = SHELL_EXIT_ERROR;
               }
               MFS_free_path(abs_path);
               _mem_free(buf);
            }
            else {
               printf("Error, unable to allocate memory for write buffer\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filename> <bytes> [<seek_mode>] [<offset>]\n", argv[0]);
      } else  {
         printf("Usage: %s <filename> <bytes> [<seek_mode>] [<offset>]\n", argv[0]);
         printf("   <filename>   = filename to write\n");
         printf("   <bytes>      = number of bytes to write\n");
         printf("   <seek_mode>  = one of: begin, end or current\n");
         printf("   <offset>     = seek offset\n");
      }
   }
   return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS
/* EOF */
