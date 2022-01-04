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
* $FileName: dnsutil.c$
* $Version : 3.5.14.0$
* $Date    : Jan-20-2010$
*
* Comments:
*
*   This file contains the functions used exclusively by the
*   user interface functions RTCS_gethostbyname,
*   DNS_gethostbyname, and DNS_gethostbyaddr for RTCS.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_UDP
#if RTCSCFG_ENABLE_DNS

#include <ctype.h>
#include <string.h>
#include <fio.h>
#include "dns.h"

/*

GCC compiler did not have ctype in libc.
Used these definitions to resolve the problem:

#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c)-'A'+'a' : (c))
#define isalpha(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define isdigit(c) ( (c) >= '0' && (c) <= '9' )
#define isalnum(c) (isalpha(c) || isdigit(c))

*/

/* Global variable used for gethostbyaddr and gethostbyname calls */
static INTERNAL_HOSTENT_STRUCT RTCS_HOST;
static uchar RTCS_HOST_NAMES[DNS_MAX_NAMES][DNS_MAX_CHARS_IN_NAME];
static _ip_address RTCS_HOST_ADDRS[DNS_MAX_ADDRS];

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_is_dotted_IP_addr()
* Returned Value  : char _PTR_
* Comments        :
*
*END*-----------------------------------------------------------------*/

uint_32 DNS_is_dotted_IP_addr
   (
   uchar _PTR_ name,
   uchar _PTR_ fill_buffer_ptr
   )

{  /* Body */

   uint_32     i;
   uint_32     j = 0;
   uint_32     k;
   uint_32     size = 0;
   uint_32     digit_count = 0;
   uint_32     dot_count = 0;
   int_32      byte_num;
   uint_32     addr_number = 0;
   uchar       number_chars[4];
   uchar       actual_number;
   uchar       addr[4][3];
   uchar _PTR_ number_buffer_ptr;
   uchar       appended_name[12] = {"in-addr.arpa"};
   boolean     leading_zero;

   /*
   ** first check to see if name is written in correct dotted decimal
   ** IP address format
   */
   for (i=0; name[i] != '\0'; ++i ) {
      if ( name[i] == '.' ) {
         dot_count++;

         if ( dot_count > 3 ) {
            /* invalid IP address */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */

         if ( digit_count == 0 ) {
            /* there are no digits before the '.' */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */

         byte_num = byte_num / 10; /* shift back */

         if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         } else {
            addr_number++;
            number_chars[j] = digit_count;
            j++;
            digit_count = 0;
         } /* Endif */
      } else { /* a digit */

         if ( digit_count == 0 ) {
            byte_num = 0;
         }/* Endif */
         ++digit_count;

         if ( digit_count > 3 ) {
            /* too many digits between the '.' */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */

         if ( isdigit(name[i]) ) {
            /* number is in range */
            addr[addr_number][digit_count - 1] = name[i];
            byte_num = byte_num + name[i] - '0';
            byte_num = byte_num * 10;
         } else {
            /* if the characters are not decimal digits it's invalid */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */
      }/* Endif */
   } /* Endfor */

   if ( digit_count == 0 ) {
      /* there are no digits after the last '.' */
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   } /* Endif */

   byte_num = byte_num / 10;
   if ( (byte_num < 0 ) || (byte_num > 255) ) {
      /* if the number does fall within this range it's invalid */
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   } else {
      number_chars[j] = digit_count;
   }  /* Endif */

   if ( dot_count != 3 ) {
      /* the wrong number of dots were found */
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   }/* Endif */

   /*
   ** If we got this far, it's a legal IP addr, reverse the numbers,
   ** in the fill_buffer and append "in-addr.arpa" to it.
   */
   for ( i = 0; i < 4; i++) {

      leading_zero       = FALSE;
      number_buffer_ptr  = fill_buffer_ptr;
      actual_number      = number_chars[j];
      htonc(fill_buffer_ptr, number_chars[j]);
      fill_buffer_ptr++;
      size++;

      for ( k = 0; k < number_chars[j]; k++ ) {
         if ( addr[addr_number][k] == '0' ) {
            if ( addr[addr_number][k] == '0' && k == 0 ) {
               leading_zero = TRUE;
               actual_number--;
               continue;
            } /* Endif */

            if ( addr[addr_number][k] == '0' && k == 1
                 && leading_zero == TRUE && k < number_chars[j] ) {
               actual_number--;
               continue;
            } /* Endif */

            if ( addr[addr_number][k] == '0' && k == 1
                 && leading_zero == TRUE && k == number_chars[j] ) {

               htonc(fill_buffer_ptr, addr[addr_number][k]);
               fill_buffer_ptr++;
               htonc(number_buffer_ptr, actual_number);
               number_buffer_ptr++;
               size++;
               break;
            }/* Endif */
         } /* Endif */

         htonc(fill_buffer_ptr, addr[addr_number][k]);
         fill_buffer_ptr++;
         size++;
      } /* Endfor */

      addr_number--;
      j--;
   } /* Endfor */

   htonc(fill_buffer_ptr, 7);
   fill_buffer_ptr++;

   for ( i = 0; i < 7;  i++) {
      htonc(fill_buffer_ptr, appended_name[i]);
      fill_buffer_ptr++;
   } /* Endfor */

   htonc(fill_buffer_ptr, 4);
   fill_buffer_ptr++;
   i++;

   for ( ; i < 12;  i++) {
      htonc(fill_buffer_ptr, appended_name[i]);
      fill_buffer_ptr++;
   } /* Endfor */

   /*
   ** Need to append '\0' to terminate a domain name properly.
   ** This signifies the ROOT domain
   */
   htonc(fill_buffer_ptr, '\0');

   return( DNS_OK );

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_insert_IP_query()
* Returned Value  : uint_32
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_insert_IP_query
   (
   uchar _PTR_ name,
   uchar _PTR_ fill_buffer_ptr
   )
{ /* Body */
   uchar my_buff[DNS_MAX_IP_STRING_SIZE];
   sprintf((char _PTR_)my_buff,"%ld.%ld.%ld.%ld", (uint_32)name[0],
      (uint_32)name[1], (uint_32)name[2], (uint_32)name[3]);
   return(DNS_is_dotted_IP_addr(my_buff, fill_buffer_ptr));
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_append_local_name()
* Returned Value  : char _PTR_
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint_32 DNS_append_local_name
   (
   uchar   _PTR_ qname_buffer,
   uint_32       total_length
   )

{  /* Body */

   uint_32       local_name_length = 0;
   uint_32       error;
   uchar   _PTR_ label_length_ptr;
   uchar         label_length = 0;


   if ( DNS_Local_network_name[local_name_length] == '\0' ) {
      return( RTCSERR_DNS_INVALID_LOCAL_NAME );
   }/* Endif */

   /* If the Local Network is the ROOT Domain Name */
   if ( ( DNS_Local_network_name[local_name_length] == '.' )
        && ( DNS_Local_network_name[local_name_length + 1] == '\0' )) {
      htonc((qname_buffer + total_length), '\0');
      return( DNS_OK );
   }/* Endif */

   label_length_ptr = qname_buffer + total_length;
   error            = DNS_OK;

   while ( DNS_Local_network_name[local_name_length] ) {

      /*
      ** RFC 1035 says labels must start with a letter, but in practice
      ** some aren't
      */
      if ( isalnum(DNS_Local_network_name[local_name_length]) ) {
         htonc((qname_buffer + total_length + 1),
               DNS_Local_network_name[local_name_length]);
         total_length++;
         local_name_length++;
         label_length++;
      } else {
         error = RTCSERR_DNS_INVALID_LOCAL_NAME;
         break;
      }  /* Endif */

      while ( ( DNS_Local_network_name[local_name_length] != '.' ) &&
              ( DNS_Local_network_name[local_name_length] != '\0' ) ) {

         /* Domain Name characters can only be letters, hyphens, or numbers */
         if ( (isalnum(DNS_Local_network_name[local_name_length]) ||
               DNS_Local_network_name[local_name_length] == '-'  ) &&
              total_length < DNS_MAX_CHARS_IN_NAME  &&
              label_length <= DNS_MAX_LABEL_LENGTH ) {
            htonc((qname_buffer + total_length + 1),
                  DNS_Local_network_name[local_name_length]);
            total_length++;
            local_name_length++;
            label_length++;
         } else {
            error = RTCSERR_DNS_INVALID_LOCAL_NAME;
            break;
         } /* Endif */
      } /* Endwhile */

      if ( error == RTCSERR_DNS_INVALID_LOCAL_NAME ) {
         break;
      }/* Endif */

      htonc(label_length_ptr, label_length);
      label_length = 0;
      total_length++;
      label_length_ptr = qname_buffer + total_length;
      local_name_length++;
   } /* Endwhile */

   if ( error == DNS_OK ) {
      /*
      ** Need to append '\0' to terminate domain name properly.
      ** This signifies the ROOT domain
      */
      htonc(label_length_ptr, '\0');
   }/* Endif */

   return( error );

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_is_dotted_domain_name()
* Returned Value  : uint_32
* Comments        : Checks the DNS Domain Name passed to it to see if the
*                   proper naming conventions have been followed.
*
*END*-----------------------------------------------------------------*/

uint_32 DNS_is_dotted_domain_name
   (
   uchar _PTR_ domain_name,
   uchar _PTR_ qname_buffer
   )
{  /* Body */
   uint_32     total_length = 0;
   uint_32     error;
   uchar _PTR_ label_length_ptr;
   uchar       label_length = 0;


   if ( domain_name[total_length] == '\0' ) {
      return( RTCSERR_DNS_INVALID_NAME );
   }/* Endif */

   /* If the request is for the ROOT Domain Name */
   if ( ( domain_name[total_length] == '.' )
        && ( domain_name[total_length + 1] == '\0' )) {
      htonc(qname_buffer, '\0');
      return( DNS_OK );
   }/* Endif */

   label_length_ptr = qname_buffer;
   error            = DNS_OK;

   while ( domain_name[total_length] != '\0' ) {

      /*
      ** RFC 1035 says labels must start with a letter, but in practice
      ** some aren't
      */
      if ( isalnum(domain_name[total_length]) ) {
         htonc((qname_buffer + total_length + 1), domain_name[total_length]);
         total_length++;
         label_length++;
      } else {
         error = RTCSERR_DNS_INVALID_NAME;
         break;
      }  /* Endif */

      while ( ( domain_name[total_length] != '.') &&
              ( domain_name[total_length] != '\0' ) ) {

         /* Domain Name characters can only be letters, hyphens, or numbers */
         if ( (isalnum(domain_name[total_length]) ||
               domain_name[total_length] == '-'  ) &&
              total_length < DNS_MAX_CHARS_IN_NAME &&
              label_length <= DNS_MAX_LABEL_LENGTH ) {
            htonc((qname_buffer + total_length + 1), domain_name[total_length]);
            total_length++;
            label_length++;
         } else {
            error = RTCSERR_DNS_INVALID_NAME;
            break;
         } /* Endif */
      } /* Endwhile */

      if ( error == RTCSERR_DNS_INVALID_NAME ) {
         break;
      }/* Endif */

      htonc(label_length_ptr, label_length);
      label_length = 0;


      if ( domain_name[total_length] != '\0' ) {
         label_length_ptr = qname_buffer + total_length + 1;
      } else {
         /* It's NULL, append the local network name */
         error = DNS_append_local_name( qname_buffer, total_length + 1 );
         return( error );
      } /* Endif */
      total_length++;
   } /* Endwhile */

   if ( error == DNS_OK ) {
      /*
      ** Need to append '\0' to terminate domain name properly.
      ** This denotes the ROOT domain
      */
      htonc(label_length_ptr, '\0');
   }/* Endif */

   return( error );

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_parse_answer_name_to_dotted_form()
* Returned Value  : uchar _PTR_
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uchar _PTR_ DNS_parse_answer_name_to_dotted_form
   (
   uchar _PTR_ buffer_ptr,
   uchar _PTR_ name,
   uint_32     loc
   )

{  /* Body */

   uchar   _PTR_ compressed_name;
   uchar   _PTR_ fill_ptr;
   uchar         label_size;
   uint_32       i;
   uint_16       new_location;

   fill_ptr = RTCS_HOST_NAMES[loc];
   _mem_zero(fill_ptr, DNS_MAX_CHARS_IN_NAME);

   compressed_name = name;

   while ( ntohc(compressed_name) != '\0' ) {

      if ( ntohc(compressed_name) & DNS_COMPRESSED_NAME_MASK ) {
         new_location = ntohs(compressed_name)
                        & DNS_COMPRESSED_LOCATION_MASK;
         compressed_name = buffer_ptr + new_location;
      }/* Endif */

      label_size  = ntohc(compressed_name);
      compressed_name++;

      for ( i = 0; i < label_size; i++ ) {
           *fill_ptr++ = ntohc(compressed_name);
            compressed_name++;
      } /* Endfor */

      if ( ntohc(compressed_name) != '\0' ) {
         *fill_ptr++ = '.';
      }/* Endif */
   } /* Endwhile */

   *fill_ptr = '\0';

   return( RTCS_HOST_NAMES[loc] );

}  /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_free_HOSTENT_STRUCT()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

void DNS_free_HOSTENT_STRUCT
   (
   HOSTENT_STRUCT _PTR_ host_ptr
   )

{  /* Body */
   /*
   ** This function is no longer needed, but the wrapper is left for
   ** backwards compatibility.
   */
}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_parse_UDP_response()
* Returned Value  : HOSTENT_STRUCT_PTR
* Comments        : If answers were returned, this function will
*                   parse them into a HOSTENT_STRUCT
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT _PTR_ DNS_parse_UDP_response
   (
   uchar   _PTR_ buffer_ptr,
   uchar   _PTR_ name_ptr,
   uint_16       query_type
   )

{  /* Body */

   DNS_MESSAGE_HEADER_STRUCT     _PTR_   message_head_ptr = NULL;
   DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_   answer_middle;
   INTERNAL_HOSTENT_STRUCT       _PTR_   host_ptr = &RTCS_HOST;
   uchar                         _PTR_   answer_ptr;
   uchar                         _PTR_   answer_tail;
   uchar                         _PTR_   temp_ptr;
   uint_16                               response_length, answer_type,
      name_size, number_of_answers, num_queries;
   uint_32                               i, name_index = 0;
   uint_32                               j = 0;
   uint_32                               k = 0;
   uint_32                               buffer_size;
   uint_32                       _PTR_   addr_ptr;
   boolean                               unknown_answer_type = FALSE;

   message_head_ptr  = (DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer_ptr;
   buffer_size       = sizeof(DNS_MESSAGE_HEADER_STRUCT);
   temp_ptr = buffer_ptr + sizeof( DNS_MESSAGE_HEADER_STRUCT );

   /* Zero the global HOSTENT_STRUCT */
   _mem_zero((char_ptr)host_ptr, sizeof(INTERNAL_HOSTENT_STRUCT));

   /* Get the number of queries. */
   num_queries = ntohs(message_head_ptr->QDCOUNT);
   for (i = 0; i < num_queries; i++) {
      name_size = 0;
      while( (ntohc(temp_ptr) != '\0') &&
         name_size < DNS_MAX_CHARS_IN_NAME ) {
         name_size = name_size + ntohc(temp_ptr) + 1;
         temp_ptr  = temp_ptr  + ntohc(temp_ptr) + 1;
      } /* Endwhile */
      /* To include the terminating NULL char */
      name_size++;
      buffer_size += (name_size + sizeof(DNS_MESSAGE_TAIL_STRUCT));
      temp_ptr    += (1 + sizeof(DNS_MESSAGE_TAIL_STRUCT));
   } /* Endfor */

   number_of_answers = ntohs(message_head_ptr->ANCOUNT);
   if (number_of_answers > DNS_MAX_NAMES ) {
      number_of_answers = DNS_MAX_NAMES;
   } /* Endif */

   host_ptr->HOSTENT.h_aliases   = &host_ptr->ALIASES[0];
   host_ptr->HOSTENT.h_addr_list = (char_ptr _PTR_)&host_ptr->ADDRESSES[0];
   host_ptr->ADDRESSES[0]        = NULL;
   host_ptr->HOSTENT.h_name      = NULL;
   host_ptr->HOSTENT.h_length    = sizeof( _ip_address );

   for (i = 0; (i < number_of_answers) && (j < DNS_MAX_ADDRS) &&
       (k < DNS_MAX_NAMES); i++ )
   {
      answer_ptr = temp_ptr;
      name_size  = 0;

      while( (ntohc(temp_ptr) != '\0') &&
             name_size < DNS_MAX_CHARS_IN_NAME &&
             !(ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK)) {
         name_size += ntohc(temp_ptr);
         temp_ptr += ntohc(temp_ptr) + 1;
      } /* Endwhile */

      if ( ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK ) {
         temp_ptr++;
      }/* Endif */

      temp_ptr++;
      answer_middle   = (DNS_RESPONSE_RR_MIDDLE_STRUCT _PTR_)temp_ptr;
      response_length = ntohs(answer_middle->RDLENGTH);
      answer_type     = ntohs(answer_middle->TYPE);
      temp_ptr       += sizeof(DNS_RESPONSE_RR_MIDDLE_STRUCT);
      answer_tail     = temp_ptr;
      temp_ptr       += response_length;

      switch ( answer_type ) {

         case DNS_A:
            if ( host_ptr->HOSTENT.h_name == NULL ) {
               host_ptr->HOSTENT.h_name =
                  (char _PTR_)DNS_parse_answer_name_to_dotted_form(
                  buffer_ptr, answer_ptr, name_index );
               name_index++;
            } /* Endif */

            RTCS_HOST_ADDRS[j] = ntohl((uchar_ptr)answer_tail);
            /*
            ** j is used in case BOTH CNAME and A data is received.  If CNAME
            ** answer is first, will write into wrong address space if using
            ** i.
            */
            host_ptr->ADDRESSES[j] = &RTCS_HOST_ADDRS[j];
            j++;
            /*
            ** This is to assure that the first IP address used is the first
            ** one that was given
            */
            host_ptr->IP_address = *host_ptr->ADDRESSES[0];
            break;

         case DNS_PTR:
            if (query_type == DNS_PTR) {
               if (host_ptr->HOSTENT.h_name != NULL) {
                  host_ptr->ALIASES[k] = host_ptr->HOSTENT.h_name;
                  k++;
               } /* Endif */
               host_ptr->HOSTENT.h_name =
                  (char _PTR_)DNS_parse_answer_name_to_dotted_form(
                  buffer_ptr, answer_tail, name_index );
               name_index++;
               addr_ptr = RTCS_mem_alloc_zero( sizeof( _ip_address ));
               if ( addr_ptr == NULL ) {
                  RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY,
                                 0, 0, 0);
                  return( NULL );
               }/* Endif */

               *addr_ptr = *((_ip_address _PTR_)name_ptr);
               host_ptr->ADDRESSES[j] = addr_ptr;
               j++;
               host_ptr->IP_address = *host_ptr->ADDRESSES[0];
            } else {
               host_ptr->ALIASES[k] = (char _PTR_)
                     DNS_parse_answer_name_to_dotted_form( buffer_ptr,
                     answer_tail, name_index );
               name_index++;
               k++;
            } /* Endif */
            break;

         case DNS_CNAME:
            /* the k is used for ALIASES as the j is used for ADDRESSES */
            host_ptr->ALIASES[k] = (char _PTR_)
               DNS_parse_answer_name_to_dotted_form(
               buffer_ptr, answer_tail, name_index );
            name_index++;
            k++;
            break;

         default:
            unknown_answer_type = TRUE;
      } /* Endswitch */

      if ( unknown_answer_type == TRUE ) {
         break;
      }/* Endif */

      host_ptr->ADDRESSES[j]       = NULL;
      host_ptr->ALIASES[k]         = NULL;
      host_ptr->HOSTENT.h_addrtype = ntohs(answer_middle->CLASS);

   } /* Endfor */

   if ( number_of_answers == 0 ) {
      return( NULL );
   } /* Endif */

   return( &RTCS_HOST.HOSTENT );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_query_resolver_task()
* Returned Value  : HOSTENT_STRUCT _PTR_
* Comments        :
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT _PTR_ DNS_query_resolver_task
   (
   uchar _PTR_ name,
   uint_16     query_type
   )

{  /* Body */

   DNS_MESSAGE_HEADER_STRUCT _PTR_   message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT   _PTR_   message_tail_ptr;
   HOSTENT_STRUCT            _PTR_   host_ptr = NULL;
   sockaddr_in                       addr;
   uint_32                           local_sock;
   uint_32                           qname_size;
   uint_32                           buffer_size;
   uint_16                           rlen;
   int_32                            temp_size;
   int_32                            error;
   uchar                     _PTR_   temp_ptr;
   uchar                     _PTR_   qname_ptr;
   uchar                     _PTR_   buffer_ptr;



   /*
   ** If the size of this buffer is changed, also change the buffer size
   ** in the recvfrom() call near the bottom of this function
   */
   buffer_ptr = RTCS_mem_alloc_zero( DNS_MAX_UDP_MESSAGE_SIZE );
   if ( buffer_ptr == NULL ) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY,
                     0, 0, 0);
      return( NULL );
   }/* Endif */
   _mem_set_type(buffer_ptr, MEM_TYPE_DNS_UDP_MESSAGE);
   qname_ptr = buffer_ptr + sizeof( DNS_MESSAGE_HEADER_STRUCT );

   if ( query_type == DNS_A ) {
       error = DNS_is_dotted_domain_name( name, qname_ptr );
       if ( error == RTCSERR_DNS_INVALID_NAME ||
            error == RTCSERR_DNS_INVALID_LOCAL_NAME ) {
          _mem_free(buffer_ptr);
          return( NULL );
       }/* Endif */
   } else {
      if ( query_type == DNS_PTR ) {
         error = DNS_insert_IP_query( name, qname_ptr );
         if ( error == RTCSERR_DNS_INVALID_IP_ADDR ) {
            _mem_free(buffer_ptr);
            return( NULL );
         }/* Endif */
      } else {
         _mem_free(buffer_ptr);
         return( NULL );
      } /* Endif */
   } /* Endif */

   local_sock = socket(AF_INET, SOCK_DGRAM, 0);

   if ( local_sock == RTCS_HANDLE_ERROR ) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_OPEN_SOCKET,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   }/* Endif */

   /* Local address  */
   addr.sin_family       = AF_INET;
   addr.sin_port         = 0;
   addr.sin_addr.s_addr  = INADDR_ANY;

   error =  bind(local_sock, &addr, sizeof(addr));
   if (error != RTCS_OK) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_BIND_SOCKET,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   } /* Endif */

   /* set up buffer for sending query.   */
   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT _PTR_)buffer_ptr;

   htons(message_head_ptr->ID, 0);
   htons(message_head_ptr->CONTROL, DNS_STANDARD_QUERY);
   htons(message_head_ptr->QDCOUNT, DNS_SINGLE_QUERY);
   htons(message_head_ptr->NSCOUNT, 0);
   htons(message_head_ptr->ARCOUNT, 0);
   htons(message_head_ptr->ANCOUNT, 0);

   qname_size = strlen((char _PTR_)qname_ptr );
   /* Need to include the last '\0' character as well */
   qname_size++;

   temp_ptr = buffer_ptr + sizeof( DNS_MESSAGE_HEADER_STRUCT )
                          + qname_size;

   message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT _PTR_)temp_ptr;

   htons(message_tail_ptr->QTYPE, query_type);
   htons(message_tail_ptr->QCLASS, DNS_IN);

   buffer_size = sizeof(DNS_MESSAGE_HEADER_STRUCT) + qname_size
                 + sizeof(DNS_MESSAGE_TAIL_STRUCT);

    /* Remote address, DNS_Resolver currently uses port 1024 */
   addr.sin_port        = DNS_RESOLVER_PORT;
   addr.sin_addr.s_addr = DNS_RESOLVER_IP_ADDR;

   rlen = sizeof(addr);

   /* Send the buffer to the resolver for making a query */
   error = sendto(local_sock, buffer_ptr, buffer_size, 0, &addr, rlen);
   if (error == RTCS_ERROR) {
      shutdown(local_sock, FLAG_ABORT_CONNECTION);
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_SEND_QUERY,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   }/* Endif */

   /* Get the response from the resolver, if none received, return NULL */
   error = (uint_32)RTCS_selectset( &local_sock, 1, DNS_QUERY_TIMEOUT );
   if ( !error || error == RTCS_ERROR ) {
      shutdown(local_sock, FLAG_ABORT_CONNECTION);
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_NO_RESPONSE_FROM_RESOLVER,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   } /* Endif */

   temp_size = recvfrom(local_sock, buffer_ptr, DNS_MAX_UDP_MESSAGE_SIZE,
                        0, &addr, &rlen);
   if ( temp_size == RTCS_ERROR ) {
     shutdown(local_sock, FLAG_ABORT_CONNECTION);
     RTCS_log_error(ERROR_DNS, RTCSERR_DNS_PACKET_RECEPTION_ERROR,
                    0, 0, 0);
     _mem_free(buffer_ptr);
     return( NULL );
   }/* Endif */

   host_ptr = DNS_parse_UDP_response(buffer_ptr, name, query_type);
   shutdown(local_sock, FLAG_ABORT_CONNECTION);
   _mem_free(buffer_ptr);
   return( host_ptr );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_check_local_host_file()
* Returned Value  : HOSTENT_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT_PTR DNS_check_local_host_file
   (
      uchar_ptr  name
   )
{  /* Body */
   INTERNAL_HOSTENT_STRUCT_PTR host_ptr = &RTCS_HOST;
   uint_32                     i;
   boolean                     name_found;
   uint_32                     num_aliases = 0;
   uint_32                     host_index = 0;
   uint_32                     digit_count = 0;
   uint_32                     dot_count = 0;
   uint_32                     name_size, name_index = 0;
   int_32                      byte_num;
   boolean                     dotted_decimal;
   uint_32                     temp = 0;

   /* zero memory for HOSTENT_STRUCT */
   _mem_zero(host_ptr, sizeof(INTERNAL_HOSTENT_STRUCT));

   host_ptr->HOSTENT.h_aliases = &host_ptr->ALIASES[0];
   host_ptr->HOSTENT.h_addr_list = (char_ptr _PTR_)&host_ptr->ADDRESSES[0];

   /* assign address type and length */
   host_ptr->HOSTENT.h_addrtype = AF_INET;
   host_ptr->HOSTENT.h_length = 4;     /* IP addresses only used - 4 bytes */

   /* check to see if name is written in dotted decimal IP format */
   dotted_decimal = TRUE;
   for (i=0; name[i] != '\0'; ++i ) {
      if ( name[i] == '.' ) {
         dot_count++;
         if ( dot_count > 3 ) {
            /* invalid IP address */
            dotted_decimal = FALSE;
            break;
         }/* Endif */

         if ( digit_count == 0 ) {
            /* there are no digits before the '.' */
            dotted_decimal = FALSE;
            break;
         }/* Endif */
         digit_count = 0;
         byte_num = byte_num / 10; /* shift back */

         if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
            dotted_decimal = FALSE;
            break;
         } else  {
            temp = temp + byte_num;
            temp = temp * 0x100;
         }/* Endif */
      } else { /* a digit */

         if ( digit_count == 0 ) {
            byte_num = 0;
         }/* Endif */
         ++digit_count;

         if ( digit_count > 3 ) {
            /* too many digits between the '.' */
            dotted_decimal = FALSE;
            break;
         }/* Endif */

         if ( isdigit(name[i]) ) {
            /* number is in range */
            byte_num = byte_num + name[i] - '0';
            byte_num = byte_num * 10;
         } else {
            /* if the characters are not decimal digits it's invalid */
            dotted_decimal = FALSE;
            break;
         }/* Endif */
      }/* Endif */
   } /* Endfor */

   if ( dotted_decimal ) { /* check last number */
      if ( digit_count == 0 ) {
         /* there are no digits before the '.' */
         dotted_decimal = FALSE;
      }/* Endif */

      byte_num = byte_num / 10;
      if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
         dotted_decimal = FALSE;
      } else {
         temp = temp + byte_num;
      }/* Endif */

      if ( dot_count != 3 ) {
         /* the wrong number of dots were found */
         dotted_decimal = FALSE;
      }/* Endif */

   }/* Endif */


   if ( i == 0 ) {
      /* no name string of first char was not a number */
      dotted_decimal = FALSE;
   }/* Endif */

   _mem_zero(RTCS_HOST_NAMES[name_index], DNS_MAX_CHARS_IN_NAME);
   if ( dotted_decimal == TRUE ) {
      name_size = strlen((char _PTR_)name );
      name_size++;
      _mem_copy(name, RTCS_HOST_NAMES[name_index], name_size);
      /* assign the hostent struct from the dotted decimal IP name */
      host_ptr->HOSTENT.h_name = (char _PTR_)RTCS_HOST_NAMES[name_index];
      host_ptr->ALIASES[num_aliases] = NULL;
      host_ptr->IP_address = temp;
      host_ptr->ADDRESSES[0] = &host_ptr->IP_address;
      host_ptr->ADDRESSES[1] = NULL;
      name_found = TRUE;
   } else  {

      name_found = FALSE;

      /* search for the name in the hosts structure */
      while ( ((RTCS_Hosts_list[host_index]).ip_address != 0) && !name_found) {
      /*end of list hasn't been reached*/
         /* check the aliases for the name */
         i = 0;
         while ( ((RTCS_Hosts_list[host_index]).aliases[i] != NULL) &&
                 !name_found)
         {
            if ( strcmp((char _PTR_)(RTCS_Hosts_list[host_index]).aliases[i],
                        (char _PTR_)name) == 0 )
            {
               name_found = TRUE;
            }/* Endif */
            ++i;
         } /* Endwhile */

         /* check the name field for the name */
         if ( strcmp((char _PTR_)(RTCS_Hosts_list[host_index]).host_name,
             (char _PTR_)name) == 0)
         {
            name_found = TRUE;
         } /* Endif */

         if (name_found == TRUE ) { /* host name was found */
            name_size = strlen( (RTCS_Hosts_list[host_index]).host_name );
            name_size++;
            _mem_copy((RTCS_Hosts_list[host_index]).host_name,
               RTCS_HOST_NAMES[name_index], name_size);

            host_ptr->HOSTENT.h_name = (char _PTR_)RTCS_HOST_NAMES[name_index];
            name_index++;

            /* assign alias list */
            while ( (num_aliases < MAX_HOST_ALIASES)
               && ((RTCS_Hosts_list[host_index]).aliases[num_aliases] != NULL)
               && (name_index < DNS_MAX_NAMES))
            {
               name_size =
                  strlen((RTCS_Hosts_list[host_index]).aliases[num_aliases]);
               name_size++;


               _mem_zero(RTCS_HOST_NAMES[name_index], DNS_MAX_CHARS_IN_NAME);
               _mem_copy((RTCS_Hosts_list[host_index]).aliases[num_aliases],
                  RTCS_HOST_NAMES[name_index], name_size);
               host_ptr->ALIASES[num_aliases] =
                  (char _PTR_)RTCS_HOST_NAMES[name_index];
               name_index++;
               num_aliases++;
            } /* Endwhile */
            host_ptr->ALIASES[num_aliases] = NULL;

            /* assign addresses (in our case only one IP address is allowed) */
            _mem_copy((_ip_address _PTR_)
               &((RTCS_Hosts_list[host_index]).ip_address), &RTCS_HOST_ADDRS[0],
               sizeof(_ip_address));

            host_ptr->ADDRESSES[0] = (uint_32_ptr)&RTCS_HOST_ADDRS[0];
            host_ptr->ADDRESSES[1] = NULL;
         } /* Endif */
         ++host_index;
      } /* Endwhile */
   }/* Endif */


   if ( !name_found ) {
      return( NULL );
   }/* Endif */

   return( &host_ptr->HOSTENT );
}/* EndBody */

#endif
#endif
/* EOF */
