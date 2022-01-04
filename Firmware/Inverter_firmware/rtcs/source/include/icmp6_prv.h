#ifndef __icmp6_prv_h__
#define __icmp6_prv_h__
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
* $FileName: icmp6_prv.h$
* $Version : 3.8.1.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Private definitions for the ICMP6 protocol layer.
*
*END************************************************************************/

#include "icmp6.h"
#include "ip6_prv.h"     /* For IP_HEADER */

#if RTCSCFG_ENABLE_ICMP6_STATS 
   #define IF_ICMP6_STATS_ENABLED(x) x
#else
   #define IF_ICMP6_STATS_ENABLED(x)
#endif

/***********************************************************************
*
* Type definitions
*
***********************************************************************/

/***********************************************************************
 * Generic ICMP packet header
 ***********************************************************************
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |     Code      |          Checksum             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * +                         Message Body                          +
 * |                                                               |
 *
 ***********************************************************************/      
typedef struct icmp6_header
{
   uchar    TYPE[1];
   uchar    CODE[1];
   uchar    CHECKSUM[2];
} ICMP6_HEADER, _PTR_ ICMP6_HEADER_PTR;


/***********************************************************************
 * ICMPv6 Echo packet
 ***********************************************************************
 * RFC4443 4:
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    Type       |       Code    |             Checksum          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |             Identifier        |       Sequence Number         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data ...
 * +-+-+-+-+- 
 ***********************************************************************/
typedef struct icmp6_echo_header
{
   ICMP6_HEADER HEAD;
   uchar       ID[2];
   uchar       SEQ[2];
} ICMP6_ECHO_HEADER, _PTR_ ICMP6_ECHO_HEADER_PTR;

/***********************************************************************
 * ICMPv6 Error packet
 ***********************************************************************/
typedef struct icmp6_err_header
{
   ICMP6_HEADER HEAD;
   uchar        DATA[4];
} ICMP6_ERR_HEADER, _PTR_ ICMP6_ERR_HEADER_PTR;

/***********************************************************************
 * ICMP Configuration.  This information is persistent for the ICMP layer.
 ***********************************************************************/
typedef struct icmp6_cfg_struct
{
#if RTCSCFG_ENABLE_ICMP6_STATS 
   ICMP6_STATS      STATS;
#endif
   ICMP6_PARM_PTR   ECHO_PARM_HEAD;
   uint_16          ECHO_SEQ;
   uint_16          RESERVED;
} ICMP6_CFG_STRUCT, _PTR_ ICMP6_CFG_STRUCT_PTR;

/*
* Error messages are identified as such by a
* zero in the high-order bit of their message Type field values. Thus,
* error messages have message types from 0 to 127; informational
* messages have message types from 128 to 255.
*/
#define ICMP6_TYPE_IS_ERROR(t) (((t) & 0x80) == 0x00)

#endif /* __icmp6_prv_h__ */


