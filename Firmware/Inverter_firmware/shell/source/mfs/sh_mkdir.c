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
* $FileName: sh_mkdir.c$
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


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_mkdir
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  mkdir <directory>
*
*END*---------------------------------------------------------------------*/

int_32  Shell_mkdir(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean                    print_usage, temp, shorthelp = FALSE;
   int_32                     error = 0, return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR               fs_ptr;
   SHELL_CONTEXT_PTR          shell_ptr = Shell_get_context( argv );
   char_ptr                   abs_path = NULL;
 
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc !=  2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
         if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
            printf("Error, unable to allocate memory for paths\n" );
            return_code = SHELL_EXIT_ERROR;
         }
         else
         {
            _io_get_dev_for_path(abs_path,&temp,PATHNAME_SIZE,(char *)argv[1],Shell_get_current_filesystem_name(shell_ptr));
            fs_ptr = _io_get_fs_by_name(abs_path);
            /* check if filesystem is mounted */
            if (fs_ptr == NULL)  {
               printf("Error, file system not mounted\n" );
               return_code = SHELL_EXIT_ERROR;
            } else  {
               error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,Shell_get_current_filesystem_name(shell_ptr));
               if(!error)
               {
                  error = ioctl(fs_ptr, IO_IOCTL_CREATE_SUBDIR, (pointer) abs_path);
               }
               if (error)  {
                  printf("Error creating directory %s\n", argv[1]);
               }
            }
          MFS_free_path(abs_path);  
         }
      }
   }
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <directory> \n", argv[0]);
      } else  {
         printf("Usage: %s <directory>\n", argv[0]);
         printf("   <directory> = name of directory to create\n");
      }
   }
   return return_code;
}

#endif //SHELLCFG_USES_MFS
/* EOF*/
