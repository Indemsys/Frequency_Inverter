#ifndef __rtcshdlc_h__
#define __rtcshdlc_h__
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
* $FileName: rtcshdlc.h$
* $Version : 3.6.10.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by application
*   programs in order to use the HDLC device.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>

#include <fio.h>


/***************************************
**
** MQX compatibility
*/
#define _RTCS_poolid            _pool_id
#define _RTCS_queueid           _queue_id
#define _RTCS_queue_number      _queue_number
#define RTCS_memcopy            _mem_copy
#define RTCS_memalloc           _mem_alloc_zero
#define RTCS_memfree            _mem_free

/***************************************
**
** Constants
*/
#define HDLCIO_OK              0
#define RTCS_HDLC_MESSAGE_SIZE 1502

/***************************************
**
** Structures
*/

typedef struct rtcs_hdlc_header_struct
{
   MESSAGE_HEADER_STRUCT         HEADER;
   uint_32                       PACKET_SIZE;
} RTCS_HDLC_HEADER_STRUCT, _PTR_ RTCS_HDLC_HEADER_PTR;

typedef struct rtcs_hdlc_message_struct
{
   RTCS_HDLC_HEADER_STRUCT HDLC_HEADER;
   uchar       DATA[RTCS_HDLC_MESSAGE_SIZE];
} RTCS_HDLC_MESSAGE_STRUCT, _PTR_ RTCS_HDLC_MESSAGE_STRUCT_PTR;



typedef struct hdlcio_stats {
   RTCS_STATS_STRUCT  COMMON;

   uint_32  ST_RX_RUNT;
   uint_32  ST_RX_GIANT;

} HDLCIO_STATS, _PTR_ HDLCIO_STATS_PTR;


typedef struct hdlcio_struct {
   struct _iopcb_table PCB_TABLE;
   HDLCIO_STATS        STATS;
   _RTCS_poolid        POOL_ID;
   _RTCS_queueid       QID;
   uint_16             CHANNEL_Q;
   _ppp_mutex _PTR_    MUTEX_PTR;
   _rtcs_part          PART_ID;
   void (_CODE_PTR_    UP)(pointer);
   void (_CODE_PTR_    DOWN)(pointer);
   pointer             PARAM;

} HDLCIO_STRUCT, _PTR_ HDLCIO_STRUCT_PTR;

extern _iopcb_handle _iopcb_hdlc_init(uint_16, _RTCS_poolid, _RTCS_queueid, _ppp_mutex _PTR_);
extern void          _iopcb_hdlc_rcv_unblock(pointer);
extern void          _iopcb_hdlcio_rcv_unblock( pointer );
extern _iopcb_handle _iopcb_hdlcio_init(MQX_FILE_PTR);

#endif
/* EOF */
