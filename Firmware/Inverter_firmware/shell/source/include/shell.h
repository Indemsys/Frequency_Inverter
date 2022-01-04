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
* $FileName: shell.h$
* $Version : 3.8.23.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the shell API.
*
*END************************************************************************/

#ifndef __shell_h__
#define __shell_h__

#include <fio.h>

#ifdef __rtcs_h__
#include <sh_rtcs.h>
#endif

#ifdef __mfs_h__
#include <sh_mfs.h>
#endif

/** MGCT: <category name="Shell Settings"> */

/*
** MGCT: <option type="bool"/>
*/  
#ifndef SHELLCFG_USES_MFS
#define SHELLCFG_USES_MFS   1
#endif

/*
** MGCT: <option type="bool"/>
*/  
#ifndef SHELLCFG_USES_RTCS
#define SHELLCFG_USES_RTCS  1
#endif

/** MGCT: </category> */

#ifdef BSP_DEFAULT_ENET_DEVICE
#include <sh_enet.h>
#endif

#define SHELL_MAX_ARGS       14

#define SHELL_EXIT_SUCCESS   0
#define SHELL_EXIT_ERROR    -1

#define SHELL_MAX_DEVLEN    8
#define SHELL_MAX_FILELEN   256
#define SHELL_PATHNAME_LEN  260
#define SHELL_BLOCK_SIZE    128

#define MEM_TYPE_SHELL_BASE                   ( (IO_SHELL_COMPONENT) << (MEM_TYPE_COMPONENT_SHIFT))
#define MEM_TYPE_SHELL_CONTEXT                (MEM_TYPE_SHELL_BASE+1)

#define hexnum(c)  ((c<='9')?(c-'0'):((c<='F')?(c-'A'+10) :(c-'a'+10))) 

#define SHELL_COMMAND(cmd) { #cmd, Shell_##cmd },

typedef struct shell_command_struct  {
   char_ptr  COMMAND;
   int_32      (*SHELL_FUNC)(int_32 argc, char_ptr argv[]);
} SHELL_COMMAND_STRUCT, _PTR_ SHELL_COMMAND_PTR; 


/*
** Function prototypes 
*/

#ifdef __cplusplus
extern "C" {
#endif

extern MQX_FILE_PTR Shell_default_fs;
extern char_ptr Shell_default_fs_name;

extern const SHELL_COMMAND_STRUCT Shell_commands[];

extern int_32  Shell( const SHELL_COMMAND_STRUCT[], char_ptr );
extern int_32  Shell_parse_command_line( char_ptr command_line_ptr, char_ptr argv[] );
extern boolean Shell_parse_number( char_ptr arg, uint_32_ptr num_ptr);
extern boolean Shell_check_help_request(int_32 argc, char_ptr argv[], boolean _PTR_ short_ptr );
extern boolean Shell_parse_uint_32( char_ptr arg, uint_32_ptr num_ptr);
extern boolean Shell_parse_uint_16( char_ptr arg, uint_16_ptr num_ptr);
extern boolean Shell_parse_int_32( char_ptr arg, int_32_ptr num_ptr);
extern boolean Shell_parse_hexnum( char_ptr arg, uint_32_ptr num_ptr);
extern void Shell_create_prefixed_filename( char_ptr new_ptr, char_ptr in_ptr,  pointer argv);
extern int_32 Shell_abort(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_command_list(int_32 argc, char_ptr argv[] );
extern int_32 Shell_exit(int_32 argc, char_ptr argv[] );
extern int_32 Shell_help(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_kill(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_pause(int_32 argc, char_ptr argv[] );
extern int_32 Shell_tad(int_32 argc, char_ptr argv[] );
extern int_32 Shell_sh(int_32 argc, char_ptr argv[] );
extern int_32 Shell_md(int_32 argc, char_ptr argv[] );

extern MQX_FILE_PTR Shell_get_current_filesystem(pointer argv);
extern char_ptr Shell_get_current_filesystem_name(pointer argv);

extern int_32 Shell_set_current_filesystem(pointer argv, MQX_FILE_PTR fp);
extern int_32 Shell_set_current_filesystem_by_name(pointer argv, char_ptr dev_name);


extern int_32 Shell_erase(int_32 argc, char_ptr argv[] );
extern int_32 Shell_write_block(int_32 argc, char_ptr argv[] );
extern int_32 Shell_read_block(int_32 argc, char_ptr argv[] );

#ifdef __cplusplus
}
#endif

#endif
/*EOF */
