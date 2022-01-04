/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName: enmediactl.c$
* $Version : 3.5.2.0$
* $Date    : Nov-27-2009$
*
* Comments:
*
*   This file contains the Ethernet IOCTL implementation
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_mediactl
*  PARAMS         : handle -> handle to enet layer.
*                   command_id -> ENET command to execute.
*                   inout_param -> input/output data for command.
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Joins a multicast group on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/
uint_32 ENET_mediactl
   (
      /* [IN] the Ethernet state structure */
      _enet_handle      handle,

      /* [IN] IOCTL COMMAND */
      uint_32           command_id,

      /* [IN]/[OUT] input/output param */
      pointer     inout_param
   )
{ 
   uint_32 error = ENET_ERROR;
   ENET_CONTEXT_STRUCT_PTR enet_ptr = (ENET_CONTEXT_STRUCT_PTR)handle;

   #if MQX_CHECK_ERRORS
      if (NULL == handle) {
         return ENET_ERROR;
      } /* Endif */
   #endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself and with ENET_leave.
   */
   if (enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->MEDIACTL)
   {  
      error =  (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->MEDIACTL)(enet_ptr, command_id,inout_param);
   }
   
   return error;
}
/* EOF */
