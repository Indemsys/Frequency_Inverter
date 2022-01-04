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
* $FileName: gen16.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file generates the lookup table for the CCITT-16
*   FCS.
*
*END************************************************************************/
#include <stdio.h>
#define SIZE 256
#define POLY 0x8408

#if RCTSCFG_GENERATE_HASH_TABLES && RTCSCFG_ENABLE_IP4
/* Start CR 1679 */
int RTCS_PPP_SUPPORT_gen16 (void)
/* End CR 1679 */
{ /* Body */
   int count = 0, i;
   register int carry;
   unsigned short crc;

   printf("/*\n** The CCITT-16 Lookup Table\n*/\n\n"
          "static uint_16 fcstab[] = {");

   for (;;) {
      if (!(count%8)) printf("\n  ");
      crc = count;
      for (i=8;i--;) {
         carry = crc & 1;
         crc >>= 1;
         if (carry) crc ^= POLY;
      } /* Endfor */
      printf(" 0x%04X", crc);
      if (++count == SIZE) break;
      printf(",");
   } /* Endfor */
   printf("\n};\n");

   return 0;
} /* Endbody */
#endif
/* EOF */
