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
* $FileName: dnshosts.c$
* $Version : 3.0.9.0$
* $Date    : Mar-5-2009$
*
* Comments:
*
*   This file contains the default hosts file structures
*   which contains the current list of root name servers.
*   It also contains the default local network name (for
*   appending to non-authoritative names (don't end in a '.')),
*   the default local server name struct, the default local
*   server list, and the initial Zone of Authority list.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP
#if RTCSCFG_ENABLE_DNS

#include "dns.h"

char DNS_Local_network_name[] = ".";
uint_32 DNS_CACHE_NAME_LIMIT = 0;

/***********************************************************************

 This section is for the Root Servers List.
 This should only be assigned to the DNS config_ptr->ROOT_SLIST_PTR

***********************************************************************/

DNS_SLIST_STRUCT DNS_M_Root_server[] =
      {{"M.ROOT-SERVERS.NET.", NULL,
        0xCA0C1B21, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_L_Root_server[] =
      {{"L.ROOT-SERVERS.NET.", DNS_M_Root_server,
        0xC620400C, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_K_Root_server[] =
      {{"K.ROOT-SERVERS.NET.", DNS_L_Root_server,
        0xC1000E81, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_J_Root_server[] =
      {{"J.ROOT-SERVERS.NET.", DNS_K_Root_server,
        0xC03A801E, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_I_Root_server[] =
      {{"I.ROOT-SERVERS.NET.", DNS_J_Root_server,
        0xC0249411, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_H_Root_server[] =
      {{"H.ROOT-SERVERS.NET.", DNS_I_Root_server,
        0x803F0235, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_G_Root_server[] =
      {{"G.ROOT-SERVERS.NET.", DNS_H_Root_server,
        0xC0702404, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_F_Root_server[] =
      {{"F.ROOT-SERVERS.NET.", DNS_G_Root_server,
        0xC00505F1, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_E_Root_server[] =
      {{"E.ROOT-SERVERS.NET.", DNS_F_Root_server,
        0xC0CBE60A, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_D_Root_server[] =
      {{"D.ROOT-SERVERS.NET.", DNS_E_Root_server,
        0x80080A5A, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_C_Root_server[] =
      {{"C.ROOT-SERVERS.NET.", DNS_D_Root_server,
        0xC021040C, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_B_Root_server[] =
      {{"B.ROOT-SERVERS.NET.", DNS_C_Root_server,
        0xC0E44FC9, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT DNS_A_Root_server[] =
      {{"A.ROOT-SERVERS.NET.", DNS_B_Root_server,
        0xC6290004, 0, 0, 0, 0, 0, DNS_A, DNS_IN }};



/***********************************************************************
 End of Root List Section
***********************************************************************/


/***********************************************************************

 This section is for the Local Servers List.
 This should only be assigned to the DNS config_ptr->LOCAL_SLIST_PTR

***********************************************************************/


char DNS_Local_server_name[] = "ns.arc.com.";


DNS_SLIST_STRUCT DNS_Local_server_list[] =
   {{DNS_Local_server_name, 0, INADDR_LOOPBACK, 0, 0, 0, 0,
     0, DNS_A, DNS_IN }};

DNS_SLIST_STRUCT _PTR_ DNS_First_Local_server = &DNS_Local_server_list[0];


/***********************************************************************
 End of Local List Section
***********************************************************************/


/***********************************************************************

 This section is for the Name Server initial Zone of Authority List.
 This should only be used by the Name Server and only assigned to
 the config_ptr->NS_ZONE_AUTH_LIST;

***********************************************************************/


/***********************************************************************
 End of Zone of Authority List Section
***********************************************************************/

#endif
#endif
/* EOF */
