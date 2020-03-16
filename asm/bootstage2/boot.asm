;-------------------------------- Boot Sector - Stage 2------------------------
%define CORE_FLAG 0xB000 ; An empty memory location to store 
                         ; a flag indicating AP startup status 
                         ; and the AP ID last started up

[ORG 0x8000]   ; Set load address to 0x8000

xor ax, ax  ; Make it zero
mov ds, ax  ; Set data segment to zero

jmp load_kernel   ; jump to load kernel

%include "print.asm"    ; Inline include of of Print Functions
%include "disk.asm"     ; Inline include of Disk Functions
%include "stack_ops.asm"; Inline include of Disk Functions
%include "a20.asm"      ; Inline include of Disk Functions

read_sectors_batch:
    disk_read_loop:             ; Read Sectors loop
        mov ax,[sectors_to_read]; Check if finished reading
        cmp ax,0x0
        je disk_read_loop_done  ; If so exit loop
        call read_sector        ; Else read current sector
                                ; and increment disk and memory
    jmp disk_read_loop          ; Loop
    disk_read_loop_done:        ; Exit loop label
ret

copy_disk_buffer:
    ; Set copy parameters
    ;mov ecx,0xA600              ; size of the code, 512 bytes (0x200)
    mov ecx,[disk_buffer_size]              ; size of the code, 512 bytes (0x200)
    mov eax,[disk_buffer_src]            ; note 32 bit offset
    mov ebx,[disk_buffer_dest]           ; note 32 bit offset

  copy_loop:
    mov dl,byte [ds:eax]
    mov byte [ds:ebx],dl
    add eax,1
    add ebx,1
    sub ecx,1
    cmp ecx,0
    jne copy_loop

ret


load_kernel:
        call bios_cls           ; Clear screen
        mov si, welcome_msg     ; Print welcome message
        call bios_print

        call EnableA20

        ; Setting up Unreal mode
        cli                    ; no interrupts
        push ds                ; save real mode

        lgdt [gdtinfo]         ; load gdt register

        mov  eax, cr0          ; switch to pmode by
        or al,1                ; set pmode bit
        mov  cr0, eax

        jmp $+2                ; tell 386/486 to not crash

        mov  bx, 0x08          ; select descriptor 1
        mov  ds, bx            ; 8h = 1000b

        and al,0xFE            ; back to realmode
        mov  cr0, eax          ; by toggling bit again

        pop ds                 ; get back old segment
        sti



     	call get_boot_drive     ; Identify the boot device and save
                                ; its number in variable boot_drive
; We start reading with preset values of number of sector 0x8,
; and memory addres to read data to is 0x9000

    ; Read initial Batch of sectors that represents kernel Stage1
    call read_sectors_batch


    ; Read User Mode Program (bosml.bin)
;    mov [read_finish],al        ; after next iteration
;    mov ax,0x320                 ; Set number to sectors to read to 0x53
;    mov [sectors_to_read],ax
;    mov ax,0x1000               ; set read memory segment 0x1000
;    mov [read_segment],ax
;    mov ax,0x0                  ; Set memory address to zero
;    mov [read_address],ax       ; Effective address 0x1000:0000 = 0x10000
;    call read_sectors_batch     ; Read Sectors

;    mov eax,0x64000
;    mov [disk_buffer_size],eax
;    mov eax,0x10000
;    mov [disk_buffer_src],eax
;    mov eax,0x100000
;    mov [disk_buffer_dest],eax

;    call copy_disk_buffer

; Read RAM Disk
    ;mov eax,0x300000  // old 
    mov eax,0x4200000
    mov [disk_buffer_dest],eax

read_ram_disk:
    mov [read_finish],al        ; after next iteration

    mov ax,0x320                 ; Set number to sectors to read to 0x320
    mov [sectors_to_read],ax
    mov ax,0x1000               ; set read memory segment 0x1000
    mov [read_segment],ax
    mov ax,0x0                  ; Set memory address to zero
    mov [read_address],ax       ; Effective address 0x1000:0000 = 0x10000
    call read_sectors_batch     ; Read Sectors

    mov eax,0x64000
    mov [disk_buffer_size],eax
    mov eax,0x10000
    mov [disk_buffer_src],eax
    call copy_disk_buffer

    mov eax,[disk_buffer_dest]
    add eax,0x64000
    mov [disk_buffer_dest],eax

    mov eax,[ramdisk_read_loop]
    sub eax,1
    mov [ramdisk_read_loop],eax
    cmp eax,0
    jne read_ram_disk

    ; Read my main Kernel
    
    mov eax,0x100000
    mov [disk_buffer_dest],eax

read_kernel_disk:
    mov [read_finish],al        ; after next iteration

    mov ax,0x320                 ; Set number to sectors to read to 0x320
    mov [sectors_to_read],ax
    mov ax,0x1000               ; set read memory segment 0x1000
    mov [read_segment],ax
    mov ax,0x0                  ; Set memory address to zero
    mov [read_address],ax       ; Effective address 0x1000:0000 = 0x10000
    call read_sectors_batch     ; Read Sectors

    mov eax,0x64000
    mov [disk_buffer_size],eax
    mov eax,0x10000
    mov [disk_buffer_src],eax
    call copy_disk_buffer

    mov eax,[disk_buffer_dest]
    add eax,0x64000
    mov [disk_buffer_dest],eax

    mov eax,[kernel_read_loop]
    sub eax,1
    mov [kernel_read_loop],eax
    cmp eax,0
    jne read_kernel_disk



    ; Read my main Kernel
    
    mov eax,0x200000
    mov [disk_buffer_dest],eax

read_bosml_disk:
    mov [read_finish],al        ; after next iteration

    mov ax,0x320                 ; Set number to sectors to read to 0x320
    mov [sectors_to_read],ax
    mov ax,0x1000               ; set read memory segment 0x1000
    mov [read_segment],ax
    mov ax,0x0                  ; Set memory address to zero
    mov [read_address],ax       ; Effective address 0x1000:0000 = 0x10000
    call read_sectors_batch     ; Read Sectors

    mov eax,0x64000
    mov [disk_buffer_size],eax
    mov eax,0x10000
    mov [disk_buffer_src],eax
    call copy_disk_buffer

    mov eax,[disk_buffer_dest]
    add eax,0x64000
    mov [disk_buffer_dest],eax

    mov eax,[bosml_read_loop]
    sub eax,1
    mov [bosml_read_loop],eax
    cmp eax,0
    jne read_bosml_disk


finished_load:                  ; Finished reading sectors
    mov si,newline              ; Print new line
    call bios_print
    mov si,done_read_msg        ; Print completion message
    call bios_print

    mov bl,0                    ; Clear out the CORE_FLAG to indicate that
    mov byte [CORE_FLAG],bl     ; no APs are started so far

    jmp 0x9000                  ; Jump to where we loaded the kernel,
                                ; offset kernel_segments
halt:                           ; Halt loop
        jmp halt

%include "data.asm"

gdtinfo:
   dw gdt_end - gdt - 1   ;last byte in table
   dd gdt                 ;start of table

gdt         dd 0,0        ; entry 0 is always unused
flatdesc    db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:


times 4096-($-$$) db 0 ; Pad to 510 bytes

