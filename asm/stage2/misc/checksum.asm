[BITS 64]
SECTION .text

global ip_hdr_checksum



ip_hdr_checksum:
    xor rax,rax
    xor rcx,rcx
    mov cx,WORD[rdi]
    add eax,ecx
    mov cx,WORD[rdi+2]
    add eax,ecx
    mov cx,WORD[rdi+4]
    add eax,ecx
    mov cx,WORD[rdi+6]
    add eax,ecx
    mov cx,WORD[rdi+8]
    add eax,ecx
    mov cx,WORD[rdi+10]
    add eax,ecx
    mov cx,WORD[rdi+12]
    add eax,ecx
    mov cx,WORD[rdi+14]
    add eax,ecx
    mov cx,WORD[rdi+16]
    add eax,ecx
    mov cx,WORD[rdi+18]
    add eax,ecx   
    mov ecx,eax
    shr eax,16
    and ecx,0x0000FFFF
    add eax,ecx
    not ax
ret


icmp_checksum:
    xor rax,rax
    xor rcx,rcx
    mov cx,WORD[rdi]
    add eax,ecx
    mov cx,WORD[rdi+2]
    add eax,ecx
    mov cx,WORD[rdi+4]
    add eax,ecx
    mov cx,WORD[rdi+6]
    add eax,ecx
    mov cx,WORD[rdi+8]
    add eax,ecx
    mov cx,WORD[rdi+10]
    add eax,ecx
    mov cx,WORD[rdi+12]
    add eax,ecx
    mov cx,WORD[rdi+14]
    add eax,ecx
    mov cx,WORD[rdi+16]
    add eax,ecx
    mov cx,WORD[rdi+18]
    add eax,ecx   
    mov cx,WORD[rdi+20]
    add eax,ecx
    mov cx,WORD[rdi+22]
    add eax,ecx
    mov cx,WORD[rdi+24]
    add eax,ecx
    mov cx,WORD[rdi+26]
    add eax,ecx
    mov cx,WORD[rdi+28]
    add eax,ecx
    mov cx,WORD[rdi+30]
    add eax,ecx
    mov cx,WORD[rdi+32]
    add eax,ecx
    mov cx,WORD[rdi+34]
    add eax,ecx
    mov cx,WORD[rdi+36]
    add eax,ecx
    mov cx,WORD[rdi+38]
    add eax,ecx   
    
    
    
    mov ecx,eax
    shr eax,16
    and ecx,0x0000FFFF
    add eax,ecx
    not ax
ret
