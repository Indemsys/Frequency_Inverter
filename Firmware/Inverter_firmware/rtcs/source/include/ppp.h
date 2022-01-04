#ifndef __ppp_h__
#define __ppp_h__
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
* $FileName: ppp.h$
* $Version : 3.8.18.0$
* $Date    : Sep-21-2012$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by application
*   programs in order to use the PPP Library.
*
*END************************************************************************/

#include <rtcs.h>
#include <rtcsrtos.h>
#include <rtcs_in.h>
#include <fio.h>
#include <ppphdlc.h>
#include <io_dun.h>



#ifndef PPP_SECRETS_NOT_SHARED
    #define PPP_SECRETS_NOT_SHARED 0
#endif

/***************************************
**
** Constants
*/


/*
** The default and minimum Maximum Receive Unit.
*/

#define DEFAULT_MRU  1500
#define MINIMUM_MRU    68

/*
** PPP error codes - backward compatibility
*/

   /* Initialization Failures */
#define  PPPERR_ALLOC_FAILED              RTCSERR_PPP_ALLOC_FAILED
#define  PPPERR_INIT_MUTEX_FAILED         RTCSERR_PPP_INIT_MUTEX_FAILED
#define  PPPERR_CREATE_CTRL_POOL_FAILED   RTCSERR_PPP_CREATE_CTRL_POOL_FAILED
#define  PPPERR_CREATE_PKT_POOL_FAILED    RTCSERR_PPP_CREATE_PKT_POOL_FAILED
#define  PPPERR_CREATE_RX_FAILED          RTCSERR_PPP_CREATE_RX_FAILED
#define  PPPERR_CREATE_TX_FAILED          RTCSERR_PPP_CREATE_TX_FAILED
#define  PPPERR_INVALID_CALLBACK          RTCSERR_PPP_INVALID_CALLBACK

   /* Send Failures */
#define  PPPERR_INVALID_HANDLE            RTCSERR_PPP_INVALID_HANDLE
#define  PPPERR_LINK_NOT_OPEN             RTCSERR_PPP_LINK_NOT_OPEN
#define  PPPERR_PACKET_TOO_SHORT          RTCSERR_PPP_PACKET_TOO_SHORT
#define  PPPERR_PACKET_TOO_LONG           RTCSERR_PPP_PACKET_TOO_LONG
#define  PPPERR_OUT_OF_BUFFERS            RTCSERR_PPP_OUT_OF_BUFFERS
#define  PPPERR_INVALID_PROTOCOL          RTCSERR_PPP_INVALID_PROTOCOL
#define  PPPERR_ALLOC_CALL_FAILED         RTCSERR_PPP_ALLOC_CALL_FAILED
#define  PPPERR_PROT_NOT_FOUND            RTCSERR_PPP_PROT_NOT_FOUND

   /* Other Failures (interface level) */
#define  PPPERR_FSM_ACTIVE                RTCSERR_PPP_FSM_ACTIVE

   /* Internal Failures */
#define  PPPERR_OPEN_QUEUE_FAILED         RTCSERR_PPP_OPEN_QUEUE_FAILED

/*
** LCP States.  Do not modify these constants!
*/

#define PPP_STATE_INITIAL  0
#define PPP_STATE_STARTING 1
#define PPP_STATE_CLOSED   2
#define PPP_STATE_STOPPED  3
#define PPP_STATE_CLOSING  4
#define PPP_STATE_STOPPING 5
#define PPP_STATE_REQ_SENT 6
#define PPP_STATE_ACK_RCVD 7
#define PPP_STATE_ACK_SENT 8
#define PPP_STATE_OPENED   9

/*
** LCP Options.  These control the LCP negotiation.
*/

#define PPP_OPT_SILENT  0x01
#define PPP_OPT_RESTART 0x02


/*
** PPP callback function numbers.
*/

#define PPP_CALL_LINK_UP      0
#define PPP_CALL_LINK_DOWN    1
#define PPP_CALL_ECHO_REPLY   2
#define PPP_CALL_AUTH_FAIL    3

#define PPP_CALL_MAX          4


/* Start CR 2207 */
#define PPP_PAP_LSECRET       1
#define PPP_PAP_RSECRETS      2
#define PPP_CHAP_LSECRETS     3
#define PPP_CHAP_RSECRETS     4
/* End CR 2207 */
/***************************************
**
** Global parameters
*/

extern uint_32 _PPPTASK_stacksize;
extern uint_32 _PPPTASK_priority;

extern uint_32 _PPP_MIN_XMIT_TIMEOUT;
extern uint_32 _PPP_MAX_XMIT_TIMEOUT;
extern uint_32 _PPP_MAX_CONF_RETRIES;
extern uint_32 _PPP_MAX_TERM_RETRIES;
extern uint_32 _PPP_MAX_CONF_NAKS;

extern uint_32 _PPP_ACCM;


/***************************************
**
** Type definitions
*/

/*
** The PPP handle.  This is returned by PPP_initialize(), and required
** as a parameter for all other PPP functions.
*/

typedef pointer _ppp_handle;

/*
** A secret.  This is the name/secret pair used by both PAP and CHAP.
*/

typedef struct ppp_secret {
   uint_16     PPP_ID_LENGTH;
   uint_16     PPP_PW_LENGTH;
   char_ptr    PPP_ID_PTR;
   char_ptr    PPP_PW_PTR;
} PPP_SECRET, _PTR_ PPP_SECRET_PTR;

/* Start CR 2207 */
#if PPP_SECRETS_NOT_SHARED
#define PPP_SECRET(p,s) p->s
#else
#define PPP_SECRET(p,s) s
/* End CR 2207 */

/* The secret to use if the peer requests PAP authentication */
extern PPP_SECRET_PTR   _PPP_PAP_LSECRET;

/* The array of secrets to use to authenticate the peer with PAP */
extern PPP_SECRET_PTR   _PPP_PAP_RSECRETS;

/* The name of the local host (required for CHAP) */
extern char_ptr   _PPP_CHAP_LNAME;

/* The array of secrets to use if the peer requests CHAP authentication */
extern PPP_SECRET_PTR   _PPP_CHAP_LSECRETS;

/* The array of secrets to use to authenticate the peer with CHAP */
extern PPP_SECRET_PTR   _PPP_CHAP_RSECRETS;
/* Start CR 2207 */
#endif
/* End CR 2207 */

/*
** Each PPP channel requires two tasks.  The Tx task is a mindless drone
** that sends out everything we tell it to.  The Rx task contains the
** LCP automaton.
*/

extern void PPP_rx_task (pointer, pointer);
extern void PPP_tx_task (pointer, pointer);


/***************************************
**
** The PPP interface functions
*/

extern uint_32 PPP_initialize (_iopcb_handle, _ppp_handle _PTR_);
extern uint_32 PPP_shutdown (_ppp_handle);

extern uint_32 PPP_getcall (_ppp_handle, uint_32, void (_CODE_PTR_ _PTR_)(), pointer _PTR_);
extern uint_32 PPP_setcall (_ppp_handle, uint_32, void (_CODE_PTR_ _PTR_)(), pointer _PTR_);

extern uint_32 PPP_register   (_ppp_handle, uint_16, void (_CODE_PTR_)(PCB_PTR, pointer), pointer);
extern uint_32 PPP_unregister (_ppp_handle, uint_16);

extern void    PPP_lowerup   (_ppp_handle);
extern void    PPP_lowerdown (_ppp_handle);
extern uint_32 PPP_open      (_ppp_handle, uint_32);
extern uint_32 PPP_close     (_ppp_handle);

extern uint_32 PPP_send (_ppp_handle, uint_16, PCB_PTR);

extern uint_32 PPP_discard (_ppp_handle, uchar, PCB_PTR);
extern uint_32 PPP_echo    (_ppp_handle, uchar, PCB_PTR);

extern uint_32 PPP_getmtu (_ppp_handle);


/*
** PPP 2.73 compatibility
*/

typedef uint_32 _ppp_channel;

typedef struct ppp_call { 
   uint_32  PARAM;
   void     (_CODE_PTR_ PPP_Link_up)    (uint_32);
   void     (_CODE_PTR_ PPP_Link_down)  (uint_32);
   void     (_CODE_PTR_ PPP_Echo_reply) (uint_32, uchar, PCB_PTR);
   void     (_CODE_PTR_ PPP_Auth_fail)  (uint_32);
} PPP_CALL, _PTR_ PPP_CALL_PTR;

extern PPP_CALL PPP_DEFAULT_CALL;

#define PPP_TASK_NUMBERS   (0x40)
#define PPP_RX_TASK        PPP_TASK_NUMBERS
#define PPP_TX_TASK        PPP_TASK_NUMBERS+1
#define PPP_Rx_task        PPP_rx_task
#define PPP_Tx_task        PPP_tx_task

extern int_32 PPP_Initialize(_ppp_channel, MQX_FILE_PTR, PPP_CALL_PTR);
extern int_32 PPP_Setcall(_ppp_channel, uint_32, void (_CODE_PTR_)(), uint_32);

/* Start CR 2207 */
extern boolean PPP_set_chap_lname( _ppp_handle, char_ptr ); 
extern boolean PPP_set_secrets(_ppp_handle,uint_32,PPP_SECRET_PTR);
/* End CR 2207 */

/*
** Compatibility for PPP_Register, PPP_Unregister, PPP_Send,
** PPP_Discard and PPP_Echo is not provided.
*/


#endif
/* EOF */
