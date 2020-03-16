;---------------------------CheckCPU Routine-----------------------------;
; This routine checks whether CPU supports long mode or not.
; It return normally if CPU supports Long Mode, or halts execution
; otherwise
;-------------------------------------------------------------------------;

CheckCPU:
    push_e_general
    push_e_index
    pushfd              ; Get eflags register in EAX register.
    pop eax             ; And make a copy in ECX for later comparison
    mov ecx, eax        ; in addition to restoring e-flags at the end
                        ; of the routine

                        ; We will need cpuid instruction to check for long mode
                        ; But first we need to check if cpuid instruction is supported
                        ; The ID bit (bit 21) of the flags can only be set if CPU ID
                        ; is supported

    xor eax, 0x200000   ; Flip bit 21
    push eax            ; Push it into the stack
    popfd               ; Pop the same value to the eflags regsiter
                        ; Here if cpuid us supported bit 21 should
                        ; remain as is

    pushfd              ; Push the eflags to the stack again
    pop eax             ; Copy the eflags regsiter to EAX

    xor eax, ecx        ; Flip the values in EAX again
    shr eax, 21         ; Shift right 21 bits
    and eax, 1          ; Check whether bit 21 is set or not. If EAX now contains 0, CPUID isn't supported.
    push ecx            ; retore eflags values by pushing original value of ECX to stack
    popfd               ; and poping it into the eflags

    test eax, eax       ; Test if Eax is equal to zero
    jz .NoLongMode      ; If EAX jump to .NoLongMode to halt

    mov eax, 0x80000000 ; Call cpuid with function 0x80000000
    cpuid

    cmp eax, 0x80000001               ; Check whether extended function 0x80000001 is available are not.
    jb .NoLongMode                    ; If not, long mode not supported, and jump to .NoLongMode

    mov eax, 0x80000001               ; Else call cpuid with extended function 0x80000001
    cpuid

    test edx, 1 << 29                 ; Test if the LM-bit bit 29, is set or not.
    jz .NoLongMode                    ; If not Long mode not supported, and jump to .NoLongMode

    mov si, LongMode                  ; If long mode is supported print a message indicating that
                                      ; Point SI register to LongMode String
    call video_print                  ; Call video_print to print it on the screen
    pop_e_index
    pop_e_general

    ret

.NoLongMode:                            ; If No Long Mode is detected we come here.
    mov si, NoLongMode                  ; Print a Message indicating that Long Mode is not supported
                                        ; Set SI to NoLongMode string
    call video_print                    ; Call video_print to print it on the screen
    jmp halt                            ; Jump to the halt label to suspend execution


NoLongMode db "ERROR: CPU does not support long mode.", 13, 10, 0
LongMode db "CPU supports long mode.", 13,10, 0
