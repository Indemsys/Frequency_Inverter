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
* $FileName: binfile.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the binary file decoder for the
*   Exec function of the RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>


/* The binary file decoder information to pass to RTCS_load() */
uint_32 BIN_start  (pointer ff_data);
uint_32 BIN_decode (uint_32 size, uchar_ptr data);
uint_32 BIN_eod    (void);
void    BIN_exec   (void);

static FF_CALL_STRUCT ff_bin = {
   BIN_start,
   BIN_decode,
   BIN_eod,
   BIN_exec
};

const FF_CALL_STRUCT_PTR FF_BIN = &ff_bin;


/* Binary file decoder state information */
static uchar_ptr download_address;
static uchar_ptr execute_address;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_start
* Returned Value : error code
* Comments       : Routine to begin loading a binary file.
*
*END*-----------------------------------------------------------------*/

uint_32 BIN_start
   (
      pointer  ff_data
   )
{ /* Body */

   download_address = ((BINFILE_DATA_STRUCT_PTR)ff_data)->DOWNLOAD_ADDR;
   execute_address  = ((BINFILE_DATA_STRUCT_PTR)ff_data)->EXEC_ADDR;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_decode
* Returned Value : status
* Comments       : Copy the transferred bytes to the target.
*
*END*-----------------------------------------------------------------*/

uint_32 BIN_decode
   (
      uint_32   size,
         /* [IN] number of bytes to decode */
      uchar_ptr data
         /* [IN] bytes to decode */
   )
{ /* Body */

   _mem_copy(data, download_address, size);
   download_address += size;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_eod
* Returned Value : error code
* Comments       : Routine to end decoding.
*
*END*-----------------------------------------------------------------*/

uint_32 BIN_eod
   (
      void
   )
{ /* Body */

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_exec
* Returned Value : none
* Comments       : Routine to execute a decoded binary file.
*
*END*-----------------------------------------------------------------*/

void BIN_exec
   (
      void
   )
{ /* Body */

   (*(void(_CODE_PTR_)(void))(uint_32)execute_address)();

} /* Endbody */


/* EOF */
