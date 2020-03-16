;----------hexa2ascii Routine-------------------------------------------;
; This routine converts a 4 digits Hexa decimal values to 
; their ascii reporesentation
; AX: Hexa decimal value to be converted to a string
; SI: A pointer to a memory location to store the hexa decimal string into
;-------------------------------------------------------------------------;

hexa2ascii:
    mov dx,16   ; Used as a divider
    mov bx,ax   ; Store AX to BX as we will use AX in something else
    next_digit:
        mov ax,bx               ; Store value for AX in BX
        cmp dl,0x0              ; if DL is zero then divide by zero case
        je hexa2ascii_done      ; Then exit
        mov cx,dx               ; Move DX into CX
        sub cx,4                ; Subtract 4 from CX
        shr ax,cl               ; Shift right AX with a number of 
                                ; bits equivelant to the value in CL
                                ; to get the first digit value
        and al,0x0f             ; And the value in AL with 0x0f to clear out
                                ; the upper 4 bits of the AL
        cmp al,0x9              ; if less than 0x9 then it is a digit
        jle  less_than_9        ; jump to less_tan_9
        sub al,0xa              ; Else it is a letter, so we subtract 0xa
        add al,'A'              ; Add the value in al to the ascii of 'A'
        jmp store_digit         ; Jump to store_result
    less_than_9:
        add al,'0'              ; Add the value in AL to the ascii of '0'
    store_digit:
        mov [si],al             ; Store the digit in the current location of
                                ; the memory buffer pointed to by SI
        add si,0x1              ; increment SI by 1
        sub dx,0x4              ; increment DX by for to work on the next digit
        jmp next_digit          ; loop until DX is 0x0. Maximum 4 times: 16/4
    hexa2ascii_done:
ret
