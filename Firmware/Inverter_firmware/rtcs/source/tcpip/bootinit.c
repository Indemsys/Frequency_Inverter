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
* $FileName: bootinit.c$
* $Version : 3.8.4.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the initialization of the
*   BOOTP and DHCP clients.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4

#include "rtcs_prv.h"
#include "tcpip.h"


uint_32          BOOT_count;
UCB_STRUCT_PTR   BOOT_port;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : BOOT_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the BOOTP and DHCP clients.
*
*END*-----------------------------------------------------------------*/

uint_32 BOOT_init
   (
      void
   )
{ /* Body */

   BOOT_count = 0;
   BOOT_port = NULL;
   return RTCS_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
