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
* $FileName: sh_copy.c$
* $Version : 3.8.16.0$
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
#define MIN_COPY_BLOCK_SIZE  512
#define MAX_COPY_BLOCK_SIZE 8192

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  Shell_copy
* Returned Value   :  uint_32 error code
* Comments  :  Renames or moves a file.
*
*END*---------------------------------------------------------------------*/

int_32  Shell_copy(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean     print_usage, shorthelp = FALSE;
   int_32      size = 0, wsize = 0, copysize = MAX_COPY_BLOCK_SIZE, return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR in_fd=NULL, out_fd=NULL;
   char_ptr    abs_path=NULL;   
   boolean     cache_enabled=TRUE;
   char_ptr    copybuffer = NULL;
   _mqx_int    c;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   int_32               error = 0;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  
   {
      if (argc != 3)  
      {
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
      else if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) 
      {
         printf("Error, unable to allocate memory for paths\n" );
         return_code = SHELL_EXIT_ERROR;
      } 
      else 
      {
         error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
         if(!error)
         {
             in_fd = fopen(abs_path, "r");
         }
         
         if (in_fd == NULL || error)  {
             printf("Error, unable to open source file\n" );
             return_code = SHELL_EXIT_ERROR;
         } else {
            error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[2],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
            if(!error)
            {
                out_fd = fopen(abs_path, "w");
            }
            if (out_fd == NULL || error)  {
               printf("Error, unable to create destination file\n" );
               return_code = SHELL_EXIT_ERROR;
            } else {
               MFS_free_path(abs_path);         
               abs_path = NULL;

               do {
                  copybuffer = _mem_alloc(copysize);
                  if (copybuffer != NULL) break;
                  else copysize >>= 1;
               } while (copysize >= MIN_COPY_BLOCK_SIZE);
               
               if (copybuffer == NULL)
               {
                  printf("Warning, unable to allocate copy buffer, copy will be slower\n" );
                  while ((c = fgetc(in_fd)) >= 0) {
                     fputc(c, out_fd);
                  }
               }
               else {
                  do {
                     size = read(in_fd, copybuffer, copysize);
                     if (size > 0) {
                        wsize = write(out_fd, copybuffer, size);
                     }
                     else
                        break;
                  } while (wsize == size);
               }
            }
         }
         if (abs_path) MFS_free_path(abs_path);
         if (copybuffer) _mem_free(copybuffer);
         if (in_fd) fclose(in_fd); 
         if (out_fd) fclose(out_fd); 
      }
   }
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <source> <dest> \n", argv[0]);
      } else  {
         printf("Usage: %s <source> <dest>\n", argv[0]);
         printf("   <source> = source file to copy\n");
         printf("   <dest>   = name of new file\n");
      }
   }
   return return_code;
} /* Endbody */
#endif //SHELLCFG_USES_MFS

/* EOF*/
