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
* $FileName: sh_format.c$
* $Version : 3.7.14.0$
* $Date    : Feb-7-2011$
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

static const char_ptr BYTE_SIZES[] = { "bytes", "kB", "MB", "GB", "TB" };

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_format
* Returned Value   :  int_32 error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  format <drive:> [<volume label>]
*
*END*---------------------------------------------------------------------*/

int_32  Shell_format(int_32 argc, char_ptr argv[] )
{ /* Body */
   boolean                    print_usage, shorthelp = FALSE;
   int_32                     return_code = SHELL_EXIT_SUCCESS;
   int_32                     error_code = IO_ERROR;
   MQX_FILE_PTR               fs_ptr = NULL;
   char_ptr                   volume_ptr[SFILENAME_SIZE];
   uint_64                    big_number;
   uint_32                    small_number;
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc < 2 || argc >  3) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
         if (argc == 3)  {
            memcpy (volume_ptr, argv[2], SFILENAME_SIZE);
         } else  {
            volume_ptr[0] = 0;
         }
         fs_ptr = _io_get_fs_by_name(argv[1]);
         if (fs_ptr == NULL)  {
            printf("Error, file system %s not found\n", argv[1] );
            return_code = SHELL_EXIT_ERROR;
         } else {
            printf("\nFormating...\n");
            error_code = ioctl(fs_ptr, IO_IOCTL_DEFAULT_FORMAT,  NULL);
            if ( !error_code && (*volume_ptr)) {
               error_code = ioctl(fs_ptr, IO_IOCTL_SET_VOLUME,  (pointer) volume_ptr);
            }       
            if (error_code) {
               printf("Error while formatting: 0x%x\n", error_code);
            } else  {
               /* print disk information */
               error_code = ioctl(fs_ptr, IO_IOCTL_GET_VOLUME, (uint_32_ptr) volume_ptr);
               printf("Done. Volume name is %s\nFree disk space: ", volume_ptr);
               ioctl(fs_ptr, IO_IOCTL_GET_CLUSTER_SIZE, &small_number);
               big_number = (uint_64)ioctl(fs_ptr, IO_IOCTL_FREE_CLUSTERS, NULL) * small_number;
               for (small_number = 0; big_number > MAX_UINT_32; big_number >>= 10) small_number++;
               printf ("%lu %s\n", (uint_32)big_number, BYTE_SIZES[small_number]);
            } 
         }
      }
   }
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <drive:> [<volume label>]\n", argv[0]);
      } else  {
         printf("Usage: %s <drive:> [<volume label>]\n", argv[0]);
         printf("   <drive:> = specifies the drive name (followed by a colon)\n");
         printf("   <volume label>  = specifies the volume label\n");
      }
   }
   return return_code;
}


/* EOF*/
