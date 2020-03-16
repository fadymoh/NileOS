;------------------------------- Start of Staget 1 Kernel Code ------------------------------
%define FREE_SPACE 0x1000
%define CORE_FLAG 0xB000 ; An empty memory location to store 
                         ; a flag indicating AP startup status 
                         ; and the AP ID last started up
ORG 0x9000     ; Set origin to 0x9000 where this will be loaded
[BITS 16]      ; Impose real mode

main_kernel:            ; Main Kernel entry point


    mov bl,byte [CORE_FLAG]
    cmp bl,0
    jg skip_memory_scanner ; Skip if AP, if bsp => continue 


    mov si,welcome_from_kernel  ; Print Welcome message defined below
    call video_print



    xor ax, ax          ; Set AX to zero

    mov ss, ax          ; Set Stack Segment to 0

    mov sp, main_kernel ; Set the stack pointer to start just right below this code
                        ; Note: The stack grows upwards

    mov ds, ax          ; Set DS to zero
    mov es, ax          ; Set ES to zero
    mov fs, ax          ; Set FS to zero
    mov gs, ax          ; Set GS to zero

    cld                 ; Clear Direction Flag

    call CheckA20       ; Check if the A20 Line address is enabled

    cmp byte [a20_enabled],0x01 ; If enabled skip enabling it
    je skip_enable_a20          ; Else call EnableA20
        call EnableA20
    skip_enable_a20:            ; Skipping EnableA20
        call CheckCPU           ; Check If CPU supports Long Mode




    mov edi,memory_info         ; Set EDI register to point to memory location
                                ; memory_info to store mory entries into
    call memory_scanner         ; Call memory_scanner to detect memory entries

skip_memory_scanner:            ; We come here right away if this is an AP
    xor ax,ax
    mov ds,ax
    mov edi, FREE_SPACE
    ; Switch to Long Mode.
    jmp SwitchToLongMode

halt:                   ; Halt the Kernel
    jmp halt            ; Infinite for loop

; Welcome message to be printed upon entry to the Kernel first stage
welcome_from_kernel   db 'Welcome Bare Metal Team From Kernel Land', 13, 10, 0

%include "stack_ops.asm"    ; Inline include for print functions
%include "print.asm"    ; Inline include for print functions
%include "a20.asm"      ; Inline include for A20 Detection and enablement functions
%include "check_cpu.asm"    ; Inline include for checking if CPU supports Long MOde
%include "memory_scanner.asm"    ; Inline code for memory detection function
%include "long_mode_directly.asm"    ; Inline code for switching to Long Mode

;-----------------------------------------------------------------------------------

; Pad out file to 4K.
times 4096 - ($-$$) db 0
