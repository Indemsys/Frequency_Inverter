#ifndef __lcp_h__
#define __lcp_h__
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
* $FileName: lcp.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the defines and data structure
*   definitions required by LCP.
*
*END************************************************************************/

/*
** lcp.h - Link Control Protocol definitions.
**
** Copyright (c) 1989 Carnegie Mellon University.
** All rights reserved.
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation,
** advertising materials, and other materials related to such
** distribution and use acknowledge that the software was developed
** by Carnegie Mellon University.  The name of the
** University may not be used to endorse or promote products derived
** from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
** WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/


/*
** LCP-specific packet types
*/

#define LCP_CODE_PROT_REJ  8       /* Protocol Reject */
#define LCP_CODE_ECHO_REQ  9       /* Echo Request */
#define LCP_CODE_ECHO_REP  10      /* Echo Reply */
#define LCP_CODE_DISC_REQ  11      /* Discard Request */


/*
** LCP Configuration Options
*/

#define LCP_CI_MRU      1     /* Maximum Receive Unit */
#define LCP_CI_ACCM     2     /* Async Control Character Map */
#define LCP_CI_AP       3     /* Authentication Protocol */
#define LCP_CI_QP       4     /* Quality Protocol */
#define LCP_CI_MAGIC    5     /* Magic Number */
#define LCP_CI_PFC      7     /* Protocol Field Compression */
#define LCP_CI_ACFC     8     /* Address and Control Field Compression */
#define LCP_CI_FCS      9     /* Frame Check Sequence Method */


/*
** Prototypes
*/

extern uint_32 LCP_init    (_ppp_handle);
extern uint_32 LCP_destroy (_ppp_handle);

extern void LCP_sendprotrej (PCB_PTR, PPPFSM_CFG_PTR);

#define LCP_input PPPFSM_input

#endif
/* EOF */
