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
* $FileName: sh_enet.c$
* $Version : 3.8.6.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   Example using RTCS Library.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include <enet.h>
#include "sh_rtcs.h" 
#include "sh_enet.h" 

 

boolean Shell_parse_enet_address( char_ptr arg, _enet_address enet_address)
{
   int i,j=0;
   
   if (strlen(arg) != 17)  {
      return FALSE;
   }
   
   for (i=0;i<16;i+=3)  {
      if ( isxdigit(arg[i]) && isxdigit(arg[i+1]) && ((arg[i+2]==':') || (arg[i+2]=='\0')) )  {
         enet_address[j++] = hexnum(arg[i]) * 16 + hexnum(arg[i+1]);
      } else  {
         return FALSE;
      }
   } 
   return TRUE;   
}

#endif /* SHELLCFG_USES_RTCS */
/* EOF*/
