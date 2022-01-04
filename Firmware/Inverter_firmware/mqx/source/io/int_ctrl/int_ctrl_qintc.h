/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: int_ctrl_qintc.h$
* $Version : 3.8.5.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the type definitions for the Freescale
*   Qorivva interrupt controller (INTC) driver
*
*END************************************************************************/

#ifndef __int_ctrl_qintc_h__
#define __int_ctrl_qintc_h__


#ifdef __cplusplus
extern "C" {
#endif


extern uint_32 _qintc_install(VQINTC_REG_STRUCT_PTR base, int vector);
extern uint_32 _qintc_set_prio(uint_32 vector, uint_32 prio);
extern uint_32 _qintc_set_sw_interrupt(uint_32 vector);
extern uint_32 _qintc_clear_sw_interrupt(uint_32 vector);

extern VQINTC_REG_STRUCT_PTR _bsp_get_qintc_base_address(void);
extern boolean _bsp_qintc_enable_access(void);

#if PSP_NUM_CORES>1
extern VQINTC_REG_STRUCT_PTR _bsp_get_core_qintc_base_address(uint_32 core_num);
extern uint_32 _qintc_set_core_sw_interrupt(uint_32 core_num, uint_32 vector);
extern uint_32 _qintc_clear_core_sw_interrupt(uint_32 core_num, uint_32 vector);
extern uint_32 _qintc_set_core0_sw_interrupt(uint_32 vector);
extern uint_32 _qintc_set_core1_sw_interrupt(uint_32 vector);
#endif

#ifdef __cplusplus
}
#endif


#endif

