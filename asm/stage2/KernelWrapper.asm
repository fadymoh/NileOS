%define CORE_FLAG 0xB000 ; An empty memory location to store 
                         ; a flag indicating AP startup status 
                         ; and the AP ID last started up

[BITS 64]
SECTION .text           ; Text section contains machine instructions
                        ; Basically code segment
global _start           ; Define _start label as global to be seen by
                        ; the linker file
_start:                 ; Entry point defined in the linker file
jmp Main64              ; Jump to Main entry point label

Main64:

    cli                           ; Clear the interrupt flag.
                                  ; Disable Interrupts
    mov rsp,_stack_start          ; Move stack start inro stack pointer

    mov rdi,rsp                   ; Set stack_start and stack_end to be passed 
    mov rsi,_stack_end            ; as parameteres to the kernel_main C function

    mov bl,byte [CORE_FLAG]       ; Check the CORE_FLAG location
    cmp bl,0                      ; If it is larger than zero then this is an AP
    jg goto_ap_kernel_main        ; Go to kernel_main of AP


    extern bsp_kernel_main        ; Define an external symbol for the bsp kernel main function
                                  ; function defined in the C source
    call bsp_kernel_main          ; Call Kernel main C function
    
    jmp khalt

goto_ap_kernel_main:

    extern ap_kernel_main         ; Define an external symbol for the bsp kernel main function
                                  ; function defined in the C source
    call ap_kernel_main           ; Call Kernel main C function


khalt:                            ; Halt loop
    hlt
    jmp khalt

SECTION .bss            ; BSS section conatines uninitialized global
                        ; or static variables
_stack_end:             ; Label defining the end of the stack
    resb    8192        ; reserve 8 KB
_stack_start:           ; Label defining the start of the stack
