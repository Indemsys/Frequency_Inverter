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
* $FileName: sh_tftp.c$
* $Version : 3.8.13.0$
* $Date    : Oct-6-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"
        
         
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_TFTP_client
*  Returned Value:  none
*  Comments  :  SHELL utility to TFTP to or from a host
*  Usage:  tftp host get source [destination] [mode] 
*
*END*-----------------------------------------------------------------*/

int_32  Shell_TFTP_client(int_32 argc, char_ptr argv[] )
{
   _ip_address          hostaddr;
   char                 hostname[MAX_HOSTNAMESIZE]="";
   char_ptr             file_ptr;
   uint_32              tftp_handle, buffer_size,byte_number;
   uchar_ptr            buffer_ptr;
   TFTP_DATA_STRUCT     tftp_data;
   MQX_FILE_PTR         fd;
   int_32               error;
   boolean              print_usage, shorthelp = FALSE;
   int_32               return_code = SHELL_EXIT_SUCCESS;
   char                 path[SHELL_MAX_FILELEN];  
   boolean              trans = FALSE;
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
   
      if ((argc >= 3) && (argc <= 5))  {
         RTCS_resolve_ip_address( argv[1], &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (!hostaddr)  {
            printf("Unable to resolve host.\n");
            return_code = SHELL_EXIT_ERROR;
         } else  {
            tftp_data.SERVER   = hostaddr;
            tftp_data.FILENAME = argv[2];
            tftp_data.FILEMODE = "netascii";
            if (argc > 3)  {
               file_ptr = argv[3];
               if (argc > 4) {
                  tftp_data.FILEMODE = argv[4];
               } else {
                  tftp_data.FILEMODE = "netascii";
               }
            } else {
               file_ptr = argv[2];
            }
#if SHELLCFG_USES_MFS  
            
            Shell_create_prefixed_filename(path, file_ptr, argv);
            fd = fopen(path,"a");
            if (fd)  {
               printf("\nDownloading file %s from TFTP server: %s [%ld.%ld.%ld.%ld]\n",
                  tftp_data.FILENAME,hostname, IPBYTES(hostaddr));
               tftp_handle = (*FT_TFTP->OPEN)( (pointer) &tftp_data );
               if ( tftp_handle != RTCS_OK )  {
                  printf("\nError opening file %s\n",tftp_data.FILENAME);
                  return_code = SHELL_EXIT_ERROR;
               } else  {
                if (! (*FT_TFTP->EOFT)())  {
                   do {
                     buffer_ptr = (*FT_TFTP->READ)( &buffer_size );
                     if ((buffer_ptr != NULL) && (buffer_size))  {
                         fseek(fd, 0 , IO_SEEK_CUR);
                        _io_write(fd,buffer_ptr,buffer_size); 
                        trans = TRUE;
                     } else {
                   
                         switch (buffer_size) {
                         case 0:
                            // end of file
                           break;
                         case (RTCSERR_TFTP_ERROR + 1):
                            printf("\nFile %s not found\n", tftp_data.FILENAME);
                            break;
                         case (RTCSERR_TFTP_ERROR + 2):
                            printf("\nAccess violation\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 3):
                            printf("\nDisk full or allocation exceeded\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 4):
                            printf("\nIllegal TFTP operation\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 5):
                            printf("\nUnknown transfer ID\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 6):
                            printf("\nFile already exists\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 7):
                            printf("\nNo such user\n");
                            break;
                         default:
                            if(trans) 
                            {
                              trans =FALSE;
                              break;
                            }
                          else
                            printf("\nError reading file %s\n", tftp_data.FILENAME);
                         } /* Endswitch */
                        }
                     } while((buffer_ptr !=NULL) && buffer_size &&  (! (*FT_TFTP->EOFT)()) );
                     fclose(fd);
                  }
                  
                  error = (*FT_TFTP->CLOSE)();
                  
               }
               
            } else  {
               printf("\nError opening local file %s\n",file_ptr);
               return_code = SHELL_EXIT_ERROR;
            }
#else
            tftp_handle = (*FT_TFTP->OPEN)( (pointer) &tftp_data );
            if ( tftp_handle != RTCS_OK )  {
               printf("\nError opening file %s\n",tftp_data.FILENAME);
               return_code = SHELL_EXIT_ERROR;
            } else  {
               printf("SHELLCFG_USES_MFS is not set to 1 in user_config.h - file wont be written to disk\n");
            }
            error = (*FT_TFTP->CLOSE)();
#endif            
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <host> <source> [<dest>] [<mode>]\n", argv[0]);
      } else  {
         printf("Usage: %s <host> <source> [<dest>] [<mode>]\n", argv[0]);
         printf("   <host>   = host ip address or name\n");
         printf("   <source> = remote file name\n");
         printf("   <dest>   = local file name\n");
         printf("   <mode>   = file transfer mode (netascii, etc.)\n");
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */

/* EOF */
