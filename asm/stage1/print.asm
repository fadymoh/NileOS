;----------------------push_segment_index Macro-----------------------;
; This routine prints a string to the screen through updating the video
; buffer avoiding using BIOS interrupts.
; The routine expects the SI register to point tostring to be
; displayed.
; This routine will print characters until it hit a character with ascii
; value 13 (New Line) and will stop
;----------------------------------------------------------------------;

video_print:
    push_segment
    mov bx,0B800h           ; set BX to the start of the video RAM
    mov es,bx               ; Set ES to the start of teh video RAM
    mov bx,[start_location] ; Store the start location for printing in BX

video_print_loop:           ; Loop for a character by charcater processing
    lodsb                   ; Load character pointer to by SI into al
    cmp al,13               ; Check  new line character to stop printing
    je out_video_print_loop ; If so get out
    mov byte [es:bx],al     ; Else Store the charcater into current video location
    add bx,1                ; Increment current video location
    mov byte [es:bx],1Fh    ; Store Blue Backgroun, Yellow font color
    add bx,1                ; Increment current video location
                            ; Each position on the screen is represented by 2 bytes
                            ; The first byte stores the ascii code of the character
                            ; and the second one stores the color attributes
                            ; Foreground and background colors (16 colors) stores in the
                            ; lower and higher 4-bits
    jmp video_print_loop    ; Loop to print next character

out_video_print_loop:
    mov ax,[start_location] ; Store the start location for printing in AX
    add ax,0xA0             ; Add a line to the value of start location (80 x 2 bytes)
    mov [start_location],ax

    pop_segment
ret


end_of_string  db 13        ; The end of the string indicator
start_location   dw  0x500  ; A default start position (Line # 8)
