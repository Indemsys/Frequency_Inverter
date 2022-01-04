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
* $FileName: sh_cache.c$
* $Version : 3.8.9.0$
* $Date    : Jul-25-2012$
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

#if SHELLCFG_USES_MFS
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_cache
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  cache [on|off] [<filesys:>]
*
*END*---------------------------------------------------------------------*/

int_32  Shell_cache(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean                    print_usage, shorthelp = FALSE;
   int_32                     return_code = SHELL_EXIT_SUCCESS;
   int_32                     i;
   MQX_FILE_PTR               fs_ptr;
   char_ptr                   name_ptr = NULL;
   boolean                    on = TRUE;

   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc >  3) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
      
         for (i=1;i<argc;i++)  {
            if (strcmp("on", argv[i])==0)  {
               on = TRUE;
            } else if (strcmp("off", argv[i])==0)  {
               on = FALSE;
            } else if (_io_validate_device(argv[i])) {
               name_ptr = argv[i];
            } else  {
               printf("Error, invalid parameter\n");
               return_code = SHELL_EXIT_ERROR;
               print_usage = TRUE;
               break;
            }
         }
         
         if (return_code == SHELL_EXIT_SUCCESS)  {
            if (name_ptr==NULL) {
               name_ptr = Shell_get_current_filesystem_name(argv);
            }
    
           fs_ptr = fopen(name_ptr,0);
            if (fs_ptr == NULL )  {
               printf("Error, unable to access file system\n" );
               return_code = SHELL_EXIT_ERROR;
            } else  {
               ioctl(fs_ptr, (on ? IO_IOCTL_WRITE_CACHE_ON : IO_IOCTL_WRITE_CACHE_OFF), NULL);
               fclose(fs_ptr);   
            } 
         }
      }
   }
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [on|off] [<filesys:>]\n", argv[0]);
      } else  {
         printf("Usage: %s [on|off] [<filesys:>]\n", argv[0]);
         printf("   <filesys:> = name of MFS file system\n");
      }
   }
   return return_code;
}


#endif //SHELLCFG_USES_MFS

/* EOF*/
