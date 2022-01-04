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
* $FileName: snmp.c$
* $Version : 3.8.16.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains an implementation of an
*   SNMP{v1,v2c} agent.
*
*END************************************************************************/

#include <rtcs.h>
#include <rtcs_prv.h>

#if RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_UDP 
#if RTCSCFG_ENABLE_IP4

#include <string.h>
#include "snmpcfg.h"
#include "asn1.h"
#include "snmp.h"

typedef struct snmp_context  {
   uint_32     UDPSOCK;
} SNMP_CONTEXT, _PTR_ SNMP_CONTEXT_PTR;

_rtcs_taskid SNMP_task_id = 0;

#ifdef __MQX__ 
void SNMP_Exit_handler(void);
#endif


static uchar inbuf[SNMPCFG_BUFFER_SIZE];
static uchar outbuf[SNMPCFG_BUFFER_SIZE];

void SNMP_task (pointer, pointer);

/*
** This agent generates GetResponse PDUs to GetRequests,
** GetNextRequests and GetBulkRequests.  All GetResponses
** have the same format, specifically:
**
**    30 <len>
**       02 01 <00-01>
**       04 <len> <community>
**       A2 <len>
**          02 <len> <id>
**          02 01 00
**          02 01 00
**          30 <len>
**
**             30 <len>
**                06 <len> <object-id>
**                <tag> <len> <value>
**             30 <len>
**                06 <len> <object-id>
**                <tag> <len> <value>
**             30 <len>
**                06 <len> <object-id>
**                <tag> <len> <value>
**             ...
**
** Building this packet is a bit problematic.
**
** If we start from the top, we can't fill in the <len>
** fields, because
**    1) we don't yet know the length of the remainder
**       of the packet, and
**    2) since we don't know the length, we don't know
**       how many bytes the length field will occupy
**       (each <len> is 1 byte for lengths 00-7F,
**       2 bytes for lengths 80-FF, or 3 bytes for
**       lengths 0100-FFFF).
**
** We'd have to assume the worst case for each <len>
** and reserve 3 bytes for each, and make a second
** pass to fill them.
**
** If we start from the bottom, then building responses
** to GetBulkRequests becomes hard.
**
** So what do we do?  We start from the middle. :)
**
** We reserve space for the maximum header (the first eight
** lines above).  To do this, we make an assumption:  that
** no <len> ever gets a value larger than FFFF.  This is
** pretty safe, since SNMP packets have to fit inside IP
** datagrams, and IP datagrams only have 16-bit length
** fields.
**
** Then we make repeated MIB calls to fill in the
** VarBindList.
**
** Once the VarBindList is written, we write the header
** backwards.
**
*/


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_request
* Returned Value   :
* Comments  :  Parse a VarBindList in a PDU.
*
*END*-----------------------------------------------------------------*/

static boolean SNMP_request
   (
      SNMP_PARSE_PTR snmp,
      uchar_ptr      invarp,
      uint_32        invarlen,
      uchar_ptr      outvarp,
      uint_32        outvarlen,
      uint_32 _PTR_  writelen
   )
{ /* Body */

   IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_VAR_GETS++);
   snmp->errstat = RTCSMIB_request(snmp->pdutype, invarp, invarlen, outvarp, outvarlen, writelen);

   if (snmp->errstat == SNMP_ERROR_noError) {
      return TRUE;
   } /* Endif */

   if (snmp->errstat == SNMP_ERROR_PARSE) {
      snmp->errstat = SNMP_ERROR_noError;
      return FALSE;
   } /* Endif */

   if (snmp->version == 0) {
      switch (snmp->errstat) {
      case SNMP_ERROR_noSuchObject:
      case SNMP_ERROR_noSuchInstance:
      case SNMP_ERROR_endOfMibView:
      case SNMP_ERROR_notWritable:
         snmp->errstat = SNMP_ERROR_noSuchName;
         break;
      case SNMP_ERROR_noAccess:
      case SNMP_ERROR_wrongType:
      case SNMP_ERROR_wrongLength:
      case SNMP_ERROR_wrongEncoding:
      case SNMP_ERROR_wrongValue:
      case SNMP_ERROR_noCreation:
      case SNMP_ERROR_inconsistentValue:
      case SNMP_ERROR_resourceUnavailable:
         snmp->errstat = SNMP_ERROR_badValue;
         break;
      } /* Endswitch */
      return FALSE;
   } /* Endif */

   if ((snmp->version == 1) && (snmp->errstat >= 0x80)) {
      uint_32 oidlen, varlen, totlen;

      invarp++;
      if (*invarp < 0x80) {
         invarp++;
      } else {
         invarp += (*invarp & 0x7F) + 1;
      } /* Endif */

      invarp++;
      if (*invarp < 0x80) {
         oidlen = *invarp++;
      } else {
         uint_32 lenlen = *invarp++ & 0x7F;
         oidlen = 0;
         
         while (lenlen--) {
            oidlen <<= 8;
            oidlen += *invarp++;
         } /* Endwhile */
      } /* Endif */

      varlen = oidlen;
      if (varlen < 0x80) {
         varlen += 2;
      } else if (varlen < 0x100) {
         varlen += 3;
      } else {
         varlen += 4;
      } /* Endif */
      varlen += 2;

      totlen = varlen;
      if (totlen < 0x80) {
         totlen += 2;
      } else if (totlen < 0x100) {
         totlen += 3;
      } else {
         totlen += 4;
      } /* Endif */

      if (totlen > outvarlen) {
         snmp->errstat = SNMP_ERROR_tooBig;
         return FALSE;
      } /* Endif */

      *outvarp++ = ASN1_TYPE_SEQUENCE;
      if (varlen < 0x80) {
         *outvarp++ = varlen;
      } else if (varlen < 0x100) {
         *outvarp++ = 0x81;
         *outvarp++ = varlen;
      } else {
         *outvarp++ = 0x82;
         htons(outvarp, varlen);
         outvarp += 2;
      } /* Endif */

      *outvarp++ = ASN1_TYPE_OBJECT;
      if (oidlen < 0x80) {
         *outvarp++ = oidlen;
      } else if (oidlen < 0x100) {
         *outvarp++ = 0x81;
         *outvarp++ = oidlen;
      } else {
         *outvarp++ = 0x82;
         htons(outvarp, oidlen);
         outvarp += 2;
      } /* Endif */
      _mem_copy(invarp, outvarp, oidlen);
      outvarp += oidlen;

      *outvarp++ = snmp->errstat;
      *outvarp++ = 0;

      *writelen = totlen;
      return TRUE;
   } /* Endif */

   return FALSE;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse_varbindlist
* Returned Value   :
* Comments  :  Parse a VarBindList in a PDU.
*
*END*-----------------------------------------------------------------*/

static boolean SNMP_parse_varbindlist
   (
      SNMP_PARSE_PTR snmp,
      uint_32_ptr    writelen
   )
{ /* Body */
   uchar_ptr   outp, varp;
   uint_32     outlen, varlen, varcount, index;

   outp = snmp->outbuf;
   outlen = snmp->outlen;
   index = 0;
   *writelen = 0;

   /* 
   ** Set the value of non repeaters variable to zero 
   ** if it is less than zero
   */
   #if 0 // nonrep is unsigned  
   if(snmp->nonrep < 0) {      
      snmp->nonrep = 0;
   }
   #endif
   
   while (snmp->nonrep && snmp->inlen) {
      snmp->nonrep--;
      varp = snmp->inbuf;
      varlen = snmp->inlen;
      index++;
      if (!SNMP_request(snmp, varp, varlen, outp, outlen, &varlen)) {
         if (SNMP_ERROR_USEINDEX(snmp->errstat)) {
            snmp->errindex = index;
         } /* Endif */
         return FALSE;
      } /* Endif */
      outp += varlen;
      outlen -= varlen;
      *writelen += varlen;
      ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, varlen);
      ASN1_READ_IGNORE(snmp, varlen);
   } /* Endwhile */

   while (snmp->inlen) {
      varp = snmp->inbuf;
      varlen = snmp->inlen;
      index++;
      for (varcount = 0; varcount < snmp->reps; varcount++) {
         if (!SNMP_request(snmp, varp, varlen, outp, outlen, &varlen)) {
            if (SNMP_ERROR_USEINDEX(snmp->errstat)) {
               snmp->errindex = index;
            } /* Endif */
            return FALSE;
         } /* Endif */
         varp = outp;
         outp += varlen;
         outlen -= varlen;
         *writelen += varlen;
      } /* Endfor */
      ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, varlen);
      ASN1_READ_IGNORE(snmp, varlen);
   } /* Endwhile */

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse_pduv1
* Returned Value   :
* Comments  :  Parse a {Get,GetNext,Set}Request PDU.
*
*END*-----------------------------------------------------------------*/

static boolean SNMP_parse_pduv1
   (
      SNMP_PARSE_PTR snmp
   )
{ /* Body */
   uint_32     typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errstatp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_IGNORE(snmp, typelen);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errindexp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_IGNORE(snmp, typelen);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, typelen);
   snmp->inlen = typelen;

   /*
   ** We want nonrep to be (at least) the number of VarBinds in
   ** the VarBindList.  We pick typelen, because each VarBind is
   ** guaranteed to be at least one byte (in fact, they're guaranteed
   ** to be at least five bytes), so typelen is a guaranteed upper
   ** bound.  We pick reps = 1, because if ever nonrep weren't big
   ** enough (which will never happen), the remaining VarBinds will
   ** still be treated as non-repeaters.
   */
   snmp->nonrep = typelen;
   snmp->reps = 1;

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse_awesome_pdubulk
* Returned Value   :
* Comments  :  Parse a GetBulkRequest PDU.
*
*END*-----------------------------------------------------------------*/

static boolean SNMP_parse_awesome_pdubulk
   (
      SNMP_PARSE_PTR snmp
   )
{ /* Body */
   uint_32     typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errstatp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_INT(snmp, typelen, snmp->nonrep);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   if (typelen == 0) return FALSE;
   snmp->errindexp = snmp->inbuf + (typelen - 1);
   ASN1_READZERO_INT(snmp, typelen, snmp->reps);

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, typelen);
   snmp->inlen = typelen;

   return TRUE;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_parse
* Returned Value   :
* Comments  :  Parse an SNMP packet.
*
*END*-----------------------------------------------------------------*/

static boolean SNMP_parse
   (
      SNMP_PARSE_PTR snmp,
      uint_32_ptr    writelen
   )
{ /* Body */
   uint_32     typelen, maxheadlen;
   uint_32     ctylen, idlen;
   uchar_ptr   ctyp,   idp,  pdup;
   volatile uint_8 tmp;

   maxheadlen = 0;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_SEQUENCE, typelen);
   maxheadlen += 4;
   snmp->inlen = typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, typelen);
   ASN1_READ_INT(snmp, typelen, snmp->version);
   maxheadlen += 3;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_OCTET, ctylen);
   ctyp = snmp->inbuf;
   
   /* test for community string match */
   for (tmp=0; tmp < SNMPCFG_NUM_COMMUNITY; tmp++) {
      if (ctylen == snmp->communitylen[tmp]) {
         if( !memcmp(ctyp, snmp->community[tmp], ctylen) ) {
            break;
        }
      }
   } /* Endfor */
   
   /* if community string is bad, return */
   if(tmp == SNMPCFG_NUM_COMMUNITY) {
      IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_COMMUNITY++);
      /* Check before decrement, because SNMP_task will increment it */
      IF_SNMP_STATS_ENABLED(if(snmp->STATS.ST_RX_BAD_PARSE > 0) snmp->STATS.ST_RX_BAD_PARSE--);
      return FALSE;
   } /* Endif */
   
   ASN1_READ_IGNORE(snmp, ctylen);
   maxheadlen += ctylen + 4;

   pdup = snmp->inbuf;
   ASN1_READ_TYPELEN(snmp, snmp->pdutype, typelen);
   *pdup = ASN1_TYPE_PDU_RESPONSE;
   maxheadlen += 4;
   snmp->inlen = typelen;

   ASN1_READ_TYPELEN_EXPECT(snmp, ASN1_TYPE_INTEGER, idlen);
   idp = snmp->inbuf;
   ASN1_READ_IGNORE(snmp, idlen);
   maxheadlen += idlen + 4;

   maxheadlen += 10; /* two INTEGERs and a SEQUENCE */

   snmp->outbuf += maxheadlen;
   snmp->outlen -= maxheadlen;

#if RTCSCFG_ENABLE_SNMP_STATS
   switch (snmp->pdutype) {
   case ASN1_TYPE_PDU_GET:       snmp->STATS.ST_RX_GETREQ++;     break;
   case ASN1_TYPE_PDU_GETNEXT:   snmp->STATS.ST_RX_GETNEXTREQ++; break;
   case ASN1_TYPE_PDU_RESPONSE:  snmp->STATS.ST_RX_RESPONSE++;   break;
   case ASN1_TYPE_PDU_SET:       snmp->STATS.ST_RX_SETREQ++;     break;
   case ASN1_TYPE_PDU_TRAP:      snmp->STATS.ST_RX_TRAP++;       break;
   case ASN1_TYPE_PDU_GETBULK:   snmp->STATS.ST_RX_GETBULKREQ++; break;
   case ASN1_TYPE_PDU_INFORM:    snmp->STATS.ST_RX_INFORM++;     break;
   case ASN1_TYPE_PDU_TRAPV2:    snmp->STATS.ST_RX_V2TRAP++;     break;
   case ASN1_TYPE_PDU_REPORT:    snmp->STATS.ST_RX_REPORT++;     break;
   default:                      snmp->STATS.ST_RX_OTHER++;      break;
   } /* Endswitch */
#endif

   switch (snmp->pdutype) {
   case ASN1_TYPE_PDU_GET:
   case ASN1_TYPE_PDU_GETNEXT:
   case ASN1_TYPE_PDU_SET:
      if (snmp->version > 1) {
         IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_VERSION++);
         IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_PARSE--); /* because SNMP_task will increment it */
         return FALSE;
      } /* Endif */
      if (!SNMP_parse_pduv1(snmp)) return FALSE;
      break;
   case ASN1_TYPE_PDU_GETBULK:
      if (snmp->version != 1) {
         IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_VERSION++);
         IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_PARSE--); /* because SNMP_task will increment it */
         return FALSE;
      } /* Endif */
      if (!SNMP_parse_awesome_pdubulk(snmp)) return FALSE;
      snmp->pdutype = ASN1_TYPE_PDU_GETNEXT;
      break;
   default:
      IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_PDU++);
      IF_SNMP_STATS_ENABLED(snmp->STATS.ST_RX_BAD_PARSE--); /* because SNMP_task will increment it */
      return FALSE;
   } /* Endswitch */

   if (!SNMP_parse_varbindlist(snmp, &typelen)) return FALSE;

   ASN1_BKWRITE_TYPELEN       (snmp, typelen, ASN1_TYPE_SEQUENCE, typelen);
   ASN1_BKWRITE_TYPELEN_INT   (snmp, typelen, 0);
   ASN1_BKWRITE_TYPELEN_INT   (snmp, typelen, 0);
   ASN1_BKWRITE_TYPELEN_BYTES (snmp, typelen, ASN1_TYPE_INTEGER, idp, idlen);
   ASN1_BKWRITE_TYPELEN       (snmp, typelen, ASN1_TYPE_PDU_RESPONSE, typelen);
   ASN1_BKWRITE_TYPELEN_BYTES (snmp, typelen, ASN1_TYPE_OCTET, ctyp, ctylen);
   ASN1_BKWRITE_TYPELEN_INT   (snmp, typelen, snmp->version);
   ASN1_BKWRITE_TYPELEN       (snmp, typelen, ASN1_TYPE_SEQUENCE, typelen);

   *writelen = typelen;
   return TRUE;

} /* Endbody */





/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_init_with_traps
* Returned Value   : error code
* Comments  :  Start the SNMP agent.
*
*END*-----------------------------------------------------------------*/

uint_32 SNMP_init_with_traps
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize,
      /* Address to an array of IP addresses, NULL terminated */
      _ip_address _PTR_ traplist 
   )
{ /* Body */
   return RTCS_task_create(name, priority, stacksize, SNMP_task, traplist);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : SNMP_task
* Returned Value   : none
* Comments  :  SNMP agent.
*
*END*-----------------------------------------------------------------*/

void SNMP_task
   (
      pointer traps,
      pointer creator
   )
{ /* Body */
   SNMP_PARSE        snmp;
   sockaddr_in       addr;
   uint_32           sock, error, count;
   int_32            inlen, j;
   uint_32           outlen,time;
   uint_16           addrlen;
   uint_32           option;
   char_ptr          community_array[] = SNMPCFG_COMMUNITY_LIST;
   _ip_address _PTR_ trap_receiver_list_ptr = (_ip_address _PTR_)traps;

   SNMP_CONTEXT    snmp_context =  { 0 };

   SNMP_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if the SNMP agent is terminated */
   _task_set_environment( _task_get_id(), (pointer) &snmp_context );
   _task_set_exit_handler( _task_get_id(), SNMP_Exit_handler );
#endif 
  
   /* SNMP agent services port 161 */
   addr.sin_family      = AF_INET;
   addr.sin_port        = IPPORT_SNMP;
   addr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   snmp_context.UDPSOCK = socket(PF_INET, SOCK_DGRAM, 0);
   sock = snmp_context.UDPSOCK;
   
   if (sock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   
   /* Begin ENGR00243356 */
   /* Set socket options to no wait */
   option = TRUE;
   setsockopt(sock, SOL_UDP, OPT_SEND_NOWAIT, &option, sizeof(option));
   /* End ENGR00243356 */  
   
   error = bind(sock, &addr, sizeof(addr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   IF_SNMP_STATS_ENABLED(_mem_zero(&snmp.STATS, sizeof(snmp.STATS)));
   snmp.trapsock     = sock;

   snmp.currentcommunity = 0;

   for (j = 0; j < SNMPCFG_NUM_COMMUNITY; j++) {
      snmp.community[j]   = community_array[j];
      snmp.communitylen[j]   = strlen((const char *)(community_array[j]));
   } /* Endfor */
   
   for (j = 0; j < SNMPCFG_MAX_TRAP_RECEIVERS; j++)
      snmp.trap_receiver_list[j] = 0;
   
   if(trap_receiver_list_ptr != NULL) {
      count = 0;
      while(*trap_receiver_list_ptr != 0 && count < SNMPCFG_MAX_TRAP_RECEIVERS) {
         snmp.trap_receiver_list[count] = *trap_receiver_list_ptr;
         trap_receiver_list_ptr++;
         count++;   
      } /* Endwhile */
   } /* Endif */
   
   SNMP_set_data(&snmp);

   RTCS_task_resume_creator(creator, RTCS_OK);

   /* Send some traps -- but only if the application has added some traps receivers */
   if (RTCS_count_trap_receivers_internal() != 0) {
      SNMP_trap_select_community(snmp.community[0]);
      time = RTCS_time_get();   
      if(time < 30) {
#ifdef SNMPCFG_SEND_V2_TRAPS
         SNMPv2_trap_coldStart();
#else
         SNMP_trap_coldStart();
#endif         
      } else {
#ifdef SNMPCFG_SEND_V2_TRAPS
         SNMPv2_trap_warmStart();
#else
         SNMP_trap_warmStart();
#endif         
      } /* Endif */
   } /* Endif */

   for (;;) {

      addrlen = sizeof(addr);
      inlen = recvfrom(sock, inbuf, SNMPCFG_BUFFER_SIZE, 0, &addr, &addrlen);
      IF_SNMP_STATS_ENABLED(snmp.STATS.COMMON.ST_RX_TOTAL++);

      snmp.inbuf = inbuf;
      snmp.inlen = inlen;
      snmp.outbuf = outbuf;
      snmp.outlen = SNMPCFG_BUFFER_SIZE;
      snmp.pdutype = 0; /* anything except ASN1_TYPE_PDU_SET */
      snmp.errstat = SNMP_ERROR_noError;
      snmp.errindex = 0;
      if (SNMP_parse(&snmp, &outlen)) {
         IF_SNMP_STATS_ENABLED(snmp.STATS.COMMON.ST_TX_TOTAL++);
         IF_SNMP_STATS_ENABLED(snmp.STATS.ST_TX_RESPONSE++);
         sendto(sock, snmp.outbuf, outlen, 0, &addr, addrlen);
      } else if (snmp.errstat || snmp.pdutype == ASN1_TYPE_PDU_SET) {
         if (snmp.errindex >= 0xFF) {
            snmp.errstat = SNMP_ERROR_tooBig;
            snmp.errindex = 0;
         } /* Endif */
         *snmp.errstatp  = snmp.errstat;
         *snmp.errindexp = snmp.errindex;
#if RTCSCFG_ENABLE_SNMP_STATS
         snmp.STATS.COMMON.ST_TX_TOTAL++;
         snmp.STATS.ST_TX_RESPONSE++;
         switch (snmp.errstat) {
         case SNMP_ERROR_noError:    break;
         case SNMP_ERROR_tooBig:     snmp.STATS.ST_TX_TOOBIG++;     break;
         case SNMP_ERROR_noSuchName: snmp.STATS.ST_TX_NOSUCHNAME++; break;
         case SNMP_ERROR_badValue:   snmp.STATS.ST_TX_BADVALUE++;   break;
         case SNMP_ERROR_readOnly:   snmp.STATS.ST_TX_READONLY++;   break;
         case SNMP_ERROR_genErr:     snmp.STATS.ST_TX_GENERR++;     break;
         default:                    snmp.STATS.ST_TX_OTHER++;      break;
         } /* Endswitch */
#endif
         sendto(sock, inbuf, inlen, 0, &addr, addrlen);
      } else {
         IF_SNMP_STATS_ENABLED(snmp.STATS.ST_RX_BAD_PARSE++);
         IF_SNMP_STATS_ENABLED(snmp.STATS.COMMON.ST_RX_DISCARDED++);
      } /* Endif */

   } /* Endfor */

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_Exit_handler
* Returned Value   : error code
* Comments  :  Start the EDS task.
*
*END*-----------------------------------------------------------------*/

void SNMP_Exit_handler( void )
{ /* Body */
   SNMP_CONTEXT_PTR    snmp_context_ptr;
   
   snmp_context_ptr = (SNMP_CONTEXT_PTR) _task_get_environment( _task_get_id() );
   if (snmp_context_ptr != NULL)  {
      if (snmp_context_ptr->UDPSOCK)  {
         shutdown(snmp_context_ptr->UDPSOCK, 0);
         snmp_context_ptr->UDPSOCK = 0;   
      }   
   }
   SNMP_task_id = 0;
} /* Endbody */


#endif /* RTCSCFG_ENABLE_IP4 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_init
* Returned Value   : error code
* Comments  :  Start the SNMP agent.
*
*END*-----------------------------------------------------------------*/

uint_32 SNMP_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4
   
   if (SNMP_task_id == 0)  {
      return RTCS_task_create(name, priority, stacksize, SNMP_task, NULL);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
#else

    return RTCSERR_IP_IS_DISABLED; 

#endif /* RTCSCFG_ENABLE_IP4 */      
   
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : SNMP_stop
* Returned Value   : error code
* Comments  :  Start the EDS task.
*
*END*-----------------------------------------------------------------*/

uint_32 SNMP_stop( void )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   if (SNMP_task_id == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(SNMP_task_id);
   SNMP_task_id = 0;
   return RTCS_OK;
   
#else

    return RTCSERR_IP_IS_DISABLED; 

#endif /* RTCSCFG_ENABLE_IP4 */ 
   
} /* Endbody */



#endif /* RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_UDP */
/* EOF */
