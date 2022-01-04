#ifndef _usb_dcd_kn__prv_h
#define _usb_dcd_kn__prv_h
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
* $FileName: usb_dcd_kn_prv.h$
* $Version : 3.8.1.0$
* $Date    : Jul-27-2011$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the USB DCD drivers for Kinetis family.
*
*END************************************************************************/

#include "usb_dcd_kn.h"

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** KUSB_DCD_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct kusb_dcd_info_struct
{  
	/* Current initialized values */
   KUSB_DCD_INIT_STRUCT              INIT;
   
   USBDCD_MemMapPtr                  USB_DCD_PTR;
   
   /* USB_DCD Method (polled or interrupt) */
   uint_8										 METHOD;
   
   /* The previous interrupt handler and data */
   void                  (_CODE_PTR_ OLD_ISR)(pointer);
   pointer                           OLD_ISR_DATA;

   /* DCD DETECTION SEQUENCE STATE */
   uint_8                            STATE;

} KUSB_DCD_INFO_STRUCT, _PTR_ KUSB_DCD_INFO_STRUCT_PTR; 


#endif
/* EOF */
