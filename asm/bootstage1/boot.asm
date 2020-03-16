;-------------------------------- Boot Sector ------------------------

[ORG 0x7c00]   ; Set load address to 0x7c00
    cli
    xor ax, ax      ; Make it zero
    mov ds, ax      ; Set data segment to zero
    mov ss, ax      ; Set data segment to zero
    mov sp, 0x7c00  ; Very important setting stack pointer
                    ; to point to the beginning of the boot
                    ; sector. The stack grows upward and in 
                    ; our case we will only need a 2 double word
                    ; as we will not be calling more than one nested
                    ; subroutine until we reach stage1 and change the 
                    ; stack. to point to an approporiate reserved space
                

    mov ah, 0x0                    ; RESET-command
    int 13h                        ; Call interrupt 13h

    mov [boot_drive],dl     ; Save boot drive

    cmp dl,0
    je boot_from_floppy
    jmp boot_from_disk

boot_from_floppy:           ; Print message indicating booting from floppy
    mov si,boot_from_floppy_msg
    call bios_print
    jmp load_next_sector

boot_from_disk:             ; Print message indicating booting from disk
	mov si,boot_from_disk_msg
	call bios_print

init_disk:              ; Only in case booting from disk
    mov ah,0x9          ; Initialize Fixed Disk Table
    mov dl,[boot_drive] ; Restore Boot Drive
    int 0x13

load_next_sector:       ; Load one sector from disk to next memory aligned 4 KiB
    mov ax, 0x0         ; Load read segment into ES
    mov es, ax
    mov bx, 0x8000      ; Destination address
    mov dl,[boot_drive] ; Restore Boot Drive
    mov ah, 0x02        ; READ SECTOR-command
    mov al, 0x8         ; Number of sectors to read (1 Sector)
    mov ch, 0x0         ; Cylinder
    mov cl, 0x2         ; Starting Sector
    mov dh, 0x0         ; Head
    int 0x13            ; Call interrupt 13h

    jnc cont            ; On Disk operation error

    mov si,error_msg    ; Print an error message
    call bios_print     ; And error code.
    mov si,hexa_string
    call hexa2ascii
    mov si,hexa_string
    call bios_print

    jmp hlt             ; Jump to htl to halt and pause execution

cont:
   jmp 0x8000   ; jump to load kernel
hlt:
    jmp hlt

%include "print.asm"    ; Inline include of of Print Functions
%include "hexa2ascii.asm"    ; Inline include of of Print Functions

error_msg db  "Error in read sector (Code): ",0         ; A message for Disk Errors
boot_from_disk_msg db  "Booting From Disk ",13,10,0     ; Message Indicating booting from Disk
boot_from_floppy_msg db  "Booting from Floppy ",13,10,0 ; Message Indicating booting from Floppy
newline db 13,10,0          ; A String for ptinting a new line
hexa_string  times 10 db 0  ; A buffer for storing hexa decimal numbers converted to strings
boot_drive  db 0            ; Stores the ID of the boot drive. Either 0x0 ot 0x80

times 446-($-$$) db 0   ; Padding to make the MBR 512 bytes and account
                        ; to partition table as well.

; Hardcoded partition entries
part_boot:
    db 0x80 ; Bootable and Active
    db 0x00 ; Starting Heads
    db 0x01 ; Starting Sector
    db 0x00 ; Starting Cylender
    db 0x83 ; Linux Partition
    db 0x80 ; 128 Heads
    db 10111111b ; 63 sectors
    db 00000000b ; 1024 Cylendars
    dd 0x01 ; Starting Sector LBA
    dd 0xF4200 ; Number of sectors in partition
part_sda2:
    dw 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
part_sda3:
    dw 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
part_sda4:
    dw 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000

db 0x55         ; Boot Seactor signature
db 0xAA

