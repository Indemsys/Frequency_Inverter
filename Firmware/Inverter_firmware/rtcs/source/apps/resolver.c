/**HEADER*******************************************************************
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
* $FileName: resolver.c$
* $Version : 3.8.28.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the Domain Name System resolver
*   functions for RTCS.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_DNS | RTCSCFG_ENABLE_LWDNS

#include <ctype.h>
#include <string.h>
#include "dns.h"

#endif

#if RTCSCFG_ENABLE_DNS

/*

GCC compiler did not have ctype in libc.
Used these definitions to resolve the problem:

#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c)-'A'+'a' : (c))
#define isalpha(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define isdigit(c) ( (c) >= '0' && (c) <= '9' )
#define isalnum(c) (isalpha(c) || isdigit(c))

*/


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_get_query_id()
* Returned Value  : uint_16.
* Comments        : The function returns a unique value that
*                   is used by the resolver for queries and keeps track
*                   of the last query id used.
*
*END*-----------------------------------------------------------------*/

static uint_16 DNS_get_query_id
   (
   void
   )

{ /* Body */

   static uint_16 current_query_id = 0;

   /* The masking is required for TI machines to operate correctly */
   if ( current_query_id == 0 ) {
      current_query_id = RTCS_rand() & DNS_UINT_16_MASK;
      if ( current_query_id == 0 ) {
         current_query_id++;
      } /* Endif */
   } else  {
      current_query_id++;
      current_query_id &= DNS_UINT_16_MASK;
      if ( current_query_id == 0 ) {
         current_query_id++;
      } /* Endif */
   } /* Endif */
   return( current_query_id );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_update_TTL()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/


static void DNS_update_TTL
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr
   )

{ /* Body */

   TIME_STRUCT       time;
   int_32            difference;

   _time_get( &time );

   difference = time.SECONDS - query_ptr->LAST_TTL_CHECK;

   if ( query_ptr->TTL <= difference ) {
      query_ptr->TTL = 0;
   } else {
      query_ptr->TTL -= difference;
   } /* Endif */

   query_ptr->LAST_TTL_CHECK = time.SECONDS;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_free_internal_query()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void DNS_free_internal_query
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_  query
   )

{ /* Body */

   DNS_CNAME_STRUCT _PTR_    free_cname_ptr;
   DNS_CNAME_STRUCT _PTR_    next_cname_ptr;

   if ( query->QNAME_PTR != NULL ) {
      _mem_free( query->QNAME_PTR );
   } /* Endif */

   free_cname_ptr = query->CNAME;
   while ( free_cname_ptr != NULL ) {
      next_cname_ptr = free_cname_ptr->NEXT;
      _mem_free( free_cname_ptr->CNAME_PTR );
      RTCS_part_free( free_cname_ptr );
      free_cname_ptr = next_cname_ptr;
   } /* Endwhile */

   RTCS_part_free( query );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_Send_UDP_Query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_Send_UDP_Query
   (
   DNS_CONTROL_STRUCT         _PTR_  control_ptr,
   DNS_INTERNAL_QUERY_STRUCT _PTR_  query_ptr,
   boolean                          cname_retry
   )

{ /* Body */

   DNS_MESSAGE_HEADER_STRUCT _PTR_   message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT   _PTR_   message_tail_ptr;
   DNS_CNAME_STRUCT          _PTR_   cname_ptr;
   sockaddr_in                       raddr;
   int_32                            error;
   uint_32                           buffer_size;
   uint_32                           name_size;
   uchar                     _PTR_   name_ptr;
   uchar                     _PTR_   temp_ptr;

   /* Remote address, Name Servers use port 53 */
   raddr.sin_family      = AF_INET;
   raddr.sin_port        = DNS_NAMESERVER_PORT;
   raddr.sin_addr.s_addr = query_ptr->SERVER_IPADDR;

   /* set up buffer for sending query.   */
   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)control_ptr->RES_BUFFER;

   htons(message_head_ptr->ID, query_ptr->QUERY_ID);
   htons(message_head_ptr->CONTROL, query_ptr->CONTROL);
   htons(message_head_ptr->QDCOUNT, DNS_SINGLE_QUERY);
   htons(message_head_ptr->ANCOUNT, 0);
   htons(message_head_ptr->NSCOUNT, 0);
   htons(message_head_ptr->ARCOUNT, 0);

   name_ptr  = control_ptr->RES_BUFFER + sizeof(DNS_MESSAGE_HEADER_STRUCT);
   if ( cname_retry == TRUE ) {
      cname_ptr = query_ptr->CNAME;
      while ( cname_ptr->NEXT != NULL ) {
         cname_ptr = cname_ptr->NEXT;
      } /* Endwhile */
      name_size = strlen((char _PTR_)cname_ptr->CNAME_PTR );
      temp_ptr  = cname_ptr->CNAME_PTR;
   } else {
      name_size = strlen((char _PTR_)query_ptr->QNAME_PTR );
      temp_ptr  = query_ptr->QNAME_PTR;
   } /* Endif */

   /* To include the terminating NULL char */
   name_size++;
   buffer_size = sizeof(DNS_MESSAGE_HEADER_STRUCT) +  name_size
                 + sizeof(DNS_MESSAGE_TAIL_STRUCT);

   while( name_size != 0 ) {
     htonc(name_ptr, *temp_ptr);
     name_ptr++;
     temp_ptr++;
     name_size--;
   } /* Endwhile */

   message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT _PTR_)name_ptr;
   htons(message_tail_ptr->QTYPE, query_ptr->QTYPE);
   htons(message_tail_ptr->QCLASS, query_ptr->QCLASS);


   error = sendto(control_ptr->RES_SOCKET, control_ptr->RES_BUFFER, buffer_size, 0,
                  &raddr, sizeof(raddr));
   if (error == RTCS_ERROR) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_SEND_QUERY, 1, 0, 0);
      error = RTCSERR_DNS_UNABLE_TO_SEND_QUERY;
   } else {
      error = DNS_OK;
   } /* Endif */

   return( error );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_return_UDP_response_to_query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_return_UDP_response_to_query
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_ current_query_ptr,
   uchar _PTR_                     buffer_ptr,
   uint_32                         buffer_size,
   uint_32                         query_sock
   )

{ /* Body */

   int_32        error;

   error = sendto(query_sock, buffer_ptr, buffer_size, 0,
                  &current_query_ptr->CLIENT_SIN,
                  sizeof(current_query_ptr->CLIENT_SIN) );
   if (error == RTCS_ERROR) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_SEND_QUERY, 2, 0, 0);

      DNS_free_internal_query( current_query_ptr );
      return( RTCSERR_DNS_UNABLE_TO_SEND_QUERY );
   } /* Endif */

   DNS_free_internal_query( current_query_ptr );
   return( DNS_OK );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_free_resource_record()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/


static void DNS_free_resource_record
   (
   DNS_RR_HEADER_STRUCT _PTR_ list_ptr
   )

{ /* Body */

   DNS_ADDR_RECORD_STRUCT _PTR_ addr_rr_ptr;
   DNS_NAME_RECORD_STRUCT _PTR_ name_rr_ptr;

   switch ( list_ptr->TYPE ) {
      case DNS_A:
         addr_rr_ptr = (DNS_ADDR_RECORD_STRUCT _PTR_)((void _PTR_)list_ptr);
         RTCS_part_free( addr_rr_ptr );
         break;

      case DNS_PTR:
      case DNS_CNAME:
      case DNS_NS:
         name_rr_ptr = (DNS_NAME_RECORD_STRUCT _PTR_)((void _PTR_)list_ptr);
         if ( name_rr_ptr->ANS_NAME_PTR ) {
            _mem_free( name_rr_ptr->ANS_NAME_PTR );
         }/* Endif */
         RTCS_part_free( name_rr_ptr );
         break;

      default:
         /*
         ** If we reach here, there could be a potential memory leak.
         ** Depending upon the Resource Record type, it may not all
         ** be freed.
         */
         RTCS_part_free( list_ptr );
   } /* Endswitch */

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_free_slist_records()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/


static void DNS_free_slist_records
   (
   DNS_SLIST_STRUCT _PTR_ list_ptr
   )

{ /* Body */

/* NOT IMPLEMENTED */
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_get_best_name_server_for_query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

/* Start CR 2424
   NOTE: This only works if DNS entries are added correctly into the linked
   list, with the last entry's NEXT == NULL.  This should be done using the
   API for adding DNS entries (CR 2257):
        {
          DNS_SLIST_STRUCT tmp_slist_entry;
          DNS_init_slist_entry(&tmp_slist_entry);
          return( DNS_add_slist_entry(&tmp_slist_entry));
        }
*/
#define PRIMARY_NAME_SERVER_TIMEOUT 300 // 5 minutes

static uint_32 DNS_get_best_name_server_for_query
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr,
   DNS_CONTROL_STRUCT         _PTR_ control_ptr
   )

{ /* Body */

   static DNS_SLIST_STRUCT _PTR_ best_ptr = NULL;
   TIME_STRUCT                   time;
   static uint_32                last_time;

   // Start with the Primary DNS Server
   if (best_ptr == NULL) {
      best_ptr = control_ptr->LOCAL_SLIST_PTR;
      query_ptr->SERVER_IPADDR = best_ptr->IPADDR;
      return( DNS_OK );
   }

   // Only change our notion of best server if this query has exhausted
   // its QUERY_RETRIES on our current best server.  If the query expired
   // on some other server, we cannot use that info to change our current
   // notion of best server, since our current best server may be fine.
   if (query_ptr->SERVER_IPADDR == best_ptr->IPADDR) {
      _time_get( &time );

      // Go back to the Primary every so often to see if it has returned.
      if (best_ptr != control_ptr->LOCAL_SLIST_PTR) {
         if ((time.SECONDS - last_time) >= PRIMARY_NAME_SERVER_TIMEOUT) {
            best_ptr = control_ptr->LOCAL_SLIST_PTR;
            query_ptr->SERVER_IPADDR = best_ptr->IPADDR;
            return( DNS_OK );
         }
      }

      // If the query has exhausted its retries, change our notion of best.
      // If that takes us back to the Primary, we can ignore the timeout.
      if (query_ptr->QUERY_RETRIES <= 0) {
         if (best_ptr->NEXT) {
            best_ptr = best_ptr->NEXT;
            last_time = time.SECONDS;
         }
         else {
            best_ptr = control_ptr->LOCAL_SLIST_PTR;
         }
      }
   }
   query_ptr->SERVER_IPADDR = best_ptr->IPADDR;
   return( DNS_OK );

} /* Endbody */
// End CR 2424


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_get_next_expired_query()
* Returned Value  : DNS_QUERY_ID_STRUCT _PTR_
* Comments        :
*
*END*-----------------------------------------------------------------*/


static  DNS_INTERNAL_QUERY_STRUCT _PTR_ DNS_get_next_expired_query
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_ _PTR_ query_head_ptr
   )

{ /* Body */

   DNS_INTERNAL_QUERY_STRUCT _PTR_ expired_query_ptr;


   if ( *query_head_ptr == NULL ) {
      return( NULL );
   } /* Endif */


   expired_query_ptr = *query_head_ptr;

   DNS_update_TTL( expired_query_ptr );

   if ( expired_query_ptr->TTL == 0 ) {
      *query_head_ptr = expired_query_ptr->NEXT;
      return( expired_query_ptr );
   } /* Endif */

   return( NULL );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_add_query_to_list()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_add_query_to_list
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_ _PTR_ query_head_ptr,
   DNS_INTERNAL_QUERY_STRUCT _PTR_       new_query_ptr
   )

{ /* Body */

   DNS_INTERNAL_QUERY_STRUCT _PTR_ previous_ptr = NULL;
   DNS_INTERNAL_QUERY_STRUCT _PTR_ temp_ptr;

   /*
   ** set the new_query_ptr->NEXT to be NULL, in case this is a query
   ** retry, the NEXT field may not be NULL. Just to be safe.
   */
   new_query_ptr->NEXT = NULL;

   /* first check to see if list is empty */
   if ( *query_head_ptr == NULL) {
      *query_head_ptr = new_query_ptr;
      return( DNS_OK );
   } /* Endif */

   temp_ptr  = *query_head_ptr;

   while ( temp_ptr != NULL ) {
      DNS_update_TTL( temp_ptr );

      if ( new_query_ptr->TTL < temp_ptr->TTL  ) {
         if ( previous_ptr ) {
            previous_ptr->NEXT  = new_query_ptr;
            new_query_ptr->NEXT = temp_ptr;
            return( DNS_OK );
         } else {
            new_query_ptr->NEXT = temp_ptr;
            *query_head_ptr     = new_query_ptr;
            return( DNS_OK );
         } /* Endif */
      } /* Endif */
      previous_ptr = temp_ptr;
      temp_ptr = temp_ptr->NEXT;
   } /* Endwhile */

   previous_ptr->NEXT = new_query_ptr;
   return( DNS_OK );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_send_no_query_error_response()
* Returned Value  : uint_32
* Comments        : This function is only used if DNS is unable to allocate
*                   or fill a DNS_INTERNAL_QUERY_STRUCT
*
*END*-----------------------------------------------------------------*/

static void DNS_send_no_query_error_response
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr,
   uint_16                 error_code,
   sockaddr_in       _PTR_ client_sin_ptr
   )

{ /* Body */

   DNS_MESSAGE_HEADER_STRUCT _PTR_  message_head_ptr;
   int_32                           error;
   uint_16                          pack_variable;

   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)control_ptr->RES_BUFFER;
   pack_variable = ntohs(message_head_ptr->CONTROL);
   htons(message_head_ptr->CONTROL,
         (pack_variable | DNS_RESPONSE | error_code));
   htons(message_head_ptr->ANCOUNT, DNS_NO_ANSWER);
   htons(message_head_ptr->ARCOUNT, 0);
   htons(message_head_ptr->NSCOUNT, 0);

   error = sendto(control_ptr->RES_SOCKET, control_ptr->RES_BUFFER,
                           control_ptr->RES_BUFFER_SIZE, 0,
                           client_sin_ptr, sizeof(*client_sin_ptr));
   if (error == RTCS_ERROR) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_SEND_QUERY, 3, 0, 0);
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_fill_generic_response_buffer()
* Returned Value  : uint-32
* Comments        : This function is used to fill the response buffer
*                   with information common to all queries
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_fill_generic_response_buffer
   (
   uchar                     _PTR_ buffer,
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr,
   uint_16                         error_code
   )

{ /* Body */

   DNS_MESSAGE_HEADER_STRUCT _PTR_   message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT   _PTR_   message_tail_ptr;
   uchar                     _PTR_   name_ptr;
   uchar                     _PTR_   temp_ptr;
   uint_32                           name_size;
   uint_32                           buffer_size;

   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer;

   name_size = strlen((char _PTR_)query_ptr->QNAME_PTR );
   /* To include the terminating NULL char */
   name_size++;

   buffer_size =   sizeof(DNS_MESSAGE_HEADER_STRUCT) + name_size
                 + sizeof(DNS_MESSAGE_TAIL_STRUCT);

   temp_ptr = buffer + sizeof(DNS_MESSAGE_HEADER_STRUCT)
                     + name_size;

   message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT _PTR_)temp_ptr;

   name_ptr  = buffer + sizeof(DNS_MESSAGE_HEADER_STRUCT);
   temp_ptr  = query_ptr->QNAME_PTR;

   htons(message_head_ptr->ID, query_ptr->QUERY_ID);
   htons(message_head_ptr->CONTROL,
        (query_ptr->CONTROL | error_code | DNS_RESPONSE));
   htons(message_head_ptr->ANCOUNT, DNS_NO_ANSWER);
   htons(message_head_ptr->QDCOUNT, DNS_SINGLE_QUERY);
   htons(message_head_ptr->NSCOUNT, DNS_NO_ANSWER);
   htons(message_head_ptr->ARCOUNT, DNS_NO_ANSWER);

   while ( name_size ) {
      htonc(name_ptr, *temp_ptr);
      name_ptr++;
      temp_ptr++;
      name_size--;
   } /* Endwhile */

   htons(message_tail_ptr->QTYPE, query_ptr->QTYPE);
   htons(message_tail_ptr->QCLASS, query_ptr->QCLASS);

   return( buffer_size );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_send_error_response()
* Returned Value  : void
* Comments        : This function is used if a DNS_INTERNAL_QUERY_STRUCT
*                   exists
*
*END*-----------------------------------------------------------------*/

static void DNS_send_error_response
   (
   uchar                     _PTR_ buffer,
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr,
   uint_16                         error_code,
   uint_32                         query_sock,
   int_32                          actual_error
   )

{ /* Body */

   int_32                            error;
   uint_32                           buffer_size;

   if ( actual_error != DNS_OK ) {
      RTCS_log_error(ERROR_DNS, actual_error, 0, 0, 0);
   } /* Endif */

   buffer_size = DNS_fill_generic_response_buffer( buffer, query_ptr,
                                                   error_code );
   /* Send the response buffer to the client */
   error = sendto(query_sock, buffer, buffer_size, 0,
                  &query_ptr->CLIENT_SIN,
                  sizeof(query_ptr->CLIENT_SIN));
   if (error == RTCS_ERROR) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_SEND_QUERY, 4, 0, 0);
   } /* Endif */

   DNS_free_internal_query( query_ptr );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_check_outstanding_queries()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/


static void DNS_check_outstanding_queries
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr
   )

{ /* Body */

   DNS_INTERNAL_QUERY_STRUCT _PTR_   current_ptr;
   int_32                            error;

   current_ptr = DNS_get_next_expired_query( &control_ptr->RES_QUERY_HEAD_PTR );

      while ( current_ptr ) {
         /*
         ** cycle through all queries and retry with different server, or
         ** return "name server error" response to client
         */
         // Start CR 2424
         current_ptr->QUERY_RETRIES--;
         error = DNS_get_best_name_server_for_query( current_ptr, control_ptr );
         // End CR 2424

         if ( current_ptr->QUERY_RETRIES <= 0 ) {
            DNS_send_error_response( control_ptr->RES_BUFFER, current_ptr,
                                     DNS_QUERY_REFUSED,
                                     control_ptr->RES_SOCKET, DNS_OK );
            current_ptr = DNS_get_next_expired_query(
                             &control_ptr->RES_QUERY_HEAD_PTR );
            continue;
         } /* Endif */

         if ( error != DNS_OK ) {
            DNS_send_error_response( control_ptr->RES_BUFFER, current_ptr,
                                     DNS_SERVER_FAILURE, control_ptr->RES_SOCKET,
                                     error );
            current_ptr = DNS_get_next_expired_query(
                             &control_ptr->RES_QUERY_HEAD_PTR );
            continue;
         } /* Endif */

         current_ptr->TTL      = DNS_QUERY_RETRY_TIME;
         current_ptr->QUERY_ID = DNS_get_query_id();

         error = DNS_Send_UDP_Query( control_ptr, current_ptr, FALSE );
         if ( error == RTCS_ERROR ) {
            DNS_send_error_response( control_ptr->RES_BUFFER, current_ptr,
                                     DNS_QUERY_REFUSED, control_ptr->RES_SOCKET,
                                     error );
            current_ptr = DNS_get_next_expired_query(
                             &control_ptr->RES_QUERY_HEAD_PTR );
            continue;
         } /* Endif */

         DNS_add_query_to_list( &control_ptr->RES_QUERY_HEAD_PTR, current_ptr );
         current_ptr = DNS_get_next_expired_query(
                          &control_ptr->RES_QUERY_HEAD_PTR );
      } /* Endwhile */

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_update_cache()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void DNS_update_cache
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr
   )

{ /* Body */
   DNS_RR_LIST_STRUCT _PTR_ _PTR_ local_cache_ptr = &control_ptr->CACHE_PTR;
   DNS_RR_LIST_STRUCT       _PTR_ previous_cache_ptr = NULL;
   DNS_RR_LIST_STRUCT       _PTR_ next_cache_ptr;
   DNS_RR_LIST_STRUCT       _PTR_ free_cache_ptr;
   DNS_RR_HEADER_STRUCT     _PTR_ previous_rr_head_ptr;
   DNS_RR_HEADER_STRUCT     _PTR_ next_rr_head_ptr;
   DNS_RR_HEADER_STRUCT     _PTR_ free_rr_head_ptr;
   TIME_STRUCT                    time;
   uint_32                        difference;

   _time_get( &time);

   next_cache_ptr = *local_cache_ptr;

   while ( next_cache_ptr != NULL ) {
      next_rr_head_ptr = next_cache_ptr->RR_HEAD_PTR;
      previous_rr_head_ptr = NULL;
      while ( next_rr_head_ptr != NULL ) {
         difference = time.SECONDS - next_rr_head_ptr->LAST_TTL_CHECK;
         if ( next_rr_head_ptr->TTL <= difference ) {
            if ( previous_rr_head_ptr ) {
               previous_rr_head_ptr->NEXT = next_rr_head_ptr->NEXT;
               free_rr_head_ptr           = next_rr_head_ptr;
               next_rr_head_ptr           = next_rr_head_ptr->NEXT;
            } else {
               next_cache_ptr->RR_HEAD_PTR = next_rr_head_ptr->NEXT;
               free_rr_head_ptr            = next_rr_head_ptr;
               next_rr_head_ptr            = next_rr_head_ptr->NEXT;
            } /* Endif */

            DNS_free_resource_record( free_rr_head_ptr );
         } else {
            next_rr_head_ptr->LAST_TTL_CHECK = time.SECONDS;
            next_rr_head_ptr->TTL            =  next_rr_head_ptr->TTL
                                                - difference;
            previous_rr_head_ptr = next_rr_head_ptr;
            next_rr_head_ptr     = next_rr_head_ptr->NEXT;
         } /* Endif */
      } /* Endwhile */

      if ( next_cache_ptr->RR_HEAD_PTR == NULL ) {
         if ( previous_cache_ptr ) {
            previous_cache_ptr->NEXT = next_cache_ptr->NEXT;
            free_cache_ptr = next_cache_ptr;
            next_cache_ptr = next_cache_ptr->NEXT;
         } else {
            *local_cache_ptr = next_cache_ptr->NEXT;
            free_cache_ptr = next_cache_ptr;
            next_cache_ptr = next_cache_ptr->NEXT;
         } /* Endif */

         if (control_ptr->NUM_NAMES_CACHED > 0) {
            control_ptr->NUM_NAMES_CACHED--;
         }/* Endif */
         _mem_free( free_cache_ptr->NAME_PTR );
         RTCS_part_free( free_cache_ptr );
      } else {
         previous_cache_ptr = next_cache_ptr;
         next_cache_ptr     = next_cache_ptr->NEXT;
      } /* Endif */
   } /* Endwhile */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_check_packet_for_name()
* Returned Value  : uint_32
* Comments        : Crude, but it will do for now.
*                   We should check every lable for compression, not
*                   just whole names -- could reduce packet size.
*                   (See DNS_assign_answer_middle_ptr() for a more
*                    elegant solution idea)
*                   However, this doesn't violate RFC 1035.
*
*END*-----------------------------------------------------------------*/

static uint_16 DNS_check_packet_for_name
   (
   uchar _PTR_ buffer_ptr,
   uint_32     buffer_size,
   uchar _PTR_ name_ptr
   )
{ /* Body */
   DNS_MESSAGE_HEADER_STRUCT _PTR_ message_head =
      (DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer_ptr;
   uchar _PTR_ temp_ptr = buffer_ptr + sizeof(DNS_MESSAGE_HEADER_STRUCT);
   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ response_ptr;
   uint_16 i, num_queries = ntohs(message_head->QDCOUNT);

   /* First, need to check the original queries. */
   for (i = 0; i < num_queries; i++) {
      if (strcmp((char _PTR_)temp_ptr, (char _PTR_)name_ptr) == 0) {
         return((uint_16)(temp_ptr - buffer_ptr));
      } /* Endif */
      if(ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK) {
         temp_ptr += sizeof(uint_16);
      } else {
         temp_ptr += strlen((char _PTR_)temp_ptr) + 1;
      } /* Endif */
      temp_ptr += sizeof(DNS_MESSAGE_TAIL_STRUCT);
   } /* Endfor */

   /* Now check the answer sections. */
   while (temp_ptr < buffer_ptr + buffer_size) {
      if (strcmp((char _PTR_)temp_ptr, (char _PTR_)name_ptr) == 0) {
         return((uint_16)(temp_ptr - buffer_ptr));
      } /* Endif */
      if(ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK) {
         temp_ptr += sizeof(uint_16);
      } else {
         temp_ptr += strlen((char _PTR_)temp_ptr) + 1;
      } /* Endif */
      response_ptr = (DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_)temp_ptr;
      temp_ptr += sizeof(DNS_RESPONSE_RR_MIDDLE_STRUCT);
      /* Checking the answer section for name as well. */
      if ((ntohs(response_ptr->TYPE) == DNS_PTR ||
           ntohs(response_ptr->TYPE) == DNS_CNAME ||
           ntohs(response_ptr->TYPE) == DNS_NS) &&
          (strcmp((char _PTR_)temp_ptr, (char _PTR_)name_ptr) == 0))
      {
          return((uint_16)(temp_ptr - buffer_ptr));
      } /* Endif */
      temp_ptr += ntohs(response_ptr->RDLENGTH);
   } /* Endwhile */
   return(0);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_fill_answer_section_of_buffer()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_fill_answer_section_of_buffer
   (
   DNS_CONTROL_STRUCT   _PTR_ control_ptr,
   DNS_RR_HEADER_STRUCT _PTR_ rr_ptr,
   uchar                _PTR_ buffer_ptr,
   int_32               _PTR_ buffer_size_ptr
   )

{ /* Body */

   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ response_ptr;
   uchar                         _PTR_ answer_ptr;
   uchar                         _PTR_ temp_ptr;
   uint_32                             new_buffer_size = *buffer_size_ptr;
   uint_32                             name_size;
   uint_16                             pack_variable, name_location;

   answer_ptr = buffer_ptr + *buffer_size_ptr;

   name_location = DNS_check_packet_for_name(buffer_ptr, *buffer_size_ptr,
      rr_ptr->DOMAIN_NAME_PTR);
   if(name_location) {
      /* Name is already in packet, use offset */
      pack_variable = (DNS_COMPRESSED_NAME_MASK << 8) | name_location;
      htons(answer_ptr, pack_variable);
      new_buffer_size += sizeof( uint_16 );
      answer_ptr      += sizeof( uint_16 );
   } else {
      /* Name doesn't exist in packet yet */
      temp_ptr  = rr_ptr->DOMAIN_NAME_PTR;
      name_size = strlen((char _PTR_)rr_ptr->DOMAIN_NAME_PTR );
      /* Need to include the last '\0' character as well */
      name_size++;
      new_buffer_size += name_size;
      while( name_size != 0 ) {
         htonc(answer_ptr, *temp_ptr);
         answer_ptr++;
         temp_ptr++;
         name_size--;
      } /* Endwhile */
   } /* Endif */

   response_ptr = (DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_)answer_ptr;

   htons(response_ptr->TYPE, rr_ptr->TYPE);
   htons(response_ptr->CLASS, rr_ptr->CLASS);
   htonl(response_ptr->TTL, rr_ptr->TTL);

   new_buffer_size += sizeof( DNS_RESPONSE_RR_MIDDLE_STRUCT );
   answer_ptr      += sizeof( DNS_RESPONSE_RR_MIDDLE_STRUCT );

   switch ( rr_ptr->TYPE ) {
      case DNS_A:
         htons(response_ptr->RDLENGTH, sizeof( _ip_address ));
         htonl(answer_ptr,
            ((DNS_ADDR_RECORD_STRUCT _PTR_)((void _PTR_)rr_ptr))->IPADDR);
         new_buffer_size += sizeof( _ip_address );
         break;

      case DNS_PTR:
      case DNS_CNAME:
      case DNS_NS:
         name_location = DNS_check_packet_for_name(buffer_ptr, *buffer_size_ptr,
            ((DNS_NAME_RECORD_STRUCT _PTR_)((void _PTR_)rr_ptr))->ANS_NAME_PTR);
         if(name_location) {
            /* Name is already in packet, use offset */
            pack_variable = (DNS_COMPRESSED_NAME_MASK << 8) | name_location;
            htons(answer_ptr, pack_variable);
            new_buffer_size += sizeof( uint_16 );
            answer_ptr      += sizeof( uint_16 );
         } else {
            temp_ptr = ((DNS_NAME_RECORD_STRUCT _PTR_)
               ((void _PTR_)rr_ptr))->ANS_NAME_PTR;
            name_size = strlen((char _PTR_)temp_ptr);
            name_size++; /* need the '\0' char */
            htons(response_ptr->RDLENGTH, name_size);
            new_buffer_size += name_size;
            while ( name_size != 0 ) {
               htonc(answer_ptr, *temp_ptr);
               answer_ptr++;
               temp_ptr++;
               name_size--;
            } /* Endwhile */
         } /* Endif */
         break;

      default:
      /*
      ** Unknown rr type, set new_buffer_size to 0, so cached data
      ** will not be used.
      */
      new_buffer_size = 0;
   } /* Endswitch */

   if (new_buffer_size) {
      *buffer_size_ptr = new_buffer_size;
   } /* Endif */

   return( new_buffer_size );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_fill_response_buffer_with_rr_data()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_fill_response_buffer_with_rr_data
   (
   DNS_CONTROL_STRUCT   _PTR_ control_ptr,
   DNS_RR_HEADER_STRUCT _PTR_ record_ptr,
   boolean                    all_data,
   boolean                    resolver_query
   )

{ /* Body */

   DNS_MESSAGE_HEADER_STRUCT _PTR_ message_head_ptr;
   DNS_RR_HEADER_STRUCT      _PTR_ temp_rr_ptr;
   int_32                    _PTR_ buffer_size_ptr;
   uchar                     _PTR_ buffer_ptr;
   uint_32                         buffer_size = 0;
   uint_16                         num_answers = 0;
   uint_16                         pack_variable;

   temp_rr_ptr = record_ptr;

   if ( resolver_query == TRUE ) {
      message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT_PTR)control_ptr->RES_BUFFER;
      buffer_size_ptr  = &control_ptr->RES_BUFFER_SIZE;
   } else {
      message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT_PTR)control_ptr->NS_BUFFER;
      buffer_size_ptr  = &control_ptr->NS_BUFFER_SIZE;
   } /* Endif */
   buffer_ptr = (uchar _PTR_)message_head_ptr;

   pack_variable    = ntohs( message_head_ptr->CONTROL );

   if ( all_data == TRUE ) {
      while ( temp_rr_ptr != NULL ) {
         num_answers++;
         buffer_size = DNS_fill_answer_section_of_buffer( control_ptr, temp_rr_ptr,
            buffer_ptr, buffer_size_ptr );
         temp_rr_ptr = temp_rr_ptr->NEXT;
      } /* Endwhile */
   } else {
      num_answers++;
      buffer_size = DNS_fill_answer_section_of_buffer( control_ptr, temp_rr_ptr, buffer_ptr,
         buffer_size_ptr );
   } /* Endif */

   /* insert return information here */
   htons(message_head_ptr->ANCOUNT, num_answers);
   pack_variable |= DNS_RESPONSE;
   htons(message_head_ptr->CONTROL, pack_variable);

   return( buffer_size );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_rr_canonical_data_exists()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_rr_canonical_data_exists
   (
   DNS_RECURSIVE_CACHE_STRUCT_PTR cache_ptr
   )
{ /* Body */
   uint_32                    buffer_size = 0;
   DNS_RR_LIST_STRUCT   _PTR_ new_list_ptr = cache_ptr->CACHE_HEAD;
   DNS_RR_HEADER_STRUCT _PTR_ record_ptr;

   if (++cache_ptr->NUM_TRIES >= DNS_MAX_NUM_CNAME_RETRIES ) {
      return 0;
   } /* Endif */

   while ( new_list_ptr != NULL ) {
      if( strcmp((char _PTR_)new_list_ptr->NAME_PTR,
                 (char _PTR_)cache_ptr->NAME_PTR) == 0 ) {
         /* The names match, see if the type of data requested exists */
         record_ptr = new_list_ptr->RR_HEAD_PTR;
         /* Search through the records for the data requested. */
         while ( record_ptr != NULL ) {
            if (( cache_ptr->NEW_QUERY_PTR->QTYPE == record_ptr->TYPE ) &&
                ( cache_ptr->NEW_QUERY_PTR->QCLASS == record_ptr->CLASS )) {
               /* We have a hit on data & class, fill buffer with info. */
               buffer_size =
                  DNS_fill_answer_section_of_buffer(cache_ptr->CONTROL_PTR,
                  record_ptr, cache_ptr->BUFFER_PTR,
                  cache_ptr->BUFFER_SIZE_PTR);
               break;
            } else if ( cache_ptr->NEW_QUERY_PTR->QCLASS == record_ptr->CLASS &&
                        record_ptr->TYPE == DNS_CNAME )
            {
               /*
               ** We have an alias, see if the data type exists for the
               ** Canonical Name.
               */
               cache_ptr->NAME_PTR  = ((DNS_NAME_RECORD_STRUCT _PTR_)
                  ((void _PTR_)record_ptr))->ANS_NAME_PTR;
               buffer_size = DNS_rr_canonical_data_exists(cache_ptr);
               if (buffer_size) {
                  /* We've had a hit, put alias hit in the buffer as well.*/
                  buffer_size =
                     DNS_fill_answer_section_of_buffer(cache_ptr->CONTROL_PTR,
                     record_ptr, cache_ptr->BUFFER_PTR,
                     cache_ptr->BUFFER_SIZE_PTR);
               } /* Endif */
               break;
            } else {
               record_ptr = record_ptr->NEXT;
            } /* Endif */
         } /* Endwhile */
         break;
      } else {
         new_list_ptr = new_list_ptr->NEXT;
      } /* Endif */
   } /* Endwhile */

   return(buffer_size);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_assign_answer_middle_ptr()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

static DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ DNS_assign_answer_middle_ptr
   (
   uchar _PTR_ answer_head_ptr,
   uint_16     total_answers,
   uint_16     current_answer_num
   )

{ /* Body */

   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ rr_middle_ptr = NULL;
   uchar                         _PTR_ temp_ptr;
   uint_32                             name_size = 0;

   if ( total_answers - 1 > current_answer_num ) {
      temp_ptr = answer_head_ptr;

      while( (ntohc(temp_ptr) != '\0') &&
             name_size < DNS_MAX_CHARS_IN_NAME &&
             !(ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK)) {
            name_size += ntohc(temp_ptr);
            temp_ptr = temp_ptr + ntohc(temp_ptr) + 1;
      } /* Endwhile */

      if ( ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK ) {
         temp_ptr+= 2;
      } else {
         temp_ptr++;
      }/* Endif */

      rr_middle_ptr = (DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_)temp_ptr;
   } /* Endif */

   return( rr_middle_ptr );

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_assign_answer_pointers()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void DNS_assign_answer_pointers
   (
   uchar                               _PTR_ buffer,
   uchar                         _PTR_ _PTR_ answer_ptr,
   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ _PTR_ answer_middle_ptr
   )

{ /* Body */

   uchar _PTR_ temp_ptr;
   uint_32     name_size;

   temp_ptr = buffer + sizeof(DNS_MESSAGE_HEADER_STRUCT);
   name_size = strlen((char _PTR_)temp_ptr);
   name_size++;

   *answer_ptr = temp_ptr + name_size + sizeof(DNS_MESSAGE_TAIL_STRUCT);
   temp_ptr    = *answer_ptr;

   /* Passing this function two hard coded values to force the assignment */
   *answer_middle_ptr = DNS_assign_answer_middle_ptr( temp_ptr, 3, 0 );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_rr_data_exists()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/


static uint_32 DNS_rr_data_exists
   (
   DNS_INTERNAL_QUERY_STRUCT _PTR_ new_query_ptr,
   DNS_CONTROL_STRUCT        _PTR_ control_ptr,
   uchar                     _PTR_ buffer_ptr,
   int_32                    _PTR_ buffer_size_ptr,
   uint_32                         query_type
   )

{ /* Body */

   DNS_RR_LIST_STRUCT   _PTR_ new_list_ptr = NULL;
   DNS_RR_LIST_STRUCT   _PTR_ cache_head;
   DNS_RR_LIST_STRUCT   _PTR_ previous_ptr = NULL;
   DNS_RR_HEADER_STRUCT _PTR_ record_ptr;
   DNS_RECURSIVE_CACHE_STRUCT cache;
   DNS_MESSAGE_HEADER_STRUCT _PTR_   message_head_ptr;
   uint_16                    pack_variable;
   uint_32                    ans_count, i, buffer_size = 0;
   int_32                     cmp_result;
   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ answer_middle_ptr;
   uchar                _PTR_ answer_head_ptr;
   boolean                    resolver_query = FALSE;

   if ( query_type == DNS_NS_CACHE_DATA ) {
      new_list_ptr = control_ptr->CACHE_PTR;
      resolver_query = FALSE;
   } else if ( query_type == DNS_NS_AUTH_DATA ) {
      new_list_ptr = control_ptr->NS_ZONE_AUTH_LIST;
      resolver_query = FALSE;
   } else if (query_type == DNS_RES_CACHE_DATA ) {
      new_list_ptr = control_ptr->CACHE_PTR;
      resolver_query = TRUE;
   } /* Endif */
   cache_head = new_list_ptr;

   while ( new_list_ptr != NULL ) {
      cmp_result = strcmp((char _PTR_)new_list_ptr->NAME_PTR,
         (char _PTR_)new_query_ptr->QNAME_PTR );
      if ( cmp_result == 0 ) {
         /* The names match, see if the type of data requested exists */
         record_ptr = new_list_ptr->RR_HEAD_PTR;
         if ( new_query_ptr->QTYPE == DNS_ALL ) {
            buffer_size = DNS_fill_response_buffer_with_rr_data(
                                                            control_ptr,
                                                            record_ptr,
                                                            TRUE,
                                                            resolver_query );
         } else {
            /*
            ** We have a hit on the name, but we only want one specific record,
            ** search through the records for this name for the data requested.
            */
            while ( record_ptr != NULL ) {
               if (( new_query_ptr->QTYPE == record_ptr->TYPE ) &&
                   ( new_query_ptr->QCLASS == record_ptr->CLASS )) {
                  /* We have a hit on data & class, fill buffer with info. */
                  buffer_size = DNS_fill_response_buffer_with_rr_data(
                                                            control_ptr,
                                                            record_ptr,
                                                            FALSE,
                                                            resolver_query );
                  break;
               } else if ( new_query_ptr->QCLASS == record_ptr->CLASS &&
                           record_ptr->TYPE == DNS_CNAME )
               {
                  /*
                  ** We have an alias, see if the data type exists for the
                  ** Canonical Name.
                  */
                  ans_count = ntohs(((DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer_ptr)->ANCOUNT );
                  if(ans_count != 0 ) {
                     /*
                     ** We are checking a response, not a new query, see if the
                     ** answer exists in the packet already.
                     */
                     DNS_assign_answer_pointers(buffer_ptr, &answer_head_ptr,
                        &answer_middle_ptr);
                     for (i = 0; (i <= ans_count) && answer_middle_ptr; i++) {
                         if ((new_query_ptr->QTYPE == ntohs(answer_middle_ptr->TYPE)) &&
                              (new_query_ptr->QCLASS == ntohs(answer_middle_ptr->CLASS)))
                         {
                            buffer_size = *buffer_size_ptr;
                            break;
                         } /* Endif */
                         answer_head_ptr = ((uchar _PTR_)(answer_middle_ptr + 1)) +
                            ntohs(answer_middle_ptr->RDLENGTH);
                         answer_middle_ptr = DNS_assign_answer_middle_ptr(
                            answer_head_ptr, ans_count, i);
                     } /* Endfor */
                     break;
                  } else {
                     cache.NEW_QUERY_PTR = new_query_ptr;
                     cache.CONTROL_PTR = control_ptr;
                     cache.CACHE_HEAD  = cache_head;
                     cache.NAME_PTR    = record_ptr->DOMAIN_NAME_PTR;
                     cache.BUFFER_PTR  = buffer_ptr;
                     cache.BUFFER_SIZE_PTR = buffer_size_ptr;
                     cache.NUM_TRIES       = 0;
                     buffer_size = DNS_rr_canonical_data_exists(&cache);
                     if (buffer_size) {
                        /* insert return information here */
                        message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer_ptr;
                        htons(message_head_ptr->ANCOUNT, cache.NUM_TRIES);
                        pack_variable = ntohs(message_head_ptr->CONTROL);
                        pack_variable |= DNS_RESPONSE;
                        htons(message_head_ptr->CONTROL, pack_variable);
                     } /* Endif */
                     break;
                  } /* Endif */
               } else {
                  record_ptr = record_ptr->NEXT;
               } /* Endif */
            } /* Endwhile */
         } /* Endif */

         break;
      } else {
         previous_ptr = new_list_ptr;
         new_list_ptr = new_list_ptr->NEXT;
      } /* Endif */
   } /* Endwhile */

   if ( previous_ptr != NULL && new_list_ptr != NULL &&
        (query_type == DNS_NS_CACHE_DATA ||
         query_type == DNS_RES_CACHE_DATA)  ) {
      /*
      ** The most recently accessed data in the cache should always be at
      ** the head of the list.  If the new_list_ptr is NULL, cached
      ** data for the name doesn't exist in the list.
      */
      previous_ptr->NEXT    = new_list_ptr->NEXT;
      new_list_ptr->NEXT    = control_ptr->CACHE_PTR;
      control_ptr->CACHE_PTR = new_list_ptr;
   } /* Endif */

   return( buffer_size );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_is_valid_IP_addr()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_is_valid_IP_addr
   (
   uchar _PTR_       buffer_ptr,
   uchar _PTR_       domain_name,
   uchar _PTR_ _PTR_ local_addr_buffer
   )

{ /* Body */
   uchar _PTR_ check_ptr;
   uchar _PTR_ temp_ptr;
   uchar _PTR_ free_buffer;
   uchar       appended_name[] = "in-addr.arpa";
   uint_16     new_location;
   uint_16     label_size;
   int_32      byte_num;
   uint_32     digit_count;
   uint_32     total_size = 0;
   uint_32     i;
   uint_32     j;

   temp_ptr = (uchar _PTR_)RTCS_mem_alloc_zero( DNS_MAX_CHARS_IN_NAME );
   if ( temp_ptr == NULL ) {
      RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 1, 0, 0);
      return( DNS_SERVER_FAILURE );
   } /* Endif */

   free_buffer = temp_ptr;

   /* first check to see if name is written in correct IP address format */

   check_ptr = domain_name;

   for ( i = 0; i < 4; i++) {
      label_size = ntohc(check_ptr);

      if ( label_size & DNS_COMPRESSED_NAME_MASK ) {
         new_location = ntohs(check_ptr) & DNS_COMPRESSED_LOCATION_MASK;
         check_ptr    = buffer_ptr + new_location;
         label_size   = ntohc(check_ptr);
      } /* Endif */

      /*
      ** Some IP addresses using "in-addr.arpa" do not always have four
      ** labels, if this is the case, break out of the loop.
      */
      if ( label_size == 7 ) {
         break;
      } /* Endif */

      *temp_ptr++ = label_size;
      check_ptr++;
      total_size++;
      digit_count = 0;

     if (( label_size < 1 ) || ( label_size > 3 )) {
         _mem_free( free_buffer );
         return( RTCSERR_DNS_INVALID_IP_ADDR );
      } /* Endif */

      for ( j = 0; j < label_size; j++ ) {
         if ( digit_count == 0 ) {
              byte_num = 0;
         } /* Endif */
         digit_count++;

         if ( isdigit(ntohc(check_ptr)) ) {
            /* number is in range */
            byte_num    = byte_num + ntohc(check_ptr) - '0';
            byte_num    = byte_num * 10;
            *temp_ptr++ = ntohc(check_ptr);
            check_ptr++;
            total_size++;
         } else {
            /* if the characters are not decimal digits it's invalid */
            _mem_free( free_buffer );
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         } /* Endif */
      } /* Endfor */

      byte_num = byte_num / 10; /* shift back */

      if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
            _mem_free( free_buffer );
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         } /* Endif */
   } /* Endfor */

   if ( ntohc(check_ptr) != 7 ) {
      if ( ntohc(check_ptr) & DNS_COMPRESSED_NAME_MASK ) {
         new_location = ntohs(check_ptr) & DNS_COMPRESSED_LOCATION_MASK;
         check_ptr     = buffer_ptr + new_location;
         if ( ntohc(check_ptr) != 7 ) {
            _mem_free( free_buffer );
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         } /* Endif */

      }else {
         _mem_free( free_buffer );
         return( RTCSERR_DNS_INVALID_IP_ADDR  );
      } /* Endif */
   } /* Endif */

   *temp_ptr++ = ntohc(check_ptr);
   check_ptr++;

   /* should be "in-addr" */
   for ( i = 0; i < 7;  i++) {
      if ( tolower(ntohc(check_ptr)) != appended_name[i] ) {
         _mem_free( free_buffer );
         return( RTCSERR_DNS_INVALID_IP_ADDR  );
      } /* Endif */
      *temp_ptr++ = appended_name[i];
      check_ptr++;
   } /* Endfor */

   if ( ntohc(check_ptr) != 4 ) {
      if ( ntohc(check_ptr) & DNS_COMPRESSED_NAME_MASK ) {
         new_location = ntohs(check_ptr) & DNS_COMPRESSED_LOCATION_MASK;
         check_ptr     = buffer_ptr + new_location;
         if ( ntohc(check_ptr) != 4 ) {
            _mem_free( free_buffer );
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         } /* Endif */

      }else {
         _mem_free( free_buffer );
         return( RTCSERR_DNS_INVALID_IP_ADDR  );
      } /* Endif */
   } /* Endif */

   *temp_ptr++ = ntohc(check_ptr);
   check_ptr++;
   i++;

   /* should be "arpa" */
   for ( ; i < 12;  i++) {
      if ( tolower(ntohc(check_ptr)) != appended_name[i] ) {
         _mem_free( free_buffer );
         return( RTCSERR_DNS_INVALID_IP_ADDR  );
      } /* Endif */
      *temp_ptr++ = appended_name[i];
      check_ptr++;
   } /* Endfor */

   /*
   ** If we got this far, it's a legal IP addr, get a buffer,
   ** fill it with the address.
   */

   /*
   ** Total size is the size of the IP address, 14 is the size of
   ** "in-addr.arpa" including all label length chars and the terminating
   ** '\0' character.
   */
   total_size += sizeof(appended_name) + 2;
   *local_addr_buffer = (uchar _PTR_)RTCS_mem_alloc_zero( total_size );
   if ( *local_addr_buffer == NULL ) {
      // START CR 2178
      _mem_free( free_buffer );
      // END CR 2178
      RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 2, 0, 0);
      return( DNS_SERVER_FAILURE );
   } /* Endif */

   _mem_copy(free_buffer, *local_addr_buffer, total_size);
   _mem_free( free_buffer );

   return( DNS_OK );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_is_valid_domain_name()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/


uint_32 DNS_is_valid_domain_name
   (
   uchar _PTR_         buffer_ptr,
   uchar _PTR_         domain_name,
   uchar _PTR_  _PTR_  local_name_buffer
   )

{ /* Body */
   uchar _PTR_  fill_ptr;
   uchar _PTR_  free_ptr;
   uchar _PTR_  check_name;
   uint_16      new_location;
   uint_32      total_length = 0;
   uint_32      label_size;
   uint_32      error;
   uint_32      i;

   error = DNS_is_valid_IP_addr(buffer_ptr, domain_name, local_name_buffer);
   if ( error != DNS_OK ) {
       /* If the request is for the ROOT Domain Name */
      if ( ntohc(domain_name) == '\0' ) {
         *local_name_buffer = (uchar _PTR_)RTCS_mem_alloc_zero( 1 );
         if ( *local_name_buffer == NULL ) {
            RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 3, 0, 0);
            return( DNS_SERVER_FAILURE );
         } /* Endif */

         (*local_name_buffer)[0] = '\0';
         return( DNS_OK );
      } /* Endif */

      fill_ptr = (uchar _PTR_)RTCS_mem_alloc_zero( DNS_MAX_CHARS_IN_NAME );
      if ( fill_ptr == NULL ) {
         RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 4, 0, 0);
         return( DNS_SERVER_FAILURE );
      } /* Endif */

      free_ptr = fill_ptr;
      check_name = domain_name;
      while ( ntohc(check_name) != '\0' ) {
         if ( ntohc(check_name) & DNS_COMPRESSED_NAME_MASK ) {
              new_location = ntohs(check_name) & DNS_COMPRESSED_LOCATION_MASK;
              check_name   = buffer_ptr + new_location;
         } /* Endif */

         label_size  = ntohc(check_name);
         *fill_ptr++ = label_size;
         check_name++;
         total_length++;

         /*
         ** RFC 1035 says labels must start with a letter, but in practice
         ** some aren't
         */
         if ( isalnum(ntohc(check_name)) ) {
            *fill_ptr++ = ntohc(check_name);
            check_name++;
            total_length++;
         } else {
            _mem_free( free_ptr );
            return( RTCSERR_DNS_INVALID_NAME );
         } /* Endif */


         for ( i = 1; i < label_size; i++ ) {
            /* Domain Name characters can only be letters, hyphens, or numbers */
            if ((isalnum(ntohc(check_name)) || ntohc(check_name) == '-' ) &&
                total_length < DNS_MAX_CHARS_IN_NAME ) {
               *fill_ptr++ = ntohc(check_name);
               check_name++;
               total_length++;
            } else {
               _mem_free( free_ptr );
               return( RTCSERR_DNS_INVALID_NAME );
            } /* Endif */
         } /* Endfor */
      } /* Endwhile */

      /* This is to append the terminating '\0' character to the string */
      *fill_ptr = '\0';
      total_length++;

      *local_name_buffer = (uchar _PTR_)RTCS_mem_alloc_zero( total_length );
      if ( *local_name_buffer == NULL ) {
          /* START CR 2178 */
            _mem_free( free_ptr );
          /* END CR 2178 */
            RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 5, 0, 0);
            return( DNS_SERVER_FAILURE );
      } /* Endif */

      _mem_copy(free_ptr, *local_name_buffer, total_length);
      _mem_free( free_ptr );
      error = DNS_OK;
   } /* Endif */

   return( error );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_handle_new_query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

 static uint_32 DNS_handle_new_query
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr,
   sockaddr_in        _PTR_ client_sin_ptr
   )

{ /* Body */

   DNS_INTERNAL_QUERY_STRUCT _PTR_  new_query_ptr;
   DNS_MESSAGE_HEADER_STRUCT _PTR_  message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT   _PTR_  message_tail_ptr;
   uchar                     _PTR_  qname_ptr;
   uchar                     _PTR_  temp_ptr;
   TIME_STRUCT                      time;
   uint_32                          buffer_size;
   uint_32                          local_name_size;
/* Start CR 2171 */
   int_32                           error=DNS_OK;
/* End CR 2171 */

   _time_get( &time );

   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)control_ptr->RES_BUFFER;
   qname_ptr = control_ptr->RES_BUFFER + sizeof( DNS_MESSAGE_HEADER_STRUCT );


   new_query_ptr = (DNS_INTERNAL_QUERY_STRUCT _PTR_)
      RTCS_part_alloc_zero(control_ptr->INT_QUERY_PARTID);
   if ( new_query_ptr == NULL ) {
      RTCS_log_error( ERROR_DNS, RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY, 0, 0, 0);
      DNS_send_no_query_error_response( control_ptr, DNS_SERVER_FAILURE,
                                        client_sin_ptr );
      return( DNS_SERVER_FAILURE );
   } /* Endif */

   temp_ptr = qname_ptr;

   local_name_size = 0;
   while( (ntohc(temp_ptr) != '\0') &&
          local_name_size < DNS_MAX_CHARS_IN_NAME ) {
      local_name_size = local_name_size + ntohc(temp_ptr) + 1;
      temp_ptr  = temp_ptr  + ntohc(temp_ptr) + 1;
   } /* Endwhile */

   /* to include the last NULL (or compressed pointer) char */
   temp_ptr++;
   message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT _PTR_)temp_ptr;

   switch ( ntohs(message_tail_ptr->QTYPE) ) {

      case DNS_A:
         error = DNS_is_valid_domain_name( control_ptr->RES_BUFFER, qname_ptr,
                                           &new_query_ptr->QNAME_PTR );
         break;

      case DNS_PTR:
         error = DNS_is_valid_IP_addr( control_ptr->RES_BUFFER, qname_ptr,
                                       &new_query_ptr->QNAME_PTR );
         break;

      default:
         // Start CR 2171
         DNS_free_internal_query( new_query_ptr );
         DNS_send_no_query_error_response(control_ptr, DNS_QUERY_NOT_IMPLEMENTED,
                                          client_sin_ptr );
         return DNS_OK;
         // End CR 2171
   } /* Endswitch */

   if ( error == RTCSERR_DNS_INVALID_NAME || error == RTCSERR_DNS_INVALID_IP_ADDR ) {
      DNS_free_internal_query( new_query_ptr );
      DNS_send_no_query_error_response( control_ptr, DNS_NAME_ERROR,
                                        client_sin_ptr );
      return( DNS_OK );
   } /* Endif */

   if ( error == DNS_SERVER_FAILURE ) {
      DNS_free_internal_query( new_query_ptr );
      DNS_send_no_query_error_response( control_ptr, DNS_SERVER_FAILURE,
                                        client_sin_ptr );
      return( DNS_OK );
   } /* Endif */

   new_query_ptr->QTYPE          = ntohs(message_tail_ptr->QTYPE);
   new_query_ptr->QCLASS         = ntohs(message_tail_ptr->QCLASS);
   new_query_ptr->CONTROL        = ntohs(message_head_ptr->CONTROL)
                                   | DNS_RECURSION_DESIRED;

   new_query_ptr->CLIENT_SIN.sin_family      = client_sin_ptr->sin_family;
   new_query_ptr->CLIENT_SIN.sin_port        = client_sin_ptr->sin_port;
   new_query_ptr->CLIENT_SIN.sin_addr.s_addr =
                                            client_sin_ptr->sin_addr.s_addr;
   /*
   ** Check to see if the information is in the local cache. If it is,
   ** return it to the client, otherwise, make the query to a name server
   */
   buffer_size = DNS_rr_data_exists( new_query_ptr, control_ptr,
      control_ptr->RES_BUFFER, &control_ptr->RES_BUFFER_SIZE,
      DNS_RES_CACHE_DATA );

   if ( buffer_size != 0 ) {
      error = DNS_return_UDP_response_to_query( new_query_ptr,
                                                control_ptr->RES_BUFFER,
                                                buffer_size,
                                                control_ptr->RES_SOCKET );
      return( DNS_OK );
   } /* Endif */

   new_query_ptr->TTL            = DNS_QUERY_RETRY_TIME;
   new_query_ptr->QUERY_ID       = DNS_get_query_id();
   new_query_ptr->QUERY_RETRIES  = DNS_MAX_NUMBER_OF_QUERY_TRIES;
   new_query_ptr->LAST_TTL_CHECK = time.SECONDS;
   new_query_ptr->NEXT           = NULL;
   new_query_ptr->SERVER_IPADDR  = 0;

   error = DNS_get_best_name_server_for_query( new_query_ptr,
                                               control_ptr );
   if ( error != DNS_OK ) {
      DNS_send_error_response( control_ptr->RES_BUFFER, new_query_ptr,
                               DNS_SERVER_FAILURE, control_ptr->RES_SOCKET,
                               error );
      return( RTCSERR_DNS_UNABLE_TO_SEND_QUERY );
   } /* Endif */

   error = DNS_Send_UDP_Query( control_ptr, new_query_ptr, FALSE );
   if ( error != DNS_OK ) {
      DNS_send_error_response( control_ptr->RES_BUFFER, new_query_ptr,
                               DNS_SERVER_FAILURE, control_ptr->RES_SOCKET,
                               error );
       return( RTCSERR_DNS_UNABLE_TO_SEND_QUERY );
   } /* Endif */

   error = DNS_add_query_to_list( &control_ptr->RES_QUERY_HEAD_PTR,
                                  new_query_ptr );
   if ( error != DNS_OK ) {
      DNS_send_error_response( control_ptr->RES_BUFFER, new_query_ptr,
                               DNS_SERVER_FAILURE, control_ptr->RES_SOCKET,
                               error );
   } /* Endif */

   return( error );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_retrieve_query()
* Returned Value  : DNS_INTERNAL_QUERY_STRUCT_PTR
* Comments        :
*
*END*-----------------------------------------------------------------*/


static DNS_INTERNAL_QUERY_STRUCT _PTR_ DNS_retrieve_query
   (
   uint_16                               id,
   DNS_INTERNAL_QUERY_STRUCT _PTR_ _PTR_ query_head_ptr
   )

{ /* Body */

   DNS_INTERNAL_QUERY_STRUCT _PTR_ previous_ptr = NULL;
   DNS_INTERNAL_QUERY_STRUCT _PTR_ next_query_ptr;

   next_query_ptr  = *query_head_ptr;

   while ( next_query_ptr != NULL ) {
      if ( next_query_ptr->QUERY_ID == id ) {
         if ( previous_ptr != NULL ) {
            previous_ptr->NEXT  = next_query_ptr->NEXT;
            break;
         } else {
            *query_head_ptr = next_query_ptr->NEXT;
            break;
         } /* Endif */
      } /* Endif */
      previous_ptr = next_query_ptr;
      next_query_ptr = next_query_ptr->NEXT;
   } /* Endwhile */

   return( next_query_ptr );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_remove_last_entry_from_cache
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/
static void DNS_remove_last_entry_from_cache
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr
   )
{ /* Body */
   DNS_RR_LIST_STRUCT _PTR_  _PTR_ rr_list_ptr = &control_ptr->CACHE_PTR;
   DNS_RR_HEADER_STRUCT      _PTR_ rr_ptr;
   DNS_RR_HEADER_STRUCT      _PTR_ free_ptr;


   if (*rr_list_ptr) {
      while ((*rr_list_ptr)->NEXT) {
         /* Run the list, we have to delete the last entry. */
         rr_list_ptr = &(*rr_list_ptr)->NEXT;
      } /* Endwhile */

      /* Delete the entry */
      if ( control_ptr->NUM_NAMES_CACHED > 0 ) {
         control_ptr->NUM_NAMES_CACHED--;
      }/* Endif */

      rr_ptr = (*rr_list_ptr)->RR_HEAD_PTR;
      while (rr_ptr) {
         free_ptr = rr_ptr;
         rr_ptr = rr_ptr->NEXT;
         DNS_free_resource_record(free_ptr);
      } /* Endwhile */

      _mem_free((*rr_list_ptr)->NAME_PTR);
      RTCS_part_free(*rr_list_ptr);
      *rr_list_ptr = NULL;
   }/* Endif */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_add_to_cache()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void DNS_add_to_cache
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr,
   uint_32                 module
   )

{ /* Body */

   DNS_MESSAGE_HEADER_STRUCT     _PTR_ message_head_ptr;
   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_ answer_middle_ptr;
   DNS_RR_HEADER_STRUCT          _PTR_ rr_head_ptr;
   DNS_RR_HEADER_STRUCT          _PTR_ temp_cache_ptr;
   DNS_RR_HEADER_STRUCT          _PTR_ previous_cache_ptr = NULL;
   DNS_RR_LIST_STRUCT            _PTR_ rr_list_ptr;
   DNS_ADDR_RECORD_STRUCT        _PTR_ addr_rr_ptr;
   DNS_NAME_RECORD_STRUCT        _PTR_ name_rr_ptr;
   TIME_STRUCT                         time;
   uchar                         _PTR_ buffer_ptr = NULL;
   uchar                         _PTR_ answer_head_ptr;
   uchar                         _PTR_ temp_ptr;
   uint_16                             num_answers;
   uint_16                             control;
   uint_16                             answer_length;
   uint_32                             i;
   uint_32                             error;
   int_32                              cmp_val;
   int_32                              ttl;

   if ( module == DNS_NS_CACHE_DATA ) {
      buffer_ptr = control_ptr->NS_BUFFER;
   } /* Endif */

   if ( module == DNS_RES_CACHE_DATA ) {
      buffer_ptr = control_ptr->RES_BUFFER;
   } /* Endif */

   if ( buffer_ptr != NULL ) {
      DNS_assign_answer_pointers(buffer_ptr, &answer_head_ptr,
                                 &answer_middle_ptr );
      message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer_ptr;
      num_answers = ntohs(message_head_ptr->ANCOUNT);

      control = ntohs(message_head_ptr->CONTROL);
      if ( (control & DNS_TRUNCATION) == 0 ) {
         /* If the truncation bit is not set, cache information */
         for ( i = 0; (i < num_answers) && answer_middle_ptr; i++ ) {
            ttl           = ntohl(answer_middle_ptr->TTL);
            answer_length = ntohs(answer_middle_ptr->RDLENGTH);
            temp_ptr      = (uchar _PTR_)answer_middle_ptr;
            temp_ptr      += sizeof(DNS_RESPONSE_RR_MIDDLE_STRUCT);

            if ( (ttl == 0) || (ttl > DNS_MAX_ALLOWED_TTL)) {

               /* bypass this answer, set the pointers for the next one */
               temp_ptr = temp_ptr + answer_length;
               answer_head_ptr = temp_ptr;
               answer_middle_ptr = DNS_assign_answer_middle_ptr(answer_head_ptr,
                                                                num_answers, i);
               continue;
            } /* Endif */

            if ((control & DNS_NAME_ERROR) != 0 ) {
               if ((control & DNS_AUTHORITATIVE_NAME_ERROR) !=
                   DNS_AUTHORITATIVE_NAME_ERROR ) {
                  /*
                  ** Don't cache this answer if the name error isn't
                  ** authoritative.
                  */
                  break;
               } /* Endif */
            } /* Endif */

            switch ( ntohs(answer_middle_ptr->TYPE) ) {
               case DNS_A:
                  addr_rr_ptr = (DNS_ADDR_RECORD_STRUCT _PTR_)
                     RTCS_part_alloc_zero(control_ptr->ADDR_RR_PARTID);
                  if (!addr_rr_ptr) {
                    RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 5, 0, 0);
                    return;
                  }/* Endif */
                  addr_rr_ptr->IPADDR = ntohl((uchar_ptr)temp_ptr);
                  rr_head_ptr = (DNS_RR_HEADER_STRUCT _PTR_)
                     ((void _PTR_)addr_rr_ptr);
                  break;

               case DNS_PTR:
               case DNS_CNAME:
               case DNS_NS:
                  name_rr_ptr = (DNS_NAME_RECORD_STRUCT _PTR_)
                     RTCS_part_alloc_zero(control_ptr->NAME_RR_PARTID);
                  if (!name_rr_ptr) {
                    RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 5, 0, 0);
                    return;
                  }/* Endif */
                  error = DNS_is_valid_domain_name(buffer_ptr, temp_ptr,
                                                &name_rr_ptr->ANS_NAME_PTR );
                  if ( error != DNS_OK ) {
                     RTCS_part_free(name_rr_ptr);
                     /* set pointers for next answer */
                     temp_ptr = temp_ptr + answer_length;
                     answer_head_ptr = temp_ptr;
                     answer_middle_ptr = DNS_assign_answer_middle_ptr(
                                                              answer_head_ptr,
                                                              num_answers, i );
                     continue;
                  } /* Endif */
                  rr_head_ptr = (DNS_RR_HEADER_STRUCT _PTR_)
                     ((void _PTR_)name_rr_ptr);
                  break;

               default:
                  /* Unknown type, don't cache */
                  /* set pointers for next answer */
                  temp_ptr = temp_ptr + answer_length;
                  answer_head_ptr = temp_ptr;
                  answer_middle_ptr = DNS_assign_answer_middle_ptr(
                                                              answer_head_ptr,
                                                              num_answers, i );
                  continue;
            } /* Endswitch */

            if ( (control & DNS_AUTHORITATIVE_NAME_ERROR) !=
                 DNS_AUTHORITATIVE_NAME_ERROR ) {
               rr_head_ptr->TTL = ttl;
            } else {
               rr_head_ptr->TTL = DNS_NAME_ERROR_TTL;
            } /* Endif */

            rr_head_ptr->CLASS = ntohs(answer_middle_ptr->CLASS);
            rr_head_ptr->TYPE  = ntohs(answer_middle_ptr->TYPE);
            rr_head_ptr->NEXT  = NULL;

            /*
            ** May have to check name format here. Answers may truncate
            ** differently when same answer name is used more than once.
            */
            error = DNS_is_valid_domain_name(buffer_ptr, answer_head_ptr,
                                             &rr_head_ptr->DOMAIN_NAME_PTR );
            if ( error != DNS_OK ) {
               DNS_free_resource_record( rr_head_ptr );
               /* set pointers for next answer */
               temp_ptr = temp_ptr + answer_length;
               answer_head_ptr = temp_ptr;
               answer_middle_ptr = DNS_assign_answer_middle_ptr(
                                                              answer_head_ptr,
                                                              num_answers, i );
               continue;
            } /* Endif */

            _time_get( &time );
            rr_head_ptr->LAST_TTL_CHECK = time.SECONDS;
            /*
            ** need exclusive access, both the Resolver and Name Server can
            ** modify the cache
            */
            RTCS_mutex_lock(&control_ptr->CACHE_MUTEX);
            rr_list_ptr = control_ptr->CACHE_PTR;

            while ( rr_list_ptr != NULL ) {
               cmp_val = strcmp((char _PTR_)rr_head_ptr->DOMAIN_NAME_PTR,
                                (char _PTR_)rr_list_ptr->NAME_PTR );
               if ( cmp_val == 0 ) {
                  /* The two are equal, cached data for this name exists. */
                  break;
               } /* Endif */
               rr_list_ptr = rr_list_ptr->NEXT;
            } /* Endwhile */

            if ( rr_list_ptr == NULL ) {
               /* We're either at the head or the tail of the list */
               rr_list_ptr = (DNS_RR_LIST_STRUCT _PTR_)
                  RTCS_part_alloc_zero(control_ptr->RR_LIST_PARTID);
               if ( rr_list_ptr == NULL ) {
                  RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY, 0, 0, 0);
                  _mem_free( rr_head_ptr->DOMAIN_NAME_PTR );
                  DNS_free_resource_record( rr_head_ptr );
                  /* set pointers for next answer */
                  temp_ptr = temp_ptr + answer_length;
                  answer_head_ptr = temp_ptr;
                  answer_middle_ptr = DNS_assign_answer_middle_ptr(
                                                              answer_head_ptr,
                                                              num_answers, i );
                  continue;
               } /* Endif */

               rr_list_ptr->NAME_PTR    = rr_head_ptr->DOMAIN_NAME_PTR;
               rr_list_ptr->RR_HEAD_PTR = rr_head_ptr;
               rr_list_ptr->NEXT        = control_ptr->CACHE_PTR;
               control_ptr->CACHE_PTR    = rr_list_ptr;
               control_ptr->NUM_NAMES_CACHED++;
            } else {
               /*
               ** Cached data using the same name exists, search the
               ** existing sub-list under the name for the same record type
               */
               temp_cache_ptr = rr_list_ptr->RR_HEAD_PTR;
               while ( temp_cache_ptr != NULL ) {
                  if ( temp_cache_ptr->TYPE == rr_head_ptr->TYPE ) {
                  /* cached data of this type exists, replace with new stuff */
                     break;
                  } /* Endif */
                  previous_cache_ptr = temp_cache_ptr;
                  temp_cache_ptr     = temp_cache_ptr->NEXT;
               } /* Endwhile */

               if ( temp_cache_ptr == NULL ) {
                  /* data of this type doesn't exist. Insert new stuff */
                  rr_head_ptr->NEXT        = rr_list_ptr->RR_HEAD_PTR;
               } else {
                  /* This data exists previously, replace it */
                  rr_head_ptr->NEXT = rr_list_ptr->RR_HEAD_PTR->NEXT;

                  if ( previous_cache_ptr == NULL ) {
                     /* We're at the head of the list */
                     DNS_free_resource_record( rr_list_ptr->RR_HEAD_PTR );
                  } else {
                     previous_cache_ptr->NEXT = temp_cache_ptr->NEXT;
                     DNS_free_resource_record( temp_cache_ptr );
                  } /* Endif */
               } /* Endif */
               rr_list_ptr->RR_HEAD_PTR = rr_head_ptr;
               /*
               ** We only keep one reference to the name for the resource
               ** records for any given name.  Since a node was pre-existing,
               ** just have this record refer to it's string, and free the
               ** one we allocated.
               */
               _mem_free(rr_head_ptr->DOMAIN_NAME_PTR);
               rr_head_ptr->DOMAIN_NAME_PTR = rr_list_ptr->NAME_PTR;
            } /* Endif */
            RTCS_mutex_unlock(&control_ptr->CACHE_MUTEX);
            temp_ptr = temp_ptr + answer_length;
            answer_head_ptr = temp_ptr;
            answer_middle_ptr = DNS_assign_answer_middle_ptr( answer_head_ptr,
                                                              num_answers, i );
         } /* Endfor */

         while(DNS_CACHE_NAME_LIMIT &&
            (control_ptr->NUM_NAMES_CACHED > DNS_CACHE_NAME_LIMIT) &&
            /* This is here as a paranoid check to break out of loop. */
            control_ptr->CACHE_PTR)
         {
            /*
            ** We're over the limit, delete the last entry in the cache until
            ** we're at the limit.
            */
            DNS_remove_last_entry_from_cache(control_ptr);
         }/* Endwhile */
      } /* Endif */
   } /* Endif */
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_retry_query_with_CNAME()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_retry_query_with_CNAME
   (
   DNS_CONTROL_STRUCT         _PTR_ control_ptr,
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr,
   DNS_RESPONSE_RR_MIDDLE_STRUCT  _PTR_ answer_ptr
   )

{ /* Body */
   DNS_CNAME_STRUCT  _PTR_ _PTR_  previous_ptr;
   DNS_CNAME_STRUCT  _PTR_  temp_ptr;
   DNS_CNAME_STRUCT  _PTR_  new_cname_ptr;
   uchar             _PTR_  cname_ptr;
   int_32                   error;
   uint_32                  cname_retries = 0;
   int_32                   cmp_result;

   cname_ptr = (uchar _PTR_)answer_ptr;
   cname_ptr += sizeof( DNS_RESPONSE_RR_MIDDLE_STRUCT );

   new_cname_ptr = (DNS_CNAME_STRUCT _PTR_)
      RTCS_part_alloc_zero(control_ptr->CNAME_PARTID);
   if ( !new_cname_ptr ) {
      RTCS_log_error(ERROR_DNS, DNS_SERVER_FAILURE, 5, 0, 0);
      return( DNS_SERVER_FAILURE );
   }/* Endif */
   error = DNS_is_valid_domain_name( control_ptr->RES_BUFFER, cname_ptr,
                           (uchar _PTR_ _PTR_)&new_cname_ptr->CNAME_PTR );
   if ( error != DNS_OK ) {
      DNS_send_error_response( control_ptr->RES_BUFFER,
                               query_ptr, DNS_NAME_ERROR,
                               control_ptr->RES_SOCKET, DNS_OK );
   } /* Endif */

   previous_ptr = &query_ptr->CNAME;

   temp_ptr = *previous_ptr;

   while ( temp_ptr != NULL ) {
      cmp_result = strcmp((char _PTR_)new_cname_ptr->CNAME_PTR,
         (char _PTR_)temp_ptr->CNAME_PTR );
      if ( cmp_result == 0 ) {
         /* the names are equal, we have a CNAME loop, return error */
         _mem_free(new_cname_ptr->CNAME_PTR);
         RTCS_part_free(new_cname_ptr);
         DNS_send_error_response( control_ptr->RES_BUFFER,
                                  query_ptr, DNS_NAME_ERROR,
                                  control_ptr->RES_SOCKET, DNS_OK );
         error = DNS_NAME_ERROR;
         break;
      } /* Endif */
      cname_retries++;
      if ( cname_retries == DNS_MAX_NUM_CNAME_RETRIES ) {
         /* BIND only allows for 8 CNAME query retries */
         _mem_free(new_cname_ptr->CNAME_PTR);
         RTCS_part_free(new_cname_ptr);
         error = DNS_return_UDP_response_to_query( query_ptr,
                                                   control_ptr->RES_BUFFER,
                                                   control_ptr->RES_BUFFER_SIZE,
                                                   control_ptr->RES_SOCKET );
         error = DNS_NAME_ERROR;
         break;
      } /* Endif */
      previous_ptr = &(*previous_ptr)->NEXT;
      temp_ptr     = *previous_ptr;
   } /* Endwhile */

   if ( error == DNS_OK ) {
      *previous_ptr  = new_cname_ptr;
      new_cname_ptr->NEXT = NULL;
      query_ptr->QUERY_ID  = DNS_get_query_id();

      error = DNS_Send_UDP_Query( control_ptr, query_ptr, TRUE );
      if ( error == RTCS_ERROR ) {
         DNS_send_error_response( control_ptr->RES_BUFFER, query_ptr,
                                  DNS_SERVER_FAILURE, control_ptr->RES_SOCKET,
                                  error );
      } else {

      DNS_add_query_to_list( &control_ptr->RES_QUERY_HEAD_PTR, query_ptr );

      } /* Endif */
   } /* Endif */

   return( error );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_search_response_for_referral()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_search_response_for_referral
   (
   DNS_CONTROL_STRUCT         _PTR_ control_ptr,
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr
   )

{ /* Body */

   /* NOT IMPLEMENTED */

   /* SPR P122-0258-01
   ** This is here to get the resolver to return quickly when a name
   ** that doesn't exist is queried for, since we only support queries
   ** to a single DNS Server for now. When multiple DNS Server support
   ** is added, remove this function call and allow for proper resend
   ** behaviour.
   */

   DNS_send_error_response( control_ptr->RES_BUFFER,
                            query_ptr, DNS_QUERY_NOT_IMPLEMENTED,
                            control_ptr->RES_SOCKET, DNS_OK );
   /* END SPR P122-0258-01 */

   return( DNS_OK );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_handle_internal_query()
* Returned Value  : DNS_INTERNAL_QUERY_STRUCT_PTR
* Comments        : This function is for the Name Server or Resolver to
*                   make queries for NS records to be used to make queries
*                   for authoritative data if the original query fails.
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_handle_internal_query
   (
   DNS_CONTROL_STRUCT         _PTR_ control_ptr,
   DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr
   )

{ /* Body */
   return( DNS_OK );
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_Send_TCP_Query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/



 static uint_32 DNS_Send_TCP_Query
    (
    DNS_CONTROL_STRUCT         _PTR_ control_ptr,
    DNS_INTERNAL_QUERY_STRUCT _PTR_ query_ptr
    )

{ /* Body */

   /* NOT IMPLEMENTED */
   DNS_send_error_response( control_ptr->RES_BUFFER,
                            query_ptr, DNS_QUERY_NOT_IMPLEMENTED,
                            control_ptr->RES_SOCKET, DNS_OK );

   return( DNS_OK );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_handle_UDP_response()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

 static uint_32 DNS_handle_UDP_response
   (
   DNS_CONTROL_STRUCT   _PTR_ control_ptr
   )

{ /* Body */

   DNS_MESSAGE_HEADER_STRUCT     _PTR_  message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT       _PTR_  message_tail_ptr;
   DNS_INTERNAL_QUERY_STRUCT     _PTR_  query_ptr;
   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_  answer_middle_ptr;
   uchar                         _PTR_  answer_ptr;
   TIME_STRUCT                          time;
   uint_16                              answer_count;
   uint_16                              control;
   uint_16                              answer_type;
   int_32                               error = DNS_OK;

   _time_get( &time );
   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)control_ptr->RES_BUFFER;
   answer_count     = ntohs( message_head_ptr->ANCOUNT );
   control          = ntohs( message_head_ptr->CONTROL );
   query_ptr        = DNS_retrieve_query( ntohs(message_head_ptr->ID),
                                           &control_ptr->RES_QUERY_HEAD_PTR );
   if ( query_ptr != NULL ) {
      /* Query still exists, respond accordingly */

 /* Start CR 1965 */
         if ( query_ptr->INTERNAL_QUERY == TRUE ) {
            /* Name Server query was made by resolver, not for a client */
            error = DNS_handle_internal_query( control_ptr, query_ptr );
            if ( error != DNS_OK ) {
               DNS_send_error_response( control_ptr->RES_BUFFER,
                                        query_ptr, DNS_SERVER_FAILURE,
                                        control_ptr->RES_SOCKET, error );
            } /* Endif */
         } else {
            if ( answer_count != 0 ) {
               /* An answer was received, respond accordingly */
               /*
               ** Cache the response, so we don't have to request whatever was
               ** given to us in the future.
               */
               DNS_add_to_cache( control_ptr, DNS_RES_CACHE_DATA );
               DNS_assign_answer_pointers( control_ptr->RES_BUFFER, &answer_ptr,
                                           &answer_middle_ptr );
               if ( answer_middle_ptr ) {
                  message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT _PTR_)(answer_ptr
                     - sizeof(DNS_MESSAGE_TAIL_STRUCT));
                  answer_type = ntohs(answer_middle_ptr->TYPE);

                  if ( answer_type == ntohs(message_tail_ptr->QTYPE) ) {
                     /* Answer is what we wanted as a response, return to client */
                     error = DNS_return_UDP_response_to_query( query_ptr,
                        control_ptr->RES_BUFFER, control_ptr->RES_BUFFER_SIZE,
                        control_ptr->RES_SOCKET );
                  } else {
                     /*
                     ** An answer exists, but it's not what we want.  Check all
                     ** the answers to see if what we are looking for is in the
                     ** buffer at all.  If it is, return the buffer to the client,
                     ** if not, use the information received to make another query
                     */
                     if ( answer_type == DNS_CNAME ) {
                        if ( query_ptr->QUERY_RETRIES-- <= 0 ) {
                           DNS_send_error_response( control_ptr->RES_BUFFER,
                              query_ptr, DNS_QUERY_REFUSED,
                              control_ptr->RES_SOCKET, DNS_OK );
                        } else {
                           /*
                           ** If we get here, an answer exists, but doesn't
                           ** match query type, or there is more than one
                           ** answer, since we've cached the response, check
                           ** the cache again to see if the answer was in the
                           ** packet we received.  If it isn't there, retry the
                           ** query using the cannonical name reference.
                           */
                           if (DNS_rr_data_exists( query_ptr, control_ptr,
                               control_ptr->RES_BUFFER,
                               &control_ptr->RES_BUFFER_SIZE, DNS_RES_CACHE_DATA))
                           {
                              error = DNS_return_UDP_response_to_query(
                                 query_ptr, control_ptr->RES_BUFFER,
                                 control_ptr->RES_BUFFER_SIZE,
                                 control_ptr->RES_SOCKET );
                           } else {
                              error = DNS_retry_query_with_CNAME( control_ptr,
                                 query_ptr, answer_middle_ptr);
                           } /* Endif */
                        } /* Endif */
                     } /* Endif */
                  } /* Endif */
               } /* Endif */
            } else {
               /*
               ** No answer exists in the response.  Check Name Server records
               ** for referrals to retry query.  Also check additional records.
               ** and RCODE for clues to retry the query.
               */
               error = DNS_search_response_for_referral( control_ptr,
                  query_ptr );

               if ( control & DNS_TRUNCATION ) {
                  /* If the message was truncated, retry the query using TCP */
                  error = DNS_Send_TCP_Query( control_ptr, query_ptr );
            } /* Endif */
         } /* Endif */
      } /* Endif */
/* End CR 1965 */
/* Start CR 1119 */
#if 0
   } else {
      /*
      ** Query doesn't exist anymore.  Cache if answer is authoritative,
      ** otherwise, do nothing
      */
      if ( ((control & DNS_RCODE_MASK) == DNS_NO_ERROR ||
            (control & DNS_RCODE_MASK) == DNS_NAME_ERROR) &&
            (control & DNS_AUTHORITATIVE_ANSWER) ) {
         DNS_add_to_cache( control_ptr, DNS_RES_CACHE_DATA );
      } /* Endif */
#endif
/* End CR 1119 */
   } /* Endif */

   return( error );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_refresh_root_servers_task()
* Returned Value  :
* Comments        :
*
*END*-----------------------------------------------------------------*/

static void DNS_refresh_root_servers_task
   (
   uint_32 temp_ptr
   )
{ /* Body */

  /*NOT IMPLEMENTED */
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_refresh_root_servers()
* Returned Value  :
* Comments        :
*
*END*-----------------------------------------------------------------*/

 static uint_32 DNS_refresh_root_servers
   (
   DNS_CONTROL_STRUCT _PTR_ control_ptr
   )
{ /* Body */

  /* NOT IMPLEMENTED */
  return( DNS_OK );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_Resolver_task()
* Returned Value  :
* Comments        :
*
*END*-----------------------------------------------------------------*/

void DNS_Resolver_task
   (
   pointer new_control_ptr,
   pointer creator
   )
{ /* Body */

   DNS_CONTROL_STRUCT         _PTR_  control_ptr;
   DNS_MESSAGE_HEADER_STRUCT _PTR_  message_head_ptr;
   DNS_SLIST_STRUCT          _PTR_  first_root_ptr;
   sockaddr_in                      addr;
   TIME_STRUCT                      time;
   uint_32                          difference;
   uint_16                          rlen;
   int_32                           error;

   control_ptr = (DNS_CONTROL_STRUCT _PTR_)new_control_ptr;

   /*
   ** Root Server List updating is not yet implemented,
   ** remove "control_ptr->UPDATING = TRUE;" when it is coded.
   */
   control_ptr->UPDATING = TRUE;

   _mem_zero((uchar _PTR_) &addr, sizeof(addr));

   addr.sin_family      = AF_INET;
   addr.sin_port        = DNS_RESOLVER_PORT;
   addr.sin_addr.s_addr = INADDR_ANY;

   error = RTCS_attachsock( control_ptr->RES_SOCKET );
   if ( error == (int_32)RTCS_SOCKET_ERROR ) {
      RTCS_task_exit(creator, RTCSERR_DNS_INITIALIZATION_FAILURE);
   } /* Endif */

   error = bind(control_ptr->RES_SOCKET, &addr, sizeof(sockaddr_in));
   if ( error != RTCS_OK ) {
      RTCS_task_exit(creator, RTCSERR_DNS_INITIALIZATION_FAILURE);
   } /* Endif */
   RTCS_task_resume_creator(creator, RTCS_OK);

   while ( TRUE ) {

      _time_get ( &time );

      /*
      ** Check the TTL expiry time for root name servers, if expired
      ** call refresh function to update them
      */
      first_root_ptr = (DNS_SLIST_STRUCT _PTR_)control_ptr->ROOT_SLIST_PTR;
      difference = time.SECONDS - first_root_ptr->LAST_TTL_CHECK;
      first_root_ptr->LAST_TTL_CHECK = time.SECONDS;

      RTCS_mutex_lock(&control_ptr->CACHE_MUTEX);
      if (first_root_ptr->TTL <= difference
          && ( control_ptr->UPDATING == FALSE )) {
         RTCS_mutex_unlock(&control_ptr->CACHE_MUTEX);
         error = DNS_refresh_root_servers( control_ptr );
         if ( error != DNS_OK ) {
            RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_UPDATE_ROOT_SERVERS, 0, 0, 0);
         } /* Endif */
      } else {
         RTCS_mutex_unlock(&control_ptr->CACHE_MUTEX);
         first_root_ptr->TTL = first_root_ptr->TTL - difference;
      } /* Endif */

      DNS_check_outstanding_queries( control_ptr );

      if ( control_ptr->NS_BUFFER == NULL ) {
         /*
         ** If the Name Server task is running, it has the responsibility
         ** to update the cache(using the mutex!), if it isn't, let the
         ** resolver do it.
         */
         /*
         ** Don't need the mutex here, since the DNS Server doesn't exist on
         ** the target, the resolver will be the only task accessing it.
         */
         DNS_update_cache(control_ptr);
      } /* Endif */

      /*
      ** now wait for queries or responses, bypass rest of while loop
      ** if none are received
      */

      error = (uint_32)RTCS_selectset( &control_ptr->RES_SOCKET, 1,
                                       DNS_RESOLVER_RETRY_TIME );

      if (( error == (uint_32)NULL ) || ( error == RTCS_ERROR )) {
         continue;
      } /* Endif */


      /* Received a message, respond accordingly */
      rlen = sizeof(addr);
      control_ptr->RES_BUFFER_SIZE = recvfrom(control_ptr->RES_SOCKET,
                                             control_ptr->RES_BUFFER,
                                             DNS_MAX_UDP_MESSAGE_SIZE,
                                             0, &addr, &rlen);
      if (control_ptr->RES_BUFFER_SIZE == RTCS_ERROR) {
         RTCS_log_error(ERROR_DNS, RTCSERR_DNS_SOCKET_RECV_ERROR, 0, 0, 0);
         continue;
      } /* Endif */

/* Start CR 1119 */
      if (control_ptr->RES_BUFFER_SIZE < sizeof(DNS_MESSAGE_HEADER_STRUCT)) {
         RTCS_log_error(ERROR_DNS, RTCSERR_DNS_SHORT_MESSAGE_ERROR, 0, 0, 0);
         continue;
      } /* Endif */
/* End CR 1119 */

      message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT_PTR)control_ptr->RES_BUFFER;

      if ( ntohs(message_head_ptr->CONTROL) & DNS_RESPONSE ) {
         /* It's a response to a query, parse and return it to client */
         error = DNS_handle_UDP_response( control_ptr );
         if ( error != DNS_OK ) {
            RTCS_log_error( ERROR_DNS, error, 0, 0, 0);
         } /* Endif */
      } else {
         if (addr.sin_addr.s_addr == INADDR_LOOPBACK) {
            /* It's a new query */
            error = DNS_handle_new_query( control_ptr, &addr );
         } else {
            error = ERROR_DNS;
         } /* Endif */
         if ( error != DNS_OK ) {
            RTCS_log_error( ERROR_DNS, error, 0, 0, 0);
         } /* Endif */
      } /* Endif */
   } /* Endwhile */

} /* Endbody */

#endif



#if RTCSCFG_ENABLE_LWDNS

_ip_address LWDNS_server_ipaddr = 0;

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : LWDNS_Send_Query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/


static uint_32 LWDNS_Send_Query
   (
   uint_32     sock,
   char_ptr    name_to_resolve,
   char_ptr    send_buffer,
   uint_32_ptr id_ptr
   )
{
   DNS_MESSAGE_HEADER_STRUCT_PTR    message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT_PTR      message_tail_ptr;
   int_32                           error;
   uint_32                          i;
   uint_32                          name_size;
   char_ptr                         name_ptr,len_ptr;
   uint_16                          id;
   uchar                            len;


   name_size = strlen((char _PTR_)name_to_resolve ) ; //To include the terminating NULL char

   /* set up buffer for sending query.   */
   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)send_buffer;
   name_ptr  = &send_buffer[sizeof(DNS_MESSAGE_HEADER_STRUCT)];
   id = RTCS_rand(); // TASK_NUMBER_FROM_TASKID(_task_get_id()) ;
   if (id_ptr) {
      *id_ptr = id;
   }
   htons(message_head_ptr->ID, id );
   htons(message_head_ptr->CONTROL, DNS_STANDARD_QUERY|DNS_RECURSION_DESIRED);
   htons(message_head_ptr->QDCOUNT, DNS_SINGLE_QUERY);
   htons(message_head_ptr->ANCOUNT, 0);
   htons(message_head_ptr->NSCOUNT, 0);
   htons(message_head_ptr->ARCOUNT, 0);

   len_ptr=name_ptr;
   len=0;
   if (*name_ptr != '.')
      name_ptr++;

   for (i=0;i<name_size;i++) {
      if (name_to_resolve[i] == '.') {
         *len_ptr = len;
         len = 0;
         len_ptr = name_ptr;
      } else {
         htonc(name_ptr, name_to_resolve[i]);
         len++;
      }
     name_ptr++;
   }
   *len_ptr = len;
    *name_ptr++ = 0;

   message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT _PTR_) name_ptr;
   name_ptr += sizeof(DNS_MESSAGE_TAIL_STRUCT);

   htons(message_tail_ptr->QTYPE, DNS_A);
   htons(message_tail_ptr->QCLASS, DNS_IN);


   error = send(sock, send_buffer, (name_ptr-send_buffer), 0);

   if (error == RTCS_ERROR) {
      error = RTCS_geterror(sock);
   } else {
      error = DNS_OK;
   }

   return( error );
}

static boolean domains_equal(char_ptr req_domain, char_ptr res_domain)
{
   uint_32  req_index=0,res_index=0,i;
   uchar    len, c;

   len = ntohc (res_domain + res_index);
   res_index++;
   do {
      if ( len & DNS_COMPRESSED_NAME_MASK ) {
         return FALSE;
      }
      for (i=0;i<len;i++) {
         if (req_domain[req_index++] != tolower(res_domain[res_index++])) {
            return FALSE;
         }
      }
      len = ntohc (res_domain + res_index);
      res_index++;
      c = ntohc (req_domain + req_index);
      req_index++;
   } while (len && (c == '.'));
   return (len ==0) && (c==0);
}

static boolean domain_name_extract(uint_32 start, char_ptr rx_buffer,char_ptr ipname,uint_32 ipnamesize)
{
   uint_32 s_index=start, d_index=0,i;
   uchar   b;

   do {
      b = ntohc (rx_buffer + s_index);
      if ( b & DNS_COMPRESSED_NAME_MASK ) {
          s_index = ntohs(rx_buffer + s_index) & DNS_COMPRESSED_LOCATION_MASK;
      } else {
         s_index++;
         for (i=0;i<b;i++) {
            if (d_index >= ipnamesize-1) break;
            ipname[d_index++] = rx_buffer[s_index++];
         }
         if (d_index >= ipnamesize-1) break;
         ipname[d_index++] = '.';
      }
   } while (rx_buffer[s_index]);

   if (d_index) {
      ipname[--d_index] = 0;
   } else {
      ipname[0] = 0;
   }

   return TRUE;
}

static uint_32 domain_name_fragment_length(uchar_ptr name)
{
   uint_32 i=0;

   while ( (name[i] != 0) && ((ntohc (name + i) & DNS_COMPRESSED_NAME_MASK) == 0)) {
      i++;
   }
   return (name[i]?i+2:i+1);
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : LWDNS_Get_Response()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

 static boolean LWDNS_Get_Response
   (
   uint_32           sock,
   char_ptr          name_to_resolve,
   char_ptr          rx_buffer,
   uint_32           req_id,
   _ip_address _PTR_ ipaddr_ptr,
   char_ptr          ipname,
   uint_32           ipnamesize
   )
{
   DNS_MESSAGE_HEADER_STRUCT_PTR       message_head_ptr;
   DNS_RESPONSE_RR_MIDDLE_STRUCT_PTR   answer_middle_ptr;
   uchar_ptr                           answer_ptr;
   uint_16                             answer_count,query_count;
   uint_16                             control, rdlength;
   uint_16                             answer_type;
   _ip_address                         ip_addr = INADDR_ANY;
   sockaddr_in                         addr;
   uint_16                             rlen,id;
   int_32                              response_size;
   char_ptr                           temp_ptr;
   _ip_address *                       ip_answer_ptr;
   uint_32                             i,offset;



   rlen = sizeof(addr);
   response_size = recv(sock,rx_buffer, DNS_MAX_UDP_MESSAGE_SIZE, 0);
   if ((response_size != RTCS_ERROR)   && (response_size >= sizeof(DNS_MESSAGE_HEADER_STRUCT))) {
      message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT_PTR) rx_buffer;
      id           = ntohs( message_head_ptr->ID );
      query_count  = ntohs( message_head_ptr->QDCOUNT );
      answer_count = ntohs( message_head_ptr->ANCOUNT );
      control      = ntohs( message_head_ptr->CONTROL );

      if ( (id == req_id ) && (query_count==1) && answer_count) {

         temp_ptr = &rx_buffer[sizeof(DNS_MESSAGE_HEADER_STRUCT)];


         if (domains_equal(name_to_resolve, temp_ptr)) {
            temp_ptr +=strlen(temp_ptr) + 1 + sizeof(DNS_MESSAGE_TAIL_STRUCT);

            for (i=0;i<answer_count;i++) {
               answer_ptr =  (uchar_ptr)temp_ptr;

               offset = domain_name_fragment_length(answer_ptr);

               answer_middle_ptr = (DNS_RESPONSE_RR_MIDDLE_STRUCT_PTR) &answer_ptr[offset];
               answer_type = ntohs( answer_middle_ptr->TYPE );
               rdlength = ntohs( answer_middle_ptr->RDLENGTH );
               if ( (answer_type == DNS_A ) && (rdlength == 4)) {
                  if (ipname != NULL) {
                     domain_name_extract(temp_ptr-rx_buffer,rx_buffer,ipname,ipnamesize);
                  }
                  ip_answer_ptr = (_ip_address *)  &answer_ptr[offset  + sizeof(DNS_RESPONSE_RR_MIDDLE_STRUCT)];
                  if (ipaddr_ptr != NULL) {
                     *ipaddr_ptr = ntohl ((uchar_ptr)ip_answer_ptr);
                  }
                  return TRUE;
               }


               temp_ptr += offset + sizeof(DNS_RESPONSE_RR_MIDDLE_STRUCT) + rdlength;
            }
         }
      }
   }

   return FALSE;
}



boolean LWDNS_resolve(
   char_ptr          name_to_resolve,
   _ip_address _PTR_ ipaddr_ptr,
   char_ptr          ipname,
   uint_32           ipnamesize
   )
{
   boolean           success=FALSE;
   sockaddr_in       laddr,raddr;
   uint_32           sock=0;
   char_ptr          buf=NULL;
   uint_32           result,id;
   int_32            error;

   *ipaddr_ptr = 0;
   if (LWDNS_server_ipaddr) {
      buf= RTCS_mem_alloc(DNS_MAX_UDP_MESSAGE_SIZE);
      if (buf) {
         _mem_set_type(buf,MEM_TYPE_DNS_UDP_MESSAGE);
         _mem_zero((uchar _PTR_) &laddr, sizeof(laddr));
         laddr.sin_family      = AF_INET;
         laddr.sin_port        = 0; // DNS_RESOLVER_PORT;
         laddr.sin_addr.s_addr = INADDR_ANY;

         _mem_zero((uchar _PTR_) &raddr, sizeof(raddr));
         /* Remote address, Name Servers use port 53 */
         raddr.sin_family      = AF_INET;
         raddr.sin_port        = DNS_NAMESERVER_PORT;
         raddr.sin_addr.s_addr = LWDNS_server_ipaddr;

#if RTCSCFG_ENABLE_UDP
         /* Bind to UDP port */
         sock = socket(PF_INET, SOCK_DGRAM, 0);
#else
         sock = socket(PF_INET, SOCK_STREAM, 0);
#endif
         if (sock) {
            error = bind(sock, (const sockaddr *)&laddr, sizeof(laddr));
            if ( error == RTCS_OK ) {
               error = connect(sock, (const sockaddr *)(&raddr), sizeof(raddr));
               if (error == RTCS_OK ) {
                  error = LWDNS_Send_Query(sock,  name_to_resolve, buf, &id);
                  if (error == RTCS_OK ) {
                     result = (uint_32)RTCS_selectset( &sock, 1, 3000 );

                     if (result==sock) {
                        success = LWDNS_Get_Response(sock, name_to_resolve, buf, id, ipaddr_ptr, ipname, ipnamesize);
                     }
                  }
               }
            }
            shutdown(sock,0);
         }
         _mem_free(buf);
      }
   }

   return success;
}

#endif
/* EOF */
