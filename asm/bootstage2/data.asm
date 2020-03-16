;----------Data Declaration Section -----------------------------------------------------;

welcome_msg   db 'Welcome to Bare Metal Team', 13, 10, 0 ; A welcome message
boot_from_msg   db 'Boot from ', 0                       ; A prefix message to be
                                                         ; displayed before the boot
                                                         ; device name
boot_from_floppy_msg   db 'Floppy', 0                    ; A string to be displayed
                                                         ; after the boot_from_msg
                                                         ; if booted from floppy
boot_from_harddisk_msg   db 'HD', 0                      ; A string to be displayed
                                                         ; after the boot_from_msg if
                                                         ; booted from internal drive
boot_from_none_msg   db 'NONE', 0                        ; A string to be displayed
                                                         ; after the boot_from_msg if
                                                         ; failed to boot
done_read_msg   db 'Done Read',13, 10, 0                 ; A message indicating that
                                                         ; reading process is finished
read_error   db 'err',13, 10, 0                          ; An error message indicating
                                                         ; error in read operation
newline   db 13, 10, 0                                   ; New line string
dot   db '.',0                                           ; A dot character to be printed
                                                         ; when new sector is read
boot_drive  db 0

; Boot Device Paraneters loaded by INT 13h function 08 in case of booting from hard drive
; or set manually in case of booting from floppy

heads     dd 0                 ; Number of heads in boot device
cyl       dd 0                 ; Number of cylenders in boot device
sectors   dd 0                 ; Number of sectors/cylender in boot device


; Variables indicating the current read sector on boot device

read_head      dd  0        ; Current Head number
read_cyl       dd  0        ; Current Cylender Number
read_sec       dd  0xa      ; Current Sector Number. We set this to 2 as the BIOS will
                            ; read the first sector automatically to 0x7C00


read_segment    dw 0           ; Memory segment to read current sector to
read_address    dw 0x9000      ; Memory address within segment to read current sector to
                               ; Maximum value of the read_address is 0xFFFF

sectors_to_read dw 16        ; We start by reading 16 sectors first and then we read
                               ; the rest
read_finish     db   0         ; A flag indicating that disk read operation is finished


ramdisk_read_loop dd 0x40
kernel_read_loop dd 0x02
bosml_read_loop dd 0x02

disk_buffer_size dd 0
disk_buffer_src dd 0
disk_buffer_dest dd 0

extended_disk_info db 30


e_dap           db    0x10
e_unused        db    0x0
e_sectors       dw    0x1
e_offset        dw    0x0
e_segment       dw    0x0
e_start_sec1    dd    0x0
e_start_sec     dd    0x0


;-----------------------------------------------------------------------------------------;
