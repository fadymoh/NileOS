;----------------------push_segment_index Macro-----------------------;
; This macro pushs the eflags, DS, ES, DI, SI regsiters to the stack
;----------------------------------------------------------------------;

%macro push_segment_index 0
    pushf
    push ds
    push es
    push di
    push si

%endmacro

;----------------------pop_segment_index Macro-----------------------;
; This macro pops the eflags, DS, ES, DI, SI regsiters from the stack
;----------------------------------------------------------------------;

%macro pop_segment_index 0
    pop si
    pop di
    pop es
    pop ds
    popf

%endmacro

;----------------------push_segment Macro------------------------------;
; This macro pushs the DS, ES regsiters to the stack
;----------------------------------------------------------------------;

%macro push_segment 0
    push ds
    push es

%endmacro

;----------------------pop_segment Macro-------------------------------;
; This macro pops the DS, ES regsiters from the stack
;----------------------------------------------------------------------;
%macro pop_segment 0
    pop es
    pop ds

%endmacro


;----------------------push_e_index Macro------------------------------;
; This macro pushs the EDI, ESI regsiters to the stack
;----------------------------------------------------------------------;

%macro push_e_index 0
    push edi
    push esi

%endmacro


;----------------------pop_e_index Macro-------------------------------;
; This macro pops the EDI, ESI regsiters to the stack
;----------------------------------------------------------------------;

%macro pop_e_index 0
    pop esi
    pop edi

%endmacro


;----------------------push_general Macro------------------------------;
; This macro push the AX, BX, CX, DX regsiters to the stack
;----------------------------------------------------------------------;

%macro push_general 0
    push ax
    push bx
    push cx
    push dx

%endmacro

;----------------------pop_general Macro-------------------------------;
; This macro pops the AX, BX, CX, DX regsiters to the stack
;----------------------------------------------------------------------;



%macro pop_general 0
    pop dx
    pop cx
    pop bx
    pop ax

%endmacro

;----------------------push_general Macro------------------------------;
; This macro pushs the EAX, EBX, ECX, EDX regsiters to the stack
;----------------------------------------------------------------------;


%macro push_e_general 0
    push eax
    push ebx
    push ecx
    push edx

%endmacro


;----------------------pop_general Macro-------------------------------;
; This macro pops the EAX, EBX, ECX, EDX regsiters to the stack
;----------------------------------------------------------------------;


%macro pop_e_general 0
    pop edx
    pop ecx
    pop ebx
    pop eax

%endmacro
