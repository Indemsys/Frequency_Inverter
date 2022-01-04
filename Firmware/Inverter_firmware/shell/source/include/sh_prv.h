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
* $FileName: sh_prv.h$
* $Version : 3.6.7.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#ifndef __sh_prv_h__
#define __sh_prv_h__

#define SHELL_MAX_CMDLEN     80

#define Shell_get_context(ptr) ((SHELL_CONTEXT_PTR)(pointer) ptr)


typedef struct {
   char_ptr             ARGV[SHELL_MAX_ARGS];           // MUST BE FIRST !!
   int_32               ARGC;
   char                 CMD_LINE[SHELL_MAX_CMDLEN];
   char                 HISTORY[SHELL_MAX_CMDLEN];
   SHELL_COMMAND_PTR    COMMAND_LIST_PTR;
   MQX_FILE_PTR         COMMAND_FP;
   boolean              EXIT;
#if SHELLCFG_USES_MFS   
   char                 CURRENT_DEVICE_NAME[SHELL_MAX_DEVLEN]; 
   MQX_FILE_PTR         CURRENT_DEVICE_FP;
   char                 CURRENT_DIR[SHELL_PATHNAME_LEN + 1];
#endif   
} SHELL_CONTEXT, _PTR_ SHELL_CONTEXT_PTR;

/*
** Function prototypes 
*/
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif
/*EOF*/
