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
* $FileName: ppphdlc.c$
* $Version : 3.8.21.0$
* $Date    : Sep-27-2012$
*
* Comments:
*
*   This file contains the PPPHDLC device functions needed
*   for communication.
*
*END************************************************************************/

#include <ppphdlc.h>
#include <fio.h>
#include <rtcs_in.h>
#include "fcs16.h"

PPPHDLC_OPT PPPHDLC_DEFAULT_OPTIONS = {
/* ACCM[]   */   {0xFFFFFFFFl, 0x00000000l, 0x00000000l, 0x60000000l,
                  0x00000000l, 0x00000000l, 0x00000000l, 0x00000000l},
/* PFC      */   FALSE,
/* ACFC     */   FALSE
};

/*
** A macro to test whether a character is flagged in the ACCM
*/

#define ACCM_ISSET(c,accm) ((accm)[(c)/PPPHDLC_BITS_IN_UINT32] & \
                           (1UL << ((c)%PPPHDLC_BITS_IN_UINT32)))

static uint_32 _iopcb_ppphdlc_open  (_iopcb_handle, void (_CODE_PTR_)(pointer), void (_CODE_PTR_)(pointer), pointer);
static uint_32 _iopcb_ppphdlc_close (_iopcb_handle);
static void    _iopcb_ppphdlc_write (_iopcb_handle, PCB_PTR, uint_32);
static PCB_PTR _iopcb_ppphdlc_read  (_iopcb_handle, uint_32);
static uint_32 _iopcb_ppphdlc_ioctl (_iopcb_handle, uint_32, pointer);
static void  PPPHDLC_putc (PPPHDLC_STRUCT_PTR, uchar, PPPHDLC_OPT_PTR);
static uchar PPPHDLC_getc (PPPHDLC_STRUCT_PTR, uint_32_ptr, uint_32_ptr, PPPHDLC_OPT_PTR,uint_32_ptr);
static void    PPPHDLC_CCITT16_start (uint_16_ptr);
static void    PPPHDLC_CCITT16_calc  (uint_16_ptr, uchar);
static void    PPPHDLC_CCITT16_send  (PPPHDLC_STRUCT_PTR, PPPHDLC_OPT_PTR);
static boolean PPPHDLC_CCITT16_check (uint_16_ptr);


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_ppphdlc_init
*  Returned Value : a HANDLE to be used by other functions
*  Comments       :
*      Sets some of the values of the ppphdlc structure and
*  returns the handle of the structure.
*
*END*-----------------------------------------------------------------*/

_iopcb_handle _iopcb_ppphdlc_init
   (
      MQX_FILE_PTR device
         /* [IN] - the IO handle*/
   )
{  /* Body */

#if RTCSCFG_ENABLE_IP4

   PPPHDLC_STRUCT_PTR ppphdlc_ptr;
   _ppphdlc_partid    part_id;

   ppphdlc_ptr = PPPHDLC_memalloc(sizeof(PPPHDLC_STRUCT));
   if (!ppphdlc_ptr) {
      return NULL;
   } /* Endif */

   part_id = PPPHDLC_partcreate(sizeof(PCB)
                              + sizeof(PCB_FRAGMENT)
                              + PPPHDLC_FRAMESIZE_MAXDATA
                              + PPPHDLC_FRAMESIZE_FCS,
                                PPPHDLC_INIT, PPPHDLC_GROW, PPPHDLC_MAX, NULL, NULL);
   if (!part_id) {
      return NULL;
   } /* Endif */

   if (PPPHDLC_mutex_init(&ppphdlc_ptr->MUTEX) != PPPHDLC_OK) {
      return NULL;
   } /* Endif */

   PPPHDLC_memcopy(&PPPHDLC_DEFAULT_OPTIONS, &ppphdlc_ptr->SEND_OPT, sizeof(PPPHDLC_OPT));
   PPPHDLC_memcopy(&PPPHDLC_DEFAULT_OPTIONS, &ppphdlc_ptr->RECV_OPT, sizeof(PPPHDLC_OPT));

   ppphdlc_ptr->DEVICE  = device;
   ppphdlc_ptr->PART_ID = part_id;

   ppphdlc_ptr->PCB_TABLE.OPEN  = _iopcb_ppphdlc_open;
   ppphdlc_ptr->PCB_TABLE.CLOSE = _iopcb_ppphdlc_close;
   ppphdlc_ptr->PCB_TABLE.READ  = _iopcb_ppphdlc_read;
   ppphdlc_ptr->PCB_TABLE.WRITE = _iopcb_ppphdlc_write;
   ppphdlc_ptr->PCB_TABLE.IOCTL = _iopcb_ppphdlc_ioctl;

   return &ppphdlc_ptr->PCB_TABLE;

#else

    return NULL;

#endif /* RTCSCFG_ENABLE_IP4  */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_ppphdlc_open
*  Returned Value : uint_32, an ERROR code
*  Comments       :
*      Sets some of the values of the ppphdlc structure and opens the link.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_ppphdlc_open
   (
      _iopcb_handle       handle,
         /* [IN] - the IO handle*/
      void (_CODE_PTR_    up)(pointer),
         /* [IN] - function to put the link up */
      void (_CODE_PTR_    down)(pointer),
         /* [IN] - function to put the link down */
      pointer             param
         /* [IN] - for the up/down functions */
   )
{  /* Body */
   PPPHDLC_STRUCT_PTR ppphdlc_ptr = (PPPHDLC_STRUCT_PTR)((void _PTR_)handle);

   ppphdlc_ptr->UP      = up;
   ppphdlc_ptr->DOWN    = down;
   ppphdlc_ptr->PARAM   = param;

   /* open connection */
   if (ppphdlc_ptr->UP) {
      up(param);
   } /* Endif */

   return PPPHDLC_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_ppphdlc_write
*  Returned Value :
*  Comments       :
*      Transmits the data
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void _iopcb_ppphdlc_write
   (
      _iopcb_handle      handle,
         /* [IN] - the handle */
      PCB_PTR            pcb_ptr,
         /* [IN] - the packet */
      uint_32            flags
         /* [IN] - the flags */
   )
{ /* Body */
   PPPHDLC_STRUCT_PTR ppphdlc_ptr = (PPPHDLC_STRUCT_PTR)((void _PTR_)handle);
   PPPHDLC_OPT        ppphdlc_opt, _PTR_ ppphdlc_opt_ptr;
   uint_32            length;
   uchar_ptr          fragment;
   uint_32            i = 0;

   PPPHDLC_mutex_lock(&ppphdlc_ptr->MUTEX);
   ppphdlc_opt = ppphdlc_ptr->SEND_OPT;
   PPPHDLC_mutex_unlock(&ppphdlc_ptr->MUTEX);

   if (flags & 1) {
      ppphdlc_opt_ptr = &PPPHDLC_DEFAULT_OPTIONS;
   } else {
      ppphdlc_opt_ptr = &ppphdlc_opt;
   } /* Endif */

   /* Send the HDLC flag sequence */
   fputc(PPPHDLC_FLAG, ppphdlc_ptr->DEVICE);
   PPPHDLC_CCITT16_start(&ppphdlc_ptr->FCS_SEND);

   /* Send the header */
   if (!ppphdlc_opt_ptr->ACFC) {
      PPPHDLC_putc(ppphdlc_ptr, PPPHDLC_ADDR, ppphdlc_opt_ptr);
      PPPHDLC_putc(ppphdlc_ptr, PPPHDLC_CTRL, ppphdlc_opt_ptr);
   } /* Endif */

   /* Send the packet */
   length = pcb_ptr->FRAG[i].LENGTH;
   fragment = pcb_ptr->FRAG[i].FRAGMENT;

   /* Compress the protocol field if possible */
   if (!ppphdlc_opt_ptr->PFC || *fragment) {
      PPPHDLC_putc(ppphdlc_ptr, *fragment, ppphdlc_opt_ptr);
   } /* Endif */
   fragment++;
   length--;

   while (length != 0)  {
      while (length != 0) {
         PPPHDLC_putc(ppphdlc_ptr, *fragment++, ppphdlc_opt_ptr);
         length--;
      } /* Endwhile */
      i++;
      length = pcb_ptr->FRAG[i].LENGTH;
      fragment = pcb_ptr->FRAG[i].FRAGMENT;
   } /* Endwhile */

   /* Send the checksum */
   PPPHDLC_CCITT16_send(ppphdlc_ptr, ppphdlc_opt_ptr);
   ppphdlc_ptr->STATS.COMMON.ST_TX_TOTAL++;

   /* Verify the checksum */
   if (!PPPHDLC_CCITT16_check(&ppphdlc_ptr->FCS_SEND))  {
      ppphdlc_ptr->STATS.COMMON.ST_TX_MISSED++;
   } /* Endif */

   fputc(PPPHDLC_FLAG, ppphdlc_ptr->DEVICE);
   PCB_free(pcb_ptr);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_ppphdlc_read
*  Returned Value : PCB_PTR
*  Comments       :
*      Receives the data
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static PCB_PTR _iopcb_ppphdlc_read
   (
      _iopcb_handle    handle,
         /* [IN] - the structure handle */
      uint_32          flags
         /* [IN] - flags */
   )
{  /* Body */
   PPPHDLC_STRUCT_PTR ppphdlc_ptr = (PPPHDLC_STRUCT_PTR)((void _PTR_)handle);
   PCB_FRAGMENT _PTR_ pcb_frag_ptr;
   PPPHDLC_OPT        ppphdlc_opt;
   PCB_PTR            pcb;
   uint_32            err;
   uint_32            retry,wait;
   uint_32           *rx_stop_ptr;     
   uchar_ptr          p;
   uint_16            data_len;
   register uchar     c;
   
   /* 
    * If device does not have any data it has not sense to create PCB and wait for data. 
    * It take less resources.
    */
   if(!fstatus(ppphdlc_ptr->DEVICE))  
   {  
        return NULL;  
   }
   rx_stop_ptr = (uint_32 *)flags;
   PPPHDLC_mutex_lock(&ppphdlc_ptr->MUTEX);
   ppphdlc_opt = ppphdlc_ptr->RECV_OPT;
   PPPHDLC_mutex_unlock(&ppphdlc_ptr->MUTEX);

   retry = 20;
   pcb = NULL;
   while (!pcb) 
   {
      pcb = PPPHDLC_partalloc(ppphdlc_ptr->PART_ID);
      if (!pcb) 
      {
         ppphdlc_ptr->STATS.COMMON.ST_RX_DISCARDED++;
         PPPHDLC_delay(1);
         return NULL;
      } /* Endif */
   } /* Endwhile */

   pcb->FREE = (void (_CODE_PTR_)(PCB_PTR))PPPHDLC_partfree;
   pcb->PRIVATE = pcb;
   pcb_frag_ptr = pcb->FRAG;
   pcb_frag_ptr->FRAGMENT = (uchar_ptr)pcb + sizeof(PCB) + sizeof(PCB_FRAGMENT);
   pcb_frag_ptr++;
   pcb_frag_ptr->LENGTH = 0;
   pcb_frag_ptr->FRAGMENT = NULL;

restart:
   retry--;  
   if(!retry)  
   {  
   /* if we cannot take right packet we are returning 0 pcb */
     pcb->FRAG[0].LENGTH = 0;  
     return pcb;  
   }   
   
   p = pcb->FRAG[0].FRAGMENT;
   /* zerou's check summ  */
   PPPHDLC_CCITT16_start(&ppphdlc_ptr->FCS_RECV);

   c = PPPHDLC_getc(ppphdlc_ptr, &err, NULL, &ppphdlc_opt,rx_stop_ptr);
   if (err)
   {  
      goto restart;
   }
   /* Receive the frame header */
   /* 
      ACFC -Address-and-Control-Field-Compression. 
      So, if it is not compressed and it is not 0xFF 
      start wait 200 times for start of PPP packet 0x7E
   */
   if (!ppphdlc_opt.ACFC && (c != PPPHDLC_ADDR)) 
   {
      ppphdlc_ptr->STATS.ST_RX_BAD_ADDR++;
      wait = 200;
      while(wait) 
      {
          while(!fstatus(ppphdlc_ptr->DEVICE))
          {
          /* checking if RX_STOP is 1, it means that task aborted */
            if(*rx_stop_ptr)
            {
                if(pcb != NULL)  
                {
                    PCB_free(pcb);
                }   
              return(0);  
            }
            _time_delay(1);
          }
          if(fgetc(ppphdlc_ptr->DEVICE) == PPPHDLC_FLAG)
          {
            wait=0;
          }
 	  wait--;
      }  
      goto restart;
   } /* Endif */

   /* If we got one, wait for control byte */
   /*
      If we reach here  it means that we received 0xFF (PPPHDLC_ADDR , and before it 0x7E ,start PPP)
      So , now we are reading for 0x03 (PPPHDLC_CTRL) 
   */
   if (c == PPPHDLC_ADDR) 
   {
      c = PPPHDLC_getc(ppphdlc_ptr, &err, &ppphdlc_ptr->STATS.ST_RX_RUNT, &ppphdlc_opt,rx_stop_ptr);
      if (err) goto restart;

      if (c != PPPHDLC_CTRL) 
      {
        ppphdlc_ptr->STATS.ST_RX_BAD_CTRL++;
         
        /* new fix */
        wait = 200; 
        while(wait) 
 	{
          while(!fstatus(ppphdlc_ptr->DEVICE))
          {
            /* checking if RX_STOP is 1, it means that task aborted */
            if(*rx_stop_ptr)
            {
                if(pcb != NULL)  
                {
                    PCB_free(pcb);
                }   
              return(0);  
            }
            _time_delay(1);
          }
          if(fgetc(ppphdlc_ptr->DEVICE) == PPPHDLC_FLAG)
          {
            wait=0;
          }
 	  wait--;
        }  
        goto restart;
      } /* Endif */
      c = PPPHDLC_getc(ppphdlc_ptr, &err, &ppphdlc_ptr->STATS.ST_RX_RUNT, &ppphdlc_opt,rx_stop_ptr);
      if (err) goto restart;
   } /* Endif */

   data_len = 0;

   /* Decompress the protocol field if necessary */
   if (c & 1) 
   {
      *p++ = 0;
      ++data_len;
   } /* Endif */

   /* We must recieve at least a frame check sequence */
   while (data_len < PPPHDLC_FRAMESIZE_FCS) 
   {
      *p++ = c;
      ++data_len;
      c = PPPHDLC_getc(ppphdlc_ptr, &err, &ppphdlc_ptr->STATS.ST_RX_RUNT, &ppphdlc_opt,rx_stop_ptr);
      if (err) goto restart;
   } /* Endwhile */

   /* Receive data */
   for (;;) 
   {
 
      *p++ = c;
      ++data_len;
      c = PPPHDLC_getc(ppphdlc_ptr, &err, NULL, &ppphdlc_opt,rx_stop_ptr);


      if (err == PPPHDLC_END)
      { 
        break;
      }  
      if (err == PPPHDLC_ABORT)
      { 
        goto restart;
      }  
      /*
        If data_len >= maximum packet size - it means some error happend or some unsupported packet come.
        So, we start to wait for another valid packet waiting 200 times for  0x7E (PPPHDLC_FLAG - start of PPP packet)
      */
      if (data_len >= (PPPHDLC_FRAMESIZE_MAXDATA + PPPHDLC_FRAMESIZE_FCS)) 
      {
         ppphdlc_ptr->STATS.ST_RX_GIANT++;

         wait = 200; 
         while(wait) 
 	 {
            while(!fstatus(ppphdlc_ptr->DEVICE))
            {
             /* checking if RX_STOP is 1, it means that task aborted */
                 if(*rx_stop_ptr)
                 {
                     if(pcb != NULL)  
                     {
                         PCB_free(pcb);
                     }   
                     return(0);  
                 }
                 _time_delay(1);
            }
            if(fgetc(ppphdlc_ptr->DEVICE) == PPPHDLC_FLAG)
            {
                 wait=0;
            }
 	    wait--;
 	}          

        goto restart;
      } /* Endif */
   } /* Endfor */

   /* Validate the FCS */
   /*
      checking controll summ here (last 2 bytes)
   */
   if (!PPPHDLC_CCITT16_check(&ppphdlc_ptr->FCS_RECV))  
   {
      ppphdlc_ptr->STATS.ST_RX_BAD_FCS++;
      goto restart;
   } /* Endif */
   pcb->FRAG[0].LENGTH = data_len - PPPHDLC_FRAMESIZE_FCS;
   return pcb;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_ppphdlc_close
*  Returned Value : uint_32, an ERROR code
*  Comments       :
*     Close link and free memory
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_ppphdlc_close
   (
      _iopcb_handle  handle
         /* [IN] - the structure handle */
   )
{  /* Body */
   PPPHDLC_STRUCT_PTR ppphdlc_ptr = (PPPHDLC_STRUCT_PTR)((void _PTR_)handle);

   /* close connection */
   if (ppphdlc_ptr->DOWN) {
      ppphdlc_ptr->DOWN(ppphdlc_ptr->PARAM);
   } /* Endif */

   /* Free memory */
   /*Free memory block by block. */
   PPPHDLC_partdestroy(ppphdlc_ptr->PART_ID);
   PPPHDLC_mutex_destroy(&ppphdlc_ptr->MUTEX);
   PPPHDLC_memfree(ppphdlc_ptr);

   return PPPHDLC_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _iopcb_ppphdlc_ioctl
*  Returned Value : an ERROR code
*  Comments       :
*      Store and set the options
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uint_32 _iopcb_ppphdlc_ioctl
   (
      _iopcb_handle    handle,
         /* [IN] - the structure handle */
      uint_32          option,
         /* [IN] - options */
      pointer          value
         /* [IN] - option value */
   )
{  /* Body */
   PPPHDLC_STRUCT_PTR ppphdlc_ptr = (PPPHDLC_STRUCT_PTR)((void _PTR_)handle);

   PPPHDLC_mutex_lock(&ppphdlc_ptr->MUTEX);

   switch(option)  {
      case IOPCB_IOCTL_S_ACCM:
         ppphdlc_ptr->SEND_OPT.ACCM[0] = *(uint_32 _PTR_)value;
         break;
      case IOPCB_IOCTL_R_ACCM:
         ppphdlc_ptr->RECV_OPT.ACCM[0] = *(uint_32 _PTR_)value;
         break;
      case IOPCB_IOCTL_S_PFC:
         ppphdlc_ptr->SEND_OPT.PFC = *(boolean _PTR_)value;
         break;
      case IOPCB_IOCTL_R_PFC:
         ppphdlc_ptr->RECV_OPT.PFC = *(boolean _PTR_)value;
         break;
      case IOPCB_IOCTL_S_ACFC:
         ppphdlc_ptr->SEND_OPT.ACFC = *(boolean _PTR_)value;
         break;
      case IOPCB_IOCTL_R_ACFC:
         ppphdlc_ptr->RECV_OPT.ACFC = *(boolean _PTR_)value;
         break;
      case IOPCB_IOCTL_GET_IFTYPE:
         *(uint_32_ptr)value = IPIFTYPE_RS232;
         break;
   } /* Endswitch */

   PPPHDLC_mutex_unlock(&ppphdlc_ptr->MUTEX);

   return PPPHDLC_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPPHDLC_putc
* Returned Value  :  None
* Comments        :
*     Outputs one character, escaped if necessary, and updates
*     the FCS.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void PPPHDLC_putc
   (
      PPPHDLC_STRUCT_PTR  handle,
            /* [IN] - the ppphdlc structure */
      uchar               c,
            /* [IN] - character to send */
      PPPHDLC_OPT_PTR     opt
            /* [IN] - the options */
   )
{ /* Body */
   
   if (ACCM_ISSET(c, opt->ACCM)) {
      fputc(PPPHDLC_ESC, handle->DEVICE);
      fputc((char)c ^ 0x20, handle->DEVICE);
   } else {
      fputc((char)c, handle->DEVICE);
   } /* Endif */

   PPPHDLC_CCITT16_calc(&handle->FCS_SEND, c);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPPHDLC_getc
* Returned Value  :  character
* Comments        :
*     Retrieves one character and updates the FCS.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static uchar PPPHDLC_getc
   (
      PPPHDLC_STRUCT_PTR  handle,
            /* [IN] - the handle of the ppphdlc structure */
      uint_32_ptr         error,
            /* [OUT] - end of frame condition */
      uint_32_ptr         stat,
            /* [IN] - statistics counter to increment on flag character */
      PPPHDLC_OPT_PTR     opt,
            /* [IN] - the options */
      uint_32_ptr         rx_stop_ptr      
   )
{ /* Body */
   uchar c;
   boolean esc = FALSE;

   for (;;) 
   {
   
        /* We will read from device only if it has any data*/
        /*TBD read with timeout or with abort should be used here. But not implemented yet in MQX. */
        while(!fstatus(handle->DEVICE))
        {
        /* checking if RX_STOP is 1 it means that task aborted */
            if(*rx_stop_ptr)
            {
                *error = PPPHDLC_ABORT; 
                return(0);  
            }
            _time_delay(1);
        }   
 
        c = (uchar)fgetc(handle->DEVICE);

         /* The flag sequence ends a frame */
      if (c == PPPHDLC_FLAG) 
      {
         if (esc) 
         {
            handle->STATS.ST_RX_ABORTED++;
            *error = PPPHDLC_ABORT;
         } 
         else 
         {
            if (stat) (*stat)++;
            *error = PPPHDLC_END;
         } /* Endif */
         return 0;
      } /* Endif */

      /* Toss all flagged control characters */
      if (c < 0x20 && ACCM_ISSET(c, opt->ACCM)) 
      {
         continue;
      } /* Endif */

      /* Escaped characters get bit 5 toggled */
      if (esc) 
      {
         c ^= 0x20;
      } 
      else if (c == PPPHDLC_ESC) 
      {
         esc = TRUE;
         continue;
      } /* Endif */

      PPPHDLC_CCITT16_calc(&handle->FCS_RECV, c);
      *error = PPPHDLC_OK;
      return c;
   } /* Endfor */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPPHDLC_CCITT16_start
* Returned Value  :  None
* Comments        :
*     Initializes the FCS_DATA for a CRC calculation.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void PPPHDLC_CCITT16_start
   (
      uint_16_ptr        fcs
            /* [IN] -  fcs information */
   )
{ /* Body */
   *fcs = 0xFFFF;
} /* Endbody */

#endif 

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPPHDLC_CCITT16_calc
* Returned Value  :  None
* Comments        :
*     Updates the CRC calculation given a new character.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void PPPHDLC_CCITT16_calc
   (
      uint_16_ptr    fcs,
            /* [IN] - FCS information */
      uchar          c
            /* [IN] - character */
   )
{ /* Body */
   uint_16 crc = *fcs;

   crc = (crc >> 8) ^ fcstab[(crc ^ c) & 0xFF];
   *fcs = crc;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPPHDLC_CCITT16_send
* Returned Value  :  None
* Comments        :
*     Sends a CRC.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static void PPPHDLC_CCITT16_send
   (
      PPPHDLC_STRUCT_PTR   ppphdlc_ptr,
            /* [IN] - the handle of the ppphdlc structure */
      PPPHDLC_OPT_PTR      opt
            /* [IN] - the options */
   )
{ /* Body */
   uint_16 crc = ~ppphdlc_ptr->FCS_SEND & 0xFFFF;
   PPPHDLC_putc(ppphdlc_ptr,  crc      & 0xFF, opt);
   PPPHDLC_putc(ppphdlc_ptr, (crc>>=8) & 0xFF, opt);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  PPPHDLC_CCITT16_check
* Returned Value  :  TRUE if CRC is correct
* Comments        :
*     Validates a CRC calculation.
*
*END*-----------------------------------------------------------------*/

#if RTCSCFG_ENABLE_IP4

static boolean PPPHDLC_CCITT16_check
   (
      uint_16_ptr   fcs
            /* [IN] - FCS information */
   )
{ /* Body */

   return (*fcs == 0xF0B8);

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */


/* EOF */
