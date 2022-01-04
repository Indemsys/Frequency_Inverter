#ifndef __checksum_h__
#define __checksum_h__
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
* $FileName: checksum.h$
* $Version : 3.8.4.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the IP checksum function prototypes.
*
*END************************************************************************/


/***************************************
**
** Code macros
**
*/

#define IP_Sum_invert(s)   (((s) == 0xFFFF) ? (s) : ~(s) & 0xFFFF)


/***************************************
**
** Prototypes
**
*/

uint_16 IP_Sum_immediate
(
   uint_16,             /* [IN] initial sum           */
   uint_16              /* [IN] number to add to sum  */
);

uint_16 IP_Sum_PCB
(
   uint_16,             /* [IN] initial sum           */
   RTCSPCB_PTR          /* [IN] the PCB               */
);

uint_16 IP_Sum_pseudo
(
   uint_16,             /* [IN] initial sum           */
   RTCSPCB_PTR,         /* [IN] the PCB               */
   int_32               /* [IN] IP layer              */
);

#if RTCSCFG_ENABLE_IP6
    uint_16 IP6_Sum_pseudo ( uint_16, RTCSPCB_PTR );
#endif

#endif
/* EOF */
