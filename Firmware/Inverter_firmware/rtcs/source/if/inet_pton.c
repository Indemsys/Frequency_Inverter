/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: inet_pton.c$
* $Version : 3.8.6.0$
* $Date    : Jun-14-2012$
*
*END************************************************************************/

/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1996,1999 by Internet Software Consortium.
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

static int_32	inet_pton4(const char *src, in_addr *dst);
static int_32	inet_pton6(const char *src, in6_addr *dst);

/****************************************************************************************
 * int
 * inet_pton(af, src, dst)
 *	convert from presentation format (which usually means ASCII printable)
 *	to network format (which is usually some kind of binary format).
 * return:
 *	RTCS_OK if the address was valid for the specified address family
 *	RTCS_ERROR if the address wasn't valid (`dst' is untouched in this case)
 *	RTCS_ERROR if some other error occurred (`dst' is untouched in this case, too)
 ***************************************************************************************/
int_32 inet_pton (
                     int_32 af,               /*[IN]  family name                     */
                     const char *src,         /*[IN]  pointer to prn form of address  */
                     void *dst,               /*[OUT] pointer to bin form of address  */
                     unsigned int sizeof_dst  /*[IN]  size of dst buffer              */
                 )
{
	switch (af) {
	case AF_INET:
	    /*checking value sizeof_dst of output buffer to avoid overflow*/
        if(sizeof_dst < sizeof( *((in_addr*)dst) ))
	    {
	        return (RTCS_ERROR);
	    }
		return (inet_pton4(src, dst));
	case AF_INET6:
		/*checking value sizeof_dst of output buffer to avoid overflow*/
        if(sizeof_dst < sizeof(*((in6_addr*)dst)) )
	    {
	        return (RTCS_ERROR);
	    }
		return (inet_pton6(src, dst));
	default:
		return (RTCS_ERROR);
	}
}


/****************************************************************************************
 * int
 * inet_pton4(src, dst)
 *	like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *	RTCS_OK if `src' is a valid dotted quad, else RTCS_ERROR.
 * notice:
 *	does not touch `dst' unless it's returning 1.
 ****************************************************************************************/

static int_32 inet_pton4(
                            const char *src, /*[IN]  pointer to inet4 address in prn format */
                            in_addr *dst     /*[OUT] pointer to inet4 address in bin format */
                         )
{
	static const char digits[] = "0123456789";
	int saw_digit, octets, ch;
	uchar tmp[sizeof(*dst)], *tp;
    uint_32 temp=0;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0')
	{
		const char *pch;

		if ((pch = strchr(digits, ch)) != NULL)
		{
			unsigned int new = *tp * 10 + (pch - digits);

			if (saw_digit && *tp == 0)
			{
				return  (RTCS_ERROR);
			}
			if (new > 255)
			{
				return(RTCS_ERROR);
			}
			*tp = new;
			if (!saw_digit)
			{
				if (++octets > 4)
				{
					return(RTCS_ERROR);
				}
				saw_digit = 1;
			}
		}
		else if (ch == '.' && saw_digit)
		{
			if (octets == 4)
			{
				return(RTCS_ERROR);
			}
			*++tp = 0;
			saw_digit = 0;
		}
		else
		{
			return(RTCS_ERROR);
        }
	}
	if (octets < 4)
	{
		return(RTCS_ERROR);
	}

	memcpy(&temp, tmp, sizeof(temp));
	htonl(dst,temp);




	return (RTCS_OK);
}

/****************************************************************************************
 * int
 * inet_pton6(src, dst)
 *	convert presentation level address to network order binary form.
 * return:
 *	RTCS_OK if `src' is a valid [RFC1884 2.2] address, else RTCS_ERROR.
 * notice:
 *	(1) does not touch `dst' unless it's returning 1.
 *	(2) :: in a full address is silently ignored.
 * credit:
 *	inspired by Mark Andrews.
 ****************************************************************************************/
static int_32 inet_pton6(
                            const char *src,/*[IN]  pointer to inet6 address in prn format */
                            in6_addr *dst   /*[OUT] pointer to inet6 address in bin format */
                         )
{
	static const char   xdigits_l[] = "0123456789abcdef",
			            xdigits_u[] = "0123456789ABCDEF";
	uchar tmp[sizeof(*dst)], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, seen_xdigits;
	unsigned int val;

	#ifndef	NS_INADDRSZ
	    #define NS_INADDRSZ     4
	#endif

	#ifndef NS_INT16SZ
		#define NS_INT16SZ		2
	#endif

	memset((tp = tmp), '\0', sizeof(*dst));
	endp = tp + sizeof(*dst);
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return(RTCS_ERROR);
	curtok = src;
	seen_xdigits = 0;
	val = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) {
			val <<= 4;
			val |= (pch - xdigits);
			if (++seen_xdigits > 4)
				return(RTCS_ERROR);
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!seen_xdigits) {
				if (colonp)
					return(RTCS_ERROR);
				colonp = tp;
				continue;
			} else if (*src == '\0') {
				return(RTCS_ERROR);
			}
			if (tp + NS_INT16SZ > endp)
				return(RTCS_ERROR);
			*tp++ = (uchar) (val >> 8) & 0xff;
			*tp++ = (uchar) val & 0xff;
			seen_xdigits = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) > 0)
		{
		    if(inet_pton4(curtok,(in_addr*)tp) == RTCS_OK)
		    {
			    tp += NS_INADDRSZ;
			    seen_xdigits = 0;
			    break;	/*%< '\\0' was seen by inet_pton4(). */
            }
		}
		return(RTCS_ERROR);
	}
	if (seen_xdigits) {
		if (tp + NS_INT16SZ > endp)
			return(RTCS_ERROR);
		*tp++ = (uchar) (val >> 8) & 0xff;
		*tp++ = (uchar) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		if (tp == endp)
			return(RTCS_ERROR);
		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return(RTCS_ERROR);
	memcpy(dst, tmp, sizeof(*dst));
	return (RTCS_OK);
}

