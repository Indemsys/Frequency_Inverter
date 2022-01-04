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
* $FileName: sh_compare.c$
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
#define COMPARE_BLOCK_SIZE 512

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  Shell_compare
* Returned Value   :  uint_32 error code
* Comments  :  Renames or moves a file.
*
*END*---------------------------------------------------------------------*/

int_32  Shell_compare(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean     print_usage, shorthelp = FALSE;
   int_32      size1, size2, return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR in_fd_1=NULL, in_fd_2=NULL;
   char_ptr    file1=NULL,file2=NULL;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   int_32               error = 0;      

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 3)  {
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
      else if (MFS_alloc_path(&file1) != MFS_NO_ERROR) {
         printf("Error, unable to allocate memory for paths\n" );
         return_code = SHELL_EXIT_ERROR;
      } else {
         error = _io_rel2abs(file1,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
         if(!error)
         {
            in_fd_1 = fopen(file1, "r");
         }      

         error = _io_rel2abs(file1,shell_ptr->CURRENT_DIR,(char *) argv[2],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
         if(!error)
         {
            in_fd_2 = fopen(file1, "r");
         }  

         MFS_free_path(file1);         

         if (in_fd_1 == NULL)  {
             printf("Error, unable to open file %s\n", argv[1] );
             return_code = SHELL_EXIT_ERROR;
         } else  if (in_fd_2 == NULL)  {
             printf("Error, unable to open file %s\n", argv[2] );
             return_code = SHELL_EXIT_ERROR;
         } else {
            file1 = _mem_alloc(COMPARE_BLOCK_SIZE);
            file2 = _mem_alloc(COMPARE_BLOCK_SIZE);
            if ((file1==NULL) || (file2==NULL)) {
               printf("Error, unable to allocate buffer space" );
               return_code = SHELL_EXIT_ERROR;
            } else {
               do {
                  size1 = read(in_fd_1, file1, COMPARE_BLOCK_SIZE);
                  size2 = read(in_fd_2, file2, COMPARE_BLOCK_SIZE);
                  if (size1!=size2) {
                     printf("Compare failed, files have different sizes\n" );
                     return_code = SHELL_EXIT_ERROR;
                  } else if (size1 > 0) {
                     if (memcmp(file1,file2,COMPARE_BLOCK_SIZE)!=0) {
                        printf("Compare failed, files are different\n" );
                        return_code = SHELL_EXIT_ERROR;
                     }                     
                  }
               } while ((size1>0) && (return_code == SHELL_EXIT_SUCCESS));
               if (return_code == SHELL_EXIT_SUCCESS) {
                  printf("The files are identical\n" );
               }
            }
            if (file1) _mem_free(file1);
            if (file2) _mem_free(file2);
         }
         if (in_fd_1) fclose(in_fd_1); 
         if (in_fd_2) fclose(in_fd_2); 
      }
   }
      
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <file1> <file2> \n", argv[0]);
      } else  {
         printf("Usage: %s <file1> <file2>\n", argv[0]);
         printf("   <file1> = first file to compare\n");
         printf("   <file2> = second file to compare\n");
      }
   }
   return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS
/* EOF*/
