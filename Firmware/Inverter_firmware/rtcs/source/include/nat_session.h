#ifndef __nat_session_h__
#define __nat_session_h__
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
* $FileName: nat_session.h$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   Private Network Address Translator definitions.
*
*END************************************************************************/

 


/* 
** NAT_EVENT_STRUCT:
**    Used for timeouts.
**
*/
struct nat_session;

typedef struct nat_event_struct { 
   uint_32                    TIME;    
   struct nat_session_struct _PTR_   SESSION_PTR;
   TCPIP_EVENT_PTR            EVENT;
   struct nat_event_struct _PTR_     NEXT;
   struct nat_event_struct _PTR_     PREV;
} NAT_EVENT_STRUCT, _PTR_ NAT_EVENT_STRUCT_PTR;


/* 
** NAT_SESSION_EXT_STRUCT 
**    Holds ALG specific information attached to a session
*/
typedef struct nat_session_ext_struct {
   struct nat_session_ext_struct _PTR_  NEXT;
   uint_32     ALG_TYPE;
} NAT_SESSION_EXT_STRUCT, _PTR_ NAT_SESSION_EXT_STRUCT_PTR;

/* 
** NAT_SESSION_STRUCT: 
**    Holds information about a sessions. One of these is allocated for 
**    each session.
*/
typedef struct nat_session_struct {
   struct nat_session_struct _PTR_      NEXT_IN;       /* Used for IN tree           */
   struct nat_session_struct _PTR_      NEXT_OUT;      /* Used for OUT tree          */
   _ip_address                   PRV_HST;       /* private host IP            */
   _ip_address                   PUB_HST;       /* public host IP             */
   _ip_address                   NAT_HST;       /* replacement IP             */
   uint_16                       PRV_PORT;      /* private host port          */
   uint_16                       PUB_PORT;      /* public host port           */
   uint_16                       NAT_PORT;      /* replacement port           */
   uint_16                       RESERVED;      /* unused                     */
   uint_32                       IP_PROT;       /* IP protocol                */
   NAT_EVENT_STRUCT              TIMEOUT;       /* to close sessions          */   
   uint_32                       STATE;         /* holds state information    */
   NAT_SESSION_EXT_STRUCT_PTR    ALG_INFO_PTR;  /* ALG specifif information   */
   boolean                       SNAT_OR_DNAT;  /* SNAT or DNAT session       */ 
} NAT_SESSION_STRUCT, _PTR_ NAT_SESSION_STRUCT_PTR;



#endif
