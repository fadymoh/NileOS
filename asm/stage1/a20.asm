;---------------------------A20 Error Routine-----------------------------;
; This routine prints an error message indicating failure setting the A20
; Line Address and jump to the halt
;-------------------------------------------------------------------------;

A20Error:
	mov si, a20_error   ; Assigns the string variable a20_error to the si
                        ; register
	call video_print    ; call video_print routine to print the error
                        ; message on the screen
	jmp halt            ; jmp to the hatt labl to halt execution of the
                        ; program


;---------------------------CheckA20 Routine------------------------------;
; This routine checks if the A20 Line Address bit is enabled or not using
; INT 0x15 function 0x2402
; It prints a message indicating the status of the A20 bit Line Address
; and sets the value of the memory variable a20_enabled to 1
;-------------------------------------------------------------------------;
CheckA20:
    push_segment_index   ; Call push_segment_index macro to push DS, ES, SI, DI
    mov ax, 0x2402       ; Call INT 0x15 Function 0x2402 to get the A20 line
    int 0x15             ; Address status: disabled for AL = 0, and enabled
                         ; for AL=1
    jnc   continue_a20        ; Jump to A20Error upon INT errors
    cmp ah,0x87
    je A20Error
continue_a20:
    cmp al,0             ; if AL = 0 then A20 is disabled
    je check_a20__disabled ; then jmp to check_a20__disabled
                            ; ELSE
    xor ax,ax               ; Set AX register to zero
    mov al, 1               ; Set Al register to 1
    mov byte [a20_enabled],al ; Set memory byte location a20_enabled to 1
    mov si, a20_enabled_string  ; Print a message indicating that A20 line address
                                ; is enabled. Point SI register to a20_enabled_string
    call video_print            ; Call video_print to print the message on the screen
    jmp check_a20__exit         ; Jump to check_a20__exit

    check_a20__disabled:        ; If A20 line address is disabled
        mov si, a20_disabled_string ; Print a message indicating that  the A20 line
                                    ; address is disabled. Point SI register to
                                    ; a20_disabled_string
        call video_print            ; Call video_print to print the message on the screen


    check_a20__exit:       ; Label indicating common code need to be excuted before return
        pop_segment_index  ; Call pop_segment_index macro to pop DS, ES, SI, DI
ret


;---------------------------EnableA20 Routine------------------------------;
; This routine enables the A20 Line Address using INT 0x15 function 0x2401
;-------------------------------------------------------------------------;

EnableA20:

    push_segment_index  ; Call push_segment_index macro to push DS, ES, SI, DI
	mov ax, 0x2401      ; Call INT 0x15 Function 0x2401 to set the A20 line
	int 0x15
    jc   A20Error       ; Jump to A20Error upon INT errors

    mov si, a20_set_string  ; Print a message indicating that the A20 line address
                        ; is enabled. Point SI register to a20_set string
    call video_print    ; Call video_print to print the message on the scree
    pop_segment_index   ; Call pop_segment_index macro to pop DS, ES, SI, DI
ret

a20_set_string   db 'A20 Enabled Successfullt', 13, 10, 0    ; String indicating that
                                                    ; the A20 line address is set successfully
a20_error   db 'A20 Error', 13, 10, 0           ; String indicating errors when performing
                                                ; INT 0x15 functions

a20_enabled_string   db 'A20 is already enabled', 13, 10, 0 ; String indicating that the A20 Line
                                                    ; address is already enabled
a20_disabled_string   db 'A20 is disabled', 13, 10, 0 ; String indicating that the A20 Line
                                                    ; address is already disabled
a20_enabled db 0                        ; A flag indicating the A20 Line Address status
                                        ; 0 = Disabled, 1 = Enabled
