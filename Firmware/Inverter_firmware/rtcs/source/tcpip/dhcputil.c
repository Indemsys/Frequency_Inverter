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
* $FileName: dhcputil.c$
* $Version : 3.8.4.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the utility functions for use by both the
*   DHCP Client and DHCP Server
*   For more information, refer to:
*   RFC 1541 (DHCP)
*   RPC 1533 (DHCP Options).
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_find_option
* Returned Value  : uchar_ptr
* Comments        : This function searches through the OPTIONS section
*                   of a DHCP message to find a specific option type.
*                   If it is found, a pointer to the specific option
*                   field is returned, If no option of the specified
*                   type exists, NULL is returned.
*
*END*-----------------------------------------------------------------*/

uchar_ptr DHCPCLNT_find_option
   (
      uchar_ptr   msgptr,
      uint_32     msglen,
      uchar       option
   )
{ /* Body */



return( DHCP_find_option((msgptr + sizeof(DHCP_HEADER) + 4),
                         ((msglen - sizeof(DHCP_HEADER)) - 4),
                         option));

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_find_option
* Returned Value  : uchar_ptr
* Comments        : This function searches through the OPTIONS section
*                   of a DHCP message to find a specific option type.
*                   If it is found, a pointer to the specific option
*                   field is returned, If no option of the specified
*                   type exists, NULL is returned.
*
*END*-----------------------------------------------------------------*/

uchar_ptr DHCP_find_option
   (
      uchar_ptr   msgptr,
      uint_32     msglen,
      uchar       option
   )
{ /* Body */
   uchar optype;
   uchar oplen;

   for (;;) {

      /* Get the next option code */
      if (msglen == 0) {
         return NULL;
      } /* Endif */
      optype = ntohc(msgptr);
      msgptr++;
      msglen--;

      if (optype == DHCPOPT_END) {
         return NULL;
      } /* Endif */
      if (optype == DHCPOPT_PAD) {
         continue;
      } /* Endif */

      /* Get the option length */
      if (msglen == 0) {
         return NULL;
      } /* Endif */
      oplen = ntohc(msgptr);
      msgptr++;
      msglen--;

      if (msglen < oplen) {
         return NULL;
      } /* Endif */

      if (optype == option) {
         return msgptr-2;
      } /* Endif */

      msgptr += oplen;
      msglen -= oplen;

   } /* Endfor */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_int8
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out an 8-bit option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_int8
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      uchar             optval
   )
{ /* Body */
   uchar_ptr opt = *optptr;

   if ((*optlen) < 3) return FALSE;

   htonc(opt, opttype); opt++;
   htonc(opt, 1);       opt++;
   htonc(opt, optval);  opt++;

   *optlen -= 3;
   *optptr = opt;
   return TRUE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_int16
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out a 16-bit option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_int16
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      uint_16           optval
   )
{ /* Body */
   uchar_ptr opt = *optptr;

   if ((*optlen) < 4) return FALSE;

   htonc(opt, opttype); opt++;
   htonc(opt, 2);       opt++;
   htons(opt, optval);  opt += 2;

   *optlen -= 4;
   *optptr = opt;
   return TRUE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_int32
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out a 32-bit option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_int32
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      uint_32           optval
   )
{ /* Body */
   uchar_ptr opt = *optptr;

   if ((*optlen) < 6) return FALSE;

   htonc(opt, opttype); opt++;
   htonc(opt, 4);       opt++;
   htonl(opt, optval);  opt += 4;

   *optlen -= 6;
   *optptr = opt;
   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_variable
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out a variable size request option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_variable
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      uchar       _PTR_ options,
      uint_32           num_options
   )
{ /* Body */
   uint_32   i;
   uchar_ptr opt = *optptr;

   if ((*optlen) < (num_options + 2)) return FALSE;

   htonc(opt, opttype); opt++;
   htonc(opt, (uchar)num_options); opt++;

   for ( i = 0; i < num_options; i++ ) {
      htonc(opt, options[i]); opt++;
   } /* Endfor */

   *optlen -= (num_options +  2);
   *optptr = opt;
   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_addr
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out an IP address option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_addr
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      _ip_address       addr
   )
{ /* Body */
   uchar_ptr opt = *optptr;

   if ((*optlen) < 6) return FALSE;

   htonc(opt, opttype); opt++;
   htonc(opt, 4);       opt++;
   htonl(opt, addr);    opt += 4;

   *optlen -= 6;
   *optptr = opt;
   return TRUE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_addrlist
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out an IP address list option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_addrlist
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      _ip_address _PTR_ addrptr,
      uint_32           addrcount
   )
{ /* Body */
   uchar_ptr   opt = *optptr;
   _ip_address addr;

   if ((*optlen) < 4*addrcount+2) return FALSE;

   htonc(opt, opttype);     opt++;
   htonc(opt, 4*addrcount); opt++;
   while (addrcount--) {
      addr = *addrptr++;
      htonl(opt, addr);     opt += 4;
   } /* Endwhile */

   *optlen -= 4*addrcount+2;
   *optptr = opt;
   return TRUE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCP_option_string
* Returned Value  : TRUE if successful, FALSE otherwise
* Comments        : This function writes out a variable-length option
*                   to the optptr buffer.
*
*END*-----------------------------------------------------------------*/

boolean DHCP_option_string
   (
      uchar_ptr   _PTR_ optptr,
      uint_32     _PTR_ optlen,
      uchar             opttype,
      char_ptr          optval
   )
{ /* Body */
   uint_32   slen = 0;
   uchar_ptr opt;

   opt = (uchar_ptr)optval;
   while (*opt++) slen++;

   if ((*optlen) < slen+2) return FALSE;
   opt = *optptr;

   htonc(opt, opttype); opt++;
   htonc(opt, slen);    opt++;
   _mem_copy(optval, opt, slen); opt += slen;

   *optlen -= slen+2;
   *optptr = opt;
   return TRUE;

} /* Endbody */


#endif /* RTCSCFG_ENABLE_IP4 */


/* EOF */
