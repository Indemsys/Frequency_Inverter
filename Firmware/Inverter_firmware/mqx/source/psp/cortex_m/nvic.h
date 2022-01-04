/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
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
* $FileName: cortexa5.h$
* $Version : 3.8.7.0$
* $Date    : Oct-2-2012$
*
* Comments:
* ARM Nested Vectored Interrupt Controller (NVIC)
*
*END************************************************************************/

#ifndef __nvic_h__
#define __nvic_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <mqx.h>

/* Initialization of interrupt vector in GIC (vector, priority, enable/disable) */
_mqx_uint _nvic_int_init(_mqx_uint irq, _mqx_uint prior, boolean enable);

/* Enable interrupt for given vector */
_mqx_uint _nvic_int_enable(_mqx_uint irq);

/* Disable interrupt for given vector */
_mqx_uint _nvic_int_disable(_mqx_uint irq);

#ifdef __cplusplus
}
#endif

#endif // __nvic_h__
