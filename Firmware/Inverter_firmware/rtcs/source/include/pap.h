#ifndef __pap_h__
#define __pap_h__
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
* $FileName: pap.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the defines and data structure
*   definitions required by PAP.
*
*END************************************************************************/


/*
** PAP header = Code, id, length.
*/

#define PAP_HDR_LEN   (4)

/*
** PAP packet types
*/

#define PAP_CODE_AUTH_REQ  1     /* Authenticate Request */
#define PAP_CODE_AUTH_ACK  2     /* Authenticate Ack */
#define PAP_CODE_AUTH_NAK  3     /* Authenticate Nak */

/*
** Internal PAP states
*/

#define PAP_STATE_CLOSED   0
#define PAP_STATE_INITIAL  1
#define PAP_STATE_AUTH_ACK PAP_CODE_AUTH_ACK
#define PAP_STATE_AUTH_NAK PAP_CODE_AUTH_NAK

/*
** Private state required by the PAP server
*/

typedef struct pap_data {

   /* Statistics counters */
   uint_32  ST_PAP_SHORT;     /* packet too short */
   uint_32  ST_PAP_CODE;      /* invalid code */
   uint_32  ST_PAP_ID;        /* incorrect id */
   uint_32  ST_PAP_NOAUTH;    /* request received but PAP not negotiated */
   uint_32  ST_PAP_NOREQ;     /* reply received but no request made */

   uchar CLIENT_STATE;
   uchar SERVER_STATE;
   uchar CURID;
} PAP_DATA, _PTR_ PAP_DATA_PTR;


/*
** Prototypes
*/

extern void PAP_init  (_ppp_handle);
extern void PAP_send  (_ppp_handle);
extern void PAP_open  (_ppp_handle);

extern void PAP_input (PCB_PTR, _ppp_handle);


#endif
/* EOF */
