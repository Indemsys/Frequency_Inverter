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
* $FileName: sh_flush.c$
* $Version : 3.8.4.0$
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


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_flush_cache
* Returned Value   :  int_32 error code
* Comments  :  flush the write cache.
*
* Usage:  flush [filesys:]
*
*END*---------------------------------------------------------------------*/

int_32  Shell_flush_cache(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean                    print_usage, shorthelp = FALSE;
   int_32                     return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR               fs_ptr;
   char_ptr                   name_ptr = NULL;

   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         if (argc==2) {
            name_ptr = argv[1];
         } else {
            name_ptr = Shell_get_current_filesystem_name(argv);
         }
 
         if (_io_validate_device(name_ptr)) {
            fs_ptr = fopen(name_ptr,0);
            if (fs_ptr == NULL )  {
               printf("Error, unable to access file system\n" );
               return_code = SHELL_EXIT_ERROR;
            } else  {
               ioctl(fs_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);
               fclose(fs_ptr);   
            } 
         } 
      } 
   } 
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filesys:>\n", argv[0]);
      } else  {
         printf("Usage: %s <filesys:>\n", argv[0]);
         printf("   filesys: = name of MFS file system\n");
      }
   } 
   return return_code;
} /* Endbody */


/* EOF*/
