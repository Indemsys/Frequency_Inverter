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
* $FileName: enrecv.c$
* $Version : 3.0.4.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the Ethernet receive
*   support function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_find_receiver
*  Returned Value : ECB for received packet, or NULL.
*  Comments       :
*        Finds an application for a received packet.
*
*END*-----------------------------------------------------------------*/

ENET_ECB_STRUCT_PTR ENET_find_receiver
   (
      /* [IN] the Ethernet state structure */
      ENET_CONTEXT_STRUCT_PTR  enet_ptr,

      /* [IN] the received packet */
      ENET_HEADER_PTR      packet_ptr,
      
      uint_32_ptr          length_ptr
   )
{ 
   ENET_ECB_STRUCT_PTR  ecb_ptr;
   ENET_MCB_STRUCT_PTR  mcb_ptr;
   uchar_ptr            type_ptr;
   uint_32              hdrlen;
   _enet_address        dest;
   uint_16              type;


   hdrlen = sizeof(ENET_HEADER);
   type_ptr = packet_ptr->TYPE;
   type = ntohs(type_ptr);

   if (type == ENETPROT_8021Q) {
      ENET_8021QTAG_HEADER_PTR tag_ptr = (ENET_8021QTAG_HEADER_PTR)(type_ptr+2);
      hdrlen += sizeof(ENET_8021QTAG_HEADER);
      type_ptr = tag_ptr->TYPE;
      type = ntohs(type_ptr);
   } 

   if (type <= ENETPROT_LENGTH_TYPE_BOUNDARY) {               
      ENET_8022_HEADER_PTR llc_ptr = (ENET_8022_HEADER_PTR)(type_ptr+2);
      if ((ntohc(llc_ptr->DSAP) != 0xAA)
       || (ntohc(llc_ptr->SSAP) != 0xAA)) {
         return NULL;
      } 
      if (*length_ptr < hdrlen + type) {
         return NULL;
      } 
      *length_ptr = hdrlen + type;
      type_ptr = llc_ptr->TYPE;
      type = ntohs(type_ptr);
   } 

   for (ecb_ptr = enet_ptr->ECB_HEAD; ecb_ptr; ecb_ptr = ecb_ptr->NEXT) {
      if (ecb_ptr->TYPE == type) {

         ntohe(packet_ptr->DEST, dest);
         if ((dest[0] & 1) && !((dest[0] == 0xFF)
                             && (dest[1] == 0xFF)
                             && (dest[2] == 0xFF)
                             && (dest[3] == 0xFF)
                             && (dest[4] == 0xFF)
                             && (dest[5] == 0xFF))) {

            /*
            ** The destination is a multicast address.
            ** Check the joined mulicast groups.
            */
            for (mcb_ptr = ecb_ptr->MCB_HEAD; mcb_ptr; mcb_ptr = mcb_ptr->NEXT) {
               if ((dest[0] == mcb_ptr->GROUP[0])
                && (dest[1] == mcb_ptr->GROUP[1])
                && (dest[2] == mcb_ptr->GROUP[2])
                && (dest[3] == mcb_ptr->GROUP[3])
                && (dest[4] == mcb_ptr->GROUP[4])
                && (dest[5] == mcb_ptr->GROUP[5])) {
                  break;
               } 
            } 

            if (!mcb_ptr) {
               /*
               ** We received a packet multicasted to a group we
               ** haven't joined.  Break out of the big for loop
               ** and discard the packet.  We don't continue the
               ** big for loop because there is only one ECB per
               ** type and we already found it.
               */
               ecb_ptr = NULL;
            } 
         } 

         break;
      } 
   } 

   return ecb_ptr;

} 


/* EOF */
