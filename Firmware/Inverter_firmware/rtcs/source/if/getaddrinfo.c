/**HEADER*******************************************************************************
* 
* Copyright (c) 2011 , Freescale Semiconductor;
* All Rights Reserved                      
*
**************************************************************************************** 
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
****************************************************************************************
*
* $FileName: getaddrinfo.c$
* $Version : 3.8.3.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*   getaddrinfo() is used to get a list of IP addresses and port
*   numbers for host hostname and service servname. 
*
*END************************************************************************************/

/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 *
 * This code is derived from software contributed to ISC by
 * Berkeley Software Design, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC AND BERKELEY SOFTWARE DESIGN, INC.
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <ctype.h>
#include <rtcs.h>
#include <posix.h> // it is for errno, if we want to know error number
#include <stdlib.h>//stdtol() 
#include <addrinfo.h>


#define SA(addr)	((struct sockaddr *)(addr))
#define SIN(addr)	((struct sockaddr_in *)(addr))
#define SIN6(addr)	((struct sockaddr_in6 *)(addr))
#define SLOCAL(addr)	((struct sockaddr_un *)(addr))

/*! \struct addrinfo
 */
static struct addrinfo
	*ai_reverse(struct addrinfo *oai),
	*ai_clone(struct addrinfo *oai, int family),
	*ai_alloc(int family, int addrlen);

static char *RTCS_strsep(char **stringp, const char *delim);
static int add_ipv4(const char *hostname, int flags, struct addrinfo **aip, int socktype, int port);
static int add_ipv6(const char *hostname, int flags, struct addrinfo **aip, int socktype, int port);
static void set_order(int, int (**)(const char *, int, struct addrinfo **, int, int));



#define FOUND_IPV4	0x1
#define FOUND_IPV6	0x2
#define FOUND_MAX	2

#define ISC_AI_MASK (AI_PASSIVE|AI_CANONNAME|AI_NUMERICHOST)


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : getaddrinfo
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Get a list of IP addresses and port numbers for host hostname and service servname.
*
*END*-----------------------------------------------------------------*/
int_32 getaddrinfo
(
	const char 				*hostname, 	/* host name or IP or NULL							*/
	const char 				*servname,	/* service name or port number						*/
	const struct addrinfo 	*hints,		/* set flags										*/
	struct addrinfo 		**res		/* [OUT]list of address structures					*/
)

{
//	struct servent *sp;
	const char 		*proto = NULL;
	int 			family; 
	int				socktype; 
	int				flags; 
	int				protocol;
	
	struct addrinfo *ai;
	struct addrinfo *ai_list;
	uint_16 port; 
	uint_16 err; 
	uint_16 i;
	
	
	int (*net_order[FOUND_MAX+1])(const char *, int, struct addrinfo **,int, int);

	/*hostname and send name can not be NULL in same time*/
	if (hostname == NULL && servname == NULL)
	{
		return (EAI_NONAME);
	}

	proto = NULL;
	if (hints != NULL) 
	{
		if ((hints->ai_flags & ~(ISC_AI_MASK)) != 0)
		{
			return (EAI_BADFLAGS);
		}
		if (hints->ai_addrlen || hints->ai_canonname || hints->ai_addr || hints->ai_next) 
		{
			errno = EINVAL;
			return (EAI_SYSTEM);
	    }

		family = hints->ai_family;
		socktype = hints->ai_socktype;
		protocol = hints->ai_protocol;
		flags = hints->ai_flags;
		
		/* looking for correct protocol depended on family and socket type */
		switch (family) {
			case AF_UNSPEC:
			
			/* switch operator is not implemented here */
				if(hints->ai_socktype == SOCK_STREAM)
				{
					proto = "tcp";
				
				}else if(hints->ai_socktype == SOCK_DGRAM)	{
				
					proto = "udp";
				
				}else{
				
					return (EAI_SOCKTYPE);				
				
				}

			break;
			case AF_INET:
			case AF_INET6:
				if(hints->ai_socktype == 0)
				{
					break;
			
				}else if(hints->ai_socktype == SOCK_STREAM)	{
				
					proto = "tcp";
				
				}else if(hints->ai_socktype == SOCK_DGRAM)	{
					
					proto = "udp";
			
				}else{
				
					return (EAI_SOCKTYPE);
			
				}

			break;

		default:
			return (EAI_FAMILY);

		}/* end of  switch (family) */
	} else {
	
		protocol = 0;
		family = 0;
		socktype = 0;
		flags = 0;
	
	}/* end of (hints != NULL) */

	if(proto !=NULL)
	{
		protocol = (strcmp(proto,"tcp")) ? 2 : 1;

	}

	/*
	 * Ok, only AF_INET and AF_INET6 left.
	 */
	 
	/*************************************/

	/*
	 * First, look up the service port if it was
	 * requested.  If the socket type wasn't specified, then
	 * try and figure it out.
	 */
	if (servname != NULL) {
		char *e;

		port = strtol(servname, &e, 10);
		if (*e == '\0') {					//*e - end pointer
			if (socktype == 0)				//I am not shure that it is necessary here
			{								//
				return (EAI_SOCKTYPE);		//
			}								//

			htons(&port,(uint_16) port);	//MQX version of htons

		} else {
		/* b23362 - we use only port number.We do not use a service name*/
			return (EAI_SERVICE);			
		}/*end of (*e == '\0')*/
	} else {

		port = 0;

	} /* end (servname != NULL)	*/

	/*
	 * Next, deal with just a service name, and no hostname.
	 * (we verified that one of them was non-null up above).
	 */
	
	ai_list = NULL;
				 
	if (hostname == NULL && (flags & AI_PASSIVE) != 0) 
	{
	    if (family == AF_INET || family == 0) 
		{
		
			/* Allocate an addrinfo structure, and a sockaddr structure */		
			ai = ai_alloc(AF_INET, sizeof(struct sockaddr_in));

			if (ai == NULL)
			{
				freeaddrinfo(ai_list);
				return (EAI_MEMORY);
			}
			ai->ai_socktype = socktype;
			ai->ai_protocol = protocol;
			SIN(ai->ai_addr)->sin_port = port;
			ai->ai_next = ai_list;
			ai_list = ai;
		}
		if (family == AF_INET6 || family == 0) 
		{
			ai = ai_alloc(AF_INET6, sizeof(struct sockaddr_in6));
			if (ai == NULL) 
			{
				freeaddrinfo(ai_list);
				return (EAI_MEMORY);
			}
			ai->ai_socktype = socktype;
			ai->ai_protocol = protocol;
			SIN6(ai->ai_addr)->sin6_port = port;
			ai->ai_next = ai_list;
			ai_list = ai;
		}

		*res = ai;
		return (0);
	}/* end of (hostname == NULL && (flags & AI_PASSIVE) != 0) */


	/*
	 * If the host is not NULL and the family isn't specified or AI_NUMERICHOST 
	 * specified, check first to see if it is a numeric address.
	 * Though the gethostbyname2() routine
	 * will recognize numeric addresses, it will only recognize
	 * the format that it is being called for.  Thus, a numeric
	 * AF_INET address will be treated by the AF_INET6 call as
	 * a domain name, and vice versa.  Checking for both numerics
	 * here avoids that.
	 */
	if (hostname != NULL && (family == 0 || (flags & AI_NUMERICHOST) != 0)) 
	{
		char abuf[sizeof(struct in6_addr)];
		char nbuf[NI_MAXHOST];
		int addrsize, addroff;
		char *p, *ep;
		char ntmp[NI_MAXHOST];
		uint_32 scopeid;

		/*
		 * Scope identifier portion.
		 * scope id must be a decimal number
		 */
		ntmp[0] = '\0';
		if (strchr(hostname, '%') != NULL) 
		{
			strncpy(ntmp, hostname, sizeof(ntmp) - 1);
			ntmp[sizeof(ntmp) - 1] = '\0';
            /*Returns a pointer to the first occurrence of character '%'.*/			
			p = strchr(ntmp, '%');
			ep = NULL;

			/*
			 * Vendors may want to support non-numeric
			 * scopeid around here.
			 */

			if (p != NULL)
			{
				scopeid = (uint_32)strtoul(p + 1, &ep, 10);
			}				
			if (p != NULL && ep != NULL && ep[0] == '\0')
			{
				*p = '\0';
			}else {
				ntmp[0] = '\0';
				scopeid = 0;
			}
		} else{
			scopeid = 0;
		} /* end of  (strchr(hostname, '%') != NULL) */	


		  /*
	 	   * Converts a human readable IP address into an address 
	 	   * family appropriate 32bit or 128bit binary structure.
	 	   */
	    if (inet_pton(AF_INET, hostname, (struct in_addr *)abuf,sizeof( *((struct in_addr *)abuf)))== RTCS_OK)
	    {
			if (family == AF_INET6) 
			{
				/*
			 	 * Convert to a V4 mapped address.
			 	 */
				struct in6_addr *a6 = (struct in6_addr *)abuf;
				memcpy(&a6->s6_addr[12], &a6->s6_addr[0], 4);
				memset(&a6->s6_addr[10], 0xff, 2);
				memset(&a6->s6_addr[0], 0, 10);
				goto inet6_addr;
			}
			addrsize = sizeof(struct in_addr);
			addroff = (char *)(&SIN(0)->sin_addr) - (char *)0;
			family = AF_INET;
			goto common;
		} else if (ntmp[0] != '\0' && inet_pton(AF_INET6, ntmp, abuf,sizeof(struct in6_addr)) == RTCS_OK)
		{
			if (family && family != AF_INET6)
			{
				return (EAI_NONAME);
			}
			addrsize = sizeof(struct in6_addr);
			addroff = (char *)(&SIN6(0)->sin6_addr) - (char *)0;
			family = AF_INET6;
			goto common;
		} else if (inet_pton(AF_INET6, hostname, abuf,sizeof(struct in6_addr)) == RTCS_OK) 
		{
			if (family != 0 && family != AF_INET6)
			{
				return (EAI_NONAME);
			}
inet6_addr:
			addrsize = sizeof(struct in6_addr);
			addroff = (char *)(&SIN6(0)->sin6_addr) - (char *)0;
			family = AF_INET6;

common:
			ai = ai_clone(ai_list, family);
			if (ai == NULL)
			{
				return (EAI_MEMORY);
			}
			ai_list = ai;
			ai->ai_socktype = socktype;
			//ai->ai_protocol = protocol;
			SIN(ai->ai_addr)->sin_port = port;
			memcpy((char *)ai->ai_addr + addroff, abuf, addrsize);

			if (flags & AI_CANONNAME) 
			{

				if (ai->ai_family == AF_INET6)
				{
					SIN6(ai->ai_addr)->sin6_scope_id = scopeid;
				}	

				if (getnameinfo(	ai->ai_addr,
			    					ai->ai_addrlen, 
			    					nbuf, 
			    					sizeof(nbuf),
					      			NULL, 
					      			0,
					      			NI_NUMERICHOST
					       		) == 0) 
				{
					ai->ai_canonname = strdup(nbuf);
					if (ai->ai_canonname == NULL) 
					{
						freeaddrinfo(ai_list);
						return (EAI_MEMORY);
					}
				} else 	
				{
					/* XXX raise error? */
					ai->ai_canonname = NULL;
				}
			}/*end of (flags & AI_CANONNAME)*/
			goto done;
		} else if ((flags & AI_NUMERICHOST) != 0) 
		{
			return (EAI_NONAME);
		}
	}/* end of (inet_pton(AF_INET, hostname, (struct in_addr *)abuf)== 1) */

	set_order(family, net_order);
	for (i = 0; i < FOUND_MAX; i++) {
		if (net_order[i] == NULL)
			break;
		err = (net_order[i])(hostname, flags, &ai_list,
				     socktype, port);
		if (err != 0)
			return (err);
	}

	if (ai_list == NULL)
		return (EAI_NODATA);

done:
	ai_list->ai_protocol = protocol;
	ai_list->ai_flags = flags;

	ai_list = ai_reverse(ai_list);

	*res = ai_list;
	return (0);
}

static char *RTCS_strsep	(
								char **stringp, 
								const char *delim
							) 
{
	char *string = *stringp;
	char *s;
	const char *d;
	char sc, dc;

	if (string == NULL)
		return (NULL);

	for (s = string; *s != '\0'; s++) {
		sc = *s;
		for (d = delim; (dc = *d) != '\0'; d++)
			if (sc == dc) {
				*s++ = '\0';
				*stringp = s;
				return (string);
			}
	}
	*stringp = NULL;
	return (string);
}

static void set_order	(
							int family, 
							int (**net_order)(const char *, int, struct addrinfo **,int, int)
						)
{
	char *order, *tok;
	int found;

	if (family) 
	{
		switch (family) 
		{
			case AF_INET:
				*net_order++ = add_ipv4;
				break;
			case AF_INET6:
				*net_order++ = add_ipv6;
			break;
		}
	} else {
		order = NET_ORDER;//"inet";// : inet6";//getenv("NET_ORDER");
		found = 0;
		while (order != NULL) 
		{
			/*
			 * We ignore any unknown names.
			 */
			tok = RTCS_strsep(&order, ":");
			if (strcasecmp(tok, "inet6") == 0) 
			{
				if ((found & FOUND_IPV6) == 0)
					{
						*net_order++ = add_ipv6;
					}
						
				found |= FOUND_IPV6;
			} else if (strcasecmp(tok, "inet") == 0 || strcasecmp(tok, "inet4") == 0) 
			{
				if ((found & FOUND_IPV4) == 0)
				{
					*net_order++ = add_ipv4;
				}
				found |= FOUND_IPV4;
			}
		}

		/*
		 * Add in anything that we didn't find.
		 */
		if ((found & FOUND_IPV4) == 0)
			*net_order++ = add_ipv4;
		if ((found & FOUND_IPV6) == 0)
			*net_order++ = add_ipv6;
	}
	*net_order = NULL;
	return;
}

static char v4_loop[4] = { 127, 0, 0, 1 };

#define SETERROR(code) \
	do { result = (code);			\
		if (result != 0) goto cleanup;	\
	} while (0)

static int add_ipv4
(
	const char 		*hostname, 
	int 			flags, 
	struct addrinfo **aip,
	int 			socktype, 
	int 			port
)
{
	struct addrinfo *ai;
	int 			result = 0;
	boolean 		resolving_result = FALSE;
	_ip_address 	ipaddr; 
    char  		   *ipname = NULL; 

    /*
     *	In this case addr = 127.0.0.1 to get connection 
     *	inside host using LOOPBACK interface .
     */
     
	if (hostname == NULL && (flags & AI_PASSIVE) == 0) 
	{
		ai = ai_clone(*aip, AF_INET);
		if (ai == NULL) 
		{
			freeaddrinfo(*aip);
			SETERROR(EAI_MEMORY);
		}
		*aip = ai;
		ai->ai_socktype = socktype;
		SIN(ai->ai_addr)->sin_port = port;
		memcpy(&SIN(ai->ai_addr)->sin_addr, v4_loop, 4);
	} else 	{
		/* if the hostname is not NULL,lets try to resolve it 		*/
		/* here we are using RTCS DNS  service to get addr by name 	*/
	ipname= RTCS_mem_alloc(MAX_HOSTNAME_SIZE);
	if(ipname == NULL)
	{
		SETERROR(EAI_MEMORY);
	}
		memset (ipname, 0, sizeof (ipname));
		resolving_result =  RTCS_resolve_ip_address(
													(char *)hostname, 
															&ipaddr,        	 
															ipname, 			
															MAX_HOSTNAME_SIZE	
          	   		                               );
		if (resolving_result != TRUE) 
		{
				SETERROR(EAI_FAIL);
		}
		ai = ai_clone(*aip, AF_INET);
			if (ai == NULL) 
			{
				freeaddrinfo(*aip);
				SETERROR(EAI_MEMORY);
			}
			*aip = ai;
			ai->ai_socktype = socktype;
			((struct sockaddr_in *)(ai->ai_addr))->sin_port = port;
			memcpy(&((struct sockaddr_in *)(ai->ai_addr))->sin_addr,&ipaddr,4);
			if (flags & AI_CANONNAME) 
			{
				ai->ai_canonname = strdup(ipname);
				if (ai->ai_canonname == NULL)
				{
					SETERROR(EAI_NONAME);
				}
			}
	
		
	}/* end of (hostname == NULL && (flags & AI_PASSIVE) == 0) */
 
 cleanup:
	_mem_free(ipname);
	return (result);
}


static char v6_loop[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

static int add_ipv6	(
						const char 			*hostname, 
						int 				flags, 
						struct addrinfo 	**aip,
	 					int 				socktype, 
	 					int 				port
	 				)
{

	struct addrinfo *ai;
	int 			result = 0;
	boolean 		resolving_result = FALSE;
    char  		   *ipname = NULL; 


	if (hostname == NULL && (flags & AI_PASSIVE) == 0) 
	{
		ai = ai_clone(*aip, AF_INET6);
		if (ai == NULL) 
		{
			freeaddrinfo(*aip);
			SETERROR(EAI_MEMORY);
		}
		*aip = ai;
		ai->ai_socktype = socktype;
		SIN6(ai->ai_addr)->sin6_port = port;
		memcpy(&SIN6(ai->ai_addr)->sin6_addr, v6_loop, 16);
	
	} else {
   
   /*	IPv6 DNS resolver is not ready yet*/
    
   /*
	*	resolving_result = RTCS_resolve_ip_address6(
	*										(char *)hostname, 
	*										&ipaddr,        	 
	*										ipname, 			
	*										MAX_HOSTNAME_SIZE	
    *    	   		                     );
	*/
		resolving_result= TRUE;
		ipname = "www.example.com";
	/*************************************************/
	
		if (resolving_result!= TRUE) 
		{
				SETERROR(EAI_FAIL);
		}				
		ai = ai_clone(*aip, AF_INET6);
		if (ai == NULL) 
		{
			freeaddrinfo(*aip);
			SETERROR(EAI_MEMORY);
		}
		*aip = ai;
		ai->ai_socktype = socktype;
		SIN6(ai->ai_addr)->sin6_port = port;
		
		/* memcpy(&SIN6(ai->ai_addr)->sin6_addr, &ipaddr, 16); */
		memcpy(&SIN6(ai->ai_addr)->sin6_addr, v6_loop, 16);
		
		if (flags & AI_CANONNAME) 
		{
			ai->ai_canonname = strdup(ipname);
			if (ai->ai_canonname == NULL)
			{
				freeaddrinfo(*aip);
				SETERROR(EAI_NONAME);
			}
		}
		
	}/* end of (hostname == NULL && (flags & AI_PASSIVE) == 0) */
	
 cleanup:

	return (result);
}



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : freeaddrinfo
* Returned Value  : void
* Comments        : Free address info.
*
*END*-----------------------------------------------------------------*/
void freeaddrinfo	(
						struct addrinfo *ai
					) 
{
	struct addrinfo *ai_next;

	while (ai != NULL) 
	{
		ai_next = ai->ai_next;
		if (ai->ai_addr != NULL)
		{
			_mem_free(ai->ai_addr);
		}
		if (ai->ai_canonname)
		{
			_mem_free(ai->ai_canonname);
		}
		_mem_free(ai);
		ai = ai_next;
	}
}


/*!
 * Allocate an addrinfo structure, and a sockaddr structure
 * of the specificed length.  We initialize:
 *	ai_addrlen
 *	ai_family
 *	ai_addr
 *	ai_addr->sa_family
 *	ai_addr->sa_len	(LWRES_PLATFORM_HAVESALEN)
 * and everything else is initialized to zero.
 */
static struct addrinfo *ai_alloc	(
										int family, 
										int addrlen
									) 
{
	struct addrinfo *ai;

	ai = (struct addrinfo *) _mem_alloc_system_zero(sizeof(*ai));
	if (ai == NULL)
	{
		return (NULL);
	}
	ai->ai_addr = SA(_mem_alloc_system_zero(addrlen));
	if (ai->ai_addr == NULL) 
	{
		_mem_free(ai);
		return (NULL);
	}
	ai->ai_addrlen = addrlen;
	ai->ai_family = family;

    ai->ai_addr->sa_family = family;
	
	return (ai);
}

static struct addrinfo *ai_clone	(
										struct addrinfo *oai, 
										int family
									) 
{
	struct addrinfo *ai;

	ai = ai_alloc(family, ((family == AF_INET6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in)));
	if (ai == NULL) 
	{
		freeaddrinfo(oai);
		return (NULL);
	}
	if (oai == NULL)
	{
		return (ai);
	}
	ai->ai_flags = oai->ai_flags;
	ai->ai_socktype = oai->ai_socktype;
	ai->ai_protocol = oai->ai_protocol;
	ai->ai_canonname = NULL;
	ai->ai_next = oai;
	return (ai);
}

static struct addrinfo *ai_reverse	(
										struct addrinfo *oai
									) 
{
	struct addrinfo *nai, *tai;

	nai = NULL;

	while (oai != NULL) 
	{
		/*
		 * Grab one off the old list.
		 */
		tai = oai;
		oai = oai->ai_next;
		/*
		 * Put it on the front of the new list.
		 */
		tai->ai_next = nai;
		nai = tai;
	}
	return (nai);
}


char * strdup	( 
					const char *s
				)
{
  unsigned int len = strlen (s) + 1;
  
  char *result = (char*) _mem_alloc_system(len);
  if (result != NULL)
  {
	  _mem_copy((void*) s, result, len);
  }
  return result;
}
