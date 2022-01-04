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
* $FileName: sh_dir.c$
* $Version : 3.8.14.0$
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
#include <mfs.h>
#include <shell.h>
#include <sh_prv.h>

#if SHELLCFG_USES_MFS
#define BUFFER_SIZE  256

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_dir
* Returned Value   :  int_32 error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_dir(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;
   int_32               len;
   MQX_FILE_PTR         fs_ptr;
   char_ptr             path_ptr, mode_ptr;
   pointer              dir_ptr;
   char_ptr             buffer = NULL;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   int_32               error = 0;


   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 3)  {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         path_ptr  ="*.*";
         mode_ptr = "m";
         if (argc >= 2)  {
            path_ptr = argv[1];
            if (argc== 3)  {
               mode_ptr = argv[2];
            }
         }
         
         fs_ptr = Shell_get_current_filesystem(argv);
         /* check if filesystem is mounted */ 
         if (fs_ptr == NULL)  {
             printf("Error, file system not mounted\n");
             return_code = SHELL_EXIT_ERROR;
         } else  {
            buffer = _mem_alloc(BUFFER_SIZE);
            error = ioctl(fs_ptr, IO_IOCTL_CHANGE_CURRENT_DIR, shell_ptr->CURRENT_DIR);
            if (buffer && !error) {
            
               dir_ptr = _io_mfs_dir_open(fs_ptr, path_ptr, mode_ptr );
            
               if (dir_ptr == NULL)  {
                  printf("File not found.\n");
                  return_code = SHELL_EXIT_ERROR;
               } else {
                  while ((_io_is_fs_valid(fs_ptr)) && (len = _io_mfs_dir_read(dir_ptr, buffer, BUFFER_SIZE)) > 0) {
                     printf(buffer);
                  } 
                  _io_mfs_dir_close(dir_ptr);
               }
               _mem_free(buffer);
            } else {
               if(buffer == NULL){
                 printf("Error, unable to allocate space.\n" );
               } else {
                 printf("Error, directory does not exist.\n" );
               }
               return_code = SHELL_EXIT_ERROR;
            }
         }
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [<filespec>] [<attr>]]\n", argv[0]);
      } else  {
         printf("Usage: %s [<filespec> [<attr>]]\n", argv[0]);
         printf("   <filespec>   = files to list\n");
         printf("   <attr>       = attributes of files: adhrsv*\n");
      }
   }
   return return_code;
} /* Endbody */
#endif //SHELLCFG_USES_MFS
/* EOF*/
