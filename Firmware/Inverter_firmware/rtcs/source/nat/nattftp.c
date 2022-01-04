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
* $FileName: nattftp.c$
* $Version : 3.6.3.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the NAT TFTP ALG.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>
#include <rtcs_prv.h>
#include <tftp.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_ALG_tftp
* Returned Value  : error code
* Comments        :
*        Initializes the NAT TFTP ALG.
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_ALG_tftp
   (
      pointer ptr       /* [IN] pointer to NAT config struct */
   )
{ /* Body */
   NAT_CFG_STRUCT_PTR      nat_cfg_ptr = ptr;
   NAT_ALG_CFG_STRUCT_PTR  tftp_ptr;

   tftp_ptr = _mem_alloc_system(sizeof(NAT_ALG_CFG_STRUCT));
   
   if (tftp_ptr)  {
      tftp_ptr->NEXT = nat_cfg_ptr->ALG_INFO_PTR;
      nat_cfg_ptr->ALG_INFO_PTR = tftp_ptr;  
      tftp_ptr->ALG_TYPE = NAT_ALG_TFTP_TYPE;
      tftp_ptr->ALG_EXEC = NAT_ALG_tftp_apply;   
      return RTCS_OK;
   } /* Endif */
   
   return RTCSERR_OUT_OF_MEMORY;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_ALG_tftp_apply
* Returned Value  : error code
* Comments        :
*        Applies the NAT TFTP ALG to a packet.
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_ALG_tftp_apply
   (
      RTCSPCB_PTR _PTR_    pcb_ptr_ptr,               /* [IN/OUT] PCB containing packet   */
      boolean              pub_to_prv,                /* [IN] Direction of packet         */
      pointer              alg_cfg_ptr,               /* [IN] Pointer to TFTP config      */
      pointer _PTR_        session_ptr_ptr   /* [OUT] Pointer to session         */
   )
{ /* Body */
   NAT_SESSION_STRUCT_PTR  nat_session_ptr = *((NAT_SESSION_STRUCT_PTR *)session_ptr_ptr);
   uchar_ptr               data_ptr = (pointer)RTCSPCB_DATA(*pcb_ptr_ptr);
   IP_HEADER_PTR           ip_header_ptr = (IP_HEADER_PTR)(pointer)data_ptr;
   TRANSPORT_UNION         transport;
   TFTP_HEADER_PTR         tftp_header_ptr;
   uint_16                 opcode, block;
 
   if (nat_session_ptr == NULL) {
      return RTCS_OK;
   } /* Endif */
   
   
   /* TFTP uses UDP */
   if (ntohc(ip_header_ptr->PROTOCOL) != IPPROTO_UDP) {
      return RTCS_OK;
   } /* Endif */
      
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);
   
   /* The UDP length should be long enough to contain a TFTP header */
   if (ntohs(transport.UDP_PTR->LENGTH) < sizeof(UDP_HEADER) + sizeof(TFTP_HEADER)) {
      return RTCS_OK;
   } /* Endif */         
   
   tftp_header_ptr = (TFTP_HEADER_PTR)(pointer)(transport.UDP_PTR + 1);

   opcode = ntohs(tftp_header_ptr->OP);
   block = ntohs(tftp_header_ptr->BLOCK);
   
   /*
   ** A normal reply should have:  OP 3 (DATA) and BLOCK 1 (first block) or
   ** an OP 4 and BLOCK 0. Error replies should have: OP 5, and a variable 
   ** error code 
   */
   if ((opcode != 3 || block != 1) &&
       (opcode != 4 || block != 0) &&
       (opcode != 5))   
   {
      return RTCS_OK;
   } /* Endif */      

   if (nat_session_ptr->SNAT_OR_DNAT == SNAT) {
      /* We only need to consider the first packet returned by the server */
      if (!pub_to_prv) {
         return RTCS_OK;
      } /* Endif */

      /* Make sure port number is the TFTP port number */
      if (nat_session_ptr->PUB_PORT != IPPORT_TFTP) {
         return RTCS_OK;
      } /* Endif */
      
      /* Set new server port number */
      nat_session_ptr->PUB_PORT = ntohs(transport.UDP_PTR->SRC_PORT);
   } else {
      /* We only need to consider the first packet returned by the server */
      if (pub_to_prv){
         return RTCS_OK;
      } /* Endif */

      /* Make sure port number is the TFTP port number */
      if (nat_session_ptr->PRV_PORT != IPPORT_TFTP) {
         return RTCS_OK;
      } /* Endif */
      
      /* Set new server port number */
      nat_session_ptr->PRV_PORT = ntohs(transport.UDP_PTR->SRC_PORT);       
   } /* Endif */
   
   return RTCS_OK;   
} /* Endbody */

#endif

/* EOF */
