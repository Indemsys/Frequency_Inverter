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
* $FileName: sstream.c$
* $Version : 3.8.10.0$
* $Date    : Sep-10-2012$
*
* Comments:
*
*   This file contains the socket interface functions for
*   (PF_INET,SOCK_STREAM) sockets.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "socket.h"

#if RTCSCFG_ENABLE_TCP

#define RTCS_ENTER(f,a)    RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_STREAM_ ## f, a)

#define RTCS_EXIT(f,a)     RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_STREAM_ ## f, a); \
                           return a

#define RTCS_EXIT2(f,a,e)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_STREAM_ ## f, a); \
                           return e



uint_32 SOCK_STREAM_socket      (uint_32);

uint_32 SOCK_STREAM_bind      (uint_32, const sockaddr _PTR_, uint_16);
uint_32 SOCK_STREAM_connect     (uint_32, const sockaddr _PTR_, uint_16);
uint_32 SOCK_STREAM_listen      (uint_32, uint_32);
uint_32 SOCK_STREAM_accept      (uint_32, sockaddr _PTR_, uint_16 _PTR_);
uint_32 SOCK_STREAM_getsockname (uint_32, sockaddr _PTR_, uint_16 _PTR_);
uint_32 SOCK_STREAM_getpeername (uint_32, sockaddr _PTR_, uint_16 _PTR_);
 int_32 SOCK_STREAM_recv        (uint_32, pointer, uint_32, uint_32);
 int_32 SOCK_STREAM_send        (uint_32, pointer, uint_32, uint_32);
uint_32 SOCK_STREAM_shutdown    (uint_32, uint_32);



const RTCS_SOCKET_CALL_STRUCT SOCK_STREAM_CALL = {
   SOCK_STREAM_socket,
   SOCK_STREAM_bind,
   SOCK_STREAM_connect,
   SOCK_STREAM_listen,
   SOCK_STREAM_accept,
   SOCK_STREAM_getsockname,
   SOCK_STREAM_getpeername,
   SOCK_STREAM_recv,
   NULL,
   NULL,
   SOCK_STREAM_send,
   NULL,
   NULL,
   NULL,
   SOCK_STREAM_shutdown
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_socket
* Returned Value  : error code
* Comments  :  Creates an unbound socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_STREAM_socket
   (
      uint_32     sock
         /* [IN] socket handle */
   )
{ /* Body */
   TCP_PARM    parms;
   uint_32     error;

   parms.SOCKET = sock;

   error = RTCSCMD_issue(parms, TCP_Process_create);
   if (error) return error;

   ((SOCKET_STRUCT_PTR)sock)->STATE   = SOCKSTATE_STREAM_GROUND;
   ((SOCKET_STRUCT_PTR)sock)->TCB_PTR = parms.TCB_PTR;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name     : SOCK_STREAM_bind
* Returned Value    : error code
* Comments          : Binds the local endpoint of a socket for IPv4 and IPv6 families.
*END*-----------------------------------------------------------------*/
uint_32  SOCK_STREAM_bind
   (
        uint_32                    sock,
         /* [IN] socket handle */
        const sockaddr _PTR_    localaddr,
         /* [IN] IPv4 or IPv6 local address to which to bind the socket */
        uint_16                    addrlen
         /* [IN] length of the address, in bytes */
   )
{ /* Body */
    TCP_PARM    parms;
    uint_32     error=0;

    RTCS_ENTER(BIND, sock);

#if RTCSCFG_CHECK_ERRORS
    if (((SOCKET_STRUCT_PTR)sock)->STATE != SOCKSTATE_STREAM_GROUND) 
    {
        RTCS_EXIT(BIND, RTCSERR_SOCK_IS_BOUND);
    } /* Endif */
#endif

#if RTCSCFG_CHECK_ADDRSIZE
    if((localaddr->sa_family)==AF_INET)
    { 
        if (addrlen < sizeof(sockaddr_in)) 
        {
            RTCS_EXIT(BIND, RTCSERR_SOCK_SHORT_ADDRESS);
        } /* Endif */
    }
#if RTCSCFG_ENABLE_IP6    
    else if((localaddr->sa_family)==AF_INET6)
    { 
        if (addrlen < sizeof(struct sockaddr_in6)) 
        {
            RTCS_EXIT(BIND, RTCSERR_SOCK_SHORT_ADDRESS);
        } /* Endif */
    }
#endif    
    else
    {
        /* wrong inet family */
        RTCS_EXIT(BIND, RTCSERR_SOCK_INVALID_AF);
    }
#endif


#if!RTCSCFG_CHECK_ADDRSIZE
    #if RTCSCFG_CHECK_ERRORS

        #if RTCSCFG_ENABLE_IP6    
            if ((localaddr->sa_family != AF_INET)&&(localaddr->sa_family != AF_INET6)) 
            {
                RTCS_EXIT(BIND, RTCSERR_SOCK_INVALID_AF);
            } /* Endif */
        #else
            if (localaddr->sa_family != AF_INET) 
            {
                RTCS_EXIT(BIND, RTCSERR_SOCK_INVALID_AF);
            } /* Endif */
        #endif

    #endif
#endif

    if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) 
    {
      RTCS_EXIT(BIND, RTCSERR_TCP_CONN_RLSD);
    } /* Endif */

    parms.TCB_PTR  = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR;

#if RTCSCFG_ENABLE_IP6    
    if(localaddr->sa_family == AF_INET)
    {
#endif
        parms.IP_ADDR  = ((sockaddr_in*)localaddr)->sin_addr.s_addr;
        parms.TCP_PORT = ((sockaddr_in *)localaddr)->sin_port; 
                                                               
#if RTCSCFG_ENABLE_IP6                                                                     
    }                                                             
    else if(localaddr->sa_family == AF_INET6)
    {
        IN6_ADDR_COPY((in6_addr*)(&( ((sockaddr_in6 *)localaddr)->sin6_addr )),&(parms.IPV6_ADDR));
        parms.TCP_PORT = ((struct sockaddr_in6 *)localaddr)->sin6_port;//TBD: To take port value here we can use only (sockaddr_in *)localaddr)->sin_port
                                                                      //     but to avoid a problem uncompatibility we are checking family.       
        
        /* set a scope_id of binded address */
        parms.IF_SCOPE_ID = ((struct sockaddr_in6 *)localaddr)->sin6_scope_id;
    }

#endif
    /* set family here to send it to TCP_process_bind */
    parms.AF = localaddr->sa_family;
    
    /* Call main bind function */
    error = RTCSCMD_issue(parms, TCP_Process_bind);
    if (error) 
    {
      RTCS_EXIT(BIND, error);
    } /* Endif */

#if RTCSCFG_ENABLE_IP6    
    if((localaddr->sa_family)==AF_INET)
    {
#endif    

        ((SOCKET_STRUCT_PTR)sock)->LOCAL_PORT = ((sockaddr_in *)localaddr)->sin_port;
#if RTCSCFG_ENABLE_IP6    
    }                                                                                
    else if((localaddr->sa_family)==AF_INET6)
    {
        ((SOCKET_STRUCT_PTR)sock)->LOCAL_PORT = ((struct sockaddr_in6 *)localaddr)->sin6_port;
    }
#endif

    ((SOCKET_STRUCT_PTR)sock)->STATE = SOCKSTATE_STREAM_BOUND;
    RTCS_EXIT(BIND, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name     : SOCK_STREAM_connect
* Returned Value    : error code
* Comments          : Binds the remote endpoint of a socket. IPv4 and IPv6 support.
*END*-----------------------------------------------------------------*/
uint_32  SOCK_STREAM_connect 
   (
      uint_32                   sock,
         /* [IN] socket handle */
      const sockaddr _PTR_      peeraddr,
         /* [IN] remote IPv4 or IPv6 address to which to bind the socket */
      uint_16                   addrlen
         /* [IN] length of the address, in bytes */
   )
{ /* Body */
            TCP_PARM            parms;
            TCP_OPTIONS_STRUCT  options;
   register SOCKET_STRUCT_PTR   socket_ptr = (SOCKET_STRUCT_PTR)sock;
            uint_32             error=0;

   RTCS_ENTER(CONNECT, sock);

#if RTCSCFG_CHECK_ERRORS
    switch (((SOCKET_STRUCT_PTR)sock)->STATE) 
    {
        case SOCKSTATE_STREAM_LISTENING: RTCS_EXIT(CONNECT, RTCSERR_SOCK_IS_LISTENING);
        case SOCKSTATE_STREAM_CONNECTED: RTCS_EXIT(CONNECT, RTCSERR_SOCK_IS_CONNECTED);
    } /* Endswitch */
#endif


    /* Checking right family */
    error=1;
#if RTCSCFG_ENABLE_IP4
    if(peeraddr->sa_family == AF_INET)
    {
        error=0;
    } /* Endif */
#endif
#if RTCSCFG_ENABLE_IP6    
    if(peeraddr->sa_family == AF_INET6) 
    {
        error=0;
    } /* Endif */
#endif    
    if(error)
    {
        RTCS_EXIT(CONNECT, RTCSERR_SOCK_INVALID_AF);    
    }


    /* Checking address lenght */
#if RTCSCFG_CHECK_ADDRSIZE

    #if RTCSCFG_ENABLE_IP6
    if((peeraddr->sa_family)==AF_INET)
    { 
    #endif

        if (addrlen < sizeof(sockaddr_in)) 
        {
            RTCS_EXIT(CONNECT, RTCSERR_SOCK_SHORT_ADDRESS);
        } /* Endif */
        
    #if RTCSCFG_ENABLE_IP6    
    }
    else if((peeraddr->sa_family)==AF_INET6)
    { 
        if (addrlen < sizeof(struct sockaddr_in6)) 
        {
            RTCS_EXIT(CONNECT, RTCSERR_SOCK_SHORT_ADDRESS);
        } /* Endif */
    }
    #endif
        
#endif // end RTCSCFG_CHECK_ADDRSIZE



    if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR)
    {
        RTCS_EXIT(CONNECT, RTCSERR_TCP_CONN_RLSD);
    } /* Endif */

    /* Bind if socket was not binded yet */
    if (((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_STREAM_GROUND) 
    {
        #if RTCSCFG_ENABLE_IP6    
        if((peeraddr->sa_family)==AF_INET)
        {    
        #endif

            sockaddr_in localaddr;
            localaddr.sin_family      = AF_INET;
            localaddr.sin_port        = 0;
            localaddr.sin_addr.s_addr = INADDR_ANY;
            error = SOCK_STREAM_bind(sock, (const sockaddr*)(&localaddr), sizeof(localaddr));                  

        #if RTCSCFG_ENABLE_IP6        
        }
        else if((peeraddr->sa_family)==AF_INET6)
        {    
            sockaddr_in6 localaddr;
            localaddr.sin6_family      = AF_INET6;
            localaddr.sin6_port        = 0;
            /* set scope_id to NULL */
            localaddr.sin6_scope_id    = 0;// Let to IP level to choose interface.
            IN6_ADDR_COPY((in6_addr*)(&in6addr_any), &(localaddr.sin6_addr));
            //localaddr.sin_addr.s_addr = INADDR_ANY;
            error = SOCK_STREAM_bind(sock, (const sockaddr*)(&localaddr), sizeof(localaddr));      
        }
        #endif
    }      
      
    if (error) 
    {
        RTCS_EXIT(CONNECT, error);
    } /* Endif */


   /*
   **  Setup the known 'connect' options
   */
   
    #if RTCSCFG_ENABLE_IP6    
    if((peeraddr->sa_family)==AF_INET)
    {
    #endif    
    
        options.remhost    = ((sockaddr_in *)peeraddr)->sin_addr.s_addr;    
        options.remport    = ((sockaddr_in *)peeraddr)->sin_port;
    
    #if RTCSCFG_ENABLE_IP6    
    }
    else if((peeraddr->sa_family)==AF_INET6)
    {
        IN6_ADDR_COPY((in6_addr*)&(((sockaddr_in6 *)peeraddr)->sin6_addr),&(options.remhost_ipv6)); 
        options.remport    = ((sockaddr_in6 *)peeraddr)->sin6_port;
   /*  
    *  Chose and add scope_id here. 
    *  If peeraddr scope_id is not NULL we will use it.  
    *  If it is NULL, we will use scope_id of local binded address
    */
        parms.IF_SCOPE_ID = ((sockaddr_in6 *)peeraddr)->sin6_scope_id;       
        if(parms.IF_SCOPE_ID ==0)
        {
          //Here if_scope_id is if nember that was binded with local address
            parms.IF_SCOPE_ID =((SOCKET_STRUCT_PTR)sock)->TCB_PTR->if_scope_id;
        }   
    }
    #endif
 //  options.remhost    = peeraddr->sin_addr.s_addr;
 //  IN6_ADDR_COPY((in6_addr*)&(peeraddr->sin6_addr),&(options.remhost_ipv6)); 
 //options.remport    = peeraddr->sin6_port;

   options.utimeout   = socket_ptr->SEND_TIMEOUT;
   options.timeout    = socket_ptr->CONNECT_TIMEOUT;
   options.rto        = socket_ptr->RETRANSMISSION_TIMEOUT;
   options.active     = socket_ptr->ACTIVE;
   options.tbsize     = socket_ptr->TBSIZE;
   options.rbsize     = socket_ptr->RBSIZE;
   options.maxrto     = socket_ptr->MAXRTO;
   options.maxrcvwnd  = socket_ptr->MAXRCV_WND;
   options.keepalive  = socket_ptr->KEEPALIVE;
   options.nowait     = socket_ptr->NOWAIT;
   options.nonagle    = socket_ptr->NO_NAGLE_ALGORITHM;
   options.noswrbuf   = socket_ptr->NOSWRBUF;
   options.timewaitto = socket_ptr->TIMEWAIT_TIMEOUT;
   /* Start CR 1576 */
   options.tcpsecuredraft0 = socket_ptr->TCPSECUREDRAFT_0;
   /* End CR 1576 */
   options.delayackto = socket_ptr->DELAY_ACK;

   /*
   **  Setup the default 'connect' options
   */
   options.tos        = 0;
   options.precede    = 0xFF;
   options.ttl        = 0;

   parms.TCB_PTR     = socket_ptr->TCB_PTR;
   parms.OPTIONS_PTR = &options;
   
   parms.AF = socket_ptr->AF;

   error = RTCSCMD_issue(parms, TCP_Process_open);
   if (error) {
      RTCS_EXIT(CONNECT, error);
   } /* Endif */

   ((SOCKET_STRUCT_PTR)sock)->STATE = SOCKSTATE_STREAM_CONNECTED;
   RTCS_EXIT(CONNECT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_listen
* Returned Value  : error code
* Comments  :  Enables remote connections on a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_STREAM_listen
   (
      uint_32     sock,
         /* [IN] socket handle */
      uint_32     backlog
         /* [IN] not implemented */
   )
{ /* Body */
            TCP_PARM             parms;
            TCP_OPTIONS_STRUCT   options;
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
            uint_32              error=0;

   RTCS_ENTER(LISTEN, sock);

#if RTCSCFG_CHECK_ERRORS
   switch (((SOCKET_STRUCT_PTR)sock)->STATE) {
   case SOCKSTATE_STREAM_GROUND:    RTCS_EXIT(LISTEN, RTCSERR_SOCK_NOT_BOUND);
   case SOCKSTATE_STREAM_LISTENING: RTCS_EXIT(LISTEN, RTCSERR_SOCK_IS_LISTENING);
   case SOCKSTATE_STREAM_CONNECTED: RTCS_EXIT(LISTEN, RTCSERR_SOCK_IS_CONNECTED);
   } /* Endswitch */
#endif

   if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) {
      RTCS_EXIT(LISTEN, RTCSERR_TCP_CONN_RLSD);
   } /* Endif */

   /*
   **  Setup the known 'connect' options
   */
   /*  Checking right family  */
    error=1;
#if RTCSCFG_ENABLE_IP4
    if(socket_ptr->AF == AF_INET) 
    {
        error=0;
    } /* Endif */
#endif
#if RTCSCFG_ENABLE_IP6    
    if(socket_ptr->AF == AF_INET6) 
    {
        error=0;
    } /* Endif */
#endif    
    if(error)
    {
        RTCS_EXIT(LISTEN, RTCSERR_SOCK_INVALID_AF);    
    }
   
   #if RTCSCFG_ENABLE_IP6
    
   if(socket_ptr->AF == AF_INET6 )
   {
        IN6_ADDR_COPY((in6_addr *)&in6addr_any, &options.remhost_ipv6);
        
        /* add scope_id of listen interface here */
        parms.IF_SCOPE_ID = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->if_scope_id;
        
   }
   else if(socket_ptr->AF == AF_INET)
   {
   #endif
        options.remhost    = INADDR_ANY;
   #if RTCSCFG_ENABLE_IP6
   }
   #endif

   options.remport    = 0;
   options.utimeout   = socket_ptr->SEND_TIMEOUT;
   options.timeout    = socket_ptr->CONNECT_TIMEOUT;
   options.rto        = socket_ptr->RETRANSMISSION_TIMEOUT;
   options.active     = FALSE;
   options.tbsize     = socket_ptr->TBSIZE;
   options.rbsize     = socket_ptr->RBSIZE;
   options.maxrto     = socket_ptr->MAXRTO;
   options.maxrcvwnd  = socket_ptr->MAXRCV_WND;
   options.keepalive  = socket_ptr->KEEPALIVE;
   options.nowait     = socket_ptr->NOWAIT;
   options.nonagle    = socket_ptr->NO_NAGLE_ALGORITHM;
   options.noswrbuf   = socket_ptr->NOSWRBUF;
   options.timewaitto = socket_ptr->TIMEWAIT_TIMEOUT;
   options.delayackto = socket_ptr->DELAY_ACK;

   /*
   **  Setup the default 'connect' options
   */
   options.tos        = 0;
   options.precede    = 0xFF;
   options.ttl        = 0;

   parms.TCB_PTR     = socket_ptr->TCB_PTR;
   parms.OPTIONS_PTR = &options;
   parms.SOCKET      = sock;
   parms.AF          = socket_ptr->AF;

   error = RTCSCMD_issue(parms, TCP_Process_open);

   if(error) 
   {
        RTCS_EXIT(LISTEN, error);
   } /* Endif */

   ((SOCKET_STRUCT_PTR)sock)->STATE = SOCKSTATE_STREAM_LISTENING;
   
   RTCS_EXIT(LISTEN, RTCS_OK);
} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_accept
* Returned Value  : socket
* Comments  :  Wait for a connection on a socket, support IPv4 and IPv6.
*
*END*-----------------------------------------------------------------*/
uint_32  SOCK_STREAM_accept
   (
             uint_32                sock,
         /* [IN] socket handle */
      struct sockaddr _PTR_     peeraddr,
         /* [OUT] remote address IPv6 to which the socket is connected */
             uint_16     _PTR_      addrlen
         /* [IN/OUT] length of the address IPv6, in bytes */
   )
{ /* Body */
   TCP_PARM             parms;
   SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
   SOCKET_STRUCT_PTR    new_socket_ptr;
   uint_32              error;
   uint_32              newsock;

   RTCS_ENTER(ACCEPT, sock);

#if RTCSCFG_CHECK_ERRORS
   switch (((SOCKET_STRUCT_PTR)sock)->STATE) {
   case SOCKSTATE_STREAM_GROUND:
            RTCS_setsockerror(sock, RTCSERR_SOCK_NOT_LISTENING);
            RTCS_EXIT2(ACCEPT, RTCSERR_SOCK_NOT_LISTENING, RTCS_SOCKET_ERROR);
   case SOCKSTATE_STREAM_BOUND:
            RTCS_setsockerror(sock, RTCSERR_SOCK_NOT_LISTENING);
            RTCS_EXIT2(ACCEPT, RTCSERR_SOCK_NOT_LISTENING, RTCS_SOCKET_ERROR);
   case SOCKSTATE_STREAM_CONNECTED:
            RTCS_setsockerror(sock, RTCSERR_SOCK_IS_CONNECTED);
            RTCS_EXIT2(ACCEPT, RTCSERR_SOCK_IS_CONNECTED, RTCS_SOCKET_ERROR);
   } /* Endswitch */
#endif

   /*
   ** Allocate a new socket structure for the established TCB
   */
   new_socket_ptr = SOCK_Get_sock_struct((RTCS_SOCKET_CALL_STRUCT_PTR)SOCK_STREAM,
                                         RTCS_task_getid());
   newsock = (uint_32)new_socket_ptr;

   if (new_socket_ptr == NULL) {
      RTCS_setsockerror(sock, RTCSERR_OUT_OF_SOCKETS);
      RTCS_EXIT2(ACCEPT, RTCSERR_OUT_OF_SOCKETS, RTCS_SOCKET_ERROR);
   } /* Endif */

   parms.TCB_PTR = socket_ptr->TCB_PTR;
   new_socket_ptr->AF = socket_ptr->AF;
   parms.SOCKET = newsock;

   error = RTCSCMD_issue(parms, TCP_Process_accept);
   if (error) {
      SOCK_Free_sock_struct(new_socket_ptr);
      RTCS_setsockerror(sock, error);
      RTCS_EXIT2(ACCEPT, error, RTCS_SOCKET_ERROR);
   } /* Endif */

   new_socket_ptr->STATE = SOCKSTATE_STREAM_CONNECTED;
   
   error = SOCK_STREAM_getpeername(newsock, peeraddr, addrlen);

   /*
   ** If there is an error, it is because the TCB was freed (by a RST).
   ** We will free the socket and return a socket error
   */
   if (error) {
      SOCK_Free_sock_struct(new_socket_ptr);
      RTCS_setsockerror(sock, error);
      RTCS_EXIT2(ACCEPT, error, RTCS_SOCKET_ERROR);
   } /* Endif */

   /*
   ** inherit the socket options from listen socket
   */
   new_socket_ptr->CONNECT_TIMEOUT        = socket_ptr->CONNECT_TIMEOUT;
   new_socket_ptr->RETRANSMISSION_TIMEOUT = socket_ptr->RETRANSMISSION_TIMEOUT;
   new_socket_ptr->SEND_TIMEOUT           = socket_ptr->SEND_TIMEOUT;
   new_socket_ptr->RECEIVE_TIMEOUT        = socket_ptr->RECEIVE_TIMEOUT;
   new_socket_ptr->RECEIVE_PUSH           = socket_ptr->RECEIVE_PUSH;
   new_socket_ptr->SEND_NOWAIT            = socket_ptr->SEND_NOWAIT;
   new_socket_ptr->SEND_WAIT              = socket_ptr->SEND_WAIT;
   new_socket_ptr->SEND_PUSH              = socket_ptr->SEND_PUSH;
   new_socket_ptr->RECEIVE_NOWAIT         = socket_ptr->RECEIVE_NOWAIT;
   new_socket_ptr->CHECKSUM_BYPASS        = socket_ptr->CHECKSUM_BYPASS;
   new_socket_ptr->ACTIVE                 = socket_ptr->ACTIVE;
   new_socket_ptr->TBSIZE                 = socket_ptr->TBSIZE;
   new_socket_ptr->RBSIZE                 = socket_ptr->RBSIZE;
   new_socket_ptr->MAXRTO                 = socket_ptr->MAXRTO;
   new_socket_ptr->MAXRCV_WND             = socket_ptr->MAXRCV_WND;
   new_socket_ptr->KEEPALIVE              = socket_ptr->KEEPALIVE;
   new_socket_ptr->NOWAIT                 = socket_ptr->NOWAIT;
   new_socket_ptr->NO_NAGLE_ALGORITHM     = socket_ptr->NO_NAGLE_ALGORITHM;
   new_socket_ptr->NOSWRBUF               = socket_ptr->NOSWRBUF;
   new_socket_ptr->CALL_BACK              = socket_ptr->CALL_BACK;
   new_socket_ptr->TYPE_MIRROR            = socket_ptr->TYPE_MIRROR;
   new_socket_ptr->TIMEWAIT_TIMEOUT       = socket_ptr->TIMEWAIT_TIMEOUT;
   new_socket_ptr->LOCAL_PORT             = socket_ptr->LOCAL_PORT;
   new_socket_ptr->AF                     = socket_ptr->AF;
   
   /* Start CR 1144 */
   new_socket_ptr->LINK_OPTIONS           = socket_ptr->LINK_OPTIONS;
   /* End CR 1144 */

   RTCS_EXIT2(ACCEPT, RTCS_OK, newsock);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_send
* Returned Value  : number of bytes sent or RTCS_ERROR
* Comments  :  Send data to a connected socket.
*
*END*-----------------------------------------------------------------*/

int_32  SOCK_STREAM_send
   (
      uint_32        sock,
         /* [IN] socket handle */
      pointer        buffer,
         /* [IN] data to transmit */
      uint_32        buflen,
         /* [IN] length of the buffer, in bytes */
      uint_32        flags
         /* [IN] flags to underlying protocols */
   )
{ /* Body */
            TCP_PARM             parms;
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
            uint_32              error;

   RTCS_ENTER(SEND, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE != SOCKSTATE_STREAM_CONNECTED) {
      RTCS_setsockerror(sock, RTCSERR_SOCK_NOT_CONNECTED);
      RTCS_EXIT2(SEND, RTCSERR_SOCK_NOT_CONNECTED, RTCS_ERROR);
   } /* Endif */
#endif

   if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) {
      RTCS_setsockerror(sock, RTCSERR_TCP_CONN_RLSD);
      RTCS_EXIT2(SEND, RTCSERR_TCP_CONN_RLSD, RTCS_ERROR);
   } /* Endif */

   if (flags == 0) {
      if (socket_ptr->SEND_NOWAIT) flags |= TCPO_NOWAIT;
      if (socket_ptr->SEND_WAIT)   flags |= TCPO_WAIT;
      if (socket_ptr->SEND_PUSH)   flags |= TCPO_PUSH;
   } /* Endif */

   parms.TCB_PTR    = socket_ptr->TCB_PTR;
   parms.SOCKET     = sock;
   parms.BUFFER_PTR = buffer;
   parms.BUFF_LENG  = buflen;
   parms.TIMEOUT    = socket_ptr->SEND_TIMEOUT;
   parms.OPTIONS    = flags;

    if(flags & (TCPO_NOWAIT|TCPO_WAIT)!= 0) /* TCPO_NOWAIT or TCPO_WAIT */
    {   
       error = RTCSCMD_issue(parms, TCP_Process_send);
       if (error)
       {
            RTCS_setsockerror(sock, error);
            RTCS_EXIT2(SEND, error, RTCS_ERROR);
       } 
     
       RTCS_EXIT2(SEND, RTCS_OK, parms.BUFF_LENG);
    }   
    else /* Default */
    {
        uint_32 result_length = 0;
        do
        {
            error = RTCSCMD_issue(parms, TCP_Process_send);
            if (error)
            {
                RTCS_setsockerror(sock, error);
                RTCS_EXIT2(SEND, error, RTCS_ERROR);
            } 
             
            if(parms.BUFF_LENG >0)
            {  
                result_length += parms.BUFF_LENG;
            }
            else
            {
                RTCS_selectset(&sock, 1, 1);
            }
            
            parms.BUFFER_PTR = (pointer) ((uint_32)(parms.BUFFER_PTR) + parms.BUFF_LENG);
            parms.BUFF_LENG  = buflen - result_length;
        }
        while(parms.BUFF_LENG > 0);
        
        RTCS_EXIT2(SEND, RTCS_OK, result_length);
    }

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_recv
* Returned Value  : number of bytes received or RTCS_ERROR
* Comments  :  Receive data from a connected socket.
*
*END*-----------------------------------------------------------------*/

int_32  SOCK_STREAM_recv
   (
      uint_32        sock,
         /* [IN] socket handle */
      pointer        buffer,
         /* [IN] buffer for received data */
      uint_32        buflen,
         /* [IN] length of the buffer, in bytes */
      uint_32        flags
         /* [IN] flags to underlying protocols */
   )
{ /* Body */
            TCP_PARM             parms;
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
            uint_32              error;

   RTCS_ENTER(RECV, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE != SOCKSTATE_STREAM_CONNECTED) {
      RTCS_setsockerror(sock, RTCSERR_SOCK_NOT_CONNECTED);
      RTCS_EXIT2(RECV, RTCSERR_SOCK_NOT_CONNECTED, RTCS_ERROR);
   } /* Endif */
#endif

   if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) {
      RTCS_setsockerror(sock, RTCSERR_TCP_CONN_RLSD);
      RTCS_EXIT2(RECV, RTCSERR_TCP_CONN_RLSD, RTCS_ERROR);
   } /* Endif */

   if (flags == 0) {
      if (socket_ptr->RECEIVE_NOWAIT) flags |= TCPO_NOWAIT;
                                 else flags |= TCPO_WAIT;
      if (socket_ptr->RECEIVE_PUSH)   flags |= TCPO_PUSH;
   } /* Endif */

   parms.TCB_PTR    = socket_ptr->TCB_PTR;
   parms.SOCKET     = sock;
   parms.BUFFER_PTR = buffer;
   parms.BUFF_LENG  = buflen;
   parms.TIMEOUT    = socket_ptr->RECEIVE_TIMEOUT;
   parms.OPTIONS    = flags;

   error = RTCSCMD_issue(parms, TCP_Process_receive);
   if (error) {
      RTCS_setsockerror(sock, error);
  
      /* Start CR 2340 */
      /* If data was copied to the userbuf, but not all that
         the recv() asked for, and a timer was started that has
         now timed out, we need to return with the count, and not
         RTCS_ERROR */
      if (error == RTCSERR_TCP_TIMED_OUT) {
         /* rcvusernxt is the number of data bytes in rcvuserbuf.
            TCB comments say this is undefined in the special case 
            when rcvuserbuf == rcvbuf.  However, TCP_Setup_receive() 
            does not use the rcvuserbuf when there is a timeout.  So
            this test is not needed, but do it anyway to be safe. */
         if (parms.TCB_PTR->rcvuserbuf != parms.TCB_PTR->rcvbuf) {
            if (parms.TCB_PTR->rcvusernxt > 0) {
               RTCS_EXIT2(RECV, RTCS_OK, parms.TCB_PTR->rcvusernxt);
            }
         }
      }
      /* End CR 2340 */
      
      RTCS_EXIT2(RECV, error, RTCS_ERROR);
   } /* Endif */

   RTCS_EXIT2(RECV, RTCS_OK, parms.BUFF_LENG);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_getsockname
* Returned Value  : error code
* Comments  :  Retrieve the address of the local endpoint of a
*              bound socket. Support IPv6 and IPv4.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_STREAM_getsockname
   (
      uint_32              sock,
         /* [IN] socket handle */
      sockaddr _PTR_    name,
         /* [OUT] address of local endpoint */
      uint_16     _PTR_    namelen
         /* [IN/OUT] length of the address, in bytes */
   )
{ /* Body */

   RTCS_ENTER(GETSOCKNAME, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_STREAM_GROUND) {
      RTCS_EXIT(GETSOCKNAME, RTCSERR_SOCK_NOT_BOUND);
   } /* Endif */
#endif

   if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) {
      RTCS_EXIT(GETSOCKNAME, RTCSERR_TCP_CONN_RLSD);
   } /* Endif */


    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET) 
            {
        #endif
                if(!namelen) 
                {
        #if RTCSCFG_CHECK_ADDRSIZE
                }
                else if (*namelen < sizeof(sockaddr_in)) 
                {
                    sockaddr_in fullname;
                    fullname.sin_family      = AF_INET;
                    fullname.sin_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_port;
                    fullname.sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_host;
                    _mem_copy(&fullname, name, *namelen);
                    *namelen = sizeof(sockaddr_in);
        #endif
                }
                else 
                {
                    ((sockaddr_in *)name)->sin_family      = AF_INET;
                    ((sockaddr_in *)name)->sin_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_port;
                    ((sockaddr_in *)name)->sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_host;
                   *namelen = sizeof(sockaddr_in);
                } /* Endif */
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET6) 
            {
        #endif
                if(!namelen) 
                {

        #if RTCSCFG_CHECK_ADDRSIZE
                } 
                else if (*namelen < sizeof(struct sockaddr_in6)) 
                {
                    struct sockaddr_in6 fullname;
                    fullname.sin6_family      = AF_INET6;
                    fullname.sin6_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_port;
                    IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_ipv6_host,&(fullname.sin6_addr));
                    _mem_copy(&fullname,name, *namelen);
        #endif
                } 
                else 
                {
                    ((sockaddr_in6 *)name)->sin6_family   = AF_INET6;
                    ((sockaddr_in6 *)name)->sin6_port     = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_port;
                    IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->TCB_PTR->local_ipv6_host,&(((sockaddr_in6 *)name)->sin6_addr));
                } /* Endif */
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif

   RTCS_EXIT(GETSOCKNAME, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_getpeername
* Returned Value  : error code
* Comments  :  Retrieve the address of the remote endpoint of a
*              connected socket. Support IPv4 and IPv6
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_STREAM_getpeername
   (
      uint_32                     sock,
         /* [IN] socket handle */
      struct sockaddr _PTR_   name,
         /* [IN] address of remote endpoint */
      uint_16     _PTR_           namelen
         /* [IN/OUT] length of the address, in bytes */
   )
{ /* Body */

   RTCS_ENTER(GETPEERNAME, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE != SOCKSTATE_STREAM_CONNECTED) 
   {
      RTCS_EXIT(GETPEERNAME, RTCSERR_SOCK_NOT_CONNECTED);
   } /* Endif */
#endif

   if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) 
   {
      RTCS_EXIT(GETPEERNAME, RTCSERR_TCP_CONN_RLSD);
   } /* Endif */

   
#if RTCSCFG_ENABLE_IP4       

    #if RTCSCFG_ENABLE_IP6   //it is switch off if else  if only one family used    
    if (((SOCKET_STRUCT_PTR)sock)->AF ==AF_INET)
    {
    #endif
        if (!namelen) 
        {
     #if RTCSCFG_CHECK_ADDRSIZE
        } 
        else if (*namelen < sizeof(sockaddr_in)) 
        {
            sockaddr_in fullname;
            fullname.sin_family      = AF_INET;
            fullname.sin_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_port;
            fullname.sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_host;
            _mem_copy(&fullname, name, *namelen);
            *namelen = sizeof(sockaddr_in);
     #endif
        } 
        else 
        {
            ((struct sockaddr_in*)name)->sin_family      = AF_INET;
            ((struct sockaddr_in*)name)->sin_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_port;
            ((struct sockaddr_in*)name)->sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_host;
            *namelen = sizeof(sockaddr_in);
        } /* Endif */

    #if RTCSCFG_ENABLE_IP6   //it is switch off if else  if only one family used    
        goto rtcs_exit_0;
    }
    #endif

#endif

#if RTCSCFG_ENABLE_IP6       

    #if RTCSCFG_ENABLE_IP4   //it is switch off if else  if only one family used    
    else if (((SOCKET_STRUCT_PTR)sock)->AF ==AF_INET6)
    {
    #endif
        if (!namelen) 
        {

        #if RTCSCFG_CHECK_ADDRSIZE
        } 
        else if (*namelen < sizeof(struct sockaddr_in6)) 
        {
            struct sockaddr_in6 fullname;
            fullname.sin6_family      = AF_INET6;
            fullname.sin6_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_port;
            IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_ipv6_host, &(fullname.sin6_addr));
            _mem_copy(&fullname, name, *namelen);
            *namelen = sizeof(struct sockaddr_in6);
        #endif
        } 
        else 
        {
            ((struct sockaddr_in6*)name)->sin6_family      = AF_INET6;
            ((struct sockaddr_in6*)name)->sin6_port        = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_port;
            IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->TCB_PTR->remote_ipv6_host, &(((struct sockaddr_in6*)name)->sin6_addr));
            *namelen = sizeof(struct sockaddr_in6);
        } /* Endif */

    #if RTCSCFG_ENABLE_IP4       
    }
    #endif
#endif      

rtcs_exit_0:

           RTCS_EXIT(GETPEERNAME, RTCS_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_STREAM_shutdown
* Returned Value  : error code
* Comments  :  Close a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_STREAM_shutdown
   (
      uint_32        sock,
         /* [IN] socket handle */
      uint_32        how
         /* [IN] how to terminate the socket */
   )
{ /* Body */
   TCP_PARM    parms;
   uint_32     error;

   RTCS_ENTER(SHUTDOWN, sock);

   if (!((SOCKET_STRUCT_PTR)sock)->TCB_PTR) {
      RTCS_EXIT(SHUTDOWN, RTCS_OK);
   } /* Endif */

   parms.TCB_PTR = ((SOCKET_STRUCT_PTR)sock)->TCB_PTR;

   if (how == FLAG_ABORT_CONNECTION) {
      error = RTCSCMD_issue(parms, TCP_Process_abort);
   } else {
      error = RTCSCMD_issue(parms, TCP_Process_close);
   } /* Endif */

   if (error) {
      RTCS_EXIT(SHUTDOWN, error);
   } /* Endif */

   RTCS_EXIT(SHUTDOWN, RTCS_OK);

} /* Endbody */

#endif

/* EOF */
