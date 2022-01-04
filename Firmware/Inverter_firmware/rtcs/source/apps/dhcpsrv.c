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
* $FileName: dhcpsrv.c$
* $Version : 3.8.15.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the DHCP Server for RTCS.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

#include "dhcpsrv.h"

static DHCPSRV_STATE_STRUCT_PTR DHCPSRV_cfg = NULL;

void DHCPSRV_task (pointer, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_lease_start
* Returned Value  : void
* Comments        : This function starts a lease or offer.
*
* Notes           : This function must be called with the IPLIST
*                   semaphore locked.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_lease_start
   (
      DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrhead,
      DHCPSRV_ADDR_STRUCT_PTR       addr,
      uint_32                       time
   )
{ /* Body */

   while ((*addrhead) && (*addrhead)->EXPIRE < time) {
      if (time < DHCP_LEASE_INFINITE) {
         time -= (*addrhead)->EXPIRE;
      } /* Endif */
      addrhead = &(*addrhead)->NEXT;
   } /* Endwhile */

   if (*addrhead && (*addrhead)->EXPIRE < DHCP_LEASE_INFINITE) {
      (*addrhead)->EXPIRE -= time;
   } /* Endif */

   addr->EXPIRE = time;
   addr->NEXT = (*addrhead);
   *addrhead = addr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_lease_stop
* Returned Value  : dequeued IP address
* Comments        : This function terminates a lease or offer.
*
* Notes           : This function must be called with the IPLIST
*                   semaphore locked.
*
*END*-----------------------------------------------------------------*/

static DHCPSRV_ADDR_STRUCT_PTR DHCPSRV_lease_stop
   (
      DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrhead
   )
{ /* Body */
   DHCPSRV_ADDR_STRUCT_PTR addr = *addrhead;

   if (addr->NEXT && addr->NEXT->EXPIRE < DHCP_LEASE_INFINITE) {
      addr->NEXT->EXPIRE += addr->EXPIRE;
   } /* Endif */

   *addrhead = addr->NEXT;
   return addr;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_lease_extend
* Returned Value  : void
* Comments        : This function extends the EXPIRE time of an
*                   offer or a lease by delta.
*
* Notes           : This function must be called with the IPLIST
*                   semaphore locked.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_lease_extend
   (
      DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrhead,
      uint_32                       delta
   )
{ /* Body */
   DHCPSRV_ADDR_STRUCT_PTR addr = *addrhead;

   delta += addr->EXPIRE;
   addr = DHCPSRV_lease_stop(addrhead);
   DHCPSRV_lease_start(addrhead, addr, delta);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_ippool_add
* Returned Value  : error code
* Comments        : This function adds a block of IP addresses to
*                   the IP_AVAIL list.
*
*END*-----------------------------------------------------------------*/

uint_32 DHCPSRV_ippool_add
   (
      _ip_address             ipstart,
      uint_32                 ipnum,
      DHCPSRV_DATA_STRUCT_PTR params,
      uchar_ptr               optptr,
      uint_32                 optlen
   )
{ /* Body */
   DHCPSRV_OPTIONS_STRUCT_PTR    options;
   DHCPSRV_ADDR_STRUCT_PTR       addr;
   uchar_ptr                     moreoptions = NULL;

   options = RTCS_mem_alloc_system(sizeof(DHCPSRV_OPTIONS_STRUCT) + optlen);
   if (!options) {
      return RTCSERR_OUT_OF_MEMORY;
   } /* Endif */
   _mem_set_type(options, MEM_TYPE_DHCPSRV_OPTIONS_STRUCT);

   moreoptions = (uchar_ptr)(options+1);
   _mem_copy(optptr, moreoptions, optlen);

   options->COUNT = 0;
   options->SERVERID   = params->SERVERID;
   options->LEASE      = params->LEASE;
   options->MASK       = params->MASK;
   options->OPTION_PTR = moreoptions;
   options->OPTION_LEN = optlen;
   options->SADDR      = params->SADDR;
   _mem_copy(params->SNAME, options->SNAME, sizeof(options->SNAME) - 1);
   options->SNAME[sizeof(options->SNAME) - 1] = '\0';
   _mem_copy(params->FILE,  options->FILE,  sizeof(options->FILE)  - 1);
   options->FILE[sizeof(options->FILE) - 1] = '\0';

   RTCS_mutex_lock(&DHCPSRV_cfg->IPLIST_SEM);

   for (; ipnum; ipnum--, ipstart++) {
      addr = RTCS_mem_alloc_system(sizeof(DHCPSRV_ADDR_STRUCT));
      if (!addr) {
         if (options->COUNT == 0) {
            _mem_free(options);
         } /* Endif */
         RTCS_mutex_unlock(&DHCPSRV_cfg->IPLIST_SEM);
         return RTCSERR_OUT_OF_MEMORY;
      } /* Endif */
      _mem_set_type(options, MEM_TYPE_DHCPSRV_ADDR_STRUCT);

      addr->CLIENTID_LEN = 0;
      addr->CLIENTID_PTR = NULL;
      addr->IP_ADDR      = ipstart;
      addr->OPTIONS      = options;
      options->COUNT++;
      DHCPSRV_lease_start(&DHCPSRV_cfg->IP_AVAIL, addr, 0);
   } /* Endfor */

   RTCS_mutex_unlock(&DHCPSRV_cfg->IPLIST_SEM);
   return RTCS_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_expire
* Returned Value  : time to next expiry, 0 = forever.
* Comments        : This function expires offers and leases.
*
*END*-----------------------------------------------------------------*/

static uint_32 DHCPSRV_expire
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
   DHCPSRV_ADDR_STRUCT_PTR addr;
   TIME_STRUCT             time;
   uint_32                 elapsed;

   RTCS_mutex_lock(&state->IPLIST_SEM);
   _time_get(&time);

   /* Expire offers */
   elapsed = time.SECONDS - state->TIME;
   while (state->IP_OFFERED) {

      if (state->IP_OFFERED->EXPIRE > elapsed) {
         state->IP_OFFERED->EXPIRE -= elapsed;
         break;
      } /* Endif */

      addr = state->IP_OFFERED;
      state->IP_OFFERED = addr->NEXT;
      elapsed -= addr->EXPIRE;
      DHCPSRV_lease_start(&state->IP_AVAIL, addr, 0);

   } /* Endwhile */

   /* Expire leases */
   elapsed = time.SECONDS - state->TIME;
   while (state->IP_LEASED && state->IP_LEASED->EXPIRE < DHCP_LEASE_INFINITE) {

      if (state->IP_LEASED->EXPIRE > elapsed) {
         state->IP_LEASED->EXPIRE -= elapsed;
         break;
      } /* Endif */

      addr = state->IP_LEASED;
      state->IP_LEASED = addr->NEXT;
      elapsed -= addr->EXPIRE;
      DHCPSRV_lease_start(&state->IP_AVAIL, addr, 0);

   } /* Endwhile */

   /* Start CR 1547 */
   /* Expire takens */
   elapsed = time.SECONDS - state->TIME;
   while (state->IP_TAKEN && state->IP_TAKEN->EXPIRE < DHCP_LEASE_INFINITE) {

      if (state->IP_TAKEN->EXPIRE > elapsed) {
         state->IP_TAKEN->EXPIRE -= elapsed;
         break;
      } /* Endif */

      addr = state->IP_TAKEN;
      state->IP_TAKEN = addr->NEXT;
      elapsed -= addr->EXPIRE;
      DHCPSRV_lease_start(&state->IP_AVAIL, addr, 0);


   } /* Endwhile */
   /* End CR 1547 */

   /* Calculate time to next expiry */
   elapsed = DHCP_LEASE_INFINITE;
   if (state->IP_OFFERED && state->IP_OFFERED->EXPIRE < elapsed) {
      elapsed = state->IP_OFFERED->EXPIRE;
   } /* Endif */
   if (state->IP_LEASED  && state->IP_LEASED->EXPIRE  < elapsed) {
      elapsed = state->IP_LEASED->EXPIRE;
   } /* Endif */
   if (elapsed == DHCP_LEASE_INFINITE) {
      elapsed = 0;
   } /* Endif */

   state->TIME = time.SECONDS;
   RTCS_mutex_unlock(&state->IPLIST_SEM);

   return elapsed;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_find_option
* Returned Value  : uchar_ptr
* Comments        : This function searches through the OPTIONS section
*                   of a DHCP message to find a specific option type.
*                   If it is found, a pointer to the specific option
*                   field is returned, If no option of the specified
*                   type exists, NULL is returned.
*
*END*-----------------------------------------------------------------*/

static uchar_ptr DHCPSRV_find_option
   (
      uchar_ptr   msgptr,
      uint_32     msglen,
      uchar       option
   )
{ /* Body */
   uchar optype;
   uchar oplen;

   for (;;) {

      /* Get the next option code */
      if (msglen == 0) {
         return NULL;
      } /* Endif */
      optype = ntohc(msgptr);
      msgptr++;
      msglen--;

      if (optype == DHCPOPT_END) {
         return NULL;
      } /* Endif */
      if (optype == DHCPOPT_PAD) {
         continue;
      } /* Endif */

      /* Get the option length */
      if (msglen == 0) {
         return NULL;
      } /* Endif */
      oplen = ntohc(msgptr);
      msgptr++;
      msglen--;

      if (msglen < oplen) {
         return NULL;
      } /* Endif */

      if (optype == option) {
         return msgptr-2;
      } /* Endif */

      msgptr += oplen;
      msglen -= oplen;

   } /* Endfor */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_find_msgtype
* Returned Value  : value of the Message Type option
* Comments        : This function searches through the OPTIONS section
*                   of a DHCP message for the message type option.  If
*                   the message type option is is not present, 0 is
*                   returned.
*
*END*-----------------------------------------------------------------*/

static uchar DHCPSRV_find_msgtype
   (
      uchar_ptr   msgptr,
      uint_32     msglen
   )
{ /* Body */
   uchar msgtype = 0;

   msgptr = DHCPSRV_find_option(msgptr, msglen, DHCPOPT_MSGTYPE);
   if (msgptr != NULL) {
      msgptr++;
      if (ntohc(msgptr) == DHCPSIZE_MSGTYPE) {
         msgptr++;
         msgtype = ntohc(msgptr);
         if (!DHCPTYPE_ISVALID(msgtype)) {
            msgtype = 0;
         } /* Endif */
      } /* Endif */
   } /* Endif */

   return msgtype;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_find_clientid
* Returned Value  : IP address associated to client ID
* Comments        : This function searches a DHCPSRV_ADDR list for
*                   an IP address associated to a client.  If no
*                   address is found, NULL is returned.
*
* Notes           : This function must be called with the IPLIST
*                   semaphore locked.
*
*END*-----------------------------------------------------------------*/

static DHCPSRV_ADDR_STRUCT_PTR _PTR_ DHCPSRV_find_clientid
   (
      DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrhead,
      uchar_ptr                     cid_ptr,
      uint_32                       cid_len,
      uint_32_ptr                   expire_ptr
   )
{ /* Body */
   uint_32  i, expire;
   boolean  found;

   expire = 0;
   while (*addrhead) {
      expire += (*addrhead)->EXPIRE;
      if ((*addrhead)->CLIENTID_PTR && (*addrhead)->CLIENTID_LEN == cid_len) {

         found = TRUE;
         for (i=0; i<cid_len; i++) {
            if (cid_ptr[i] != (*addrhead)->CLIENTID_PTR[i]) {
               found = FALSE;
               break;
            } /* Endif */
         } /* Endfor */
         if (found) {
            if (expire_ptr) *expire_ptr = expire;
            return addrhead;
         } /* Endif */

      } /* Endif */
      addrhead = &(*addrhead)->NEXT;
   } /* Endwhile */

   return NULL;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_find_addr
* Returned Value  : IP address associated to client ID
* Comments        : This function searches a DHCPSRV_ADDR list for
*                   a specified IP address.  If the address is not
*                   found, NULL is returned.
*
* Notes           : This function must be called with the IPLIST
*                   semaphore locked.
*
*END*-----------------------------------------------------------------*/

static DHCPSRV_ADDR_STRUCT_PTR _PTR_ DHCPSRV_find_addr
   (
      DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrhead,
      _ip_address                   addr
   )
{ /* Body */

   while (*addrhead) {

      if ((*addrhead)->IP_ADDR == addr) {
         return addrhead;
      } /* Endif */

      addrhead = &(*addrhead)->NEXT;
   } /* Endwhile */

   return NULL;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_update_clientid
* Returned Value  : its first parameter if successful, NULL otherwise
* Comments        : This function updates a DHCPSRV_ADDR's client
*                   ID field with a new one.
*
* Notes           : This function must be called with the IPLIST
*                   semaphore locked.
*
*END*-----------------------------------------------------------------*/

static DHCPSRV_ADDR_STRUCT_PTR _PTR_ DHCPSRV_update_clientid
   (
      DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrhead,
      uchar_ptr                     cid_ptr,
      uint_32                       cid_len
   )
{ /* Body */
   DHCPSRV_ADDR_STRUCT_PTR addr = *addrhead;
   uchar_ptr               cid;

   cid = RTCS_mem_alloc(cid_len);
   if (!cid) return NULL;
   _mem_set_type(cid, MEM_TYPE_DHCPSRV_CID);

   if (addr->CLIENTID_PTR) _mem_free(addr->CLIENTID_PTR);

   _mem_copy(cid_ptr, cid, cid_len);
   addr->CLIENTID_PTR = cid;
   addr->CLIENTID_LEN = cid_len;

   return addrhead;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_write_header
* Returned Value  : void
* Comments        : This function fills the send buffer with the
*                   initial values needed in a DHCP response.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_write_header
   (
      DHCPSRV_STATE_STRUCT_PTR   state,
      DHCPSRV_ADDR_STRUCT_PTR    lease_ptr,
      uint_32                    lease_time,
      uchar                      msgtype
   )
{ /* Body */
   DHCP_HEADER_PTR   outp;
   DHCP_HEADER_PTR   inp;
   uchar_ptr         optptr;
   volatile uint_32           optlen;
   uint_32           temp_long;
   uint_16           temp_short;
   uchar             temp_char;

   outp = (DHCP_HEADER_PTR)state->SND_BUFFER;
   inp  = (DHCP_HEADER_PTR)state->RCV_BUFFER;

   /* Build a DHCPOFFER, DHCPACK, or DHCPNAK packet */
   htonc(outp->OP, DHCPOP_BOOTREPLY);
   htonc(outp->HTYPE, ARPLINK_ETHERNET);
   temp_char = ntohc(inp->HLEN);
   htonc(outp->HLEN, temp_char);
   htonc(outp->HOPS, 0);

   /* Use client's existing transaction ID */
   temp_long = ntohl(inp->XID);
   htonl(outp->XID, temp_long);

   htons(outp->SECS, 0);

   if (ntohl(inp->GIADDR) == 0) {
      htons(outp->FLAGS, 0);
   } else {
      temp_short = ntohs(inp->FLAGS);
      htons(outp->FLAGS, temp_short);
   } /* Endif */

   if (msgtype == DHCPTYPE_DHCPOFFER) {
      htonl(outp->CIADDR, 0);
   } else {
      temp_long = ntohl(inp->CIADDR);
      htonl(outp->CIADDR, temp_long);
   } /* Endif */

   if (msgtype == DHCPTYPE_DHCPNAK) {
      htonl(outp->YIADDR, 0);
      htonl(outp->SIADDR, 0);
   } else {
      htonl(outp->YIADDR, lease_ptr->IP_ADDR);
      htonl(outp->SIADDR, lease_ptr->OPTIONS->SADDR);
   } /* Endif */

   htonl(outp->GIADDR, 0);

   _mem_copy(inp->CHADDR, outp->CHADDR, sizeof(outp->CHADDR));

   if (msgtype == DHCPTYPE_DHCPNAK) {
      _mem_zero(outp->SNAME, sizeof(outp->SNAME));
      _mem_zero(outp->FILE,  sizeof(outp->FILE));
   } else {
      _mem_copy(lease_ptr->OPTIONS->SNAME, outp->SNAME, sizeof(outp->SNAME));
      _mem_copy(lease_ptr->OPTIONS->FILE,  outp->FILE,  sizeof(outp->FILE));
   } /* Endif */

   /*
   ** Fill in the required response options. These are message type,
   ** subnet mask, and server id.
   */

   optlen = sizeof(DHCP_HEADER);
   optptr = state->SND_BUFFER + optlen;

   /* The Magic Cookie must always be the first thing in the OPTIONS */
   htonl(optptr, DHCP_MAGIC);
   optptr += DHCPSIZE_MAGIC;
   optlen += DHCPSIZE_MAGIC;

#define DHCP_OPTION(type,len,val) \
            htonc(optptr, DHCPOPT_  ## type); optptr++; optlen++; \
            htonc(optptr, DHCPSIZE_ ## type); optptr++; optlen++; \
            hton ## len(optptr, val);                             \
            optptr += DHCPSIZE_ ## type;                          \
            optlen += DHCPSIZE_ ## type

   DHCP_OPTION(MSGTYPE,  c, msgtype);
   if (msgtype != DHCPTYPE_DHCPNAK) {
      DHCP_OPTION(SERVERID, l, lease_ptr->OPTIONS->SERVERID);
      DHCP_OPTION(MASK,     l, lease_ptr->OPTIONS->MASK);
      DHCP_OPTION(LEASE,    l, lease_time);
   } /* Endif */

   state->SND_BUFFER_LEN = optlen;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_write_options
* Returned Value  : void
* Comments        : This function fills the send buffer with the
*                   options specified by the application.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_write_options
   (
      DHCPSRV_STATE_STRUCT_PTR   state,
      DHCPSRV_ADDR_STRUCT_PTR    lease_ptr
   )
{ /* Body */
   uchar_ptr   inp, outp, scanptr;
   int_32      scanlen;
   uint_32     copylen;
   volatile uchar       optval, optlen;

   outp = state->SND_BUFFER + state->SND_BUFFER_LEN;
   inp  = lease_ptr->OPTIONS->OPTION_PTR;

   scanptr = inp;
   scanlen = sizeof(state->SND_BUFFER) - state->SND_BUFFER_LEN - 1;
   if (scanlen > lease_ptr->OPTIONS->OPTION_LEN) {
      scanlen = lease_ptr->OPTIONS->OPTION_LEN;
   } /* Endif */
   copylen = 0;

   for (;;) {

      if (scanlen < 1) break;
      optval = ntohc(scanptr);
      scanptr++;
      scanlen--;

      if (optval == DHCPOPT_END) {
         break;
      } /* Endif */
      if (optval == DHCPOPT_PAD) {
         copylen++;
         continue;
      } /* Endif */

      if (scanlen < 1) break;
      optlen = ntohc(scanptr);
      scanptr++;
      scanlen--;

      if (scanlen < optlen) break;

      scanptr += optlen;
      scanlen -= optlen;
      copylen += optlen + 2;

   } /* Endfor */

   _mem_copy(inp, outp, copylen);
   state->SND_BUFFER_LEN += copylen;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_write_end
* Returned Value  : void
* Comments        : This function writes an END option to the
*                   OPTIONS field.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_write_end
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
   uchar_ptr   optptr;
   volatile uint_32     optlen;

   optlen = state->SND_BUFFER_LEN;
   optptr = state->SND_BUFFER + optlen;

   htonc(optptr, DHCPOPT_END);
   optlen++;

   state->SND_BUFFER_LEN = optlen;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_send
* Returned Value  : void
* Comments        : This function sends a prebuilt DHCP reply.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_send
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
   _ip_address    giaddr;
   sockaddr_in    raddr;

   giaddr = ntohl(((DHCP_HEADER_PTR)state->RCV_BUFFER)->GIADDR);
   if (giaddr) {
      raddr.sin_family      = AF_INET;
      raddr.sin_port        = IPPORT_BOOTPS;
      raddr.sin_addr.s_addr = giaddr;
   } else {
      raddr.sin_family      = AF_INET;
      raddr.sin_port        = IPPORT_BOOTPC;
      raddr.sin_addr.s_addr = INADDR_BROADCAST;
   } /* Endif */

   /* Start CR 1131 */
   sendto(state->SOCKET, state->SND_BUFFER, state->SND_BUFFER_LEN,
      RTCS_MSG_NOLOOP, (sockaddr *)&raddr, sizeof(raddr));
   /* End CR 1131 */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_service_discover
* Returned Value  : void
* Comments        : This function replies to a received DHCPDISCOVER
*                   message.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_service_discover
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
   uchar_ptr                     inp,   cid_ptr, opt_ptr;
   uint_32                       inlen, cid_len;
   DHCPSRV_ADDR_STRUCT_PTR       addr;
   DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrp;
   uint_32                       time, lease;
   boolean                       leased, avail, newcid;

   inp   = state->RCV_BUFFER     + sizeof(DHCP_HEADER) + 4;
   inlen = state->RCV_BUFFER_LEN - sizeof(DHCP_HEADER) - 4;

   /* Get the client ID; use CHADDR if none */
   cid_ptr = DHCPSRV_find_option(inp, inlen, DHCPOPT_CLIENTID);
   if (cid_ptr) {
      cid_ptr++;
      cid_len = ntohc(cid_ptr);
      cid_ptr++;
   } else {
      DHCP_HEADER_PTR pkt_ptr = (DHCP_HEADER_PTR)state->RCV_BUFFER;
      cid_ptr = pkt_ptr->CHADDR;
      cid_len = ntohc(pkt_ptr->HLEN);
      if (cid_len > sizeof(pkt_ptr->CHADDR)) {
         cid_len = sizeof(pkt_ptr->CHADDR);
      } /* Endif */
   } /* Endif */

   addrp = NULL;
   RTCS_mutex_lock(&state->IPLIST_SEM);

   /* First check whether the client already has a lease */
   if (!addrp) {
      addrp = DHCPSRV_find_clientid(&state->IP_LEASED,  cid_ptr, cid_len, &time);
      leased = TRUE;
      avail  = FALSE;
      newcid = FALSE;
   } /* Endif */

   /* Next check whether the client has been offered a lease */
   if (!addrp) {
      addrp = DHCPSRV_find_clientid(&state->IP_OFFERED, cid_ptr, cid_len, &time);
      leased = FALSE;
      avail  = FALSE;
      newcid = FALSE;
   } /* Endif */

   /* Next check whether any available address was previously offered/leased to the client */
   if (!addrp) {
      addrp = DHCPSRV_find_clientid(&state->IP_AVAIL,   cid_ptr, cid_len, NULL);
      leased = FALSE;
      avail  = TRUE;
      newcid = FALSE;
   } /* Endif */

   /* Next check whether the client's requested address is available */
   if (!addrp) {
      _ip_address reqaddr;

      opt_ptr = DHCPSRV_find_option(inp, inlen, DHCPOPT_ADDRESS);
      if (opt_ptr && ntohc(opt_ptr+1) == DHCPSIZE_ADDRESS) {
         reqaddr = ntohl(opt_ptr+2);
      } else {
         DHCP_HEADER_PTR pkt_ptr = (DHCP_HEADER_PTR)state->RCV_BUFFER;
         reqaddr = ntohl(pkt_ptr->CIADDR);
      } /* Endif */

      addrp = DHCPSRV_find_addr(&state->IP_AVAIL, reqaddr);
      leased = FALSE;
      avail  = TRUE;
      newcid = TRUE;
   } /* Endif */

   /* Finally, offer the first available address */
   if (!addrp) {
      if (state->IP_AVAIL) {
         addrp = &state->IP_AVAIL;
      } /* Endif */
      leased = FALSE;
      avail  = TRUE;
      newcid = TRUE;
   } /* Endif */

   if (newcid && addrp) {
      addrp = DHCPSRV_update_clientid(addrp, cid_ptr, cid_len);
   } /* Endif */

   if (addrp) {

      /* Determine length of offer */
      opt_ptr = DHCPSRV_find_option(inp, inlen, DHCPOPT_LEASE);
      if (opt_ptr && ntohc(opt_ptr+1) == DHCPSIZE_LEASE) {
         lease = ntohl(opt_ptr+2);
         if (lease > (*addrp)->OPTIONS->LEASE) {
            lease = (*addrp)->OPTIONS->LEASE;
         } /* Endif */
      } else if (leased) {
         lease = time;
      } else {
         lease = (*addrp)->OPTIONS->LEASE;
      } /* Endif */
      (*addrp)->OFFER = lease;

      DHCPSRV_write_header (state, *addrp, lease, DHCPTYPE_DHCPOFFER);
      DHCPSRV_write_options(state, *addrp);
      DHCPSRV_write_end    (state);

      /* Start CR 1547 */
      /* probe the address to see if it is already taken (if so configured) */
  
  
      if (addrp && DHCPSRV_cfg->FLAGS & DHCPSVR_FLAG_DO_PROBE) {
          uint_32   timeout = 50; /* miliseconds */
          uint_32   error;
          
          /* note: this will hang the dhserver application while we await the probe result */
          error = RTCS_ping((*addrp)->IP_ADDR, &timeout, 0xFACE, 0, 0, 0);
          
          if (error == RTCS_OK) {
             /* bad news: someone answered this ping, we can't use this address */
             addr = DHCPSRV_lease_stop(addrp);
             DHCPSRV_lease_start(&state->IP_TAKEN, addr, DHCPTIME_OFFER);
             avail = FALSE;
             
             /* nak this discover, client will send us another */
             DHCPSRV_write_header(state, NULL, 0, DHCPTYPE_DHCPNAK);
             DHCPSRV_write_end(state);
             
          }
      }
     /* End CR 1547 */ 
      
      /* Reserve the offer for at least DHCPTIME_OFFER */
      if (avail) {
         addr = DHCPSRV_lease_stop(addrp);
         DHCPSRV_lease_start(&state->IP_OFFERED, addr, DHCPTIME_OFFER);
      } else if (time < DHCPTIME_OFFER) {
         DHCPSRV_lease_extend(addrp, DHCPTIME_OFFER - time);
      } /* Endif */

   } else {
      DHCPSRV_write_header(state, NULL, 0, DHCPTYPE_DHCPNAK);
      DHCPSRV_write_end(state);
   } /* Endif */

   RTCS_mutex_unlock(&state->IPLIST_SEM);

   DHCPSRV_send(state);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_service_request
* Returned Value  : void
* Comments        : This function replies to a received DHCPREQUEST
*                   message.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_service_request
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
   uchar_ptr                     inp,   cid_ptr, sid_ptr, lease_ptr;
   uint_32                       inlen, cid_len;
   _ip_address                   serverid = 0;
   uint_32                       lease = 0;
   DHCPSRV_ADDR_STRUCT_PTR       addr;
   DHCPSRV_ADDR_STRUCT_PTR _PTR_ addrp;
   /* Start CR 1599 */
   boolean                       reply=FALSE;
   /* End CR 1599 */

   inp   = state->RCV_BUFFER     + sizeof(DHCP_HEADER) + 4;
   inlen = state->RCV_BUFFER_LEN - sizeof(DHCP_HEADER) - 4;

   /* Get the client ID; use CHADDR if none */
   cid_ptr = DHCPSRV_find_option(inp, inlen, DHCPOPT_CLIENTID);
   if (cid_ptr) {
      cid_ptr++;
      cid_len = ntohc(cid_ptr);
      cid_ptr++;
   } else {
      DHCP_HEADER_PTR pkt_ptr = (DHCP_HEADER_PTR)state->RCV_BUFFER;
      cid_ptr = pkt_ptr->CHADDR;
      cid_len = ntohc(pkt_ptr->HLEN);
      if (cid_len > sizeof(pkt_ptr->CHADDR)) {
         cid_len = sizeof(pkt_ptr->CHADDR);
      } /* Endif */
   } /* Endif */

   /* Get the server ID */
   sid_ptr = DHCPSRV_find_option(inp, inlen, DHCPOPT_SERVERID);
   if (sid_ptr && ntohc(sid_ptr+1) == DHCPSIZE_SERVERID) {
      sid_ptr += 2;
      serverid = ntohl(sid_ptr);
   } else {
      sid_ptr = NULL;
   } /* Endif */

   /* Get the requested lease */
   lease_ptr = DHCPSRV_find_option(inp, inlen, DHCPOPT_LEASE);
   if (lease_ptr && ntohc(lease_ptr+1) == DHCPSIZE_LEASE) {
      lease_ptr += 2;
      lease = ntohl(lease_ptr);
   } else {
      lease_ptr = NULL;
   } /* Endif */

   addrp = NULL;
   RTCS_mutex_lock(&state->IPLIST_SEM);

   if (!addrp) {
      addrp = DHCPSRV_find_clientid(&state->IP_LEASED,  cid_ptr, cid_len, NULL);
   } /* Endif */

   if (!addrp && sid_ptr) {
      addrp = DHCPSRV_find_clientid(&state->IP_OFFERED, cid_ptr, cid_len, NULL);
   } /* Endif */

   /* Start CR 2089 */
   /* check whether any available address was previously offered/leased to the client */   
   if (!addrp) {
      addrp = DHCPSRV_find_clientid(&state->IP_AVAIL, cid_ptr, cid_len, NULL);
   } /* Endif */    
   /* End CR 2089 */
      
   if (addrp) {

      if (sid_ptr && serverid != (*addrp)->OPTIONS->SERVERID) {

         reply = FALSE;

         addr = DHCPSRV_lease_stop(addrp);
         DHCPSRV_lease_start(&state->IP_AVAIL, addr, 0);

      } else if (lease_ptr && lease > (*addrp)->OPTIONS->LEASE) {

         DHCPSRV_write_header(state, NULL, 0, DHCPTYPE_DHCPNAK);
         DHCPSRV_write_end(state);
         reply = TRUE;

      } else {

         if (!lease_ptr) {
            lease = (*addrp)->OFFER;
         } /* Endif */

         DHCPSRV_write_header (state, *addrp, lease, DHCPTYPE_DHCPACK);
         DHCPSRV_write_options(state, *addrp);
         DHCPSRV_write_end    (state);
         reply = TRUE;

         addr = DHCPSRV_lease_stop(addrp);
         DHCPSRV_lease_start(&state->IP_LEASED, addr, lease);

      } /* Endif */
   } /* Endif */

   RTCS_mutex_unlock(&state->IPLIST_SEM);

   if (reply) {
      DHCPSRV_send(state);
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_service_release
* Returned Value  : void
* Comments        : This function services a received DHCPRELEASE
*                   message.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_service_release
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_service_decline
* Returned Value  : void
* Comments        : This function services a received DHCPDECLINE
*                   message.
*
*END*-----------------------------------------------------------------*/

static void DHCPSRV_service_decline
   (
      DHCPSRV_STATE_STRUCT_PTR   state
   )
{ /* Body */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPSRV_init
* Returned Value  : error code
* Comments        : Start the DHCP server.
*
*END*-----------------------------------------------------------------*/

uint_32 DHCPSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, DHCPSRV_task, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name   : DHCPSRV_task
* Returned Value  :
* Comments        : The DHCP server.
*
*END*-----------------------------------------------------------------*/

void DHCPSRV_task
   (
      pointer dummy,
      pointer creator
   )

{ /* Body */
   DHCPSRV_STATE_STRUCT    state;
   sockaddr_in             addr;
   uint_32                 timeout;
   uchar_ptr               msgptr;
   uint_32                 msglen;
   uchar                   msgtype;
   int_32                  error;

   state.IP_AVAIL   = NULL;
   state.IP_OFFERED = NULL;
   state.IP_LEASED  = NULL;

   /* Start CR 1547 */
   state.IP_TAKEN   = NULL;
   state.FLAGS      = DHCPSVR_FLAG_DO_PROBE;
   /* End CR 1547 */

   RTCS_mutex_init(&state.IPLIST_SEM);

   state.SOCKET = socket(PF_INET, SOCK_DGRAM, 0);
   if (state.SOCKET == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */

   addr.sin_family      = AF_INET;
   addr.sin_port        = IPPORT_BOOTPS;
   addr.sin_addr.s_addr = INADDR_ANY;

   error = bind(state.SOCKET,(sockaddr *)(&addr), sizeof(sockaddr_in));
  
   if (error != RTCS_OK) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   DHCPSRV_cfg = &state;

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      /* Wait for a message */
      timeout = DHCPSRV_expire(&state) * 1000;
      error = RTCS_selectset(&state.SOCKET, 1, timeout);
      if ((error == 0) || (error == RTCS_ERROR)) continue;

      /* Received a message, respond accordingly */
      error = recvfrom(state.SOCKET, state.RCV_BUFFER, sizeof(state.RCV_BUFFER), 0, NULL, NULL);
      if (error == RTCS_ERROR) continue;

      /* The datagram must contain at least a header and a magic cookie */
      if (error < sizeof(DHCP_HEADER) + DHCPSIZE_MAGIC) continue;
      msgptr = state.RCV_BUFFER + sizeof(DHCP_HEADER);
      if (ntohl(msgptr) != DHCP_MAGIC) continue;
      msgptr += DHCPSIZE_MAGIC;
      msglen = error - sizeof(DHCP_HEADER) - DHCPSIZE_MAGIC;
      state.RCV_BUFFER_LEN = error;

      /* Update the time before starting any new events */
      DHCPSRV_expire(&state);

      /* Service the packet */
      msgtype = DHCPSRV_find_msgtype(msgptr, msglen);
      switch (msgtype) {

      case DHCPTYPE_DHCPDISCOVER:
         DHCPSRV_service_discover(&state);
         break;

      case DHCPTYPE_DHCPREQUEST:
         DHCPSRV_service_request(&state);
         break;

      case DHCPTYPE_DHCPRELEASE:
         DHCPSRV_service_release(&state);
         break;

      case DHCPTYPE_DHCPDECLINE:
         DHCPSRV_service_decline(&state);
         break;

      } /* Endswitch */

   } /* Endwhile */
} /* Endbody */

/* Start CR 1547 */
/*FUNCION*-----------------------------------------------------------------
*
* Function Name   : DHCPSRV_set_config_flag_on
* Returned Value  : RTCS_OK if all went well
* Comment         : turns on the flags passed in
*                 : May be called anytime after DHCPSRV_init()
*                 : Flags are defined in dhcpsrv.h and look like :
*                 : DHCPSVR_FLAG_*
*
*END*-----------------------------------------------------------------*/

static uint_32 DHCPSRV_set_config_flag_on
   (
      uint_32   flags /* [IN]   flags to govern behaviour of dhcp server */
   )

{ /* Body */

   RTCS_mutex_lock(&DHCPSRV_cfg->IPLIST_SEM);

   DHCPSRV_cfg->FLAGS |= flags;
   
   RTCS_mutex_unlock(&DHCPSRV_cfg->IPLIST_SEM);
   
   return RTCS_OK;
} /* Endbody */

 
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : DHCPSRV_set_config_flag_off
* Returned Value  : RTCS_OK if all went well
* Comment         : Turns off the flags passed in
*                 : May be called anytime after DHCPSRV_init()
*                 : Flags are defined in dhcpsrv.h and look like :
*                 : DHCPSVR_FLAG_*
*
*END*-----------------------------------------------------------------*/

static uint_32 DHCPSRV_set_config_flag_off
   (
      uint_32   flags /* [IN]   flags to govern behaviour of dhcp server */
   )

{ /* Body */

   RTCS_mutex_lock(&DHCPSRV_cfg->IPLIST_SEM);
   
   DHCPSRV_cfg->FLAGS &= !flags;
   
   RTCS_mutex_unlock(&DHCPSRV_cfg->IPLIST_SEM);
   
   return RTCS_OK;
} /* Endbody */

#endif

/* End CR 1547 */
/* EOF */
