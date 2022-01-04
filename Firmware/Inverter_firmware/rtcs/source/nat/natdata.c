/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: natdata.c$
* $Version : 3.8.4.0$
* $Date    : Aug-29-2011$
*
* Comments:
*
*   This file contains the NAT data strucutre interface functions.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <rtcs_prv.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_insert
* Returned Value  : NAT_SESSION_STRUCT_PTR or NULL if error
* Comments        :
*        Creates a new session. Will create a new session for ICMP error
*     messages, but won't link them into the IP_RADIX trees.
*
*END*-----------------------------------------------------------------*/

static void NAT_insert_find_unused
   (
      pointer _PTR_  node_ptr_ptr,
      pointer        data_ptr
   )
{ /* Body */
    boolean _PTR_    found_ptr = data_ptr;

   /* TRUE if there is something there, FALSE if not */
   *found_ptr = (*node_ptr_ptr != NULL);

} /* Endbody */



static void NAT_insert_insert
   (
      pointer _PTR_  node_ptr_ptr,
      pointer        data_ptr
   )
{ /* Body */
   IPRADIX_NODE_PTR        node_ptr = (pointer) node_ptr_ptr;
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = data_ptr;

   /* Link according to incoming or outgoing */
   if ((nat_session_ptr->STATE & NAT_STATE_INSERT_IN) == NAT_STATE_INSERT_IN) {
      nat_session_ptr->NEXT_IN = node_ptr->DATA;
   } else {
      nat_session_ptr->NEXT_OUT = node_ptr->DATA;
   } /* Endif */

   nat_session_ptr->STATE &= ~NAT_STATE_INS_ERR;
   node_ptr->DATA = nat_session_ptr;

} /* Endbody */


NAT_SESSION_STRUCT_PTR NAT_insert
   (
      IP_HEADER_PTR     ip_header_ptr, /* [IN] Pointer to the IP datagram */
      uint_32_ptr       error_ptr      /* [OUT] Reason for returning NULL */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   NAT_SESSION_STRUCT_PTR  new_nat_session_ptr;
   uint_16_ptr             last_used_port_ptr;
   TRANSPORT_UNION         transport;
   uint_32                 key;
   uint_16                 icmp_type, port;
   boolean                 used = FALSE;
   boolean                 route_exists;
   
   /* Get a pointer to the upper level protocol */
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);

   /* Allocate memory */
   new_nat_session_ptr = RTCS_part_alloc_zero(nat_cfg_ptr->SESSION_PART);

   if (new_nat_session_ptr==NULL) {
      *error_ptr = RTCSERR_OUT_OF_MEMORY;
      return NULL;
   } /* Endid */

   /* Setup event information */
   new_nat_session_ptr->TIMEOUT.SESSION_PTR = new_nat_session_ptr;

   /* Copy information (works for all protocols) */
   new_nat_session_ptr->PRV_HST = ntohl(ip_header_ptr->SOURCE);
   new_nat_session_ptr->PUB_HST = ntohl(ip_header_ptr->DEST);

   /* Choose a NAT host IP. */
   new_nat_session_ptr->NAT_HST = IP_route_find(new_nat_session_ptr->PUB_HST, 0);
   route_exists = new_nat_session_ptr->NAT_HST != INADDR_ANY;

   new_nat_session_ptr->IP_PROT = ntohc(ip_header_ptr->PROTOCOL);
   new_nat_session_ptr->SNAT_OR_DNAT = SNAT;


   /* No route to host available */
   if (!route_exists) {
      RTCS_part_free(new_nat_session_ptr);
      *error_ptr = RTCSERR_IP_UNREACH;
      return NULL;
   } /* Endif */

   /* Do protocol specific initialization */
   switch (new_nat_session_ptr->IP_PROT) {
      case IPPROTO_ICMP:
         icmp_type = ntohc(transport.ICMP_PTR->TYPE);
         if (icmp_type == ICMPTYPE_ECHO_REQ     ||
             icmp_type == ICMPTYPE_TIME_REQ     ||
             icmp_type == ICMPTYPE_INFO_REQ     ||
             icmp_type == ICMPTYPE_AM_REQ)
         {
            new_nat_session_ptr->PRV_PORT = ntohs(transport.ECHO_PTR->SEQ);     
            last_used_port_ptr = &nat_cfg_ptr->ICMP_ID;
         } else {
            if (ICMPTYPE_ISQUERY(icmp_type)) {
               /* This means a reply without a request. Should not happen */
               RTCS_part_free(new_nat_session_ptr);
               *error_ptr = RTCSERR_NAT_UNEXPECTED;
               new_nat_session_ptr = NULL;
            } else {
               /* We will be deleting the session, so don't store it */
               *error_ptr = RTCS_OK;
            } /* Endif*/
            return new_nat_session_ptr;
         } /* Endif */

         new_nat_session_ptr->TIMEOUT.TIME = nat_cfg_ptr->TIMEOUT_ICMP;
         break;

      case IPPROTO_TCP:
         new_nat_session_ptr->PRV_PORT = ntohs(transport.TCP_PTR->source_port);
         new_nat_session_ptr->PUB_PORT = ntohs(transport.TCP_PTR->dest_port);           
         last_used_port_ptr = &nat_cfg_ptr->TCP_PORT;
         new_nat_session_ptr->TIMEOUT.TIME = nat_cfg_ptr->TIMEOUT_TCP;
         break;

      case IPPROTO_UDP:
         new_nat_session_ptr->PRV_PORT = ntohs(transport.UDP_PTR->SRC_PORT);
         new_nat_session_ptr->PUB_PORT = ntohs(transport.UDP_PTR->DEST_PORT);
         last_used_port_ptr = &nat_cfg_ptr->UDP_PORT;
         new_nat_session_ptr->TIMEOUT.TIME = nat_cfg_ptr->TIMEOUT_UDP;
         break;

      default:    /* Should never get here */
         RTCS_part_free(new_nat_session_ptr);
         // CR-TBD no error returned
         *error_ptr = RTCSERR_NAT_UNEXPECTED;
         return NULL;
   } /* Enswitch */


   /*
   ** Reserve a port or query id.
   ** Take the last used number from the last_used_num pointer, and
   ** decrement by 1. Search radix tree to see if used, and
   ** continue until an unused number is found. When the #define
   ** minimum NAT port is reached wrap up to the maximum port.
   ** If we return to the *last_used_num, they are
   ** all reserved, so we free the session and return NULL.
   */   
   
   port = NAT_NEXT_PORT(nat_cfg_ptr,*last_used_port_ptr);

   while (port != *last_used_port_ptr) {

      /* Search tree for port/id number */
      key = NAT_KEY(new_nat_session_ptr->IP_PROT,port);
      IPRADIX_insert(&nat_cfg_ptr->ROOT_IN, key, 0xFFFFFFFF, 0, NAT_insert_find_unused, &used);

      if (used == FALSE) {          /* We found a free port/id number */
            new_nat_session_ptr->NAT_PORT = port;
         *last_used_port_ptr = port;
      } else {                      /* Port/id is used, keep looking */
         used = FALSE;
         port=NAT_NEXT_PORT(nat_cfg_ptr,port);
      }/* Endif */
   }/* Endwhile */
   
      if (port != new_nat_session_ptr->NAT_PORT) {
         *error_ptr = RTCSERR_NAT_NO_SESSION;
         RTCS_part_free(new_nat_session_ptr);
      return NULL;
   }/* Endif */

   /* Update STATS */
   nat_cfg_ptr->STATS.ST_SESSIONS++;
   nat_cfg_ptr->STATS.ST_SESSIONS_SNAT++;
   nat_cfg_ptr->STATS.ST_SESSIONS_OPEN++;
   if (nat_cfg_ptr->STATS.ST_SESSIONS_OPEN > nat_cfg_ptr->STATS.ST_SESSIONS_OPEN_MAX) {
      nat_cfg_ptr->STATS.ST_SESSIONS_OPEN_MAX = nat_cfg_ptr->STATS.ST_SESSIONS_OPEN;
   } /* Endif */

   /*
   ** Call IPRADIX_insert on each radix tree. Pass in a function that will
   ** link the session struct onto a linked list starting at the node.
   ** If last_used_port_ptr == NULL, we are routing a one time ICMP message and
   ** there is no need to save the session.
   */
   /* Insert into incoming packet tree */
   new_nat_session_ptr->STATE = NAT_STATE_INSERT_IN | NAT_STATE_INS_ERR;
   IPRADIX_insert(&nat_cfg_ptr->ROOT_IN, key, 0xFFFFFFFF, nat_cfg_ptr->RADIX_IN,
      NAT_insert_insert, new_nat_session_ptr);

   /* If the insert failed, we free the session struct */
   if (new_nat_session_ptr->STATE & NAT_STATE_INS_ERR) {
      NAT_delete(new_nat_session_ptr);
      *error_ptr = RTCSERR_OUT_OF_MEMORY;
      return NULL;
   } /* Endif */

   /* insert into outgoing packet tree */
   key = NAT_KEY(new_nat_session_ptr->IP_PROT,new_nat_session_ptr->PRV_PORT);
   new_nat_session_ptr->STATE = NAT_STATE_INSERT_OUT | NAT_STATE_INS_ERR;;

   IPRADIX_insert(&nat_cfg_ptr->ROOT_OUT, key, 0xFFFFFFFF, nat_cfg_ptr->RADIX_OUT,
      NAT_insert_insert, new_nat_session_ptr);

   /* If the insert failed, we free the session struct */
   if (new_nat_session_ptr->STATE & NAT_STATE_INS_ERR) {
      NAT_delete(new_nat_session_ptr);
      *error_ptr = RTCSERR_OUT_OF_MEMORY;
      return NULL;
   } /* Endif */

   /* Return a pointer to the new structure */
   return new_nat_session_ptr;

} /* Endbody */


NAT_SESSION_STRUCT_PTR NAT_insert_dnat
   (
      IP_HEADER_PTR        ip_header_ptr, /* [IN] Pointer to the IP datagram */
      DNAT_RULE_STRUCT_PTR dnat_rule_ptr,
      uint_32_ptr          error_ptr      /* [OUT] Reason for returning NULL */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = RTCS_getcfg(NAT);
   NAT_SESSION_STRUCT_PTR  new_nat_session_ptr;
   TRANSPORT_UNION         transport;
   uint_32                 key;
   boolean                 used = FALSE;

   /* Get a pointer to the upper level protocol */
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);

   /* Allocate memory */
   new_nat_session_ptr = RTCS_part_alloc_zero(nat_cfg_ptr->SESSION_PART);

   if (new_nat_session_ptr==NULL) {
      *error_ptr = RTCSERR_OUT_OF_MEMORY;
      return NULL;
   } /* Endid */

   /* Setup event information */
   new_nat_session_ptr->TIMEOUT.SESSION_PTR = new_nat_session_ptr;

   /* Copy information (works for all protocols) */
   new_nat_session_ptr->PRV_HST = dnat_rule_ptr->PRIVATE_IP; 
   new_nat_session_ptr->PUB_HST = ntohl(ip_header_ptr->SOURCE);
   new_nat_session_ptr->NAT_HST = ntohl(ip_header_ptr->DEST);
   new_nat_session_ptr->IP_PROT = dnat_rule_ptr->IP_PROTOCOL;
   new_nat_session_ptr->SNAT_OR_DNAT = DNAT;

   /* Do protocol specific initialization */
   switch (new_nat_session_ptr->IP_PROT) {
 
      case IPPROTO_TCP:
         new_nat_session_ptr->PUB_PORT = ntohs(transport.TCP_PTR->source_port);             
         new_nat_session_ptr->NAT_PORT = ntohs(transport.TCP_PTR->dest_port);
         new_nat_session_ptr->PRV_PORT = new_nat_session_ptr->NAT_PORT - 
            dnat_rule_ptr->PUBLIC_START_PORT + dnat_rule_ptr->PRIVATE_START_PORT;
         if (dnat_rule_ptr->TIMEOUT) {
            new_nat_session_ptr->TIMEOUT.TIME = dnat_rule_ptr->TIMEOUT;
         } else {            
            new_nat_session_ptr->TIMEOUT.TIME = nat_cfg_ptr->TIMEOUT_TCP;
         }
         break;

      case IPPROTO_UDP:
         new_nat_session_ptr->PUB_PORT = ntohs(transport.UDP_PTR->SRC_PORT);
         new_nat_session_ptr->NAT_PORT = ntohs(transport.UDP_PTR->DEST_PORT);
         new_nat_session_ptr->PRV_PORT = new_nat_session_ptr->NAT_PORT -
           dnat_rule_ptr->PUBLIC_START_PORT + dnat_rule_ptr->PRIVATE_START_PORT;
          if (dnat_rule_ptr->TIMEOUT) {
            new_nat_session_ptr->TIMEOUT.TIME = dnat_rule_ptr->TIMEOUT;
         } else {            
            new_nat_session_ptr->TIMEOUT.TIME = nat_cfg_ptr->TIMEOUT_UDP;
         }
         break;

      default:    /* Should never get here */
         RTCS_part_free(new_nat_session_ptr);
         *error_ptr = RTCSERR_NAT_UNEXPECTED;
         return NULL;
   } /* Enswitch */


 

   /* Update STATS */
   nat_cfg_ptr->STATS.ST_SESSIONS++;
   nat_cfg_ptr->STATS.ST_SESSIONS_DNAT++;
   nat_cfg_ptr->STATS.ST_SESSIONS_OPEN++;
   if (nat_cfg_ptr->STATS.ST_SESSIONS_OPEN > nat_cfg_ptr->STATS.ST_SESSIONS_OPEN_MAX) {
      nat_cfg_ptr->STATS.ST_SESSIONS_OPEN_MAX = nat_cfg_ptr->STATS.ST_SESSIONS_OPEN;
   } /* Endif */

   /*
   ** Call IPRADIX_insert on each radix tree. Pass in a function that will
   ** link the session struct onto a linked list starting at the node.
   ** If last_used_port_ptr == NULL, we are routing a one time ICMP message and
   ** there is no need to save the session.
   */
   /* Insert into incoming packet tree */
   key = NAT_KEY(new_nat_session_ptr->IP_PROT,new_nat_session_ptr->NAT_PORT);
   new_nat_session_ptr->STATE = NAT_STATE_INSERT_IN | NAT_STATE_INS_ERR;
   IPRADIX_insert(&nat_cfg_ptr->ROOT_IN, key, 0xFFFFFFFF, nat_cfg_ptr->RADIX_IN,
      NAT_insert_insert, new_nat_session_ptr);

   /* If the insert failed, we free the session struct */
   if (new_nat_session_ptr->STATE & NAT_STATE_INS_ERR) {
      NAT_delete(new_nat_session_ptr);
      *error_ptr = RTCSERR_OUT_OF_MEMORY;
      return NULL;
   } /* Endif */

   /* insert into outgoing packet tree */
   key = NAT_KEY(new_nat_session_ptr->IP_PROT,new_nat_session_ptr->PRV_PORT);
   new_nat_session_ptr->STATE = NAT_STATE_INSERT_OUT | NAT_STATE_INS_ERR;;

   IPRADIX_insert(&nat_cfg_ptr->ROOT_OUT, key, 0xFFFFFFFF, nat_cfg_ptr->RADIX_OUT,
      NAT_insert_insert, new_nat_session_ptr);

   /* If the insert failed, we free the session struct */
   if (new_nat_session_ptr->STATE & NAT_STATE_INS_ERR) {
      NAT_delete(new_nat_session_ptr);
      *error_ptr = RTCSERR_OUT_OF_MEMORY;
      return NULL;
   } /* Endif */

   /* Return a pointer to the new structure */
   return new_nat_session_ptr;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_lookup
* Returned Value  : NAT_SESSION_STRUCT_PTR or NULL no session found
* Comments        :
*        Returns a pointer to a session, if it exists.
*
*END*-----------------------------------------------------------------*/

struct nat_lookup {
   NAT_SESSION_STRUCT_PTR  NAT_SESSION_PTR;
   _ip_address             SRC;
   _ip_address             DEST;
   boolean                 TO_PRV_NET;
   uint_16                 PORT;
} ;

static void NAT_lookup_func
   (
      pointer _PTR_  node_ptr_ptr,
      pointer        data_ptr
      
   )
{ /* Body */
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = *node_ptr_ptr;
   struct nat_lookup _PTR_ testdata = data_ptr;
   
   /* Go through linked list of NAT_SESSION_STRUCTs */
   while (nat_session_ptr) {
      if (testdata->TO_PRV_NET) {   
         /* From public network to private network */
         if ((testdata->SRC == nat_session_ptr->PUB_HST) &&
             (testdata->DEST == nat_session_ptr->NAT_HST) && 
             (testdata->PORT == nat_session_ptr->PUB_PORT) ) 
         {
            testdata->NAT_SESSION_PTR = nat_session_ptr;
            break;
         } /* Endif */

         /* Go to next node */
         nat_session_ptr = nat_session_ptr->NEXT_IN;

      } else {                      
         /* From private network to public network */
         if ((testdata->SRC == nat_session_ptr->PRV_HST) &&
             (testdata->DEST == nat_session_ptr->PUB_HST) && 
             (testdata->PORT == nat_session_ptr->PUB_PORT)) 
         {
            testdata->NAT_SESSION_PTR = nat_session_ptr;
            break;
         } /* Endif */

         /* Go to next node */
         nat_session_ptr = nat_session_ptr->NEXT_OUT;

      } /* Endif */
   } /* Endwhile */
} /* Endbody */

static void NAT_lookup_func_lax
   (
      pointer _PTR_  node_ptr_ptr,
      pointer        data_ptr
   )
{ /* Body */
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = *node_ptr_ptr;
   struct nat_lookup _PTR_ testdata = data_ptr;

   /* Go through linked list of NAT_SESSION_STRUCTs */
   while (nat_session_ptr) {
      if (testdata->TO_PRV_NET) {   /* From public network to private network */
         if (testdata->SRC == nat_session_ptr->PUB_HST    &&
             testdata->DEST == nat_session_ptr->NAT_HST)
         {
            testdata->NAT_SESSION_PTR = nat_session_ptr;
            break;
         } /* Endif */

         /* Go to next node */
         nat_session_ptr = nat_session_ptr->NEXT_IN;
      } else {                      /* From private network to public network */
         if (testdata->SRC == nat_session_ptr->PRV_HST    &&
             testdata->DEST == nat_session_ptr->PUB_HST)
         {
            testdata->NAT_SESSION_PTR = nat_session_ptr;
            break;
         } /* Endif */

         /* Go to next node */
         nat_session_ptr = nat_session_ptr->NEXT_OUT;
      } /* Endif */
   } /* Endwhile */
} /* Endbody */


NAT_SESSION_STRUCT_PTR NAT_lookup
   (
      IP_HEADER_PTR ip_header_ptr,  /* [IN] Pointer to the IP datagram */
      boolean       icmp_err,       /* [IN] Is the lookup for an icmp error? */
      boolean       lax,            /* [IN] Non-strict lookup used by ALGs */
      uint_32_ptr   error_ptr
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR   nat_cfg_ptr = RTCS_getcfg(NAT);
   uint_32              key, ip_protocol = ntohc(ip_header_ptr->PROTOCOL);
   TRANSPORT_UNION     transport;
   struct nat_lookup    data;
   IPRADIX_NODE_PTR     root_ptr;
   boolean              pub_to_prv;
   DNAT_RULE_STRUCT_PTR dnat_rule_ptr;

   /* Get a pointer to the upper level protocol */
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);

   data.NAT_SESSION_PTR = NULL;
   data.PORT = 0;

   if (icmp_err) { /* Do a reverse lookup for ICMP errors */
      data.SRC = ntohl(ip_header_ptr->DEST);
      data.DEST = ntohl(ip_header_ptr->SOURCE);
   } else {
      data.SRC = ntohl(ip_header_ptr->SOURCE);
      data.DEST = ntohl(ip_header_ptr->DEST);
   } /* Endif */

   key = (ip_protocol & 0xFF) << 16;

   /* Find out if the packet is entering or exiting the private network */
   if (NAT_is_private_addr(&nat_cfg_ptr->PRIVATE_NETWORKS,data.SRC)) {
      data.TO_PRV_NET = FALSE;
      root_ptr = &nat_cfg_ptr->ROOT_OUT;
   } else {
      data.TO_PRV_NET = TRUE;
      root_ptr = &nat_cfg_ptr->ROOT_IN;
   } /* Endif */

   /*
   ** If we have received an ICMP error, we are search for the session
   ** corresponding to the embedded header, so we do a reverse lookup
   */
   if (icmp_err) {
      pub_to_prv = data.TO_PRV_NET?FALSE:TRUE;
   } else {
      pub_to_prv = data.TO_PRV_NET;
   } /* Endif */

   if (ip_protocol == IPPROTO_ICMP) {
      key = NAT_KEY(ip_protocol,ntohs(transport.ECHO_PTR->SEQ));
   } else {
      /* If the incoming packet is not an ICMP error, then the source is private */
      if (pub_to_prv) {
         /* It's TCP or UDP, and both have ports in same header position */
         key = NAT_KEY(ip_protocol,ntohs(transport.UDP_PTR->DEST_PORT));
         data.PORT = ntohs(transport.UDP_PTR->SRC_PORT);            
      } else {
         /* It's TCP or UDP, and both have ports in same header position */
         key = NAT_KEY(ip_protocol,ntohs(transport.UDP_PTR->SRC_PORT));
         data.PORT = ntohs(transport.UDP_PTR->DEST_PORT);           
      }/* Endif */
   } /* Endif */

   /* Search for an exact match on key */
   if (lax) {
      /* Non strict lookup used by ALGs */
      IPRADIX_insert(root_ptr, key, 0xFFFFFFFF, 0, NAT_lookup_func_lax, &data);
   } else {
      IPRADIX_insert(root_ptr, key, 0xFFFFFFFF, 0, NAT_lookup_func, &data);
   } /* Endif */


   if ((data.NAT_SESSION_PTR==NULL) && pub_to_prv) {
      // Maybe the header matches a DNAT rule
      dnat_rule_ptr = DNAT_lookup_rule(nat_cfg_ptr, ip_header_ptr, pub_to_prv);
      if (dnat_rule_ptr!=NULL) {
         // insert a new DNAT session here   
         data.NAT_SESSION_PTR = NAT_insert_dnat(ip_header_ptr, dnat_rule_ptr, error_ptr );
      }
   }

   return data.NAT_SESSION_PTR;

} /* Endbody */




/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_delete
* Returned Value  : void
* Comments        :
*        Deletes a NAT session.
*
*END*-----------------------------------------------------------------*/

struct nat_delete {
   _ip_address             SRC;
   _ip_address             DEST;
   boolean                 TO_PRV_NET;
   uint_16                 PORT;
} ;

static void NAT_delete_func
   (
      pointer _PTR_  node_ptr_ptr,
      pointer        data_ptr
   )
{ /* Body */
   NAT_SESSION_STRUCT_PTR _PTR_ search_ptr_ptr = (NAT_SESSION_STRUCT_PTR _PTR_)node_ptr_ptr;
   struct nat_delete _PTR_      testdata = data_ptr;

  
   /* Search through list */
   while (*search_ptr_ptr) {
      if (testdata->TO_PRV_NET) {   /* Searching the IN tree */
         if ((*search_ptr_ptr)->PUB_HST == testdata->SRC      &&
             (*search_ptr_ptr)->NAT_HST == testdata->DEST     &&
             (*search_ptr_ptr)->PUB_PORT == testdata->PORT)
         {
            *search_ptr_ptr = (*search_ptr_ptr)->NEXT_IN;
            break;
         } /* Endif */
         search_ptr_ptr = &(*search_ptr_ptr)->NEXT_IN;
      } else {
         if ((*search_ptr_ptr)->PRV_HST == testdata->SRC      &&
            (*search_ptr_ptr)->PUB_HST == testdata->DEST     &&
            (*search_ptr_ptr)->PUB_PORT == testdata->PORT)
         {
            *search_ptr_ptr = (*search_ptr_ptr)->NEXT_OUT;
            break;
         } /* Endif */
         search_ptr_ptr = &(*search_ptr_ptr)->NEXT_OUT;
      } /* Endif */
   } /* Endwhile */

} /* Endbody */

void NAT_delete
   (
      NAT_SESSION_STRUCT_PTR   nat_session_ptr       /* [IN] Session to delete */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR         nat_cfg_ptr = RTCS_getcfg(NAT);
   NAT_SESSION_EXT_STRUCT_PTR nat_session_ext_ptr, next_nat_session_ext_ptr;
   struct nat_delete          data;
   uint_32                    key;

   /* Remove session from IN tree */
   key = NAT_KEY(nat_session_ptr->IP_PROT,nat_session_ptr->NAT_PORT);
   
   data.SRC = nat_session_ptr->PUB_HST;
   data.DEST = nat_session_ptr->NAT_HST;
   data.PORT = nat_session_ptr->PUB_PORT;
   data.TO_PRV_NET = TRUE;

   IPRADIX_insert(&nat_cfg_ptr->ROOT_IN, key, 0xFFFFFFFF, 0, NAT_delete_func,
      &data);

   /* Remove session from OUT tree */
   key = NAT_KEY(nat_session_ptr->IP_PROT, nat_session_ptr->PRV_PORT);

   data.SRC = nat_session_ptr->PRV_HST;
   data.DEST = nat_session_ptr->PUB_HST;
   data.PORT = nat_session_ptr->PUB_PORT;
   data.TO_PRV_NET = FALSE;

   IPRADIX_insert(&nat_cfg_ptr->ROOT_OUT, key, 0xFFFFFFFF, 0, NAT_delete_func,
      &data);

   nat_session_ext_ptr = nat_session_ptr->ALG_INFO_PTR;
   while (nat_session_ext_ptr)  {
      next_nat_session_ext_ptr = nat_session_ext_ptr->NEXT;
      if (NAT_alg_free_func_table[nat_session_ext_ptr->ALG_TYPE])  {
         NAT_alg_free_func_table[nat_session_ext_ptr->ALG_TYPE](nat_session_ext_ptr);
      } /* Endif */
      nat_session_ext_ptr = next_nat_session_ext_ptr;
   } /* Endwhile */

   nat_cfg_ptr->STATS.ST_SESSIONS_OPEN--;
   RTCS_part_free(nat_session_ptr);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_find_next_session
* Returned Value  : void
* Comments        :
*        FInds the next NAT session.
*
*END*-----------------------------------------------------------------*/

struct nat_find_next_session {
   NAT_SESSION_STRUCT_PTR  LAST_SESSION_PTR;
   NAT_SESSION_STRUCT_PTR  NEXT_SESSION_PTR;
   boolean                 FOUND_LAST;
   boolean                 FOUND_NEXT;
   uint_32                 TREE;
};

static boolean NAT_find_next_session_test(  
   uint_32  key,
   uint_32  mask,
   pointer  node_pointer,
   pointer  data_pointer
)
{ 
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = node_pointer;
   struct nat_find_next_session _PTR_ data_ptr = data_pointer;

   if (node_pointer== NULL) {
      return FALSE;
   }

   while (nat_session_ptr != NULL) {
      if (data_ptr->FOUND_LAST || (data_ptr->LAST_SESSION_PTR ==NULL )) {
         data_ptr->NEXT_SESSION_PTR = nat_session_ptr;
         data_ptr->FOUND_NEXT = TRUE;
         return TRUE;
      } else {
         if (data_ptr->LAST_SESSION_PTR == nat_session_ptr) {
            data_ptr->FOUND_LAST = TRUE;
         }
      }
      if (data_ptr->TREE) {
         nat_session_ptr = nat_session_ptr->NEXT_IN;
      } else {
         nat_session_ptr = nat_session_ptr->NEXT_OUT;
      }
   }
   return FALSE;
}

void NAT_find_next_session_internal
   (
      NAT_PARM_PTR           parm_ptr 
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR   nat_cfg_ptr = RTCS_getcfg(NAT);
    struct nat_find_next_session    data;
   IPRADIX_NODE_PTR     root_ptr;
	

   if(nat_cfg_ptr == NULL)
   {
       parm_ptr->CONFIG = NULL;  
       RTCSCMD_complete(parm_ptr, (uint_32)RTCS_ERROR);
       return;
   }  
   
   
   
   data.LAST_SESSION_PTR = parm_ptr->CONFIG;
   data.NEXT_SESSION_PTR = NULL;
   data.FOUND_LAST = FALSE;
   data.FOUND_NEXT = FALSE;
   data.TREE = parm_ptr->OPTION;

   /* Find out if the packet is entering or exiting the private network */
   if (data.TREE) {
      root_ptr = &nat_cfg_ptr->ROOT_IN;
   } else {
      root_ptr = &nat_cfg_ptr->ROOT_OUT;
   } /* Endif */


   IPRADIX_walk(root_ptr, NAT_find_next_session_test, &data);
   if (data.FOUND_NEXT) {
      parm_ptr->CONFIG = data.NEXT_SESSION_PTR;
   } else {
      parm_ptr->CONFIG = NULL;
   }

   RTCSCMD_complete(parm_ptr, RTCS_OK);
} /* Endbody */

#endif
