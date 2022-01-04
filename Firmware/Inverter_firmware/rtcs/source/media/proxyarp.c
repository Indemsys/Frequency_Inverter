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
* $FileName: proxyarp.c$
* $Version : 3.8.4.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains a hook into ARP to allow an
*   application to do ProxyARP.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4

#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_do_proxy
*  Returned Value : TRUE or FALSE
*  Comments       :
*        Decides whether or not to send ARP replies on behalf
*        of another IP host.  Default is to never do ProxyARP,
*        but this function can be replaced in an application.
*
*END*-----------------------------------------------------------------*/

boolean ARP_do_proxy
   (
      IP_IF_PTR      iflocal,
            /* [IN] the interface that the ARP request arrived on */
      _ip_address    iplocal
            /* [IN] the IP address to test */
   )
{ /* Body */

   return FALSE;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
