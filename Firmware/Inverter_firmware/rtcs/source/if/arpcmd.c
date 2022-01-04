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
* $FileName: arpcmd.c$
* $Version : 3.8.5.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains the interface functions to the
*   packet driver interface.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"

#include "arpif.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_arp_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Add an entry to an interfaces ARP cache.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_arp_add
   (
      _rtcs_if_handle _PTR_   ihandle,
         /* [IN] the RTCS interface state structure */
      _ip_address              paddr,
         /* [IN] the address to add */
      char           laddr[6]
   )
{

#if RTCSCFG_ENABLE_IP4

    ARPIF_PARM   parms;
    uint_32     error;

    parms.ihandle  = ihandle;
    parms.PADDR  = paddr;
    _mem_copy(laddr, parms.LADDR, 6); 

   error = RTCSCMD_issue(parms, ARPIF_add);

   return error;

   
#else

   return RTCSERR_IP_IS_DISABLED;
   
#endif /* RTCSCFG_ENABLE_IP4 */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_arp_delete
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Add an entry to an interfaces ARP cache.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_arp_delete
   (
      _rtcs_if_handle _PTR_   ihandle,
         /* [IN] the RTCS interface state structure */
      _ip_address                paddr
         /* [IN] the address to add */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   ARPIF_PARM   parms;
   uint_32     error;

   parms.ihandle  = ihandle;
   parms.PADDR  = paddr;

   error = RTCSCMD_issue(parms, ARPIF_delete);

   return error;
   
#else

   return RTCSERR_IP_IS_DISABLED;
   
#endif /* RTCSCFG_ENABLE_IP4 */


} /* Endbody */


/* EOF */
