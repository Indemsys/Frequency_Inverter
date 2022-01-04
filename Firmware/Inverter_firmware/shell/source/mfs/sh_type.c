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
* $FileName: sh_type.c$
* $Version : 3.8.12.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <mfs.h>
#include <shell.h>
#include <sh_prv.h>

#if SHELLCFG_USES_MFS

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_type
*  Returned Value:  none
*  Comments  :  SHELL utility to Ping a host
*
*END*-----------------------------------------------------------------*/

int_32  Shell_type(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR         fd = NULL;
   char_ptr             abs_path;   
   _mqx_int             c;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   int_32               error = 0;   
   

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 2)  {
         /* check if filesystem is mounted */ 
         if (NULL == Shell_get_current_filesystem(argv))    {
            printf("Error, file system not mounted\n" );
            return_code = SHELL_EXIT_ERROR;
         }
         else if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
            printf("Error, unable to allocate memory for paths\n" );
            return_code = SHELL_EXIT_ERROR;
         } 
         else {
            error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
            if(!error)
            {
               fd = fopen(abs_path, "r");
            }
            MFS_free_path(abs_path);
            if (fd && !error)  {
               do {
                  c = fgetc(fd);
                  if (c!= IO_EOF) {
                     fputc((char)c, stdout);
                  }
               } while (c!=IO_EOF);
            
               fclose(fd);
               printf("\n");
            } else  {
               printf("Error, unable to open file %s.\n", argv[1] );
               return_code = SHELL_EXIT_ERROR;
            }
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filename>\n", argv[0]);
      } else  {
         printf("Usage: %s <filename>\n", argv[0]);
         printf("   <filename>   = filename to display\n");
      }
   }
   return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS

/* EOF */
