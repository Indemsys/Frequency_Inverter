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
* $FileName: alggen.c$
* $Version : 3.0.2.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the generic NAT ALG code.
*
*END************************************************************************/

#include <rtcs.h>
#include <rtcs_prv.h>
#include <fio.h>
#include <tftp.h>

#if RTCSCFG_ENABLE_NAT

#include "nat.h"
#include "nat_prv.h"


/* 
** List of function pointers used to free ALG specific data 
** Index into table is ALG_TYPE.
*/
NAT_ALG_FREE_FUNC NAT_alg_free_func_table[] =  {
   NULL,                /* NAT_ALG_TFTP_TYPE */
   RTCS_part_free       /* NAT_ALG_FTP_TYPE */
};

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : NAT_init_algs
* Returned Value  : error code
* Comments        :
*        Initializes all of the ALGs in the NAT_alg_table.
*
*END*-----------------------------------------------------------------*/

uint_32 NAT_init_algs
   (
      NAT_CFG_STRUCT_PTR   nat_cfg_ptr     /* [IN] NAT config struct */
   )
{ /* Body */
   NAT_ALG _PTR_           table_ptr = NAT_alg_table;
   NAT_ALG_CFG_STRUCT_PTR  cfg_ptr, next_cfg_ptr;
   uint_32                 error = RTCS_OK;
   
   while ((*table_ptr != NAT_ALG_ENDLIST) && (error==RTCS_OK))  {
      error = (*table_ptr)(nat_cfg_ptr);   
      table_ptr++;   
   } /* Endfor */

   if (error != RTCS_OK)  {
      /* An error occured. Free memory associated with ALGs */
      cfg_ptr = nat_cfg_ptr->ALG_INFO_PTR;
      while (cfg_ptr)  {
         next_cfg_ptr = cfg_ptr->NEXT;     
         _mem_free(cfg_ptr);
         cfg_ptr = next_cfg_ptr;  
      } /* Endwhile */
      
      nat_cfg_ptr->ALG_INFO_PTR = NULL;   
   } /* Endif */

   return error;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : NAT_ALG_TCP_checksum
* Returned Value : void
* Comments       : Clears current value in the checksum field and
*                  recalculates it.
*
*END------------------------------------------------------------------*/

void NAT_ALG_TCP_checksum
   (
      IP_HEADER_PTR    ip_header_ptr    /* [IN]  pointer to IP header */
   )
{ /* Body */
   TRANSPORT_UNION    transport;
   uint_16           checksum;
   uint_16           protocol;
   uint_16           iplen = IPH_LEN(ip_header_ptr);
   uint_16           len = ntohs(ip_header_ptr->LENGTH) - iplen; 
   
   /* Get TCP header */
   transport.PTR = TRANSPORT_PTR(ip_header_ptr);

   htons(transport.TCP_PTR->checksum, 0);    /* Clear checksum field */
   protocol = ntohc(ip_header_ptr->PROTOCOL); /* PROTOCOL */
   checksum = (uint_16) _mem_sum_ip(protocol, 8, ip_header_ptr->SOURCE);  /* IP SRC and DST ADDR */
   checksum = (uint_16) _mem_sum_ip(checksum, len, transport.PTR);

   /* TCP LENGTH */
   checksum = IP_Sum_immediate(checksum, len);
   
   checksum = IP_Sum_invert(checksum);
   htons(transport.TCP_PTR->checksum, checksum);   

} /* Endbody */

#endif

