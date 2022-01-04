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
* $FileName: dhcpclnt.c$
* $Version : 3.8.12.0$
* $Date    : Aug-6-2012$
*
* Comments:
*
*   This file contains the implementation of the DHCP client
*   for the TCP/IP protocol suite.
*   For more information, refer to:
*   RFC 2131 (DHCP)
*   RPC 2132 (DHCP Options).
*   Limitations:  Although DHCP can be used over any medium, this
*   implementation will work only over an Ethernet
*   link layer.
*   Implementation Note:
*   The DHCP Client packet is set up in such a way that it may be re-used
*   for any message the DCHP Client needs to send.  Only DHCP_MAGIC is
*   required for a DHCPDISCOVER message, the other options may or may not
*   be present if the application did not specify them upon initialization.
*   If the option is passed to the DHCP Client, it will be placed in the
*   packet in the following order.  The Server and Client IP pads will be
*   used even if no other options are specified.
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |                       DHCP_MAGIC (4)                          |
*   +---------------------------------------------------------------+
*   |                                                               |
*   //                 ClIENT ID (variable, 6 - 255)               //
*   |                                                               |
*   +-----------------------------------------------+---------------+
*   |                  OVERLOAD (3)                 |
*   +-----------------------------------------------+--------------------------+
*   |                  SERVER IP PAD (6)  (zero, unless used)                  |
*   +--------------------------------------------------------------------------+
*   |                  CLIENT IP PAD (6)  (zero, unless used)                  |
*   +-------------------+------------------------------------------------------+
*   | BUFFER END PAD (1)|
*   +-------------------+-------------------------------------------+
*   |                                                               |
*   //                  PARAMETER LIST (variable, 3 - 255)         //
*   |                                                               |
*   +---------------------------------------------------------------+
*   |                   REQUESTED LEASE TIME (4)                    |
*   +---------------------------------------------------------------+
*   |                                                               |
*   //                  CLASS ID (variable, 6 -255)                //
*   |                                                               |
*   +---------------------------------------------------------------+
*   |                   MAX MESSAGE SIZE (4)                        |
*   +---------------------------------------------------------------+
*   |                                                               |
*   //                  SITE SPECIFIC OPTIONS (variable)           //
*   |                                                               |
*   +--------------------+------------------------------------------+
*   | BUFFER END PAD (1) |
*   +--------------------------------------------------------------------------+
*   |                   REQUESTED CLIENT IP PAD (6)                            |
*   +--------------------+-----------------------------------------------------+
*   | BUFFER END PAD (1) |
*   +--------------------+
*   Pointers are kept to modify the packet, depending on the current state.
*   Please refer to RFC 2131, pg 38, to see what options are allowed in
*   each situation.  As an example, if sending a DHCPDECLINE message, only the
*   Server IP is allowed.  Therefore, the Server IP pad section is filled using
*   the DHCPCLNT_modify_options() function and the first buffer end pad is
*   filled with 0xFF. Only this section of the buffer will be sent to the
*   Server.  When a state change occurs, these pads will be zeroed out and
*   the packet will be set up for the correct message type.
*   There are three timed events which may or may not be active depending on
*   the current state. If we are initializing for the first time, only the
*   RESEND event will be active. Once we have a lease and are in a bound state,
*   the RESEND event is cancelled and the RENEW and TERMINATE events are set.
*   If we are in a RESENDING state, all three events will be set until we are
*   re-bound. If we are in a REBINDING state, only the RESEND and TERMINATE
*   events will be active.  If we received a DHCPNAK message when renewing or
*   rebinding, we will try to get a new lease starting from the INITIALIZE
*   state. If the DHCP server does not wish to renew the current lease,
*   then only the TERMINATE event will be set for the remainder of the lease. SMR
*
*END************************************************************************/

#include <string.h>
#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"
#include "dhcp_prv.h"
#include "dhcpuprv.h"
#include "arp.h"

extern UCB_STRUCT_PTR   BOOT_port;


uint_32 DHCPCLNT_probe_address
   (
      _ip_address       ipaddress,
      pointer           interface
   );

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_init
* Returned Values : void
* Comments        :
*     Initialize the DHCP client. This function should only be called by the
*     user's application
*
*END*-----------------------------------------------------------------*/

void DHCPCLNT_init
   (
      TCPIP_PARM_IF_DHCP _PTR_ parms
         /* [IN/OUT] This is the DHCP Client structure from the application */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr;
   uint_32                buff_len;
   uint_32                error;

   buff_len = parms->OPT_BUFF_SIZE + sizeof(DHCP_HEADER) + 23;

   dhcp_ptr = RTCS_mem_alloc_zero(sizeof(DHCP_CLNT_STRUCT) + buff_len);
#if RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS
   if ( dhcp_ptr == NULL ) {
      RTCSCMD_complete(parms, RTCSERR_OUT_OF_MEMORY);
      return;
   } /* Endif */
#endif
   _mem_set_type(dhcp_ptr, MEM_TYPE_DHCP_CLNT_STRUCT);

    RTCS_rand_seed(RTCS_time_get()); /* Just add more randomize  to Transition ID.*/
   
   error = DHCPCLNT_start(parms, dhcp_ptr);

#if RTCSCFG_CHECK_ERRORS
   if ( error ) {
      /* Self Destruct, can't do anything if the port isn't open */
      _mem_free( dhcp_ptr );
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */
#endif

   DHCPCLNT_open(dhcp_ptr);

   RTCSCMD_complete(parms, error);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);
    return;

#endif /* RTCSCFG_ENABLE_IP4 */  
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_reinit
* Returned Values : void
* Comments        :
*     Initialize the DHCP client without the initial DHCP negotiation.
*
*END*-----------------------------------------------------------------*/

void DHCPCLNT_reinit
   (
      TCPIP_PARM_IF_DHCP _PTR_ parms
         /* [IN/OUT] This is the DHCP Client structure from the application */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr;
   uint_32                buff_len;
   uint_32                error;

   buff_len = parms->OPT_BUFF_SIZE + sizeof(DHCP_HEADER) + 23;

   dhcp_ptr = RTCS_mem_alloc_zero(sizeof(DHCP_CLNT_STRUCT) + buff_len);
#if RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS
   if ( dhcp_ptr == NULL ) {
      RTCSCMD_complete(parms, RTCSERR_OUT_OF_MEMORY);
      return;
   } /* Endif */
#endif
   _mem_set_type(dhcp_ptr, MEM_TYPE_DHCP_CLNT_STRUCT);

   error = DHCPCLNT_start(parms, dhcp_ptr);

#if RTCSCFG_CHECK_ERRORS
   if ( error ) {
      /* Self Destruct, can't do anything if the port isn't open */
      _mem_free( dhcp_ptr );
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */
#endif

   dhcp_ptr->DATA.ServerIp        = parms->SERVER_IP_ADDR;
   dhcp_ptr->DATA.ClientIPaddress = parms->CLNT_IP_ADDR;
   dhcp_ptr->DATA.LEASE           = parms->LEASE;
   dhcp_ptr->DATA.Netmask         = parms->CLNT_IP_MASK;

   dhcp_ptr->PARMS_BIND.address = parms->CLNT_IP_ADDR;
   dhcp_ptr->PARMS_BIND.locmask = INADDR_BROADCAST;
   dhcp_ptr->PARMS_BIND.netmask = parms->CLNT_IP_MASK;
   dhcp_ptr->PARMS_BIND.probe   = FALSE;

   RTCSCMD_internal(dhcp_ptr->PARMS_BIND, IPIF_bind);

   dhcp_ptr->BOUND = TRUE;
   dhcp_ptr->STATE = DHCP_BOUND;

   dhcp_ptr->NEW_RENEW_TIME = 0;
   dhcp_ptr->NEW_REBIND_TIME = 0;
   DHCPCLNT_set_timed_events(dhcp_ptr);
   TCPIP_Event_add(&dhcp_ptr->RENEW);
   TCPIP_Event_add(&dhcp_ptr->TERMINATE);

   RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);
    return;

#endif /* RTCSCFG_ENABLE_IP4 */  
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_start
* Returned Values : error code
* Comments        :
*     Initialize the DHCP client state structure.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

uint_32 DHCPCLNT_start
   (
      TCPIP_PARM_IF_DHCP _PTR_ parms,
         /* [IN/OUT] This is the DHCP Client structure from the application */
      DHCP_CLNT_STRUCT _PTR_ dhcp_ptr
         /* [IN/OUT] This is the DHCP Client control structure */
   )
{ /* Body */
   uint_32     buff_len;

   /*
   ** 23: 4 = Magic Cookie, 3 = Msg Type, 6 = ServerID, 6 = Requested IP,
   ** 4 = Various Ends
   */
   buff_len = parms->OPT_BUFF_SIZE + sizeof(DHCP_HEADER) + 23;

   if ( parms->FLAGS & DHCP_SEND_INFORM_MESSAGE ) {
      dhcp_ptr->STATE = DHCP_INFORMED;
      dhcp_ptr->DATA.ClientIPaddress = parms->CLNT_IP_ADDR;
      if ( parms->SERVER_IP_ADDR == INADDR_ANY ) {
         dhcp_ptr->DATA.ServerIp = INADDR_BROADCAST;
      } else  {
         dhcp_ptr->DATA.ServerIp = parms->SERVER_IP_ADDR;
      } /* Endif */
      dhcp_ptr->BOUND = TRUE;
   } else  {
      dhcp_ptr->STATE = DHCP_INITIALIZE;
   }/* Endif */
   dhcp_ptr->HANDLE             = (IP_IF _PTR_)parms->HANDLE;
   dhcp_ptr->PARMS_BIND.ihandle = parms->HANDLE;
   dhcp_ptr->HANDLE->BOOTFN     = NULL;
   dhcp_ptr->HANDLE->BOOT       = dhcp_ptr;
   dhcp_ptr->CHOICE_FUNC        = parms->OPTIONS->CHOICE_FUNC;
   dhcp_ptr->BIND_FUNC          = parms->OPTIONS->BIND_FUNC;
   dhcp_ptr->UNBIND_FUNC        = parms->OPTIONS->UNBIND_FUNC;
   dhcp_ptr->FAILURE_FUNC       = parms->OPTIONS->FAILURE_FUNC;
   dhcp_ptr->NAK_FUNC           = parms->OPTIONS->NAK_FUNC;
   dhcp_ptr->FLAGS              = parms->FLAGS;
   dhcp_ptr->PACKET             = (uchar _PTR_)(dhcp_ptr + 1);
   dhcp_ptr->TOTAL_PACKET_SIZE  = buff_len;
   dhcp_ptr->RESEND.EVENT       = DHCPCLNT_send;
   dhcp_ptr->RESEND.PRIVATE     = dhcp_ptr;
   dhcp_ptr->RENEW.EVENT        = DHCPCLNT_renew_lease;
   dhcp_ptr->RENEW.PRIVATE      = dhcp_ptr;
   dhcp_ptr->TERMINATE.EVENT    = DHCPCLNT_terminate_lease;
   dhcp_ptr->TERMINATE.PRIVATE  = dhcp_ptr;
   dhcp_ptr->INIT_TIMEOUT       = parms->TIMEOUT * 1000;

   DHCPCLNT_fill_header( dhcp_ptr );
   DHCPCLNT_fill_options( dhcp_ptr, parms );

   return BOOT_open(&DHCPCLNT_service);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_fill_header
* Returned Values : void
* Comments        :
*     Sets up the DHCP Client header structure.
*
*END*-----------------------------------------------------------------*/
#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_fill_header
   (
   DHCP_CLNT_STRUCT  _PTR_ dhcp_ptr
   /* [IN/OUT] This is the DHCP Client control structure */
   )
{ /* Body */
   DHCP_HEADER _PTR_      header_ptr;
   uint_32                xid;

   header_ptr = (DHCP_HEADER _PTR_)dhcp_ptr->PACKET;

   /* Build the DHCP HEADER  */
   htonc(header_ptr->OP,    DHCPOP_BOOTREQUEST);
   htonc(header_ptr->HTYPE, dhcp_ptr->HANDLE->DEV_TYPE);
   htonc(header_ptr->HLEN,  dhcp_ptr->HANDLE->DEV_ADDRLEN);
   htonc(header_ptr->HOPS,  0);
   /* Pick a random transaction ID */
   xid = RTCS_rand();
   htonl(header_ptr->XID, xid);

   /* htons(header_ptr->FLAGS, DHCPFLAG_BROADCAST); */
   htons(header_ptr->FLAGS, _DHCP_broadcast ? DHCPFLAG_BROADCAST : 0);
   
   if ( dhcp_ptr->STATE == DHCP_INFORMED ) {
      htonl(header_ptr->CIADDR, dhcp_ptr->DATA.ClientIPaddress);
   } else {
      htonl(header_ptr->CIADDR, INADDR_ANY);
   } /* Endif */
   htonl(header_ptr->YIADDR, INADDR_ANY);
   htonl(header_ptr->SIADDR, INADDR_ANY);
   htonl(header_ptr->GIADDR, INADDR_ANY);

   _mem_zero(header_ptr->CHADDR, sizeof(header_ptr->CHADDR));
   _mem_copy(dhcp_ptr->HANDLE->DEV_ADDR, header_ptr->CHADDR, dhcp_ptr->HANDLE->DEV_ADDRLEN);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_fill_options
* Returned Values : void
* Comments        :
*     Checks and fills in the DHCP Client options in the buffer. This
*     function should only be called once upon initialization. The DHCP_FILL
*     macro is defined in DHCP_PRV.H.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_fill_options
   (
   DHCP_CLNT_STRUCT  _PTR_ dhcp_ptr,
   /* [IN/OUT] This is the DHCP Client control structure */
   TCPIP_PARM_IF_DHCP _PTR_ parms
   /* [IN/OUT] This is the DHCP Client structure from the application */
   )
{ /* Body */
   uchar _PTR_ outp;
   uchar _PTR_ temp_ptr;
   uchar _PTR_ opt_ptr;
   uint_32     len;
   uint_32     i;
   uint_32     new_total_length;
   uchar       addr_buffer[6];
   uchar _PTR_ buffer_end   = parms->OPT_BUFF
                              + parms->OPT_BUFF_SIZE - 1;

   outp = dhcp_ptr->PACKET + sizeof(DHCP_HEADER);
   htonl(outp, DHCP_MAGIC);            outp += 4;
   htonc(outp, DHCPOPT_MSGTYPE);       outp++;
   htonc(outp, 1);                     outp++;
   dhcp_ptr->MSGTYPE_PTR = outp;
   if ( dhcp_ptr->STATE == DHCP_INFORMED ) {
      htonc(outp, DHCPTYPE_DHCPINFORM);
   } else  {
      htonc(outp, DHCPTYPE_DHCPDISCOVER);
   } /* Endif */
   outp++;
   /*
   ** Only the following options are allowed for a DHCP Client message.
   ** Client ID, Use 'file'/'sname' fields, Parameter request list,
   ** IP address lease time, Class Id, Max message size, Requested IP addr.
   ** DO NOT change the order of insertion into the buffer, as other
   ** functions will depend on the order given here.
   ** See pg. 33 of RFC 1541 for more details.
   */
   /* Minimum size allowed is 2 (added 2 for option and length fields) */
   DHCP_FILL(CLIENTID, (len >= 4))
   /*
   ** Allows 'file'/'sname' fields to be used in replies from DHCP server
   ** Since we also use these fields for BOOTP, we do not allow this
   ** functionality at this time.  However, if we are to offer it in the future,
   ** other functions depend on us filling in the overload option here.
   ** Leave the commented code here for potential future use. SMR
   */
   /* Minimum size allowed is 1 (added 2 for option and length fields) */
   /* DHCP_FILL(OVERLOAD, (len == 3)) */

   dhcp_ptr->SERVERID_PTR = outp;
   /* This is to pad for insertion of ServerID and Requested IP later */
   outp += 13;

   /* Minimum size allowed is 1 (added 2 for option and length fields) */
   DHCP_FILL(PARAMLIST, (len >= 3))
   if ( dhcp_ptr->STATE != DHCP_INFORMED ) {
      /* Size must be 4 (added 2 for option and length fields) */
      DHCP_FILL(LEASE, (len == 6))
   }/* Endif */
   /* Minimum size allowed is 1 (added 2 for option and length fields) */
   DHCP_FILL(CLASSID, (len >= 3))

   opt_ptr = DHCP_find_option(parms->OPT_BUFF,
                              parms->OPT_BUFF_SIZE,
                              DHCPOPT_MSGSIZE);
   if (opt_ptr) {
      /* 576 octets is an imposed minimum, RFC 2131 */
      if ( ntohs(opt_ptr + 2) < DHCP_MIN_MESSAGE_SIZE ) {
         htonc(outp, DHCPOPT_MSGSIZE);       outp++;
         htonc(outp, 2);                     outp++;
         htons(outp, DHCP_MIN_MESSAGE_SIZE); outp += 2;
      } else  {
         _mem_copy(opt_ptr, outp, 4);
         outp += 4;
      } /* Endif */
      for (i = 0; i < 4; i++) {
         htonc(opt_ptr, 0);
         opt_ptr++;
      } /* Endfor */
   } /* Endif */

   /*
   ** Have to parse out the ADDRESS option, as it goes after the site specific
   ** options in the packet.
   */
   opt_ptr = DHCP_find_option(parms->OPT_BUFF,
                              parms->OPT_BUFF_SIZE,
                              DHCPOPT_ADDRESS);
   if ( opt_ptr ) {
      for (i = 0; i < 6; i++) {
         addr_buffer[i] = ntohc(opt_ptr);
         htonc(opt_ptr, 0);
         opt_ptr++;
      } /* Endfor */
   }/* Endif */

   /*
   ** Remove all the remaining options and place in the site specific section.
   ** Break out of the while loop once all the options are parsed.
   */

   temp_ptr = parms->OPT_BUFF;
   while ( temp_ptr ) {
      temp_ptr = parms->OPT_BUFF;
      while ((ntohc(temp_ptr) == 0) && (temp_ptr < buffer_end)) {
         temp_ptr++;
      } /* Endwhile */
      if ( ntohc(temp_ptr) != 0 ) {
         new_total_length = buffer_end - temp_ptr + 1;
         len = ntohc(temp_ptr + 1) + 2;
         if ( len <= new_total_length ) {
            _mem_copy(temp_ptr, outp, len);
            outp += len;
         } else  {
            len = new_total_length;
         }/* Endif */
         for ( i = 0; i < len; i++) {
            htonc(temp_ptr, 0);
            temp_ptr++;
         } /* Endfor */
      } else  {
         temp_ptr = NULL;
      }/* Endif */
   } /* Endwhile */

   dhcp_ptr->REQUEST_OPT_PTR = outp;
   /* This pad is for DHCP REQUEST messages so we won't overwrite the addr */
   outp++;

   if (opt_ptr && (dhcp_ptr->STATE != DHCP_INFORMED)) {
      for ( i = 0; i < 6; i++ ) {
         htonc(outp, addr_buffer[i]);
         outp++;
      } /* Endfor */
   } /* Endif */

   /* Add the DHCPEND flag last */
   htonc(outp, DHCPOPT_END);
   outp++;
   dhcp_ptr->CURRENT_PACKET_SIZE = outp - dhcp_ptr->PACKET;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_open
* Returned Values : void
* Comments        : This function is used to open the DHCP Client Port
*                   and set up the RESEND event.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_open
   (
   DHCP_CLNT_STRUCT  _PTR_ dhcp_ptr
   /* [IN/OUT] This is the DHCP Client control structure */
   )
{ /* Body */
   DHCP_HEADER _PTR_   header_ptr = (DHCP_HEADER _PTR_)dhcp_ptr->PACKET;

   htons(header_ptr->SECS, 0);
   /* Set initial timeout */
   dhcp_ptr->RETRY_TIMEOUT = DHCP_TIMEOUT_MIN;
   dhcp_ptr->SECS = 0;
   /* Start the retransmission timer to start sending immediately */
   dhcp_ptr->RESEND.TIME    = 0;
   TCPIP_Event_add( &dhcp_ptr->RESEND );

   if (dhcp_ptr->INIT_TIMEOUT) {
      /* Set a cancellation event in case of failure to bind. */
      dhcp_ptr->TERMINATE.EVENT    = DHCPCLNT_bind_attempt_timeout;
      dhcp_ptr->TERMINATE.TIME     = dhcp_ptr->INIT_TIMEOUT;
      TCPIP_Event_add(&dhcp_ptr->TERMINATE);
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_modify_options
* Returned Values : void
* Comments        :
*     Modifies the DHCP Client buffer for packet send, depending on the
*     current context. Allowed options change depending on the packet type.
*     NOTE: State MUST be checked BEFORE calling, this function is not used
*     in an initial or terminated state
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_modify_options
   (
   DHCP_CLNT_STRUCT  _PTR_ dhcp_ptr
   /* [IN/OUT] This is the DHCP Client control structure */
   )
{  /* Body */
   DHCP_HEADER _PTR_ header_ptr;
   uchar       _PTR_ outp;
   uint_32           i;

   header_ptr = (DHCP_HEADER _PTR_)dhcp_ptr->PACKET;
   outp       = dhcp_ptr->SERVERID_PTR;

   for ( i = 0 ; i < 13 ; i++ ) {
         htonc(outp, 0); outp++;
      } /* Endfor */
   outp -= 13;
   htonc(dhcp_ptr->REQUEST_OPT_PTR, 0);

   if ( dhcp_ptr->BOUND && dhcp_ptr->STATE != DHCP_REQUESTING ) {
      htonl(header_ptr->CIADDR, dhcp_ptr->DATA.ClientIPaddress);
   } else  {
      htonl(header_ptr->CIADDR, INADDR_ANY);
   }/* Endif */

   if ( dhcp_ptr->STATE == DHCP_INITIALIZE ) {
      htonc(dhcp_ptr->MSGTYPE_PTR, DHCPTYPE_DHCPDISCOVER);
      dhcp_ptr->CURRENT_PACKET_SIZE = dhcp_ptr->TOTAL_PACKET_SIZE;
   } else  {
      htonc(dhcp_ptr->MSGTYPE_PTR, DHCPTYPE_DHCPREQUEST);
      if ( dhcp_ptr->STATE == DHCP_REQUESTING ||
           dhcp_ptr->STATE == DHCP_DECLINING  ||
           dhcp_ptr->STATE == DHCP_RELEASING ) {
         htonc(outp, DHCPOPT_SERVERID); outp++;
         htonc(outp, 4); outp++;
         if ( dhcp_ptr->STATE == DHCP_RELEASING ) {
            htonc(dhcp_ptr->MSGTYPE_PTR, DHCPTYPE_DHCPRELEASE);
            htonl(outp, dhcp_ptr->DATA.ServerIp); outp += 4;
         } else  {
            htonl(outp, dhcp_ptr->NEW_DATA.ServerIp); outp += 4;
            htonc(outp, DHCPOPT_ADDRESS); outp++;
            htonc(outp, 4); outp++;
            htonl(outp, dhcp_ptr->NEW_DATA.ClientIPaddress); outp += 4;
            if (dhcp_ptr->STATE == DHCP_DECLINING ) {
               htonl(header_ptr->CIADDR, INADDR_ANY);
               htonc(dhcp_ptr->MSGTYPE_PTR, DHCPTYPE_DHCPDECLINE);
            } else  {
               outp = dhcp_ptr->REQUEST_OPT_PTR;
            }/* Endif */
         } /* Endif */
      } else  {
         /* We're Renewing or Rebinding */
         outp = dhcp_ptr->REQUEST_OPT_PTR;
      }/* Endif */
      htonc(outp, DHCPOPT_END); outp++;
      dhcp_ptr->CURRENT_PACKET_SIZE = outp - dhcp_ptr->PACKET;
   }/* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_send
* Returned Value  : boolean
* Comments        :
*     Called by the Timer.  Send a DHCP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

boolean DHCPCLNT_send
   (
   TCPIP_EVENT_PTR   event
   /* [IN/OUT] the resend event */
   )
{ /* Body */
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr = event->PRIVATE;
   DHCP_HEADER     _PTR_ header_ptr;
   RTCSPCB_PTR           pcb_ptr;
   _ip_address           destination_address;
   uint_32               retry_time;

   if ( dhcp_ptr->S_EVENT_TIME_LEFT ) {
      DHCPCLNT_TIME_OVERFLOW_CHECK(retry_time, dhcp_ptr->S_EVENT_TIME_LEFT)
      dhcp_ptr->RESEND.TIME = retry_time;
      return( TRUE );
   }/* Endif */

   switch ( dhcp_ptr->STATE ) {

      case DHCP_REQUESTING:
      case DHCP_INFORMED:
         /* Must retry 4 times before starting over, total is under 60 secs */
         if ( dhcp_ptr->RETRY_TIMEOUT != DHCP_TIMEOUT_MAX ) {
            /*Set the event to trigger for the next retransmission (+/- 1 sec)*/
            retry_time = dhcp_ptr->RETRY_TIMEOUT +
                         (RTCS_rand() & 0x7FF) - 0x400;
            if ( dhcp_ptr->RENEW_TIME &&
                 dhcp_ptr->RENEW_TIME < dhcp_ptr->REBIND_TIME )
            {
               dhcp_ptr->RENEW_TIME += (retry_time / 1000);
            } else  {
               /* We were rebinding, increment rebind time */
               dhcp_ptr->REBIND_TIME += (retry_time / 1000);
            }/* Endif */
            dhcp_ptr->RESEND.TIME = retry_time;
            /* Double the timeout */
            dhcp_ptr->RETRY_TIMEOUT <<= 1;
            if (dhcp_ptr->RETRY_TIMEOUT > DHCP_TIMEOUT_MAX) {
               dhcp_ptr->RETRY_TIMEOUT = DHCP_TIMEOUT_MAX;
            } /* Endif */
            dhcp_ptr->STATS.ST_TX_REQUEST++;
            break;
         } else  {
            if ( dhcp_ptr->STATE == DHCP_INFORMED ) {
               /*
               ** There was no response to our request for parameters, inform
               ** the application to use defaults and self destruct
               */
               if ( dhcp_ptr->BIND_FUNC != NULL ) {
                  dhcp_ptr->BIND_FUNC(NULL, 0,
                                      (_rtcs_if_handle)dhcp_ptr->HANDLE);
               } /* Endif */
               BOOT_close();
               dhcp_ptr->HANDLE->BOOT = NULL;
               /* Free the DHCP Structure */
               _mem_free(dhcp_ptr);
               return( FALSE );
            } else  {
               /* We were initializing from an unbound state before */
               dhcp_ptr->STATE = DHCP_INITIALIZE;
               dhcp_ptr->RETRY_TIMEOUT = DHCP_TIMEOUT_MIN;
               DHCPCLNT_modify_options( dhcp_ptr );
            } /* Endif */
         }/* Endif */

      case DHCP_INITIALIZE:
         /* Set the event to trigger for the next retransmission (+/- 1 sec) */
         retry_time = dhcp_ptr->RETRY_TIMEOUT + (RTCS_rand() & 0x7FF) - 0x400;

         dhcp_ptr->RESEND.TIME = retry_time;
         /* Double the timeout */
         dhcp_ptr->RETRY_TIMEOUT <<= 1;
         if (dhcp_ptr->RETRY_TIMEOUT > DHCP_TIMEOUT_MAX) {
            dhcp_ptr->RETRY_TIMEOUT = DHCP_TIMEOUT_MAX;
         } /* Endif */
         dhcp_ptr->STATS.ST_TX_DISCOVER++;
         break;

      case DHCP_RENEWING:
         retry_time = (dhcp_ptr->REBIND_TIME - dhcp_ptr->RENEW_TIME)/2;
         if ( retry_time < 60 ) {
            /* A 60 second minimum is imposed */
            retry_time = 60;
         }/* Endif */
         dhcp_ptr->RENEW_TIME += retry_time;

         dhcp_ptr->S_EVENT_TIME_LEFT = retry_time;
         DHCPCLNT_TIME_OVERFLOW_CHECK(retry_time, dhcp_ptr->S_EVENT_TIME_LEFT)

         dhcp_ptr->RESEND.TIME = retry_time;
         dhcp_ptr->STATS.ST_TX_REQUEST++;
         break;

      case DHCP_REBINDING:
         retry_time = (dhcp_ptr->DATA.LEASE - dhcp_ptr->REBIND_TIME)/2;
         if ( retry_time < 60 ) {
            /* A 60 second minimum is imposed */
            retry_time = 60;
         }/* Endif */
         if ((dhcp_ptr->REBIND_TIME + retry_time) < dhcp_ptr->DATA.LEASE) {
            dhcp_ptr->REBIND_TIME += retry_time;
         }/* Endif */

         dhcp_ptr->S_EVENT_TIME_LEFT = retry_time;
         DHCPCLNT_TIME_OVERFLOW_CHECK(retry_time, dhcp_ptr->S_EVENT_TIME_LEFT)

         dhcp_ptr->RESEND.TIME = retry_time;
         dhcp_ptr->STATS.ST_TX_REQUEST++;

   } /* Endswitch */

   /* Allocate a PCB */
   pcb_ptr = RTCSPCB_alloc_send();
   if (pcb_ptr == NULL) {
      return TRUE;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb_ptr);

   /* The only field that changes in DHCP packets is 'secs' */
   header_ptr = (DHCP_HEADER _PTR_)dhcp_ptr->PACKET;
   htons(header_ptr->SECS, dhcp_ptr->SECS);
   dhcp_ptr->SECS += (retry_time / 1000);

   /* Put the DHCP packet in the PCB */
   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_PORT(IPPORT_BOOTPC), 0);
   RTCSPCB_append_fragment(pcb_ptr, dhcp_ptr->CURRENT_PACKET_SIZE, dhcp_ptr->PACKET);

   if ( dhcp_ptr->BOUND ) {
      /* We already have an IP address available, we're renewing or rebinding */
      if ( dhcp_ptr->STATE == DHCP_RENEWING ||
           dhcp_ptr->STATE == DHCP_INFORMED )
      {
         /* Unicast the message to the server */
         destination_address = dhcp_ptr->DATA.ServerIp;
      } else {
         /* Broadcast the request */
         destination_address = INADDR_BROADCAST;
      } /* Endif */

      UDP_send_internal(BOOT_port,
                        dhcp_ptr->DATA.ClientIPaddress,
                        destination_address,
                        DHCP_SERVER_PORT,
                        pcb_ptr,
                        0);
   } else {
      /* Send the datagram */
      BOOT_send(pcb_ptr, dhcp_ptr->HANDLE);
   } /* Endif */

   return TRUE;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_send_one_shot
* Returned Values : void
* Comments        : Sends a single DHCP packet without resend.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_send_one_shot
   (
    DHCP_CLNT_STRUCT  _PTR_ dhcp_ptr
   /* [IN/OUT] This is the DHCP Client control structure */
   )
{ /* Body */
   RTCSPCB_PTR       pcb_ptr;

   /* Allocate a PCB */
   pcb_ptr = RTCSPCB_alloc_send();
   if (pcb_ptr == NULL) {
      return;
   } /* Endif */

   //RTCSLOG_PCB_ALLOC(pcb_ptr);

   /* Put the DHCP packet in the PCB */
   RTCSLOG_PCB_WRITE(pcb_ptr, RTCS_LOGCTRL_PORT(IPPORT_BOOTPC), 0);
   RTCSPCB_append_fragment(pcb_ptr, dhcp_ptr->CURRENT_PACKET_SIZE, dhcp_ptr->PACKET);

   if ( dhcp_ptr->BOUND ) {
      /* We already have an IP address available */
      UDP_send_internal(BOOT_port,
                        dhcp_ptr->NEW_DATA.ClientIPaddress,
                        dhcp_ptr->NEW_DATA.ServerIp,
                        DHCP_SERVER_PORT,
                        pcb_ptr,
                        0);
   } else {
      /* Send the datagram */
      BOOT_send(pcb_ptr, dhcp_ptr->HANDLE);
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_rebind_lease
* Returned Values : boolean
* Comments        :
*     Attempts to renew the current lease unicasting to the server.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean DHCPCLNT_rebind_lease
   (
   TCPIP_EVENT_PTR   event
   /* [IN/OUT] the renew event */
   )
{ /* Body */
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr = event->PRIVATE;
   uint_32                rebind_time;
   boolean                reset_event = FALSE;

   if ( dhcp_ptr->R_EVENTS_TIME_LEFT ) {
      DHCPCLNT_TIME_OVERFLOW_CHECK(rebind_time, dhcp_ptr->R_EVENTS_TIME_LEFT)
      dhcp_ptr->RENEW.TIME = rebind_time;
      reset_event = TRUE;
   } else {
      TCPIP_Event_cancel(&dhcp_ptr->RESEND);
      /*
      ** Need to make sure that the renew time is now greater than the rebind
      ** for use by DHCPCLNT_send to know the previous state if requests fail.
      */
      dhcp_ptr->RENEW_TIME = dhcp_ptr->DATA.LEASE;
      dhcp_ptr->STATE = DHCP_REBINDING;
      DHCPCLNT_modify_options( dhcp_ptr );
      DHCPCLNT_open( dhcp_ptr );
   } /* Endif */

   return( reset_event );
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_renew_lease
* Returned Values : boolean
* Comments        :
*     Attempts to renew the current lease unicasting to the server.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean DHCPCLNT_renew_lease
   (
   TCPIP_EVENT_PTR   event
   /* [IN/OUT] the renew event */
   )
{ /* Body */
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr = event->PRIVATE;
   uint_32                renew_time;

   if (!dhcp_ptr->R_EVENTS_TIME_LEFT ) {
      dhcp_ptr->STATE = DHCP_RENEWING;
      DHCPCLNT_modify_options( dhcp_ptr );

      dhcp_ptr->INIT_TIMEOUT = 0;

      DHCPCLNT_open( dhcp_ptr );

      /* Set up for DHCP Rebinding */
      dhcp_ptr->R_EVENTS_TIME_LEFT = dhcp_ptr->REBIND_TIME;
      dhcp_ptr->RENEW.EVENT = DHCPCLNT_rebind_lease;
   } /* Endif */

   DHCPCLNT_TIME_OVERFLOW_CHECK(renew_time, dhcp_ptr->R_EVENTS_TIME_LEFT);
   dhcp_ptr->RENEW_TIME = 0;

   dhcp_ptr->RENEW.TIME = renew_time;

   return( TRUE );
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_terminate_lease
* Returned Values : boolean
* Comments        :
*     If a lease expires, or a DHCPNAK message is received, this function
*     will broadcast another DHCPDISCOVER message.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean DHCPCLNT_terminate_lease
   (
   TCPIP_EVENT_PTR   event
   /* [IN/OUT] the termination event */
   )
{  /* Body */
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr = event->PRIVATE;
   uint_32                terminate_time;
   boolean                retry = FALSE;
   boolean                reset_event = FALSE;

   if ( dhcp_ptr->TERMINATE_TIME ) {
      DHCPCLNT_TIME_OVERFLOW_CHECK(terminate_time, dhcp_ptr->TERMINATE_TIME)
      dhcp_ptr->TERMINATE.TIME = terminate_time;
      reset_event = TRUE;
   } else {
      if (dhcp_ptr->BOUND) {
         RTCSCMD_internal( dhcp_ptr->PARMS_BIND, IPIF_unbind );
         if ( dhcp_ptr->UNBIND_FUNC != NULL ) {
            retry = dhcp_ptr->UNBIND_FUNC((_rtcs_if_handle)dhcp_ptr->HANDLE);
         } /* Endif */
      } else {
         if ( dhcp_ptr->FAILURE_FUNC != NULL ) {
            dhcp_ptr->FAILURE_FUNC((_rtcs_if_handle)dhcp_ptr->HANDLE);
         } /* Endif */   
      } /* Endif */

      TCPIP_Event_cancel(&dhcp_ptr->RENEW);
      TCPIP_Event_cancel(&dhcp_ptr->RESEND);

      if ( retry ) {
         dhcp_ptr->REBIND_TIME = 0;
         dhcp_ptr->BOUND = FALSE;
         dhcp_ptr->STATE = DHCP_INITIALIZE;
         DHCPCLNT_modify_options( dhcp_ptr );
         DHCPCLNT_open( dhcp_ptr );
      } else  {
         BOOT_close();
         dhcp_ptr->HANDLE->BOOT = NULL;
         /* Free the DHCP Structure */
         _mem_free(dhcp_ptr);
      }/* Endif */
   } /* Endif */

   return( reset_event );
}  /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_bind_attempt_timeout
* Returned Values : boolean
* Comments        :
*     This function will timeout a lease attempt if the binding is not
*     successful.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean DHCPCLNT_bind_attempt_timeout
   (
   TCPIP_EVENT_PTR   event
   /* [IN/OUT] the termination event */
   )
{  /* Body */
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr = event->PRIVATE;
   boolean                retry = FALSE;

   if ( dhcp_ptr->FAILURE_FUNC != NULL ) {
      retry = dhcp_ptr->FAILURE_FUNC((_rtcs_if_handle)dhcp_ptr->HANDLE);
   } /* Endif */

   TCPIP_Event_cancel(&dhcp_ptr->RESEND);

   if ( retry ) {
      dhcp_ptr->REBIND_TIME = 0;
      dhcp_ptr->BOUND = FALSE;
      dhcp_ptr->STATE = DHCP_INITIALIZE;
      DHCPCLNT_modify_options( dhcp_ptr );
      DHCPCLNT_open( dhcp_ptr );
   } else  {
      BOOT_close();
      dhcp_ptr->HANDLE->BOOT = NULL;
      /* Free the DHCP Structure */
      _mem_free(dhcp_ptr);
   }/* Endif */

   return FALSE;
}  /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_decline
* Returned Values : void
* Comments        :
*     Declines a lease, sends a message to the DCHP Server.
*     Add error code message later.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_decline
   (
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr,
   /* [IN/OUT] This is the DHCP Client control structure */
   uint_32     error
   /* [IN] The reason for rejection */
   )
{  /* Body */
    uint_32     old_state = dhcp_ptr->STATE;

    dhcp_ptr->STATE = DHCP_DECLINING;
    DHCPCLNT_modify_options( dhcp_ptr );

    DHCPCLNT_send_one_shot( dhcp_ptr );
    dhcp_ptr->STATS.ST_TX_DECLINE++;
    dhcp_ptr->STATE = old_state;
    DHCPCLNT_modify_options( dhcp_ptr );

}  /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_release_internal
* Returned Values : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

void DHCPCLNT_release_internal
   (
      TCPIP_PARM_IF_DHCP_RELEASE _PTR_ parms
         /* [IN/OUT] This is the DHCP Client structure from the application */
   )
{  /* Body */
   IP_IF_PTR              if_ptr = (IP_IF_PTR)parms->HANDLE;
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr;

   if ( if_ptr->BOOT != NULL ) {
      dhcp_ptr = (DHCP_CLNT_STRUCT _PTR_)if_ptr->BOOT;

      TCPIP_Event_cancel(&dhcp_ptr->RENEW);
      TCPIP_Event_cancel(&dhcp_ptr->RESEND);
      TCPIP_Event_cancel(&dhcp_ptr->TERMINATE);

      dhcp_ptr->STATE = DHCP_RELEASING;
      DHCPCLNT_modify_options( dhcp_ptr );

      DHCPCLNT_send_one_shot( dhcp_ptr );
      if ( dhcp_ptr->BOUND ) {
         RTCSCMD_internal( dhcp_ptr->PARMS_BIND, IPIF_unbind );
      }/* Endif */
      BOOT_close();
      /* Free the DHCP Structure */
      _mem_free(dhcp_ptr);
      if_ptr->BOOT = NULL;
   }/* Endif */

   RTCSCMD_complete(parms, RTCS_OK);
}  /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_parse_offer
* Returned Values : void
* Comments        : We only look for the options that affect us directly.
*                   It is assumed that the application will parse any
*                   site specific options.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_parse_offer
   (
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr,
   /* [IN/OUT] This is the DHCP Client control structure */
   RTCSPCB_PTR pcb_ptr
   /*[IN] The offer we're accepting */
   )
{ /* Body */
   DHCP_HEADER _PTR_ header_ptr;
   uchar       _PTR_ opt;
   uint_32           len;
   uint_32           optlen;
   uchar             optval;

   dhcp_ptr->NEW_RENEW_TIME = 0;
   dhcp_ptr->NEW_REBIND_TIME = 0;
   /* Make sure we have a server ID */
   header_ptr = (DHCP_HEADER _PTR_)RTCSPCB_DATA(pcb_ptr);

   /* Get our IP address, and pick the default netmask */
   dhcp_ptr->NEW_PARMS_BIND.address = ntohl(header_ptr->YIADDR);
   dhcp_ptr->NEW_DATA.ClientIPaddress = dhcp_ptr->NEW_PARMS_BIND.address;
   dhcp_ptr->NEW_PARMS_BIND.locmask = 0xFFFFFFFFL;
   dhcp_ptr->NEW_PARMS_BIND.netmask =
      IN_DEFAULT_NET(dhcp_ptr->NEW_PARMS_BIND.address);

   /* Parse the options field */

   opt = (uchar _PTR_)header_ptr + sizeof(DHCP_HEADER) + 4;
   len = RTCSPCB_SIZE(pcb_ptr) - (sizeof(DHCP_HEADER) + 4);

#define DHCP_NEXTOPT    opt += optlen; \
                        break

   while (len) {

      /* Get the next option code */
      optval = ntohc(opt);
      opt++;
      len--;

      /* Interpret the pad and end options */
      if (optval == DHCPOPT_END) break;
      if (optval == DHCPOPT_PAD) continue;

      /* All other codes have a length byte */
      if (len == 0) break;
      optlen = ntohc(opt);
      opt++;
      len--;
      if (len < optlen) break;
      len -= optlen;

      switch (optval) {

      case DHCPOPT_LEASE:
         if (optlen != 4) {DHCP_NEXTOPT;}
         dhcp_ptr->NEW_DATA.LEASE = ntohl(opt);
         opt += 4;
         break;

      case DHCPOPT_MASK:
         if (optlen != 4) {DHCP_NEXTOPT;}
         dhcp_ptr->NEW_PARMS_BIND.netmask = ntohl(opt);
         dhcp_ptr->NEW_DATA.Netmask = dhcp_ptr->NEW_PARMS_BIND.netmask;
         opt += 4;
         break;

      case DHCPOPT_SERVERNAME:

         _mem_copy(opt, dhcp_ptr->NEW_DATA.SNAME, optlen);
         opt+=optlen;
         break;

      case DHCPOPT_FILENAME:

         _mem_copy(opt, dhcp_ptr->NEW_DATA.BOOTFILE, optlen);
         opt+=optlen;
         break;

      case DHCPOPT_RENEWALL:
         if ( optlen != DHCPSIZE_LEASE ) {
            DHCP_NEXTOPT;
         } /* Endif */
         dhcp_ptr->NEW_RENEW_TIME = ntohl(opt);
         opt += DHCPSIZE_LEASE;
         break;

      case DHCPOPT_REBINDING:
         if ( optlen != DHCPSIZE_LEASE ) {
            DHCP_NEXTOPT;
         } /* Endif */
         dhcp_ptr->NEW_REBIND_TIME = ntohl(opt);
         opt += DHCPSIZE_LEASE;
         break;

      default:
         DHCP_NEXTOPT;
      } /* Endswitch */

   } /* Endwhile */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_copy_binding
* Returned Values : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_copy_binding
   (
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr
   /* [IN/OUT] This is the DHCP Client control structure */
   )
{  /* Body */
   dhcp_ptr->PARMS_BIND.address = dhcp_ptr->NEW_PARMS_BIND.address;
   dhcp_ptr->PARMS_BIND.locmask = dhcp_ptr->NEW_PARMS_BIND.locmask;
   dhcp_ptr->PARMS_BIND.netmask = dhcp_ptr->NEW_PARMS_BIND.netmask;
   dhcp_ptr->DATA.ServerIp = dhcp_ptr->NEW_DATA.ServerIp;
   dhcp_ptr->DATA.ClientIPaddress = dhcp_ptr->NEW_DATA.ClientIPaddress;
   dhcp_ptr->DATA.LEASE = dhcp_ptr->NEW_DATA.LEASE;
   dhcp_ptr->DATA.Netmask = dhcp_ptr->NEW_DATA.Netmask;
   dhcp_ptr->DATA.SADDR   = dhcp_ptr->NEW_DATA.SADDR;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_verify_packet
* Returned Values : uchar
* Comments        :
*     Verifies the validity of an incoming DHCP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uchar DHCPCLNT_verify_packet
 (
 DHCP_CLNT_STRUCT _PTR_ dhcp_ptr,
 RTCSPCB_PTR            pcb_ptr,
 uchar            _PTR_ bootreply,
 uint_32          _PTR_ packet_size,
 _ip_address      _PTR_ temp_addr,
 _ip_address      _PTR_ current_ip,
 uint_32          _PTR_ error
 )

{ /* Body */
   DHCP_HEADER      _PTR_ send_header_ptr;
   DHCP_HEADER      _PTR_ header_ptr;
   uchar            _PTR_ opt;
   uint_32                len;
   uchar            _PTR_ packet_option;

   /* Make sure the datagram is large enough for the magic cookie */
   if (RTCSPCB_SIZE(pcb_ptr) < sizeof(DHCP_HEADER) + 4) {
      *error = RTCSERR_DHCPCLNT_PACKET_SIZE_ERROR;
      return 0;
   } /* Endif */

   send_header_ptr = (DHCP_HEADER _PTR_)dhcp_ptr->PACKET;
   header_ptr = (DHCP_HEADER _PTR_)bootreply;
   /* Make sure the XID matches */
   if (ntohl(header_ptr->XID) != ntohl(send_header_ptr->XID)) {
      *error = RTCSERR_DHCPCLNT_XID_MISMATCH;
      return 0;
   } /* Endif */

   /* Parse the options field to find the message type */
   opt = bootreply + sizeof(DHCP_HEADER);
   *packet_size = RTCSPCB_SIZE(pcb_ptr);
   len = *packet_size - sizeof(DHCP_HEADER);
   if (ntohl(opt) != DHCP_MAGIC) {
      *error = RTCSERR_DHCP_PACKET_ERROR;
      return 0;
   } /* Endif */
   opt += 4;
   len -= 4;
   /* These are to avoid problems in DHCPCLNT_modify_options if we decline */
   *temp_addr = dhcp_ptr->NEW_DATA.ServerIp;
   dhcp_ptr->NEW_DATA.ServerIp = IP_source(pcb_ptr);
   dhcp_ptr->NEW_DATA.ClientIPaddress = ntohl(header_ptr->YIADDR);

   packet_option = DHCP_find_option(opt, len, DHCPOPT_SERVERID);
   if ( !packet_option || ntohc(packet_option + 1) != 4 ) {
      /* Server id option is required in all DHCP responses */
      DHCPCLNT_decline( dhcp_ptr, RTCSERR_DHCP_SERVER_OPTION_MISSING );
      dhcp_ptr->NEW_DATA.ServerIp = *temp_addr;
      *error = RTCSERR_DHCPCLNT_ERROR_DECLINED;
      return 0;
   } else  {
      packet_option += 2;
      *current_ip = ntohl(packet_option);
   } /* Endif */

   packet_option = DHCP_find_option(opt, len, DHCPOPT_MSGTYPE);
   if ( !packet_option || ntohc(packet_option + 1) != 1 ) {
      /* Message type option is required in all DHCP packets */
      DHCPCLNT_decline( dhcp_ptr, RTCSERR_DHCP_MESSAGE_OPTION_MISSING);
      dhcp_ptr->NEW_DATA.ServerIp = *temp_addr;
      *error = RTCSERR_DHCPCLNT_ERROR_DECLINED;
      return 0;
   } /* Endif */
   packet_option += 2;
   *error = RTCS_OK;
   return(ntohc(packet_option));
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_service_offer
* Returned Values : void
* Comments        :
*     Services an incoming DHCP offer packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_service_offer
 (
 DHCP_CLNT_STRUCT _PTR_ dhcp_ptr,
 RTCSPCB_PTR            pcb_ptr,
 uchar            _PTR_ bootreply,
 uint_32                packet_size,
 _ip_address            temp_addr,
 _ip_address            current_ip
  )

{  /* Body */

   dhcp_ptr->STATS.ST_RX_OFFER++;
   if ( dhcp_ptr->STATE == DHCP_INITIALIZE ) {
      if ( dhcp_ptr->CHOICE_FUNC != NULL ) {
         if (dhcp_ptr->CHOICE_FUNC(bootreply, packet_size) < 0) {
            /* It's been rejected */
            dhcp_ptr->NEW_DATA.ServerIp = temp_addr;
            return;
         }/* Endif */
      }/* Endif */
      
      /* start the probe - this needs to complete before we service the dhACK */
      if (dhcp_ptr->FLAGS & DHCP_SEND_PROBE) {
         DHCPCLNT_probe_address( dhcp_ptr->NEW_DATA.ClientIPaddress,  dhcp_ptr->HANDLE);
      }
      /* We're accepting this offer */
      TCPIP_Event_cancel(&dhcp_ptr->RESEND);
      dhcp_ptr->NEW_DATA.ServerIp = current_ip;
      DHCPCLNT_parse_offer( dhcp_ptr, pcb_ptr );
      dhcp_ptr->STATE = DHCP_REQUESTING;
      DHCPCLNT_modify_options( dhcp_ptr );
      dhcp_ptr->RETRY_TIMEOUT = DHCP_TIMEOUT_MIN;
      
      /* 
      ** put a bit of delay in before we send the REQEST. This is to give the
      ** probe (started above) a chance to compete.
      */
      dhcp_ptr->RESEND.TIME = 50;  /* 50 miliseconds */
      TCPIP_Event_add( &dhcp_ptr->RESEND );
   }/* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_set_timed_events
* Returned Values : void
* Comments        :
*     Sets the renewall, rebinding, and termination times.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_set_timed_events
(
DHCP_CLNT_STRUCT _PTR_ dhcp_ptr
)


{  /* Body */
   uint_32                random;
   uint_32                renewall_range;
   uint_32                renewall_time;
   uint_32                terminate_time;

   /* Set or reset the renewall event */
   if ( dhcp_ptr->NEW_RENEW_TIME ) {
      dhcp_ptr->RENEW_TIME = dhcp_ptr->NEW_RENEW_TIME;
   } else {
      /*
      ** The renewall time wasn't specified by the DHCP server,
      ** we need to set it ourselves.
      */
      DHCP_SET_RENEWALL_RANGE(dhcp_ptr->DATA.LEASE, renewall_range);
      random = RTCS_rand() % renewall_range;
      dhcp_ptr->RENEW_TIME =
         DHCP_RENEWALL_BIAS( dhcp_ptr->DATA.LEASE ) + random;
   } /* Endif */

   /* Set or reset rebinding event */
   if ( dhcp_ptr->NEW_REBIND_TIME ) {
      dhcp_ptr->REBIND_TIME = dhcp_ptr->NEW_REBIND_TIME
         - dhcp_ptr->RENEW_TIME;
   } else {
      /*
      ** The rebinding time wasn't specified by the DHCP server,
      ** we need to set it ourselves.
      */
      DHCP_SET_RENEWALL_RANGE(dhcp_ptr->DATA.LEASE, renewall_range);
      random = RTCS_rand() % renewall_range;
      dhcp_ptr->REBIND_TIME =
         (DHCP_REBINDING_BIAS(dhcp_ptr->DATA.LEASE) + random)
         - dhcp_ptr->RENEW_TIME;
   }/* Endif */

   dhcp_ptr->R_EVENTS_TIME_LEFT = dhcp_ptr->RENEW_TIME;
   DHCPCLNT_TIME_OVERFLOW_CHECK(renewall_time, dhcp_ptr->R_EVENTS_TIME_LEFT)
   dhcp_ptr->RENEW.TIME = renewall_time;
   dhcp_ptr->RENEW.EVENT = DHCPCLNT_renew_lease;
   dhcp_ptr->TERMINATE_TIME = dhcp_ptr->DATA.LEASE;

   DHCPCLNT_TIME_OVERFLOW_CHECK(terminate_time, dhcp_ptr->TERMINATE_TIME)
   dhcp_ptr->TERMINATE.TIME = terminate_time;
}  /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_service_ack
* Returned Values : void
* Comments        :
*     Services an incoming DHCP ack packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_service_ack
(
DHCP_CLNT_STRUCT _PTR_ dhcp_ptr,
RTCSPCB_PTR            pcb_ptr,
uchar            _PTR_ bootreply,
uint_32                packet_size,
IP_IF_PTR              if_ptr
)

{  /* Body */
   DHCP_HEADER      _PTR_ header_ptr;
   boolean                renew_again = TRUE;

   dhcp_ptr->STATS.ST_RX_ACK++;
   if ( dhcp_ptr->STATE == DHCP_INFORMED ) {
      /* Give the application the info and self destruct */
      TCPIP_Event_cancel(&dhcp_ptr->RESEND);
      if ( dhcp_ptr->BIND_FUNC != NULL ) {
         dhcp_ptr->BIND_FUNC(bootreply, packet_size, (_rtcs_if_handle)if_ptr);
      } /* Endif */
      BOOT_close();
      dhcp_ptr->HANDLE->BOOT = NULL;
      /* Free the DHCP Structure */
      _mem_free(dhcp_ptr);
      return;
   } /* Endif */

   header_ptr = (DHCP_HEADER _PTR_)bootreply;
   /* OK, we've got a valid lease */
   dhcp_ptr->DATA.SADDR = ntohl(header_ptr->SIADDR);
   dhcp_ptr->NEW_DATA.ClientIPaddress = ntohl(header_ptr->YIADDR);

   TCPIP_Event_cancel(&dhcp_ptr->TERMINATE);
   DHCPCLNT_parse_offer( dhcp_ptr, pcb_ptr );
   if ( dhcp_ptr->BOUND ) {
      if (dhcp_ptr->NEW_DATA.LEASE < dhcp_ptr->DATA.LEASE) {
         /* Our request was refused, wait for termination to bind again */
         renew_again = FALSE;
      }/* Endif */
      dhcp_ptr->DATA.LEASE = dhcp_ptr->NEW_DATA.LEASE;
      /* Cancel renewall and lease expiry events */
      TCPIP_Event_cancel(&dhcp_ptr->RENEW);
   } else {
      /* Bind the received IP address to this interface */
      DHCPCLNT_copy_binding( dhcp_ptr );
      RTCSCMD_internal( dhcp_ptr->PARMS_BIND, IPIF_bind );
      dhcp_ptr->TERMINATE.EVENT = DHCPCLNT_terminate_lease;
   } /* Endif */

   if ( dhcp_ptr->DATA.LEASE != DHCP_LEASE_INFINITE ) {
      DHCPCLNT_set_timed_events(dhcp_ptr);
   } /* Endif */

   TCPIP_Event_cancel(&dhcp_ptr->RESEND);
   dhcp_ptr->BOUND = TRUE;
   if ( DHCPCLNT_verify_address( dhcp_ptr )) {
      /* Address is in use by another host, decline and restart */
      DHCPCLNT_decline( dhcp_ptr, RTCSERR_DHCP_ADDR_IN_USE);
      RTCSCMD_internal( dhcp_ptr->PARMS_BIND, IPIF_unbind );
      dhcp_ptr->BOUND = FALSE;
      dhcp_ptr->RENEW_TIME = 0;
      dhcp_ptr->STATE = DHCP_INITIALIZE;
      DHCPCLNT_modify_options( dhcp_ptr );
      DHCPCLNT_open( dhcp_ptr );
   } else  {
      dhcp_ptr->STATE = DHCP_BOUND;
      if ( dhcp_ptr->BIND_FUNC != NULL ) {
         dhcp_ptr->BIND_FUNC(bootreply, packet_size,
            (_rtcs_if_handle)if_ptr);
      }/* Endif */

      if (dhcp_ptr->DATA.LEASE == DHCP_LEASE_INFINITE) {
         if(!(dhcp_ptr->FLAGS & DHCP_MAINTAIN_STATE_ON_INFINITE_LEASE)) {
            BOOT_close();
            dhcp_ptr->HANDLE->BOOT = NULL;
            /* Free the DHCP Structure */
            _mem_free(dhcp_ptr);
         } /* Endif */
      } else  {
         if (renew_again) {
            TCPIP_Event_add(&dhcp_ptr->RENEW);
         }/* Endif */
         TCPIP_Event_add(&dhcp_ptr->TERMINATE);
      } /* Endif */
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_service_nak
* Returned Values : void
* Comments        :
*     Services an incoming DHCP NAK packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void DHCPCLNT_service_nak
(
DHCP_CLNT_STRUCT _PTR_ dhcp_ptr,
uchar            _PTR_ bootreply,
uint_32                packet_size
)
{ /* Body */
   boolean retry = FALSE;

   /* Our request was refused. Start request over */
   dhcp_ptr->STATS.ST_RX_NAK++;

   if ( dhcp_ptr->NAK_FUNC != NULL )
   {
      retry = dhcp_ptr->NAK_FUNC(bootreply, packet_size, (_rtcs_if_handle)dhcp_ptr->HANDLE);
   } /* Endif */

   if ( retry ) {
      dhcp_ptr->STATE = DHCP_REQUESTING;
      DHCPCLNT_modify_options( dhcp_ptr ); 
   } else {
      dhcp_ptr->RENEW_TIME = 0;
      dhcp_ptr->TERMINATE_TIME = 0;

      TCPIP_Event_cancel(&dhcp_ptr->TERMINATE);
      DHCPCLNT_terminate_lease(&dhcp_ptr->TERMINATE);
   } /* Endif */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_service
* Returned Values : void
* Comments        :
*     Services an incoming DHCP packet.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

void DHCPCLNT_service
   (
      RTCSPCB_PTR    pcb_ptr,     /* [IN] DHCP packet */
      UCB_STRUCT_PTR ucb_ptr      /* [IN]     target UCB       */
   )
{ /* Body */
   IP_IF_PTR              if_ptr   = (IP_IF_PTR)pcb_ptr->IFSRC;
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr = (DHCP_CLNT_STRUCT _PTR_)if_ptr->BOOT;
   uchar            _PTR_ bootreply;
   uchar                  msgtype;
   uint_32                packet_size;
   uint_32                error;
   _ip_address            current_ip;
   _ip_address            temp_addr;

   RTCSLOG_PCB_READ(pcb_ptr, RTCS_LOGCTRL_PORT(IPPORT_BOOTPC), 0);

   if ((dhcp_ptr != NULL) && (dhcp_ptr->STATE != DHCP_BOUND)) {
      bootreply = RTCSPCB_DATA(pcb_ptr);
      msgtype = DHCPCLNT_verify_packet(dhcp_ptr, pcb_ptr, bootreply,
         &packet_size, &temp_addr, &current_ip, &error);
      if (error) {
         RTCSLOG_PCB_FREE(pcb_ptr, error);
         RTCSPCB_free(pcb_ptr);
         return;
      } /* Endif */
      if ( msgtype == DHCPTYPE_DHCPOFFER ) {
         DHCPCLNT_service_offer(dhcp_ptr, pcb_ptr, bootreply, packet_size,
            temp_addr, current_ip);
      } else {
         /* It's an ACK or a NAK message */
         if ( dhcp_ptr->STATE != DHCP_INITIALIZE ) {
            if ( msgtype == DHCPTYPE_DHCPACK ) {
               DHCPCLNT_service_ack(dhcp_ptr, pcb_ptr, bootreply, packet_size,
                  if_ptr);
            } else if (msgtype == DHCPTYPE_DHCPNAK ) {
               DHCPCLNT_service_nak(dhcp_ptr, bootreply, packet_size);
            } /* Endif */
         } /* Endif */
      } /* Endif */
   } /* Endif */

   RTCSLOG_PCB_FREE(pcb_ptr, RTCS_OK);
   RTCSPCB_free(pcb_ptr);
} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_verify_address
* Returned Values : boolean, TRUE if the address is in use by another host,
*                   FALSE otherwise
* Comments        :
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean DHCPCLNT_verify_address
   (
   DHCP_CLNT_STRUCT _PTR_ dhcp_ptr
   /* [IN] This is the DHCP Client control structure */
   )
{  /* Body */
   _ip_address      new_ip = dhcp_ptr->DATA.ClientIPaddress;
   IP_IF_PTR        if_ptr = dhcp_ptr->HANDLE;
   
   if (! dhcp_ptr->FLAGS & DHCP_SEND_PROBE) {
      /* we are configured to not probe, so don't try to verify */
      return FALSE;
   } 
   
   /* note: an arp probe was sent out during DHCPCLNT_service_offer.
      we expect that if an arp response was comming, it arrived by
      now.
    */
    
   return ARP_is_complete (if_ptr, new_ip);
}  /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_probe_address
* Returned Values : RTCS_OK meanse that probe was sent out the interface. 
* Parameters      :
*
*     _ip_address       probe_addr    [IN] IP address to probe the network for
*     pointer     :     src_interface [IN] interface we will send the probe from
* 
* Comments        :
*     Send out an arp reuest to ipaddress from interface. An arp reply will
*     be noted by the arp system on this interface and may be referenced later.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

uint_32 DHCPCLNT_probe_address
   (
      _ip_address       probe_addr,
      pointer           src_interface
   )
{ /* Body */

    return( ARP_request(src_interface, probe_addr, probe_addr ));

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */
/* EOF */
