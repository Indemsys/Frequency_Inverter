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
* $FileName: genmd5.c$
* $Version : 3.8.5.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file generates the lookup table for the MD5 hash.
*
*END************************************************************************/

#include <stdio.h>
#include <math.h>
#define SIZE 64

#if RCTSCFG_GENERATE_HASH_TABLES && RTCSCFG_ENABLE_IP4
/* Start CR 1679 */
int RTCS_PPP_SUPPORT_genmd5 (void)
/* End CR 1679 */
{ /* Body */
   int count = 0;

   printf("/*\n** The MD5 Lookup Table\n*/\n\n"
          "static uint_32 mdtab[] = {");

   for (;;) {
      if (!(count%4)) printf("\n  ");
      printf(" 0x%08lXl", (long)ldexp(fabs(sin(++count)),32));
      if (count == SIZE) break;
      printf(",");
   } /* Endfor */
   printf("\n};\n");

   return 0;
} /* Endbody */
#endif

/* EOF */
