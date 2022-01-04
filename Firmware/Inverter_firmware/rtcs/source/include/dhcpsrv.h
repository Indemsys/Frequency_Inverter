#ifndef __dhcpsrv_h__
#define __dhcpsrv_h__
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
* $FileName: dhcpsrv.h$
* $Version : 3.6.6.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   Dynamic Host Configuration Protocol definitions.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

#define DHCPTIME_OFFER  10       /* 10 seconds */

/* Start CR 1547 */
#define DHCPSVR_FLAG_DO_PROBE   0x00000001
/* End CR 1547 */

/***************************************
**
** Type definitions
**
*/

typedef struct dhcpsrv_options_struct {

   uint_32        COUNT;

   _ip_address    SERVERID;
   uint_32        LEASE;
   uint_32        MASK;

   uchar_ptr      OPTION_PTR;
   uint_32        OPTION_LEN;

   _ip_address    SADDR;
   uchar          SNAME[64];
   uchar          FILE[128];

} DHCPSRV_OPTIONS_STRUCT, _PTR_ DHCPSRV_OPTIONS_STRUCT_PTR;

typedef struct dhcpsrv_addr_struct {

   struct dhcpsrv_addr_struct  _PTR_ NEXT;
   uint_32                    EXPIRE;
   uint_32                    OFFER;
   uint_32                    CLIENTID_LEN;
   uchar_ptr                  CLIENTID_PTR;

   _ip_address                IP_ADDR;
   DHCPSRV_OPTIONS_STRUCT_PTR OPTIONS;

} DHCPSRV_ADDR_STRUCT, _PTR_ DHCPSRV_ADDR_STRUCT_PTR;

typedef struct dhcpsrv_state_struct {

   uchar                   SND_BUFFER[DHCP_MSGSIZE_MIN];
   uchar                   RCV_BUFFER[DHCP_MSGSIZE_MIN];
   uint_32                 SND_BUFFER_LEN;
   uint_32                 RCV_BUFFER_LEN;
   uint_32                 SOCKET;
   uint_32                 TIME;
   _rtcs_mutex             IPLIST_SEM;
   /* Start CR 1547 */
   uint_32                 FLAGS;
   /* End CR 1547 */
   DHCPSRV_ADDR_STRUCT_PTR IP_AVAIL;
   DHCPSRV_ADDR_STRUCT_PTR IP_OFFERED;
   DHCPSRV_ADDR_STRUCT_PTR IP_LEASED;
   /* Start CR 1547 */
   DHCPSRV_ADDR_STRUCT_PTR IP_TAKEN;
   /* End CR 1547 */

} DHCPSRV_STATE_STRUCT, _PTR_ DHCPSRV_STATE_STRUCT_PTR;


#endif
/* EOF */
