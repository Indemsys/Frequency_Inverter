/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: sema4.h$
* $Version : 3.8.2.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*  This file contains the type definitions for the MPXxxx SEMA4 module.
*
*END************************************************************************/

#ifndef __sema4_h__
#define __sema4_h__

#define __sema4_h__version "$Version:3.8.2.0$"
#define __sema4_h__date    "$Date:Sep-24-2012$"


#define SEMA4_NUM_GATES 16

#define SEMA4_UNLOCK    0

/* PPC dont have SEMA4 register structure */
#if PSP_MQX_CPU_IS_PPC
typedef volatile struct sema4_reg_struct
{
   uchar    GATE[SEMA4_NUM_GATES];
   RESERVED_REGISTER(0x10,0x40);
   uint_16  CP0INE;
   RESERVED_REGISTER(0x42,0x48);
   uint_16  CP1INE;
   RESERVED_REGISTER(0x4a,0x80);
   uint_16  CP0NTF;
   RESERVED_REGISTER(0x82,0x88);
   uint_16  CP1NTF;
   RESERVED_REGISTER(0x8a,0x100);
   uint_16  RSTGT;
   RESERVED_REGISTER(0x102,0x104);
   uint_16  RSTNTF;
   RESERVED_REGISTER(0x106,0x400);
} SEMA4_REG_STRUCT;

typedef volatile struct sema4_reg_struct *SEMA4_MemMapPtr;
#endif

#endif
