/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: mqx_main.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the main C language entrypoint, starting up the MQX
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : main
* Returned Value   : should return "status"
* Comments         :
*   Starts MQX running
*
*END*----------------------------------------------------------------------*/
int main(void)
{ /* Body */

   extern const MQX_INITIALIZATION_STRUCT MQX_init_struct;

   /* Start MQX */
   _mqx( (MQX_INITIALIZATION_STRUCT_PTR) &MQX_init_struct );
   return 0;

} /* Endbody */
