;----------Load Boot Drive Parameters Routine-----------------------------;
; Retreive the parameters of the first internal disk drive and load it
; into memory variables
;-------------------------------------------------------------------------;

load_boot_drive_params:
    mov ah,0x8                      ; Get Drive Parameters
    mov dl,0x80                     ; First Internal Hard Drive
    int 0x13

    add dh,0x1                      ; DH has the number of heads-1
    mov [heads],dh                  ; Store number of heads into memory

    mov ax,cx                       ; The 10 left most bits of CX has the
    shr ax,0x6                      ; number of Cylenders-1
    add ax,0x1
    mov [cyl],al                    ; Store number of cylenders into memory

    mov ax,cx                       ; The 6 right most bits of CX has the
    and ax,0000000000111111b        ; number of sectors per cylender - 1
    add ax,0x1
    mov [sectors],al                ; Store number of sector to memory
ret
;-------------------------------------------------------------------------;


load_boot_drive_params1:
    mov ah,0x48                      ; Get Drive Parameters
    mov dl,0x80                     ; First Internal Hard Drive
    mov si,extended_disk_info
    int 0x13

    mov edx,[extended_disk_info+4]                      ; DH has the number of heads-1
    mov [cyl],edx                  ; Store number of heads into memory
    mov edx,[extended_disk_info+8]                      ; DH has the number of heads-1
    mov [heads],edx                  ; Store number of heads into memory
    mov edx,[extended_disk_info+12]                      ; DH has the number of heads-1
    mov [sectors],edx                  ; Store number of heads into memory

ret


;----------Load Floppy Parameters Routine-----------------------------;
; Sets Parameters for floppy disk into memory variables.
; We assume a 1.44 MB Floppy Disk.
;-------------------------------------------------------------------------;

set_floppy_params:
    mov ax,0x2
    mov [heads],al   ; 2 Heads
    mov ax,0x50
    mov [cyl],ax     ; 80 cylenders
    mov ax,0x13
    mov [sectors],al ; 18 sectors, we add 1 t the number os sectors
                     ; as INT 13h read function is not zero based
ret
;-------------------------------------------------------------------------;


;----------Get Boot Drive Routine-----------------------------------------;
; Determines the boot drive, and load the parameters accordingly
;-------------------------------------------------------------------------;
get_boot_drive:
        mov si,boot_from_msg
        call bios_print                ; Prints "Boot From" String
        mov ah, 0x0                    ; RESET-command
        int 13h                        ; Call interrupt 13h
        mov [boot_drive], dl           ; Store boot disk
        cmp dl,0                       ; If drive number in DL is 0 then we
        je floppy_boot                 ; booted from floppy, then jump to
                                       ; read floppy params
        cmp dl,0x80                    ; If drive number in DL is not 80h
        jne get_boot_drive_error       ; then jump to error
        call load_boot_drive_params    ; We booted from hard drive then
                                       ; call get_boot_drive_params
        mov si,boot_from_harddisk_msg  ; Print rest of the boot message
        jmp get_boot_drive_done        ; Exit

    floppy_boot:                       ; We have booted from floppy
        call set_floppy_params         ; Load floppy params
        mov si,boot_from_floppy_msg    ; Print rest of boot message
        jmp get_boot_drive_done        ; Exit

    get_boot_drive_error:              ; If we are here then there is an error
        mov si,boot_from_none_msg      ; Print error and halt
        boot_drv_halt:
            jmp boot_drv_halt

    get_boot_drive_done:               ; We are done successfully.
        call bios_print                ; print new line and exit
        mov si,newline
        call bios_print
ret
;-------------------------------------------------------------------------;


;----------Read Disk Sector Routine-----------------------------------------;
; This routine reads disk sector from floppy. The disk sector to be read
; is identified by the memory variables defined in the data declaration
;-------------------------------------------------------------------------;

read_disk_sector:
        mov ax, [read_segment]      ; Load read segment into ES
        mov es, ax
        mov bx, [read_address]      ; Destination address
        mov ah, 0x2                 ; READ SECTOR-command
        mov al, 0x1                 ; Number of sectors to read (1 Sector)
        mov dl, [boot_drive]        ; Load From boot disk
        mov ch, [read_cyl]          ; Cylinder
        mov cl, [read_sec]          ; Starting Sector
        mov dh, [read_head]         ; Head
        int 0x13                    ; Call interrupt 13h
        or ah, ah                   ; Check for error code
        jnz read_disk_sector_error  ; if error jmp to error handler

        mov ax,[sectors_to_read]    ; The variable sectors_to_read has the total
        sub ax,0x1                  ; number of sectors to read, and is being
        mov [sectors_to_read],ax    ; decremented each time we read a sector

        jmp read_disk_sector_done   ; If we are here then we read the sector
                                    ; successfully, hence we exit

        read_disk_sector_error:     ; Error Occurred
            mov si,read_error       ; Print Error Message
            call bios_print
            read_disk_sector_hlt:   ; Halt System
                jmp read_disk_sector_hlt

        read_disk_sector_done:      ; We are done successfully
            mov si,dot              ; Print a dot on the screen indicating
            call bios_print         ; a sector has been read successfully
ret
;-------------------------------------------------------------------------;


;----------Advance Disk Current Sector Routine----------------------------;
; This routine change the data variables read_sec, read_head, read_cyl to
; point to the next sector to be read
;-------------------------------------------------------------------------;

advance_disk_current_sector:
    mov eax,[read_sec]
    add eax,0x1                              ; Advance read_sec by 1
    mov [read_sec],eax
    cmp eax,[sectors]                        ; check if read_sec overflows
    jne advance_disk_current_sector_done    ; If not exit
    mov eax,0x1                              ; Else set read_sec to 1
    mov [read_sec],eax
    mov eax,[read_head]
    add eax,0x1                              ; Advance read_head by 1
    mov [read_head],eax
    cmp eax,[heads]                          ; Check if read_head over_flows
    jne advance_disk_current_sector_done    ; If not exit
    mov eax,0x0                              ; Else set read_head to 1
    mov [read_head],eax
    mov eax,[read_cyl]                       ; Advance read_cyl by 1
    add eax,0x1
    mov [read_cyl],eax
    advance_disk_current_sector_done:       ; Exit
ret
;-------------------------------------------------------------------------;


;----------Advance Current Memory Pointer Routine-------------------------;
; This routine advances the current memory location used to read in from
; floppy by 1 sector (512 bytes)
;-------------------------------------------------------------------------;

advance_current_memory_ptr:
    mov ax,[read_address]                   ; check if read_address overflows
    cmp ax,0xFE00
    je next_segment                         ; if yes go to increment segment
    add ax,0x200                            ; Else increment it with 512 bytes
    mov [read_address],ax
    jmp advance_current_memory_ptr_done     ; Exit
    next_segment:                           ; Increment Segment
        mov ax,0x0
        mov [read_address],ax               ; Set read_address to 0, start of
        mov ax,[read_segment]               ; the segment
        add ax,0x1000                       ; Advance segment by one memory
        mov [read_segment],ax               ; page, 4 KB
    advance_current_memory_ptr_done:
ret

;-------------------------------------------------------------------------;

;----------Read Sector Routine Routine------------------------------------;
; Raeds current sector, advance sector pointer by one sector, and advance
; read memory location by 512 bytes
;-------------------------------------------------------------------------;
read_sector:
    call read_disk_sector               ; Read Current Sector
    call advance_disk_current_sector    ; Advanvce sector by 1
    call advance_current_memory_ptr     ; Advanve memory by 512 bytes
ret
;-------------------------------------------------------------------------;
