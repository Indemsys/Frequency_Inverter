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
* $FileName: sdgram.c$
* $Version : 3.8.7.0$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   This file contains the socket interface functions for
*   (PF_INET,SOCK_DGRAM) sockets.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_UDP

#include "tcpip.h"
#include "udp.h"
#include "socket.h"

#define RTCS_ENTER(f,a)    RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_DGRAM_ ## f, a)

#define RTCS_EXIT(f,a)     RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_DGRAM_ ## f, a); \
                           return a

#define RTCS_EXIT2(f,a,e)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_DGRAM_ ## f, a); \
                           return e


uint_32 SOCK_DGRAM_socket      (uint_32);
uint_32 SOCK_DGRAM_bind        (uint_32, const sockaddr _PTR_, uint_16);
uint_32 SOCK_DGRAM_connect     (uint_32, const sockaddr _PTR_, uint_16);
uint_32 SOCK_DGRAM_getsockname (uint_32, sockaddr _PTR_, uint_16 _PTR_);
uint_32 SOCK_DGRAM_getpeername (uint_32, sockaddr _PTR_, uint_16 _PTR_);
 int_32 SOCK_DGRAM_recv        (uint_32, pointer, uint_32, uint_32);
 int_32 SOCK_DGRAM_recvfrom    (uint_32, pointer, uint_32, uint_32, sockaddr _PTR_, uint_16 _PTR_);//IPv4+6
 int_32 SOCK_DGRAM_send        (uint_32, pointer, uint_32, uint_32);
 int_32 SOCK_DGRAM_sendto      (uint_32, pointer, uint_32, uint_32, sockaddr _PTR_, uint_16);
uint_32 SOCK_DGRAM_shutdown    (uint_32, uint_32);

const RTCS_SOCKET_CALL_STRUCT SOCK_DGRAM_CALL = {
   SOCK_DGRAM_socket,
   SOCK_DGRAM_bind,
   SOCK_DGRAM_connect,
   NULL,
   NULL,
   SOCK_DGRAM_getsockname,
   SOCK_DGRAM_getpeername,
   SOCK_DGRAM_recv,
   SOCK_DGRAM_recvfrom,
   NULL,
   SOCK_DGRAM_send,
   SOCK_DGRAM_sendto,
   NULL,
   NULL,
   SOCK_DGRAM_shutdown
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_socket
* Returned Value  : error code
* Comments  :  Creates an unbound socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_DGRAM_socket
   (
      uint_32     sock
         /* [IN] socket handle */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   error = RTCSCMD_issue(parms, UDP_open);
   if (error) return error;

   ((SOCKET_STRUCT_PTR)sock)->STATE         = SOCKSTATE_DGRAM_GROUND;
   ((SOCKET_STRUCT_PTR)sock)->UCB_PTR       = parms.ucb;
   ((SOCKET_STRUCT_PTR)sock)->MCB_PTR       = &parms.ucb->MCB_PTR;
   ((SOCKET_STRUCT_PTR)sock)->IGMP_LEAVEALL = &parms.ucb->IGMP_LEAVEALL;
   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
************************************************************************
*
* Function Name   : SOCK_DGRAM_bind
* Returned Value  : error code 
*
* Comments  :  Binds the local endpoint of a socket. Support IPv6 and IPv4
*
************************************************************************
*END*-----------------------------------------------------------------*/
uint_32  SOCK_DGRAM_bind
   (
      uint_32                    sock,
         /* [IN] socket handle */
      const sockaddr _PTR_    localaddr,
         /* [IN] local address to which to bind the socket */
      uint_16                    addrlen
         /* [IN] length of the address, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   RTCS_ENTER(BIND, sock);

#if RTCSCFG_CHECK_ERRORS
   if(((SOCKET_STRUCT_PTR)sock)->STATE != SOCKSTATE_DGRAM_GROUND) 
   {
      RTCS_EXIT(BIND, RTCSERR_SOCK_IS_BOUND);
   } /* Endif */
#endif

#if RTCSCFG_CHECK_ADDRSIZE

    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (localaddr->sa_family == AF_INET) 
            {
        #endif
                if (addrlen < sizeof(sockaddr_in)) 
                {
                    RTCS_EXIT(BIND, RTCSERR_SOCK_SHORT_ADDRESS);
                } /* Endif */
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (localaddr->sa_family == AF_INET6) 
            {
        #endif
                if (addrlen < sizeof(struct sockaddr_in6)) 
                {
                    RTCS_EXIT(BIND, RTCSERR_SOCK_SHORT_ADDRESS);
                } /* Endif */
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
#endif


#if RTCSCFG_CHECK_ERRORS
    error =1;

    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (localaddr->sa_family == AF_INET) 
            {
        #endif
                error = 0;
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (localaddr->sa_family == AF_INET6) 
            {
        #endif
                error = 0;
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
        if (error != 0) 
        {
            RTCS_EXIT(BIND, RTCSERR_SOCK_INVALID_AF);
        } /* Endif */
#endif


   parms.ucb        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   parms.udpword    = sock;


    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (localaddr->sa_family == AF_INET) 
            {
        #endif
                parms.udpport    =((sockaddr_in *)localaddr)->sin_port;
                parms.udpservice = UDP_process;
                parms.ipaddress  =((sockaddr_in *)localaddr)->sin_addr.s_addr;
                error = RTCSCMD_issue(parms, UDP_bind);
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (localaddr->sa_family == AF_INET6) 
            {
        #endif
                parms.udpport    =((sockaddr_in6 *)localaddr)->sin6_port;
                parms.udpservice = UDP_process6;
                memcpy( &(parms.ipv6address), &(((sockaddr_in6 *)localaddr)->sin6_addr),sizeof(struct in6_addr));
                /*add scope_id here*/
                parms.if_scope_id = ((sockaddr_in6 *)localaddr)->sin6_scope_id;
                error = RTCSCMD_issue(parms, UDP_bind6);
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
   if (error) 
   {
      RTCS_EXIT(BIND, error);
   } /* Endif */


   ((SOCKET_STRUCT_PTR)sock)->STATE = SOCKSTATE_DGRAM_BOUND;

   RTCS_EXIT(BIND, RTCS_OK);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
************************************************************************
*
* Function Name   : SOCK_DGRAM_connect
* Returned Value  : error code
* Comments  :  Binds the remote endpoint of a socket. Support IPv6 and IPv4.
*
*     1. If connect succeeds the socket is in OPEN state.
*     2. If connect fails, the socket is in BOUND or GROUND state, and the
*        local IP is the same as it was immediatly after the bind() call.
*
***********************************************************************
*END*-----------------------------------------------------------------*/
uint_32  SOCK_DGRAM_connect
   (
      uint_32                    sock,
         /* [IN] socket handle */
      const struct sockaddr _PTR_    peeraddr,
         /* [IN] remote address to which to bind the socket */
      uint_16                    addrlen
         /* [IN] length of the address, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error = RTCS_OK;
   uint_16     state = ((SOCKET_STRUCT_PTR)sock)->STATE;
    
    
   RTCS_ENTER(CONNECT, sock);

#if RTCSCFG_CHECK_ADDRSIZE


    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (peeraddr->sa_family == AF_INET) 
            {
        #endif
                if (addrlen < sizeof(sockaddr_in)) 
                {
                    error = RTCSERR_SOCK_SHORT_ADDRESS;
                } /* Endif */
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (peeraddr->sa_family == AF_INET6) 
            {
        #endif
                if (addrlen < sizeof(struct sockaddr_in6)) 
                {
                    error = RTCSERR_SOCK_SHORT_ADDRESS;
                } /* Endif */
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
#endif

#if RTCSCFG_CHECK_ERRORS
        if(!error)
        {
            error = RTCSERR_SOCK_INVALID_AF;
    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if(peeraddr->sa_family == AF_INET) 
            {
        #endif
                error = RTCS_OK;
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if(peeraddr->sa_family == AF_INET6) 
            {
        #endif
                error = RTCS_OK;
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
        
        }
#endif

   
   /* If socket is unbound, bind it  SOCK_DGRAM_bind */
   if (state == SOCKSTATE_DGRAM_GROUND) 
   {
      //struct sockaddr_in6 localaddr;   
      struct sockaddr localaddr;   

      if (error)  
      {
         RTCS_EXIT(CONNECT, error);
      } /* Endif */

     
    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (peeraddr->sa_family == AF_INET) 
            {
        #endif

                localaddr.sa_family = AF_INET;
                ((sockaddr_in*)&localaddr)->sin_port = 0;
                /*Set INADDR_ANY address*/
                memset(&(((sockaddr_in*)&localaddr)->sin_addr), 0, sizeof(in_addr));

        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (peeraddr->sa_family == AF_INET6) 
            {
        #endif
                localaddr.sa_family = AF_INET6;
                ((sockaddr_in6 *)&localaddr)->sin6_port = 0;
                /* add local scope_id here */
                ((sockaddr_in6 *)&localaddr)->sin6_scope_id = 0;
                /*Set INADDR_ANY address*/
                memset(&(((sockaddr_in6 *)&localaddr)->sin6_addr), 0, sizeof(in6_addr));
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
      
      error = SOCK_DGRAM_bind(sock, &localaddr, sizeof(localaddr));
      if (error)  {
         RTCS_EXIT(CONNECT, error);
      } /* Endif */
   } /* Endif */



   /** check below for IPv6 ************************************/
   /*
   ** Set socket in BOUND state and proceed normaly. If connect
   ** fails, leave the socket in BOUND state. If connect
   ** succeeds, set state to OPEN.
   */
   ((SOCKET_STRUCT_PTR)sock)->STATE = SOCKSTATE_DGRAM_BOUND;

    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (peeraddr->sa_family == AF_INET) 
            {
        #endif
                parms.ipaddress =((sockaddr_in *)peeraddr)->sin_addr.s_addr;
                parms.udpport   =((sockaddr_in *)peeraddr)->sin_port;
                parms.ucb       =((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
                parms.udpword   = error;
                error = RTCSCMD_issue(parms, UDP_connect);
        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (peeraddr->sa_family == AF_INET6) 
            {
        #endif
                memcpy( &(parms.ipv6address), &(((sockaddr_in6 *)peeraddr)->sin6_addr),sizeof(struct in6_addr));
                parms.udpport   = ((sockaddr_in6 *)peeraddr)->sin6_port;
                parms.ucb       = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
                parms.udpword   = error;
               /*
                * choose and add scope_id here. 
                * If peeraddr(target addr) scope_id is NULL we well use local(binded) scope_id 
                */
                parms.if_scope_id = ((sockaddr_in6 *)peeraddr)->sin6_scope_id;
                if(parms.if_scope_id ==0)
                {
                    parms.if_scope_id = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IF_SCOPE_ID;                        
                }
                error = RTCSCMD_issue(parms, UDP_connect6);
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
    
   if (error) {
      RTCS_EXIT(CONNECT, error);
   } /* Endif */

   /* Success */
   ((SOCKET_STRUCT_PTR)sock)->STATE = SOCKSTATE_DGRAM_OPEN;
   RTCS_EXIT(CONNECT, RTCS_OK);

} /* Endbody */



#if 0
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_getsockname
* Returned Value  : error code
* Comments  :  Retrieve the address of the local endpoint of a
*              bound socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_DGRAM_getsockname
   (
      uint_32              sock,
         /* [IN] socket handle */
      sockaddr_in _PTR_    name,
         /* [OUT] address of local endpoint */
      uint_16     _PTR_    namelen
         /* [IN/OUT] length of the address, in bytes */
   )
{ /* Body */

   RTCS_ENTER(GETSOCKNAME, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_DGRAM_GROUND) {
      RTCS_EXIT(GETSOCKNAME, RTCSERR_SOCK_NOT_BOUND);
   } /* Endif */
#endif

   if (!namelen) {

#if RTCSCFG_CHECK_ADDRSIZE
   } else if (*namelen < sizeof(sockaddr_in)) {
      sockaddr_in fullname;
      fullname.sin_family      = AF_INET;
      fullname.sin_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->PORT;
      fullname.sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IPADDR;
      _mem_copy(&fullname, name, *namelen);
      *namelen = sizeof(sockaddr_in);
#endif

   } else {
      name->sin_family      = AF_INET;
      name->sin_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->PORT;
      name->sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IPADDR;
      *namelen = sizeof(sockaddr_in);

   } /* Endif */

   RTCS_EXIT(GETSOCKNAME, RTCS_OK);

} /* Endbody */

#endif



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_getsockname
* Returned Value  : error code
* Comments  :  Retrieve the address of the local endpoint of a
*              bound socket.Support IPv6 and IPv4
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_DGRAM_getsockname
   (
      uint_32              sock,
         /* [IN] socket handle */
      sockaddr _PTR_       name,
         /* [OUT] address of local endpoint */
      uint_16     _PTR_    namelen
         /* [IN/OUT] length of the address, in bytes */
   )
{ /* Body */

   RTCS_ENTER(GETSOCKNAME, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_DGRAM_GROUND) {
      RTCS_EXIT(GETSOCKNAME, RTCSERR_SOCK_NOT_BOUND);
   } /* Endif */
#endif

    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET) 
            {
        #endif
                if (!namelen) 
                {

        #if RTCSCFG_CHECK_ADDRSIZE
                } else if (*namelen < sizeof(sockaddr_in)) 
                {
                    sockaddr_in fullname;
                    fullname.sin_family      = AF_INET;
                    fullname.sin_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->PORT;
                    fullname.sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IPADDR;
                    _mem_copy(&fullname, name, *namelen);
        #endif
                }
                else 
                {
                    ((sockaddr_in *)name)->sin_family      = AF_INET;
                    ((sockaddr_in *)name)->sin_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->PORT;
                    ((sockaddr_in *)name)->sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IPADDR;
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
                    fullname.sin6_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_PORT;
                    IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IPV6ADDR,&(fullname.sin6_addr));
                    _mem_copy(&fullname,name, *namelen);
        #endif
                } 
                else 
                {
                    ((sockaddr_in6 *)name)->sin6_family   = AF_INET6;
                    ((sockaddr_in6 *)name)->sin6_port     = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_PORT;
                    IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->UCB_PTR->IPV6ADDR,&(((sockaddr_in6 *)name)->sin6_addr));
                    *namelen = sizeof(sockaddr_in6);
                } /* Endif */
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif

   RTCS_EXIT(GETSOCKNAME, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_getpeername
* Returned Value  : error code
* Comments  :  Retrieve the address of the peer endpoint of a
*              connected socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_DGRAM_getpeername
   (
      uint_32              sock,
         /* [IN] socket handle */
      sockaddr _PTR_    name,
         /* [OUT] address of peer endpoint */
      uint_16     _PTR_    namelen
         /* [IN/OUT] length of the address, in bytes */
   )
{ /* Body */

        RTCS_ENTER(GETPEERNAME, sock);

    #if RTCSCFG_CHECK_ERRORS
        if (((SOCKET_STRUCT_PTR)sock)->STATE != SOCKSTATE_DGRAM_OPEN) 
        {
            RTCS_EXIT(GETPEERNAME, RTCSERR_SOCK_NOT_CONNECTED);
        } /* Endif */
    #endif


    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET) 
            {
        #endif
                if (!namelen) 
                {

        #if RTCSCFG_CHECK_ADDRSIZE
                } else if (*namelen < sizeof(sockaddr_in)) 
                {
                    sockaddr_in fullname;
                    fullname.sin_family      = AF_INET;
                    fullname.sin_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_PORT;
                    fullname.sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_HOST;
                    _mem_copy(&fullname, name, *namelen);
        #endif
                }
                else 
                {
                    ((sockaddr_in *)name)->sin_family      = AF_INET;
                    ((sockaddr_in *)name)->sin_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_PORT;
                    ((sockaddr_in *)name)->sin_addr.s_addr = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_HOST;
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
                    fullname.sin6_port        = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_PORT;
                    IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_HOST6,&(fullname.sin6_addr));
                    _mem_copy(&fullname,name, *namelen);
        #endif
                } 
                else 
                {
                    ((sockaddr_in6 *)name)->sin6_family   = AF_INET6;
                    ((sockaddr_in6 *)name)->sin6_port     = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_PORT;
                    IN6_ADDR_COPY(&((SOCKET_STRUCT_PTR)sock)->UCB_PTR->REMOTE_HOST6,&(((sockaddr_in6 *)name)->sin6_addr));
                    *namelen = sizeof(sockaddr_in);
                } /* Endif */
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
 
    RTCS_EXIT(GETPEERNAME, RTCS_OK); 
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_recv
* Returned Value  : number of bytes received or RTCS_ERROR
* Comments  :  Receives data from a socket.
*
*END*-----------------------------------------------------------------*/

int_32  SOCK_DGRAM_recv
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
   int_32   len;

   RTCS_ENTER(RECV, sock);
   len = SOCK_DGRAM_recvfrom(sock, buffer, buflen, flags, NULL, NULL);
   RTCS_EXIT2(RECV, (len < 0) ? RTCS_ERROR : RTCS_OK, len);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_recvfrom
* Returned Value  : number of bytes received or RTCS_ERROR
* Comments  :  Receive data from a socket for IPv6 and IPv4 family.
*
*END*-----------------------------------------------------------------*/
int_32  SOCK_DGRAM_recvfrom
   (
      uint_32              sock,
         /* [IN] socket handle */
      pointer              buffer,
         /* [IN] buffer for received data */
      uint_32              buflen,
         /* [IN] length of the buffer, in bytes */
      uint_32              flags,
         /* [IN] flags to underlying protocols */
      
         /*sockaddr_in6 _PTR_    sourceaddr,*/
      struct sockaddr _PTR_ sourceaddr,
         /* [OUT] address from which data was received */
      uint_16     _PTR_    addrlen
         /* [IN/OUT] length of the address, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   RTCS_ENTER(RECVFROM, sock);

#if RTCSCFG_CHECK_ERRORS
   if (((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_DGRAM_GROUND) {
      RTCS_setsockerror(sock, RTCSERR_SOCK_NOT_BOUND);
      RTCS_EXIT2(RECVFROM, RTCSERR_SOCK_NOT_BOUND, RTCS_ERROR);
   } /* Endif */
#endif

    parms.ucb      = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
    parms.udpptr   = buffer;
    parms.udpword  = buflen;
    parms.udpflags = flags;

    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET) 
            {
        #endif

                error = RTCSCMD_issue(parms, UDP_receive);
                if (error) 
                {
                    RTCS_setsockerror(sock, error);
                    RTCS_EXIT2(RECVFROM, error, RTCS_ERROR);
                } /* Endif */
                if (!addrlen) 
                {
                #if RTCSCFG_CHECK_ADDRSIZE
                } 
                else if (*addrlen < sizeof(sockaddr_in)) 
                {
                    sockaddr_in fullname;
                    fullname.sin_family      = AF_INET;
                    fullname.sin_port        = parms.udpport;
                    fullname.sin_addr.s_addr = parms.ipaddress;
                    _mem_copy(&fullname, sourceaddr, *addrlen);
                    *addrlen = sizeof(sockaddr_in);
                #endif
                } 
                else 
                {
                    ((sockaddr_in *)sourceaddr)->sin_family      = AF_INET;
                    ((sockaddr_in *)sourceaddr)->sin_port        = parms.udpport;
                    ((sockaddr_in *)sourceaddr)->sin_addr.s_addr = parms.ipaddress;
                    *addrlen = sizeof(sockaddr_in);
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
                error = RTCSCMD_issue(parms, UDP_receive6);
                if (error) 
                {
                    RTCS_setsockerror(sock, error);
                    RTCS_EXIT2(RECVFROM, error, RTCS_ERROR);
                } /* Endif */
                /* copy addr structure to output */ 
                if(!addrlen)//if parameter of addrlen is not NULL
                {
                #if RTCSCFG_CHECK_ADDRSIZE
                } 
                else if (*addrlen < sizeof(struct sockaddr_in6)) 
                {
                    struct sockaddr_in6 fullname;
                    fullname.sin6_family      = AF_INET6;
                    fullname.sin6_port        = parms.udpport;
                    fullname.sin6_scope_id    = parms.if_scope_id;
                    IN6_ADDR_COPY(&(parms.ipv6address),&(fullname.sin6_addr));
                    /*do safe mem copy to avoid overflow*/
                    _mem_copy(&fullname, sourceaddr, *addrlen);
                    /*return real value of size of addr structure*/
                    *addrlen = sizeof(struct sockaddr_in6);
                #endif
                } 
                else 
                {   
                    ((sockaddr_in6 *)sourceaddr)->sin6_family      = AF_INET6;
                    ((sockaddr_in6 *)sourceaddr)->sin6_port        = parms.udpport;
                    ((sockaddr_in6 *)sourceaddr)->sin6_scope_id    = parms.if_scope_id;
                    IN6_ADDR_COPY(&(parms.ipv6address),&(((sockaddr_in6 *)sourceaddr)->sin6_addr));
                    *addrlen = sizeof(struct sockaddr_in6);
                }
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
   
   RTCS_EXIT2(RECVFROM, RTCS_OK, parms.udpword);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_send
* Returned Value  : number of bytes sent or RTCS_ERROR
* Comments  :  Send data to a connected socket.
*
*END*-----------------------------------------------------------------*/

int_32  SOCK_DGRAM_send
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
   int_32 len= -1 ; // it will generate error if AF != AF_INET or AF_INET6

   RTCS_ENTER(SEND, sock);
     
   len = SOCK_DGRAM_sendto(sock, buffer, buflen, flags, NULL, 0);
 
   RTCS_EXIT2(SEND, (len < 0) ? RTCS_ERROR : RTCS_OK, len);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_sendto
* Returned Value  : number of bytes sent or RTCS_ERROR
* Comments  :  Send data to a socket.
*
*END*-----------------------------------------------------------------*/

int_32  SOCK_DGRAM_sendto
   (
      uint_32              sock,
         /* [IN] socket handle */
      pointer              send_buffer,
         /* [IN] data to transmit */
      uint_32              buflen,
         /* [IN] length of the buffer, in bytes */
      uint_32              flags,
         /* [IN] flags to underlying protocols */
      sockaddr _PTR_    destaddr,
         /* [IN] address to which to send data */
      uint_16              addrlen
         /* [IN] length of the address, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;
   sockaddr    addr;
   uint_16     len = sizeof(addr);

   RTCS_ENTER(SENDTO, sock);

#if RTCSCFG_CHECK_ADDRSIZE
   
    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET) 
            {
        #endif

                if(destaddr && addrlen < sizeof(sockaddr_in)) 
                {
                    RTCS_setsockerror(sock, RTCSERR_SOCK_SHORT_ADDRESS);
                    RTCS_EXIT2(SENDTO, RTCSERR_SOCK_SHORT_ADDRESS, RTCS_ERROR);
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
                if(destaddr && addrlen < sizeof(sockaddr_in6)) 
                {
                    RTCS_setsockerror(sock, RTCSERR_SOCK_SHORT_ADDRESS);
                    RTCS_EXIT2(SENDTO, RTCSERR_SOCK_SHORT_ADDRESS, RTCS_ERROR);
                } /* Endif */

        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif

#endif


   if(!destaddr) 
   {
      error = SOCK_DGRAM_getpeername(sock, &addr, &len);
      if(error) 
      {
         RTCS_setsockerror(sock, RTCSERR_SOCK_NOT_CONNECTED);
         RTCS_EXIT2(SENDTO, RTCSERR_SOCK_NOT_CONNECTED, RTCS_ERROR);
      } /* Endif */
   } 
   else 
   {

      _mem_copy(destaddr, &addr, sizeof(addr));

   } /* Endif */


    #if RTCSCFG_ENABLE_IP4
        #if RTCSCFG_ENABLE_IP6
            if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET) 
            {
        #endif
                if(((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_DGRAM_GROUND) 
                {
                    sockaddr_in localaddr;
                    localaddr.sin_family      = AF_INET;
                    localaddr.sin_port        = 0;
                    localaddr.sin_addr.s_addr = INADDR_ANY;
                    error = SOCK_DGRAM_bind(sock,(sockaddr *)&localaddr, sizeof(localaddr));
                    if(error) 
                    {
                        RTCS_setsockerror(sock, error);
                        RTCS_EXIT2(SENDTO, error, RTCS_ERROR);
                    } /* Endif */
                } /* Endif */
                parms.ipaddress = ((sockaddr_in *)&addr)->sin_addr.s_addr;
                parms.udpport   = ((sockaddr_in *)&addr)->sin_port;

        #if RTCSCFG_ENABLE_IP6
            }
        #endif
    #endif

    #if RTCSCFG_ENABLE_IP6
        #if RTCSCFG_ENABLE_IP4
            else if (((SOCKET_STRUCT_PTR)sock)->AF == AF_INET6) 
            {
        #endif
                if(((SOCKET_STRUCT_PTR)sock)->STATE == SOCKSTATE_DGRAM_GROUND) 
                {
                    struct sockaddr_in6 localaddr;
                    localaddr.sin6_family      = AF_INET6;
                    localaddr.sin6_port        = 0;
                    IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(localaddr.sin6_addr));
                    error = SOCK_DGRAM_bind(sock, (sockaddr *)&localaddr, sizeof(localaddr));
                    if (error) 
                    {
                        RTCS_setsockerror(sock, error);
                        RTCS_EXIT2(SENDTO, error, RTCS_ERROR);
                    } /* Endif */
                } /* Endif */

                IN6_ADDR_COPY(&(((sockaddr_in6 *)&addr)->sin6_addr),&(parms.ipv6address));
                parms.udpport   = ((sockaddr_in6 *)&addr)->sin6_port;
                parms.if_scope_id =((sockaddr_in6 *)&addr)->sin6_scope_id; 
   
        #if RTCSCFG_ENABLE_IP4
            }
        #endif 
    #endif
   
   parms.ucb       = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   parms.udpptr    = send_buffer;
   parms.udpword   = buflen;
   parms.udpflags  = flags;

   error = RTCSCMD_issue(parms, UDP_send);

   if(error) 
   {
      RTCS_setsockerror(sock, error);
      RTCS_EXIT2(SENDTO, error, RTCS_ERROR);
   } /* Endif */

   RTCS_EXIT2(SENDTO, RTCS_OK, buflen);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_DGRAM_shutdown
* Returned Value  : error code
* Comments  :  Close a socket.
*
*END*-----------------------------------------------------------------*/

uint_32  SOCK_DGRAM_shutdown
   (
      uint_32        sock,
         /* [IN] socket handle */
      uint_32        how
         /* [IN] how to terminate the socket */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   RTCS_ENTER(SHUTDOWN, sock);

   parms.ucb = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   error = RTCSCMD_issue(parms, UDP_close);
   if (error) {
      RTCS_EXIT(SHUTDOWN, error);
   } /* Endif */

   RTCS_EXIT(SHUTDOWN, RTCS_OK);

} /* Endbody */

#endif 
/* EOF */
