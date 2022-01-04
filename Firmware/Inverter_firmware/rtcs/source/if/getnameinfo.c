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
****************************************************************************************/
/*
 * Portions Copyright (C) 2004, 2005, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Portions Copyright (C) 1999-2001, 2003  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
 
#include <string.h>
#include <ctype.h>
#include <rtcs.h>
#include <addrinfo.h>

#define SUCCESS 0

/*% afd structure definition */
static struct afd {
	int a_af;
	unsigned int a_addrlen;
	unsigned int a_socklen;
} afdl [] = {
	/*!
	 * First entry is linked last...
	 */
	{ AF_INET, sizeof(struct in_addr), sizeof(struct sockaddr_in) },
	{ AF_INET6, sizeof(struct in6_addr), sizeof(struct sockaddr_in6) },
	{0, 0, 0},
};

#define ENI_NOSERVNAME	1
#define ENI_NOHOSTNAME	2
#define ENI_MEMORY	3
#define ENI_SYSTEM	4
#define ENI_FAMILY	5
#define ENI_SALEN	6
#define ENI_NOSOCKET 	7

/*!
 * The test against 0 is there to keep the Solaris compiler
 * from complaining about "end-of-loop code not reached".
 */
#define ERR(code) \
	do { result = (code);			\
		if (result != 0) goto cleanup;	\
	} while (0)


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : getnameinfo()
* Returned Value  : RTCS_OK or error code
* Comments        :
*     Lightweight resolver socket address structure to hostname and service name.
*
*END*-----------------------------------------------------------------*/


int_32 getnameinfo
(
    const struct sockaddr  *sa, 
    unsigned int            salen, 
    char                   *host,
	unsigned int            hostlen, 
	char                   *serv, 
	unsigned int            servlen, 
	int                     flags
)
{

	struct afd *afd;
	struct servent *sp = NULL;
	unsigned short port;
#ifdef LWRES_PLATFORM_HAVESALEN
	unsigned int len;
#endif
	int family, i;
	const void *addr;
	char *p;
#if 0
	unsigned long v4a;
	unsigned char pfx;
#endif
#if 0
	char numserv[sizeof("65000")];
#endif
	char numaddr[sizeof("abcd:abcd:abcd:abcd:abcd:abcd:255.255.255.255")
		    + 1 + sizeof("4294967295")];
	const char *proto;
	uint_32 lwf = 0;
	lwres_context_t *lwrctx = NULL;
	lwres_gnbaresponse_t *by = NULL;
	int result = SUCCESS;
	int n;

	if (sa == NULL)
		ERR(ENI_NOSOCKET);

#ifdef LWRES_PLATFORM_HAVESALEN
	len = sa->sa_len;
	if (len != salen)
		ERR(ENI_SALEN);
#endif

	/*family = sa->sa_family;*/
	family = sa->sa_family;
	
	for (i = 0; afdl[i].a_af; i++)
		if (afdl[i].a_af == family) {
			afd = &afdl[i];
			goto found;
		}
	ERR(ENI_FAMILY);

 found:
	if (salen != afd->a_socklen)
		ERR(ENI_SALEN);

	switch (family) {
	case AF_INET:
		port = ((const struct sockaddr_in *)sa)->sin_port;
		addr = &((const struct sockaddr_in *)sa)->sin_addr.s_addr;
		break;

	case AF_INET6:
		port = ((const struct sockaddr_in6 *)sa)->sin6_port;
		addr = ((const struct sockaddr_in6 *)sa)->sin6_addr.s6_addr;
		break;

	default:
		port = 0;
		addr = NULL;
	//	INSIST(0);//#define INSIST(x)		assert(x)
	}
	proto = (flags & NI_DGRAM) ? "udp" : "tcp";

	if (serv == NULL || servlen == 0U) {
		/*
		 * Caller does not want service.
		 */
	#if 0 /*We do not support getservbyport function */ 
	    }
		else if ((flags & NI_NUMERICSERV) != 0 ||(sp = getservbyport(port, proto)) == NULL) 
	{
		snprintf(numserv, sizeof(numserv), "%d", ntohs(port));
		if ((strlen(numserv) + 1) > servlen)
		{	
			ERR(ENI_MEMORY);
		}
		strcpy(serv, numserv);
	} 
	#endif
	}else {
		if ((strlen(sp->s_name) + 1) > servlen)
		{
			ERR(ENI_MEMORY);
		}
		strcpy(serv, sp->s_name);
	}

	if (host == NULL || hostlen == 0U) {
		/*
		 * What should we do?
		 */
	} else if (flags & NI_NUMERICHOST) {
		if (inet_ntop(afd->a_af, addr, numaddr, sizeof(numaddr)) == NULL)
		{
			ERR(ENI_SYSTEM);
		}
		if (afd->a_af == AF_INET6 && ((const struct sockaddr_in6 *)sa)->sin6_scope_id) 
		{
			char *p = numaddr + strlen(numaddr);
			const char *stringscope = NULL;
#if 0
			if ((flags & NI_NUMERICSCOPE) == 0) {
				/*
				 * Vendors may want to add support for
				 * non-numeric scope identifier.
				 */
				stringscope = foo;
			}
#endif
			if (stringscope == NULL) 
			{
			  //snprintf(p, sizeof(numaddr) - (p - numaddr),"%%%u",((const struct sockaddr_in6 *)sa)->sin6_scope_id);
				sprintf(p,"%%%u",((const struct sockaddr_in6 *)sa)->sin6_scope_id);
			} else {
			  //snprintf(p, sizeof(numaddr) - (p - numaddr),"%%%s", stringscope);
				sprintf(p,"%%%s", stringscope);
			}
		}
//#endif
		if (strlen(numaddr) + 1 > hostlen)
			ERR(ENI_MEMORY);
		strcpy(host, numaddr);
	} else {
		switch (family) {
		case AF_INET:
			lwf = LWRES_ADDRTYPE_V4;
			break;
		case AF_INET6:
			lwf = LWRES_ADDRTYPE_V6;
			break;
		default:
			return (ENI_FAMILY);
		}

		n = lwres_context_create(&lwrctx, NULL, NULL, NULL, 0);
		if (n == 0)
		{	
			//(void) lwres_conf_parse(lwrctx, lwres_resolv_conf);
		}
		if (n == 0)
		{	
			//n = lwres_getnamebyaddr(lwrctx, lwf, (lwres_uint16_t)afd->a_addrlen,addr, &by);
		}
		if (n == 0) 
		{
			if (flags & NI_NOFQDN) 
			{
				p = strchr(by->realname, '.');
				if (p)
				{
					*p = '\0';
				}
			}
			if ((strlen(by->realname) + 1) > hostlen)
			{
				ERR(ENI_MEMORY);
			}
			strcpy(host, by->realname);
		} else {
			if (flags & NI_NAMEREQD)
			{
				ERR(ENI_NOHOSTNAME);
			}
			if (inet_ntop(afd->a_af, addr, numaddr,sizeof(numaddr))== NULL)
			{
				ERR(ENI_NOHOSTNAME);
			}
			if ((strlen(numaddr) + 1) > hostlen)
			{
				ERR(ENI_MEMORY);
			}
			strcpy(host, numaddr);
		}
	}
	result = SUCCESS;
 cleanup:
	if (by != NULL)
		lwres_gnbaresponse_free(lwrctx, &by);
	if (lwrctx != NULL) {
		lwres_conf_clear(lwrctx);
		lwres_context_destroy(&lwrctx);
	}
	return (result);
}




/*%
 * Creates a #lwres_context_t structure for use in
 *  lightweight resolver operations.
 */
uint_32 lwres_context_create
(
	lwres_context_t 	  **contextp, 
	void 				   *arg,
	lwres_malloc_t 			malloc_function,
	lwres_free_t 			free_function,
	unsigned int 			flags
)
{
	lwres_context_t *ctx;

	//REQUIRE(contextp != NULL && *contextp == NULL);

	/*
	 * If we were not given anything special to use, use our own
	 * functions.  These are just wrappers around malloc() and free().
	 */
	if (malloc_function == NULL || free_function == NULL) {
	//	REQUIRE(malloc_function == NULL);
	//	REQUIRE(free_function == NULL);
		malloc_function = lwres_malloc;
		free_function = lwres_free;
	}

	ctx = malloc_function(arg, sizeof(lwres_context_t));
	if (ctx == NULL)
		return (LWRES_R_NOMEMORY);

	/*
	 * Set up the context.
	 */
	ctx->malloc = malloc_function;
	ctx->free = free_function;
	ctx->arg = arg;
	ctx->sock = -1;

	ctx->r_timeout = LWRES_DEFAULT_TIMEOUT;
//b23362
	//ctx->serial = time(NULL); /* XXXMLG or BEW */

	ctx->use_ipv4 = 1;
	ctx->use_ipv6 = 1;
	if ((flags & (LWRES_CONTEXT_USEIPV4 | LWRES_CONTEXT_USEIPV6)) ==
	    LWRES_CONTEXT_USEIPV6) {
		ctx->use_ipv4 = 0;
	}
	if ((flags & (LWRES_CONTEXT_USEIPV4 | LWRES_CONTEXT_USEIPV6)) ==
	    LWRES_CONTEXT_USEIPV4) {
		ctx->use_ipv6 = 0;
	}

	/*
	 * Init resolv.conf bits.
	 */
	lwres_conf_init(ctx);

	*contextp = ctx;
	return (LWRES_R_SUCCESS);
}

/*% Release the memory in resolver context ctx that was allocated to the lwres_gnbaresponse_t. */
void lwres_gnbaresponse_free
(
	lwres_context_t *ctx, 
	lwres_gnbaresponse_t **structp
)
{
	lwres_gnbaresponse_t *gnba;

	//REQUIRE(ctx != NULL);
	//REQUIRE(structp != NULL && *structp != NULL);

	gnba = *structp;
	*structp = NULL;

	if (gnba->naliases > 0) {
		CTXFREE(gnba->aliases, sizeof(char *) * gnba->naliases);
		CTXFREE(gnba->aliaslen,
			sizeof(uint_16) * gnba->naliases);
	}
	if (gnba->base != NULL)
		CTXFREE(gnba->base, gnba->baselen);
	CTXFREE(gnba, sizeof(lwres_gnbaresponse_t));
}


/*% Frees up all the internal memory used by the config data structure, returning it to the lwres_context_t. */
void lwres_conf_clear
(
	lwres_context_t *ctx
) 
{
	int i;
	lwres_conf_t *confdata;

	//REQUIRE(ctx != NULL);
	confdata = &ctx->confdata;

	for (i = 0; i < confdata->nsnext; i++)
		lwres_resetaddr(&confdata->nameservers[i]);

	if (confdata->domainname != NULL) {
		CTXFREE(confdata->domainname,
			strlen(confdata->domainname) + 1);
		confdata->domainname = NULL;
	}

	for (i = 0; i < confdata->searchnxt; i++) {
		if (confdata->search[i] != NULL) {
			CTXFREE(confdata->search[i],
				strlen(confdata->search[i]) + 1);
			confdata->search[i] = NULL;
		}
	}

	for (i = 0; i < LWRES_CONFMAXSORTLIST; i++) {
		lwres_resetaddr(&confdata->sortlist[i].addr);
		lwres_resetaddr(&confdata->sortlist[i].mask);
	}

	confdata->nsnext = 0;
	confdata->lwnext = 0;
	confdata->domainname = NULL;
	confdata->searchnxt = 0;
	confdata->sortlistnxt = 0;
	confdata->resdebug = 0;
	confdata->ndots = 1;
	confdata->no_tld_query = 0;
}


/*%
Destroys a #lwres_context_t, closing its socket.
contextp is a pointer to a pointer to the context that is
to be destroyed. The pointer will be set to NULL
when the context has been destroyed.
 */
void lwres_context_destroy
(
	lwres_context_t **contextp
) 
{
	lwres_context_t *ctx;

//	REQUIRE(contextp != NULL && *contextp != NULL);

	ctx = *contextp;
	*contextp = NULL;

//b23362
//	if (ctx->sock != -1) {
//
//		(void)close(ctx->sock);
//		ctx->sock = -1;
//	}

	CTXFREE(ctx, sizeof(lwres_context_t));
}


void *lwres_malloc
(
	void 	*arg, 
	unsigned int 	len
) 
{
	void *mem;
	//UNUSED(arg);

	mem =  RTCS_mem_alloc_system((_mem_size) len);
	//mem = malloc(len);
	if (mem == NULL)
		return (NULL);

	memset(mem, 0xe5, len);

	return (mem);
}

void lwres_free
(
	void *arg, 
	void *mem, 
	unsigned int len
) 
{
//	UNUSED(arg);

	memset(mem, 0xa9, len);
	_mem_free(mem);
}

/*% intializes data structure for subsequent config parsing. */
void lwres_conf_init
(
	lwres_context_t *ctx
) 
{
	int i;
	lwres_conf_t *confdata;

//	REQUIRE(ctx != NULL);
	confdata = &ctx->confdata;

	confdata->nsnext = 0;
	confdata->lwnext = 0;
	confdata->domainname = NULL;
	confdata->searchnxt = 0;
	confdata->sortlistnxt = 0;
	confdata->resdebug = 0;
	confdata->ndots = 1;
	confdata->no_tld_query = 0;

	for (i = 0; i < LWRES_CONFMAXNAMESERVERS; i++)
		lwres_resetaddr(&confdata->nameservers[i]);

	for (i = 0; i < LWRES_CONFMAXSEARCH; i++)
		confdata->search[i] = NULL;

	for (i = 0; i < LWRES_CONFMAXSORTLIST; i++) {
		lwres_resetaddr(&confdata->sortlist[i].addr);
		lwres_resetaddr(&confdata->sortlist[i].mask);
	}
}


void lwres_resetaddr
(
	lwres_addr_t *addr
) 
{
//	REQUIRE(addr != NULL);

	memset(addr->address, 0, LWRES_ADDR_MAXLEN);
	addr->family = 0;
	addr->length = 0;
}
