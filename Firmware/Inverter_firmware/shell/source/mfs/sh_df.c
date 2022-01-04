/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: sh_df.c$
* $Version : 3.8.3.0$
* $Date    : May-16-2011$
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
* Function Name    :   Shell_df
* Returned Value   :  int_32 error code
* Comments  :  Prints out disk free information for current filesystem .
*
*END*---------------------------------------------------------------------*/

int_32  Shell_df(int_32 argc, char_ptr argv[])
{ /* Body */
    boolean              print_usage;
    boolean              shorthelp = FALSE;
    int_32               return_code = SHELL_EXIT_SUCCESS;
    SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context(argv);
    MQX_FILE_PTR         fs_ptr;
    int_64               space;
    int_32               clusters;
    uint_32              cluster_size;
    int_32               error = 0;
    char_ptr             fs_name;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp);

    if (!print_usage)  {
        if (argc > 2)  {        
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s [<filesystem>]\n", argv[0]);
        }
        else {
            printf("Usage: %s [filesystem]\n", argv[0]);
            printf("   <filesystem> = filesystem to query for free space\n");
        }
        return return_code;
    }
           

    if (argc == 2) {
        fs_name = argv[1];
        fs_ptr = _io_get_fs_by_name(fs_name);
    }
    else {
        fs_name = Shell_get_current_filesystem_name(shell_ptr);
        fs_ptr = Shell_get_current_filesystem(argv);
    }          

    /* check if filesystem is mounted */ 
    if (fs_ptr == NULL)  {
        printf("Error, file system not mounted\n");
        return return_code = SHELL_EXIT_ERROR;
    }


    ioctl(fs_ptr, IO_IOCTL_FREE_SPACE, &space);
    clusters = ioctl(fs_ptr, IO_IOCTL_FREE_CLUSTERS, NULL);
    error = ioctl(fs_ptr, IO_IOCTL_GET_CLUSTER_SIZE, &cluster_size);
    if (error) cluster_size=0;

    printf("Free disk space on %s\n", fs_name);
    printf("%ld clusters, %ld bytes each\n", (long int)clusters, (long int)cluster_size);
    printf("%lu KB\n", (unsigned long int)(space>>10));

    return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS
