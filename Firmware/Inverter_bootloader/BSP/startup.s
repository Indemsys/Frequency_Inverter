/*
 * File:    crt0.s
 * Purpose: Lowest level routines for Kinetis.
 *
 * Notes:   
 *
 */

;         AREA   Crt0, CODE, READONLY      ; name this block of code



    SECTION .noinit : CODE
    EXPORT  __startup
    import  __iar_program_start

__startup

    MOV     r0,#0                   ; Initialize the GPRs
    MOV     r1,#0
    MOV     r2,#0
    MOV     r3,#0
    MOV     r4,#0
    MOV     r5,#0
    MOV     r6,#0
    MOV     r7,#0
    MOV     r8,#0
    MOV     r9,#0
    MOV     r10,#0
    MOV     r11,#0
    MOV     r12,#0
//    CPSIE   i                       ; Unmask interrupts



    /* disable interrupts and clear pending flags */
    ldr r0, =0xe000e180     /* NVIC_ICER0 - Interrupt Clear-Enable Registers */
    ldr r1, =0xe000e280     /* NVIC_ICPR0 - Interrupt Clear-Pending Registers */
    ldr r2, =0xffffffff
    mov r3, #8

_boot_loop
    cbz r3, _boot_loop_end
    str r2, [r0], #4        /* NVIC_ICERx - clear enable IRQ register */
    str r2, [r1], #4        /* NVIC_ICPRx - clear pending IRQ register */
    sub r3, r3, #1
    b _boot_loop
_boot_loop_end

/*
    Нужно только для RTOS MQX
    // prepare process stack pointer
    mrs r0, MSP
    msr PSP, r0

    // switch to proccess stack (PSP) 
    mrs r0, CONTROL
    orr r0, r0, #2
    msr CONTROL, r0
    isb #15          // Instruction Synchronization Barrier.
*/

/*
   Float-point сопроцессор будет включен в инициализационном блоке IAR
// CPACR is located at address 0xE000ED88 
    LDR.W   R0, =0xE000ED88
    // Read CPACR 
    LDR     R1, [R0]
    // Set bits 20-23 to enable CP10 and CP11 coprocessors 
    ORR     R1, R1, #(0xF << 20)
    // Write back the modified value to the CPACR 
    STR     R1, [R0]
*/

/*
    Выключение сохранения в стеке состояния FP нужно только для RTOS MQX
    // turn off fpu register stacking in exception entry
    ldr r0, =0xE000EF34     // FPCCR 
    mov r1, #0x00000000
    str r1, [r0]
*/

    BL      __iar_program_start                  ; call the C code
__done
    B       __done


    END
