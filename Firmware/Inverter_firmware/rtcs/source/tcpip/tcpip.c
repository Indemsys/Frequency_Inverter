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
* $FileName: tcpip.c$
* $Version : 3.8.10.0$
* $Date    : Oct-9-2012$
*
* Comments:
*
*   This is the main task for the TCP/IP protocol suite.
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "ticker.h"
#include "tcpip.h"
#include "arp.h"


uint_32 (_CODE_PTR_ TCP_tick)(void);

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TCPIP_fake_tick
* Returned Value  : 0
* Comments        :
*
*  The fake TCP_Tick_server(), used when TCP is not installed.
*
*END*-----------------------------------------------------------------*/

static uint_32 TCPIP_fake_tick
   (
      void
   )
{ /* Body */
   return 0;
} /* Endbody */

/*TASK*------------------------------------------------------------------
*
* Task Name      :   TCPIP_task
* Comments       :
*     This task services requests from the socket layer and the
*     application, and services incoming packets from the link layer(s).
*
*END*------------------------------------------------------------------*/

void TCPIP_task
   (
      pointer  dummy,
      pointer  creator
   )
{ /* Body */
   TCPIP_CFG_STRUCT           TCPIP_cfg;
   RTCS_DATA_PTR              RTCS_data_ptr;
   uint_32                    i;
   TCPIP_MESSAGE_PTR          tcpip_msg;
   uint_32                    timeout = 1, timebefore, timeafter, timedelta;
   uint_32                    status;           /* Return status */
   _queue_id                  tcpip_qid;
   
    RTCSLOG_FNE2(RTCSLOG_FN_TCPIP_task, creator);

   RTCS_data_ptr = RTCS_get_data();
   RTCS_setcfg(TCPIP, &TCPIP_cfg);

   TCPIP_cfg.status = RTCS_OK;

   tcpip_qid = RTCS_msgq_open(TCPIP_QUEUE, 0);
   if (tcpip_qid == 0) {
      RTCS_task_exit(creator, RTCSERR_OPEN_QUEUE_FAILED);
   } /* Endif */

   RTCS_data_ptr->TCPIP_TASKID = RTCS_task_getid();

   /*
   ** Initialize the Time Service
   */
   TCP_tick = TCPIP_fake_tick;
   TCPIP_Event_init();
   timebefore = RTCS_time_get();

   /*
   ** Allocate a block of PCBs
   */
   status = RTCSPCB_init();
   if (status != RTCS_OK) {
      RTCS_task_exit(creator, status);
   } /* Endif */
    
    IF_FREE       = NULL;
    
   /*
   ** Initialize the protocols
   */
   
#if RTCSCFG_ENABLE_IP4
    /*********************************************
    *   Initialize IPv4
    **********************************************/
    status = IP_init();
    if (status)
    {
        RTCS_task_exit(creator, status);
    } 

#if RTCSCFG_ENABLE_ICMP

    status = ICMP_init();
    if (status)
    {
        RTCS_task_exit(creator, status);
    }
   
#endif /* RTCSCFG_ENABLE_ICMP */

    ARP_init();
    
    BOOT_init();

#endif /* RTCSCFG_ENABLE_IP4 */

#if RTCSCFG_ENABLE_IP6

    /*********************************************
    *   Initialize IPv6
    **********************************************/
    status = IP6_init();
    if (status)
    {
      RTCS_task_exit(creator, status);
    } 

    /* Init ICMP6. */ 
    status = ICMP6_init(); //TBD Add it to RTCS6_protocol_table
    if (status)
    {
        RTCS_task_exit(creator, status);
    } 
    
#endif /* RTCSCFG_ENABLE_IP6*/

#if (RTCSCFG_ENABLE_IP_REASSEMBLY && RTCSCFG_ENABLE_IP4) || (RTCSCFG_ENABLE_IP6_REASSEMBLY && RTCSCFG_ENABLE_IP6)
    /* Initialize the reassembler */
    status = IP_reasm_init();
    if (status)
    {
        RTCS_task_exit(creator, status);
    } 
#endif

    /* Add loopback interface.*/
    status = IPLOCAL_init ();    
    if (status)
    {
        RTCS_task_exit(creator, status);
    };

   for (i = 0; RTCS_protocol_table[i]; i++) {
      status = (*RTCS_protocol_table[i])();
      if (status) {
         RTCS_task_exit(creator, status);
      } /* Endif */
   } /* Endfor */

   _RTCS_initialized= TRUE;
   RTCS_task_resume_creator(creator, RTCS_OK);

   while (1) {
      /*
      ** Update any option that can be changed
      */
#if 0
      RTCS_data_ptr->IP_FORWARD_COPY =    _IP_forward;
      RTCS_data_ptr->TCP_BYPASS_RX_COPY = _TCP_bypass_rx;
      RTCS_data_ptr->TCP_BYPASS_TX_COPY = _TCP_bypass_tx;
      RTCS_data_ptr->TCP_RTO_MIN_COPY =   _TCP_rto_min;
#endif

#if 0
      /*
      ** Process TCP events
      */
      timeout = TCP_tick();

      /*
      ** Set up the timeout to hit at exactly the time of the next timeout.
      */
      timeafter = RTCS_time_get();
      timedelta = timeafter - timebefore;
      timebefore = timeafter;
      timedelta = TCPIP_Event_time(timedelta);

      if (timedelta != 0) {
         if ((timedelta < timeout) || (timeout == 0)) {
            timeout = timedelta;
         } /* Endif */
      } /* Endif */
#endif

      tcpip_msg = (TCPIP_MESSAGE_PTR)RTCS_msgq_receive(tcpip_qid, timeout, RTCS_data_ptr->TCPIP_msg_pool);

      if (tcpip_msg) {
         if (NULL != tcpip_msg->COMMAND)
            tcpip_msg->COMMAND(tcpip_msg->DATA);
         
         RTCS_msg_free(tcpip_msg);
      }
      
      timeout = TCP_tick();

      timeafter = RTCS_time_get();
      timedelta = RTCS_timer_get_interval(timebefore, timeafter); 
      timebefore = timeafter;
      timedelta = TCPIP_Event_time(timedelta);
      
      if (timedelta != 0) {
        if ((timedelta < timeout) || (timeout == 0)) {
            timeout = timedelta;
         }
      }

   } /* Endwhile */
} /* Endbody */

#if 0
void TCP_Transmit_msg(pointer data) {
    TCP_CFG_STRUCT_PTR   tcp_cfg;
    TCB_STRUCT_PTR       tcb = data;
    
    tcp_cfg = RTCS_getcfg(TCP);
    TCP_Transmit(data, tcp_cfg, FALSE, TRUE);
    
         /*
         ** If sender idle, restart keep-alive timer
         */
         if ( tcb->sndlen == 0 && tcb->keepaliveto != 0 ) {
            TCP_Send_keepalive(tcb,tcp_cfg);
         } /* Endif */
}
#endif

/* EOF */
