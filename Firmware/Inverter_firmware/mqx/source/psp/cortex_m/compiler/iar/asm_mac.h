/**HEADER*********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2008 Embedded Access Inc.;
* All Rights Reserved
*
******************************************************************************
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
******************************************************************************
*
* $FileName: asm_mac.h$
* $Version : 3.8.6.0$
* $Date    : Oct-3-2012$
*
* Comments:
*   This file contains macros used by the IAR ARM assembler
*
*END*************************************************************************/

#ifndef __asm_mac_h__
#define __asm_mac_h__   1

#define ASM_PREFIX(x) x

#define ASM_LABEL(value)      value
#define ASM_EXTERN(value)     EXTERN  value
#define ASM_ALIGN(value)      ALIGNROM  value/4
#define ASM_PUBLIC(label)     PUBLIC label
#define ASM_SET(label, value) ASM_LABEL(label) SET value
#define ASM_CONST16(value)    DC16 value
#define ASM_CONST32(value)    DC32 value
#define ASM_LABEL_CONST32(label, value) ASM_LABEL(label) DC32 value
#define ASM_DATA_SECTION(label)    SECTION label : DATA (4)
#define ASM_CODE_SECTION(label)    SECTION label : CODE (4)  
#define ASM_CODE_SECTION_NOOPT(label)    SECTION label : CODE (4) : ROOT  
#define ASM_END               END
#define ASM_COMP_SPECIFIC_DIRECTIVES
#define ASM_EQUATE(label, value) label   EQU  value

/* CFI annotations for public symbols called from C code */

  CFI Names CFINames0
  CFI StackFrame CFA R13 DATA
  CFI Resource R0:32, R1:32, R2:32, R3:32, R4:32, R5:32, R6:32, R7:32
  CFI Resource R8:32, R9:32, R10:32, R11:32, R12:32, R13:32, R14:32
  CFI EndNames         CFINames0

  CFI Common CFICommon0 Using         CFINames0
  CFI CodeAlign 2
  CFI DataAlign 4
  CFI ReturnAddress R14 CODE
  CFI CFA R13+0
  CFI R0 SameValue
  CFI R1 SameValue
  CFI R2 SameValue
  CFI R3 SameValue
  CFI R4 SameValue
  CFI R5 SameValue
  CFI R6 SameValue
  CFI R7 SameValue
  CFI R8 SameValue
  CFI R9 SameValue
  CFI R10 SameValue
  CFI R11 SameValue
  CFI R12 SameValue
  CFI R14 SameValue
  CFI EndCommon CFICommon0

/* Note that these macros should NOT be on the beggining of line when used
   in assembler code. Prepend allways by at least one space. 
   (was not an issue in EWARM 6.40.x, space seems to be needed in 6.50.x) */
#define ASM_PUBLIC_BEGIN(name) CFI Block CFIBlock_##name Using CFICommon0
#define ASM_PUBLIC_FUNC(name)  CFI Function name
#define ASM_PUBLIC_END(name)   CFI EndBlock CFIBlock_##name

#endif /* __asm_mac_h__ */
