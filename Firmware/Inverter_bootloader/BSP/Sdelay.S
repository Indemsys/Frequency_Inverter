         SECTION .text : CODE (4)  
         PUBLIC Delay_m8

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


        ALIGNROM  1
        END
