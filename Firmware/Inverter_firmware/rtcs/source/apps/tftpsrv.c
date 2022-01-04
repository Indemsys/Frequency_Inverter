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
* $FileName: tftpsrv.c$
* $Version : 3.8.12.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the TFTP server implementation.
*
*END************************************************************************/


#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_UDP

#include <string.h>
#include <fio.h>
#include "tftp.h"

_rtcs_taskid TFTPSRV_task_id = 0;

#ifdef __MQX__ 
void TFTPSRV_Exit_handler(void);
#endif

void TFTPSRV_task(pointer, pointer);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTPSRV_init
* Returned Value   : error code
* Comments  :  Start the TFTP server
*
*END*-----------------------------------------------------------------*/

uint_32 TFTPSRV_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   if (TFTPSRV_task_id == 0)  {
      return RTCS_task_create(name, priority, stacksize, TFTPSRV_task, NULL);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name   : TFTPSRV_task
* Returned Value  : void
* Comments        : This task waits on port 69 for read and write
*                   requests, as well as servicing TFTP transactions
*                   already in progress.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_task
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr;
   sockaddr_in                laddr;
   uint_32                    i, numtrans, timeout;
   uint_32                    error;
   
   tftpsrv_ptr = RTCS_mem_alloc_zero(sizeof(TFTPSRV_STATE_STRUCT));
  
   if (tftpsrv_ptr == NULL) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_MEMORY);
   } /* Endif */

    TFTPSRV_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if the TFTP Server is terminated */
   _task_set_environment( _task_get_id(), (pointer) tftpsrv_ptr );
   _task_set_exit_handler( _task_get_id(), TFTPSRV_Exit_handler );
#endif 

   tftpsrv_ptr->SRV_SOCK = socket(PF_INET, SOCK_DGRAM, 0);
   if (tftpsrv_ptr->SRV_SOCK == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */

   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_TFTP;
   laddr.sin_addr.s_addr = INADDR_ANY;

   error = bind(tftpsrv_ptr->SRV_SOCK, (const sockaddr *)&laddr, sizeof(laddr));
   if (error != RTCS_OK) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {
      timeout = TFTPSRV_service_timer(tftpsrv_ptr);
      numtrans = tftpsrv_ptr->NUM_TRANSACTIONS;
      error = RTCS_selectset(&tftpsrv_ptr->SRV_SOCK, numtrans+1, timeout);
      if ((error == RTCS_OK) || (error == RTCS_SOCKET_ERROR)) {
         continue;
      } /* Endif */
      if (error == tftpsrv_ptr->SRV_SOCK) {
         /* New request, service it */
         TFTPSRV_service_request(tftpsrv_ptr);
      } else {
         for (i = 0; i < numtrans; i++) {
            if (error == tftpsrv_ptr->SOCKETS[i]) {
               TFTPSRV_service_transaction(tftpsrv_ptr, tftpsrv_ptr->TRANS_PTRS[i]);
               break;
            } /* Endif */
         } /* Endfor */
      } /* Endif */
   } /* Endfor */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_service_request
* Returned Value  : void
* Comments        : This function takes an initial TFTP request and
*                   starts the transaction process.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_service_request
   (
      TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr
         /* [IN/OUT] The TFTP Server state */
   )
{ /* Body */
   TFTP_TRANS_STRUCT_PTR   trans_ptr;
   sockaddr_in             sockaddr_t;
   int_32                  pkt_len, i;
   uint_32                 error;
   char_ptr                filename, filemode;
   uint_16                 sockaddrlen, pkt_op;

   /* receive the datagram */
   sockaddrlen = sizeof(sockaddr_t);
   pkt_len = recvfrom(tftpsrv_ptr->SRV_SOCK, tftpsrv_ptr->BUFFER,
                      TFTP_MAX_MESSAGE_SIZE, 0, (sockaddr *)&sockaddr_t, &sockaddrlen);
   if (pkt_len == RTCS_ERROR) {
      return;
   } /* Endif */

   /* limit the number of concurrent transactions */
   if (tftpsrv_ptr->NUM_TRANSACTIONS >= TFTPSRV_MAX_TRANSACTIONS) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_busy, sockaddr_t);
      return;
   } /* Endif */

   /* parse the request; extract op, filename and filemode */
   i = 2;
   filename = (char_ptr)tftpsrv_ptr->BUFFER + i;
   for (; i<pkt_len; i++) {
      if (tftpsrv_ptr->BUFFER[i] == '\0') break;
   } /* Endfor */
   i++;
   filemode = (char_ptr)tftpsrv_ptr->BUFFER + i;
   for (; i<pkt_len; i++) {
      if (tftpsrv_ptr->BUFFER[i] == '\0') break;
   } /* Endfor */
   if (i >= pkt_len) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_op, sockaddr_t);
      return;
   } /* Endif */
   pkt_op = ntohs(tftpsrv_ptr->BUFFER);

   /* allocate state for the new transaction */
   trans_ptr = RTCS_mem_alloc_zero(sizeof(TFTP_TRANS_STRUCT));
   if (trans_ptr == NULL) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_srv, sockaddr_t);
      return;
   } /* Endif */
   _mem_set_type(trans_ptr, MEM_TYPE_TFTP_TRANS_STRUCT);
   
   /* validate the requested operation */
   switch (pkt_op) {
   case TFTPOP_RRQ:
      trans_ptr->RECV_OP = TFTPOP_ACK;
      trans_ptr->SEND_OP = TFTPOP_DATA;
      break;
   case TFTPOP_WRQ:
      trans_ptr->RECV_OP = TFTPOP_DATA;
      trans_ptr->SEND_OP = TFTPOP_ACK;
      break;
   default:
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_op, sockaddr_t);
      _mem_free(trans_ptr);
      return;
   } /* Endswitch */

   /* open the requested file */
   error = TFTPSRV_open_device(pkt_op, filename, filemode, &trans_ptr->TRANS_FILE_PTR);
   if (error) {
      switch (error) {
      case RTCS_EACCES:
         TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_accvio, sockaddr_t);
         break;
      case RTCS_ENOENT:
         TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_nofile, sockaddr_t);
         break;
      case RTCS_EEXIST:
         TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_exists, sockaddr_t);
         break;
      default:
         TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_srv, sockaddr_t);
         break;
      } /* Endswitch */
      _mem_free(trans_ptr);
      return;
   } /* Endif */

   /* create a socket for the new transaction */
   trans_ptr->SOCK = socket(PF_INET, SOCK_DGRAM, 0);
   if (trans_ptr->SOCK == RTCS_SOCKET_ERROR) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_srv, sockaddr_t);
      RTCS_io_close(trans_ptr->TRANS_FILE_PTR);
      _mem_free(trans_ptr);
      return;
   } /* Endif */

   trans_ptr->ADDR.sin_family      = sockaddr_t.sin_family;
   trans_ptr->ADDR.sin_port        = sockaddr_t.sin_port;
   trans_ptr->ADDR.sin_addr.s_addr = sockaddr_t.sin_addr.s_addr;

   sockaddr_t.sin_family      = AF_INET;
   sockaddr_t.sin_port        = 0;
   sockaddr_t.sin_addr.s_addr = INADDR_ANY;

   error = bind(trans_ptr->SOCK, (const sockaddr *)&sockaddr_t, sizeof(sockaddr_t));
   if (error != RTCS_OK) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_srv, sockaddr_t);
      shutdown(trans_ptr->SOCK, 0);
      RTCS_io_close(trans_ptr->TRANS_FILE_PTR);
      _mem_free(trans_ptr);
      return;
   } /* Endif */

   /* build the first packet */
   trans_ptr->BLOCK = 0;
   switch (trans_ptr->SEND_OP) {
   case TFTPOP_DATA:
      TFTPSRV_build_DATA(trans_ptr);
      break;
   case TFTPOP_ACK:
      TFTPSRV_build_ACK(trans_ptr);
      trans_ptr->EXIT = FALSE;
      break;
   } /* Endswitch */
   if (trans_ptr->SEND_SIZE < sizeof(TFTP_HEADER)) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_srv, sockaddr_t);
      shutdown(trans_ptr->SOCK, 0);
      RTCS_io_close(trans_ptr->TRANS_FILE_PTR);
      _mem_free(trans_ptr);
      return;
   } /* Endif */

   /* send the first packet */
   TFTPSRV_timer_start(tftpsrv_ptr, trans_ptr, TFTP_timeout_init(&trans_ptr->XMIT_TIMER));
   TFTPSRV_send(trans_ptr);

   tftpsrv_ptr->SOCKETS[tftpsrv_ptr->NUM_TRANSACTIONS] = trans_ptr->SOCK;
   tftpsrv_ptr->TRANS_PTRS[tftpsrv_ptr->NUM_TRANSACTIONS] = trans_ptr;
   tftpsrv_ptr->NUM_TRANSACTIONS++;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_open_device
* Returned Value  : error code
* Comments        : This function opens the requested device, if possible.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTPSRV_open_device
   (
      uint_32        pkt_op,
         /* [IN] the transaction type */
      char_ptr       filename,
         /* [IN] the requested file */
      char_ptr       filemode,
         /* [IN] the transfer mode */
      MQX_FILE_PTR _PTR_ fd_ptr
         /* [OUT] the open file descriptor */
   )
{ /* Body */
   char_ptr open_mode;
   uint_32  error;

   if (strcmp(filemode, "netascii") == 0) {
      if (pkt_op == TFTPOP_RRQ) {
         open_mode = "r";
      } else {
         open_mode = "w";
      } /* Endif */
   } else if (strcmp(filemode, "octet") == 0) {
      if (pkt_op == TFTPOP_RRQ) {
         open_mode = "rb";
      } else {
         open_mode = "wb";
      } /* Endif */
   } else {
      return RTCS_EACCES;
   } /* Endif */

   if (!TFTPSRV_access(filename, pkt_op)) {
      return RTCS_EACCES;
   } /* Endif */

   *fd_ptr = RTCS_io_open(filename, open_mode, &error);
   return error;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_service_transaction
* Returned Value  : void
* Comments        : This function services a transaction in progress
*                   which has just received a packet.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_service_transaction
   (
      TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr,
         /* [IN/OUT] The TFTP Server state */
      TFTP_TRANS_STRUCT_PTR      trans_ptr
         /* [IN/OUT] The transaction state */
   )

{ /* Body */
   sockaddr_in sockaddr_tftp;
   uint_32     block_num;
   int_32      pkt_len, write_len;
   uint_16     sockaddrlen, pkt_op;

   /* receive the datagram */
   sockaddrlen = sizeof(sockaddr_tftp);
   pkt_len = recvfrom(trans_ptr->SOCK, tftpsrv_ptr->BUFFER,
                      TFTP_MAX_MESSAGE_SIZE, 0,(sockaddr *)&sockaddr_tftp, &sockaddrlen);
   if (pkt_len == RTCS_ERROR) {
      return;
   } /* Endif */

   /* verify the sender's address and port */
   if ((trans_ptr->ADDR.sin_port        != sockaddr_tftp.sin_port) ||
       (trans_ptr->ADDR.sin_addr.s_addr != sockaddr_tftp.sin_addr.s_addr)) {
      TFTP_SEND(tftpsrv_ptr->SRV_SOCK, _tftp_error_tid, sockaddr_tftp);
      return;
   } /* Endif */

   /* get op and block number */
   if (pkt_len < sizeof(TFTP_HEADER)) {
      TFTP_SEND(trans_ptr->SOCK, _tftp_error_tid, sockaddr_tftp);
      return;
   } /* Endif */
   pkt_op    = ntohs(tftpsrv_ptr->BUFFER);
   block_num = ntohs(tftpsrv_ptr->BUFFER + 2);

   /* verify the requested operation */
   if (pkt_op != trans_ptr->RECV_OP) {
      if (pkt_op != TFTPOP_ERROR) {
         TFTP_SEND(trans_ptr->SOCK, _tftp_error_op, sockaddr_tftp);
      } /* Endif */
      TFTPSRV_close_transaction(tftpsrv_ptr, trans_ptr);
      return;
   } /* Endif */

   switch (pkt_op) {
   case TFTPOP_DATA:
      /* We are servicing a write request from a TFTP Client here */
      if (block_num == trans_ptr->BLOCK) {

         pkt_len -= sizeof(TFTP_HEADER);
         write_len = RTCS_io_write(trans_ptr->TRANS_FILE_PTR,
                                   tftpsrv_ptr->BUFFER + sizeof(TFTP_HEADER),
                                   pkt_len);
         if (write_len != pkt_len) {
            TFTP_SEND(trans_ptr->SOCK, _tftp_error_srv, trans_ptr->ADDR);
            TFTPSRV_close_transaction(tftpsrv_ptr, trans_ptr);
            return;
         } /* Endif */

         TFTPSRV_build_ACK(trans_ptr);
         trans_ptr->EXIT = (write_len < sizeof(TFTP_PACKET) - sizeof(TFTP_HEADER));
         TFTPSRV_timer_cancel(tftpsrv_ptr, trans_ptr);
         TFTPSRV_timer_start(tftpsrv_ptr, trans_ptr, TFTP_timeout_update(&trans_ptr->XMIT_TIMER));
         TFTPSRV_send(trans_ptr);

      } else if (block_num == trans_ptr->BLOCK - 1) {
         TFTPSRV_timer_cancel(tftpsrv_ptr, trans_ptr);
         TFTPSRV_timer_start(tftpsrv_ptr, trans_ptr, TFTP_timeout_restart(&trans_ptr->XMIT_TIMER));
         TFTPSRV_send(trans_ptr);
      } /* Endif */
      break;

   case TFTPOP_ACK:
      /* We are servicing a read request */
      if (block_num == trans_ptr->BLOCK) {
         if (trans_ptr->EXIT) {
            /* We've received the last ACK, exit */
            TFTPSRV_close_transaction(tftpsrv_ptr, trans_ptr);
            return;
         } /* Endif */
         TFTPSRV_build_DATA(trans_ptr);
         if (trans_ptr->SEND_SIZE < sizeof(TFTP_HEADER)) {
            TFTP_SEND(trans_ptr->SOCK, _tftp_error_srv, sockaddr_tftp);
            TFTPSRV_close_transaction(tftpsrv_ptr, trans_ptr);
            return;
         } /* Endif */
         TFTPSRV_timer_cancel(tftpsrv_ptr, trans_ptr);
         TFTPSRV_timer_start(tftpsrv_ptr, trans_ptr, TFTP_timeout_update(&trans_ptr->XMIT_TIMER));
         TFTPSRV_send(trans_ptr);
      } /* Endif */
      break;

   } /* Endswitch */
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_build_ACK
* Returned Value  : void
* Comments        : This function builds an ACK packet for client
*                   write requests.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_build_ACK
   (
      TFTP_TRANS_STRUCT_PTR   trans_ptr
         /* [IN/OUT] The transaction state */
   )
{ /* Body */
   htons(trans_ptr->SND.HEAD.OP,    trans_ptr->SEND_OP);
   htons(trans_ptr->SND.HEAD.BLOCK, trans_ptr->BLOCK);
   trans_ptr->BLOCK++;
   trans_ptr->SEND_SIZE = sizeof(TFTP_HEADER);
   trans_ptr->NUM_RETRIES = 0;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_build_DATA
* Returned Value  : void
* Comments        : This function builds a DATA packet for client
*                   read requests.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_build_DATA
   (
      TFTP_TRANS_STRUCT_PTR   trans_ptr
         /* [IN/OUT] The transaction state */
   )
{ /* Body */
   int_32 datalen;

   datalen = RTCS_io_read(trans_ptr->TRANS_FILE_PTR, (char_ptr)trans_ptr->SND.DATA, TFTP_DATA_SIZE);
   if (datalen < 0) {
      datalen = 0;
   } /* Endif */

   trans_ptr->BLOCK++;
   htons(trans_ptr->SND.HEAD.OP,    trans_ptr->SEND_OP);
   htons(trans_ptr->SND.HEAD.BLOCK, trans_ptr->BLOCK);
   trans_ptr->SEND_SIZE = sizeof(TFTP_HEADER) + datalen;
   trans_ptr->NUM_RETRIES = 0;
   trans_ptr->EXIT = (trans_ptr->SEND_SIZE < sizeof(TFTP_PACKET));
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_send
* Returned Value  : void
* Comments        : This function (re)sends a packet.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_send
   (
      TFTP_TRANS_STRUCT_PTR   trans_ptr
         /* [IN/OUT] The transaction state */
   )
{ /* Body */
   sendto(trans_ptr->SOCK, &trans_ptr->SND, trans_ptr->SEND_SIZE, 0,
          (sockaddr *)(&trans_ptr->ADDR), sizeof(trans_ptr->ADDR));
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_timer_start
* Returned Value  : void
* Comments        : This function starts the retransmission timer.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_timer_start
   (
      TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr,
         /* [IN/OUT] The TFTP Server state */
      TFTP_TRANS_STRUCT_PTR      trans_ptr,
         /* [IN/OUT] The transaction state */
      uint_32                    timeout
         /* [IN] The retransmission timeout */
   )
{ /* Body */
   TFTP_TRANS_STRUCT_PTR _PTR_ search_ptr;

   trans_ptr->TIME = timeout;
   for (search_ptr = &tftpsrv_ptr->EVENT_HEAD;;
        search_ptr = &(*search_ptr)->NEXT) {
      if (*search_ptr == NULL) {
         /* Append to tail of list */
         RTCS_DLIST_INS(*search_ptr, trans_ptr);
         break;
      } /* Endif */
      if ((*search_ptr)->TIME > trans_ptr->TIME) {
         /* Insert in middle (or head) of list */
         (*search_ptr)->TIME -= trans_ptr->TIME;
         RTCS_DLIST_INS(*search_ptr, trans_ptr);
         break;
      } /* Endif */
      trans_ptr->TIME -= (*search_ptr)->TIME;
   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_timer_cancel
* Returned Value  : void
* Comments        : This function stops the retransmission timer.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_timer_cancel
   (
      TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr,
         /* [IN/OUT] The TFTP Server state */
      TFTP_TRANS_STRUCT_PTR      trans_ptr
         /* [IN/OUT] The transaction state */
   )
{ /* Body */

   RTCS_DLIST_DEL(&tftpsrv_ptr->EVENT_HEAD, trans_ptr);
   if (trans_ptr->NEXT) {
      trans_ptr->NEXT->TIME += trans_ptr->TIME;
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_service_timer
* Returned Value  : time until the first transaction expiry in
*                   milliseconds, 0 if no pending transactions.
* Comments        : This retransmits the last packet for all expired
*                   transactions.  The transaction list is sorted by
*                   expiry time, so we can stop when the head of the list
*                   is unexpired.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTPSRV_service_timer
   (
      TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr
         /* [IN/OUT] The TFTP Server state */
   )
{ /* Body */
   TFTP_TRANS_STRUCT_PTR   trans_ptr;
   uint_32                 time;
   boolean                 expired;

   while (tftpsrv_ptr->EVENT_HEAD) {

      trans_ptr = tftpsrv_ptr->EVENT_HEAD;
      time = TFTP_timeout_left(&trans_ptr->XMIT_TIMER, &expired);
      if (!expired) {
         trans_ptr->TIME = time;
         return time;
      } /* Endif */

      RTCS_DLIST_DEL(tftpsrv_ptr->EVENT_HEAD, trans_ptr);

      if (trans_ptr->EXIT) {
         TFTPSRV_close_transaction(tftpsrv_ptr, trans_ptr);
         continue;
      } /* Endif */

#if TFTPSRV_TIMEOUT_RETRIES != TFTPSRV_INFINITE_RETRIES
      if (++trans_ptr->NUM_RETRIES > TFTPSRV_TIMEOUT_RETRIES) {
         TFTP_SEND(trans_ptr->SOCK, _tftp_error_to, trans_ptr->ADDR);
         TFTPSRV_close_transaction(tftpsrv_ptr, trans_ptr);
         continue;
      } /* Endif */
#endif

      TFTPSRV_send(trans_ptr);
      TFTPSRV_timer_start(tftpsrv_ptr, trans_ptr, time);

   } /* Endwhile */

   /* Nothing in the event queue */
   return 0;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_close_transaction
* Returned Value  : void
* Comments        : This function closes a transaction.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_close_transaction
   (
      TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr,
         /* [IN/OUT] The TFTP Server state */
      TFTP_TRANS_STRUCT_PTR      trans_ptr
         /* [IN/OUT] The transaction state */
   )
{ /* Body */
   uint_32 i;

   TFTPSRV_timer_cancel(tftpsrv_ptr, trans_ptr);

   for (i = 0; i < tftpsrv_ptr->NUM_TRANSACTIONS; i++) {
      if (tftpsrv_ptr->TRANS_PTRS[i] == trans_ptr) {
         break;
      } /* Endif */
   } /* Endfor */

   RTCS_io_close(trans_ptr->TRANS_FILE_PTR);
   shutdown(trans_ptr->SOCK, 0);
   if (--tftpsrv_ptr->NUM_TRANSACTIONS) {
      tftpsrv_ptr->TRANS_PTRS[i] = tftpsrv_ptr->TRANS_PTRS[tftpsrv_ptr->NUM_TRANSACTIONS];
      tftpsrv_ptr->SOCKETS[i]    = tftpsrv_ptr->SOCKETS[tftpsrv_ptr->NUM_TRANSACTIONS];
   } /* Endif */
   _mem_free(trans_ptr);

} /* Endbody */

#ifdef __MQX__ 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTPSRV_stop
* Returned Value   : error code
* Comments  :  Start the TFTPSRV task.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTPSRV_stop( void )
{ /* Body */
   if (TFTPSRV_task_id == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(TFTPSRV_task_id);
   TFTPSRV_task_id = 0;
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TFTPSRV_Exit_handler
* Returned Value   : error code
* Comments  :  Start the TFTPSRV task.
*
*END*-----------------------------------------------------------------*/

void TFTPSRV_Exit_handler( void )
{ /* Body */
   TFTPSRV_STATE_STRUCT_PTR   tftpsrv_ptr;
   
   tftpsrv_ptr = (TFTPSRV_STATE_STRUCT_PTR) _task_get_environment( _task_get_id() );
   if (tftpsrv_ptr != NULL)  {
      while (tftpsrv_ptr->NUM_TRANSACTIONS)  {
         TFTPSRV_close_transaction(tftpsrv_ptr,tftpsrv_ptr->EVENT_HEAD );    
      }
      if (tftpsrv_ptr->SRV_SOCK)  {
         shutdown(tftpsrv_ptr->SRV_SOCK, 0);
         tftpsrv_ptr->SRV_SOCK = 0;   
      }   
   }
   TFTPSRV_task_id = 0;
} /* Endbody */
#endif

#endif
/* EOF */
