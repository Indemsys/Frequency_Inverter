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
* $FileName: udp.c$
* $Version : 3.8.14.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the User
*   Datagram Protocol.  For more details, refer to RFC768.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "udp_prv.h"
#include "icmp_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_init
* Returned Values : uint_32
* Comments        :
*     Initialize the UDP layer.
*
*END*-----------------------------------------------------------------*/

uint_32 UDP_init
   (
      void
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   uint_32              status = RTCS_OK;

   UDP_cfg_ptr = RTCS_mem_alloc_zero(sizeof(UDP_CFG_STRUCT));
   if (UDP_cfg_ptr == NULL)  {
      return RTCSERR_OUT_OF_MEMORY;
   }
   _mem_set_type(UDP_cfg_ptr, MEM_TYPE_UDP_CFG_STRUCT);

   RTCS_LIST_INIT(UDP_cfg_ptr->BOUND_UCB_HEAD);
   RTCS_LIST_INIT(UDP_cfg_ptr->LBOUND_UCB_HEAD);
   RTCS_LIST_INIT(UDP_cfg_ptr->GROUND_UCB_HEAD);
   RTCS_LIST_INIT(UDP_cfg_ptr->OPEN_UCB_HEAD);
   UDP_cfg_ptr->LAST_PORT = IPPORT_USERRESERVED;

   RTCS_setcfg(UDP, UDP_cfg_ptr);

#if RTCSCFG_ENABLE_IP4
    IP_open(IPPROTO_UDP, UDP_service, NULL, &status);
    if(status != RTCS_OK)
    {
        return status;
    }
#endif /*RTCSCFG_ENABLE_IP4*/
   /* Initialisation of the UDP IPv6 service */
#if RTCSCFG_ENABLE_IP6
   IP6_open(IPPROTO_UDP, UDP_service6, NULL, &status);
#endif

   return status;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_openbind_internal
* Returned Values : error code
* Comments        :
*     Opens and binds a new UCB.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

uint_32 UDP_openbind_internal
   (
      uint_16              localport,  /* [IN] local UDP port */
      void (_CODE_PTR_     service)(RTCSPCB_PTR, UCB_STRUCT_PTR),
      UCB_STRUCT_PTR _PTR_ ucb         /* [OUT] new UCB */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   error = RTCSCMD_internal(parms, UDP_open);
   if (error) {
      return error;
   } /* Endif */

   parms.udpservice = service;
   parms.ipaddress  = INADDR_ANY;
   parms.udpport    = localport;
   parms.udpword    = 0;
   error = RTCSCMD_internal(parms, UDP_bind);
   if (error) {
      RTCSCMD_internal(parms, UDP_close);
      return error;
   } /* Endif */

   *ucb = parms.ucb;
   return RTCS_OK;

} /* Endbody */

#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_close_internal
* Returned Values : error code
* Comments        :
*     Closes a UCB.
*
*END*-----------------------------------------------------------------*/

uint_32 UDP_close_internal
   (
      UCB_STRUCT_PTR    ucb         /* [IN] old UCB */
   )
{ /* Body */
   UDP_PARM    parms;
   uint_32     error;

   parms.ucb = ucb;
   error = RTCSCMD_internal(parms, UDP_close);
   if (error) {
      return error;
   } /* Endif */

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_open
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [OUT] unbound UCB
*    _CODE_PTR_         udpservice  not used
*     _ip_address       ipaddress   not used
*     uint_16           udpport     not used
*     uint_16           udpflags    not used
*     pointer           udpptr      not used
*     uint_32           udpword     not used
*
* Comments        :
*     Creates an unbound UCB.
*
*END*-----------------------------------------------------------------*/

void UDP_open
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UCB_STRUCT_PTR       ucb;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   ucb = RTCS_mem_alloc(sizeof(UCB_STRUCT));
   if (!ucb) {
      RTCSCMD_complete(parms, RTCSERR_UDP_UCB_ALLOC);
      return;
   } /* Endif */

   _mem_set_type(ucb, MEM_TYPE_UCB_STRUCT);

   ucb->PORT = 0;
   ucb->REMOTE_PORT = 0;
   ucb->SOCKET      = 0;
   ucb->REMOTE_HOST = INADDR_ANY;
   ucb->BYPASS_RX   = FALSE;
   ucb->BYPASS_TX   = FALSE;
   ucb->NONBLOCK_RX = FALSE;
   ucb->NONBLOCK_TX = FALSE;

   RTCS_QUEUE_INIT(ucb->PHEAD, ucb->PTAIL);
   RTCS_QUEUE_INIT(ucb->RHEAD, ucb->RTAIL);

   /* Initialize the list of joined multicast groups */
   RTCS_LIST_INIT(ucb->MCB_PTR);
   ucb->IGMP_LEAVEALL = NULL;

   /* Add the new UCB into the chain of ground UCBs */
   RTCS_LIST_INS(UDP_cfg_ptr->GROUND_UCB_HEAD, ucb);

   parms->ucb = ucb;
   RTCSCMD_complete(parms, RTCS_OK);
   return;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_bind
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] unbound UCB
*    _CODE_PTR_         udpservice  [IN] service function for incoming datagrams
*     _ip_address       ipaddress   [IN] local IP address
*     uint_16           udpport     [IN] local port
*     uint_16           udpflags    not used
*     pointer           udpptr      not used
*     uint_32           udpword     [IN] socket handle
*
* Comments        :
*     Binds a UCB to a local port.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void UDP_bind
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UCB_STRUCT_PTR       ucb, search_ptr, _PTR_ tmp_ptr, _PTR_ list_ptr;
   uint_16              port;
   _ip_address          ipaddr;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   /* Initialize the UCB */
   port = parms->udpport & 0xFFFF;
   ipaddr = parms->ipaddress;

   if (port) {

      /*
      ** If port is specified, make sure its not in use with same local
      ** IP addr. We can only have one UCB per (IPADDR/port) combination,
      ** including the case were the IPADDR is INADDR_ANY.
      */
      if (ipaddr) {
         RTCS_LIST_SEARCH(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr) {
            if (search_ptr->PORT == port &&
                search_ptr->IPADDR == ipaddr)
            {
               RTCSCMD_complete(parms, RTCSERR_UDP_PORT_OPEN);
               return;
            } /* Endif */
         } /* End SEARCH */

         RTCS_LIST_SEARCH(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr) {
            if (search_ptr->PORT == port &&
                search_ptr->IPADDR == ipaddr)
            {
               RTCSCMD_complete(parms, RTCSERR_UDP_PORT_OPEN);
               return;
            } /* Endif */
         } /* End SEARCH */
      } else {
         RTCS_LIST_SEARCH(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr) {
            if (search_ptr->PORT == port) {
               RTCSCMD_complete(parms, RTCSERR_UDP_PORT_OPEN);
               return;
            } /* Endif */
         } /* End SEARCH */
      } /* Endif */
   }
   else
   {
      port = UDP_cfg_ptr->LAST_PORT;
      for (;;)
      {
         /* Get next port */
         if (--port < IPPORT_RESERVED)
         {
            port = IPPORT_USERRESERVED;
         } /* Endif */
         /* If we tested them all, return an error */
         if (port == UDP_cfg_ptr->LAST_PORT)
         {
            RTCSCMD_complete(parms, RTCSERR_UDP_PORT_ALLOC);
            return;
         } /* Endif */
         /*
         ** Check if it's in use
         */
         RTCS_LIST_SEARCH(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr)
         {
            if (search_ptr->PORT == port)
            {
               break;
            } /* Endif */
         } /* End SEARCH */
         if (!search_ptr)
         {
            RTCS_LIST_SEARCH(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)
            {
               if (search_ptr->PORT == port)
               {
                  break;
               } /* Endif */
            } /* End SEARCH */
         } /* Endif */
         if (!search_ptr)
         {
            RTCS_LIST_SEARCH(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr)
            {
               if (search_ptr->PORT == port)
               {
                  break;
               } /* Endif */
            } /* End SEARCH */
         } /* Endif */
         /* If it's not in use, keep it */
         if (!search_ptr)
         {
            break;
         } /* Endif */
      } /* Endfor */

      UDP_cfg_ptr->LAST_PORT = port;

   } /* Endif */
   ucb = parms->ucb;
   ucb->PORT         = port;
   ucb->PCOUNT       = 0;
   ucb->IPADDR       = ipaddr;
   ucb->KEEP_IPADDR  = (boolean) (ipaddr != INADDR_ANY);
   ucb->SOCKET       = parms->udpword;
   ucb->SERVICE      = parms->udpservice;
   /* This is the list the UCB will be moved to */
   if (ipaddr == INADDR_ANY) {
      list_ptr = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
   } else {
      list_ptr = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
   } /* Endif */
   /* Move the UCB fromt the GROUND list to the BOUND list */
   RTCS_LIST_SEARCHMOD(UDP_cfg_ptr->GROUND_UCB_HEAD, tmp_ptr) {
      if (*tmp_ptr == ucb) {
         RTCS_LIST_DEL(UDP_cfg_ptr->GROUND_UCB_HEAD,tmp_ptr);
         break;
      } /* Endif */
   } /* End SEARCH */
   RTCS_LIST_INS_END((*list_ptr), ucb, tmp_ptr);
   RTCSCMD_complete(parms, RTCS_OK);
   return;
} /* Endbody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_bind6
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] unbound UCB
*    _CODE_PTR_         udpservice  [IN] service function for incoming datagrams
*     _ip_address       ipaddress   [IN] local IP address
*     in6_addr          ipv6address [IN] local IPv6 address
*     uint_16           udpport     [IN] local port
*     uint_16           udpflags    not used
*     pointer           udpptr      not used
*     uint_32           udpword     [IN] socket handle
*
* Comments        :
*     Binds a UCB to a local port.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6

void UDP_bind6
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UCB_STRUCT_PTR       ucb, search_ptr, _PTR_ tmp_ptr, _PTR_ list_ptr;
   uint_16              port;
   in6_addr             ipv6address;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   /* Initialize the UCB */
   port = parms->udpport & 0xFFFF;
   /* copy IPv6 addr */
   IN6_ADDR_COPY(&(parms->ipv6address),&(ipv6address));//from ->to
   if (port) {

      /*
      ** If port is specified, make sure its not in use with same local
      ** IP addr. We can only have one UCB per (IPADDR/port) combination,
      ** including the case were the IPADDR is INADDR_ANY.
      */
      if (!IN6_ARE_ADDR_EQUAL(&(ipv6address), &in6addr_any))
      {
            RTCS_LIST_SEARCH(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)
            {
                if  (
                        search_ptr->PORT == port &&
                        IN6_ARE_ADDR_EQUAL(&(search_ptr->IPV6ADDR), &(ipv6address))
                    )
                {
                    RTCSCMD_complete(parms, RTCSERR_UDP_PORT_OPEN);
                    return;
                } /* Endif */
            } /* End SEARCH */

            RTCS_LIST_SEARCH(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr)
            {
                if  (
                        search_ptr->PORT == port &&
                        IN6_ARE_ADDR_EQUAL(&(search_ptr->IPV6ADDR), &(ipv6address))
                    )
                {
                    RTCSCMD_complete(parms, RTCSERR_UDP_PORT_OPEN);
                    return;
                } /* Endif */
            } /* End SEARCH */
      }else
      {
            RTCS_LIST_SEARCH(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr)
            {
                if(search_ptr->PORT == port)
                {
                    RTCSCMD_complete(parms, RTCSERR_UDP_PORT_OPEN);
                    return;
                } /* Endif */
            } /* End SEARCH */
      } /* Endif */

   } else
   {
      port = UDP_cfg_ptr->LAST_PORT;

      for (;;)
      {

         /* Get next port */
         if (--port < IPPORT_RESERVED)
         {
            port = IPPORT_USERRESERVED;
         } /* Endif */

         /* If we tested them all, return an error */
         if (port == UDP_cfg_ptr->LAST_PORT)
         {
            RTCSCMD_complete(parms, RTCSERR_UDP_PORT_ALLOC);
            return;
         } /* Endif */

         /*
         ** Check if it's in use
         */
         RTCS_LIST_SEARCH(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr)
         {
            if (search_ptr->PORT == port)
            {
               break;
            } /* Endif */
         } /* End SEARCH */

         if (!search_ptr)
         {
            RTCS_LIST_SEARCH(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)
            {
               if (search_ptr->PORT == port)
               {
                  break;
               } /* Endif */
            } /* End SEARCH */
         } /* Endif */

         if (!search_ptr)
         {
            RTCS_LIST_SEARCH(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr)
            {
               if (search_ptr->PORT == port)
               {
                  break;
               } /* Endif */
            } /* End SEARCH */
         } /* Endif */

         /* If it's not in use, keep it */
         if (!search_ptr)
         {
            break;
         } /* Endif */

      } /* Endfor */

      UDP_cfg_ptr->LAST_PORT = port;

   } /* Endif */

   ucb = parms->ucb;

   ucb->PORT         = port;
   ucb->PCOUNT       = 0;
   IN6_ADDR_COPY(&(ipv6address), &(ucb->IPV6ADDR));//from ->to
   ucb->KEEP_IPADDR  = (boolean) (!IN6_ARE_ADDR_EQUAL(&(ipv6address), &in6addr_any));
   ucb->SOCKET       = parms->udpword;
   ucb->SERVICE      = parms->udpservice;
   ucb->IF_SCOPE_ID  = parms->if_scope_id;
   /* This is the list the UCB will be moved to */
   if (IN6_ARE_ADDR_EQUAL(&(ipv6address), &in6addr_any))
   {
      list_ptr = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
   }
   else
   {
      list_ptr = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
   } /* Endif */
   /* Move the UCB fromt the GROUND list to the BOUND list */
   RTCS_LIST_SEARCHMOD(UDP_cfg_ptr->GROUND_UCB_HEAD, tmp_ptr) {
      if (*tmp_ptr == ucb) {
         RTCS_LIST_DEL(UDP_cfg_ptr->GROUND_UCB_HEAD,tmp_ptr);
         break;
      } /* Endif */
   } /* End SEARCH */
   RTCS_LIST_INS_END((*list_ptr), ucb, tmp_ptr);
   RTCSCMD_complete(parms, RTCS_OK);
   return;
} /* Endbody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_connect6
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] bound or open UCB
*    _CODE_PTR_         udpservice  not used
*     in6_addr          ipv6address [IN] peer IPv6 address
*     uint_16           udpport     [IN] peer port
*     uint_16           udpflags    not used
*     pointer           udpptr      not used
*     uint_32           udpword     [IN] socket level error
*
* Comments        :
*     Connects a UCB to a peer port.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6
void UDP_connect6
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UCB_STRUCT_PTR       ucb = parms->ucb, search_ptr, _PTR_ tmp_ptr;
   UCB_STRUCT_PTR       _PTR_ ins_ptr, _PTR_ del_ptr;
   uint_16              port;
   uint_32              error = parms->udpword;
   in6_addr             *t_ipv6address;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   if (!error &&(!(parms->udpport) || IN6_ARE_ADDR_EQUAL(&(parms->ipv6address), &in6addr_any)))
   {
      /* Connect requires both a port AND and IP address.  */
      error = RTCSERR_UDP_PORT_ZERO;

   }
   else if (!error)
   {
        /* Bind a local address if none is specified */
        if (IN6_ARE_ADDR_EQUAL(&(ucb->IPV6ADDR), &in6addr_any))
        {
            t_ipv6address = IP6_route_find(&(parms->ipv6address), 0);
            IN6_ADDR_COPY(t_ipv6address,&(ucb->IPV6ADDR));
            if(IN6_ARE_ADDR_EQUAL(&(ucb->IPV6ADDR), &in6addr_any))
            {
                error = RTCSERR_IP_UNREACH;
            } else
            {
                /*
                ** Make sure this connect request does not put two UCB's into the
                ** same configuration.
                */
                port = ucb->PORT;
                RTCS_LIST_SEARCH(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr)
                {
                    if  (
                            search_ptr != ucb &&
                            search_ptr->PORT == port &&
                            IN6_ARE_ADDR_EQUAL(&(search_ptr->IPV6ADDR), &(ucb->IPV6ADDR))
                        )
                    {
                        error = RTCSERR_UDP_PORT_OPEN;
                        IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(ucb->IPV6ADDR));
                        break;
                } /* Endif */
            } /* End SEARCH */
         } /* Endif */
      } /* Endif */
   }  /*Endif */

   /*
   ** Check for errors and location of UCB. If TRUE, UCB must be moved.
   ** This is an evaluates to TRUE if one of the following is true:
   ** A. There is an error, and there is a remote port  OR
   ** B. There is no error, and there is no remote port
   */
   if(!(error) == !(ucb->REMOTE_PORT))
   {
        if(ucb->REMOTE_PORT)
        {
            if(ucb->KEEP_IPADDR)
            {
                ins_ptr = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
            }
            else
            {
                ins_ptr = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
            }
            del_ptr = &(UDP_cfg_ptr->OPEN_UCB_HEAD);
        }
        else
        {
            ins_ptr = &(UDP_cfg_ptr->OPEN_UCB_HEAD);
            if(ucb->KEEP_IPADDR)
            {
                del_ptr = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
            }
            else
            {
                del_ptr = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
            }
        }
        /* Move the UCB from one list to the other */
        RTCS_LIST_SEARCHMOD(*del_ptr, tmp_ptr)
        {
            if(*tmp_ptr == ucb)
            {
                RTCS_LIST_DEL(*del_ptr, tmp_ptr);
                break;
            } /* Endif */
        } /* End SEARCH */
        RTCS_LIST_INS_END(*ins_ptr, ucb, tmp_ptr);
   } /* Endif */

   /* If there was no error, set the peer endpoint */
   if(!error)
   {
      IN6_ADDR_COPY(&(parms->ipv6address),&(ucb->REMOTE_HOST6));
      ucb->REMOTE_PORT = parms->udpport;
   }
   else
   {
        /* Disconnect the UCB. It is in bound state now */
        if(!ucb->KEEP_IPADDR)
        {
            IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(ucb->IPV6ADDR));
        } /* Endif */
        IN6_ADDR_COPY((in6_addr*)(&(in6addr_any)),&(ucb->REMOTE_HOST6));
        ucb->REMOTE_PORT = 0;
   } /* Endif */
   /* add scope_id here */
   ucb->IF_SCOPE_ID = parms->if_scope_id;
   RTCSCMD_complete(parms, error);
   return;

} /* Endbody */

#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_connect
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] bound or open UCB
*    _CODE_PTR_         udpservice  not used
*     _ip_address       ipaddress   [IN] peer IP address
*     uint_16           udpport     [IN] peer port
*     uint_16           udpflags    not used
*     pointer           udpptr      not used
*     uint_32           udpword     [IN] socket level error
*
* Comments        :
*     Connects a UCB to a peer port.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void UDP_connect
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UCB_STRUCT_PTR       ucb = parms->ucb, search_ptr, _PTR_ tmp_ptr;
   UCB_STRUCT_PTR       _PTR_ ins_ptr, _PTR_ del_ptr;
   uint_16              port;
   uint_32              error = parms->udpword;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   if (!error && (!(parms->udpport) || parms->ipaddress == INADDR_ANY)) {
      /* Connect requires both a port AND and IP address.  */
      error = RTCSERR_UDP_PORT_ZERO;

   } else if (!error)  {
      /* Bind a local address if none is specified */
      if (ucb->IPADDR == INADDR_ANY) {
         ucb->IPADDR = IP_route_find(parms->ipaddress, 0);
         if (ucb->IPADDR == INADDR_ANY) {
            error = RTCSERR_IP_UNREACH;
         } else {
            /*
            ** Make sure this connect request does not put two UCB's into the
            ** same configuration.
            */
            port = ucb->PORT;
            RTCS_LIST_SEARCH(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr) {
               if (search_ptr != ucb && search_ptr->PORT == port &&
                  search_ptr->IPADDR == ucb->IPADDR)
               {
                  error = RTCSERR_UDP_PORT_OPEN;
                  ucb->IPADDR = INADDR_ANY;
                  break;
               } /* Endif */
            } /* End SEARCH */
         } /* Endif */
      } /* Endif */
   }  /*Endif */

   /*
   ** Check for errors and location of UCB. If TRUE, UCB must be moved.
   ** This is an evaluates to TRUE if one of the following is true:
   ** A. There is an error, and there is a remote port  OR
   ** B. There is no error, and there is no remote port
   */
   if (!(error) == !(ucb->REMOTE_PORT)) {
      if (ucb->REMOTE_PORT)  {
         if (ucb->KEEP_IPADDR) {
            ins_ptr = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
         } else {
            ins_ptr = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
         } /* Endif */
         del_ptr = &(UDP_cfg_ptr->OPEN_UCB_HEAD);
      } else {
         ins_ptr = &(UDP_cfg_ptr->OPEN_UCB_HEAD);
         if (ucb->KEEP_IPADDR) {
            del_ptr = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
         } else {
            del_ptr = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
         } /* Endif */
      } /* Endif */

      /* Move the UCB from one list to the other */
      RTCS_LIST_SEARCHMOD(*del_ptr, tmp_ptr)
      {
         if (*tmp_ptr == ucb) {
            RTCS_LIST_DEL(*del_ptr, tmp_ptr);
            break;
         } /* Endif */
      } /* End SEARCH */

      RTCS_LIST_INS_END(*ins_ptr, ucb, tmp_ptr);
   } /* Endif */

   /* If there was no error, set the peer endpoint */
   if (!error)  {
      ucb->REMOTE_HOST = parms->ipaddress;
      ucb->REMOTE_PORT = parms->udpport;
   } else {
      /* Disconnect the UCB. It is in bound state now */
      if (!ucb->KEEP_IPADDR) {
         ucb->IPADDR = INADDR_ANY;
      } /* Endif */
      ucb->REMOTE_HOST = INADDR_ANY;
      ucb->REMOTE_PORT = 0;
   } /* Endif */

   RTCSCMD_complete(parms, error);
   return;

} /* Endbody */

#endif //RTCSCFG_ENABLE_IP4

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_receive6
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*    _CODE_PTR_         udpservice  not used
*     in6_addr                ipv6address;
*     uint_16           udpport     [OUT] source UDP port
*     uint_16           udpflags    [IN] flags
*     pointer           udpptr      [IN/OUT] data buffer
*     uint_32           udpword     [IN/OUT] length of buffer
*
* Comments        :
*     Processes a receive request from the application.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6
void UDP_receive6
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   RTCSPCB_PTR       pcb_ptr;

   /*
   ** Search for a waiting packet on incoming queue
   */
   RTCS_QUEUE_PEEK(parms->ucb->PHEAD, parms->ucb->PTAIL, pcb_ptr);
   if(pcb_ptr)
   {

      if(!(parms->udpflags & RTCS_MSG_PEEK))
      {
         RTCS_QUEUE_DEL_NONEMPTY(parms->ucb->PHEAD, parms->ucb->PTAIL);
         parms->ucb->PCOUNT--;
      } /* Endif */

      /*
      ** Return request to Socket Layer with packet
      */
      IN6_ADDR_COPY((in6_addr *)(((IP_V6_HEADER_PTR)(pcb_ptr->NETWORK_LAYER))->SOURCE),&(parms->ipv6address));



      parms->udpport   = UDP_source(pcb_ptr);

      if(parms->udpword > RTCSPCB_SIZE(pcb_ptr))
      {
         parms->udpword = RTCSPCB_SIZE(pcb_ptr);
      } /* Endif */
      RTCSPCB_memcopy(pcb_ptr, parms->udpptr, 0, parms->udpword);

      ((SOCKET_STRUCT_PTR)parms->ucb->SOCKET)->LINK_OPTIONS.RX = pcb_ptr->LINK_OPTIONS.RX;

      if(!(parms->udpflags & RTCS_MSG_PEEK))
      {
         RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
         RTCSPCB_free(pcb_ptr);
      } /* Endif */

      RTCSCMD_complete(parms, RTCS_OK);
      return;

   }else if(parms->ucb->NONBLOCK_RX || (parms->udpflags & RTCS_MSG_PEEK))
         {

            /*
            ** Return request to Socket Layer with zero bytes
            */
            parms->ipaddress = INADDR_ANY;
            parms->udpport   = 0;
            parms->udpword   = 0;
            RTCSCMD_complete(parms, RTCS_OK);
            return;

   }
   else
   {

      /*
      ** Otherwise, enqueue the request
      */
      RTCS_QUEUE_INS(parms->ucb->RHEAD, parms->ucb->RTAIL, parms);

   } /* Endif */

} /* Endbody */

#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_receive
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*    _CODE_PTR_         udpservice  not used
*     _ip_address       ipaddress   [OUT] source IP address
*     uint_16           udpport     [OUT] source UDP port
*     uint_16           udpflags    [IN] flags
*     pointer           udpptr      [IN/OUT] data buffer
*     uint_32           udpword     [IN/OUT] length of buffer
*
* Comments        :
*     Processes a receive request from the application.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void UDP_receive
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   RTCSPCB_PTR       pcb_ptr;

   /*
   ** Search for a waiting packet on incoming queue
   */
   RTCS_QUEUE_PEEK(parms->ucb->PHEAD, parms->ucb->PTAIL, pcb_ptr);
   if (pcb_ptr) {

      if (!(parms->udpflags & RTCS_MSG_PEEK)) {
         RTCS_QUEUE_DEL_NONEMPTY(parms->ucb->PHEAD, parms->ucb->PTAIL);
         parms->ucb->PCOUNT--;
      } /* Endif */

      /*
      ** Return request to Socket Layer with packet
      */
      parms->ipaddress = IP_source(pcb_ptr);
      parms->udpport   = UDP_source(pcb_ptr);

      if (parms->udpword > RTCSPCB_SIZE(pcb_ptr)) {
         parms->udpword = RTCSPCB_SIZE(pcb_ptr);
      } /* Endif */
      RTCSPCB_memcopy(pcb_ptr, parms->udpptr, 0, parms->udpword);

      ((SOCKET_STRUCT_PTR)parms->ucb->SOCKET)->LINK_OPTIONS.RX = pcb_ptr->LINK_OPTIONS.RX;

      if (!(parms->udpflags & RTCS_MSG_PEEK)) {
         RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
         RTCSPCB_free(pcb_ptr);
      } /* Endif */

      RTCSCMD_complete(parms, RTCS_OK);
      return;

   } else if (parms->ucb->NONBLOCK_RX || (parms->udpflags & RTCS_MSG_PEEK)) {

      /*
      ** Return request to Socket Layer with zero bytes
      */
      parms->ipaddress = INADDR_ANY;
      parms->udpport   = 0;
      parms->udpword   = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;

   } else {

      /*
      ** Otherwise, enqueue the request
      */
      RTCS_QUEUE_INS(parms->ucb->RHEAD, parms->ucb->RTAIL, parms);

   } /* Endif */

} /* Endbody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_send
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*    _CODE_PTR_         udpservice  not used
*     _ip_address       ipaddress   [IN] destination IP address
*     uint_16           udpport     [IN] destination UDP port
*     uint_16           udpflags    [IN] flags
*     pointer           udpptr      [IN] data to send
*     uint_32           udpword     [IN] length of data to send
*
* Comments        :
*     Sends a UDP packet.
*
*END*-----------------------------------------------------------------*/
void UDP_send
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   RTCSPCB_PTR          pcb;
   register boolean     nonblock;
   uint_32              error;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   if (parms->udpport == 0) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_DISCARDED++);
      RTCSCMD_complete(parms, RTCSERR_UDP_PORT_ZERO);
      return;
   } /* Endif */

   pcb = RTCSPCB_alloc_send();
   if (pcb == NULL) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      RTCSCMD_complete(parms, RTCSERR_PCB_ALLOC);
      return;
   } /* Endif */
   if (parms->udpflags & RTCS_MSG_O_NONBLOCK) {
      nonblock = parms->udpflags & RTCS_MSG_S_NONBLOCK;
   } else {
      nonblock = parms->ucb->NONBLOCK_TX;
   } /* Endif */

   if (nonblock) {
      uchar_ptr udpbuffer;

      udpbuffer = RTCS_mem_alloc_system(parms->udpword);
      if (!udpbuffer) {
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         RTCSLOG_PCB_FREE(pcb, RTCSERR_PCB_ALLOC);
         RTCSPCB_free(pcb);
         RTCSCMD_complete(parms, RTCSERR_PCB_ALLOC);
         return;
      } /* Endif */

       _mem_set_type(udpbuffer, MEM_TYPE_UDP_BUFFER);


      error = RTCSPCB_append_fragment_autofree(pcb, parms->udpword, udpbuffer);
      if (error) {
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb));
         _mem_free(udpbuffer);
         RTCSLOG_PCB_FREE(pcb, error);
         RTCSPCB_free(pcb);
         RTCSCMD_complete(parms, error);
         return;
      } /* Endif */

      _mem_copy(parms->udpptr, udpbuffer, parms->udpword);

   } else {

      error = RTCSPCB_append_fragment(pcb, parms->udpword, parms->udpptr);
      if (error) {
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
         IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb));
         RTCSLOG_PCB_FREE(pcb, error);
         RTCSPCB_free(pcb);
         RTCSCMD_complete(parms, error);
         return;
      } /* Endif */

      pcb->UDP_REQUEST = parms;

   } /* Endif */

   pcb->LINK_OPTIONS.TX = ((SOCKET_STRUCT_PTR)parms->ucb->SOCKET)->LINK_OPTIONS.TX;

   parms->COMMON.ERROR = RTCS_OK;

   /* Added IPv6 support */
   /*
        TBD :
                I create UDP_send_internal6 just for compatibility and clear view.
                Later  it should be integrated with UDP_send_internal
    */
   error=1;
#if RTCSCFG_ENABLE_IP4
   if(((SOCKET_STRUCT_PTR)parms->ucb->SOCKET)->AF == AF_INET)
   {
        error = UDP_send_internal(parms->ucb, parms->ucb->IPADDR, parms->ipaddress, parms->udpport, pcb, parms->udpflags);
   }
   #if RTCSCFG_ENABLE_IP6
   else
   #endif
#endif

#if RTCSCFG_ENABLE_IP6
   if((((SOCKET_STRUCT_PTR)parms->ucb->SOCKET)->AF == AF_INET6))
   {
     if(parms->if_scope_id)
        {
         /* parms->if_scope_id has value of scope_id from destination address.
          * If it is not NULL we will use it for calculation what interface we will be used for send packet.
          */
            pcb->IFSRC = ip6_if_get_by_scope_id( parms->if_scope_id);
        }
        else if(parms->ucb->IF_SCOPE_ID)
        {/* parms->ucb->IF_SCOPE_ID has value of scope_id from local address.
          * If it is not NULL we will use it for calculation what interface we will be used for send packet.
          */
            pcb->IFSRC = ip6_if_get_by_scope_id( parms->ucb->IF_SCOPE_ID);
        }
        else
        {   /*
             * if scope_id was not defined( is NULL) let IP level calculate send interface
             */
            pcb->IFSRC = NULL;
        }


        error = UDP_send_internal6(parms->ucb, parms->ucb->IPV6ADDR, parms->ipv6address, parms->udpport, pcb, parms->udpflags);
   }
#endif /* RTCSCFG_ENABLE_IP6 */

   if (nonblock)
   {
        RTCSCMD_complete(parms, error);
   }
   else if (error)
   {
        parms->COMMON.ERROR = error;  /* Assumes application is lower priority */
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_send_internal6
* Returned Values : error code
* Comments        :
*     Sends a UDP packet.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6
uint_32 UDP_send_internal6
   (
      UCB_STRUCT_PTR    ucb,        /* [IN] UDP layer context        */
      in6_addr          srcaddr,    /* [IN] source IPv6 address      */
      in6_addr          destaddr,   /* [IN] destination IPv6 address */
      uint_16           destport,   /* [IN] destination UDP port     */
      RTCSPCB_PTR       pcb_ptr,    /* [IN] packet to send           */
      uint_32           flags       /* [IN] optional flags           */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_HEADER_PTR       packet;
   register boolean     nochksum;
   uint_32              error;

   UDP_cfg_ptr = RTCS_getcfg(UDP);
   IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);

   error = RTCSPCB_insert_header(pcb_ptr, sizeof(UDP_HEADER));
   if (error) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb_ptr));
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      return error;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_PROTO(IPPROTO_UDP), 0);

   packet = (UDP_HEADER_PTR)RTCSPCB_DATA(pcb_ptr);
   RTCSPCB_DATA_TRANSPORT(pcb_ptr) = RTCSPCB_DATA(pcb_ptr);
   htons(packet->SRC_PORT, ucb->PORT);
   htons(packet->DEST_PORT, destport);
   htons(packet->LENGTH, RTCSPCB_SIZE(pcb_ptr));
   htons(packet->CHECKSUM, 0);

   if (flags & RTCS_MSG_O_NOCHKSUM) {
      nochksum = flags & RTCS_MSG_S_NOCHKSUM;
   } else {
      nochksum = ucb->BYPASS_TX;
   } /* Endif */
   if (!nochksum)
   {
      pcb_ptr->IP_SUM     = IP_Sum_PCB(RTCSPCB_SIZE(pcb_ptr), pcb_ptr);
      pcb_ptr->IP_SUM_PTR = packet->CHECKSUM;
   } /* Endif */
   /*
        Here I am checking is local IP is  NULL or not
        It is necessary for allow IPsend change NULL local ipv6 to correct local IPv6
    */
   if(IN6_ARE_ADDR_EQUAL(&srcaddr, &in6addr_any))
   {
        return IP6_send(pcb_ptr, IPPROTO_UDP, NULL,&destaddr, pcb_ptr->IFSRC, 0);
   }
   else
   {
        return IP6_send(pcb_ptr, IPPROTO_UDP, &srcaddr,&destaddr, pcb_ptr->IFSRC, 0);
   }
} /* Endbody */

#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_send_internal
* Returned Values : error code
* Comments        :
*     Sends a UDP packet.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
uint_32 UDP_send_internal
   (
      UCB_STRUCT_PTR    ucb,        /* [IN] UDP layer context      */
      _ip_address       srcaddr,    /* [IN] source IP address      */
      _ip_address       destaddr,   /* [IN] destination IP address */
      uint_16           destport,   /* [IN] destination UDP port   */
      RTCSPCB_PTR       pcb_ptr,    /* [IN] packet to send         */
      uint_32           flags       /* [IN] optional flags         */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_HEADER_PTR       packet;
   register boolean     nochksum;
   uint_32              error;

   UDP_cfg_ptr = RTCS_getcfg(UDP);
   IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);

   error = RTCSPCB_insert_header(pcb_ptr, sizeof(UDP_HEADER));
   if (error) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb_ptr));
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      return error;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_PROTO(IPPROTO_UDP), 0);

   packet = (UDP_HEADER_PTR)RTCSPCB_DATA(pcb_ptr);
   RTCSPCB_DATA_TRANSPORT(pcb_ptr) = RTCSPCB_DATA(pcb_ptr);
   htons(packet->SRC_PORT, ucb->PORT);
   htons(packet->DEST_PORT, destport);
   htons(packet->LENGTH, RTCSPCB_SIZE(pcb_ptr));
   htons(packet->CHECKSUM, 0);

   if (flags & RTCS_MSG_O_NOCHKSUM) {
      nochksum = flags & RTCS_MSG_S_NOCHKSUM;
   } else {
      nochksum = ucb->BYPASS_TX;
   } /* Endif */

   if (!nochksum) {
      pcb_ptr->IP_SUM     = IP_Sum_PCB(RTCSPCB_SIZE(pcb_ptr), pcb_ptr);
      pcb_ptr->IP_SUM_PTR = packet->CHECKSUM;
   } /* Endif */

   return IP_send(pcb_ptr, IPPROTO_UDP, srcaddr, destaddr, flags);

} /* Endbody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_send_IF
* Returned Values : uint_32 (error code)
* Comments        :
*     Sends a UDP packet through a specific IP interface.
*
*END*-----------------------------------------------------------------*/

uint_32 UDP_send_IF
   (
      UCB_STRUCT_PTR    ucb,           /* [IN/OUT] UDP layer context   */
      pointer           dest_if,       /* [IN]     dest interface      */
      uint_16           dest_port,     /* [IN]     dest port           */
      RTCSPCB_PTR       pcb_ptr        /* [IN/OUT] packet to send      */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_HEADER_PTR packet;
   uint_32        error;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_TOTAL++);

   if (dest_port == 0) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_DISCARDED++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_PORT_ZERO);
      RTCSPCB_free(pcb_ptr);
      return RTCSERR_UDP_PORT_ZERO;
   } /* Endif */

   error = RTCSPCB_insert_header(pcb_ptr, sizeof(UDP_HEADER));
   if (error) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_TX_MISSED++);
      IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_TX, error, (uint_32)pcb_ptr));
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      return error;
   } /* Endif */

   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_PROTO(IPPROTO_UDP), 0);

   packet = (UDP_HEADER_PTR)RTCSPCB_DATA(pcb_ptr);
   RTCSPCB_DATA_TRANSPORT(pcb_ptr) = RTCSPCB_DATA(pcb_ptr);
   htons(packet->SRC_PORT, ucb->PORT);
   htons(packet->DEST_PORT, dest_port);
   htons(packet->LENGTH, RTCSPCB_SIZE(pcb_ptr));
   htons(packet->CHECKSUM, 0);

   pcb_ptr->IP_SUM     = IP_Sum_PCB(RTCSPCB_SIZE(pcb_ptr), pcb_ptr);
   pcb_ptr->IP_SUM_PTR = packet->CHECKSUM;

   return IP_send_IF(pcb_ptr, IPPROTO_UDP, dest_if);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_service6
* Returned Values : void
* Comments        :
*     Process incoming UDP packets for IPv6 family.  Called from IP6_service.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6
void UDP_service6
   (
      RTCSPCB_PTR    pcb_ptr,          /* [IN/OUT] incoming packet */
      pointer        dummy             /* [IN]     not used        */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_HEADER_PTR       packet;
   uint_16              port, src_port, len, sum;
   UCB_STRUCT_PTR       ucb_ptr = NULL;
   UCB_STRUCT_PTR       _PTR_ search_ptr;
  /*IPv6 part************/
   IP6_HEADER_PTR          ip6_packet;
   in6_addr                ipv6source;
   in6_addr                ipv6dest;
   uint_32              error;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_TOTAL++);
   packet = (UDP_HEADER_PTR)RTCSPCB_DATA(pcb_ptr);
   RTCSPCB_DATA_TRANSPORT(pcb_ptr) = RTCSPCB_DATA(pcb_ptr);
   port = ntohs(packet->DEST_PORT);
   len = ntohs(packet->LENGTH);

   /*
   ** Make sure that
   **    sizeof(UDP_HEADER) <= pktlen <= RTCSPCB_SIZE(pcb)
   **
   ** Note: If RTCSPCB_SIZE(pcb) is too small, then pktlen is
   **       undefined.  However, if the following conditions pass,
   **       then RTCSPCB_SIZE(pcb) >= sizeof(UDP_HEADER), and
   **       therefore, pktlen is valid.
   */
   if(
        len < sizeof(UDP_HEADER)
     )
   {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_SMALL_DGRAM++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   if(
        RTCSPCB_SIZE(pcb_ptr) < len
     )
   {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_SMALL_PKT++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(pcb_ptr, RTCS_LOGCTRL_PROTO(IPPROTO_UDP), 0);

   /*
   ** Port zero is an illegal destination
   */
   if(
        port == 0
     )
   {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_BAD_PORT++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   /*
   ** Find a listening UCB, and move it to the front.
   ** There are multiple lists to search. If the UNICAST bit in the pcb is not
   ** set, we will only check the BOUND list, because this is the only list
   ** that can receive Multicast or Broadcast packets. No ucb that is bound to
   ** a specific local IP can receive a multicast or broadcast packet.
   */

   /*convert packet to IP6_HEADER_PTR type*/
   ip6_packet= (IP6_HEADER_PTR)RTCSPCB_DATA_NETWORK(pcb_ptr);

   /* extract source port number */
   src_port = ntohs(packet->SRC_PORT);

   /* using IPv6 copy macros to copy the IPv6 address structure */
   IN6_ADDR_COPY((in6_addr *)ip6_packet->SOURCE, &ipv6source);
   IN6_ADDR_COPY((in6_addr *)ip6_packet->DEST, &ipv6dest);

   /*
   ** Only search this list if the packet might be unicast.
   ** The search criterion is matching host PORT/IP and matching peer PORT/IP
   */
   if(
        pcb_ptr->TYPE & RTCSPCB_TYPE_UNICAST
     )
   {
      RTCS_LIST_PEEK(UDP_cfg_ptr->OPEN_UCB_HEAD, ucb_ptr);
      if(
            ucb_ptr &&(
                        ucb_ptr->PORT != port                                       ||
                        (!IN6_ARE_ADDR_EQUAL(&(ucb_ptr->REMOTE_HOST6), &ipv6source))||
                        ucb_ptr->REMOTE_PORT != src_port                            ||
                        (!IN6_ARE_ADDR_EQUAL(&(ucb_ptr->IPV6ADDR), &ipv6dest))
                      )
        )
      {
            ucb_ptr = NULL;
            RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr)
            {
                if(
                    (*search_ptr)->PORT == port &&
                    (IN6_ARE_ADDR_EQUAL(&((*search_ptr)->REMOTE_HOST6), &ipv6source))&&
                    (*search_ptr)->REMOTE_PORT == src_port &&
                    (IN6_ARE_ADDR_EQUAL(&((*search_ptr)->IPV6ADDR), &ipv6dest))
                  )
                {
                    ucb_ptr = *search_ptr;
                    RTCS_LIST_DEL(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr);
                    RTCS_LIST_INS(UDP_cfg_ptr->OPEN_UCB_HEAD, ucb_ptr);
                    break;
                } /* Endif */
            } /* End SEARCH */
      } /* Endif */
   } /* Endif */

#if RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST && RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   /*
   ** If no OPEN ucb matches, search the LBOUND ucb list,
   ** allowing multicast.  The local port must match the
   ** packet destination port.
   */
   if(!ucb_ptr && UDP_cfg_ptr->LBOUND_UCB_HEAD)
   {
   /*TODO:tihs part of code looks like never was tested and work at all*/
        RTCS_LIST_PEEK(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
        if(
            ucb_ptr &&
                        (
                            (ucb_ptr->PORT != port) ||
                            (!IN6_IS_ADDR_MULTICAST(&ipv6dest)
                        ) ||

                        (
                            !IGMP_is_member (&ucb_ptr->MCB_PTR, pcb_ptr->IFSRC, ipdest)

                        )

          )
        {
            ucb_ptr = NULL;
            RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)
            {
                if (
                        (
                            (*search_ptr)->PORT == port) &&
                            (IN_MULTICAST(ipdest)) &&
                            (IGMP_is_member(&(*search_ptr)->MCB_PTR, pcb_ptr->IFSRC, ipdest))
                        )
                {
                    ucb_ptr = *search_ptr;
                    RTCS_LIST_DEL(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr);
                    RTCS_LIST_INS(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
                    break;
                } /* Endif */
            } /* End SEARCH */
        } /* Endif */
    } /* Endif */
#else

   /*
   ** If no OPEN ucb matches, search the LBOUND ucb list.
   ** Only search this list if the packet could be unicast.
   ** The search criterion is a matching local IP/PORT
   */
   if(
        !ucb_ptr && UDP_cfg_ptr->LBOUND_UCB_HEAD &&
        (pcb_ptr->TYPE & RTCSPCB_TYPE_UNICAST)
     )
   {
        RTCS_LIST_PEEK(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
        if(
            ucb_ptr &&
            ucb_ptr->PORT != port ||
            (!IN6_ARE_ADDR_EQUAL(&(ucb_ptr->IPV6ADDR), &ipv6dest))
         )
        {
            ucb_ptr = NULL;
            RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)
            {
                if(
                    (*search_ptr)->PORT == port &&
                    IN6_ARE_ADDR_EQUAL(&((*search_ptr)->IPV6ADDR), &ipv6dest)
                  )
                {
                    ucb_ptr = *search_ptr;
                    RTCS_LIST_DEL(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr);
                    RTCS_LIST_INS(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
                    break;
                } /* Endif */
            } /* End SEARCH */
        }/* Endif */
    } /* Endif */

#endif

   /*
   ** If no LBOUND ucb matches, search the BOUND ucb list
   ** Always search this list, regardless of packet type.
   ** The local port must match the packet destination port
   */
   if(!ucb_ptr && UDP_cfg_ptr->BOUND_UCB_HEAD)
   {
        RTCS_LIST_PEEK(UDP_cfg_ptr->BOUND_UCB_HEAD, ucb_ptr);
        if(ucb_ptr && ucb_ptr->PORT != port)
        {
            ucb_ptr = NULL;
            RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr)
            {
                if((*search_ptr)->PORT == port)
                {
                    ucb_ptr = *search_ptr;
                    RTCS_LIST_DEL(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr);
                    RTCS_LIST_INS(UDP_cfg_ptr->BOUND_UCB_HEAD, ucb_ptr);
                    break;
                } /* Endif */
            } /* End SEARCH */
        }/* Endif */

      /* Check the message was multicast, and if the ucb is a member */
        if (
                ucb_ptr &&
                IN6_IS_ADDR_MULTICAST(&ipv6dest)
           )
        {
            ucb_ptr = NULL;
        } /* Endif */
    } /* Endif */

    /* Lets check scope_id there binded socket and scope _id of input packet*/
    error = ip6_if_get_scope_id(pcb_ptr->IFSRC);

    /*
     * If scope_id of input packet is different from UCB scope_id set UCB to NULL and
     * UCB to NULL, to discard packet and generate error.
     */
    if(ucb_ptr->IF_SCOPE_ID)
    {
        if(ucb_ptr->IF_SCOPE_ID != error)
        {
            ucb_ptr = NULL;
        }
    }
   /*
   ** If no UCB was found on the destination port, discard packet and generate
   ** a ICMP error packet
   */
   if(!ucb_ptr)
   {
        IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
        IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_NO_PORT++);
        ICMP6_send_error (ICMP6_TYPE_DEST_UNREACH, ICMP6_CODE_DU_PORT_UNREACH,
                                0   /* [IN] error parameter */,
                                pcb_ptr   /* [IN] the packet which caused the error */);
        RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
        RTCSPCB_free(pcb_ptr);
        return;
   } /* Endif */


   /*
   ** Verify the checksum if present
   */
   if(ntohs(packet->CHECKSUM) && !ucb_ptr->BYPASS_RX)
   {
        /*IP6 use different method for calculation checksum*/
        sum = IP6_Sum_pseudo(RTCSPCB_SIZE(pcb_ptr), pcb_ptr);
        sum = IP_Sum_PCB(sum, pcb_ptr);

        if (sum != 0xFFFF)
        {
            IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
            IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_BAD_CHECKSUM++);
            RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_CHECKSUM);
            RTCSPCB_free(pcb_ptr);
            return;
        } /* Endif */
   } /* Endif */


   /*
   ** Although RTCSPCB_shrink() always shrinks the first fragment,
   ** this is OK, because if RTCSPCB_SIZE(pcb_ptr) != len, then the
   ** datagram must have originated from another host, in which
   ** case the PCB only has one fragment.
   **
   ** If the packet originated on this host (and was looped back),
   ** the datagram could have several fragments, but in this case,
   ** RTCSPCB_SIZE(pcb_ptr) == len, so we'll always shrink zero bytes.
   */
   error = RTCSPCB_shrink(pcb_ptr, RTCSPCB_SIZE(pcb_ptr) - len);
   if(error)
   {
        IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_ERRORS++);
        IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_RX, error, (uint_32)pcb_ptr));
        RTCSLOG_PCB_FREE(pcb_ptr, error);
        RTCSPCB_free(pcb_ptr);
        return;
   } /* Endif */
   error = RTCSPCB_next(pcb_ptr, sizeof(UDP_HEADER));
   if(error)
   {
        IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_ERRORS++);
        IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_RX, error, (uint_32)pcb_ptr));
        RTCSLOG_PCB_FREE(pcb_ptr, error);
        RTCSPCB_free(pcb_ptr);
        return;
   } /* Endif */

   ucb_ptr->SERVICE(pcb_ptr, ucb_ptr);

} /* Endbody */

#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_service
* Returned Values : void
* Comments        :
*     Process incoming UDP packets.  Called from IP_service.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void UDP_service
   (
      RTCSPCB_PTR    pcb_ptr,          /* [IN/OUT] incoming packet */
      pointer        dummy             /* [IN]     not used        */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_HEADER_PTR       packet;
   uint_16              port, src_port, len, sum;
   UCB_STRUCT_PTR       ucb_ptr = NULL;
   UCB_STRUCT_PTR       _PTR_ search_ptr;
   _ip_address          ipdest, ipsource;
   uint_32              error;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_TOTAL++);
   packet = (UDP_HEADER_PTR)RTCSPCB_DATA(pcb_ptr);
   RTCSPCB_DATA_TRANSPORT(pcb_ptr) = RTCSPCB_DATA(pcb_ptr);
   port = ntohs(packet->DEST_PORT);
   len = ntohs(packet->LENGTH);

   /*
   ** Make sure that
   **    sizeof(UDP_HEADER) <= pktlen <= RTCSPCB_SIZE(pcb)
   **
   ** Note: If RTCSPCB_SIZE(pcb) is too small, then pktlen is
   **       undefined.  However, if the following conditions pass,
   **       then RTCSPCB_SIZE(pcb) >= sizeof(UDP_HEADER), and
   **       therefore, pktlen is valid.
   */
   if (len < sizeof(UDP_HEADER)) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_SMALL_DGRAM++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */
   if (RTCSPCB_SIZE(pcb_ptr) < len) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_SMALL_PKT++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   RTCSLOG_PCB_READ(pcb_ptr, RTCS_LOGCTRL_PROTO(IPPROTO_UDP), 0);

   /*
   ** Port zero is an illegal destination
   */
   if (port == 0) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_BAD_PORT++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_HEADER);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   /*
   ** Find a listening UCB, and move it to the front.
   ** There are multiple lists to search. If the UNICAST bit in the pcb is not
   ** set, we will only check the BOUND list, because this is the only list
   ** that can receive Multicast or Broadcast packets. No ucb that is bound to
   ** a specific local IP can receive a multicast or broadcast packet.
   */
   src_port = ntohs(packet->SRC_PORT);
   ipsource = IP_source(pcb_ptr);
   ipdest = IP_dest(pcb_ptr);

   /*
   ** Only search this list if the packet might be unicast.
   ** The search criterion is matching host PORT/IP and matching peer PORT/IP
   */
   if (pcb_ptr->TYPE & RTCSPCB_TYPE_UNICAST) {
      RTCS_LIST_PEEK(UDP_cfg_ptr->OPEN_UCB_HEAD, ucb_ptr);
      if (ucb_ptr &&
         (ucb_ptr->PORT != port ||
          ucb_ptr->REMOTE_HOST != ipsource ||
          ucb_ptr->REMOTE_PORT != src_port ||
          ucb_ptr->IPADDR != ipdest))
      {
         ucb_ptr = NULL;
         RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr) {
            if ((*search_ptr)->PORT == port &&
            (*search_ptr)->REMOTE_HOST == ipsource &&
            (*search_ptr)->REMOTE_PORT == src_port &&
            (*search_ptr)->IPADDR == ipdest)
            {
               ucb_ptr = *search_ptr;
               RTCS_LIST_DEL(UDP_cfg_ptr->OPEN_UCB_HEAD, search_ptr);
               RTCS_LIST_INS(UDP_cfg_ptr->OPEN_UCB_HEAD, ucb_ptr);
               break;
            } /* Endif */
         } /* End SEARCH */
      } /* Endif */
   } /* Endif */

#if RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST && RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   /*
   ** If no OPEN ucb matches, search the LBOUND ucb list,
   ** allowing multicast.  The local port must match the
   ** packet destination port.
   */
   if (!ucb_ptr && UDP_cfg_ptr->LBOUND_UCB_HEAD)  {
      RTCS_LIST_PEEK(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
      if (ucb_ptr && ((ucb_ptr->PORT != port) || ((IN_MULTICAST(ipdest)) && (!IGMP_is_member(&ucb_ptr->MCB_PTR, pcb_ptr->IFSRC, ipdest)))))  {
         ucb_ptr = NULL;
         RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)  {
            if ((((*search_ptr)->PORT == port) && (!IN_MULTICAST(ipdest))) ||
                (((*search_ptr)->PORT == port) && (IN_MULTICAST(ipdest)) && (IGMP_is_member(&(*search_ptr)->MCB_PTR, pcb_ptr->IFSRC, ipdest))))  {
               ucb_ptr = *search_ptr;
               RTCS_LIST_DEL(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr);
               RTCS_LIST_INS(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
               break;
            } /* Endif */
         } /* End SEARCH */
      } /* Endif */
   } /* Endif */
#else

   /*
   ** If no OPEN ucb matches, search the LBOUND ucb list.
   ** Only search this list if the packet could be unicast.
   ** The search criterion is a matching local IP/PORT
   */
   if (!ucb_ptr && UDP_cfg_ptr->LBOUND_UCB_HEAD &&
       (pcb_ptr->TYPE & RTCSPCB_TYPE_UNICAST)) {
      RTCS_LIST_PEEK(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
      if (ucb_ptr && ucb_ptr->PORT != port ||
          ucb_ptr->IPADDR != ipdest)
      {
         ucb_ptr = NULL;
         RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr)  {
            if ((*search_ptr)->PORT == port &&
                (*search_ptr)->IPADDR == ipdest)
            {
               ucb_ptr = *search_ptr;
               RTCS_LIST_DEL(UDP_cfg_ptr->LBOUND_UCB_HEAD, search_ptr);
               RTCS_LIST_INS(UDP_cfg_ptr->LBOUND_UCB_HEAD, ucb_ptr);
               break;
            } /* Endif */
         } /* End SEARCH */
      }/* Endif */
   } /* Endif */

#endif

   /*
   ** If no LBOUND ucb matches, search the BOUND ucb list
   ** Always search this list, regardless of packet type.
   ** The local port must match the packet destination port
   */
   if (!ucb_ptr && UDP_cfg_ptr->BOUND_UCB_HEAD)  {
      RTCS_LIST_PEEK(UDP_cfg_ptr->BOUND_UCB_HEAD, ucb_ptr);
      if (ucb_ptr && ucb_ptr->PORT != port)  {
         ucb_ptr = NULL;
         RTCS_LIST_SEARCHMOD_NOHEAD(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr)  {
            if ((*search_ptr)->PORT == port)  {
               ucb_ptr = *search_ptr;
               RTCS_LIST_DEL(UDP_cfg_ptr->BOUND_UCB_HEAD, search_ptr);
               RTCS_LIST_INS(UDP_cfg_ptr->BOUND_UCB_HEAD, ucb_ptr);
               break;
            } /* Endif */
         } /* End SEARCH */
      }/* Endif */

      /* Check the message was multicast, and if the ucb is a member */
      if (ucb_ptr && IN_MULTICAST(ipdest)) {
#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
         if (!IGMP_is_member(&ucb_ptr->MCB_PTR, pcb_ptr->IFSRC, ipdest)) {
            ucb_ptr = NULL;
         }
#else
         ucb_ptr = NULL;
#endif
      } /* Endif */
   } /* Endif */

   /*
   ** If no UCB was found on the destination port, discard packet and generate
   ** a ICMP error packet
   */
   if (!ucb_ptr) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_NO_PORT++);
      #if RTCSCFG_ENABLE_ICMP
      ICMP_send_error(ICMPTYPE_DESTUNREACH, ICMPCODE_DU_PORT_UNREACH, 0, pcb_ptr, -1);
      #endif
      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   /*
   ** Verify the checksum if present
   */
   if (ntohs(packet->CHECKSUM) && !ucb_ptr->BYPASS_RX) {
      sum = IP_Sum_pseudo(len, pcb_ptr, -1);
      sum = IP_Sum_PCB(sum, pcb_ptr);
      if (sum != 0xFFFF) {
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_DISCARDED++);
         IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.ST_RX_BAD_CHECKSUM++);
         RTCSLOG_PCB_FREE(pcb_ptr, RTCSERR_UDP_BAD_CHECKSUM);
         RTCSPCB_free(pcb_ptr);
         return;
      } /* Endif */
   } /* Endif */

   /*
   ** Although RTCSPCB_shrink() always shrinks the first fragment,
   ** this is OK, because if RTCSPCB_SIZE(pcb_ptr) != len, then the
   ** datagram must have originated from another host, in which
   ** case the PCB only has one fragment.
   **
   ** If the packet originated on this host (and was looped back),
   ** the datagram could have several fragments, but in this case,
   ** RTCSPCB_SIZE(pcb_ptr) == len, so we'll always shrink zero bytes.
   */
   error = RTCSPCB_shrink(pcb_ptr, RTCSPCB_SIZE(pcb_ptr) - len);
   if (error) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_ERRORS++);
      IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_RX, error, (uint_32)pcb_ptr));
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */
   error = RTCSPCB_next(pcb_ptr, sizeof(UDP_HEADER));
   if (error) {
      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_ERRORS++);
      IF_UDP_STATS_ENABLED(RTCS_seterror(&UDP_cfg_ptr->STATS.ERR_RX, error, (uint_32)pcb_ptr));
      RTCSLOG_PCB_FREE(pcb_ptr, error);
      RTCSPCB_free(pcb_ptr);
      return;
   } /* Endif */

   ucb_ptr->SERVICE(pcb_ptr, ucb_ptr);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_process6
* Returned Values : void
* Comments        :
*     Process valid incoming UDP packets for IPv6 family. Called from UDP_service.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP6

void UDP_process6
   (
      RTCSPCB_PTR    pcb_ptr,          /* [IN/OUT] incoming packet */
      UCB_STRUCT_PTR ucb_ptr           /* [IN]     target UCB      */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_PARM_PTR         req_ptr;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   /*
   ** If there is a pending request, unblock it
   */
   RTCS_QUEUE_PEEK(ucb_ptr->RHEAD, ucb_ptr->RTAIL, req_ptr);
   if (req_ptr)
   {
      RTCS_QUEUE_DEL_NONEMPTY(ucb_ptr->RHEAD, ucb_ptr->RTAIL);
      IN6_ADDR_COPY((in6_addr *)(((IP_V6_HEADER_PTR)(pcb_ptr->NETWORK_LAYER))->SOURCE),&(req_ptr->ipv6address));
      req_ptr->udpport   = UDP_source(pcb_ptr);
      /* here we get scope_id */
      req_ptr->if_scope_id = ip6_if_get_scope_id(pcb_ptr->IFSRC);
      if (req_ptr->udpword > RTCSPCB_SIZE(pcb_ptr)) {
         req_ptr->udpword = RTCSPCB_SIZE(pcb_ptr);
      } /* Endif */
      RTCSPCB_memcopy(pcb_ptr, req_ptr->udpptr, 0, req_ptr->udpword);
      ((SOCKET_STRUCT_PTR)ucb_ptr->SOCKET)->LINK_OPTIONS.RX = pcb_ptr->LINK_OPTIONS.RX;
      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
      RTCSCMD_complete(req_ptr, RTCS_OK);
      return;
   }
   else if (ucb_ptr->PCOUNT >= _UDP_max_queue_size)
   {
       IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_MISSED++);
       RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
       RTCSPCB_free(pcb_ptr);
       return;
   /*
   ** Otherwise, enqueue the packet
   */
   }
   else
   {

        RTCS_QUEUE_INS(ucb_ptr->PHEAD, ucb_ptr->PTAIL, pcb_ptr);
        ucb_ptr->PCOUNT++;

      /*
      ** Notify socket layer in case some task is blocked on select()
      ** waiting for this data.
      */
        SOCK_select_signal(ucb_ptr->SOCKET, UDP_SOCKET_ENQUEUED);
   } /* Endif */

} /* Endbody */

#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_process
* Returned Values : void
* Comments        :
*     Process valid incoming UDP packets.  Called from UDP_service.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4
void UDP_process
   (
      RTCSPCB_PTR    pcb_ptr,          /* [IN/OUT] incoming packet */
      UCB_STRUCT_PTR ucb_ptr           /* [IN]     target UCB      */
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UDP_PARM_PTR         req_ptr;

   UDP_cfg_ptr = RTCS_getcfg(UDP);

   /*
   ** If there is a pending request, unblock it
   */
   RTCS_QUEUE_PEEK(ucb_ptr->RHEAD, ucb_ptr->RTAIL, req_ptr);
   if (req_ptr) {

      RTCS_QUEUE_DEL_NONEMPTY(ucb_ptr->RHEAD, ucb_ptr->RTAIL);

      req_ptr->ipaddress = IP_source(pcb_ptr);
      req_ptr->udpport   = UDP_source(pcb_ptr);

      if (req_ptr->udpword > RTCSPCB_SIZE(pcb_ptr)) {
         req_ptr->udpword = RTCSPCB_SIZE(pcb_ptr);
      } /* Endif */

      RTCSPCB_memcopy(pcb_ptr, req_ptr->udpptr, 0, req_ptr->udpword);

      ((SOCKET_STRUCT_PTR)ucb_ptr->SOCKET)->LINK_OPTIONS.RX = pcb_ptr->LINK_OPTIONS.RX;

      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
      RTCSCMD_complete(req_ptr, RTCS_OK);
      return;

   } else if (ucb_ptr->PCOUNT >= _UDP_max_queue_size) {

      IF_UDP_STATS_ENABLED(UDP_cfg_ptr->STATS.COMMON.ST_RX_MISSED++);
      RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
      RTCSPCB_free(pcb_ptr);
      return;

   /*
   ** Otherwise, enqueue the packet
   */
   } else {

      RTCS_QUEUE_INS(ucb_ptr->PHEAD, ucb_ptr->PTAIL, pcb_ptr);
      ucb_ptr->PCOUNT++;

      /*
      ** Notify socket layer in case some task is blocked on select()
      ** waiting for this data.
      */
      SOCK_select_signal(ucb_ptr->SOCKET, UDP_SOCKET_ENQUEUED);
   } /* Endif */

} /* Endbody */
#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_close
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*    _CODE_PTR_         udpservice  not used
*     _ip_address       ipaddress   not used
*     uint_16           udpport     not used
*     uint_16           udpflags    not used
*     pointer           udpptr      not used
*     uint_32           udpword     not used
*
* Comments        :
*     Closes a UCB.
*
*END*-----------------------------------------------------------------*/

void UDP_close
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   UDP_CFG_STRUCT_PTR   UDP_cfg_ptr;
   UCB_STRUCT_PTR       ucb, _PTR_ tmp;
   UCB_STRUCT_PTR _PTR_ search_ptr;       /* Temporary UCB pointer */
   UDP_PARM_PTR         req, nextreq;     /* Temporary request pointers */
   RTCSPCB_PTR          pcb, nextpcb;     /* Temporary packet pointers */
   uint_32              error;


   UDP_cfg_ptr = RTCS_getcfg(UDP);
   ucb = parms->ucb;

   /*
   ** Remove the UCB from the chain of UCBs
   */
   if (!ucb->PORT) {
      tmp = &(UDP_cfg_ptr->GROUND_UCB_HEAD);
   } else if (!ucb->REMOTE_HOST) {
      if (ucb->IPADDR) {
         tmp = &(UDP_cfg_ptr->LBOUND_UCB_HEAD);
      } else {
         tmp = &(UDP_cfg_ptr->BOUND_UCB_HEAD);
      } /* Endif */
   } else {
      tmp = &(UDP_cfg_ptr->OPEN_UCB_HEAD);
   } /* Endif */

   RTCS_LIST_SEARCHMOD(*tmp, search_ptr) {
      if (*search_ptr == ucb) {
         break;
      } /* Endif */
   } /* End SEARCH */

   if (!*search_ptr) {
      RTCSCMD_complete(parms, RTCSERR_UDP_UCB_CLOSE);
      return;
   } /* Endif */

   RTCS_LIST_DEL(tmp, search_ptr);

   /*
   ** Free all queued packets
   */
   RTCS_QUEUE_DELALL(ucb->PHEAD, ucb->PTAIL, pcb, nextpcb) {
      RTCSLOG_PCB_FREE(pcb, RTCS_OK);
      RTCSPCB_free(pcb);
   } /* End DELALL */

   /*
   ** Release pending receive requests from host
   */
   RTCS_QUEUE_DELALL(ucb->RHEAD, ucb->RTAIL, req, nextreq) {
      RTCSCMD_complete(req, RTCSERR_READ_ABORTED);
   } /* End DELALL */

   /*
   ** Notify socket layer in case some task is blocked on select()
   ** waiting for data on this socket.
   */
   SOCK_select_signal(ucb->SOCKET, UDP_SOCKET_CLOSE);


   /*
   ** Leave all joined multicast groups
   */
   if (ucb->IGMP_LEAVEALL) {
      ucb->IGMP_LEAVEALL(&ucb->MCB_PTR);
   } /* Endif */

   /*
   ** Free the UCB memory
   */
   error = _mem_free(ucb);
   if (error) {
      RTCSCMD_complete(parms, RTCSERR_UDP_UCB_FREE);
      return;
   } /* Endif */

   RTCSCMD_complete(parms, RTCS_OK);
   return;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_source
* Returned Value  : source UDP port
* Comments        :
*     Return source UDP port of packet.
*
*END*-----------------------------------------------------------------*/

uint_16 UDP_source
   (
      RTCSPCB_PTR    pcb      /* [IN] packet to find source of */
   )
{ /* Body */
   uchar_ptr srcptr = ((UDP_HEADER_PTR)RTCSPCB_DATA_TRANSPORT(pcb))->SRC_PORT;

   return ntohs(srcptr);

} /* Endbody */


/* EOF */
