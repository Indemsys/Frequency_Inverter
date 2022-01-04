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
* $FileName: tftp.c$
* $Version : 3.8.13.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the TFTP client implementation for
*   the Exec function of the RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

#include <fio.h>
#include "tftp.h"

/* The File Transfer client information to pass to RTCS_load() */
uint_32   TFTP_open  (pointer ft_data);
uchar_ptr TFTP_read  (uint_32_ptr size);
boolean   TFTP_eof   (void);
uint_32   TFTP_close (void);

static FT_CALL_STRUCT ft_tftp = {
   TFTP_open,
   TFTP_read,
   TFTP_eof,
   TFTP_close
};

const FT_CALL_STRUCT_PTR FT_TFTP = &ft_tftp;

/* TFTP state information */
static struct {
   TFTP_PACKET    PACKET;
   TFTP_HEADER    ACK;
   sockaddr_in    SADDR;
   uchar_ptr      RRQ_PTR;
   uint_32        RRQ_LEN;
   TFTP_TO_STRUCT TIMEOUT;
   uint_32        SOCK;
   boolean        LAST;
} TFTP_config;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTP_open
* Returned Value   : socket handle or RTCS_ERROR
* Comments  :  Routine to initiate a TFTP session.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_open
   (
      pointer  ft_data
         /* [IN] the address and filename of the bootimage */
   )
{ /* Body */
   TFTP_DATA_STRUCT_PTR    tftp;
   sockaddr_in             local_addr;
   char_ptr                str_ptr;
   uchar_ptr               packet;
   uint_32                 sock;
   uint_32                 error;
   uint_32                 fn_len, fm_len;
   uint_32                 pkt_len;

   tftp = (TFTP_DATA_STRUCT_PTR) ft_data;

   /* Get a socket */
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      return RTCSERR_TFTP_SOCKET;
   } /* Endif */

   /* Must be initialized in order for TFTP_close to be able to release socket and memory */
   TFTP_config.SOCK = sock;  
   TFTP_config.RRQ_PTR = NULL;  

   /* Bind it */
   local_addr.sin_family      = AF_INET;
   local_addr.sin_port        = 0;
   local_addr.sin_addr.s_addr = INADDR_ANY;
   error = bind(sock, (const sockaddr *)&local_addr, sizeof(local_addr));
   if (error != RTCS_OK) {
      TFTP_close();
      return error;
   } /* Endif */

   if ( (tftp->FILENAME == NULL) || (tftp->FILEMODE == NULL) ){      
      TFTP_close();
      return TFTPERR_FILE_NOT_FOUND;
   } /* Endif */

   /* Prepare a read request (RRQ) */
   str_ptr = tftp->FILENAME;
   while (*str_ptr++) {};
   fn_len = str_ptr - tftp->FILENAME;
   str_ptr = tftp->FILEMODE;
   while (*str_ptr++) {};
   fm_len = str_ptr - tftp->FILEMODE;
   pkt_len = 2 + fn_len + fm_len;
   packet = RTCS_mem_alloc(pkt_len);
   if (packet == NULL) {
      TFTP_close();
      return RTCSERR_TFTP_RRQ_ALLOC;
   } /* Endif */
   _mem_set_type(packet, MEM_TYPE_TFTP_PACKET);
   TFTP_config.RRQ_PTR = packet;  
   htons(packet, TFTPOP_RRQ);
   _mem_copy(tftp->FILENAME, &packet[       2], fn_len);
   _mem_copy(tftp->FILEMODE, &packet[fn_len+2], fm_len);

   /* Send the RRQ */
   TFTP_config.SOCK = sock;
   TFTP_config.SADDR.sin_family      = AF_INET;
   TFTP_config.SADDR.sin_port        = IPPORT_TFTP;
   TFTP_config.SADDR.sin_addr.s_addr = tftp->SERVER;
   TFTP_timeout_init(&TFTP_config.TIMEOUT);
   error = sendto(sock, packet, pkt_len, 0,
                  (sockaddr *)(&TFTP_config.SADDR), sizeof(TFTP_config.SADDR));
   if (error != pkt_len) {
      TFTP_close();
      return RTCSERR_TFTP_RRQ_SEND;
   } /* Endif */

   /* Initialize the TFTP client */
   TFTP_config.RRQ_PTR = packet;
   TFTP_config.RRQ_LEN = pkt_len;
   TFTP_config.LAST    = FALSE;
   htons(TFTP_config.ACK.OP,    TFTPOP_ACK);
   htons(TFTP_config.ACK.BLOCK, 0);
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTP_read
* Returned Value   : pointer to read data, or NULL on error
* Comments  :  Routine to retrieve one file block via TFTP.
*
*END*-----------------------------------------------------------------*/

uchar_ptr TFTP_read
   (
      uint_32_ptr    size
         /* [OUT] number of bytes read, or error code */
   )
{ /* Body */
   uint_32        sock;
   sockaddr_in    remote_addr;
   uint_16        remote_size;
   uint_16        ack_block;
   uint_16        pkt_op, pkt_block;
   uint_32        pkt_size;
   uint_32        time_left;
   boolean        expired;
#if TFTP_TIMEOUT_RETRIES
   uint_32        retries = 0;
#endif

   ack_block = ntohs(TFTP_config.ACK.BLOCK);
   TFTP_timeout_restart(&TFTP_config.TIMEOUT);

   for (;;) {

      /* Check for timeout */
      time_left = TFTP_timeout_left(&TFTP_config.TIMEOUT, &expired);
      if (expired) {
#if TFTP_TIMEOUT_RETRIES
         retries++;
         if (retries > TFTP_TIMEOUT_RETRIES) {
            *size = RTCSERR_TFTP_TIMEOUT;
            TFTP_close();
            return NULL;
         } /* Endif */
#endif
         /* Retransmit the last packet */
         TFTP_RESEND();
      } /* Endif */

      /* Wait for a packet */
      sock = TFTP_WAIT(time_left);

      /* Timeout -- retransmit last packet */
      if (sock != TFTP_config.SOCK) {
         continue;
      } /* Endif */

      remote_size = sizeof(remote_addr);
      pkt_size = TFTP_RECV(TFTP_config.PACKET);

      pkt_op    = ntohs(TFTP_config.PACKET.HEAD.OP);
      pkt_block = ntohs(TFTP_config.PACKET.HEAD.BLOCK);

      /* Check source address of received packet */
      if (remote_addr.sin_addr.s_addr != TFTP_config.SADDR.sin_addr.s_addr) {
         continue;
      } /* Endif */

      /* Validate source port */
      if (ack_block && remote_addr.sin_port != TFTP_config.SADDR.sin_port) {
         TFTP_SEND(TFTP_config.SOCK, _tftp_error_tid, remote_addr);
         continue;
      } /* Endif */

      /* Check size of received packet */
      if (pkt_size < sizeof(TFTP_HEADER)) {
         TFTP_SEND(TFTP_config.SOCK, _tftp_error_op, remote_addr);
         *size = RTCSERR_TFTP_ERROR + TFTPERR_ILLEGAL_OP;
         TFTP_close();
         return NULL;
      } /* Endif */

      /* Check for error packet */
      if (pkt_op == TFTPOP_ERROR) {
         *size = RTCSERR_TFTP_ERROR + pkt_block;
         TFTP_close();
         return NULL;
      } /* Endif */

      /* Check for data packet */
      if ((pkt_op != TFTPOP_DATA)
       || (pkt_size > sizeof(TFTP_PACKET))) {
         TFTP_SEND(TFTP_config.SOCK, _tftp_error_op, remote_addr);
         *size = RTCSERR_TFTP_ERROR + TFTPERR_ILLEGAL_OP;
         TFTP_close();
         return NULL;
      } /* Endif */

      /* Check for retransmitted packet */
      if (pkt_block == ack_block) {
         TFTP_timeout_restart(&TFTP_config.TIMEOUT);
         TFTP_SEND(TFTP_config.SOCK, TFTP_config.ACK, TFTP_config.SADDR);
         continue;
      } /* Endif */

      /* Aknowledge also packets with lower id, some servers do retransmit them until they get an ack */
      if (pkt_block < ack_block) {
         TFTP_timeout_restart(&TFTP_config.TIMEOUT);
         htons(TFTP_config.ACK.BLOCK, pkt_block);
         TFTP_SEND(TFTP_config.SOCK, TFTP_config.ACK, TFTP_config.SADDR);
         htons(TFTP_config.ACK.BLOCK, ack_block); /* Restore id of last acknowledged packet */
         continue;
      } /* Endif */

      /* Drop unexpected packets */
      if (pkt_block > ack_block+1) {
         /* Some server do send more than one packet at a time, these will eventually retransmitted */
         continue;
      }

      /* We have the next packet */
      break;
   } /* Endfor */

   /* Update the adaptive timeout */
   TFTP_timeout_update(&TFTP_config.TIMEOUT);

   /* Free the original RRQ */
   if (!ack_block) {
      TFTP_config.SADDR.sin_port = remote_addr.sin_port;
      _mem_free(TFTP_config.RRQ_PTR);
      TFTP_config.RRQ_PTR = NULL;
   } /* Endif */

   /* ACK it */
   ack_block++;
   htons(TFTP_config.ACK.BLOCK, ack_block);
   TFTP_SEND(TFTP_config.SOCK, TFTP_config.ACK, TFTP_config.SADDR);
   TFTP_config.LAST = (pkt_size < sizeof(TFTP_PACKET));

   /* Return the data */
   *size = pkt_size - sizeof(TFTP_HEADER);
   return TFTP_config.PACKET.DATA;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTP_eof
* Returned Value   : TRUE or FALSE
* Comments  :  Routine to determine whether the file transfer is complete.
*
*END*-----------------------------------------------------------------*/

boolean TFTP_eof
   (
      void
   )
{ /* Body */

   return TFTP_config.LAST;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTP_close
* Returned Value   : error code
* Comments  :  Routine to close a TFTP session.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_close
   (
      void
   )
{ /* Body */
   uint_32  result = RTCSERR_SOCK_INVALID;

   if (TFTP_config.RRQ_PTR != NULL)  {
      _mem_free(TFTP_config.RRQ_PTR);  
      TFTP_config.RRQ_PTR = NULL; 
   }

   if (TFTP_config.SOCK != RTCS_SOCKET_ERROR)  {
      result = shutdown(TFTP_config.SOCK, 0);
      TFTP_config.SOCK = RTCS_SOCKET_ERROR;
      TFTP_config.LAST = TRUE;
   }
   return result;
   
} /* Endbody */

#endif
/* EOF */
