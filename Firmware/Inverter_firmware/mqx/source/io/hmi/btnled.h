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
* $FileName: btnled.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure
*   definitions for HMI btn_led driver
*
*END************************************************************************/
#ifndef __btnled_h__
#define __btnled_h__ 1

#include "psptypes.h"
#include "mqx.h"
#include "bsp.h"
#include "hmi_client.h"


typedef struct btnled_clbreg
{
    uint_32 STATE;                              /* callback register state */
    uint_32 UID;                                /* UID */
    pointer CALLBACK_PARAMETER;                 /* callback parameter */
    void(_CODE_PTR_ CLBFUNCTION) (pointer);     /* callback function */
    struct btnled_clbreg _PTR_ NEXT_CALLBACK;   /* next callback */
}BTNLED_CLBREG_STRUCT, _PTR_ BTNLED_CLBREG_STRUCT_PTR;

typedef struct btnled_context
{
    BTNLED_CLBREG_STRUCT_PTR    CLBREG;             /* pointer to callback register structure */
    LWSEM_STRUCT                LOCK_BTNLED_SEM;    /* semaphore structure */
}BTNLED_CONTEXT_STRUCT, _PTR_ BTNLED_CONTEXT_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
HMI_CLIENT_STRUCT_PTR btnled_init(void);
void btnled_poll(HMI_CLIENT_STRUCT_PTR);
boolean btnled_get_value(HMI_CLIENT_STRUCT_PTR, uint_32, uint_32_ptr);
boolean btnled_set_value(HMI_CLIENT_STRUCT_PTR, uint_32, uint_32);
boolean btnled_toogle(HMI_CLIENT_STRUCT_PTR, uint_32);
BTNLED_CLBREG_STRUCT_PTR btnled_add_clb(HMI_CLIENT_STRUCT_PTR, uint_32, uint_32, void(CODE_PTR)(pointer), pointer);
boolean btnled_remove_clb(HMI_CLIENT_STRUCT_PTR, BTNLED_CLBREG_STRUCT_PTR);
uint_32 btnled_deinit(HMI_CLIENT_STRUCT_PTR);
#ifdef __cplusplus
}
#endif
#endif

