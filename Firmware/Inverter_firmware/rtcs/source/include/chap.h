#ifndef __chap_h__
#define __chap_h__
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
* $FileName: chap.h$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the defines and data structure
*   definitions required by CHAP.
*
*END************************************************************************/


/*
** CHAP header = Code, id, length.
*/

#define CHAP_HDR_LEN    4

/*
** The length of generated challenges
*/

#define CHAP_CHALLENGE_LEN    64

/*
** CHAP packet types
*/

#define CHAP_CODE_CHALLENGE   1     /* Challenge */
#define CHAP_CODE_RESPONSE    2     /* Response to challenge */
#define CHAP_CODE_SUCCESS     3     /* Successful response */
#define CHAP_CODE_FAILURE     4     /* Failed response */

/*
** Internal CHAP states
*/

#define CHAP_STATE_CLOSED     0
#define CHAP_STATE_INITIAL    1
#define CHAP_STATE_SUCCESS    CHAP_CODE_SUCCESS
#define CHAP_STATE_FAILURE    CHAP_CODE_FAILURE

/*
** Private state required by the CHAP server
*/

typedef struct chap_data {

   /* Statistics counters */
   uint_32  ST_CHAP_SHORT;    /* packet too short */
   uint_32  ST_CHAP_CODE;     /* invalid code */
   uint_32  ST_CHAP_ID;       /* incorrect id */
   uint_32  ST_CHAP_NOAUTH;   /* challenge received but CHAP not negotiated */
   uint_32  ST_CHAP_NOCHAL;   /* response received but no challenge made */
   uint_32  ST_CHAP_NOPW;     /* no password for peer */

   uchar MD5[64];

   uchar CLIENT_STATE;
   uchar SERVER_STATE;
   uchar CURID;

} CHAP_DATA, _PTR_ CHAP_DATA_PTR;


/*
** Prototypes
*/

extern void CHAP_init      (_ppp_handle);
extern void CHAP_challenge (_ppp_handle);
extern void CHAP_open      (_ppp_handle);

extern void CHAP_input (PCB_PTR, _ppp_handle);


/*
** Available hashing algorithms
*/

extern void PPP_MD5(uchar_ptr, ...);


#endif
/* EOF */
