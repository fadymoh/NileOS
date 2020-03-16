;----------Clear the screen Routine---------------------------------------;
bios_cls:
    mov ah,0x0  ; Set Video Mode Function
    mov al,0x3  ; 80x25 16 color text
    int 0x10
Ret
;----------Clear the screen Routine---------------------------------------;


;----------Print String Routine-------------------------------------------;
; SI: points to the null terminated string to be printed
;-------------------------------------------------------------------------;

bios_print:
    my_print_bios:
        lodsb           ;Loads byte pointed to by SI and increment SI
        or al, al       ;zero=end of str
        jz done         ;Get out
        mov ah, 0x0E    ;Print character whose ascii in AL at current cursor
        int 0x10
        jmp my_print_bios ; loop to the next characater
    done:
ret
;----------Print String Routine-------------------------------------------;
