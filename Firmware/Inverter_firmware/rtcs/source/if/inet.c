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
* $FileName: inet.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains some Internet address manipulation
*   functions.
*
*END************************************************************************/

#include <ctype.h>
#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : inet_aton
* Returned Value  : -1 on success, 0 on error
* Comments  :  Converts a dotted-decimal into a binary in_addr
*
*END*-----------------------------------------------------------------*/

int_32 inet_aton
   (
      const char _PTR_  name,
         /* [IN] dotted decimal IP address */
      in_addr _PTR_     ipaddr_ptr
         /* [OUT] binary IP address */
   )
{ /* Body */
   boolean     ipok = FALSE;
   uint_32     dots;
   uint_32     byte;
   _ip_address addr;

   addr = 0;
   dots = 0;
   for (;;) {

      if (!isdigit(*name)) break;

      byte = 0;
      while (isdigit(*name)) {
         byte *= 10;
         byte += *name - '0';
         if (byte > 255) break;
         name++;
      } /* Endwhile */
      if (byte > 255) break;
      addr <<= 8;
      addr += byte;

      if (*name == '.') {
         dots++;
         if (dots > 3) break;
         name++;
         continue;
      } /* Endif */

      if ((*name == '\0') && (dots == 3)) {
         ipok = TRUE;
      } /* Endif */

      break;

   } /* Endfor */

   if (!ipok) {
      return 0;
   } /* Endif */

   if (ipaddr_ptr) {
      ipaddr_ptr->s_addr = addr;
   } /* Endif */

   return -1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : inet_addr
* Returned Value  : a binary IP address, or -1 on error
* Comments  :  Converts a dotted-decimal into a binary in_addr
*
*END*-----------------------------------------------------------------*/

_ip_address inet_addr
   (
      const char _PTR_  name
         /* [IN] dotted decimal IP address */
   )
{ /* Body */
   in_addr  ipaddr;
   int_32   ipok;

   ipok = inet_aton(name, &ipaddr);
   if (!ipok) {
      return INADDR_BROADCAST;
   } /* Endif */
   return ipaddr.s_addr;

} /* Endbody */


/* EOF */
