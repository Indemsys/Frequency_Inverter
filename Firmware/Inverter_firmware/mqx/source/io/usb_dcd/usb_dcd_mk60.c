/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: usb_dcd_mk60.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains board-specific USB DCD initialization functions.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "usb_dcd_kn.h"

/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_usb_dcd_base_address
* Returned Value   : address if successful, NULL otherwise
* Comments         :
*    This function returns the base register address of the corresponding USB DCD device.
*
*END*********************************************************************/

pointer _bsp_get_usb_dcd_base_address
(
)
{
    return (pointer) USBDCD_BASE_PTR;
}

/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_usb_dcd_vector
* Returned Value   : vector number if successful, 0 otherwise
* Comments         :
*    This function returns desired interrupt vector number for specified USB DCD device.
*
*END*********************************************************************/

uint_32 _bsp_get_usb_dcd_vector
(
)
{
    return INT_USBDCD;
}
/* EOF */
