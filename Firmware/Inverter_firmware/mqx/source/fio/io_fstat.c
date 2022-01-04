/*HEADER*********************************************************************
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
* $FileName: io_fstat.c$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the function for checking the status of input.
*
*END************************************************************************/

#include "mqx.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"

/*!
 * \brief Checks the HAVE_UNGOT_CHARACTER of the input_ucb and if none is 
 * available, call the device status function.
 * 
 * \param[in] file_ptr The stream whose status is desired.
 * 
 * \return TRUE (HAVE_UNGOT_CHARACTER is TRUE), FALSE (HAVE_UNGOT_CHARACTER is 
 * TRUE).
 * \return Value form device status function.  
 */ 
boolean _io_fstatus
   (
      MQX_FILE_PTR file_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR   dev_ptr;
   boolean                result;

#if MQX_CHECK_ERRORS
   if (file_ptr == NULL) {
      return (FALSE);
   } /* Endif */
#endif

   if ( file_ptr->HAVE_UNGOT_CHARACTER ) {
      return (TRUE);
   } else {
      dev_ptr = file_ptr->DEV_PTR;
      if (dev_ptr->IO_IOCTL != NULL) {   
         (*dev_ptr->IO_IOCTL)(file_ptr, IO_IOCTL_CHAR_AVAIL, &result);
         return(result);
      } /* Endif */
   } /* Endif */
   return (FALSE);

} /* Endbody */

/* EOF */
