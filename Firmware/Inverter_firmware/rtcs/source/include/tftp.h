#ifndef __tftp_h__
#define __tftp_h__
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
* $FileName: tftp.h$
* $Version : 3.8.12.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the definitions needed by TFTP.
*
*END************************************************************************/

/* Constants */

#define TFTPOP_RRQ     1
#define TFTPOP_WRQ     2
#define TFTPOP_DATA    3
#define TFTPOP_ACK     4
#define TFTPOP_ERROR   5

#define TFTPERR_UNKNOWN            0
#define TFTPERR_FILE_NOT_FOUND     1
#define TFTPERR_ACCESS_VIOLATION   2
#define TFTPERR_DISK_FULL          3
#define TFTPERR_ILLEGAL_OP         4
#define TFTPERR_ILLEGAL_TID        5
#define TFTPERR_FILE_EXISTS        6
#define TFTPERR_ILLEGAL_USER       7

#define TFTP_DATA_SIZE            512
#define TFTPSRV_MAX_TRANSACTIONS  20

#define TFTP_TIMEOUT_RETRIES    1        /* retransmit forever */
#define TFTP_TIMEOUT_MIN        5000     /* 5 seconds */
#define TFTP_TIMEOUT_MAX        60000    /* 1 minute */
#define TFTP_MAX_MESSAGE_SIZE   576
/*
** TFTP_MAX_MODE_SIZE is set to 9 because "netascii" is eight characters, plus
** one for the NUL, if TFTP ever allows other mode types with more characters,
** this value will have to be increased accordingly.
*/
#define TFTP_MAX_MODE_SIZE      9
#define TFTPSRV_INFINITE_RETRIES -1
/*
** This value can be set to TFTP_INFINITE_RETRIES if the end user wishes to
** never cancel a transaction. The default value when shipped is 20.
*/
#define TFTPSRV_TIMEOUT_RETRIES 20

/* TFTP Structure definitions */
typedef struct tftp_header {
   uchar       OP[2];
   uchar       BLOCK[2];
} TFTP_HEADER, _PTR_ TFTP_HEADER_PTR;

/* TFTP data packet */
typedef struct tftp_packet {
   TFTP_HEADER HEAD;
   uchar       DATA[TFTP_DATA_SIZE];
} TFTP_PACKET, _PTR_ TFTP_PACKET_PTR;

/* The Illegal TID error packet */
typedef struct tftp_error_packet {
   uchar       OP[2];
   uchar       CODE[2];
   uchar       MSG[25];
} TFTP_ERROR_PACKET, _PTR_ TFTP_ERROR_PACKET_PTR;

typedef struct tftp_to_struct {
   uint_32  TS;
   boolean  UPDATE;
   uint_32  TO;
   uint_32  M;
   uint_32  D;
} TFTP_TO_STRUCT, _PTR_ TFTP_TO_STRUCT_PTR;

typedef struct tftp_trans_struct {
   struct tftp_trans_struct _PTR_       NEXT;
   struct tftp_trans_struct _PTR_ _PTR_ PREV;
   uint_32                       TIME;
   uint_32                       SOCK;
   MQX_FILE_PTR                  TRANS_FILE_PTR;
   sockaddr_in                   ADDR;
   uint_32                       RECV_OP;
   uint_32                       SEND_OP;
   uint_32                       BLOCK;
   uint_32                       SEND_SIZE;
   TFTP_TO_STRUCT                XMIT_TIMER;
   uint_32                       NUM_RETRIES;
   boolean                       EXIT;
   TFTP_PACKET                   SND;
} TFTP_TRANS_STRUCT, _PTR_ TFTP_TRANS_STRUCT_PTR;

/* The state structure for the TFTP Server */
typedef struct tftpsrv_state_struct {
   /* DO NOT INSERT ANYTHING BETWEEN SRV_SOCK and SOCKETS! */
   uint_32                 SRV_SOCK;
   uint_32                 SOCKETS[TFTPSRV_MAX_TRANSACTIONS];
   TFTP_TRANS_STRUCT _PTR_ TRANS_PTRS[TFTPSRV_MAX_TRANSACTIONS];
   TFTP_TRANS_STRUCT _PTR_ EVENT_HEAD;
   uint_32                 NUM_TRANSACTIONS;
   uchar                   BUFFER[TFTP_MAX_MESSAGE_SIZE];
} TFTPSRV_STATE_STRUCT, _PTR_ TFTPSRV_STATE_STRUCT_PTR;

/* TFTP Macros */
#define TFTP_WAIT(t) RTCS_selectset(&TFTP_config.SOCK, 1, t)

#define TFTP_RECV(p) recvfrom(TFTP_config.SOCK, &(p), sizeof(p), 0, \
                              (sockaddr *)&remote_addr, &remote_size)

#define TFTP_SEND(s,p,a) sendto(s, &(p), sizeof(p), 0, (sockaddr *)&(a), sizeof(a))

#define TFTP_SENDRRQ() sendto(TFTP_config.SOCK, TFTP_config.RRQ_PTR, TFTP_config.RRQ_LEN, 0, \
                              (sockaddr *)(&TFTP_config.SADDR), sizeof(TFTP_config.SADDR))

#define TFTP_RESEND() TFTP_config.RRQ_PTR ? TFTP_SENDRRQ() : TFTP_SEND(TFTP_config.SOCK, TFTP_config.ACK, TFTP_config.SADDR)

extern TFTP_ERROR_PACKET _tftp_error_tid;
extern TFTP_ERROR_PACKET _tftp_error_op;
extern TFTP_ERROR_PACKET _tftp_error_accvio;
extern TFTP_ERROR_PACKET _tftp_error_srv;
extern TFTP_ERROR_PACKET _tftp_error_busy;
extern TFTP_ERROR_PACKET _tftp_error_to;
extern TFTP_ERROR_PACKET _tftp_error_nofile;
extern TFTP_ERROR_PACKET _tftp_error_exists;

extern void    TFTPSRV_service_request     (TFTPSRV_STATE_STRUCT_PTR);
extern void    TFTPSRV_service_transaction (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR);
extern void    TFTPSRV_close_transaction   (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR);
extern uint_32 TFTPSRV_service_timer       (TFTPSRV_STATE_STRUCT_PTR);

extern void TFTPSRV_build_ACK  (TFTP_TRANS_STRUCT_PTR);
extern void TFTPSRV_build_DATA (TFTP_TRANS_STRUCT_PTR);
extern void TFTPSRV_send       (TFTP_TRANS_STRUCT_PTR);

extern void TFTPSRV_timer_start  (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR, uint_32);
extern void TFTPSRV_timer_cancel (TFTPSRV_STATE_STRUCT_PTR, TFTP_TRANS_STRUCT_PTR);

extern uint_32 TFTPSRV_open_device (uint_32, char_ptr, char_ptr, MQX_FILE_PTR _PTR_);

extern uint_32 TFTP_timeout_init    (TFTP_TO_STRUCT_PTR);
extern uint_32 TFTP_timeout_restart (TFTP_TO_STRUCT_PTR);
extern uint_32 TFTP_timeout_update  (TFTP_TO_STRUCT_PTR);
extern uint_32 TFTP_timeout_left    (TFTP_TO_STRUCT_PTR, boolean _PTR_);

#endif
/* EOF */
