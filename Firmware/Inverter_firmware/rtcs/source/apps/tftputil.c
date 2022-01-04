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
* $FileName: tftputil.c$
* $Version : 3.0.5.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the TFTP utility implementation for
*   the RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>
#include "tftp.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_init
* Returned Value  : time to expiry in milliseconds
* Comments        : This function initializes the adaptive timeout.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_init
   (
      TFTP_TO_STRUCT_PTR   to
   )
{ /* Body */

   to->TS     = RTCS_time_get();
   to->UPDATE = TRUE;
   to->TO     = TFTP_TIMEOUT_MIN;
   to->M      = TFTP_TIMEOUT_MIN;
   to->D      = 0;

   return to->TO;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_restart
* Returned Value  : time to expiry in milliseconds
* Comments        : This function causes the next call to
*                   TFTP_timeout_update to not update the adaptive
*                   timeout.  Used when retransmitting before expiry.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_restart
   (
      TFTP_TO_STRUCT_PTR   to
   )
{ /* Body */

   to->TS     = RTCS_time_get();
   to->UPDATE = FALSE;

   return to->TO;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_update
* Returned Value  : time to expiry in milliseconds
* Comments        : This function updates the timeout when a response
*                   is received without retransmission.
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_update
   (
      TFTP_TO_STRUCT_PTR   to
   )
{ /* Body */
   uint_32 time, time_elapsed;
   int_32 error;

   time = RTCS_time_get();
   if (to->UPDATE) {
      time_elapsed = RTCS_timer_get_interval(to->TS, time);
      error = time_elapsed - to->M;
      to->M += error >> 3;
      if (!to->M) to->M++;
      if (error < 0) error = -error;
      error -= to->D;
      to->D += error >> 2;
      if (!to->D) to->D++;
      to->TO = to->M + (to->D << 2);
   } /* Endif */

   to->TS     = time;
   to->UPDATE = TRUE;

   return to->TO;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTP_timeout_left
* Returned Value  : time to expiry in milliseconds
* Comments        : This function determines the amout of time until
*                   the timeout expires
*
*END*-----------------------------------------------------------------*/

uint_32 TFTP_timeout_left
   (
      TFTP_TO_STRUCT_PTR   to,
      boolean _PTR_        expire
   )
{ /* Body */
   uint_32 time;

   time = RTCS_time_get();
   if ((time - to->TS) >= to->TO) {
      to->TS     = time;
      to->UPDATE = FALSE;
      to->TO <<= 1;
      if (to->TO > TFTP_TIMEOUT_MAX) {
         to->TO = TFTP_TIMEOUT_MAX;
      } /* Endif */
      if (expire) *expire = TRUE;
      return to->TO;
   } /* Endif */

   if (expire) *expire = FALSE;
   return to->TO - (time - to->TS);

} /* Endbody */

#define TFTP_ERROR_PKT(err,str)  {{0, TFTPOP_ERROR}, {0, err}, str}

/* The Illegal TID error packet */
TFTP_ERROR_PACKET _tftp_error_tid    = TFTP_ERROR_PKT(TFTPERR_ILLEGAL_TID, "Invalid TID");

/* The Illegal Operation error packet */
TFTP_ERROR_PACKET _tftp_error_op     = TFTP_ERROR_PKT(TFTPERR_ILLEGAL_OP, "Illegal Operation");

/* The Access Violation error packet */
TFTP_ERROR_PACKET _tftp_error_accvio = TFTP_ERROR_PKT(TFTPERR_ACCESS_VIOLATION, "Access Violation");

/* The Server Failure error packet */
TFTP_ERROR_PACKET _tftp_error_srv    = TFTP_ERROR_PKT(TFTPERR_UNKNOWN, "TFTP Server Failure");

/* The Server Busy error packet */
TFTP_ERROR_PACKET _tftp_error_busy   = TFTP_ERROR_PKT(TFTPERR_UNKNOWN, "TFTP Server Busy");

/* The Excessive Retrasnmissions error packet */
TFTP_ERROR_PACKET _tftp_error_to     = TFTP_ERROR_PKT(TFTPERR_UNKNOWN, "Excessive Retransmissions");

/* The File Not Found error packet */
TFTP_ERROR_PACKET _tftp_error_nofile = TFTP_ERROR_PKT(TFTPERR_FILE_NOT_FOUND, "File Not Found");

/* The File Already Exists error packet */
TFTP_ERROR_PACKET _tftp_error_exists = TFTP_ERROR_PKT(TFTPERR_FILE_EXISTS, "File Already Exists");


/* EOF */
