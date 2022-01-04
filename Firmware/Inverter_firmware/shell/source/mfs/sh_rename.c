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
* $FileName: sh_rename.c$
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
#include <mfs.h>
#include <shell.h>
#include <sh_prv.h>

#if SHELLCFG_USES_MFS

static int_32 Shell_mfs_rename(MQX_FILE_PTR fs_ptr,char_ptr old_ptr, char_ptr new_ptr)
{
   MFS_RENAME_PARAM           rename_struct;
   
   rename_struct.OLD_PATHNAME = old_ptr;
   rename_struct.NEW_PATHNAME = new_ptr;

   return ioctl(fs_ptr, IO_IOCTL_RENAME_FILE, (pointer) &rename_struct);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  Shell_rename
* Returned Value   :  uint_32 error code
* Comments  :  Renames or moves a file.
*
*END*---------------------------------------------------------------------*/

int_32  Shell_rename(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean                    print_usage, temp, shorthelp = FALSE;
   int_32                     error = 0, return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR               fs_ptr;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   char_ptr              abs_path_old = NULL;
   char_ptr              abs_path_new = NULL;


   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 3)  {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
         if (MFS_alloc_2paths(&abs_path_old,&abs_path_new) != MFS_NO_ERROR) {
            printf("Error, unable to allocate memory for paths\n" );
            return_code = SHELL_EXIT_ERROR;
         }
         else
         {
            _io_get_dev_for_path(abs_path_old,&temp,PATHNAME_SIZE,(char *)argv[1],Shell_get_current_filesystem_name(shell_ptr));
            _io_get_dev_for_path(abs_path_new,&temp,PATHNAME_SIZE,(char *)argv[2],Shell_get_current_filesystem_name(shell_ptr));            
            if (strcmp(abs_path_new,abs_path_old))
            {
               printf("Error, rename is possible only within one device\n" );
               return_code = SHELL_EXIT_ERROR;
            } else {
            fs_ptr = _io_get_fs_by_name(abs_path_new);
               if (fs_ptr == NULL)  {
                  printf("Error, file system not mounted\n" );
                  return_code = SHELL_EXIT_ERROR;
               } else {
                  error = _io_rel2abs(abs_path_old,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,Shell_get_current_filesystem_name(shell_ptr));
                  if(!error)
                  {
                     error = _io_rel2abs(abs_path_new,shell_ptr->CURRENT_DIR,(char *) argv[2],PATHNAME_SIZE,Shell_get_current_filesystem_name(shell_ptr));
                     if(!error)
                     {
                        error = Shell_mfs_rename(fs_ptr, abs_path_old, abs_path_new );
                     }
                  }
                  if (error)  {
                     printf("Error renaming file %s\n", argv[1]);
                  }
               }
            }
         }
      }
   }
   
   MFS_free_path(abs_path_old);
   MFS_free_path(abs_path_new);
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <oldname> <newname> \n", argv[0]);
      } else  {
         printf("Usage: %s <oldname> <newname>\n", argv[0]);
         printf("   <oldname> = name of file to change\n");
         printf("   <newname> = new name of file\n");
      }
   }
   return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS
/* EOF*/
