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
* $FileName: md5.c$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the implementation of the MD5
*   message digest algorithm described in RFC 1321.
*
*END************************************************************************/

#include <stdarg.h>
#include <ppp.h>
#include "ppp_prv.h"
#include "md5.h"



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_MD5
* Returned Value  :  void
* Comments        :
*     Computes the MD5 hash of a sequence of bytes.
*
*     The prototype is:
*        PPP_MD5(digest, len1, msg1, len2, msg2, ..., lenk, msgk, 0);
*
* Limitations     :
*     This function returns an incorrect hash if the complete byte
*     sequence exceeds 512MB in length.
*
*END*-----------------------------------------------------------------*/

void PPP_MD5
   (
      uchar_ptr digest,
            /* [OUT] - the MD5 message digest */
      ...
            /* [IN] - (uint_32, uchar_ptr) pairs */
   )
{ /* Body */
   va_list   ap;
   uint_32   block[16];
   uint_32   context[4];
   uint_32   temp;
   uint_32   bcount = 0, wcount = 0;
   uint_32   lenmsg = 0;
   uint_32   lenfrag;
   uchar_ptr msgfrag;

   va_start(ap, digest);

   context[0] = 0x67452301l;
   context[1] = 0xEFCDAB89l;
   context[2] = 0x98BADCFEl;
   context[3] = 0x10325476l;

   PPP_memzero(block, sizeof(block));

      /* Loop for each message fragment */
   for (;;) {

         /* Get the next message fragment */
      lenfrag = va_arg(ap, uint_32);
      if (lenfrag == 0) break;
      lenmsg += lenfrag;
      msgfrag = va_arg(ap, uchar_ptr);

         /* Copy bytes from the fragment to the block */
      while (lenfrag--) {
         temp = *msgfrag++ & 0xFF;
         block[wcount] >>= 8;
         block[wcount] |= temp << 24;
         if (++bcount == 4) {
            bcount = 0;
            if (++wcount == 16) {
               wcount = 0;
               PPP_MD5_block(context, block, mdtab);
            } /* Endif */
         } /* Endif */
      } /* Endwhile */

   } /* Endfor */

      /* Now append 0x80 */
   block[wcount] >>= 8;
   block[wcount] |= 0x80000000l;
   if (++bcount == 4) {
      bcount = 0;
      if (++wcount == 16) {
         wcount = 0;
         PPP_MD5_block(context, block, mdtab);
      } /* Endif */
   } /* Endif */

      /* Pad with 0 until wcount==14 */
   if (bcount > 0) {
      block[wcount++] >>= 8*(4-bcount);
      /* bcount henceforth ignored */
   } /* Endif */
   if (wcount > 14) {
      while (wcount < 16) block[wcount++] = 0;
      PPP_MD5_block(context, block, mdtab);
      wcount = 14;   /* PPP_MD5_task_block zeroes block */
   } /* Endif */
   while (wcount < 14) block[wcount++] = 0;

      /* Finally, append the pre-pad length */
   lenmsg <<= 3;     /* length must be in bits */
   block[14] = lenmsg;
   block[15] = 0;
   PPP_MD5_block(context, block, mdtab);

      /* Write the hash out to digest while destroying context */
   for (wcount = 0; wcount < 4; wcount++) {
      for (bcount = 0; bcount < 4; bcount++) {
         *digest++ = context[wcount] & 0xFF;
         context[wcount] >>= 8;
      } /* Endfor */
   } /* Endfor */

   va_end(ap);

} /* Endbody */


/* EOF */
