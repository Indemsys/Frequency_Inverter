
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
* $FileName: sh_rtcs.c$
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
#include "sh_rtcs.h" 


boolean Shell_parse_ip_address( char_ptr arg, _ip_address _PTR_ ipaddr_ptr)
{
   uint_32      num[4] =  { 0 };
   uint_32      i, index = 0;
   uint_32      temp = 0;
   
   if (ipaddr_ptr == NULL) return FALSE;
   
   for (i=0; arg[i] && (i<16) && (index<4); i++)  {
      if (isdigit(arg[i]))  {
         num[index] = num[index]*10 + arg[i]-'0';
      } else if (arg[i] == '.') {
         index++; 
      } else  {
         return FALSE;
      }
   }
   
   if ((arg[i] == '\0') && (index==3))  {
      for (i=0;i<4;i++)  {
         if (num[i] > 255)  {
            return FALSE;
         } else  {
            temp = (temp << 8) | num[i];
         }
      } 
   } 

   *ipaddr_ptr = temp;
   return TRUE;
}

boolean Shell_parse_netmask( char_ptr arg, _ip_address _PTR_ ipaddr_ptr)
{
   uint_32  i, mask;
   boolean  ones = FALSE;
   
   if ( ipaddr_ptr == NULL) return FALSE;
   
   if (!Shell_parse_ip_address(arg, ipaddr_ptr))  return FALSE;

   
   for (i=0;i<32;i++)  {
      mask = *ipaddr_ptr & (1<<i); 
      if (!ones)  {
         if (mask)  {
            ones = TRUE;
         }
      } else {
         if  (! mask)  {
            return FALSE;   
         }  
      } 
   }   
   return TRUE;
}

#endif /* SHELLCFG_USES_RTCS */
