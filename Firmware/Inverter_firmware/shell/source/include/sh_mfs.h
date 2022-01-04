/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: sh_mfs.h$
* $Version : 3.8.10.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#ifndef __sh_mfs_h__
#define __sh_mfs_h__

/*
** Function prototypes 
*/
#ifdef __cplusplus
extern "C" {
#endif

extern int_32 Shell_cache(int_32 argc, char_ptr argv[] );
extern int_32 Shell_compare(int_32 argc, char_ptr argv[] );
extern int_32 Shell_create(int_32 argc, char_ptr argv[] );
extern int_32 Shell_flush_cache(int_32 argc, char_ptr argv[] );
extern int_32 Shell_cd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_copy(int_32 argc, char_ptr argv[] );
extern int_32 Shell_del(int_32 argc, char_ptr argv[] );
extern int_32 Shell_df(int_32 argc, char_ptr argv[] );
extern int_32 Shell_di(int_32 argc, char_ptr argv[] );
extern int_32 Shell_dir(int_32 argc, char_ptr argv[] );
extern int_32 Shell_dirent(int_32 argc, char_ptr argv[] );
extern int_32 Shell_disect(int_32 argc, char_ptr argv[] );
extern int_32 Shell_format(int_32 argc, char_ptr argv[] );
extern int_32 Shell_mkdir(int_32 argc, char_ptr argv[] );
extern int_32 Shell_pwd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_read(int_32 argc, char_ptr argv[] );
extern int_32 Shell_rename(int_32 argc, char_ptr argv[] );
extern int_32 Shell_rmdir(int_32 argc, char_ptr argv[] );
extern int_32 Shell_type(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_use(int_32 argc, char_ptr argv[] );
extern int_32 Shell_write(int_32 argc, char_ptr argv[] );
extern int_32 Shell_write_test(int_32 argc, char_ptr argv[] );
extern int_32 Shell_read_test(int_32 argc, char_ptr argv[] );
extern int_32 Shell_append_test(int_32 argc, char_ptr argv[] );

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
