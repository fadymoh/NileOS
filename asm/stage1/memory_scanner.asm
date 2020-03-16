;---------------------------memory_scanner Routine-----------------------------;
; This routine hecks whether CPU supports long mode or not.
; It return normally if CPU supports Long Mode, or halts execution otherwise.
; This routine expects the DI register to point to the memory array
; to be used to store them memory regions entries into.
;-------------------------------------------------------------------------;
memory_scanner:
    xor ebx, ebx		    ; ebx must be 0 to start
    xor bp, bp		        ; keep an entry count in bp
    mov edx, 0x0534D4150	; Place "SMAP" into edx
    mov eax, 0xe820
    mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
    mov ecx, 24		            ; ask for 24 bytes
    int 0x15

    jc memory_scanner_failed	; carry set on first call means "unsupported function"
    mov edx, 0x0534D4150	    ; Some BIOSes apparently trash this register?
    cmp eax, edx		        ; on success, eax must have been reset to "SMAP"
    jne memory_scanner_failed
    test ebx, ebx		        ; ebx = 0 implies list is only 1 entry long (worthless)
    je memory_scanner_failed
    jmp memory_scanner_process_entry ; If successful jump to process entry section
                                ; and skip the next code section as it is needed
                                ; startig from the second detected memory entry

memory_scanner_after_first:
    mov edx, 0x0534D4150	    ; repair potentially trashed register
    mov eax, 0xe820		    ; eax, ecx get trashed on every int 0x15 call
    mov [es:di + 20], dword 1       ; force a valid ACPI 3.X entry
    mov ecx, 24		            ; ask for 24 bytes again
    int 0x15
    jc memory_scanner_successful ; carry set means "end of list already reached"

memory_scanner_process_entry:
    jcxz memort_scanner_skip_entry		; skip any 0 length entries
    cmp cl, 20		                    ; got a 24 byte ACPI 3.X response?
    jbe short memory_scanner_check_entry
    test byte [es:di + 20], 1	        ; if so: is the "ignore this data" bit clear?
    je memort_scanner_skip_entry        ; Then Skip this entry

memory_scanner_check_entry:
    mov ecx, [es:di + 8]            ; get lower dword of memory region length
    or ecx, [es:di + 12]            ; "or" it with upper dword to test for zero
    jz memort_scanner_skip_entry    ; if length qword is 0, skip entry
    inc bp			    ; got a good entry: ++count, move to next storage spot
    add edi, 24
    add edi, 8                       ; this is for virtual address to be used in kernel stage 2
                                    ; we leave this 64-bits space to store in it the starting
                                    ; virtual address, so we can handler memory in place
    add edi, 8                       ; this is for virtual address to be used in kernel stage 2
                                    ; we leave this 64-bits space to store in it the starting
                                    ; virtual address, so we can handler memory in place

memort_scanner_skip_entry:
    test ebx, ebx		                ; if ebx resets to 0, list is complete
    jne memory_scanner_after_first      ; Loop to fetch next entry


memory_scanner_successful:
    
    mov word [mmap_ent], bp	                ; store the entry count
    mov si, memory_scanner_successful_string    ; Print a message indicating finishing
                                                ; memory detection successfully
                                                ; Point SI to memory_scanner_successful_string
    call video_print                            ; Call video_print to display message on screen
    ret

memory_scanner_failed:                          ; We come here when errors occurs
    mov si, memory_error_string                 ; Print error message on the screen
                                                ; Point SI to memory_error_string
    call video_print                            ; Call video_print to display message on screen
    jmp halt


memory_info_indicator db 'MEMORY_INDICATOR', 0   ; Indicator that will be searched later from the
                                                  ; kernel C Code to fetch the memory info stored
                                                  ; by the above routine to be loaded later in
                                                  ; other more organized data strucutures.
memory_info times 1024 db 0                       ; Memory area to store the memory information.
mmap_ent   dw  0                                  ; A memory variable to store the number of memory
                                                  ; entries detected by the above routine.

memory_error_string   db 'Error Getting Memory Info', 13, 10, 0 ; Error message to be printed upon
                                                                ; errors in detecting memory
memory_scanner_successful_string   db 'Memory Scanned Successfully', 13, 10, 0 ;A message to be printed
                                                        ; after successful memory detection.