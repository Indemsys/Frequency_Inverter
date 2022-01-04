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
* $FileName: rtcsload.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the interface functions to the
*   Exec function of the RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint_32 RTCS_load
   (
      FT_CALL_STRUCT_PTR   ft_ptr,
         /* [IN] file transfer protocol to use */
      pointer              ft_data,
         /* [IN] file transfer data */
      FF_CALL_STRUCT_PTR   ff_ptr,
         /* [IN] file format to decode */
      pointer              ff_data,
         /* [IN] file format data */
      uint_32              flags
         /* [IN] optional flags */
   )
{ /* Body */
   uint_32   error;  /* also used as packet size */
   uchar_ptr data;

   /*
   ** Initialize
   */
   error = (*ft_ptr->OPEN)(ft_data);
   if (error) {
      return error;
   } /* Endif */
   error = (*ff_ptr->START)(ff_data);
   if (error) {
      (*ft_ptr->CLOSE)();
      return error;
   } /* Endif */

   /*
   ** Repeat until end of file
   */
   while (!(*ft_ptr->EOFT)()) {

      /*
      ** Retrieve some data...
      */
      data = (*ft_ptr->READ)(&error);
      if (!data) {
         (*ff_ptr->EOFD)();
         return error;
      } /* Endif */

      /*
      ** ...and decode it
      **
      ** Note:  If data is non-NULL, then error contains the
      **        number of bytes read.
      */
      error = (*ff_ptr->DECODE)(error, data);
      if (error) {
         (*ft_ptr->CLOSE)();
         return error;
      } /* Endif */
   } /* Endwhile */

   /*
   ** Check if the decoding succeeded
   */
   error = (*ff_ptr->EOFD)();
   if (error) {
      (*ft_ptr->CLOSE)();
      return error;
   } /* Endif */

   /*
   ** End the file transfer session
   */
   error = (*ft_ptr->CLOSE)();
   if (error) {
      return error;
   } /* Endif */

   /*
   ** If we get here, the file was retrieved and decoded successfully.
   ** Run the image.
   */
   if (flags & RTCSLOADOPT_EXEC) {
      (*ff_ptr->EXEC)();
   } /* Endif */

   return RTCS_OK;

} /* Endbody */


/* EOF */
