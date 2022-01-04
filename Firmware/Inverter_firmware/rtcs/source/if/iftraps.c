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
* $FileName: iftraps.c$
* $Version : 3.8.5.0$
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

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_trap_target_add
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register an IP address to direct SNMP traps to.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_trap_target_add(_ip_address ip_addr) {

#if RTCSCFG_ENABLE_IP4

   TRAP_PARM   parms;
   uint_32     error;

   parms.address = ip_addr;
   
   error = RTCSCMD_issue(parms, RTCS_insert_trap_receiver_internal);

   return error;

#else

    return RTCSERR_IP_IS_DISABLED; 

#endif /* RTCSCFG_ENABLE_IP4 */   

}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_trap_target_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register an IP address to direct SNMP traps to.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_trap_target_remove(_ip_address ip_addr) {

#if RTCSCFG_ENABLE_IP4

   TRAP_PARM   parms;
   uint_32     error;

   parms.address = ip_addr;
   
   error = RTCSCMD_issue(parms, RTCS_remove_trap_receiver_internal);

   return error;


#else

    return RTCSERR_IP_IS_DISABLED; 

#endif /* RTCSCFG_ENABLE_IP4 */     

}

/* EOF */
