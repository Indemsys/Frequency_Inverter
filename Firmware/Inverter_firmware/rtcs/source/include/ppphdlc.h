#ifndef __ppphdlc_h__
#define __ppphdlc_h__
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
* $FileName: ppphdlc.h$
* $Version : 3.8.11.0$
* $Date    : Aug-27-2012$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by application
*   programs in order to use the PPPHDLC device.
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#include <fio.h>


/***************************************
**
** MQX compatibility
*/

#define _ppphdlc_mutex             LWSEM_STRUCT
#define PPPHDLC_mutex_init(p)      _lwsem_create_hidden(p,1)
#define PPPHDLC_mutex_destroy(p)   _lwsem_destroy(p)
#define PPPHDLC_mutex_lock(p)      _lwsem_wait(p)
#define PPPHDLC_mutex_unlock(p)    _lwsem_post(p)

#define PPPHDLC_memalloc      _mem_alloc_zero
#define PPPHDLC_memfree       _mem_free
#define PPPHDLC_memcopy       _mem_copy

#define _ppphdlc_partid       _rtcs_part
#define PPPHDLC_partcreate    RTCS_part_create
#define PPPHDLC_partdestroy   RTCS_part_destroy
#define PPPHDLC_partalloc     RTCS_part_alloc
#define PPPHDLC_partfree      RTCS_part_free

#define PPPHDLC_delay         _time_delay


/***************************************
**
** Constants
*/

#define PPPHDLC_FRAMESIZE_MAXDATA 1502    /* protocol(2) + data(1500) */
#define PPPHDLC_FRAMESIZE_FCS        2

/*
** Special PPPHDLC characters
*/

#define PPPHDLC_ESC     0x7D
#define PPPHDLC_FLAG    0x7E

#define PPPHDLC_ADDR    0xFF
#define PPPHDLC_CTRL    0x03

#define PPPHDLC_OK      0
#define PPPHDLC_END     1
#define PPPHDLC_ABORT   2

/*
** Size of the Async Control Character Map
*/

#define PPPHDLC_BITS_IN_UINT32  32
#define PPPHDLC_BITS_IN_ACCM    256


///* TBC */
//#ifndef RTCS_PRINT_PPP_PACKETS
//        #define RTCS_PRINT_PPP_PACKETS 0
//#endif


typedef struct ppphdlc_opt {
   uint_32           ACCM[PPPHDLC_BITS_IN_ACCM / PPPHDLC_BITS_IN_UINT32];
   boolean           PFC;
   boolean           ACFC;
} PPPHDLC_OPT, _PTR_ PPPHDLC_OPT_PTR;

extern PPPHDLC_OPT PPPHDLC_DEFAULT_OPTIONS;

typedef struct ppphdlc_stats {
   RTCS_STATS_STRUCT COMMON;

   uint_32  ST_RX_ABORTED;
   uint_32  ST_RX_RUNT;
   uint_32  ST_RX_GIANT;
   uint_32  ST_RX_BAD_ADDR;
   uint_32  ST_RX_BAD_CTRL;
   uint_32  ST_RX_BAD_FCS;

} PPPHDLC_STATS, _PTR_ PPPHDLC_STATS_PTR;


typedef struct ppphdlc_struct {
   struct _iopcb_table PCB_TABLE;
   PPPHDLC_STATS       STATS;
   _ppphdlc_partid     PART_ID;
   _ppphdlc_mutex      MUTEX;
   MQX_FILE_PTR        DEVICE;
   PPPHDLC_OPT         SEND_OPT;
   PPPHDLC_OPT         RECV_OPT;
   uint_16             FCS_SEND;
   uint_16             FCS_RECV;
   void (_CODE_PTR_    UP)(pointer);
   void (_CODE_PTR_    DOWN)(pointer);
   pointer             PARAM;

} PPPHDLC_STRUCT, _PTR_ PPPHDLC_STRUCT_PTR;


extern _iopcb_handle _iopcb_ppphdlc_init(MQX_FILE_PTR);


#endif
/* EOF */
