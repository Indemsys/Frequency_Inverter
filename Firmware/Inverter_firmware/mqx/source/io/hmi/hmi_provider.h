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
* $FileName: hmi_provider.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure
*   definitions private to hmi provider.
*
*END************************************************************************/
#ifndef __hmi_provider_h__
#define __hmi_provider_h__ 1

#include "psptypes.h"

typedef struct hmi_provider_struct
{
    void        (_CODE_PTR_ POLL_PTR)           (pointer);                          /* poll */
    boolean     (_CODE_PTR_ GET_VALUE_PTR)      (pointer, uint_32, uint_32_ptr);    /* get value */
    void        (_CODE_PTR_ INSTALL_INTERRUPT_PTR)  (pointer);                      /* install INT */
    void        (_CODE_PTR_ UNINSTALL_INTERRUPT_PTR)  (pointer);                    /* uninstall INT */
    void         (_CODE_PTR_ ATTACH_CLIENT) (pointer,pointer);                      /* attach client */
    boolean     (_CODE_PTR_ SET_VALUE_PTR)      (pointer, uint_32, uint_32);        /* set value */
    pointer     CONTEXT_PTR;                                                        /* pointer to context data */
} HMI_PROVIDER_STRUCT, _PTR_ HMI_PROVIDER_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
