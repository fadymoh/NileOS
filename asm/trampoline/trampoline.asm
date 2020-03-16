%define CORE_FLAG 0xB000 ; An empty memory location to store 
                         ; a flag indicating AP startup status 
                         ; and the AP ID last started up
ORG 0xA000  ; Set origin to 0xA000 where this code will be loaded
[BITS 16]   ; Impose real mode
xor ax, ax          ; Set AX to zero
mov ss, ax          ; Set Stack Segment to 0
mov ax,my_stack_start ; Set the stack pointer to the temporary stack  
mov sp, ax            ; pointer to by my_stack_start
                      ; Note: The stack grows upwards

; In case we start our cores using boradcast IPI use spin lock

acquireLock:
    lock bts word [spinlock],0        ; Attempt to acquire the lock (in case lock is uncontended)
                                      ; Use atomic operations to insure consistency across cores
    jc .spin_with_pause               ; Jump to spin lock loop if lock is already acquired
    jmp start_core                    ; Else skip the spin lock and move forward to start up

.spin_with_pause:
    pause                    ; Tell CPU we're spinning
    test word [spinlock],1   ; Is the lock free?
    jnz .spin_with_pause     ; no, wait
    jmp acquireLock          ; retry

start_core:                     ; Increment the CORE_FLAG to indicate the number 
    mov bl,byte [CORE_FLAG]     ; of AP cores started so far
    add bl,1
    mov byte [CORE_FLAG],bl

    mov word [spinlock],0       ; Release the spinlock
    jmp 0x0000:0x9000           ; Jump to Kernel Stage 1

halt:                            ; Halt loop
    jmp halt

spinlock dw 0       ; spinlock variable

my_stack_end:          ; Define a 1 KB temprorary stack 
    times 1024 db 0
my_stack_start:

; Pad out file to 4K.
times 4096 - ($-$$) db 0
