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
* $FileName: md5step.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the "C" implementation of
*   PPP_MD5_task_block.
*
*END************************************************************************/

#include <stdarg.h>
#include <ppp.h>
#include "ppp_prv.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPP_MD5_block
* Returned Value  :  void
* Comments        :
*     Computes an incremental MD5 hash of a 16 word block.
*
*END*-----------------------------------------------------------------*/

#if defined(__DCC__) && defined(__ppc)
asm uint_32 rotl (uint_32 num, uint_32 bits)
{
% reg num; reg bits
   rotlw r3, num, bits
}
#else
#define rotl(num, bits) ((num << bits) | (num >> (32 - bits)))
#endif

void PPP_MD5_block
   (
      uint_32_ptr context,
            /* [IN/OUT] - the incremental hash */

      register uint_32_ptr block,
            /* [IN] - the 16 word block */

      register const uint_32 _PTR_ ctab
            /* [IN] - the sine function */
   )
{ /* Body */

#define F(x,y,z)  ((y&x)|(z&~x))
#define G(x,y,z)  ((x&z)|(y&~z))
#define H(x,y,z)  (x^y^z)
#define I(x,y,z)  (y^(x|~z))

#define P1(f,k,s) t=a+f(b,c,d)+block[k]+*ctab++; a=rotl(t,s)+b
#define P2(f,k,s) t=d+f(a,b,c)+block[k]+*ctab++; d=rotl(t,s)+a
#define P3(f,k,s) t=c+f(d,a,b)+block[k]+*ctab++; c=rotl(t,s)+d
#define P4(f,k,s) t=b+f(c,d,a)+block[k]+*ctab++; b=rotl(t,s)+c

   register uint_32  a = context[0];
   register uint_32  b = context[1];
   register uint_32  c = context[2];
   register uint_32  d = context[3];
   register uint_32  t;

   P1(F,  0,  7); P2(F,  1, 12); P3(F,  2, 17); P4(F,  3, 22);
   P1(F,  4,  7); P2(F,  5, 12); P3(F,  6, 17); P4(F,  7, 22);
   P1(F,  8,  7); P2(F,  9, 12); P3(F, 10, 17); P4(F, 11, 22);
   P1(F, 12,  7); P2(F, 13, 12); P3(F, 14, 17); P4(F, 15, 22);

   P1(G,  1,  5); P2(G,  6,  9); P3(G, 11, 14); P4(G,  0, 20);
   P1(G,  5,  5); P2(G, 10,  9); P3(G, 15, 14); P4(G,  4, 20);
   P1(G,  9,  5); P2(G, 14,  9); P3(G,  3, 14); P4(G,  8, 20);
   P1(G, 13,  5); P2(G,  2,  9); P3(G,  7, 14); P4(G, 12, 20);

   P1(H,  5,  4); P2(H,  8, 11); P3(H, 11, 16); P4(H, 14, 23);
   P1(H,  1,  4); P2(H,  4, 11); P3(H,  7, 16); P4(H, 10, 23);
   P1(H, 13,  4); P2(H,  0, 11); P3(H,  3, 16); P4(H,  6, 23);
   P1(H,  9,  4); P2(H, 12, 11); P3(H, 15, 16); P4(H,  2, 23);

   P1(I,  0,  6); P2(I,  7, 10); P3(I, 14, 15); P4(I,  5, 21);
   P1(I, 12,  6); P2(I,  3, 10); P3(I, 10, 15); P4(I,  1, 21);
   P1(I,  8,  6); P2(I, 15, 10); P3(I,  6, 15); P4(I, 13, 21);
   P1(I,  4,  6); P2(I, 11, 10); P3(I,  2, 15); P4(I,  9, 21);

   context[0] += a;
   context[1] += b;
   context[2] += c;
   context[3] += d;

   PPP_memzero(block, sizeof(uint_32[16]));

} /* Endbody */


/* EOF */
