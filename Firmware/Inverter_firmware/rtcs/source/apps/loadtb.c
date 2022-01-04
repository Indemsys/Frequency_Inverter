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
* $FileName: loadtb.c$
* $Version : 3.0.4.0$
* $Date    : Mar-5-2009$
*
* Comments:
*
*   This file contains the interface functions to RTCS_load()
*   for binary files using TFTP.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_BIN
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load_TFTP_BIN
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename,
         /* [IN] file to retrieve */
      uchar_ptr            dl_addr
         /* [IN] download start address */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;
   BINFILE_DATA_STRUCT  binfile_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   binfile_data.DOWNLOAD_ADDR = dl_addr;

   return RTCS_load(FT_TFTP, &tftp_data, FF_BIN, &binfile_data, 0);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_BIN
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_exec_TFTP_BIN
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char_ptr             filename,
         /* [IN] file to retrieve */
      uchar_ptr            dl_addr,
         /* [IN] download start address */
      uchar_ptr            exec_addr
         /* [IN] execution start address */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;
   BINFILE_DATA_STRUCT  binfile_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   binfile_data.DOWNLOAD_ADDR = dl_addr;
   binfile_data.EXEC_ADDR     = exec_addr;

   return RTCS_load(FT_TFTP, &tftp_data, FF_BIN, &binfile_data, RTCSLOADOPT_EXEC);

} /* Endbody */

#endif
/* EOF */
