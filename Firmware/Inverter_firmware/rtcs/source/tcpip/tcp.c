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
* $FileName: tcp.c$
* $Version : 3.8.17.0$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   For more details, refer to RFC793 (which contains some errors)
*   and RFC1122 (which documents those errors and adds lots more
*   useful details).
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "ticker.h"     /* for timer definitions */
#include "tcpip.h"      /* for TCPIP definitions */
#include "ip_prv.h"     /* for IP internal definitions */
#include "tcp_prv.h"    /* for TCP internal definitions */
#include "socket.h"     /* used by TCP_Process_getname */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Init
* Returned Values : int_32
* Comments        :
*
*  Initialize the TCP layer.
*  Returns 0 or positive error number
*
*END*-----------------------------------------------------------------*/

uint_32 TCP_Init
   (
      void
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;

    uint_32              status;

   tcp_cfg = RTCS_mem_alloc_zero(sizeof(TCP_CFG_STRUCT));
   if (tcp_cfg == NULL)  {
      return RTCSERR_OUT_OF_MEMORY;
   }

   _mem_set_type(tcp_cfg, MEM_TYPE_TCP_CFG_STRUCT);

   RTCS_setcfg(TCP,tcp_cfg);

   tcp_cfg->qh           = NULL;
   tcp_cfg->qhead        = &tcp_cfg->qh;
   tcp_cfg->lasttime     = 0;
   tcp_cfg->TCBhead      = NULL;
   tcp_cfg->Rchunkhead   = NULL;
   tcp_cfg->SbufNodehead = NULL;
   tcp_cfg->Rchunkfree   = NULL;
   tcp_cfg->SbufNodefree = NULL;
   tcp_cfg->next_port    = IPPORT_USERRESERVED;
   tcp_cfg->acktq        = NULL;
   tcp_cfg->xmittq       = NULL;
   tcp_cfg->sndtq        = NULL;
   tcp_cfg->rcvtq        = NULL;

   tcp_cfg->DEFER_TAIL   = &tcp_cfg->DEFER_HEAD;

   tcp_cfg->CONN_COUNT   = 0;
   tcp_cfg->HALF_OPEN_TCB_COUNT = 0;

    /*
    ** Global used for TCP_RTO_MIN because we may want to let our
    ** users adjust this.
    */
     _TCP_rto_min = TCP_RTO_MIN;

    /*
    ** Install the TCP tick server
    */
    TCP_tick = TCP_Tick_server;

#if RTCSCFG_ENABLE_IP4
    IP_open(IPPROTO_TCP, TCP4_Service_packet, tcp_cfg, &status);
    if(status != RTCS_OK)
    {
        return status;
    }
#endif /* RTCSCFG_ENABLE_IP4 */


#if RTCSCFG_ENABLE_IP6
    IP6_open(IPPROTO_TCP, TCP6_Service_packet, tcp_cfg, &status);
#endif /* RTCSCFG_ENABLE_IP6 */


    return status;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Event
* Returned Values : None.
* Comments        :
*
*  Signal the occurrence of an event
*   (of the type which the user (control/wait task) may wait upon).
*
*  This is used for non-error events; error/terminating events should
*   call TCP_Close_TCB(), closing the connection.
*
*END*-----------------------------------------------------------------*/

void TCP_Event
   (
      TCB_STRUCT_PTR tcb,     /* IN/OUT - TCP context */
      int_16         event    /* IN     - event that occurred */
   )
{ /* Body */
    RTCSLOG_FNE3(RTCSLOG_FN_TCP_Event, tcb, event);

   tcb->status |= event;
   switch ( event ) {

      case TCPS_FINRECV:
      case TCPS_FINACKED:
         if ( (~tcb->status & (TCPS_FINRECV|TCPS_FINACKED)) == 0 ) {
            tcb->status |= TCPS_FINDONE;
            event |= TCPS_FINDONE;      /* for Signal()'s */
         } /* Endif */
         break;

      default:
         /* Do nothing */
         break;

   } /* End Switch */

    RTCSLOG_FNX1(RTCSLOG_FN_TCP_Event);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_create
* Returned Values : None.
* Comments        :
*
*  Called when the application creates a SOCK_STREAM socket.
*
*  This function allocates a TCB.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_create
   (
      TCP_PARM_PTR   req_ptr     /* IN/OUT - the open request */
   )
{ /* Body */
   TCB_STRUCT_PTR tcb;     /* New TCB */

   tcb = (TCB_STRUCT_PTR)RTCS_mem_alloc_zero(sizeof(TCB_STRUCT));

   if (tcb == NULL) {
      RTCS_log_error(ERROR_TCP, RTCSERR_TCP_OPEN_FAILED, 0, 0, 0);
      RTCSCMD_complete(req_ptr, RTCSERR_OUT_OF_MEMORY);
      return;
   } /* Endif */

   _mem_set_type(tcb, MEM_TYPE_TCB);

   tcb->VALID = TCB_VALID_ID;
   tcb->SOCKET = req_ptr->SOCKET;

   /* Local and remote ports */
   tcb->local_port = 0;
   tcb->local_host = INADDR_ANY;
   tcb->remote_port= 0;
   tcb->remote_host = INADDR_ANY;

#if RTCSCFG_ENABLE_IP6
   IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(tcb->local_ipv6_host));
   IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(tcb->remote_ipv6_host));
#endif

   req_ptr->TCB_PTR = tcb;
   RTCSCMD_complete(req_ptr, RTCS_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_bind46
* Returned Values : None.
* Comments        :
*
*  Called when the application binds a SOCK_STREAM socket.
*
*  This function chooses a local port for a TCB.
*
*END*-----------------------------------------------------------------*/


void TCP_Process_bind
    (
        TCP_PARM_PTR   req_ptr     /* IN/OUT - the bind request */
    )
{ /* Body */
    TCP_CFG_STRUCT_PTR   tcp_cfg;          /* TCP Layer constants */
    TCB_STRUCT_PTR       tcb,
                         tcb2;             /* Temporary pointer */


    boolean             local_host_zero =0;
    boolean             local_host_eq=0;
    boolean             local_port_eq=0;
    boolean             remote_port_zero=0;
    boolean             local_result =1;

    tcp_cfg = RTCS_getcfg(TCP);
    tcb = req_ptr->TCB_PTR;

    if ( tcb == NULL )
    {
        RTCSCMD_complete(req_ptr, RTCSERR_TCP_CONN_RLSD);
        return;
    }


    /* setup tcb localhost address */
#if RTCSCFG_ENABLE_IP6
    if(req_ptr->AF == AF_INET)
    {
#endif

        tcb->local_host = req_ptr->IP_ADDR;

#if RTCSCFG_ENABLE_IP6
    }
    else if(req_ptr->AF == AF_INET6)
    {
        IN6_ADDR_COPY(&(req_ptr->IPV6_ADDR),&(tcb->local_ipv6_host));
        /* add binded scope_id here */
        tcb->if_scope_id = req_ptr->IF_SCOPE_ID;
    }
    else
    {
        RTCS_log_error(ERROR_TCP, RTCSERR_TCPIP_INVALID_ARGUMENT, 0, 0, 0);
    }
#endif

    /* setup tcb localhost port */
    tcb->local_port = req_ptr->TCP_PORT;

   /*
   ** Scan for available port if none specified
   */
   if ( tcb->local_port == 0 ) {
      uint_16  first_port = tcp_cfg->next_port;

        do {

            tcb->local_port = tcp_cfg->next_port;
            if (--tcp_cfg->next_port < IPPORT_RESERVED)
            {
                tcp_cfg->next_port = IPPORT_USERRESERVED;
            } /* Endif */

            if (tcp_cfg->next_port == first_port)
            {
                RTCS_log_error(ERROR_TCP, RTCSERR_TCP_NO_MORE_PORTS, 0, 0, 0);
                RTCSCMD_complete(req_ptr, RTCSERR_TCP_NO_MORE_PORTS);
                return;
            } /* Endif */

            /* is port used? */
#if RTCSCFG_ENABLE_IP6
            if(req_ptr->AF == AF_INET)
            {
#endif
                for (
                        tcb2=tcp_cfg->TCBhead;
                  (tcb2 != NULL) &&((tcb2->local_host &&(tcb2->local_host != tcb->local_host)) || (tcb2->local_port != tcb->local_port)||(tcb2->af != req_ptr->AF ));
                        tcb2=tcb2->next
                    )
                {
                } /* Endfor */

#if RTCSCFG_ENABLE_IP6
            }
            else if(req_ptr->AF == AF_INET6)
            {
                for (
                        tcb2=tcp_cfg->TCBhead;

                        (tcb2 != NULL) &&
                        (
                            (

                                (!IN6_ARE_ADDR_EQUAL(&(tcb2->local_ipv6_host),            &in6addr_any )) &&
                                (!IN6_ARE_ADDR_EQUAL(&(tcb2->local_ipv6_host), &(tcb->local_ipv6_host) ))
                            )||
                            (tcb2->local_port != tcb->local_port)||
                            (tcb2->af != req_ptr->AF )||
                            (tcb2->if_scope_id != req_ptr->IF_SCOPE_ID)
                        );
                        tcb2=tcb2->next
                   )
                {
                } /* Endfor */
            }
#endif

      } while (tcb2 != NULL);

      /*
      ** assuming no more than (IPPORT_USERRESERVED -
      **  IPPORT_RESERVED) ports will ever be opened at once,
      **  this is currently 5000-1024 = 3976 ports    (+1)
      */

    }
    else
    {

      /* is requested port used? */
#if RTCSCFG_ENABLE_IP6
        if(req_ptr->AF == AF_INET)
        {
#endif
            for(
                    tcb2=tcp_cfg->TCBhead;
                    tcb2 != NULL &&
                    (
                        (
                            tcb2->local_host &&
                            tcb2->local_host != tcb->local_host
                        )                                       ||
                        (tcb2->local_port != tcb->local_port)   ||
                        (tcb2->remote_port != 0)                ||
                        (tcb2->af != req_ptr->AF )
                    );
                    tcb2=tcb2->next
               )
            {
            } /* Endfor */
#if RTCSCFG_ENABLE_IP6
        }
        else if(req_ptr->AF == AF_INET6)
        {


            for(
                    tcb2=tcp_cfg->TCBhead;
                    (tcb2 != NULL) &&
                    (
                        (
                            (!IN6_ARE_ADDR_EQUAL(&(tcb2->local_ipv6_host),            &in6addr_any ))&&
                            (!IN6_ARE_ADDR_EQUAL(&(tcb2->local_ipv6_host), &(tcb->local_ipv6_host) ))
                        ) ||
                        (tcb2->local_port != tcb->local_port) ||
                        (tcb2->remote_port != 0)||
                        (tcb2->af != req_ptr->AF )||
                        (tcb2->if_scope_id != req_ptr->IF_SCOPE_ID)
                    );
                    tcb2=tcb2->next
               )
            {
            }

        }
#endif


      if (tcb2 != NULL) {
         RTCS_log_error(ERROR_TCP, RTCSERR_TCP_ADDR_IN_USE, 0, 0, 0);
         RTCSCMD_complete(req_ptr, RTCSERR_TCP_ADDR_IN_USE);
         return;
      } /* Endif */

   } /* Endif */

   tcb->state = BOUND;
   /*BIND table will provide bind family information */
   tcb->af = req_ptr->AF;
   tcb->next = tcp_cfg->TCBhead;
   tcp_cfg->TCBhead = tcb;

   RTCSCMD_complete(req_ptr, RTCS_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_open
* Returned Values : None.
* Comments        :
*
*  Process TcpOpen() requests, within TCP server. Support IPv4 and IPv6
*
*  On successful creation of the TCB and associated buffers:
*
*     if this is an active open, the TCB state is set to SYN_SENT;
*     if this is a passive open, the TCB state is set to LISTEN.
*
*END*-----------------------------------------------------------------*/
void TCP_Process_open
   (
      TCP_PARM_PTR   req_ptr     /* IN/OUT - the open request */
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;          /* TCP Layer constants */
   TCP_OPTIONS_PTR      coptions;         /* list of TCP options */
   TCB_STRUCT_PTR       tcb,              /* Newly allocated TCB */
                        tcb2;             /* Temporary pointer */
   uint_32              sbsize,           /* Send buffer size (bytes) */
                        rbsize;           /* Receive buffer size (bytes) */
   _ip_address          remote_host_v4;   /* remote IPv4 address */
   #if RTCSCFG_ENABLE_IP6
   in6_addr             remote_host_v6;   /* remote IPv6 address */
   #endif
   uint_32              result;           /* Returned status */

   tcp_cfg = RTCS_getcfg(TCP);
   tcb = req_ptr->TCB_PTR;

   if ( tcb == NULL ) {
      TCP_PO_FAIL (RTCSERR_TCP_CONN_RLSD);
   }

   tcp_cfg->CONN_COUNT++;

   /*
   ** Process TCP options
   */
   coptions = req_ptr->OPTIONS_PTR;

   tcb->sndrtomax = 2 * TCP_MSL;

   /*
   ** determine if the bypass of checksums are set
   */
   tcb->bypass_rx_chksum = _TCP_bypass_rx;
   tcb->bypass_tx_chksum = _TCP_bypass_tx;

   /* tcpsecure advice in draft-ietf-tcpm-tcpsecure-00.txt */
    tcb->tcpsecuredraft0 = coptions->tcpsecuredraft0;

    /* Send ring buffer size */
   if ( coptions->tbsize >= 0 ) {
      sbsize = (uint_32)coptions->tbsize;
   } else {
      sbsize = TCP_DEFAULT_SBSIZE;
   } /* Endif */

   /* Receive ring buffer size */
   if ( coptions->rbsize >= 0 ) {
      rbsize = (uint_32)coptions->rbsize;
   } else {
      rbsize = TCP_DEFAULT_RBSIZE;
   } /* Endif */

   /* User timeout (R1) */
   if ( coptions->utimeout >= 0 ) {
      tcb->sndto_1 = coptions->utimeout;
   } else {
      tcb->sndto_1 = 0;
   } /* Endif */

   /* Connection timeout (R2) */
   if ( coptions->timeout > 0 ) {
      tcb->sndto_2 = coptions->timeout;
   } else {
      tcb->sndto_2 = 2 * tcb->sndrtomax;
   } /* Endif */

   /* Initial retransmission timeout */
   if ( coptions->rto > 0 ) {
      tcb->sndrto = coptions->rto;
   } else {
      tcb->sndrto = TCP_INITIAL_RTO_DEFAULT;
   } /* Endif */

   /* Maximum retransmission timeout */
   if ( coptions->maxrto > 0 ) {
      tcb->sndrtomax = coptions->maxrto;
   } /* Endif */

   /* Maximum receive window size */
   if ( coptions->maxrcvwnd > 0 ) {
      tcb->rcvwndmax = coptions->maxrcvwnd;
   } else {
      tcb->rcvwndmax = 0xffff;
   } /* Endif */

   /* INTERNET type of service */
   if ( coptions->tos > 0 ) {
      tcb->tos = coptions->tos & 7;
   } else {
      tcb->tos = 0;
   } /* Endif */

   /* INTERNET precedence */
   if ( coptions->precede != 0xFF ) {
      tcb->tos &= 0xE0;
      tcb->tos |= coptions->precede << 5;
   } /* Endif */

   /* Remote host and port */
#if RTCSCFG_ENABLE_IP4
   if(req_ptr->AF == AF_INET)
   {
        remote_host_v4 = coptions->remhost;
   }
   #if RTCSCFG_ENABLE_IP6
   else
   #endif
#endif
#if RTCSCFG_ENABLE_IP6
   if(req_ptr->AF == AF_INET6)
   {
        IN6_ADDR_COPY(&(coptions->remhost_ipv6),&(remote_host_v6));
   }
#endif /* RTCSCFG_ENABLE_IP6 */

   tcb->remote_port = coptions->remport;
   /* Keepalive timeout */
   if ( coptions->keepalive > 0 ) {
      tcb->keepaliveto = coptions->keepalive * 60000L;
   } else {
      tcb->keepaliveto = 0;
   } /* Endif */

   /* TIME_WAIT timeout */
   if ( coptions->timewaitto ) {
      tcb->timewaitto = coptions->timewaitto;
   } else {
      tcb->timewaitto = TCP_WAITTIMEOUT;
   } /* Endif */

   /* Active or passive open? */
   if ( coptions->active ) {
      tcb->options |= TCPO_ACTIVE;
   } else {
      tcb->options &= ~TCPO_ACTIVE;
   } /* Endif */

   /* Non-blocking or blocking commands */
   if ( coptions->nowait ) {
      tcb->options |= TCPO_NOWAIT;
   } else {
      tcb->options &= ~TCPO_NOWAIT;
   } /* Endif */

   /* Use Nagle algorithm? */
   if ( coptions->nonagle ) {
      tcb->options |= TCPO_NONAGLE;
   } else {
      tcb->options &= ~TCPO_NONAGLE;
   } /* Endif */

   /* Allow switch from ring buffer to user buffer? */
   if ( coptions->noswrbuf ) {
      tcb->options |= TCPO_NOSWITCH;
   } else {
      tcb->options &=~TCPO_NOSWITCH;
   } /* Endif */

   /* delay ack timeout */
   tcb->delayackto = coptions->delayackto;

   /* Process option results */

    if ( tcb->options & TCPO_ACTIVE )
    {
#if RTCSCFG_ENABLE_IP4
        if(req_ptr->AF == AF_INET)
        {
            if ( remote_host_v4 == INADDR_ANY || tcb->remote_port == 0 )
            {
                TCP_PO_FAIL(RTCSERR_TCPIP_DESTADDR_REQUIRED);
            } /* Endif */
            if ( IN_MULTICAST(remote_host_v4) )
            {
                TCP_PO_FAIL(RTCSERR_TCP_ADDR_NA);
            } /* Endif */
        }
    #if RTCSCFG_ENABLE_IP6
        else
    #endif
#endif
#if RTCSCFG_ENABLE_IP6
        if(req_ptr->AF == AF_INET6)
        {
            if (IN6_ARE_ADDR_EQUAL(&remote_host_v6, &in6addr_any)|| (tcb->remote_port == 0))
            {
                TCP_PO_FAIL(RTCSERR_TCPIP_DESTADDR_REQUIRED);
            } /* Endif */
            if (IN6_IS_ADDR_MULTICAST(&remote_host_v6))
            {
                TCP_PO_FAIL(RTCSERR_TCP_ADDR_NA);
            } /* Endif */
        }
#endif /* RTCSCFG_ENABLE_IP6 */
   } /* Endif */



   /*  Supports windows of up to 16 bits in size*/
   if ( tcb->rcvwndmax > 0xffff ) {
      tcb->rcvwndmax = 0xffff;
   } /* Endif */



   /*
   ** Verify that requested ports/hosts combination, if fully
   **  specified, isn't already used
   */
#if RTCSCFG_ENABLE_IP4
    if(req_ptr->AF == AF_INET)
    {
        if ( tcb->local_port != 0 && tcb->remote_port != 0 && remote_host_v4 != INADDR_ANY)
        {
            for (   tcb2=tcp_cfg->TCBhead;
                    tcb2 != NULL &&                            /*   Can ignore this TCB if its:  */
                  ( tcb2->local_port != tcb->local_port    ||  /*   Local port not a match    OR */
                    tcb2->remote_port == 0                 ||  /*   Remote port not specified OR */
                    tcb2->remote_port != tcb->remote_port  ||  /*   Remote port not a match   OR */
                    tcb2->remote_host == INADDR_ANY        ||  /*   Remote host not specified OR */
                    tcb2->remote_host != remote_host_v4    ||  /*   Remote host not a match   OR */
                    tcb2->local_host != tcb->local_host );     /*   Local address not a match    */
                    tcb2 = tcb2->next)                         /*   Otherwise, comb. is in use   */
            {
            } /* Endfor */

            if ( tcb2 != NULL )
            {
                RTCS_log_error( ERROR_TCP, RTCSERR_TCP_OPEN_FAILED,RTCSERR_TCP_ADDR_IN_USE, 0, 0);
                TCP_PO_FAIL(RTCSERR_TCP_ADDR_IN_USE);
            } /* Endif */

        } /* Endif */
   }
   #if RTCSCFG_ENABLE_IP6
   else
   #endif
#endif
#if RTCSCFG_ENABLE_IP6
   if(req_ptr->AF == AF_INET6)
   {

        if (
            (tcb->local_port != 0)    &&
            (tcb->remote_port != 0)   &&
            (!IN6_ARE_ADDR_EQUAL(&remote_host_v6, &in6addr_any))
           )
        {

            for (   tcb2=tcp_cfg->TCBhead;
                    tcb2 != NULL &&                                                           /* Can ignore this TCB if its:    */
                  ( tcb2->local_port != tcb->local_port                             ||        /*   Local port not a match    OR */
                    tcb2->remote_port == 0                                          ||        /*   Remote port not specified OR */
                    tcb2->remote_port != tcb->remote_port                           ||        /*   Remote port not a match   OR */
                    IN6_ARE_ADDR_EQUAL(&(tcb2->remote_ipv6_host), &in6addr_any)     ||        /*   Remote host not specified OR */
                   !IN6_ARE_ADDR_EQUAL(&(tcb2->remote_ipv6_host), &remote_host_v6)  ||        /*   Remote host not a match   OR */

                   !IN6_ARE_ADDR_EQUAL(&(tcb2->local_ipv6_host), &(tcb->local_ipv6_host)) /*   Local address not a match    */
                  );
                    tcb2 = tcb2->next)                                                    /* Otherwise, comb. is in use     */
            {
            } /* Endfor */

            if ( tcb2 != NULL )
            {
                RTCS_log_error( ERROR_TCP,RTCSERR_TCP_OPEN_FAILED,RTCSERR_TCP_ADDR_IN_USE,0, 0);
                TCP_PO_FAIL(RTCSERR_TCP_ADDR_IN_USE);
            } /* Endif */

        } /* Endif */
    }
#endif /* RTCSCFG_ENABLE_IP6 */

   /*  Allocate send buffer */
   if ((sbsize != 0) && (tcb->options & TCPO_ACTIVE)) {

      tcb->sndringbuf = (uchar _PTR_)RTCS_mem_alloc(sbsize);
      if ( tcb->sndringbuf == NULL ) {
         RTCS_log_error( ERROR_TCP, RTCSERR_TCP_OPEN_FAILED, RTCSERR_TCPIP_NO_BUFFS, 0, 0);
         TCP_PO_FAIL(RTCSERR_TCPIP_NO_BUFFS);
      } /* Endif */
       _mem_set_type(tcb->sndringbuf, MEM_TYPE_TCP_TX_WINDOW);

   } /* Endif */
   tcb->sndringlen = sbsize;

   /*  Allocate receive buffer */
   if ((rbsize != 0) && (tcb->options & TCPO_ACTIVE)) {

      tcb->rcvringbuf = (uchar _PTR_)RTCS_mem_alloc(rbsize);
      if ( tcb->rcvringbuf == NULL ) {
         RTCS_log_error( ERROR_TCP, RTCSERR_TCP_OPEN_FAILED, RTCSERR_TCPIP_NO_BUFFS, 0, 0);
         TCP_PO_FAIL(RTCSERR_TCPIP_NO_BUFFS);
      } /* Endif */
       _mem_set_type(tcb->rcvringbuf, MEM_TYPE_TCP_RX_WINDOW);

      /*  Use ring buffer by default  */
      tcb->rcvbuf = tcb->rcvringbuf;

   } /* Endif */
   tcb->rcvringlen = rbsize;
   tcb->rcvlen     = rbsize;

   /*  Fill remainder of new TCB;
    */

   tcb->state        = LISTEN;

#if RTCSCFG_ENABLE_IP4
    if(req_ptr->AF == AF_INET)
    {
        if ( remote_host_v4 != INADDR_ANY )
        {
            tcb->rcvmss = IP_MTU(tcb->local_host, remote_host_v4) - (uint_16) (IP_HEADSIZE + TCP_HEADSIZE);
        } /* Endif */
    }
    #if RTCSCFG_ENABLE_IP6
    else
    #endif
#endif
#if RTCSCFG_ENABLE_IP6
    if(req_ptr->AF == AF_INET6)
    {
        if (!IN6_ARE_ADDR_EQUAL(&remote_host_v6, &in6addr_any))
        {
            tcb->rcvmss = IP6_MTU(&(tcb->local_ipv6_host),&remote_host_v6) - (uint_16) (IP6_HEADSIZE + TCP_HEADSIZE);
        } /* Endif */
    }
#endif /* RTCSCFG_ENABLE_IP6 */

   tcb->sndmss    = TCP_MSS_DEFAULT;   /* default, as per RFC1122, 4.2.2.6 */
   tcb->sndmax    = tcb->sndmss;       /* - options */
                                       /* options is length of any IP and  */
                                       /* TCP options sent */

   tcb->rcvmax    = 1;
   tcb->rcv2full  = 1;                 /* this way we ACK a SYN */

   if ( tcb->sndrto < _TCP_rto_min ) {
      tcb->sndrto = _TCP_rto_min;
   } /* Endif */

   tcb->sndrtta   = 0;                 /* initial RTT, as per RFC1122, */
                                       /*   4.2.3.1 */

   tcb->sndrttd   = tcb->sndrto << 1;  /* because rto == rtta + 2*rttd, and */
                                       /*   rttd is scaled by 2^2 (by 4) */

   tcb->sndcwnd   = tcb->sndmax;       /* init. cwnd to 1 packet  */


   tcb->sndcwndmax = tcb->sndcwnd;     /* for debugging statistics... */
   tcb->sndstresh = TCP_WINDOW_MAX;    /* at least as big as any eventual */
                                       /*   send window*/

   /*  Soft timeout (R1) must occur *before* hard timeout (R2) (see RFC1122
   **   section 4.2.3.5 for more details on these timeouts)
   */
   if ( tcb->sndto_2 != 0 ) {

      if ( tcb->sndto_2 < TCP_SENDTIMEOUT_MIN ) {
         tcb->sndto_2 = TCP_SENDTIMEOUT_MIN;
      } /* Endif */

      /*  If sndto_2 is set, sndto_1 must also be set; if it isn't, set it
       *   to half of sndto_2 (user can always ignore soft timeout events)
       *  Either way, sndto_1 may never exceed half of sndto_2
       */
      if ( (tcb->sndto_1 == 0) || (tcb->sndto_1 > (tcb->sndto_2 >> 1)) ) {
         tcb->sndto_1 = tcb->sndto_2 >> 1;
      } /* Endif */

      tcb->sndto_2 -= tcb->sndto_1;    /* store delta only */

   } /* Endif */


   tcb->sndprobeto = tcb->sndrto;
   tcb->sndprobetomax = tcb->sndrtomax;

   /*  Keep-alive packets should be sent *very* infrequently, usually at
    *   least 2 hours apart; here just do a reasonableness check;
    *   10 times RTO seems a reasonable minimum.
    */
   if ( tcb->keepaliveto != 0 && tcb->keepaliveto < 10 * tcb->sndrto ) {
      tcb->keepaliveto = 10 * tcb->sndrto;
   } /* Endif */

   /* init timers */
   tcb->sndacktq.t.donelist   = (TimeQ _PTR_ _PTR_)&(tcp_cfg->acktq);
   tcb->sndacktq.tcb          = tcb;
   tcb->sndxmittq.t.donelist  = (TimeQ _PTR_ _PTR_)&(tcp_cfg->xmittq);
   tcb->sndxmittq.tcb         = tcb;
   tcb->sndtq.t.donelist      = (TimeQ _PTR_ _PTR_)&(tcp_cfg->sndtq);
   tcb->sndtq.tcb             = tcb;
   tcb->rcvtq.t.donelist      = (TimeQ _PTR_ _PTR_)&(tcp_cfg->rcvtq);
   tcb->rcvtq.tcb             = tcb;

   /*  Allocate space for storing timestamps of sent packets;
    *   pointers are used for the circular queue because they are faster
    *   than indexes (which require multiplies)
    */

   tcb->sndclks = 2 * sbsize / tcb->sndmax + 1;
                                       /* max nb of clock samples */

   if ( tcb->sndclks < 3 ) {           /* minimum reasonable nb */
      tcb->sndclks = 3;
   } /* Endif */

   tcb->sndclkavail = tcb->sndclks;
   tcb->sndclk = (SndClock _PTR_)RTCS_mem_alloc(tcb->sndclks*sizeof(SndClock));
   if ( tcb->sndclk == NULL ) {
      RTCS_log_error( ERROR_TCP, RTCSERR_OUT_OF_MEMORY,
         tcb->sndclks*sizeof(SndClock), 3, 0);
      TCP_PO_FAIL(RTCSERR_OUT_OF_MEMORY);
   } /* Endif */

   _mem_set_type(tcb->sndclk, MEM_TYPE_TCP_SEND_CLOCK);

   tcb->sndclkhead = tcb->sndclk;
   tcb->sndclktail = tcb->sndclk;
   tcb->sndclkend  = tcb->sndclk + tcb->sndclks;

#if RTCSCFG_ENABLE_IP4
        if(req_ptr->AF == AF_INET)
        {
            tcb->remote_host = remote_host_v4;
        }
    #if RTCSCFG_ENABLE_IP6
        else
    #endif
#endif
#if RTCSCFG_ENABLE_IP6
        if(req_ptr->AF == AF_INET6)
        {
            IN6_ADDR_COPY(&(remote_host_v6),&(tcb->remote_ipv6_host));
            /* add scope_id here */
            tcb->if_scope_id = req_ptr->IF_SCOPE_ID;
        }
#endif /* RTCSCFG_ENABLE_IP6 */

   tcb->conn_pending = 0;

   /* TcpOpen succeeded */
#if RTCSCFG_ENABLE_TCP_STATS
   tcp_cfg->STATS.ST_CONN_OPEN++;
   if ( tcb->options & TCPO_ACTIVE )
   {
      tcp_cfg->STATS.ST_CONN_ACTIVE++;
   }
   else
   {
      tcp_cfg->STATS.ST_CONN_PASSIVE++;
   } /* Endif */
#endif

    if( tcb->options & TCPO_ACTIVE )
    {
      /* Looking for the best local interface to connection to remote host. */
#if RTCSCFG_ENABLE_IP4
        if(req_ptr->AF == AF_INET)
        {
            if (tcb->local_host == INADDR_ANY)
            {
                tcb->local_host = IP_route_find(remote_host_v4, 0);
                if (tcb->local_host == INADDR_ANY)
                {
                    TCP_PO_FAIL(RTCSERR_TCP_HOST_UNREACH);
                } /* Endif */
            } /* Endif */
        }
    #if RTCSCFG_ENABLE_IP6
        else
    #endif
#endif

#if RTCSCFG_ENABLE_IP6
        if(req_ptr->AF == AF_INET6)
        {
            if (IN6_ARE_ADDR_EQUAL(&(tcb->local_ipv6_host), &in6addr_any))
            {
                IN6_ADDR_COPY((in6_addr*)IP6_route_find(&remote_host_v6, 0),&(tcb->local_ipv6_host));
                if (IN6_ARE_ADDR_EQUAL(&(tcb->local_ipv6_host), &in6addr_any))
                {
                    TCP_PO_FAIL(RTCSERR_TCP_HOST_UNREACH);
                } /* Endif */
            } /* Endif */
        }
#endif /* RTCSCFG_ENABLE_IP6 */

        /* Hang on to open requests until established */
        req_ptr->NEXT = tcp_cfg->OPENS;
        tcp_cfg->OPENS = req_ptr;


        tcb->state = SYN_SENT;

    /*set inet family*/
        tcb->af = req_ptr->AF;

      TCP_Start_sending(tcb, tcp_cfg);
   }
   else
   {
        RTCSCMD_complete(req_ptr, RTCS_OK);
   } /* Endif */

   return;

   failopen:

      if ( tcb != NULL ) {

         if ( tcb->sndringbuf != NULL ) {
            _mem_free(tcb->sndringbuf);
            tcb->sndringbuf = NULL;
         } /* Endif */

         if ( tcb->rcvringbuf != NULL ) {
            _mem_free(tcb->rcvringbuf);
            tcb->rcvringbuf = NULL;
         } /* Endif */

         if ( tcb->sndclk != NULL ) {
            _mem_free(tcb->sndclk);
            tcb->sndclk = NULL;
         } /* Endif */

      } /* Endif */

      RTCSCMD_complete(req_ptr, result);
      return;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_accept
* Returned Values : None.
* Comments        :
*
*  Called when the application accept()s a new connection.
*
*END*-----------------------------------------------------------------*/

void TCP_Process_accept
   (
      TCP_PARM_PTR   req_ptr     /* IN/OUT - the accept request */
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;          /* TCP Layer constants */
   TCB_STRUCT_PTR       tcb;
   TCB_STRUCT_PTR _PTR_ search_ptr;

   tcp_cfg = RTCS_getcfg(TCP);
   tcb = req_ptr->TCB_PTR;

   if ( tcb == NULL ) {
      RTCSCMD_complete(req_ptr, RTCSERR_TCP_CONN_RLSD);
      return;
   }

   search_ptr = &tcp_cfg->NOSOCK_TCB;

   if (tcb->conn_pending) {
      tcb->conn_pending--;

      /* Check if a listening TCB has spawned another TCB */
      while (*search_ptr) {
         if ((*search_ptr)->LISTEN_TCB == tcb &&
             (*search_ptr)->conn_pending)
         {
            /* Found a match */
            (*search_ptr)->conn_pending--;
            req_ptr->TCB_PTR = *search_ptr;
            *search_ptr = (*search_ptr)->NOSOCK_NEXT;
            req_ptr->TCB_PTR->SOCKET = req_ptr->SOCKET;
            ((SOCKET_STRUCT_PTR)req_ptr->SOCKET)->TCB_PTR = req_ptr->TCB_PTR;
            req_ptr->TCB_PTR->LISTEN_TCB = NULL;
            RTCSCMD_complete(req_ptr, RTCS_OK);
            return;
         } /* Endif */

         search_ptr = &(*search_ptr)->NOSOCK_NEXT;
      } /* Endwhile */

      /* Didn't find any TCBs. The TCB must have been released by a RST */
      RTCSCMD_complete(req_ptr, RTCSERR_TCP_CONN_RESET);
   } else {

      /* Hang on to open requests until established */
      req_ptr->NEXT = tcp_cfg->OPENS;
      tcp_cfg->OPENS = req_ptr;

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_signal
* Returned Values : None.
* Comments        :
*
*  Process TCP timeouts.
*
*END*-----------------------------------------------------------------*/
void TCP_Process_signal
   (
      void
   )
{ /* Body */
   TCP_CFG_STRUCT_PTR   tcp_cfg;
   TCB_STRUCT_PTR       tcb;
   TcbTimeQ _PTR_       tq;
   int_32               result;

   tcp_cfg = RTCS_getcfg(TCP);

   RTCSLOG_FNE5(RTCSLOG_FN_TCP_Process_signal, tcp_cfg->xmittq, tcp_cfg->acktq, tcp_cfg->rcvtq, tcp_cfg->sndtq);

   /*
   ** Process any (re)transmission timeouts
   */
   while (1) {

      tq = tcp_cfg->xmittq;
      if ( tq == NULL ) {
         break;
      } /* Endif */
      TCP_Timer_remove(&tq->t);                /* remove from its list */

      tcb = tq->tcb;
      DEBUGTCP(printf("\nTCP: (re)transmission timeout, tcb @ %p", tcb));

      if ( tcb->VALID == TCB_VALID_ID ) {

         if ( tcb->state == TIME_WAIT ) {     /* was it a time-wait timer? */

            /*
            ** 2MSL timeout expired
            */
            TCP_Close_TCB(tcb, RTCS_OK, tcp_cfg);
            TCP_Process_release(tcb, tcp_cfg);

            continue;   /* at start of LOOP */

         } /* Endif */

         /*
         ** Are we probing a zero-sized send window?
         */
         if ( tcb->sndxmitwhat == TCP_PROBE ) {

            /*
            ** exponential backoff, clamped
            */
            if ( (tcb->sndprobeto <<= 1) > tcb->sndprobetomax ) {
               tcb->sndprobeto = tcb->sndprobetomax;
            } /* Endif */
            TCP_Transmit(tcb, tcp_cfg, FALSE, TRUE);
            /* will restart xmittq with probeto */

         } else {  /* else we must be retransmitting (sndxmitwhat EQ TCP_XMIT) */

            if ( tcb->sndxmitwhat != TCP_XMIT ) {
               RTCS_log_error( ERROR_TCP, RTCSERR_TCP_REXMIT_PROBLEM,
                  (uint_32)tcb->sndxmitwhat, (uint_32)tcb, 0 );
            } /* Endif */


            /* congestion avoidance */
            if ( tcb->sndwnd < tcb->sndcwnd ) {
               tcb->sndstresh = tcb->sndwnd >> 1;
            } else {
               tcb->sndstresh = tcb->sndcwnd >> 1;
            } /* Endif */
            tcb->sndcwnd = tcb->sndmax;           /* slow-start */

            if ( (tcb->sndrto <<= 1) > tcb->sndrtomax ) {

               /*
               ** exponential backoff
               */
               tcb->sndrto = tcb->sndrtomax;   /* clamped */

            } /* Endif */

            TCP_Transmit(tcb, tcp_cfg, TRUE, TRUE); /* force retransmission */
            tcb->rexmts++;
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_TX_DATA_DUP++);

            /*
            ** restart the timeout
            */
            result =
               TCP_Timer_start( &tq->t,      /* the timer */
                           tcb->sndrto,      /* the timeout value */
                           0,                /* no reload */
                           tcp_cfg->qhead ); /* the timer queue head */

            if ( result != RTCS_OK ) {
               RTCS_log_error( ERROR_TCP, RTCSERR_TCP_TIMER_FAILURE,
                  (uint_32)result, (uint_32)tcb, 0 );
            } /* Endif */

         } /* Endif */

      } /* Endif */

   } /* EndWhile */

   /*
   ** Then, process any ACK timers (Ack's to send)
   */
   while (1) {

      tq = tcp_cfg->acktq;
      if ( tq == NULL ) {
         break;
      } /* Endif */

      TCP_Timer_remove(&tq->t);      /* remove from its list */

      tcb = tq->tcb;

      if ( tcb->VALID == TCB_VALID_ID ) {

         DEBUGTCP(printf("\nTCP: ACK timer, tcb @ %p", tcb));
         TCP_Transmit(tcb, tcp_cfg, FALSE, TRUE);

         /*
         ** If sender idle, restart keep-alive timer
         */
         if ( tcb->sndlen == 0 && tcb->keepaliveto != 0 ) {
            TCP_Send_keepalive(tcb,tcp_cfg);
         } /* Endif */

      } /* Endif */

   } /* EndWhile */

   /*
   ** Process receive timers
   */
   while (1) {

      tq = tcp_cfg->rcvtq;
      if ( tq == NULL ) {
         break;
      } /* Endif */

      TCP_Timer_remove(&tq->t);             /* remove from its list */

      tcb = tq->tcb;
      DEBUGTCP(printf("\nTCP: receive timer, tcb @ %p", tcb));

      if ( tcb->VALID == TCB_VALID_ID ) {

         if ( tcb->rcvHead != NULL ) {   /* should be true, but check anyway */
            TCP_Reply_receive(tcb, tcp_cfg, RTCSERR_TCP_TIMED_OUT);

            /*
            ** Setup next receive
            */
            while ( tcb->rcvHead != NULL &&
                  ! TCP_Setup_receive(tcb->rcvHead, tcb, tcp_cfg)
            ) {
            } /* Endwhile */

         } /* Endif */

      } /* Endif */

   } /* EndWhile */

   /*
   ** Finally, process any Send timers (transmission not ack'd
   **  within timeout)
   */
   while (1) {

      tq = tcp_cfg->sndtq;
      if ( tq == NULL ) {
         break;
      } /* Endif */

      TCP_Timer_remove(&tq->t);                /* remove from its list */

      tcb = tq->tcb;
      DEBUGTCP(printf("\nTCP: send timer, tcb @ %p", tcb));

      if ( tcb->VALID == TCB_VALID_ID ) {

         if ( tcb->state != CLOSED && tcb->sndtohard ) {

            /*
            ** hard timeout reached
            */
            TCP_Close_TCB(tcb, RTCSERR_TCP_TIMED_OUT, tcp_cfg);
            if (tcb->SOCKET == 0) {   /* Free TCB if unused */
               TCP_Process_release(tcb, tcp_cfg);
            } /* Endif */

         } else {                                /* soft timeout reached */

            /*
            ** User timeout; sent data not acked in time
            */
            TCP_Event(tq->tcb, TCPS_TIMEOUT);
            if ( tcb->sndto_2 != 0 ) {        /* start hard timeout... */
               result =
                  TCP_Timer_start( &tq->t,      /* the timer */
                              tcb->sndto_2,     /* the timeout value */
                              0,                /* no reload */
                              tcp_cfg->qhead ); /* the timer queue head */
               if ( result != RTCS_OK ) {
                  RTCS_log_error( ERROR_TCP, RTCSERR_TCP_TIMER_FAILURE,
                     (uint_32)result, (uint_32)tcb, 1 );
               } /* Endif */
               tcb->sndtohard = TRUE;
            } /* Endif */

            /*
            ** Tell IP about it...
            */
/*          IP_Advise_delivery_problem( 0,tcb->icb,tcb->rem_host,tcb->tos );
*/
            tcb->sndtmouts++;
            /* tcp_cfg->STATS.ST_SNDTMOUTS++; */

         } /* Endif */

      } /* Endif */

   }

   RTCSLOG_FNX1(RTCSLOG_FN_TCP_Process_signal);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Return_open
* Returned Values : None.
* Comments        :
*
*  Finds and returns the TcpOpen or TcpListen request back to the
*     upper layer.
*
*END*-----------------------------------------------------------------*/

void TCP_Return_open
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      uint_32              errnum,  /* IN     - reason for returning */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   TCP_PARM_PTR  req_ptr;     /* upper layer request */
   TCP_PARM_PTR  prev_ptr;    /* previous upper layer request */
   TCB_STRUCT_PTR _PTR_ search_ptr = &tcp_cfg->NOSOCK_TCB;

   /* Return open request to upper layer */

   prev_ptr = NULL;
   req_ptr = tcp_cfg->OPENS;
   while (req_ptr != NULL &&
          tcb != req_ptr->TCB_PTR &&
          tcb->LISTEN_TCB != req_ptr->TCB_PTR) {
      prev_ptr = req_ptr;
      req_ptr = req_ptr->NEXT;
   } /* Endwhile */

   if (req_ptr != NULL) {
      if (prev_ptr == NULL) {
         tcp_cfg->OPENS = req_ptr->NEXT;
      } else {
         prev_ptr->NEXT = req_ptr->NEXT;
      } /* Endif */
      req_ptr->NEXT = NULL;

      /* Remove the TCB from the list of TCBS spawned by the listening TCB */
      while (*search_ptr) {
         if (*search_ptr == tcb) {
            *search_ptr = (*search_ptr)->NOSOCK_NEXT;
            tcb->SOCKET = req_ptr->SOCKET;
            ((SOCKET_STRUCT_PTR)req_ptr->SOCKET)->TCB_PTR = tcb;
            tcb->LISTEN_TCB = NULL;
            break;
         } /* Endif */
         search_ptr = &(*search_ptr)->NOSOCK_NEXT;
      } /* Endwhile */

      req_ptr->TCB_PTR = tcb;
      RTCSCMD_complete(req_ptr, errnum);

   } else {
      if (tcb->state != CLOSED) { /* Only increment count if we receive a new connection */
         tcb->conn_pending++;
      } /* Endid */

      if (tcb->LISTEN_TCB == NULL) {
         SOCK_select_signal(tcb->SOCKET, tcb->state);
      } else {
         if (tcb->state != CLOSED) {
            /* Only increment count if we receive a new connection */
            tcb->LISTEN_TCB->conn_pending++;
         } /* Endif */
         SOCK_select_signal(tcb->LISTEN_TCB->SOCKET, tcb->state);
      } /* Endif */

   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Clone_tcb
* Returned Values : A new TCB in the SYN_RECEIVED state (or NULL)
* Comments        :
*
*  Takes a listening TCB in the listen state and creates a new TCB to be
*  placed in the SYN_RECEIVED state.
*
*END*-----------------------------------------------------------------*/


TCB_STRUCT_PTR TCP_Clone_tcb
   (
      TCB_STRUCT_PTR    listen_ptr  /* IN - the TCB to clone */
   )
{ /* Body */
   TCB_STRUCT_PTR       new_tcb;
   TCP_CFG_STRUCT_PTR   tcp_cfg = RTCS_getcfg(TCP);

   new_tcb = RTCS_mem_alloc_zero(sizeof(TCB_STRUCT));

   /* Make sure memory allocation succeeded */
   if (new_tcb == NULL) {
      return NULL;
   } /* Endif */
   _mem_set_type(new_tcb, MEM_TYPE_TCB);

   /* Clone the TCB */
   _mem_copy(listen_ptr, new_tcb, sizeof(TCB_STRUCT));
   new_tcb->sndringbuf     = (uchar _PTR_)RTCS_mem_alloc(new_tcb->sndringlen);
   new_tcb->rcvringbuf     = (uchar _PTR_)RTCS_mem_alloc(new_tcb->rcvringlen);
   new_tcb->sndclk         = RTCS_mem_alloc(new_tcb->sndclks*sizeof(SndClock));

   /* Check if memory allocation succeeded */
   if (new_tcb->sndringbuf == NULL ||
       new_tcb->rcvringbuf == NULL ||
       new_tcb->sndclk     == NULL)
   {
      /* Free memory */
      if (new_tcb->sndringbuf) {
         _mem_free(new_tcb->sndringbuf);
      } /* Endif */

      if (new_tcb->rcvringbuf) {
         _mem_free(new_tcb->rcvringbuf);
      } /* Endif */

      if (new_tcb->sndclk) {
         _mem_free(new_tcb->sndclk);
      } /* Endif */

      _mem_free(new_tcb);
      return NULL;
   } /* Endif */

   _mem_set_type(new_tcb->sndclk, MEM_TYPE_TCP_SEND_CLOCK);
   _mem_set_type(new_tcb->sndringbuf, MEM_TYPE_TCP_TX_WINDOW);
   _mem_set_type(new_tcb->rcvringbuf, MEM_TYPE_TCP_RX_WINDOW);


   /* Initialize new TCB */
   new_tcb->next           = tcp_cfg->TCBhead;
   tcp_cfg->TCBhead        = new_tcb;
   new_tcb->SOCKET         = 0;
   new_tcb->LISTEN_TCB     = listen_ptr;
   new_tcb->rcvbuf         = new_tcb->rcvringbuf;
   new_tcb->conn_pending   = 0;
   tcp_cfg->CONN_COUNT++;
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_CONN_OPEN++);
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_CONN_PASSIVE++);

   /* Setup timer queues */
   new_tcb->rcvtq.tcb      = new_tcb;
   new_tcb->sndxmittq.tcb  = new_tcb;
   new_tcb->sndacktq.tcb   = new_tcb;
   new_tcb->sndtq.tcb      = new_tcb;

   /* Setup send clocks */
   new_tcb->sndclkhead     = new_tcb->sndclk;
   new_tcb->sndclktail     = new_tcb->sndclk;
   new_tcb->sndclkend      = new_tcb->sndclk + new_tcb->sndclks;

   new_tcb->NOSOCK_NEXT = tcp_cfg->NOSOCK_TCB;
   tcp_cfg->NOSOCK_TCB = new_tcb;
#if RTCSCFG_TCP_MAX_HALF_OPEN
   new_tcb->tcb_spawn_time = RTCS_time_get();
   /* The first element will be stored in index 0 */
   tcp_cfg->HALF_OPEN_TCB_LIST[tcp_cfg->HALF_OPEN_TCB_COUNT] = new_tcb;
   tcp_cfg->HALF_OPEN_TCB_COUNT++;
#endif

   /* New TCB needs to inherit the TX options */
   new_tcb->TX = listen_ptr->TX;

   return new_tcb;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP6_Process_packet
* Returned Values : None.
* Comments        :
*
*  Process an incoming TCP packet.  Note that the PCB is freed by
*   the routine it is consumed in.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6

void TCP6_Process_packet
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      RTCSPCB_PTR          pcb,     /* IN/OUT - the incoming packet */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   TCP_HEADER_PTR    seg;
   uint_16           seglen;
   uint_16           flags;
   uint_16           optlen;
   uchar _PTR_       opt;
   uint_32           seq,ack;
   uint_16           urg;
   int_32            result;     /* returned result */
   boolean           seg_okay;
   /*IPv6*/
   in6_addr         ipv6_source;



   seg_okay = TRUE;

   seg = (TCP_HEADER _PTR_)RTCSPCB_DATA(pcb);
   flags = ntohs(seg->flags);

   RTCSLOG_FNE5(RTCSLOG_FN_TCP_Process_packet, tcb, pcb, tcp_cfg, flags);

   RTCSPCB_next(pcb, (flags & DATAOFS_MASK) >> (DATAOFS_SHIFT - 2));

   /*
   ** Drop packets that have illegal/abnormal flag combinations, as these
   ** are almost certainly malicious.
   **
   ** - SYN FIN is probably the best known illegal combination. Remember
   **   that SYN is used to start a connection, while FIN is used to end
   **   an existing connection. It is nonsensical to perform both actions
   **   at the same time. Many scanning tools use SYN FIN packets, because
   **   many intrusion detection systems did not catch these in the past,
   **   although most do so now. You can safely assume that any SYN FIN
   **   packets you see are malicious.
   **
   ** - SYN FIN PSH, SYN FIN RST, SYN FIN RST PSH, and other variants on
   **   SYN FIN also exist. These packets may be used by attackers who are
   **   aware that intrusion detection systems may be looking for packets
   **   with just the SYN and FIN bits set, not additional bits set. Again,
   **   these are clearly malicious.
   **
   ** - Packets should never contain just a FIN flag. FIN packets are
   **   frequently used for port scans, network mapping and other stealth
   **   activities.
   **
   ** - Some packets have absolutely no flags set at all; these are referred
   **   to as "null" packets. It is illegal to have a packet with no flags set.
   */
   if ( ((flags & SYN) && (flags & FIN)) ||
        (flags == FIN) ||
        (flags == 0) ) {
      TCP_PP_DROP;
   }

   /*
   ** compute SEG.LEN
   */
   seglen = RTCSPCB_SIZE(pcb);   /* socket layer data size */
   if ( flags & SYN ) {
      seglen++;
   } /* Endif */

   if ( flags & FIN ) {
      seglen++;
   } /* Endif */

   /* using IPv6 copy macros to copy the IPv6 address structure */
   IN6_ADDR_COPY((in6_addr *)((IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->SOURCE, &ipv6_source);

   /*
   ** Process options
   */
   opt = (uchar _PTR_)seg + sizeof(TCP_HEADER);
   while ( opt < RTCSPCB_DATA(pcb) ) {

      switch ( (ntohc(opt)) ) {

         case OPT_PAD:
            opt = RTCSPCB_DATA(pcb);
            break;

         case OPT_NOOP:
            opt++;
            break;

         case OPT_MSS:

            { /* Scope */

               uint_16 mss;

               optlen = ntohc(opt+1);
               if ( optlen == 0 ) {

                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_OPTION++);
                  TCP_PP_DROP;

               } /* Endif */
               mss = ntohs(opt+2);

               /*
               ** Compare with tcb->rcvmss since it is set to
               **  our maximum allowable, and doesn't change:
               */
               if ( tcb->rcvmss != 0 && mss > tcb->rcvmss ) {
                  mss = tcb->rcvmss;
               } /* Endif */

               tcb->sndmss = mss;
               tcb->sndmax = mss; /* minus length of any IP and TCP options; */

               if ( mss < tcb->rcvmax ) {

                  tcb->rcvmax = mss;

                  /*
                  ** Will reply after 1.75*mss bytes have been received.
                  */
                  tcb->rcv2full = mss + (mss>>1) + (mss>>2);

               } /* Endif */

               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_OPT_MSS++);
               opt += optlen;
               break;

            } /* Endscope */

         default:

            /*
            ** All options will have a length byte
            */
            optlen = ntohc(opt+1);
            if ( optlen == 0 ) {

               /*
               ** should send ICMP parameter problem
               */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_OPTION++);
               TCP_PP_DROP;

            } /* Endif */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_OPT_OTHER++);
            opt += optlen;

      } /* End Switch */

   } /* Endwhile */

   /*
   **  Process packet:
   **
   **  (RFC 793 sec.3.9, 'SEGMENT ARRIVES' (pages 65-76);
   **   steps are indicated as FIRST, SECOND, THIRD, etc. as in the RFC)
   */

   seq = ntohl(seg->seq);
   ack = ntohl(seg->ack);
   urg = ntohs(seg->urgent);

   switch ( tcb->state ) {

      /*
      **   This case is executed only when in a invalid state, since
      **   packets aren't queued on CLOSED connections, and closing
      **   connections (see TCP_Close_TCB()) disown their queued packets...
      */
      case CLOSED:
         if( (flags & RST) == 0 )
         {
            TCP_STATE_DEBUG(("state = CLOSED, replying with RST on tcb %p\n", tcb));
            TCP6_Send_reply_reset(tcb,
                           &ipv6_source,        /* source address */
                           seg,           /* received packet */
                           seglen,        /* length of received packet */
                           tcp_cfg,       /* TCP layer data */
                           FALSE
                          );
         } /* Endif */
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_LATE_DATA++);
         TCP_PP_DROP;

      case LISTEN:

         if ( (flags & RST) != 0 ) {                         /* FIRST */
            TCP_STATE_DEBUG(("state = LISTEN, heard reset on tcb %p, dropping.\n", tcb));
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            TCP_PP_DROP;
         } /* Endif */

         if( (flags & ACK) != 0 )
         {                         /* SECOND */
            TCP6_Send_reply_reset(tcb,
                           &ipv6_source,        /* source address */
                           seg,           /* received packet */
                           seglen,        /* length of received packet */
                           tcp_cfg,       /* TCP layer data */
                           tcb->bypass_tx_chksum
                          );
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
            TCP_PP_DROP;
         } /* Endif */

         if ( (flags & SYN) != 0 ) {                         /* THIRD */
#if RTCSCFG_TCP_MAX_HALF_OPEN
            if ((tcp_cfg->HALF_OPEN_TCB_COUNT >= RTCSCFG_TCP_MAX_HALF_OPEN)
#if RTCSCFG_TCP_MAX_CONNECTIONS
             || (tcp_cfg->CONN_COUNT >= RTCSCFG_TCP_MAX_CONNECTIONS)
#endif
               ) {
               TCP_half_open_TCB_close(tcp_cfg);
            } /* Endif */
#endif

#if RTCSCFG_TCP_MAX_CONNECTIONS
            if (tcp_cfg->CONN_COUNT >= RTCSCFG_TCP_MAX_CONNECTIONS) {
               TCP_PP_DROP;
            } /* Endif */
#endif

            CHECK_SECURITY;
            CHECK_PRECEDENCE;

            tcb = TCP_Clone_tcb(tcb);
            if (tcb == NULL) {
               TCP_PP_DROP;
            } /* Endif */
            if ( IN6_ARE_ADDR_EQUAL(&tcb->local_ipv6_host, &in6addr_any))
            {
               IN6_ADDR_COPY((in6_addr *)((IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->DEST, &(tcb->local_ipv6_host));
            } /* Endif */
            if ( IN6_ARE_ADDR_EQUAL(&tcb->remote_ipv6_host, &in6addr_any) )
            {
                 IN6_ADDR_COPY(&ipv6_source,&(tcb->remote_ipv6_host));
                 tcb->rcvmss = IP6_MTU(&(tcb->local_ipv6_host),&ipv6_source) - (uint_16) (IP6_HEADSIZE + TCP_HEADSIZE);
               if(tcb->sndmss > tcb->rcvmss)
               {
                    tcb->sndmss = tcb->rcvmss;
               }
               tcb->sndmax = tcb->sndmss;
            } /* Endif */
            tcb->remote_port = ntohs(seg->source_port);
            /*
            ** TCP_Start_receiving sets state to SYN_RECEIVED
            */
            TCP_Start_receiving(tcb, seq, tcp_cfg);
            /*
            ** also below, in case SYN_SENT
            */
            TCP_Start_sending(tcb, tcp_cfg);
            /*
            ** Release the packet
            */
            RTCSLOG_PCB_FREE(pcb, RTCS_OK);
            RTCSPCB_free(pcb);

         }
         else
         {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_EXPECTED++);
            TCP_PP_DROP;                                    /* FOURTH */
         } /* Endif */

         break;

      case SYN_SENT:

         if ( (flags & ACK) != 0 ) {                                /* FIRST */

            if ( LE32(ack, tcb->sndiss) ||
               GT32(ack, tcb->sndnxt) )
            {

               /*
               ** invalid ACK: drop, and reply if reset
               */
               if ( ! (flags & RST) ) {
                  TCP_STATE_DEBUG(("state = SYN_SENT, heard invalid ACK on tcb %p, sending RST\n", tcb));
                  TCP6_Send_reply_reset(
                                 tcb,
                                &ipv6_source,   /* source address */
                                 seg,           /* received packet */
                                 seglen,        /* length of received packet */
                                 tcp_cfg,       /* TCP layer data */
                                 tcb->bypass_tx_chksum
                                );
               } /* Endif */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
               TCP_PP_DROP;

            } /* Endif */

            if ( LT32(ack, tcb->snduna) ) {

               /*
               ** unnacceptable ACK (already ack'ed): drop
               */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
               TCP_PP_DROP;

            } /* Endif */

         } /* Endif */

         if ( (flags & RST) != 0 ) {                                /* SECOND */

             TCP_STATE_DEBUG(("state = SYN_SENT, heard RST on tcb %p\n", tcb));
            if ( (flags & ACK) != 0 ) {
               TCP_STATE_DEBUG(("state = SYN_SENT, heard RST+ACK on tcb %p, closing&dropping\n", tcb));
               TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg);
               TCP_Process_release(tcb, tcp_cfg);
            } else {
               TCP_STATE_DEBUG(("state = SYN_SENT, ACK was not set so not closing & yes dropping on tcb %p\n", tcb));
            }/* Endif */

            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            TCP_PP_DROP;

         } /* Endif */

         CHECK_SECURITY;            /* else reset & DROP */ /* THIRD */

         if ( (flags & ACK) != 0 ) {
            CHECK_EXACT_PRECEDENCE; /* else reset & DROP */
         } else {
            CHECK_PRECEDENCE;       /* may raise; if can't, reset & DROP */
         } /* Endif */

         if ( (flags & SYN) != 0 ) {                         /* FOURTH */

            TCP_Start_receiving(tcb, seq, tcp_cfg);   /* State=>SYN_RCVD */

            if ( (flags & ACK) != 0 ) {
               if ( GE32(ack, tcb->snduna) ) {
                  TCP_Process_ack(tcb, seg, tcp_cfg);
               } /* Endif */
            } /* Endif */

            if ( GT32(tcb->snduna, tcb->sndiss) ) {
               tcb->state = ESTABLISHED;
               TCP_Update_send_window(tcb,seg);
               TCP_Return_open(tcb, RTCS_OK, tcp_cfg);
               TCP_Event(tcb, TCPS_STARTED);
               TCP_Transmit(tcb, tcp_cfg, FALSE, TRUE);
               goto step6;             /* named from RFC793... */

            } else {
               /* force retransmit of SYN */
               TCP_Transmit(tcb, tcp_cfg, TRUE, TRUE);

            } /* Endif */

            /*
            **  any data or other controls are ignored, not queued
            **  for processing after state becomes ESTABLISHED
            */

            /*
            ** Free the packet
            */
            RTCSLOG_PCB_FREE(pcb, RTCS_OK);
            RTCSPCB_free(pcb);

         } else {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_EXPECTED++);
            TCP_PP_DROP;                                    /* FIFTH */

         } /* Endif */

         break;

      /* all included in default
      ** case SYN_RECEIVED:
      ** case ESTABLISHED:
      ** case FINWAIT_1:
      ** case FINWAIT_2:
      ** case CLOSE_WAIT:
      ** case CLOSING:
      ** case LAST_ACK:
      ** case TIME_WAIT:
      */
      default:

        /**
         ** From RFC 793:
         **
         ** There are four cases for the acceptability test for an incoming
         ** segment:
         **
         ** Segment Receive  Test
         ** Length  Window
         ** ------- -------  -------------------------------------------
         **
         **    0       0     SEG.SEQ = RCV.NXT
         **
         **    0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
         **
         **   >0       0     not acceptable
         **
         **   >0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
         **               or RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND
         **
         ** If the RCV.WND is zero, no segments will be acceptable, but
         ** special allowance should be made to accept valid ACKs, URGs and
         ** RSTs.
         **
         ** If an incoming segment is not acceptable, an acknowledgment
         ** should be sent in reply (unless the RST bit is set, if so drop
         ** the segment and return):
         **
         **   <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
         **
         ** After sending the acknowledgment, drop the unacceptable segment
         ** and return.
         */

         /*
         ** Handles all cases explicitly.  seg_okay is used later in this procedure
         ** to drop an un-acceptable segment after processing its RST/ACK/URG fields
         ** in the case where the window is 0. Note, seglen may have been set to 1 to
         ** handle SYN or FIN flags.
         */
         { /* Scope */
            uint_16  rwindow;
            boolean  okay;

            rwindow = tcb->rcvwndedge - tcb->rcvnxt;

            if ( (rwindow == 0) &&                     /* Case 1 */
               (seglen == 0 || (flags & SYN && seglen == 1))  &&
               (seq == tcb->rcvnxt))
            {
               okay = TRUE;
               seg_okay = TRUE;

            } else if ( (rwindow > 0) &&                 /* Case 2 */
                  (seglen == 0 || (flags & SYN && seglen == 1))  &&
                   (LE32(tcb->rcvnxt,seq) &&
                    LT32(seq,tcb->rcvwndedge)))
            {
               okay = TRUE;
               seg_okay = TRUE;

            } else if ((flags & FIN) &&
                       LT32(tcb->rcvnxt,seq))
            {
              okay = TRUE;
              seg_okay = FALSE;

            } else if ( (seglen > 0)  &&                 /* Case 4 */
                   (rwindow > 0) &&
                   ((LE32(tcb->rcvnxt,seq) &&
                     LT32(seq,tcb->rcvwndedge)) ||
                    (LE32(tcb->rcvnxt,seq+seglen-1) &&
                     LT32(seq+seglen-1,tcb->rcvwndedge))))
            {
               okay = TRUE;
               seg_okay = TRUE;

            } else {                                      /* Case 3 or others failed */
               seg_okay = FALSE;
               if ( (rwindow == 0) && (flags & (RST | URG | ACK)) != 0 ) {
                  RTCSPCB_SIZE(pcb) = 0;
                  okay = TRUE;
                  /*tcp_cfg->STATS.ST_RX_ACC_BADSEGS++;*/
               } else {
                  okay = FALSE;
               } /* Endif */

            } /* Endif */

            if ( ! okay ) {
               /*
               ** segment unacceptable, sent ACK
               */
               if ( (flags & RST) == 0 ) {
                  TCP_Must_send_ack(tcb, tcp_cfg);
               } /* Endif */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_DATA++);
               TCP_PP_DROP;
            } /* Endif */

         } /* Endscope */

         /*
         ** Process the RST
         */
         if ( (flags & RST) != 0 ) {                                /* SECOND */
            TCP_STATE_DEBUG(("state = default(%d), heard reset on tcb %p, ", tcb->state, tcb));

            /* draft-ietf-tcpm-tcpsecure-00.txt section 2.2 */
            if (tcb->tcpsecuredraft0) {
                if (seq == tcb->rcvnxt) {
                   TCP_STATE_DEBUG(("RST is valid, closing&dropping.\n"));
                   TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg); /* aborted */
                   TCP_Process_release(tcb, tcp_cfg);
                   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
                   TCP_PP_DROP;
                }
                else
                {
                   TCP_STATE_DEBUG(("RST is suspicious, acking&dropping\n"));
                   TCP_Must_send_ack(tcb, tcp_cfg);
                   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
                   TCP_PP_DROP;
                } /* Endif */
            } else {

            TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg); /* aborted */
            TCP_Process_release(tcb, tcp_cfg);
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            TCP_PP_DROP;

            }
            TCP_STATE_DEBUG(("\n"));
         } /* Endif */

         if ( tcb->state == SYN_RECEIVED ) {                 /* THIRD */
            CHECK_SECURITY; /* else reset & DROP */
         } else {
            CHECK_TIGHT_SECURITY;
            /*
            ** else reset & TCP_Close_TCB(tcb,RTCSERR_TCP_CONN_RESET) & DROP
            */
         } /* Endif */

         /*
         ** Process the SYN
         */
         if ( (flags & SYN) != 0 ) {                         /* FOURTH */
             if (tcb->tcpsecuredraft0) {
             /*
             ** draft-ietf-tcpm-tcpsecure-00.txt section 3.2
             */
                if (seq == tcb->rcvnxt) { /* SYN and seq is exact match */
                   tcb->ackmodifier = 1; /* ack number of ack pkt will be decremented by 1 later */
                }
                else /* SYN and seq is acceptable even though not exact */
                {
                   tcb->ackmodifier = 0;
                }
                TCP_Must_send_ack(tcb, tcp_cfg);
                IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_NOT_EXPECTED++);
                TCP_PP_DROP;
             }
             else
             {
                  /*
                  ** shouldn't receive SYN
                  */
               TCP6_Send_reply_reset(
                               tcb,
                              &ipv6_source,   /* source address */
                               seg,     /* received packet */
                               seglen,  /* length of received packet */
                               tcp_cfg, /* TCP layer data */
                               tcb->bypass_tx_chksum
                              );
                if ( tcb->state != LISTEN ) {

                   TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg); /* state=>CLOSED */
                   if (tcb->SOCKET == 0) {
                      TCP_Process_release(tcb, tcp_cfg);
                   } /* Endif */

                } /* Endif */
                IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_NOT_EXPECTED++);
                TCP_PP_DROP;
             } /* Endif */
         } /* Endif */

         /*
         ** Process the ACK
         */
         if ( (flags & ACK) == 0 ) {                          /* FIFTH */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_ACK_EXPECTED++);
            if ( ! seg_okay ) {
               TCP_Must_send_ack(tcb, tcp_cfg);
            } /* Endif */
            TCP_PP_DROP;
         } /* Endif */

         switch ( tcb->state ) {

            case SYN_RECEIVED:

               if ( LT32(ack, tcb->snduna) ||
                  GT32(ack, tcb->sndnxt) )
               {

                  TCP6_Send_reply_reset(
                                 tcb,
                                &ipv6_source, /* source address */
                                 seg,         /* received packet */
                                 seglen,      /* length of received packet */
                                 tcp_cfg,     /* TCP layer data */
                                 tcb->bypass_tx_chksum
                                );
                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
                  TCP_PP_DROP;

               } /* Endif */

               tcb->state = ESTABLISHED;
#if RTCSCFG_TCP_MAX_HALF_OPEN
               TCB_HALF_OPEN_DROP;
#endif

               TCP_Update_send_window(tcb, seg);
               TCP_Return_open(tcb, RTCS_OK, tcp_cfg);
               TCP_Event(tcb, TCPS_STARTED);

               /*FALLTHROUGH*/

            case ESTABLISHED:
            case CLOSE_WAIT:
            case FINWAIT_1:
            case FINWAIT_2:
            case CLOSING:

               if ( GT32(ack, tcb->sndnxt) ) {
                  /*
                  ** ACK of unsent data
                  */
                  TCP_Must_send_ack(tcb, tcp_cfg);
                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
                  TCP_PP_DROP;
               } /* Endif */

               if ( GE32(ack, tcb->snduna) ) {

                  /*
                  ** Update send window
                  */
                  if ( LT32(tcb->sndwl1, seq) ||
                     (tcb->sndwl1 == seq && LE32(tcb->sndwl2,ack))
                  ) {
                     TCP_Update_send_window(tcb, seg);
                  } /* Endif */

                  TCP_Process_ack(tcb, seg, tcp_cfg);

               } /* Endif */

               switch ( tcb->state ) {

                  case FINWAIT_1:
                     /*
                     ** if FIN is acked...
                     */
                     if ( GT32(ack, tcb->sndbufseq + tcb->sndlen) ) {
                        tcb->state = FINWAIT_2;
                     } else {
                        break;
                     } /* Endif */

                     /*FALLTHROUGH*/

                  case FINWAIT_2:
                     /*
                     ** acknowledge Close() if transmit queue empty
                     **  (should already be empty)
                     */
                     TCP_Event(tcb, TCPS_FINACKED);
                     break;

                  case CLOSING:
                     /* if FIN is acked... */
                     if ( GT32(ack, tcb->sndbufseq + tcb->sndlen) ) {
                        tcb->state = TIME_WAIT;
                        tcb->sndxmitwhat = TCP_IDLE;  /* Ensure we're not probing */

                        result =
                           TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                                       tcb->timewaitto,     /* the timeout */
                                       0,                   /* no reload */
                                       tcp_cfg->qhead );    /* timer q head */
                        if ( result != RTCS_OK ) {
                           RTCS_log_error( ERROR_TCP,
                                                RTCSERR_TCP_TIMER_FAILURE,
                                                (uint_32)result,
                                                (uint_32)tcb,
                                                2 );
                        } /* Endif */
                     } else {
                        IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
                        TCP_PP_DROP;
                     } /* Endif */
                     break;

                  default:
                     break;

               } /* End Switch */

               break;

            case LAST_ACK:
               /*
               ** if FIN is acked...
               */
               if ( ack == tcb->sndnxt ) {  /* all data acked */
                  TCP_Event(tcb, TCPS_FINACKED);
                  TCP_Close_TCB(tcb, RTCS_OK, tcp_cfg);
                  TCP_Process_release(tcb, tcp_cfg);
                  TCP_PP_DROP;
               } /* Endif */
               break;

            case TIME_WAIT:
               break;

         } /* End Switch */

   step6:                                                   /* SIXTH */

         switch ( tcb->state ) {

            case ESTABLISHED:
            case FINWAIT_1:
            case FINWAIT_2:

               /*
               ** Check URG
               */
               if ( (flags & URG) != 0 ) {
                  if ( (tcb->status & TCPS_RURGENT) == 0 ||
                     GE32(seq + urg, tcb->rcvup)
                  ) {
                     tcb->rcvup = seq + urg + 1;
                     TCP_Event(tcb, TCPS_RURGENT);

                  } /* Endif */
               } /* Endif */

               if ( ! seg_okay ) { /* Handled all URG/RST/ACK so drop it */
                  TCP_Must_send_ack(tcb, tcp_cfg);
                  TCP_PP_DROP;
               } /* Endif */

               TCP_Process_data(tcb, tcp_cfg, seg, pcb);         /* SEVENTH */
               break;

            default:
               break;

         } /* End Switch */

         if ( ! seg_okay ) { /* Handled all URG/RST/ACK so drop it */
            TCP_Must_send_ack(tcb, tcp_cfg);
            TCP_PP_DROP;
         } /* Endif */

         switch ( tcb->state ) {

            /*
            ** do not process FIN for these states:
            */
            case SYN_SENT:
               TCP_PP_DROP;

            default:
               break;

         } /* End Switch */

         if ( (flags & FIN) != 0 ) {                                /* EIGHTH */

            /*
            ** Record receipt of FIN, if this is the first time we get it
            */
            if ( (tcb->status & TCPS_FINRECV) == 0 ) {
               tcb->rcvnxt++;
            } /* Endif */

            /*
            ** Signal 'connection closing' (receiving side)
            */
            TCP_Event(tcb,TCPS_FINRECV);

            /*
            ** FIN implies PUSHing undelivered received data to user,
            **   So terminate any Receive requests
            */
            while ( tcb->rcvHead != NULL ) {

               TCP_Reply_receive(tcb, tcp_cfg, RTCSERR_TCP_CONN_CLOSING);

               /*
               ** Setup next receive
               */
               while ( tcb->rcvHead != NULL &&
                     ! TCP_Setup_receive(tcb->rcvHead, tcb, tcp_cfg)
               ) {
               } /* Endwhile */

            } /* Endwhile */

            /*
            ** Notify socket layer in case some task is blocked on select()
            ** waiting for data on this socket.
            */
            SOCK_select_signal(tcb->SOCKET, tcb->state);
            tcb->rcvwndedge = tcb->rcvnxt;   /* zero receive window */

            TCP_Must_send_ack(tcb, tcp_cfg);       /* ACK the FIN */

            if (tcb->keepaliveto) {
               tcb->keepaliveto = 0;
            }

            switch ( tcb->state ) {

               case SYN_RECEIVED:
               case ESTABLISHED:
                  tcb->state = CLOSE_WAIT;
                  break;

               case FINWAIT_1:
                  /*
                  ** if FIN is ACKed
                  */
                  if ( GT32(ack, tcb->sndbufseq + tcb->sndlen) ) {

                     /*FALLTHROUGH*/

                  } else {
                     tcb->state = CLOSING;
                     TCP_PP_DROP;
                  } /* Endif */

                  /*FALLTHROUGH*/

               case FINWAIT_2:
                  tcb->state = TIME_WAIT;
                  tcb->sndxmitwhat = TCP_IDLE;  /* Ensure we're not probing */

                  result = TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                                       tcb->timewaitto,     /* the timeout */
                                       0,                   /* no reload */
                                       tcp_cfg->qhead );    /* queue head */
                  if ( result != RTCS_OK ) {
                     RTCS_log_error( ERROR_TCP, RTCSERR_TCP_TIMER_FAILURE,
                        (uint_32)result, (uint_32)tcb, 3 );
                  } /* Endif */
                  TCP_PP_DROP;

               default:
                  break;

            } /* End Switch */

         } /* Endif */

         RTCSLOG_PCB_FREE(pcb, RTCS_OK);
         RTCSPCB_free(pcb);

         RTCSLOG_FNX3(RTCSLOG_FN_TCP_Process_packet, tcb, 0);

         return;


   } /* End Switch */

   RTCSLOG_FNX3(RTCSLOG_FN_TCP_Process_packet, tcb, 1);

   return;

   tcp_pp_drop:
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      RTCSPCB_free(pcb);

      RTCSLOG_FNX3(RTCSLOG_FN_TCP_Process_packet, tcb, 2);

      return;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP6 */











/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Process_packet
* Returned Values : None.
* Comments        :
*
*  Process an incoming TCP packet.  Note that the PCB is freed by
*   the routine it is consumed in.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void TCP_Process_packet
   (
      TCB_STRUCT_PTR       tcb,     /* IN/OUT - TCP context */
      RTCSPCB_PTR          pcb,     /* IN/OUT - the incoming packet */
      TCP_CFG_STRUCT_PTR   tcp_cfg  /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   _ip_address       source;
   TCP_HEADER_PTR    seg;
   uint_16           seglen;
   uint_16           flags;
   uint_16           optlen;
   uchar _PTR_       opt;
   uint_32           seq,ack;
   uint_16           urg;
   int_32            result;     /* returned result */
   boolean           seg_okay;

   seg_okay = TRUE;

   seg = (TCP_HEADER _PTR_)RTCSPCB_DATA(pcb);
   flags = ntohs(seg->flags);

   RTCSLOG_FNE5(RTCSLOG_FN_TCP_Process_packet, tcb, pcb, tcp_cfg, flags);

   RTCSPCB_next(pcb, (flags & DATAOFS_MASK) >> (DATAOFS_SHIFT - 2));

   /*
   ** Drop packets that have illegal/abnormal flag combinations, as these
   ** are almost certainly malicious.
   **
   ** - SYN FIN is probably the best known illegal combination. Remember
   **   that SYN is used to start a connection, while FIN is used to end
   **   an existing connection. It is nonsensical to perform both actions
   **   at the same time. Many scanning tools use SYN FIN packets, because
   **   many intrusion detection systems did not catch these in the past,
   **   although most do so now. You can safely assume that any SYN FIN
   **   packets you see are malicious.
   **
   ** - SYN FIN PSH, SYN FIN RST, SYN FIN RST PSH, and other variants on
   **   SYN FIN also exist. These packets may be used by attackers who are
   **   aware that intrusion detection systems may be looking for packets
   **   with just the SYN and FIN bits set, not additional bits set. Again,
   **   these are clearly malicious.
   **
   ** - Packets should never contain just a FIN flag. FIN packets are
   **   frequently used for port scans, network mapping and other stealth
   **   activities.
   **
   ** - Some packets have absolutely no flags set at all; these are referred
   **   to as "null" packets. It is illegal to have a packet with no flags set.
   */
   if ( ((flags & SYN) && (flags & FIN)) ||
        (flags == FIN) ||
        (flags == 0) ) {
      TCP_PP_DROP;
   }

   /*
   ** compute SEG.LEN
   */
   seglen = RTCSPCB_SIZE(pcb);   /* socket layer data size */
   if ( flags & SYN ) {
      seglen++;
   } /* Endif */

   if ( flags & FIN ) {
      seglen++;
   } /* Endif */

   source = IP_source(pcb);

   /*
   ** Process options
   */
   opt = (uchar _PTR_)seg + sizeof(TCP_HEADER);
   while ( opt < RTCSPCB_DATA(pcb) ) {

      switch ( (ntohc(opt)) ) {

         case OPT_PAD:
            opt = RTCSPCB_DATA(pcb);
            break;

         case OPT_NOOP:
            opt++;
            break;

         case OPT_MSS:

            { /* Scope */

               uint_16 mss;

               optlen = ntohc(opt+1);
               if ( optlen == 0 ) {

                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_OPTION++);
                  TCP_PP_DROP;

               } /* Endif */
               mss = ntohs(opt+2);

               /*
               ** Compare with tcb->rcvmss since it is set to
               **  our maximum allowable, and doesn't change:
               */
               if ( tcb->rcvmss != 0 && mss > tcb->rcvmss ) {
                  mss = tcb->rcvmss;
               } /* Endif */

               tcb->sndmss = mss;
               tcb->sndmax = mss; /* minus length of any IP and TCP options; */

               if ( mss < tcb->rcvmax ) {

                  tcb->rcvmax = mss;

                  /*
                  ** Will reply after 1.75*mss bytes have been received.
                  */
                  tcb->rcv2full = mss + (mss>>1) + (mss>>2);

               } /* Endif */

               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_OPT_MSS++);
               opt += optlen;
               break;

            } /* Endscope */

         default:

            /*
            ** All options will have a length byte
            */
            optlen = ntohc(opt+1);
            if ( optlen == 0 ) {

               /*
               ** should send ICMP parameter problem
               */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_OPTION++);
               TCP_PP_DROP;

            } /* Endif */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_OPT_OTHER++);
            opt += optlen;

      } /* End Switch */

   } /* Endwhile */

   /*
   **  Process packet:
   **
   **  (RFC 793 sec.3.9, 'SEGMENT ARRIVES' (pages 65-76);
   **   steps are indicated as FIRST, SECOND, THIRD, etc. as in the RFC)
   */

   seq = ntohl(seg->seq);
   ack = ntohl(seg->ack);
   urg = ntohs(seg->urgent);

   switch ( tcb->state ) {

      /*
      **   This case is executed only when in a invalid state, since
      **   packets aren't queued on CLOSED connections, and closing
      **   connections (see TCP_Close_TCB()) disown their queued packets...
      */
      case CLOSED:
         if ( (flags & RST) == 0 ) {
            TCP_STATE_DEBUG(("state = CLOSED, replying with RST on tcb %p\n", tcb));
            TCP_Send_reply_reset(tcb,
                           source,        /* source address */
                           seg,           /* received packet */
                           seglen,        /* length of received packet */
                           tcp_cfg,       /* TCP layer data */
                           FALSE
                          );
         } /* Endif */
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_LATE_DATA++);
         TCP_PP_DROP;

      case LISTEN:

         if ( (flags & RST) != 0 ) {                         /* FIRST */
            TCP_STATE_DEBUG(("state = LISTEN, heard reset on tcb %p, dropping.\n", tcb));
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            TCP_PP_DROP;
         } /* Endif */

         if ( (flags & ACK) != 0 ) {                         /* SECOND */
            TCP_Send_reply_reset(tcb,
                           source,        /* source address */
                           seg,           /* received packet */
                           seglen,        /* length of received packet */
                           tcp_cfg,       /* TCP layer data */
                           tcb->bypass_tx_chksum
                          );
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
            TCP_PP_DROP;
         } /* Endif */

         if ( (flags & SYN) != 0 ) {                         /* THIRD */
#if RTCSCFG_TCP_MAX_HALF_OPEN
            if ((tcp_cfg->HALF_OPEN_TCB_COUNT >= RTCSCFG_TCP_MAX_HALF_OPEN)
#if RTCSCFG_TCP_MAX_CONNECTIONS
             || (tcp_cfg->CONN_COUNT >= RTCSCFG_TCP_MAX_CONNECTIONS)
#endif
               ) {
               TCP_half_open_TCB_close(tcp_cfg);
            } /* Endif */
#endif

#if RTCSCFG_TCP_MAX_CONNECTIONS
            if (tcp_cfg->CONN_COUNT >= RTCSCFG_TCP_MAX_CONNECTIONS) {
               TCP_PP_DROP;
            } /* Endif */
#endif

            CHECK_SECURITY;
            CHECK_PRECEDENCE;

            tcb = TCP_Clone_tcb(tcb);
            if (tcb == NULL) {
               TCP_PP_DROP;
            } /* Endif */

            if ( tcb->local_host == INADDR_ANY ) {
               tcb->local_host = IP_dest(pcb);
            } /* Endif */

            if ( tcb->remote_host == INADDR_ANY ) {
               tcb->remote_host = source;
               tcb->rcvmss = IP_MTU(tcb->local_host, source)
                  - (uint_16) (IP_HEADSIZE + TCP_HEADSIZE);
                if (tcb->sndmss > tcb->rcvmss)  {
                  tcb->sndmss = tcb->rcvmss;
               }
               tcb->sndmax = tcb->sndmss;
            } /* Endif */

            tcb->remote_port = ntohs(seg->source_port);

            /*
            ** TCP_Start_receiving sets state to SYN_RECEIVED
            */
            TCP_Start_receiving(tcb, seq, tcp_cfg);

            /*
            ** also below, in case SYN_SENT
            */
            TCP_Start_sending(tcb, tcp_cfg);

            /*
            ** Release the packet
            */
            RTCSLOG_PCB_FREE(pcb, RTCS_OK);
            RTCSPCB_free(pcb);

         } else {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_EXPECTED++);
            TCP_PP_DROP;                                    /* FOURTH */

         } /* Endif */

         break;

      case SYN_SENT:

         if ( (flags & ACK) != 0 ) {                                /* FIRST */

            if ( LE32(ack, tcb->sndiss) ||
               GT32(ack, tcb->sndnxt) )
            {

               /*
               ** invalid ACK: drop, and reply if reset
               */
               if ( ! (flags & RST) ) {
                  TCP_STATE_DEBUG(("state = SYN_SENT, heard invalid ACK on tcb %p, sending RST\n", tcb));
                  TCP_Send_reply_reset(tcb,
                                 source,        /* source address */
                                 seg,           /* received packet */
                                 seglen,        /* length of received packet */
                                 tcp_cfg,       /* TCP layer data */
                                 tcb->bypass_tx_chksum
                                );
               } /* Endif */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
               TCP_PP_DROP;

            } /* Endif */

            if ( LT32(ack, tcb->snduna) ) {

               /*
               ** unnacceptable ACK (already ack'ed): drop
               */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
               TCP_PP_DROP;

            } /* Endif */

         } /* Endif */

         if ( (flags & RST) != 0 ) {                                /* SECOND */

             TCP_STATE_DEBUG(("state = SYN_SENT, heard RST on tcb %p\n", tcb));
            if ( (flags & ACK) != 0 ) {
               TCP_STATE_DEBUG(("state = SYN_SENT, heard RST+ACK on tcb %p, closing&dropping\n", tcb));
               TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg);
               TCP_Process_release(tcb, tcp_cfg);
            } else {
               TCP_STATE_DEBUG(("state = SYN_SENT, ACK was not set so not closing & yes dropping on tcb %p\n", tcb));
            }/* Endif */

            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            TCP_PP_DROP;

         } /* Endif */

         CHECK_SECURITY;            /* else reset & DROP */ /* THIRD */

         if ( (flags & ACK) != 0 ) {
            CHECK_EXACT_PRECEDENCE; /* else reset & DROP */
         } else {
            CHECK_PRECEDENCE;       /* may raise; if can't, reset & DROP */
         } /* Endif */

         if ( (flags & SYN) != 0 ) {                         /* FOURTH */

            TCP_Start_receiving(tcb, seq, tcp_cfg);   /* State=>SYN_RCVD */

            if ( (flags & ACK) != 0 ) {
               if ( GE32(ack, tcb->snduna) ) {
                  TCP_Process_ack(tcb, seg, tcp_cfg);
               } /* Endif */
            } /* Endif */

            if ( GT32(tcb->snduna, tcb->sndiss) ) {
               tcb->state = ESTABLISHED;
               TCP_Update_send_window(tcb,seg);
               TCP_Return_open(tcb, RTCS_OK, tcp_cfg);
               TCP_Event(tcb, TCPS_STARTED);
               TCP_Transmit(tcb, tcp_cfg, FALSE, TRUE);
               goto step6;             /* named from RFC793... */

            } else {
               /* force retransmit of SYN */
               TCP_Transmit(tcb, tcp_cfg, TRUE, TRUE);

            } /* Endif */

            /*
            **  any data or other controls are ignored, not queued
            **  for processing after state becomes ESTABLISHED
            */

            /*
            ** Free the packet
            */
            RTCSLOG_PCB_FREE(pcb, RTCS_OK);
            RTCSPCB_free(pcb);

         } else {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_EXPECTED++);
            TCP_PP_DROP;                                    /* FIFTH */

         } /* Endif */

         break;

      /* all included in default
      ** case SYN_RECEIVED:
      ** case ESTABLISHED:
      ** case FINWAIT_1:
      ** case FINWAIT_2:
      ** case CLOSE_WAIT:
      ** case CLOSING:
      ** case LAST_ACK:
      ** case TIME_WAIT:
      */
      default:

        /**
         ** From RFC 793:
         **
         ** There are four cases for the acceptability test for an incoming
         ** segment:
         **
         ** Segment Receive  Test
         ** Length  Window
         ** ------- -------  -------------------------------------------
         **
         **    0       0     SEG.SEQ = RCV.NXT
         **
         **    0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
         **
         **   >0       0     not acceptable
         **
         **   >0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
         **               or RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND
         **
         ** If the RCV.WND is zero, no segments will be acceptable, but
         ** special allowance should be made to accept valid ACKs, URGs and
         ** RSTs.
         **
         ** If an incoming segment is not acceptable, an acknowledgment
         ** should be sent in reply (unless the RST bit is set, if so drop
         ** the segment and return):
         **
         **   <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
         **
         ** After sending the acknowledgment, drop the unacceptable segment
         ** and return.
         */

         /*
         ** Handles all cases explicitly.  seg_okay is used later in this procedure
         ** to drop an un-acceptable segment after processing its RST/ACK/URG fields
         ** in the case where the window is 0. Note, seglen may have been set to 1 to
         ** handle SYN or FIN flags.
         */
         { /* Scope */
            uint_16  rwindow;
            boolean  okay;

            rwindow = tcb->rcvwndedge - tcb->rcvnxt;

            if ( (rwindow == 0) &&                     /* Case 1 */
               (seglen == 0 || (flags & SYN && seglen == 1))  &&
               (seq == tcb->rcvnxt))
            {
               okay = TRUE;
               seg_okay = TRUE;

            } else if ( (rwindow > 0) &&                 /* Case 2 */
                  (seglen == 0 || (flags & SYN && seglen == 1))  &&
                   (LE32(tcb->rcvnxt,seq) &&
                    LT32(seq,tcb->rcvwndedge)))
            {
               okay = TRUE;
               seg_okay = TRUE;

            } else if ((flags & FIN) &&
                       LT32(tcb->rcvnxt,seq))
            {
              okay = TRUE;
              seg_okay = FALSE;

            } else if ( (seglen > 0)  &&                 /* Case 4 */
                   (rwindow > 0) &&
                   ((LE32(tcb->rcvnxt,seq) &&
                     LT32(seq,tcb->rcvwndedge)) ||
                    (LE32(tcb->rcvnxt,seq+seglen-1) &&
                     LT32(seq+seglen-1,tcb->rcvwndedge))))
            {
               okay = TRUE;
               seg_okay = TRUE;

            } else {                                      /* Case 3 or others failed */
               seg_okay = FALSE;
               if ( (rwindow == 0) && (flags & (RST | URG | ACK)) != 0 ) {
                  RTCSPCB_SIZE(pcb) = 0;
                  okay = TRUE;
                  /*tcp_cfg->STATS.ST_RX_ACC_BADSEGS++;*/
               } else {
                  okay = FALSE;
               } /* Endif */

            } /* Endif */

            if ( ! okay ) {
               /*
               ** segment unacceptable, sent ACK
               */
               if ( (flags & RST) == 0 ) {
                  TCP_Must_send_ack(tcb, tcp_cfg);
               } /* Endif */
               IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_DATA++);
               TCP_PP_DROP;
            } /* Endif */

         } /* Endscope */

         /*
         ** Process the RST
         */
         if ( (flags & RST) != 0 ) {                                /* SECOND */
            TCP_STATE_DEBUG(("state = default(%d), heard reset on tcb %p, ", tcb->state, tcb));

            /* draft-ietf-tcpm-tcpsecure-00.txt section 2.2 */
            if (tcb->tcpsecuredraft0) {
                if (seq == tcb->rcvnxt) {
                   TCP_STATE_DEBUG(("RST is valid, closing&dropping.\n"));
                   TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg); /* aborted */
                   TCP_Process_release(tcb, tcp_cfg);
                   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
                   TCP_PP_DROP;
                }
                else
                {
                   TCP_STATE_DEBUG(("RST is suspicious, acking&dropping\n"));
                   TCP_Must_send_ack(tcb, tcp_cfg);
                   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
                   TCP_PP_DROP;
                } /* Endif */
            } else {

            TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg); /* aborted */
            TCP_Process_release(tcb, tcp_cfg);
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            TCP_PP_DROP;

            }
            TCP_STATE_DEBUG(("\n"));
         } /* Endif */

         if ( tcb->state == SYN_RECEIVED ) {                 /* THIRD */
            CHECK_SECURITY; /* else reset & DROP */
         } else {
            CHECK_TIGHT_SECURITY;
            /*
            ** else reset & TCP_Close_TCB(tcb,RTCSERR_TCP_CONN_RESET) & DROP
            */
         } /* Endif */

         /*
         ** Process the SYN
         */
         if ( (flags & SYN) != 0 ) {                         /* FOURTH */
             if (tcb->tcpsecuredraft0) {
             /*
             ** draft-ietf-tcpm-tcpsecure-00.txt section 3.2
             */
                if (seq == tcb->rcvnxt) { /* SYN and seq is exact match */
                   tcb->ackmodifier = 1; /* ack number of ack pkt will be decremented by 1 later */
                }
                else /* SYN and seq is acceptable even though not exact */
                {
                   tcb->ackmodifier = 0;
                }
                TCP_Must_send_ack(tcb, tcp_cfg);
                IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_NOT_EXPECTED++);
                TCP_PP_DROP;
             }
             else
             {
                  /*
                  ** shouldn't receive SYN
                  */
               TCP_Send_reply_reset(tcb,
                              source,   /* source address */
                               seg,     /* received packet */
                               seglen,  /* length of received packet */
                               tcp_cfg, /* TCP layer data */
                               tcb->bypass_tx_chksum
                              );
                if ( tcb->state != LISTEN ) {

                   TCP_Close_TCB(tcb, RTCSERR_TCP_CONN_RESET, tcp_cfg); /* state=>CLOSED */
                   if (tcb->SOCKET == 0) {
                      TCP_Process_release(tcb, tcp_cfg);
                   } /* Endif */

                } /* Endif */
                IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_SYN_NOT_EXPECTED++);
                TCP_PP_DROP;
             } /* Endif */
         } /* Endif */

         /*
         ** Process the ACK
         */
         if ( (flags & ACK) == 0 ) {                          /* FIFTH */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_ACK_EXPECTED++);
            if ( ! seg_okay ) {
               TCP_Must_send_ack(tcb, tcp_cfg);
            } /* Endif */
            TCP_PP_DROP;
         } /* Endif */

         switch ( tcb->state ) {

            case SYN_RECEIVED:

               if ( LT32(ack, tcb->snduna) ||
                  GT32(ack, tcb->sndnxt) )
               {

                  TCP_Send_reply_reset(tcb,
                                 source,        /* source address */
                                 seg,     /* received packet */
                                 seglen,  /* length of received packet */
                                 tcp_cfg, /* TCP layer data */
                                 tcb->bypass_tx_chksum
                                );
                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
                  TCP_PP_DROP;

               } /* Endif */

               tcb->state = ESTABLISHED;
#if RTCSCFG_TCP_MAX_HALF_OPEN
               TCB_HALF_OPEN_DROP;
#endif

               TCP_Update_send_window(tcb, seg);
               TCP_Return_open(tcb, RTCS_OK, tcp_cfg);
               TCP_Event(tcb, TCPS_STARTED);

               /*FALLTHROUGH*/

            case ESTABLISHED:
            case CLOSE_WAIT:
            case FINWAIT_1:
            case FINWAIT_2:
            case CLOSING:

               if ( GT32(ack, tcb->sndnxt) ) {
                  /*
                  ** ACK of unsent data
                  */
                  TCP_Must_send_ack(tcb, tcp_cfg);
                  IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
                  TCP_PP_DROP;
               } /* Endif */

               if ( GE32(ack, tcb->snduna) ) {

                  /*
                  ** Update send window
                  */
                  if ( LT32(tcb->sndwl1, seq) ||
                     (tcb->sndwl1 == seq && LE32(tcb->sndwl2,ack))
                  ) {
                     TCP_Update_send_window(tcb, seg);
                  } /* Endif */

                  TCP_Process_ack(tcb, seg, tcp_cfg);

               } /* Endif */

               switch ( tcb->state ) {

                  case FINWAIT_1:
                     /*
                     ** if FIN is acked...
                     */
                     if ( GT32(ack, tcb->sndbufseq + tcb->sndlen) ) {
                        tcb->state = FINWAIT_2;
                     } else {
                        break;
                     } /* Endif */

                     /*FALLTHROUGH*/

                  case FINWAIT_2:
                     /*
                     ** acknowledge Close() if transmit queue empty
                     **  (should already be empty)
                     */
                     TCP_Event(tcb, TCPS_FINACKED);
                     break;

                  case CLOSING:
                     /* if FIN is acked... */
                     if ( GT32(ack, tcb->sndbufseq + tcb->sndlen) ) {
                        tcb->state = TIME_WAIT;
                        tcb->sndxmitwhat = TCP_IDLE;  /* Ensure we're not probing */

                        result =
                           TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                                       tcb->timewaitto,     /* the timeout */
                                       0,                   /* no reload */
                                       tcp_cfg->qhead );    /* timer q head */
                        if ( result != RTCS_OK ) {
                           RTCS_log_error( ERROR_TCP,
                                                RTCSERR_TCP_TIMER_FAILURE,
                                                (uint_32)result,
                                                (uint_32)tcb,
                                                2 );
                        } /* Endif */
                     } else {
                        IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_ACK++);
                        TCP_PP_DROP;
                     } /* Endif */
                     break;

                  default:
                     break;

               } /* End Switch */

               break;

            case LAST_ACK:
               /*
               ** if FIN is acked...
               */
               if ( ack == tcb->sndnxt ) {  /* all data acked */
                  TCP_Event(tcb, TCPS_FINACKED);
                  TCP_Close_TCB(tcb, RTCS_OK, tcp_cfg);
                  TCP_Process_release(tcb, tcp_cfg);
                  TCP_PP_DROP;
               } /* Endif */
               break;

            case TIME_WAIT:
               break;

         } /* End Switch */

   step6:                                                   /* SIXTH */

         switch ( tcb->state ) {

            case ESTABLISHED:
            case FINWAIT_1:
            case FINWAIT_2:

               /*
               ** Check URG
               */
               if ( (flags & URG) != 0 ) {
                  if ( (tcb->status & TCPS_RURGENT) == 0 ||
                     GE32(seq + urg, tcb->rcvup)
                  ) {
                     tcb->rcvup = seq + urg + 1;
                     TCP_Event(tcb, TCPS_RURGENT);

                  } /* Endif */
               } /* Endif */

               if ( ! seg_okay ) { /* Handled all URG/RST/ACK so drop it */
                  TCP_Must_send_ack(tcb, tcp_cfg);
                  TCP_PP_DROP;
               } /* Endif */

               TCP_Process_data(tcb, tcp_cfg, seg, pcb);         /* SEVENTH */
               break;

            default:
               break;

         } /* End Switch */

         if ( ! seg_okay ) { /* Handled all URG/RST/ACK so drop it */
            TCP_Must_send_ack(tcb, tcp_cfg);
            TCP_PP_DROP;
         } /* Endif */

         switch ( tcb->state ) {

            /*
            ** do not process FIN for these states:
            */
            case SYN_SENT:
               TCP_PP_DROP;

            default:
               break;

         } /* End Switch */

         if ( (flags & FIN) != 0 ) {                                /* EIGHTH */

            /*
            ** Record receipt of FIN, if this is the first time we get it
            */
            if ( (tcb->status & TCPS_FINRECV) == 0 ) {
               tcb->rcvnxt++;
            } /* Endif */

            /*
            ** Signal 'connection closing' (receiving side)
            */
            TCP_Event(tcb,TCPS_FINRECV);

            /*
            ** FIN implies PUSHing undelivered received data to user,
            **   So terminate any Receive requests
            */
            while ( tcb->rcvHead != NULL ) {

               TCP_Reply_receive(tcb, tcp_cfg, RTCSERR_TCP_CONN_CLOSING);

               /*
               ** Setup next receive
               */
               while ( tcb->rcvHead != NULL &&
                     ! TCP_Setup_receive(tcb->rcvHead, tcb, tcp_cfg)
               ) {
               } /* Endwhile */

            } /* Endwhile */

            /*
            ** Notify socket layer in case some task is blocked on select()
            ** waiting for data on this socket.
            */
            SOCK_select_signal(tcb->SOCKET, tcb->state);
            tcb->rcvwndedge = tcb->rcvnxt;   /* zero receive window */

            TCP_Must_send_ack(tcb, tcp_cfg);       /* ACK the FIN */

            if (tcb->keepaliveto) {
               tcb->keepaliveto = 0;
            }

            switch ( tcb->state ) {

               case SYN_RECEIVED:
               case ESTABLISHED:
                  tcb->state = CLOSE_WAIT;
                  break;

               case FINWAIT_1:
                  /*
                  ** if FIN is ACKed
                  */
                  if ( GT32(ack, tcb->sndbufseq + tcb->sndlen) ) {

                     /*FALLTHROUGH*/

                  } else {
                     tcb->state = CLOSING;
                     TCP_PP_DROP;
                  } /* Endif */

                  /*FALLTHROUGH*/

               case FINWAIT_2:
                  tcb->state = TIME_WAIT;
                  tcb->sndxmitwhat = TCP_IDLE;  /* Ensure we're not probing */

                  result = TCP_Timer_start( &tcb->sndxmittq.t,   /* the timer */
                                       tcb->timewaitto,     /* the timeout */
                                       0,                   /* no reload */
                                       tcp_cfg->qhead );    /* queue head */
                  if ( result != RTCS_OK ) {
                     RTCS_log_error( ERROR_TCP, RTCSERR_TCP_TIMER_FAILURE,
                        (uint_32)result, (uint_32)tcb, 3 );
                  } /* Endif */
                  TCP_PP_DROP;

               default:
                  break;

            } /* End Switch */

         } /* Endif */

         RTCSLOG_PCB_FREE(pcb, RTCS_OK);
         RTCSPCB_free(pcb);

         RTCSLOG_FNX3(RTCSLOG_FN_TCP_Process_packet, tcb, 0);

         return;


   } /* End Switch */

   RTCSLOG_FNX3(RTCSLOG_FN_TCP_Process_packet, tcb, 1);

   return;

   tcp_pp_drop:
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      RTCSPCB_free(pcb);

      RTCSLOG_FNX3(RTCSLOG_FN_TCP_Process_packet, tcb, 2);

      return;

} /* Endbody */
#endif //RTCSCFG_ENABLE_IP4



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP6_Service_packet
* Returned Values : int_32
* Comments        :
*
*  Process incoming TCP6 packets, possibly forward them
*   to the TCP6 server.  This is called from IP6.
*
*  Checks:
*     - for multicast packets (discards)
*     - for invalid source address
*     - that a valid TCB exists for this packet
*     - for bad checksums
*     - for TCB in CLOSED state
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6

void TCP6_Service_packet
   (
      RTCSPCB_PTR    pcb,     /* IN/OUT - the receive packet */
      void _PTR_     cfg      /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   TCP_HEADER _PTR_     seg;
   int_32               chksum;
   uint_16              seglen;
   uint_16              flags;
   uint_16              dataofs;
   TCB_STRUCT_PTR       tcb;
   TCP_CFG_STRUCT_PTR   tcp_cfg;   /* TCP layer constants */

      /*IPv6 part************/
    in6_addr                ipv6source;
    in6_addr                ipv6dest;

   /**/
    uchar in_family=0;
   /**/

   RTCSLOG_FNE3(RTCSLOG_FN_TCP_Service_packet, pcb, cfg);

   tcp_cfg = (TCP_CFG_STRUCT_PTR)cfg;
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_TOTAL++);

   seg = (TCP_HEADER _PTR_)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);

   /* Extracting family type from packet*/
   in_family = ntohc((pcb)->NETWORK_LAYER);
   in_family = (in_family&0xF0)>>4;

   /* using IPv6 copy macros to copy the IPv6 address structure */
   IN6_ADDR_COPY((in6_addr *)((IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->SOURCE, &ipv6source);
   IN6_ADDR_COPY((in6_addr *)((IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->DEST, &ipv6dest);
   if (IN6_IS_ADDR_MULTICAST(&ipv6dest))
   {  /* also check for invld dest addr */
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_MULTICASTS++);
      RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
      TCP_DROP;
   } /* Endif */
   if (IN6_ARE_ADDR_EQUAL(&ipv6source, &in6addr_any))
   {
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_SOURCE++);
      RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
      TCP_DROP;
   } /* Endif */

   flags = ntohs(seg->flags);

   /*
   ** Find a receiving TCB;
   **  Assuming this is much quicker than computing the checksum, do this
   **   first; this way we might not have to compute checksum for packets
   **   we don't want (but may have to reply a RST anyway)...
   */
   { /* Scope */
      register uint_16        rport, dport, sport;
      in6_addr                ipv6_rhost;
      in6_addr                ipv6_lhost;
      TCB_STRUCT_PTR          tcbany = NULL;
      TCB_STRUCT_PTR          tcbspec = NULL;

      dport = ntohs(seg->dest_port);
      sport = ntohs(seg->source_port);

      for ( tcb = tcp_cfg->TCBhead; tcb != NULL; tcb = tcb->next ) {

         rport = tcb->remote_port;
         /* if we inside TCP6_servece_packet function, we need set family type to AF_INET6  */
         /* it will be used for  transmit answer                                            */
      IN6_ADDR_COPY(&tcb->remote_ipv6_host, &ipv6_rhost);
      IN6_ADDR_COPY(&tcb->local_ipv6_host, &ipv6_lhost);

         if (       tcb->local_port != dport            ||
                    (rport != 0 && rport != sport)      ||
                    (
                        (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &in6addr_any)) &&
                        (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &ipv6source))
                    )                                   ||
                    (
                        (!IN6_ARE_ADDR_EQUAL(&ipv6_lhost, &in6addr_any)) &&
                        (!IN6_ARE_ADDR_EQUAL(&ipv6_lhost, &ipv6dest))
                    )                                   ||
                    tcb->state == BOUND
            )
         {
            continue;
         } /* Endif */

         /*
         ** Found candidate TCB...
         */
         if(rport != 0 && (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &in6addr_any)))
         {   /* is conn'n all specified? */
            if(tcb->state == TIME_WAIT)
            {
               tcbspec = NULL;
               tcbany = NULL;
               TCP_STATE_DEBUG(("setting RST flag in %p\n", tcb));
               flags |= RST;
               htons(seg->flags, flags);
               break;
            }
            else
            {
               tcbspec = tcb;
               continue;
            }
         } /* Endif */
         /*
         ** Connection not completely specified in TCB, there might be a TCB
         **   which is more specific;
         **
         **  Give priority to TCB's which have specified one of rport
         **   or rhost, over a TCB which specified neither:
         */
         if(
                (
                    rport != 0 ||
                    (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &in6addr_any)) ||
                    tcbany == NULL ||
                    (tcb->af == AF_INET6)
                ) &&
                (
                    (tcb->if_scope_id ==0)||
                    (tcb->if_scope_id == ip6_if_get_scope_id(pcb->IFSRC))
                )
           )
         {
            tcbany = tcb;
         } /* Endif */

      } /* Endfor */

      tcb = tcbspec;

      if ( tcb == NULL ) {
         tcb = tcbany;
      } /* Endif */

   } /* Endscope */

   RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);
   if ( tcb == NULL ) {

      /*tcp_cfg->STATS.ST_NOCLIENTS++; */
      if ( flags & RST ) {
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_DISCARDED++);
         RTCSLOG_PCB_FREE(pcb, RTCS_OK);
         TCP_DROP;
      } /* Endif */

      /*
      ** otherwise must reply with a RST (below)...
      */

   } /* Endif */

   /*
   ** Check the checksum unless we're directed to bypass it.
   */
   if(!tcb || !tcb->bypass_rx_chksum)
   {
     /*IP6 use different method for calculation checksum*/
      chksum = IP6_Sum_pseudo(RTCSPCB_SIZE(pcb), pcb);
      chksum = IP_Sum_PCB(chksum, pcb);
      if (chksum != 0xFFFF) {

         /*
         ** bad chksum, discarding packet
         */
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_CHECKSUM++);
         RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_CHECKSUM);
         TCP_DROP;

      } /* Endif */
   } /* Endif */

   /*
   ** skip header
   */
   dataofs = (flags & DATAOFS_MASK) >> (DATAOFS_SHIFT - 2);

   /*
   ** Also check for CLOSED state, because we mustn't queue a packet
   **   on a closed connection (it may be lost).
   */
   if (tcb == NULL || tcb->state == CLOSED || tcb->state == TIME_WAIT) {

      if ( flags & RST ) {
         TCP_STATE_DEBUG(("state = CLOSED || TIME_WAIT, heard reset on tcb %p, dropping.\n", tcb));
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
         RTCSLOG_PCB_FREE(pcb, RTCS_OK);
         TCP_DROP;
      } /* Endif */

      seglen = RTCSPCB_SIZE(pcb) - dataofs;
      if ( flags & SYN ) {
         seglen++;
      } /* Endif */

      if ( flags & FIN ) {
         seglen++;
      } /* Endif */

      TCP6_Send_reply_reset(tcb,
                            &ipv6source,        /* source address */
                             seg,               /* received packet */
                             seglen,            /* length of received packet */
                             tcp_cfg,           /* TCP layer data */
                             tcb ? tcb->bypass_tx_chksum : FALSE
                           );
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_LATE_DATA++);
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      TCP_DROP;
   } /* Endif */

   /*
   ** Packet may now be processed, so send upwards.
   */
   TCP6_Process_packet(tcb, pcb, tcp_cfg);  /* (frees the PCB) */

   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Service_packet, 0);

   return;

   tcp_sp_drop:
      RTCSPCB_free(pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Service_packet, 1);
      return;

} /* Endbody */

#endif //RTCSCFG_ENABLE_IP6


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP_Service_packet
* Returned Values : int_32
* Comments        :
*
*  Process incoming TCP packets, possibly forward them
*   to the TCP server.  This is called from IP.
*
*  Checks:
*     - for multicast packets (discards)
*     - for invalid source address
*     - that a valid TCB exists for this packet
*     - for bad checksums
*     - for TCB in CLOSED state
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void TCP4_Service_packet
   (
      RTCSPCB_PTR    pcb,     /* IN/OUT - the receive packet */
      void _PTR_     cfg      /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   _ip_address          source,
                        dest;
   TCP_HEADER _PTR_     seg;
   int_32               chksum;
   uint_16              seglen;
   uint_16              flags;
   uint_16              dataofs;
   TCB_STRUCT_PTR       tcb;
   TCP_CFG_STRUCT_PTR   tcp_cfg;   /* TCP layer constants */

   RTCSLOG_FNE3(RTCSLOG_FN_TCP_Service_packet, pcb, cfg);

   tcp_cfg = (TCP_CFG_STRUCT_PTR)cfg;
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_TOTAL++);

   seg = (TCP_HEADER _PTR_)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);

   source = IP_source(pcb);
   dest   = IP_dest(pcb);

   if (IN_MULTICAST(dest)) {       /* also check for invld dest addr */
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_MULTICASTS++);
      RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
      TCP_DROP;
   } /* Endif */

   if (source == INADDR_ANY)  {
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_SOURCE++);
      RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
      TCP_DROP;
   } /* Endif */

   flags = ntohs(seg->flags);

   /*
   ** Find a receiving TCB;
   **  Assuming this is much quicker than computing the checksum, do this
   **   first; this way we might not have to compute checksum for packets
   **   we don't want (but may have to reply a RST anyway)...
   */
   { /* Scope */

      register uint_16        rport, dport, sport;
      register _ip_address    rhost, lhost;
      TCB_STRUCT_PTR          tcbany = NULL;
      TCB_STRUCT_PTR          tcbspec = NULL;

      dport = ntohs(seg->dest_port);
      sport = ntohs(seg->source_port);

      for ( tcb = tcp_cfg->TCBhead; tcb != NULL; tcb = tcb->next ) {

         rport = tcb->remote_port;
         rhost = tcb->remote_host;
         lhost = tcb->local_host;

         if ( tcb->local_port != dport ||
             (rport != 0 && rport != sport) ||
             (rhost != INADDR_ANY && rhost != source) ||
             (lhost != INADDR_ANY && lhost != dest) ||
              tcb->state == BOUND)
         {
            continue;
         } /* Endif */

         /*
         ** Found candidate TCB...
         */
         if ( rport != 0 && rhost != INADDR_ANY ) {   /* is conn'n all specified? */
            if (tcb->state == TIME_WAIT) {
               tcbspec = NULL;
               tcbany = NULL;
               TCP_STATE_DEBUG(("setting RST flag in %p\n", tcb));
               flags |= RST;
               htons(seg->flags, flags);
               break;
            } else {
               tcbspec = tcb;
               continue;
            }
         } /* Endif */

         /*
         ** Connection not completely specified in TCB, there might be a TCB
         **   which is more specific;
         **
         **  Give priority to TCB's which have specified one of rport
         **   or rhost, over a TCB which specified neither:
         */
         if ( rport != 0 || rhost != INADDR_ANY || tcbany == NULL || (tcb->af == AF_INET) ) {
            tcbany = tcb;
         } /* Endif */
      } /* Endfor */
      tcb = tcbspec;
      if ( tcb == NULL )
      {
         tcb = tcbany;
      } /* Endif */
   } /* Endscope */

   RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);
   if ( tcb == NULL ) {
      /*tcp_cfg->STATS.ST_NOCLIENTS++; */
      if ( flags & RST ) {
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_DISCARDED++);
         RTCSLOG_PCB_FREE(pcb, RTCS_OK);
         TCP_DROP;
      } /* Endif */

      /*
      ** otherwise must reply with a RST (below)...
      */

   } /* Endif */

   /*
   ** Check the checksum unless we're directed to bypass it.
   */
   if (!tcb || !tcb->bypass_rx_chksum) {
      chksum = IP_Sum_pseudo(RTCSPCB_SIZE(pcb), pcb, -1);
      chksum = IP_Sum_PCB(chksum, pcb);
      if (chksum != 0xFFFF) {

         /*
         ** bad chksum, discarding packet
         */
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_CHECKSUM++);
         RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_CHECKSUM);
         TCP_DROP;

      } /* Endif */
   } /* Endif */

   /*
   ** skip header
   */
   dataofs = (flags & DATAOFS_MASK) >> (DATAOFS_SHIFT - 2);

   /*
   ** Also check for CLOSED state, because we mustn't queue a packet
   **   on a closed connection (it may be lost).
   */
   if (tcb == NULL || tcb->state == CLOSED || tcb->state == TIME_WAIT) {

      if ( flags & RST ) {
         TCP_STATE_DEBUG(("state = CLOSED || TIME_WAIT, heard reset on tcb %p, dropping.\n", tcb));
         IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
         RTCSLOG_PCB_FREE(pcb, RTCS_OK);
         TCP_DROP;
      } /* Endif */

      seglen = RTCSPCB_SIZE(pcb) - dataofs;
      if ( flags & SYN ) {
         seglen++;
      } /* Endif */

      if ( flags & FIN ) {
         seglen++;
      } /* Endif */

      TCP_Send_reply_reset(
                           tcb,
                           source,  /* source address */
                           seg,     /* received packet */
                           seglen,  /* length of received packet */
                           tcp_cfg, /* TCP layer data */
                           tcb ? tcb->bypass_tx_chksum : FALSE
                          );
      IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_LATE_DATA++);
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      TCP_DROP;

   } /* Endif */

   /*
   ** Packet may now be processed, so send upwards.
   */
   TCP_Process_packet(tcb, pcb, tcp_cfg);  /* (frees the PCB) */

   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Service_packet, 0);

   return;

   tcp_sp_drop:
      RTCSPCB_free(pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Service_packet, 1);
      return;

} /* Endbody */

#endif //RTCSCFG_ENABLE_IP4



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCP6_Service_packet
* Returned Values : int_32
* Comments        :
*
*  Process incoming TCP6 or TCP4 packets, possibly forward them
*   to the TCP server.  This function support IPv6  and IPv4.
*
*  Checks:
*     - for multicast packets (discards)
*     - for invalid source address
*     - that a valid TCB exists for this packet
*     - for bad checksums
*     - for TCB in CLOSED state
*
*END*-----------------------------------------------------------------*/
void TCP_Service_packet
   (
      RTCSPCB_PTR    pcb,     /* IN/OUT - the receive packet */
      void _PTR_     cfg      /* IN/OUT - TCP layer constants */
   )
{ /* Body */
   TCP_HEADER _PTR_     seg;
   int_32               chksum;
   uint_16              seglen;
   uint_16              flags;
   uint_16              dataofs;
   TCB_STRUCT_PTR       tcb;
   TCP_CFG_STRUCT_PTR   tcp_cfg;   /* TCP layer constants */

    /* IPv4 part */
#if RTCSCFG_ENABLE_IP4
    _ip_address         ipv4source;
    _ip_address         ipv4dest;
#endif
    /* IPv6 part */
#if RTCSCFG_ENABLE_IP6
    in6_addr            ipv6source;
    in6_addr            ipv6dest;
#endif
   /*-/\-*/
    uchar       in_family   =   0;
    uint_32     error       =   0;
   /*-\/-*/

   RTCSLOG_FNE3(RTCSLOG_FN_TCP_Service_packet, pcb, cfg);

   tcp_cfg = (TCP_CFG_STRUCT_PTR)cfg;
   IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_RX_TOTAL++);

   seg = (TCP_HEADER _PTR_)RTCSPCB_DATA(pcb);
   RTCSPCB_DATA_TRANSPORT(pcb) = RTCSPCB_DATA(pcb);


   /* Extracting family type from packet*/
    in_family = ntohc((pcb)->NETWORK_LAYER);
    in_family = (in_family&0xF0)>>4;

    /* Checking family type*/
    error = 1;
#if RTCSCFG_ENABLE_IP4
    if (in_family == IPV4)
    {
        error = 0;
    }
#endif
#if RTCSCFG_ENABLE_IP6
    if (in_family == IPV6)
    {
        error = 0;
    }
#endif
    if (error)
    {   /* If inet protocol is not IPv4 or IPv6 go out with error. */
        IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_SOURCE++);
        RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
        TCP_DROP;
    }

    /* copy the IP address from packet */
#if RTCSCFG_ENABLE_IP4
    if(in_family == IPV4)
    {
        ipv4source = IP_source(pcb);
        ipv4dest   = IP_dest(pcb);

        if (IN_MULTICAST(ipv4dest))
        {       /* also check for invld dest addr */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_MULTICASTS++);
            RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
            TCP_DROP;
        } /* Endif */
        if (ipv4source == INADDR_ANY)
        {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_SOURCE++);
            RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
            TCP_DROP;
        } /* Endif */

    }
#endif
#if RTCSCFG_ENABLE_IP6
    if(in_family == IPV6)
    {
        IN6_ADDR_COPY((in6_addr *)((IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->SOURCE, &ipv6source);
        IN6_ADDR_COPY((in6_addr *)((IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb))->DEST, &ipv6dest);

        if (IN6_IS_ADDR_MULTICAST(&ipv6dest))
        {       /* also check for invld dest addr */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_MULTICASTS++);
            RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
            TCP_DROP;
        } /* Endif */
        if (IN6_ARE_ADDR_EQUAL(&ipv6source, &in6addr_any))
        {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_SOURCE++);
            RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_HEADER);
            TCP_DROP;
        } /* Endif */

    }
#endif

   flags = ntohs(seg->flags);

   /*
   ** Find a receiving TCB;
   **  Assuming this is much quicker than computing the checksum, do this
   **   first; this way we might not have to compute checksum for packets
   **   we don't want (but may have to reply a RST anyway)...
   */
    { /* Scope */

      register uint_16        rport, dport, sport;
#if RTCSCFG_ENABLE_IP4
      register _ip_address      ipv4_rhost;
      register _ip_address      ipv4_lhost;
#endif

#if RTCSCFG_ENABLE_IP6
      in6_addr                  ipv6_rhost;
      in6_addr                  ipv6_lhost;
#endif

      TCB_STRUCT_PTR          tcbany = NULL;
      TCB_STRUCT_PTR          tcbspec = NULL;

      dport = ntohs(seg->dest_port);
      sport = ntohs(seg->source_port);


#if RTCSCFG_ENABLE_IP4
      if(in_family == IPV4)
      {
        for ( tcb = tcp_cfg->TCBhead; tcb != NULL; tcb = tcb->next )
        {

             /* We need check SOCKET family.If SOCKET was opened for another family,try another socket */
             if((tcb->SOCKET))
             {
                if(((SOCKET_STRUCT_PTR)(tcb->SOCKET))->AF != AF_INET)
                {
                    continue;
                }
             }

            rport = tcb->remote_port;
            /* we need setup family type to AF_INET */
            /* it will be used for  transmit answer */
            tcb->af = AF_INET;

            ipv4_rhost = tcb->remote_host;
            ipv4_lhost = tcb->local_host;

            if (        tcb->local_port != dport            ||
                        (rport != 0 && rport != sport)      ||
                        (
                            ipv4_rhost != INADDR_ANY &&
                            ipv4_rhost != ipv4source
                        )                                   ||
                        (
                            ipv4_lhost != INADDR_ANY &&
                            ipv4_lhost != ipv4dest
                        )                                   ||
                        tcb->state == BOUND
                )
            {
                continue;
            } /* Endif */

            /*
            ** Found candidate TCB...
            */
            if ( rport != 0 && ipv4_rhost != INADDR_ANY )
            {   /* is conn'n all specified? */
                if(tcb->state == TIME_WAIT)
                {
                    tcbspec = NULL;
                    tcbany = NULL;
                    TCP_STATE_DEBUG(("setting RST flag in %p\n", tcb));
                    flags |= RST;
                    htons(seg->flags, flags);
                    break;
                }
                else
                {
                    tcbspec = tcb;
                    continue;
                }
            } /* Endif */

            /*
            ** Connection not completely specified in TCB, there might be a TCB
            **   which is more specific;
            **
            **  Give priority to TCB's which have specified one of rport
            **   or rhost, over a TCB which specified neither:
            */
            if( rport != 0 || ipv4_rhost != INADDR_ANY || tcbany == NULL || (tcb->af == AF_INET))
            {
                tcbany = tcb;
            } /* Endif */
        } /* Endfor */
      }/*End if IPV4*/
#endif

#if RTCSCFG_ENABLE_IP6
      if(in_family == IPV6)
      {
        for ( tcb = tcp_cfg->TCBhead; tcb != NULL; tcb = tcb->next )
        {
            /* We need check SOCKET family.If SOCKET was opened for another family, try another socket. */
            if((tcb->SOCKET))
            {
                if(((SOCKET_STRUCT_PTR)(tcb->SOCKET))->AF != AF_INET6)
                {
                    continue;
                }
            }

            rport = tcb->remote_port;
            /* if we inside TCP6_servece_packet function, we need set family type to AF_INET6  */
            /* it will be used for  transmit answer                                            */
            tcb->af = AF_INET6;
            IN6_ADDR_COPY(&tcb->remote_ipv6_host, &ipv6_rhost);
            IN6_ADDR_COPY(&tcb->local_ipv6_host, &ipv6_lhost);

            if (       tcb->local_port != dport            ||
                        (rport != 0 && rport != sport)      ||
                        (
                            (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &in6addr_any)) &&
                            (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &ipv6source))

                        )                                   ||

                        (
                            (!IN6_ARE_ADDR_EQUAL(&ipv6_lhost, &in6addr_any)) &&
                            (!IN6_ARE_ADDR_EQUAL(&ipv6_lhost, &ipv6dest))

                        )                                   ||
                        tcb->state == BOUND
                )
            {
                continue;
            } /* Endif */

            /*
            ** Found candidate TCB...
            */
            if(rport != 0 && (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &in6addr_any)))
            {   /* is conn'n all specified? */
                if(tcb->state == TIME_WAIT)
                {
                    tcbspec = NULL;
                    tcbany = NULL;
                    TCP_STATE_DEBUG(("setting RST flag in %p\n", tcb));
                    flags |= RST;
                    htons(seg->flags, flags);
                    break;
                }
                else
                {
                    tcbspec = tcb;
                    continue;
                }
            } /* Endif */

            /*
            ** Connection not completely specified in TCB, there might be a TCB
            **   which is more specific;
            **
            **  Give priority to TCB's which have specified one of rport
            **   or rhost, over a TCB which specified neither:
            */
            if( rport != 0 || (!IN6_ARE_ADDR_EQUAL(&ipv6_rhost, &in6addr_any)) || tcbany == NULL || (tcb->af == AF_INET6))
            {
                tcbany = tcb;
            } /* Endif */
        } /* Endfor */
      }/*End if IPV6*/
#endif

        tcb = tcbspec;
        if ( tcb == NULL )
        {
            tcb = tcbany;
        } /* Endif */

    } /* Endscope */

    RTCSLOG_PCB_READ(pcb, RTCS_LOGCTRL_PROTO(IPPROTO_TCP), 0);

    if ( tcb == NULL )
    {
        /*tcp_cfg->STATS.ST_NOCLIENTS++; */
        if ( flags & RST )
        {
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.COMMON.ST_TX_DISCARDED++);
            RTCSLOG_PCB_FREE(pcb, RTCS_OK);
            TCP_DROP;
        }
        /*
        ** otherwise must reply with a RST (below)...
        */

    }
   /*
   ** Check the checksum unless we're directed to bypass it.
   */
    if(!tcb || !tcb->bypass_rx_chksum)
    {

#if RTCSCFG_ENABLE_IP4
        if(in_family == IPV4)
        {
            chksum = IP_Sum_pseudo(RTCSPCB_SIZE(pcb), pcb, -1);
        }
#endif
#if RTCSCFG_ENABLE_IP6
        if(in_family == IPV6)
        {
            /*IP6 use different method for calculation checksum*/
            chksum = IP6_Sum_pseudo(RTCSPCB_SIZE(pcb), pcb);
        }
#endif
        chksum = IP_Sum_PCB(chksum, pcb);
        if (chksum != 0xFFFF)
        {
            /*
            ** bad chksum, discarding packet
            */
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_BAD_CHECKSUM++);
            RTCSLOG_PCB_FREE(pcb, RTCSERR_TCP_BAD_CHECKSUM);
            TCP_DROP;
        } /* Endif */
    } /* Endif */

   /*
   ** skip header
   */
    dataofs = (flags & DATAOFS_MASK) >> (DATAOFS_SHIFT - 2);

   /*
   ** Also check for CLOSED state, because we mustn't queue a packet
   **   on a closed connection (it may be lost).
   */
    if (tcb == NULL || tcb->state == CLOSED || tcb->state == TIME_WAIT)
    {
        if ( flags & RST )
        {
            TCP_STATE_DEBUG(("state = CLOSED || TIME_WAIT, heard reset on tcb %p, dropping.\n", tcb));
            IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_RESET++);
            RTCSLOG_PCB_FREE(pcb, RTCS_OK);
            TCP_DROP;
        } /* Endif */

        seglen = RTCSPCB_SIZE(pcb) - dataofs;
        if( flags & SYN )
        {
            seglen++;
        } /* Endif */

        if( flags & FIN )
        {
            seglen++;
        } /* Endif */

#if RTCSCFG_ENABLE_IP4
        if(in_family == IPV4)
        {

            TCP_Send_reply_reset    (   tcb,
                                        ipv4source,        /* source address */
                                        seg,     /* received packet */
                                        seglen,  /* length of received packet */
                                        tcp_cfg, /* TCP layer data */
                                        tcb ? tcb->bypass_tx_chksum : FALSE
                                    );
        }
#endif
#if RTCSCFG_ENABLE_IP6
        if(in_family == IPV6)
        {

            TCP6_Send_reply_reset   (   tcb,
                                        &ipv6source,        /* source address */
                                        seg,     /* received packet */
                                        seglen,  /* length of received packet */
                                        tcp_cfg, /* TCP layer data */
                                        tcb ? tcb->bypass_tx_chksum : FALSE
                                    );
        }
#endif
        IF_TCP_STATS_ENABLED(tcp_cfg->STATS.ST_RX_LATE_DATA++);
        RTCSLOG_PCB_FREE(pcb, RTCS_OK);
        TCP_DROP;
    } /* Endif */
    /*
    ** Packet may now be processed, so send upwards.
    */

#if RTCSCFG_ENABLE_IP4
    if(in_family == IPV4)
    {
        TCP_Process_packet(tcb, pcb, tcp_cfg);  /* (frees the PCB) */
    }
#endif
#if RTCSCFG_ENABLE_IP6
    if(in_family == IPV6)
    {
        TCP6_Process_packet(tcb, pcb, tcp_cfg);  /* (frees the PCB) */
    }
#endif


   RTCSLOG_FNX2(RTCSLOG_FN_TCP_Service_packet, 0);

   return;

   tcp_sp_drop:
      RTCSPCB_free(pcb);
      RTCSLOG_FNX2(RTCSLOG_FN_TCP_Service_packet, 1);
      return;

} /* Endbody */

/* EOF */
