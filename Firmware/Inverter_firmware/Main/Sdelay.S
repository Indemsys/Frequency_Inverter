#include "asm_mac.h"
#include "mqx_cnfg.h"
#include "types.inc"
#include "psp_prv.inc"

#define __ASM__

#include <psp_cpu.h>

#undef __ASM__


 SECTION .text : CODE (4)  
 SET_FUNCTION_ALIGNMENT

  PUBLIC Delay_m8


 CFI Block CFIBlock_Delay_m8 Using CFICommon0
 CFI Function Delay_m8

         ;  (R0+1)*8
Delay_m8
         SUBS     r0,r0,#1   ; 1
         NOP                 ; 1
         NOP                 ; 1
         NOP                 ; 1
         CMP      r0,#0x00   ; 1
         BGT      Delay_m8   ; 3/1
         NOP                 ; 1
         NOP                 ; 1

         NOP                 ; 1
         NOP                 ; 1
         BX       lr         ; 3



 CFI EndBlock CFIBlock_Delay_m8

        ALIGNROM  1
        END
