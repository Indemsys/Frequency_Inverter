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
* $FileName: if_dev_khci.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file exports USB KHCI host initial structure
*
*END************************************************************************/
#ifndef __if_dev_khci_h__
#define __if_dev_khci_h__ 1

#include <mqx.h> //pointer types

#ifdef __cplusplus
extern "C" {
#endif

extern const struct usb_dev_callback_functions_struct _usb_khci_dev_callback_table;

typedef struct usb_khci_dev_init_struct {
    pointer       BASE_PTR;
    _mqx_uint     VECTOR;
    _mqx_uint     PRIORITY;
    _mqx_uint     MAX_ENDPOINTS;
} USB_KHCI_DEV_INIT_STRUCT, _PTR_ USB_KHCI_DEV_INIT_STRUCT_PTR;

#ifdef __cplusplus
}
#endif

#endif
