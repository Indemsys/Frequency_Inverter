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
* $FileName: loadts.c$
* $Version : 3.0.4.0$
* $Date    : Mar-5-2009$
*
* Comments:
*
*   This file contains the interface functions to RTCS_load()
*   for S-Record files using TFTP.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_SREC
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load_TFTP_SREC
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT  tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "netascii";

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 32)
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC32, NULL, 0);
#else
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC,   NULL, 0);
#endif

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_SREC
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_exec_TFTP_SREC
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT  tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "netascii";

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 32)
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC32, NULL, RTCSLOADOPT_EXEC);
#else
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC,   NULL, RTCSLOADOPT_EXEC);
#endif

} /* Endbody */

#endif
/* EOF */
