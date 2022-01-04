#ifndef __ccp_h__
#define __ccp_h__
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
* $FileName: ccp.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the defines and data structure
*   definitions required by CCP.
*
*END************************************************************************/

/*
** CCP-specific packet types
*/

#define CCP_CODE_RESET_REQ 14      /* Reset Request */
#define CCP_CODE_RESET_ACK 15      /* Reset Ack */


/*
** CCP Configuration Options
*/

#define CCP_CI_PRED1    1     /* Predictor type 1 */
#define CCP_CI_LZS      17    /* Stac LZS */


struct ccp_opt;
struct ppp_opt;
struct ppp_cfg;

/*
** The Compression Protocol Data Union.  This union contains the private
** data required by each supported compression method.
*/

typedef union cp_data {
   uint_32     DUMMY;
} CP_DATA, _PTR_ CP_DATA_PTR;

/*
** The Compression Procotol Call Table structure.  This structure
** contains pointers to functions that compress and decompress packets.
*/

typedef struct cp_call {
   void    (_CODE_PTR_ CP_init)  (CP_DATA_PTR, struct ccp_opt _PTR_);
   PCB_PTR (_CODE_PTR_ CP_comp)  (CP_DATA_PTR, PCB_PTR,
                                  struct ppp_cfg _PTR_, struct ppp_opt _PTR_);
   PCB_PTR (_CODE_PTR_ CP_decomp)(CP_DATA_PTR, PCB_PTR,
                                  struct ppp_cfg _PTR_, struct ppp_opt _PTR_);

   void    (_CODE_PTR_ CP_resetreq) (CP_DATA_PTR, PCB_PTR);
   void    (_CODE_PTR_ CP_resetack) (CP_DATA_PTR, PCB_PTR);
} CP_CALL, _PTR_ CP_CALL_PTR;

/*
** The CCP Options structure.  This structure contains all the
** negotiated options for an open CCP link.
*/

typedef struct ccp_opt {
   CP_CALL_PTR    CP;
   union {
      uint_32     DUMMY;
#ifdef PPP_CP_LZS
      struct {
         uint_16  HIST;
         uchar    CHECK;
      }           LZS;
#endif
   }              DATA;
} CCP_OPT, _PTR_ CCP_OPT_PTR;

/*
** The CCP Options state used during negotiation
*/

typedef struct ccp_neg {
   unsigned NEG_PRED1 : 1;    /* Not used */
   unsigned NEG_LZS   : 1;    /* Not used */
   unsigned           : 0;
#ifdef PPP_CP_LZS
   uint_16  LZS_HIST;
   uchar    LZS_CHECK;
#endif
} CCP_NEG, _PTR_ CCP_NEG_PTR;

/*
** The CCP Configuration structure
*/

typedef struct ccp_cfg {

   /* Statistics counters */
   uint_32  ST_CCP_RX_RESET;
   uint_32  ST_CCP_RX_ACK;

   CCP_OPT  SEND_OPT;
   CCP_OPT  RECV_OPT;
   CCP_NEG  RECV_NEG;
   CP_DATA  SEND_DATA;
   CP_DATA  RECV_DATA;
} CCP_CFG, _PTR_ CCP_CFG_PTR;


/*
** Prototypes
*/

extern uint_32 CCP_init    (_ppp_handle);
extern uint_32 CCP_destroy (_ppp_handle);

#define CCP_lowerup(handle)   PPPFSM_lowerup(&((PPP_CFG_PTR)(handle))->CCP_FSM)
#define CCP_lowerdown(handle) PPPFSM_lowerdown(&((PPP_CFG_PTR)(handle))->CCP_FSM)
#define CCP_open(handle)      PPPFSM_open(&((PPP_CFG_PTR)(handle))->CCP_FSM, 0)
#define CCP_close(handle)     PPPFSM_close(&((PPP_CFG_PTR)(handle))->CCP_FSM)
#define CCP_input             PPPFSM_input

#endif
/* EOF */
