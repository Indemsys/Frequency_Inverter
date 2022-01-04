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
* $FileName: trap_receivers.c$
* $Version : 3.8.6.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   
*
*END************************************************************************/
#include <rtcs.h>
#include "rtcs_prv.h"
#include "snmpcfg.h"
#include "asn1.h"
#include "snmp.h"

#if RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_IP4 
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCS_insert_trap_receiver_internal
*  Parameters     :
*
*     _ip_address          address     will be inserted into 
*                                      RTCS_trap_receivers_table
*
*  Comments       :
*        Registers an SNMP trap receiver with RTCS.
*
*END*-----------------------------------------------------------------*/

void RTCS_insert_trap_receiver_internal
   (
      TRAP_PARM _PTR_ parm
   )
{ /* Body */
   volatile SNMP_PARSE_PTR    snmpcfg = SNMP_get_data();
   _ip_address       ip_addr = parm->address;
   _mqx_uint         i;
   
   /* Find an empty entry in the receivers list */
   for (i = 0; i < SNMPCFG_MAX_TRAP_RECEIVERS; i++) {
      if (snmpcfg->trap_receiver_list[i] == 0) {
         /* Found a space */
         snmpcfg->trap_receiver_list[i] = ip_addr;
         break;
      } /* Endif */     
   } /* Endfor */
      
   if (i < SNMPCFG_MAX_TRAP_RECEIVERS) {
      RTCSCMD_complete(parm, RTCS_OK);
   } else {
      RTCSCMD_complete(parm, RTCSERR_TRAP_INSERT_FAILED);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCS_remove_trap_receiver_internal
*  Parameters     :
*
*     _ip_address          address     will be removed from 
*                                      RTCS_trap_receivers_table
*
*  Comments       :
*        Unregisters an SNMP trap receiver from RTCS.
*
*END*-----------------------------------------------------------------*/

void RTCS_remove_trap_receiver_internal
   (
      TRAP_PARM _PTR_ parm
   )
{ /* Body */
   volatile SNMP_PARSE_PTR    snmpcfg = SNMP_get_data();
   _ip_address       ip_addr = parm->address;
   _mqx_uint         i;
   
   /* Traverse the receivers list looking for our entry */
   for (i = 0; i < SNMPCFG_MAX_TRAP_RECEIVERS; i++) {
      if (snmpcfg->trap_receiver_list[i] == ip_addr) {
         /* Found it */
         snmpcfg->trap_receiver_list[i] = 0;
         break;
      } /* Endif */     
   } /* Endfor */
      
   if (i < SNMPCFG_MAX_TRAP_RECEIVERS) {
      RTCSCMD_complete(parm, RTCS_OK);
   } else {
      RTCSCMD_complete(parm, RTCSERR_TRAP_REMOVE_FAILED);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : RTCS_count_trap_receivers_internal
*  Parameters     :
*  Comments       : Returns the number of receivers installed
*
*END*-----------------------------------------------------------------*/

_mqx_uint RTCS_count_trap_receivers_internal
   (
      void
   )
{ /* Body */
    volatile SNMP_PARSE_PTR    snmpcfg = SNMP_get_data();
    _mqx_uint         i;
    _mqx_uint         count = 0;

    for (i = 0; i < SNMPCFG_MAX_TRAP_RECEIVERS; i++) {
       if (snmpcfg->trap_receiver_list[i] != 0) {
         count++;
       } /* Endif */     
    } /* Endfor */
   
   return count;
} /* Endbody */

#endif /* RTCSCFG_ENABLE_SNMP && RTCSCFG_ENABLE_IP4 */
/* EOF */
