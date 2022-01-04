/**HEADER*******************************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: inet_ntop.c$
* $Version : 3.8.3.0$
* $Date    : Aug-13-2012$
*
* Comments:
*
*   Convert a network format address to presentation format.
*
*END************************************************************************************/
/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1996-1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <ctype.h>
#include <rtcs.h>
#include <addrinfo.h>

static char	*inet_ntop4(const uchar *src, char *dst, socklen_t size);
static char	*inet_ntop6(const uchar *src, char *dst, socklen_t size);

/**************************************************************************************** 
 * char *
 * inet_ntop(af, src, dst, size)
 *	convert a network format address to presentation format.
 * return:
 *	pointer to presentation format address (`dst'), or NULL (see errno).
 * author:
 *	Paul Vixie, 1996.
 ****************************************************************************************/
char *inet_ntop(int_32 af, const void *src, char *dst, socklen_t size)
{
	switch (af) {
	case AF_INET:
		return (inet_ntop4(src, dst, size));
	case AF_INET6:
		return (inet_ntop6(src, dst, size));
	default:
		return (NULL);
	}
	/* NOTREACHED */
}

/**************************************************************************************** 
 * const char *
 * inet_ntop4(src, dst, size)
 *	format an IPv4 address
 * return:
 *	`dst' (as a const)
 * notes:
 *	(1) uses no statics
 *	(2) takes a u_char* not an in_addr as input
 *****************************************************************************************/
static char *inet_ntop4 (
                          const uchar  *h_src /*[IN]  inet4 address in bin format */,
                          char         *dst /*[OUT] inet4 address in prn format */,
                          socklen_t    size /*[IN]  sizeof dst buffer           */
                        )
{
	static const char fmt[] = "%u.%u.%u.%u";
	char tmp[sizeof "255.255.255.255"];
	int l;
    uint_32 h_address=0;
    uint_32 tmp_address=0;
    uchar *src;
    
   /* RTCS use always host presentation, so we will convert it to network presentation. */
    memcpy(&tmp_address, h_src, sizeof(h_address));
    htonl(&h_address,tmp_address);

    src = (uchar*)&h_address;
   /*l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
	* TBD
	* here we must use snprinf to avoid buffer overflow
    */
    l=sprintf(tmp,fmt,src[0], src[1], src[2], src[3]);
    
        
	if (l <= 0 || (socklen_t) l >= size) {
		return (NULL);
	}
	strlcpy(dst, tmp, size);
   /*
    * Return pointer to prn buffer.
	* It can be usefull for printf function.
	*/
	return (dst);
}

/*******************************************************************************************
 * const char *
 * inet_ntop6(src, dst, size)
 * convert IPv6 binary address into presentation (printable) format
 *******************************************************************************************/
static char *inet_ntop6 (
                          const uchar *src /*[IN]  pointer to inet6 address in bin format */,
                          char         *dst /*[OUT] pointer to inet6 address in prn format */,  
                          socklen_t    size /*[IN]  sizeof dst buffer                      */ 
                        )
{
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct { int base, len; } best, cur;

	#ifndef	NS_IN6ADDRSZ
		#define NS_IN6ADDRSZ	16
	#endif
	
	#ifndef NS_INT16SZ
		#define NS_INT16SZ		2
	#endif	
	
	unsigned int words[NS_IN6ADDRSZ / NS_INT16SZ];
	int i;

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	memset(words, '\0', sizeof words);
	for (i = 0; i < NS_IN6ADDRSZ; i++)
		words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
	best.base = -1;
	best.len = 0;
	cur.base = -1;
	cur.len = 0;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
			*tp++ = ':';
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 && (best.len == 6 ||
		    (best.len == 7 && words[7] != 0x0001) ||
		    (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4((src+12),tp, sizeof tmp - (tp - tmp)))
				return (NULL);
			tp += strlen(tp);
			break;
		}
		tp += sprintf(tp, "%x", words[i]);
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) == 
	    (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((socklen_t)(tp - tmp) > size) {
		return (NULL);
	}
	strcpy(dst, tmp);
	return (dst);
}


/*************************************************************************************	
 * $OpenBSD: strlcpy.c,v 1.11 2006/05/05 15:27:38 millert Exp $	
 *
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 *************************************************************************************/
unsigned int strlcpy (
                    char *dst           /*[OUT]  */, 
                    const char *src     /*[IN] */, 
                    unsigned int siz          /*[IN]  */
               )
{
	char *d = dst;
	const char *s = src;
	unsigned int n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst  */
		while (*s++)		/* to set ent to "\0" */
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

