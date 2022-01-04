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
* $FileName: enerr.c$
* $Version : 3.8.7.0$
* $Date    : Sep-20-2012$
*
* Comments:
*
*   This file contains the Ethernet error reporting
*   functions.
*
*END************************************************************************/

#include <mqx.h>
#include "enet.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_strerror
*  Returned Value : pointer to error string
*  Comments       :
*        Returns a string describing an error code
*
*END*-----------------------------------------------------------------*/

static const char_ptr ENET_errlist[ENETERR_MAX - ENETERR_MIN + 1] = {
   "Invalid device number",                           /* ENETERR_INVALID_DEVICE     */
   "Device already initialized",                      /* ENETERR_INIT_DEVICE        */
   "Couldn't allocate state (out of memory)",         /* ENETERR_ALLOC_CFG          */
   "Couldn't allocate PCBs (out of memory)",          /* ENETERR_ALLOC_PCB          */
   "Couldn't allocate buffer descriptors",            /* ENETERR_ALLOC_BD           */
   "Couldn't install Ethernet notifier",              /* ENETERR_INSTALL_ISR        */
   "Some PCBs are still in use",                      /* ENETERR_FREE_PCB           */
   "Out of memory",                                   /* ENETERR_ALLOC_ECB          */
   "Protocol already open",                           /* ENETERR_OPEN_PROT          */
   "Not an open protocol",                            /* ENETERR_CLOSE_PROT         */
   "Packet too short",                                /* ENETERR_SEND_SHORT         */
   "Packet too long",                                 /* ENETERR_SEND_LONG          */
   "Not a multicast address",                         /* ENETERR_JOIN_MULTICAST     */
   "Out of memory",                                   /* ENETERR_ALLOC_MCB          */
   "Not a joined group",                              /* ENETERR_LEAVE_GROUP        */
   "Tranmit ring full",                               /* ENETERR_SEND_FULL          */
   "IP Table full of IP pairs",                       /* ENETERR_IP_TABLE_FULL      */
   "Generic alloc failed",                            /* ENETERR_ALLOC              */
   "Device failed to initialize",                     /* ENETERR_INIT_FAILED        */
   "Device read or write timeout",                    /* ENETERR_DEVICE_TIMEOUT     */
   "Couldn't allocate buffers (out of memory)",       /* ENETERR_ALLOC_BUFFERS      */
   "Couldn't allocate MAC context (out of memory)",   /* ENETERR_ALLOC_MAC_CONTEXT  */
   "Couldn't allocate TX buffer (out of memory)",     /* ENETERR_NO_TX_BUFFER       */
   "Invalid initialization parameter",                /* ENETERR_INVALID_INIT_PARAM */
   "Shutdown failed, device in use",                  /* ENETERR_DEVICE_IN_USE      */
   "Device already initialized",                      /* ENETERR_INITIALIZED_DEVICE */
   "Setting of ESSID in progress",                    /* ENETERR_INPROGRESS         */
   "1588 driver lwevent creation failed",             /* ENETERR_1588_LWEVENT       */
   "Invalid mode for this ethernet driver",           /* ENETERR_INVALID_MODE       */
   "Invalid option for this ethernet driver"          /* ENETERR_INVALID_OPTION     */
};



const char * ENET_strerror
   (
      /* [IN] the ENET error code */
      _mqx_uint  error
   )
{ 
   if (error == ENET_OK) {
      return "OK";
   } 
   if ((error < ENETERR_MIN) || (error > ENETERR_MAX)) {
      return "Unknown error";
   } 
   return ENET_errlist[error - ENETERR_MIN];
}


/* EOF */
