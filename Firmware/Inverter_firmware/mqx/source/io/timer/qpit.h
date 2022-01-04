/**HEADER********************************************************************
*
* Copyright (c) 2008-2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
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
* $FileName: qpit.h$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*    This file contains definitions for the Qoriwa PIT registers
*
*END***********************************************************************/

#ifndef __qpit_h__
#define __qpit_h__


#define QPIT_NUM_TIMERS 4

#define QPIT_MCR_MDIS      (1<<1)
#define QPIT_MCR_FRZ       (1<<0)

#define QPIT_TCTRL_TIE     (1<<1)
#define QPIT_TCTRL_TEN     (1<<0)

#define QPIT_TFLG_TIF      (1<<0)


typedef struct qpit_timer_reg_struct {
   uint_32  LDVAL;
   uint_32  CVAL;
   uint_32  TCTRL;
   uint_32  TFLG;
} QPIT_TIMER_REG_STRUCT, * QPIT_TIMER_REG_STRUCT_PTR;

typedef volatile struct qpit_timer_reg_struct * VQPIT_TIMER_REG_STRUCT_PTR;

typedef struct qpit_reg_struct {
   uint_32                 MCR;
   RESERVED_REGISTER(0x0004,0x0100);
   QPIT_TIMER_REG_STRUCT   TIMERS[QPIT_NUM_TIMERS];
   RESERVED_REGISTER(0x0140,0x4000);
} QPIT_REG_STRUCT, * QPIT_REG_STRUCT_PTR;

typedef volatile struct qpit_reg_struct * VQPIT_REG_STRUCT_PTR;


#endif

