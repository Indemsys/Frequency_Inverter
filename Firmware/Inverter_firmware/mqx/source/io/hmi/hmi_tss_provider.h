/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: hmi_tss_provider.h$
* $Version : 3.8.2.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This header file is for HMI lwgpio provider. It contains  defines,
*   structures.
*
*END************************************************************************/
#ifndef __hmi_tss_provider_h__
#define __hmi_tss_provider_h__

#include "hmi_provider.h"
#include "hmi_client.h"

typedef struct hmi_tss_init_struct
{
    uint_32     UID;                                /* usage id */
    uint_8      FLAG;                               /* flag */
}HMI_TSS_INIT_STRUCT, _PTR_ HMI_TSS_INIT_STRUCT_PTR;


typedef struct hmi_tss_context_struct
{
    HMI_TSS_INIT_STRUCT_PTR    TSS_TABLE;           /* TSS element table */
    uint_8                     TSS_COUNT;           /* TSS count number */
    uint_8                     CONTROL_NUMBER;      /* control type*/
    HMI_CLIENT_STRUCT_PTR      CLIENT_HANDLE;       /* client handle */
}HMI_TSS_CONTEXT_STRUCT, _PTR_ HMI_TSS_CONTEXT_STRUCT_PTR;

typedef struct hmi_tss_system_control_struct
{
    uint_16        SYSTEM_CONFIG;                   /* TSS system config */
    uint_8         SYSTEM_TRIGGER;                  /* TSS trigger */
    uint_8         NUMBER_OF_SAMPLES;               /* number of samples */
    const uint_8 _PTR_    SENSITIVITY_VALUES;       /* pointer to sensitivity values array */
}HMI_TSS_SYSTEM_CONTROL_STRUCT, _PTR_ HMI_TSS_SYSTEM_CONTROL_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
HMI_PROVIDER_STRUCT_PTR hmi_tss_keypad_provider_init(const HMI_TSS_INIT_STRUCT _PTR_, uint_8);
HMI_PROVIDER_STRUCT_PTR hmi_tss_rotary_provider_init(const HMI_TSS_INIT_STRUCT _PTR_, uint_8);
HMI_PROVIDER_STRUCT_PTR hmi_tss_slider_provider_init(const HMI_TSS_INIT_STRUCT _PTR_, uint_8);
uint_32 hmi_tss_provider_deinit(HMI_PROVIDER_STRUCT_PTR provider);
void hmi_tss_init(const HMI_TSS_SYSTEM_CONTROL_STRUCT system_control);
void hmi_tss_on_fault(uint_8);
void hmi_tss_empty_function(void);
#ifdef __cplusplus
}
#endif

#endif
