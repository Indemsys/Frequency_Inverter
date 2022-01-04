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
* $FileName: ifbootp.c$
* $Version : 3.8.7.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the interface functions for the
*   BOOTP client.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "bootp.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind_BOOTP
* Returned Values : uint_32 (error code)
* Comments        :
*     Initialize a hardware interface using BOOTP.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_if_bind_BOOTP
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      BOOTP_DATA_STRUCT_PTR   data_ptr
         /* [IN/OUT] BOOTP parameters */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_BOOTP        parms;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   
   parms.handle = handle;
   parms.data   = data_ptr;

   return RTCSCMD_issue(parms, BOOTP_open);

#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */     

} /* Endbody */


/* EOF */
