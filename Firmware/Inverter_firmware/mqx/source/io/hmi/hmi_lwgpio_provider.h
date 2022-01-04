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
* $FileName: hmi_lwgpio_provider.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This header file is for HMI lwgpio provider. It contains  defines,
*   structures.
*
*END************************************************************************/
#ifndef __hmi_lwgpio_provider_h__
#define __hmi_lwgpio_provider_h__ 1

#include "hmi_provider.h"
#include "hmi_client.h"
#include "psptypes.h"
#include "mqx.h"
#include "bsp.h"

/* FLAG DEFINES */
#define HMI_LWGPIO_FLAG_DIRECTION_IN       (0x00000000)
#define HMI_LWGPIO_FLAG_DIRECTION_OUT      (0x00000001)
#define HMI_LWGPIO_FLAG_ACTIVE_STATE_0     (0x00000000)
#define HMI_LWGPIO_FLAG_ACTIVE_STATE_1     (0x00000002)
#define HMI_LWGPIO_FLAG_PULL_UP_OFF        (0x00000000)
#define HMI_LWGPIO_FLAG_PULL_UP_ON         (0x00000004)
#define HMI_LWGPIO_FLAG_PULL_DOWN_OFF      (0x00000000)
#define HMI_LWGPIO_FLAG_PULL_DOWN_ON       (0x00000008)
#define HMI_LWGPIO_FLAG_INT_OFF            (0x00000000)
#define HMI_LWGPIO_FLAG_INT_ON             (0x00000010)

/* GET FLAGS */
#define HMI_LWGPIO_GET_DIRECTION(x)        ((x)&0x1)
#define HMI_LWGPIO_GET_ACTIVE_STATE(x)     (((x)>>1)&0x1)
#define HMI_LWGPIO_GET_PULL_UP(x)          (((x)>>2)&0x1)
#define HMI_LWGPIO_GET_PULL_DOWN(x)        (((x)>>3)&0x1)
#define HMI_LWGPIO_GET_INT(x)              (((x)>>4)&0x1)

#define LWGPIO_PIN_FROM_ID(id)  (((id) & LWGPIO_PIN_MASK) >> LWGPIO_PIN_SHIFT)
#define LWGPIO_PORT_FROM_ID(id) (((id) & LWGPIO_PORT_MASK) >> LWGPIO_PORT_SHIFT)

/* LWGPIO INITIALIZATION STRUCTURE */
typedef struct hmi_lwgpio_init_struct
{
    uint_32 UID;                /* usage id */
    uint_32 PID;                /* pin id */
    uint_32 FUNC;               /* functionality of the pin */
    uint_32 FLAG;               /* polarity of the pin */
}HMI_LWGPIO_INIT_STRUCT, _PTR_ HMI_LWGPIO_INIT_STRUCT_PTR;

/* LWGPIO STRUCTURE */
typedef struct hmi_lwgpio_struct
{
    LWGPIO_VALUE            LAST_STATE;         /* last state */
    LWGPIO_STRUCT           HANDLE;             /* handle of LWGPIO */
    HMI_LWGPIO_INIT_STRUCT  INIT_STRUCT;        /* initialization struct */
    INT_ISR_FPTR            INIT_ISR;           /* pointer to ISR */
}HMI_LWGPIO_STRUCT, _PTR_ HMI_LWGPIO_STRUCT_PTR;

/* LWGPIO CONTEXT STRUCTURE */
typedef struct hmi_lwgpio_context_struct
{
    HMI_LWGPIO_STRUCT_PTR   BTNLED_TABLE;       /* btn led table */
    uint_32                 BTNLED_COUNT;       /* number of LWGPIO */
    HMI_CLIENT_STRUCT_PTR   CLIENT_HANDLE;
}HMI_LWGPIO_CONTEXT_STRUCT, _PTR_ HMI_LWGPIO_CONTEXT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif
HMI_PROVIDER_STRUCT_PTR hmi_lwgpio_provider_init(const HMI_LWGPIO_INIT_STRUCT _PTR_);
uint_32 hmi_lwgpio_provider_deinit(HMI_PROVIDER_STRUCT_PTR);
#ifdef __cplusplus
}
#endif

#endif
