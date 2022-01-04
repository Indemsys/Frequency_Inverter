/**HEADER********************************************************************
*
* Copyright (c) 2008-2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
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
* $FileName: timer_qpit.h$
* $Version : 3.8.9.0$
* $Date    : Aug-9-2012$
*
* Comments:
*
*    This file contains definitions for the Qoriwa PIT Driver
*
*END***********************************************************************/

#ifndef __timer_qpit_h__
#define __timer_qpit_h__

#include "qpit.h"

#ifdef __cplusplus
extern "C" {
#endif


extern VQPIT_REG_STRUCT_PTR _bsp_get_qpit_base_address(uint_32 device);
extern PSP_INTERRUPT_TABLE_INDEX _bsp_get_qpit_vector(uint_32  device, uint_32 channel);
extern boolean _bsp_qpit_enable_access(uint_32 device);
extern void _bsp_qpit_clk_en (uint_32 timer);

extern uint_32 _qpit_timer_install(uint_32 device, uint_32 channel, uint_32 tickfreq, uint_32 clk, uint_32 priority, void (_CODE_PTR_ isr_ptr)(void *));
extern uint_32 _qpit_timer_install_kernel(uint_32 device, uint_32 channel, uint_32 tickfreq, uint_32 clk, uint_32 priority);
extern uint_32 _qpit_timer_stop(uint_32 device, uint_32 channel);


uint_32 _qpit_init_freq(uint_32, uint_32, uint_32, uint_32, boolean);
void _qpit_clear_int(uint_32, uint_32);
void _qpit_unmask_int(uint_32, uint_32);
void _qpit_mask_int(uint_32, uint_32);

#ifdef __cplusplus
}
#endif


#endif
