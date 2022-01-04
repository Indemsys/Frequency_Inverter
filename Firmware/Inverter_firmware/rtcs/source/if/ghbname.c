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
* $FileName: ghbname.c$
* $Version : 3.6.13.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the gethostbyname() function.
*
*END************************************************************************/

#include <string.h>
#include <rtcs.h>

static boolean RTCS_get_dotted_address (char_ptr name, uint_32_ptr address)
{
   uint_32                     i;
   uint_32                     digit_count = 0;
   uint_32                     dot_count = 0;
   int_32                      byte_num;
   boolean                     dotted_decimal = TRUE;
   
   if ((name == NULL) || (address == NULL)) return FALSE;
   
   *address = 0;
   
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
            *address = (*address) + byte_num;
            *address = (*address) * 0x100;
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

         if ( (name[i] >= '0') && (name[i] <= '9') ) {
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
         *address = (*address) + byte_num;
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
    
   return dotted_decimal;
}






#if RTCSCFG_ENABLE_LWDNS && RTCSCFG_ENABLE_UDP
boolean RTCS_resolve_ip_address( char_ptr arg, _ip_address _PTR_ ipaddr_ptr, char_ptr ipname, uint_32 ipnamesize)
{
   /* first check to see if name is written in dotted decimal IP format */
   if(RTCS_get_dotted_address (arg, ipaddr_ptr))
   {
      /* name was in dotted decimal format */
      return TRUE;
   }
   else
   {
      /* neccessary to resolve name */
      return LWDNS_resolve(arg, ipaddr_ptr, ipname, ipnamesize);
   }
}



#elif RTCSCFG_ENABLE_DNS && RTCSCFG_ENABLE_UDP



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gethostbyname()
* Returned Value  : HOSTENT_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT_PTR RTCS_gethostbyname
   (
      char_ptr  name
   )
{  /* Body */
   INTERNAL_HOSTENT_STRUCT_PTR host_ptr;
   uint_32                     i;
   boolean                     name_found;
   boolean                     dotted_decimal;
   uint_32                     num_aliases = 0;
   uint_32                     host_index = 0;
   uint_32                     temp;

   /* allocate memory for HOSTENT_STRUCT */
   host_ptr = RTCS_mem_alloc_zero(sizeof(INTERNAL_HOSTENT_STRUCT));
   if ( host_ptr == NULL ) {
      return(NULL);
   }/* Endif */
   _mem_set_type(host_ptr, MEM_TYPE_HOSTENT_STRUCT);
   
   host_ptr->HOSTENT.h_aliases = &host_ptr->ALIASES[0];
   host_ptr->HOSTENT.h_addr_list = (char_ptr _PTR_)&host_ptr->ADDRESSES[0];

   /* assign address type and length */
   host_ptr->HOSTENT.h_addrtype = AF_INET;
   host_ptr->HOSTENT.h_length = 4;     /* IP addresses only used - 4 bytes */

   /* check to see if name is written in dotted decimal IP format */
   dotted_decimal = RTCS_get_dotted_address (name, &temp);

   if ( dotted_decimal == TRUE ) {
      /* assign the hostent struct from the dotted decimal IP name */
      host_ptr->HOSTENT.h_name = name;
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
         i=0;
         while ( ((RTCS_Hosts_list[host_index]).aliases[i] != NULL) && !name_found) {
            if ( strcmp((RTCS_Hosts_list[host_index]).aliases[i],name) == 0 ) {
               name_found = TRUE;
            }/* Endif */
            ++i;
         } /* Endwhile */

         /* check the name field for the name */
         if ( strcmp((RTCS_Hosts_list[host_index]).host_name, name) == 0)  {
            name_found = TRUE;
         } /* Endif */

         if (name_found == TRUE ) { /* host name was found */

            host_ptr->HOSTENT.h_name = (RTCS_Hosts_list[host_index]).host_name;

            /* assign alias list */
            while ( (num_aliases < MAX_HOST_ALIASES)
                     && (RTCS_Hosts_list[host_index]).aliases[num_aliases] != NULL) {

               host_ptr->ALIASES[num_aliases] = (RTCS_Hosts_list[host_index]).aliases[num_aliases];
               ++num_aliases;
            } /* Endwhile */
            host_ptr->ALIASES[num_aliases] = NULL;

            /* assign addresses (in our case only one IP address is allowed) */

            host_ptr->ADDRESSES[0] = (uint_32_ptr)&((RTCS_Hosts_list[host_index]).ip_address);
            host_ptr->ADDRESSES[1] = NULL;
         } /* Endif */
         ++host_index;
      } /* Endwhile */
   }/* Endif */


   if ( !name_found ) {
      _mem_free(host_ptr);
      return( NULL );
   }/* Endif */

   return( &host_ptr->HOSTENT );

}/* EndBody */


boolean RTCS_resolve_ip_address( char_ptr arg, _ip_address _PTR_ ipaddr_ptr, char_ptr ipname, uint_32 ipnamesize)
{
    HOSTENT_STRUCT_PTR   host_ptr;
    boolean              result = FALSE;

   if (ipaddr_ptr == NULL) return FALSE;
   
   *ipaddr_ptr = 0;
   if (*arg != '\0') {
      host_ptr = gethostbyname(arg);
      if (host_ptr != NULL)  {
         if (*host_ptr->h_addr_list != NULL) {
            *ipaddr_ptr = *(_ip_address _PTR_)*host_ptr->h_addr_list;
            strncpy(ipname, host_ptr->h_name, ipnamesize);
            result = TRUE;
         } 
         DNS_free_HOSTENT_STRUCT(host_ptr);
      } 
   } 
   
   return result;
}



#else



boolean RTCS_resolve_ip_address( char_ptr arg, _ip_address _PTR_ ipaddr_ptr, char_ptr ipname, uint_32 ipnamesize)
{
    return RTCS_get_dotted_address (arg, ipaddr_ptr);
}



#endif

/* EOF */
