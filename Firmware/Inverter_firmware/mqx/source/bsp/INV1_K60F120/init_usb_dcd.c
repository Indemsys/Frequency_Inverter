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
* $FileName: init_usb_dcd.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definition for the USB DCD
*   channel
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "usb_dcd.h"
#include "usb_dcd_kn.h"

const KUSB_DCD_INIT_STRUCT _bsp_usb_dcd_init = {
      USBDCD_CLOCK_SPEED_RESET_VALUE,       
      USBDCD_TSEQ_INIT_RESET_VALUE,         
      USB_DCD_TDCD_DBNC_RESET_VALUE,  
      USB_DCD_TVDPSRC_ON_RESET_VALUE,
      USB_DCD_TVDPSRC_CON_RESET_VALUE,   
      USB_DCD_CHECK_DM_RESET_VALUE,   
      USB_DCD_LEVEL_RESET_VALUE   
};
