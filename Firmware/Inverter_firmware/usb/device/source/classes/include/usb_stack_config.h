/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: usb_stack_config.h$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   User configuration for USB Stack components
*

*END************************************************************************/

#ifndef __usb_stack_config_h__
#define __usb_stack_config_h__

#ifndef HID_CONFIG
#define HID_CONFIG                   1
#endif

#ifndef CDC_CONFIG
#define CDC_CONFIG                   1
#endif


#ifndef AUDIO_CONFIG
#define AUDIO_CONFIG                 1
#endif

#ifndef MSD_CONFIG
#define MSD_CONFIG                   1
#endif

#ifndef PHDC_CONFIG
#define PHDC_CONFIG                  1
#endif

#if CDC_CONFIG
    #define PSTN_SUBCLASS_NOTIF_SUPPORT          (1)/* 1: TRUE; 0: FALSE*/ 
#endif

#if PHDC_CONFIG
    #define USB_METADATA_SUPPORTED              (0)/* 1: TRUE; 0: FALSE*/ 
#endif

#define IMPLEMENT_QUEUING                    (1)/* 1: TRUE; 0: FALSE*/ 

#endif
/* EOF */
