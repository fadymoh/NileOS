%define PAGE_PRESENT    (1 << 0)    ; Used to set first bit in PTE Atrributes (Present Bit)
%define PAGE_WRITE      (1 << 1)    ; Used to set second bit of the PTE Attributes (R/W Bit)

%define CODE_SEG     0x0008         ; Code segment selector in GDT
%define DATA_SEG     0x0010         ; Data segment selector in IDT

ALIGN 4                             ; Align with 4 bytes to make sure the the IDT is at aligned address
IDT:                                ; IDT Labl indicating a NULL IDT table to load through lidt later
    .Length       dw 0              ; Table size is zero
    .Base         dd 0              ; IDT base address is 0 or NULL

; Function to switch directly to long mode from real mode.
; Identity maps the first 2MiB.
; Uses Intel syntax.

; es:edi    Should point to a valid page-aligned 16KiB buffer, for the PML4, PDPT, PD and a PT.
; ss:esp    Should point to memory that can be used as a small (1 dword ) stack

SwitchToLongMode:
    ; Zero out the 16KiB buffer.
    ; Since we are doing a rep stosd, count should be bytes/4.

    mov bl,byte [CORE_FLAG]
    cmp bl,0
    jg skip_page_table                ; If this is an AP skip page table setup

    push edi                           ; REP STOSD alters DI.
    mov ecx, 0x1000                    ; set rep counter to 4096
    xor eax, eax                       ; Zero out eax
    cld                                ; Clear direction flag
    rep stosd                          ; Store EAX at address ES:EDI
    pop edi                            ; Get DI back.


    ; Build the Page Map Level 4.
    ; es:di points to the Page Map Level 4 table.
    ; lea mean load effect address
    lea eax, [es:di + 0x1000]         ; Put the address of the Page Directory Pointer Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writable flag.
    mov [es:di], eax                  ; Store the value of EAX as the first PML4E.


    ; Build the Page Directory Pointer Table.
    lea eax, [es:di + 0x2000]         ; Put the address of the Page Directory in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writable flag.
    mov [es:di + 0x1000], eax         ; Store the value of EAX as the first PDPTE.


    ; Build the Page Directory.
    lea eax, [es:di + 0x3000]         ; Put the address of the Page Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writeable flag.
    mov [es:di + 0x2000], eax         ; Store to value of EAX as the first PDE.


    lea eax, [es:di + 0x4000]         ; Put the address of the Page Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writeable flag.
    mov [es:di + 0x2008], eax         ; Store to value of EAX as the first PDE.


    push di                           ; Save DI for the time being.
    lea di, [di + 0x3000]             ; Point DI to the page table.
    mov eax, PAGE_PRESENT | PAGE_WRITE    ; Move the flags into EAX - and point it to 0x0000.

    ; Build the Page Table.
.LoopPageTable:
    mov [es:di], eax
    add eax, 0x1000
    add di, 8
    cmp eax, 0x200000                 ; If we did all 2MiB, end.
    jb .LoopPageTable                 ; Jump if below

    pop di                            ; Restore DI.

    ; Disable IRQs
    mov al, 0xFF                      ; Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
    out 0xA1, al                      ; Disable PIC Slave
    out 0x21, al                      ; Disable PIC Master

    nop
    nop

skip_page_table:                      ; If this is an AP skip page table setup
    lidt [IDT]                        ; Load a zero length IDT so that any NMI causes a triple fault.

    ; Enter long mode.
    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax

    mov edx, edi                      ; Point CR3 at the PML4.
    mov cr3, edx


    mov ecx, 0xC0000080               ; Read from the EFER (Extended Feature Enable Register) MSR. (Model Specific Register)
    rdmsr

    or eax, 0x00000100                ; Set the LME bit. (Long Mode Enabled BIT # 8)
    wrmsr

    mov ebx, cr0                      ; Activate long mode -
    or ebx,0x80000001                 ; - by enabling paging and protection simultaneously.
                                      ; Bit 0 to set protected mode
                                      ; Bit 31 for enabling Paging
    mov cr0, ebx
    
    
; enabling SSE    
;mov eax, cr0
;and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
;or ax, 0x2			;set coprocessor monitoring  CR0.MP
;mov cr0, eax
;mov eax, cr4
;or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
;mov cr4, eax
    


mov eax, 0x1
cpuid
test edx, 1<<25
jz ttttt

mov eax, cr0
and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
or ax, 0x2			;set coprocessor monitoring  CR0.MP
mov cr0, eax
mov eax, cr4
or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
mov cr4, eax

ttttt:
    lgdt [GDT64.Pointer]                ; Load GDT.Pointer defined below.


    jmp CODE_SEG:LongModeEntry             ; Load CS with 64 bit segment and flush the instruction cache


    ; Global Descriptor Table
GDT64:
    .Null: equ $ - GDT64         ; The null descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).

    .Code: equ $ - GDT64         ; The Kernel code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011000b                 ; Access.
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).

    .Data: equ $ - GDT64         ; The Kernel data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010011b                 ; Access.
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).

    .UCode: equ $ - GDT64         ; The User code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11111000b                 ; Access.
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).

    .UData: equ $ - GDT64         ; The User data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11110011b                 ; Access.
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).

ALIGN 4
    dw 0                              ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary

.Pointer:
    dw $ - GDT64 - 1                    ; 16-bit Size (Limit) of GDT.
    dd GDT64                            ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)


[BITS 64]

LongModeEntry:

    mov ax, DATA_SEG ; Set data segment to GDT Data Segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    jmp 0x100000      ; Jump to the C Kernel Land

myhang:             ; Halt Loop
    jmp myhang
