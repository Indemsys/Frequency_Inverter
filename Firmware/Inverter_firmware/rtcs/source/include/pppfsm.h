#ifndef __fsm_h__
#define __fsm_h__
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
* $FileName: pppfsm.h$
* $Version : 3.5.5.0$
* $Date    : Dec-8-2009$
*
* Comments:
*
*   This file contains the defines and data structure
*   definitions required by the PPP Finite State Machine.
*
*END************************************************************************/

/*
** fsm.h - {Link, IP} Control Protocol Finite State Machine definitions.
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
** Packet header = Code, id, length.
*/

#define CP_HDR_LEN   4


/*
**  CP (LCP, IPCP, etc.) codes.
*/

#define CP_CODE_CONF_REQ  1     /* Configuration Request */
#define CP_CODE_CONF_ACK  2     /* Configuration Ack */
#define CP_CODE_CONF_NAK  3     /* Configuration Nak */
#define CP_CODE_CONF_REJ  4     /* Configuration Reject */
#define CP_CODE_TERM_REQ  5     /* Termination Request */
#define CP_CODE_TERM_ACK  6     /* Termination Ack */
#define CP_CODE_CODE_REJ  7     /* Code Reject */


/*
** The FSM call table.  Contains a list a functions to call when various
** events occur in the FSM.
*/
struct pppfsm_cfg;

typedef struct pppfsm_call {
   uint_16             PROTOCOL;       /* Protocol number */
   boolean (_CODE_PTR_ linkup)  (struct pppfsm_cfg _PTR_); /* Link open */
   void    (_CODE_PTR_ linkdown)(struct pppfsm_cfg _PTR_); /* Link no longer open */
   void    (_CODE_PTR_ resetreq)(struct pppfsm_cfg _PTR_); /* Reset configuration options */
   uint_32 (_CODE_PTR_ buildreq)(struct pppfsm_cfg _PTR_, uchar_ptr, uint_32); /* Build ConfReq */
   uint_32 (_CODE_PTR_ recvreq) (struct pppfsm_cfg _PTR_, boolean); /* Convert ConfReq to ConfAck/Nak/Rej */
   boolean (_CODE_PTR_ recvack) (struct pppfsm_cfg _PTR_); /* TRUE if ConfAck is PPP_OK */
   boolean (_CODE_PTR_ recvnak) (struct pppfsm_cfg _PTR_); /* TRUE if ConfNak is PPP_OK */
   boolean (_CODE_PTR_ recvrej) (struct pppfsm_cfg _PTR_); /* TRUE if ConfRej is PPP_OK */
   boolean (_CODE_PTR_ testcode)(struct pppfsm_cfg _PTR_); /* TRUE if CodeRej is catastrophic */
   boolean (_CODE_PTR_ recvcode)(struct pppfsm_cfg _PTR_); /* TRUE if code is PPP_OK */
} PPPFSM_CALL, _PTR_ PPPFSM_CALL_PTR;

/*
** The FSM Configuration structure.
*/

typedef struct pppfsm_cfg {

   /* Statistics counters */
   uint_32           ST_CP_SHORT;
   uint_32           ST_CP_DOWN;
   uint_32           ST_CP_NOBUFS;
   uint_32           ST_CP_BAD_ACK;
   uint_32           ST_CP_BAD_NAK;
   uint_32           ST_CP_BAD_REJ;

   _ppp_handle       HANDLE;     /* the PPP state structure */
   PPPFSM_CALL_PTR   CALL;       /* The protocol-specific call table */
   pointer           PRIVATE;    /* Parameter for protocol-specific functions */
   uint_32           STATE;      /* Current state */
   _ppp_mutex        MUTEX;      /* Mutual exclusion for the STATE field */

   uchar             CURID;      /* ID of the next ConfReq */
   uchar             REQID;      /* ID of the last ConfReq */
   uint_32           NAKS;       /* Number of consecutive NAKs */

   PCB_PTR           PACKET;     /* Current packet */
   uchar             CODE;       /* Temporary location for the code, */
   uchar             ID;         /*    id and length fields for the  */
   uint_16           LENGTH;     /*    current packet                */
   uchar_ptr         DATA;       /* Current position in the current packet */

   uint_32           OPTIONS;    /* Options passed to PPPFSM_open() */

} PPPFSM_CFG, _PTR_ PPPFSM_CFG_PTR;


/*
** Prototypes
*/

extern uint_32 PPPFSM_init      (PPPFSM_CFG_PTR, _ppp_handle, PPPFSM_CALL_PTR, pointer);
extern uint_32 PPPFSM_destroy   (PPPFSM_CFG_PTR);
extern void    PPPFSM_open      (PPPFSM_CFG_PTR, uint_32);
extern void    PPPFSM_close     (PPPFSM_CFG_PTR);
extern void    PPPFSM_lowerup   (PPPFSM_CFG_PTR);
extern void    PPPFSM_lowerdown (PPPFSM_CFG_PTR);

extern void    PPPFSM_input (PCB_PTR, pointer);

#endif
/* EOF */
