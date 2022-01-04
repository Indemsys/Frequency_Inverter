/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2011 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2011 ARC International;
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
* $FileName: hmi_client.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure
*   definitions of HMI client
*
*END************************************************************************/
#ifndef __hmi_client_h__
#define __hmi_client_h__ 1

#include "psptypes.h"
#include "hmi_provider.h"
#include "mqx.h"

#define HMI_VALUE_ON     0
#define HMI_VALUE_OFF    1

#define HMI_VALUE_RELEASE      0
#define HMI_VALUE_PUSH         1
#define HMI_VALUE_MOVEMENT     2

typedef struct hmi_client_struct
{
    void ( _CODE_PTR_ ON_CHANGE)(pointer, uint_32, uint_32);/* function to call on change */
    HMI_PROVIDER_STRUCT_PTR     _PTR_ PROVIDER_TABLE;       /* array of pointers */
    uint_32                     MAX_PROVIDER_COUNT;         /* max provider number */
    pointer                     CONTEXT_PTR;                /* pointer to context */
} HMI_CLIENT_STRUCT, _PTR_ HMI_CLIENT_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
boolean hmi_add_provider(HMI_CLIENT_STRUCT_PTR, HMI_PROVIDER_STRUCT_PTR);
boolean hmi_remove_provider(HMI_CLIENT_STRUCT_PTR, HMI_PROVIDER_STRUCT_PTR);
HMI_CLIENT_STRUCT_PTR hmi_client_init(void);
#ifdef __cplusplus
}
#endif

#endif

