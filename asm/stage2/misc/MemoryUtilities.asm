[BITS 64]
SECTION .text

;*****************************************************************************
;   Global declaration of all included functions so they can be accessible
;   from the C-Land.
;*****************************************************************************

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Memory copy operations family
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

global memcpy
global memcpyc
global memcpy8
global memcpy16
global memcpy32
global memcpy64

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Memory move operations family
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

global memmove
global memmovec
global memmove8
global memmove16
global memmove32
global memmove64

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Memory intialization operations family
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

global memset
global memsetc
global memset8
global memset16
global memset32
global memset64

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Memory comparison operations family
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

global memcmp
global memcmpc
global memcmp8
global memcmp16
global memcmp32
global memcmp64

global rmemcmp
global rmemcmpc
global rmemcmp8
global rmemcmp16
global rmemcmp32
global rmemcmp64


global scanc
global scan8
global scan16
global scan32
global scan64


global rscanc
global rscan8
global rscan16
global rscan32
global rscan64


global strlen

global strcmp_sse42
global strlen_sse42
;*****************************************************************************
;   memcpy: copies a block of memory byte items from source address stored in
;           the RSI register to the memory address stored in RDI. The number of
;           bytes to be copied is stored in RDX.
;
;           Note that we have overloaded the memcpy function with memcpyc for 
;           character memory copy  and memcpy8 for uint8_t memory copy were  
;           both deal with memory bytes copy operation.
;
;           The function is optimized so it copies 8-bytes at a time whenever
;           possible and the remaining bytes which will be less than 8 bytes
;           will be copied byte by byte.
;
;           The function does not handle overlapping memory areas.
;*****************************************************************************
memcpyc:
memcpy8:
memcpy:
    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx

    ; We try to optimize the copy performance by copying 8 bytes at a time
    ; if possible. 

    ; Divide the value in RDX (length to be copied) by 8 to get the number 
    ; of eight bytes uints in RAX and the remainder is then stored in RDX
    mov rax,rdx
    xor rdx,rdx
    mov rcx,8
    div rcx
    ; RAX = number of eight byte units 
    ; RDX = number of remaining bytes

    ; Loop on RAX to copy the calculated eight byte units
    ; 8 bytes at a time in uphill (using CLD) direction using REP MOVSQ
    mov rcx, rax
    cld
    rep movsq


    ; Loop on RDX to copy the remaining bytes one byte at a time
    ; in uphill (using CLD) direction using REP MOVSB
    
    mov rcx,rdx
    cld
    rep movsb

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret

;*****************************************************************************
;   memcpy16: copies a block of memory two byte items from source address 
;           stored in the RSI register to the memory address stored in RDI. 
;           The number of two byte items to be copied is stored in RDX.
;
;           The function is optimized so it copies 8-bytes at a time whenever
;           possible and the remaining even number of two bytes which will be 
;           less than 8 bytes will be copied two bytes at a time.
;
;           The function does not handle overlapping memory areas.
;*****************************************************************************


memcpy16:
    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx

    ; We try to optimize the copy performance by copying 8 bytes at a time
    ; if possible. 

    ; Divide the value in RDX (length to be copied) by 4 to get the number 
    ; of eight byte uints in RAX and the remainder is then stored in RDX
    ; we divide by 4 because the length introduced in RDX is in terms of
    ; two bytes.

    mov rax,rdx
    xor rdx,rdx
    mov rcx,4
    div rcx
    ; RAX = number of eight byte units 
    ; RDX = number of remaining two byte items


    ; Loop on RAX to copy the calculated eight byte units
    ; 8 bytes at a time in uphill (using CLD) direction using REP MOVSQ
    mov rcx, rax
    cld
    rep movsq


    shl rdx,1       ; Divide RDX by 2 through shifting one location to get the 
                    ; remaining number of two byte items.

    ; Loop on RDX to copy the remaining two byte elements two bytes at a time
    ; in uphill (using CLD) direction using REP MOVSW

    mov rcx,rdx
    cld
    rep movsw

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret

;*****************************************************************************
;   memcpy32: copies a block of memory four byte items from source address 
;           stored in the RSI register to the memory address stored in RDI. 
;           The number of four byte items to be copied is stored in RDX.
;
;           The function is optimized so it copies 8-bytes at a time whenever
;           possible and the remaining even number of four bytes which will be 
;           less than 8 bytes will be copied four bytes at a time.
;
;           The function does not handle overlapping memory areas.
;*****************************************************************************


memcpy32:
    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx


    ; We try to optimize the copy performance by copying 8 bytes at a time
    ; if possible. 

    ; Divide the value in RDX (length to be copied) by 2 to get the number 
    ; of eight byte uints in RAX and the remainder is then stored in RDX
    ; we divide by 2 because the length introduced in RDX is in terms of
    ; four bytes.

    mov rax,rdx
    xor rdx,rdx
    mov rcx,2
    div rcx

    ; RAX = number of eight byte units 
    ; RDX = number of remaining four byte items


    ; Loop on RAX to copy the calculated eight byte units
    ; 8 bytes at a time in uphill (using CLD) direction using REP MOVSQ
    mov rcx, rax
    cld
    rep movsq

    shl rdx,2       ; Divide RDX by 2 through shifting two location to get the 
                    ; remaining number of four byte items.

    ; Loop on RDX to copy the remaining two byte elements two bytes at a time
    ; in uphill (using CLD) direction using REP MOVSW
    mov rcx,rdx
    cld
    rep movsd

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret



;*****************************************************************************
;   memcpy64: copies a block of memory eight byte items from source address 
;           stored in the RSI register to the memory address stored in RDI. 
;           The number of eight byte items to be copied is stored in RDX.
;
;           The function does not handle overlapping memory areas.
;*****************************************************************************

memcpy64:
    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx

    ; Loop on RDX to copy eight byte units 8 bytes at a time in uphill 
    ; (using CLD) direction using REP MOVSQ

    mov rcx,rdx
    cld
    rep movsq

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret

;*****************************************************************************
;   memmove: copies a block of memory byte items from source address stored in
;           the RSI register to the memory address stored in RDI. The number of
;           bytes to be copied is stored in RDX.
;
;           Note that we have overloaded the memmove function with memmovec for 
;           character memory move and memmove8 for uint8_t memory move were  
;           both deal with memory bytes copy operation.
;
;           The function is designed to handle overlapping memory areas.
;           In case the source address is larger than the destination address
;           we can use the optimized memcpy family to do the move. On the 
;           other hand if the source address is smaller than destination  
;           address and the memory regions overlap, using the memcpy can cause 
;           data corruption so we adopt another technique to move the data
;           through copying data downhill from the end of the memory regions
;           and heading towards the beginning of the memory regions byte by
;           byte.
;*****************************************************************************

memmovec:
memmove8:
memmove:
    cmp rsi,rdi     ; Check if the source address is larger than the destination
    jg  memcpy      ; Use memcpy by jumping to memcpy label

    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx
    ; set the RCX loop counter to RDX which has the number of 
    ; bytes to be copied.
    mov rcx,rdx 

    ; Decrement RDX by 1 so when added to RDI and RSI they point
    ; to the last element in their destination and source 
    ; memory regions respectively.
    dec rdx     
    add rdi,rdx
    add rsi,rdx
    
    ; Set loop direction to downhill (decrimenting RSI and RDI)
    std        
    ; copy bytes downhill from the end of the source memory
    ; region pointed to by RSI to memory region pointed to by RDI
    ; heading towards the beginning of the memory regions by REP MOVSB
    rep movsb  

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret


;*****************************************************************************
;   memmove16: copies a block of memory two byte items from source address 
;           stored in the RSI register to the memory address stored in RDI.   
;           The number of two bytes to be copied is stored in RDX.
;
;           The function is designed to handle overlapping memory areas.
;           In case the source address is larger than the destination address
;           we can use the optimized memcpy16 function to do the move. On the 
;           other hand if the source address is smaller than destination  
;           address and the memory regions overlap, using the memcpy can cause 
;           data corruption so we adopt another technique to move the data
;           through copying data downhill from the end of the memory regions
;           and heading towards the beginning of the memory regions two bytes
;           at a time.
;*****************************************************************************


memmove16:
    cmp rsi,rdi     ; Check if the source address is larger than the destination
    jg  memcpy16      ; Use memcpy by jumping to memcpy label

    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx

    ; set the RCX loop counter to RDX which has the number of 
    ; two bytes to be copied.
    
    mov rcx,rdx

    ; Multiply the value in  RDX by 2 through shifting one bit location
    ; and decrement it by 2 so when added to RDI and RSI they point
    ; to the last element in their destination and source memory regions 
    ; respectively.

    shl rdx,1  
    sub rdx,2

    add rdi,rdx
    add rsi,rdx
    
    ; Set loop direction to downhill (decrimenting RSI and RDI)
    std

    ; Copy two bytes downhill from the end of the source memory
    ; region pointed to by RSI to memory region pointed to by RDI
    ; heading towards the beginning of the memory regions by REP MOVSW
    ; which copies two bytes at a time.
    rep movsw

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret

;*****************************************************************************
;   memmove32: copies a block of memory four byte items from source address 
;           stored in the RSI register to the memory address stored in RDI.   
;           The number of four bytes to be copied is stored in RDX.
;
;           The function is designed to handle overlapping memory areas.
;           In case the source address is larger than the destination address
;           we can use the optimized memcpy32 function to do the move. On the 
;           other hand if the source address is smaller than destination  
;           address and the memory regions overlap, using the memcpy can cause 
;           data corruption so we adopt another technique to move the data
;           through copying data downhill from the end of the memory regions
;           and heading towards the beginning of the memory regions four bytes
;           at a time.
;*****************************************************************************

memmove32:
    cmp rsi,rdi     ; Check if the source address is larger than the destination
    jg  memcpy32    ; Use memcpy by jumping to memcpy label

    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx

    ; set the RCX loop counter to RDX which has the number of 
    ; four bytes to be copied.

    mov rcx,rdx

    ; Multiply the value in  RDX by 4 through shifting two bit locations
    ; and decrement it by 4 so when added to RDI and RSI they point
    ; to the last element in their destination and source memory regions 
    ; respectively.

    shl rdx,2
    sub rdx,4

    add rdi,rdx
    add rsi,rdx

    ; Set loop direction to downhill (decrimenting RSI and RDI)
    std
    ; Copy four bytes downhill from the end of the source memory
    ; region pointed to by RSI to memory region pointed to by RDI
    ; heading towards the beginning of the memory regions by REP MOVSD
    ; which copies four bytes at a time.

    rep movsd

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret


;*****************************************************************************
;   memmove64: copies a block of memory eight byte items from source address 
;           stored in the RSI register to the memory address stored in RDI.   
;           The number of eight bytes to be copied is stored in RDX.
;
;           The function is designed to handle overlapping memory areas.
;           In case the source address is larger than the destination address
;           we can use the optimized memcpy64 function to do the move. On the 
;           other hand if the source address is smaller than destination  
;           address and the memory regions overlap, using the memcpy can cause 
;           data corruption so we adopt another technique to move the data
;           through copying data downhill from the end of the memory regions
;           and heading towards the beginning of the memory regions four bytes
;           at a time.
;*****************************************************************************


memmove64:
    cmp rsi,rdi     ; Check if the source address is larger than the destination
    jg  memcpy64    ; Use memcpy by jumping to memcpy label

    ; Reserve registers by pushing them into the stack
    push rdi
    push rsi
    push rdx
    push rcx

    ; set the RCX loop counter to RDX which has the number of 
    ; four bytes to be copied.

    mov rcx,rdx

    ; Multiply the value in  RDX by 8 through shifting three bit locations
    ; and decrement it by 8 so when added to RDI and RSI they point
    ; to the last element in their destination and source memory regions 
    ; respectively.

    shl rdx,3
    sub rdx,8

    add rdi,rdx
    add rsi,rdx

    ; Set loop direction to downhill (decrimenting RSI and RDI)
    std

    ; Copy eight bytes downhill from the end of the source memory
    ; region pointed to by RSI to memory region pointed to by RDI
    ; heading towards the beginning of the memory regions by REP MOVSQ
    ; which copies eight bytes at a time.
    rep movsq

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rsi
    pop rdi
ret

;*****************************************************************************
;   memset: initializes a block of memory byte items pointed to by the  address 
;           stored in the RDI register to the byte value stored in RSI. The 
;           number of bytes to be initialzied is stored in RDX.
;
;           Note that we have overloaded the memset function with memsetc for 
;           character memory regions initialization and memset8 for uint8_t 
;           memory regions initialization were both deal with memory bytes 
;           copy operation.
;*****************************************************************************

memsetc:
memset8:
memset:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out the value of RAX, assign si to AX register and zero out
    ; AH so we are sure that only AL contains the byte value to be used
    ; for initialization
    xor rax,rax
    mov ax,si
    mov ah,0x0

    ; set the RCX loop counter to RDX which has the number of 
    ; bytes to be initialized

    mov rcx,rdx

    ; Clear loop direction to be uphill (incrementing RSI and RDI)

    cld

    ; Store the value of AL into the byte elements of the  memory region 
    ; pointed to by RDI, one byte at a time using REP stosb
    rep stosb

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret
;*****************************************************************************
;   memset16: initializes a block of memory two byte items pointed to by the   
;           address stored in the RDI register to the byte value stored in RSI.  
;           The number of two byte items to be initialzied is stored in RDX.
;
;*****************************************************************************
memset16:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx


    ; Zero out the value of RAX, assign si to AX register so we are sure
    ; that only ax contains the two bytes value to be used for initialization

    xor rax,rax
    mov ax,si

    ; set the RCX loop counter to RDX which has the number of 
    ; two bytes to be initialized
    mov rcx,rdx

    ; Clear loop direction to be uphill (incrementing RSI and RDI)
    cld

    ; Store the value of AX into the two byte elements of the  memory region 
    ; pointed to by RDI, two bytes at a time using REP STOSW
    rep stosw

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   memset32: initializes a block of memory four byte items pointed to by the   
;           address stored in the RDI register to the byte value stored in RSI.  
;           The number of four byte items to be initialzied is stored in RDX.
;
;*****************************************************************************

memset32:

    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out the value of RAX, assign ESX to EAX register

    xor rax,rax
    mov eax,esi

    ; set the RCX loop counter to RDX which has the number of 
    ; four bytes to be initialized

    mov rcx,rdx

    ; Clear loop direction to be uphill (incrementing RSI and RDI)
    cld

    ; Store the value of EAX into the four byte elements of the memory region 
    ; pointed to by RDI, four bytes at a time using REP STOSW
    rep stosd

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   memset64: initializes a block of memory eight byte items pointed to by the   
;           address stored in the RDI register to the byte value stored in RSI.  
;           The number of eight byte items to be initialzied is stored in RDX.
;
;*****************************************************************************

memset64:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out the value of RAX

    mov rax,rsi

    ; set the RCX loop counter to RDX which has the number of 
    ; four bytes to be initialized

    mov rcx,rdx

    ; Clear loop direction to be uphill (incrementing RSI and RDI)
    cld

    ; Store the value of RAX into the eight byte elements of the memory region 
    ; pointed to by RDI, eight bytes at a time using REP STOSW
    rep stosq

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   memcmp: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in an uphill direction starting
;           from the address stored in RDI and RSI and moving right for a
;           number of bytes equal to the value stored in RDX representing the
;           length of byte items comparisons.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer of the first byte causing
;           dissimilarity in the memory region pointed to by RDI.
;
;           Note that we have overloaded the memcmp function with memcmpc for 
;           character memory comparison and memcmp8 for uint8_t memory   
;           comparison where both deal with memory bytes comparison operation.
;
;           The function is optimized so it compares 8-bytes at a time whenever
;           possible and the remaining bytes which will be less than 8 bytes
;           will be compared one byte at a time.
;
;*****************************************************************************

memcmpc:
memcmp8:
memcmp:

    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.
    xor rax,rax
    push rax 

    ; We try to optimize the comparison performance by comparing 8 bytes 
    ; at a time if possible. 

    ; Divide the value in RDX (length to be copied) by 8 to get the number 
    ; of eight bytes uints in RAX and the remainder is then stored in RDX
    mov rax,rdx
    xor rdx,rdx
    mov rcx,8
    div rcx

    ; RAX = number of eight byte units 
    ; RDX = number of remaining bytes

    ; Loop on RAX through copying RAX into the loop counter register RCX to 
    ; compare the calculated eight byte units, 8 bytes at a time in uphill 
    ; (using CLD) direction using REPE CMPSQ

    mov rcx, rax
    cld
    repe cmpsq

    ; if no mismatch occurs goto memcmp_normal to compare the remaining bytes
    ; if there are any
    je memcmp_normal

    ; if we are here then a mismacth occured, and the mismatch occured on an
    ; eight-bytes bases, so we do not know which byte caused the mismatch
    ; so we backtrack one eight-bytes units and add 8 to the RDX so we can
    ; recompare the eight bytes with the remainder
    sub rdi,8
    sub rsi,8
    add rdx,8

    ; we now compare the remainder bytes one byte at a time in uphill 
    ; (using CLD) direction using REPE CMPSB
    memcmp_normal:
    mov rcx,rdx
    cld
    repe cmpsb

    ; if no mismatch occurs skip the mismatch handler to clean up and
    ; return from the function
    je memcmp_out 
    
    ; else handle mismatch.
    memcmp_mismatch:

    ; we decrement RDI to point to the byte that caused the mismatch, then
    ; we remove the NULL value originally pushed into the stack at the
    ; beginning of the function and replace it with RDI through pushing RDI
    ; to the stack
    dec rdi
    pop rax
    push rdi

    ; Clean up and exit the function
    memcmp_out:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the byte
    ; causing the mismatch in RDI
    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   memcmp16: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in an uphill direction starting
;           from the address stored in RDI and RSI and moving right for a
;           number of two bytes equal to the value stored in RDX representing 
;           the length of two byte items comparison.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer of the first two bytes item 
;           causing dissimilarity in the memory region pointed to by RDI.
;
;           The function is optimized so it compares 8-bytes at a time whenever
;           possible and the remaining two byte items which will be less than 4
;           items will be compared two bytes item at a time.
;
;*****************************************************************************

memcmp16:
   ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.
    xor rax,rax
    push rax 

    ; We try to optimize the comparison performance by comparing 8 bytes 
    ; at a time if possible. 

    ; Divide the value in RDX (length to be copied) by 4 to get the number 
    ; of eight bytes uints in RAX and the remainder is then stored in RDX

    mov rax,rdx
    xor rdx,rdx
    mov rcx,4
    div rcx
    ; RAX = number of eight byte units 
    ; RDX = number of remaining two byte items

    ; Loop on RAX through copying RAX into the loop counter register RCX to 
    ; compare the calculated eight byte units, 8 bytes at a time in uphill 
    ; (using CLD) direction using REPE CMPSQ

    mov rcx, rax
    cld
    repe cmpsq

    ; if no mismatch occurs goto memcmp_normal to compare the remaining bytes
    ; if there are any
    je memcmp_normal16

    ; if we are here then a mismacth occured, and the mismatch occured on an
    ; eight-bytes bases, so we do not know which two byte items caused the 
    ; mismatch so we backtrack one eight-bytes units and add 4 to the RDX 
    ; so we can recompare the four two byte items with the remainder
    sub rdi,8
    sub rsi,8
    add rdx,4

    ; we now compare the remainder two byte items two bytes at a time in uphill 
    ; (using CLD) direction using REPE CMPSW
    memcmp_normal16:

    mov rcx,rdx
    cld
    repe cmpsw

    ; if no mismatch occurs skip the mismatch handler to clean up and
    ; return from the function

    je memcmp16_out
    
    memcmp16_mismatch:

    ; we decrement RDI by 2 to point to the byte that caused the mismatch, then
    ; we remove the NULL value originally pushed into the stack at the
    ; beginning of the function and replace it with RDI through pushing RDI
    ; to the stack

    sub rdi,2
    pop rax
    push rdi

    ; Clean up and exit the function
    memcmp16_out:

    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the byte
    ; causing the mismatch in RDI
    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   memcmp32: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in an uphill direction starting
;           from the address stored in RDI and RSI and moving right for a
;           number of four bytes equal to the value stored in RDX representing 
;           the length of four byte items comparison.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer of the first two bytes item 
;           causing dissimilarity in the memory region pointed to by RDI.
;
;           The function is optimized so it compares 8-bytes at a time whenever
;           possible and the remaining four byte items which will be less than 
;           2 items will be compared four bytes item at a time.
;
;*****************************************************************************

memcmp32:
   ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.
    xor rax,rax
    push rax 

    ; We try to optimize the comparison performance by comparing 8 bytes 
    ; at a time if possible. 

    ; Divide the value in RDX (length to be copied) by 2 to get the number 
    ; of eight byte uints in RAX and the remainder is then stored in RDX

    mov rax,rdx
    xor rdx,rdx
    mov rcx,2
    div rcx
    ; RAX = number of eight byte units 
    ; RDX = number of remaining four byte items

    mov rcx, rax
    cld
    repe cmpsq
    jne memcmp32_mismatch

    ; Loop on RAX through copying RAX into the loop counter register RCX to 
    ; compare the calculated eight byte units, 8 bytes at a time in uphill 
    ; (using CLD) direction using REPE CMPSQ

    mov rcx,rdx
    cld
    repe cmpsd

    ; if no mismatch occurs goto memcmp_normal to compare the remaining bytes
    ; if there are any
    je memcmp_normal32

    ; if we are here then a mismacth occured, and the mismatch occured on an
    ; eight-bytes bases, so we do not know which two byte items caused the 
    ; mismatch so we backtrack one eight-bytes units and add 2 to the RDX 
    ; so we can recompare the two four byte items with the remainder
    sub rdi,8
    sub rsi,8
    add rdx,2

    ; we now compare the remainder four byte items four bytes at a time in uphill 
    ; (using CLD) direction using REPE CMPSW
    memcmp_normal32:

    mov rcx,rdx
    cld
    repe cmpsd

    ; if no mismatch occurs skip the mismatch handler to clean up and
    ; return from the function

    je memcmp32_out ; 
    
    memcmp32_mismatch:

    ; we decrement RDI by 4 to point to the four byte item that caused the 
    ; mismatch, then we remove the NULL value originally pushed into the 
    ; stack at the beginning of the function and replace it with RDI through 
    ; pushing RDI to the stack

    sub rdi,4
    pop rax
    push rdi

    ; Clean up and exit the function
    memcmp32_out:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the byte
    ; causing the mismatch in RDI
    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret


;*****************************************************************************
;   memcmp64: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in an uphill direction starting
;           from the address stored in RDI and RSI and moving right for a
;           number of eight bytes equal to the value stored in RDX representing 
;           the length of eight byte items comparison.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer of the first two bytes item 
;           causing dissimilarity in the memory region pointed to by RDI.
;
;*****************************************************************************

memcmp64:
   ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.
    xor rax,rax
    push rax 

    ; Loop on RDX through copying it into the loop counter register RCX to 
    ; compare the calculated eight byte units, 8 bytes at a time in uphill 
    ; (using CLD) direction using REPE CMPSQ

    mov rcx,rdx
    cld
    repe cmpsq

    ; if no mismatch occurs clean up and exit function

    je memcmp64_out

    ; if we are here then a mismatch occured, so we decrement RDI by 8 to point
    ; to the byte that caused the mismatch, then we remove the NULL value 
    ; originally pushed into the stack at the beginning of the function and
    ; replace it with RDI through pushing RDI to the stack
   
    sub rdi,8
    pop rax
    push rdi

    ; Clean up and exit the function

    memcmp64_out:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the byte
    ; causing the mismatch in RDI
    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   rmemcmp: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in a downhill direction starting
;           from the the end of the memory regions and heading left towards 
;           the addresses stored in RDI and RSI for a number of bytes equal to 
;           the value stored in RDX representing the length of byte items 
;           comparisons.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer to the first byte causing
;           dissimilarity from the end of the memory region pointed to by RDI.
;
;           Note that we have overloaded the rmemcmp function with memcmpc for 
;           character memory comparison and rmemcmp8 for uint8_t memory   
;           comparison where both deal with memory bytes comparison operation.
;
;           The function is optimized so it compares 8-bytes at a time whenever
;           possible and the remaining bytes which will be less than 8 bytes
;           will be compared one byte at a time.
;
;*****************************************************************************


rmemcmpc:
rmemcmp8:
rmemcmp:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Check if the size of the memory to be compared is bigger than eight bytes
    ; in which case we need to do byte-based comparison and skip optimization
    cmp rdx,8
    jg rmemcmp_normal

    ; If we are here then the memory region size is smaller than 8 bytes 
    ;and hence we will compare a byte at a time.
    
    ; Set loop counter register RCX to the number of bytes to be compared 
    ; stored in RDX

    mov rcx,rdx
    ; Decrement RDX and add it to RDI and RSI so they point to the last byte
    ; in both memory regions subject to comparison.
    dec rdx
    add rdi,rdx
    add rsi,rdx
    
    ; we now compare the remainder bytes one byte at a time in downhill 
    ; (using STD) direction using REPE CMPSB
    std
    repe cmpsb

    ; if no mismatch encountered adjust stack
    je rmemcmp_adjust

    
    ; else reverse one byte in RDI and RSI to identify the address of the
    ; byte that caused the mismatch, and push the address stored in RDI
    ; on the top of the stack and then jump to the stack cleant up and exit.
    inc rdi
    inc rsi
    push rdi
    jmp rmemcmp_out

    ; push a NULL value on the top of the stack to be poped to RAX before exiting
    ; the function.
    rmemcmp_adjust:
        xor rax,rax
        push rax
        ; Goto clean up and exit
        jmp rmemcmp_out

    rmemcmp_normal:
    ; If we are here then the size of the memory regions is larger than 8 bytes

    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.
    xor rax,rax
    push rax 

    ; Add RDX to RDI and RSI and decrement RDI and RSI by 8 so both point to
    ; the last eight bytes item in both memory buffers subject to comparison

    add rdi,rdx
    add rsi,rdx
    sub rdi,8
    sub rsi,8

    ; Divide the value in RDX (length to be copied) by 8 to get the number 
    ; of eight byte uints in RAX and the remainder is then stored in RDX.
    ; We divide by 8 because the length introduced in RDX is in terms of
    ; bytes.

    mov rax,rdx
    xor rdx,rdx
    mov rcx,8
    div rcx
    ; RAX = number of eight byte units 
    ; RDX = number of remaining bytes

    ; Set loop counter register RCX to the number of eight bytes to be compared 
    ; stored in RAX

    mov rcx, rax

    ; we now compare the calculated eight byte items one byte at a time in  
    ; downhill (using STD) direction using REPE CMPSQ
    std
    repe cmpsq

    ; if now mismatch occurs proceed with comparing the remaining bytes if
    ; any is available
    je rmemcmp_match_until_now

    ; if we are here then a mismacth occurred, and the mismatch occurred on an
    ; eight-bytes bases, so we do not know which byte item caused the 
    ; mismatch so we backtrack one eight-bytes by adding 15 to RDI and RSI to
    ; point to the last byte of the eight byte items that cause the mismatch
    ; and we also add 8 to RDX to increase the count of remaining bytes

    add rdx,8
    add rdi,15
    add rsi,15

    rmemcmp_match_until_now:

    ; Start comparing the remaining bytes
    ; Set loop counter register RCX to the number of remaining bytes to be  
    ; compared stored in RDX
    mov rcx,rdx

    ; we now compare the remainder bytes one byte at a time in downhill 
    ; (using STD) direction using REPE CMPSB
    std
    repe cmpsb
    ; If no mismatch occurred then goto clean up and exit the function
    je rmemcmp_out 
    
    ; Else we increment RDI to point to the byte that caused the mismatch 
    ; and replace the last item on the top of the stack with RDI
    inc rdi
    pop rax
    push rdi

    rmemcmp_out:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the byte
    ; causing the mismatch in RDI
    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   rmemcmp16: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in a downhill direction starting
;           from the the end of the memory regions and heading left towards 
;           the addresses stored in RDI and RSI for a number of two bytes equal  
;           to the value stored in RDX representing the length of two byte  
;           items comparisons.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer to the first two bytes 
;           causing dissimilarity from the end of the memory region pointed 
;           to by RDI.
;
;           The function is optimized so it compares 8-bytes at a time whenever
;           possible and the remaining bytes which will be less than 4 two  
;           bytes will be compared two bytes at a time.
;
;*****************************************************************************

rmemcmp16:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Check if the size of the memory to be compared is bigger than eight bytes
    ; in which case we need to do two byte-based comparison and skip optimization
    cmp rdx,4
    jg rmemcmp_normal16
    
    ; If we are here then the memory region size is smaller than 8 bytes 
    ; and hence we will compare two bytes at a time.

    ; Set loop counter register RCX to the number of two bytes to be compared 
    ; stored in RDX

    mov rcx,rdx

    ; Double RDX to represent the number of single bytes, and then decrement  
    ; RDX by 2 and add it to RDI and RSI so they point to the last two bytes
    ; in both memory regions subject to comparison.

    add rdx,rdx
    sub rdx,2
    add rdi,rdx
    add rsi,rdx

    ; we now compare the remainder bytes two bytes at a time in downhill 
    ; (using STD) direction using REPE CMPSW

    std
    repe cmpsw
    ; if no mismatch encountered adjust stack
    je rmemcmp_adjust16

    ; else reverse two bytes in RDI and RSI to identify the address of the two
    ; bytes that caused the mismatch, and push the address stored in RDI
    ; on the top of the stack and then jump to the stack cleant up and exit.
    add rdi,2
    add rsi,2
    push rdi
    jmp rmemcmp_out16

    ; push a NULL value on the top of the stack to be poped to RAX before exiting
    ; the function.

    rmemcmp_adjust16:
        xor rax,rax
        push rax
        ; Goto clean up and exit
        jmp rmemcmp_out16

    rmemcmp_normal16:
    ; If we are here then the size of the memory regions is larger than 8 bytes

    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.

    xor rax,rax
    push rax 

    ; Add RDX twice to RDI and RSI and decrement RDI and RSI by 8 so both point
    ; to the last eight bytes item in both memory buffers subject to comparison

    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    sub rdi,8
    sub rsi,8

    ; Divide the value in RDX (length of two bytes to be copied) by 4 to get  
    ; the number of eight byte uints in RAX and the remainder is then stored 
    ; in RDX. We divide by 4 because the length introduced in RDX is in terms 
    ; of bytes.

    mov rax,rdx
    xor rdx,rdx
    mov rcx,4
    div rcx
    ; RAX = number of eight byte units 
    ; RDX = number of remaining bytes

    ; Set loop counter register RCX to the number of eight bytes to be compared 
    ; stored in RAX

    mov rcx, rax

    ; we now compare the calculated eight byte items one byte at a time in  
    ; downhill (using STD) direction using REPE CMPSQ
    std
    repe cmpsq
    ; if now mismatch occurs proceed with comparing the remaining bytes if
    ; any is available

    je rmemcmp_match_until_now16

    ; if we are here then a mismacth occurred, and the mismatch occurred on an
    ; eight-bytes bases, so we do not know which two byte item caused the 
    ; mismatch so we backtrack one eight-bytes by adding 14 to RDI and RSI to
    ; point to the last two bytes of the eight byte items that caused the 
    ; mismatch and we also add 4 to RDX to increase the count of the remaining 
    ; two bytes

    add rdx,4
    add rdi,14
    add rsi,14

    rmemcmp_match_until_now16:

    ; Start comparing the remaining bytes
    ; Set loop counter register RCX to the number of remaining two bytes to be  
    ; compared stored in RDX

    mov rcx,rdx

    ; we now compare the remainder bytes one byte at a time in downhill 
    ; (using STD) direction using REPE CMPSW
    std
    repe cmpsw
    ; If no mismatch occurred then goto clean up and exit the function

    je rmemcmp_out16
    
    ; Else we increment RDI by 2 to point to the two bytes that caused the  
    ; mismatch and replace the last item on the top of the stack with RDI

    add rdi,2
    pop rax
    push rdi

    rmemcmp_out16:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the two byte
    ; causing the mismatch in RDI

    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi

ret

;*****************************************************************************
;   rmemcmp32: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in a downhill direction starting
;           from the the end of the memory regions and heading left towards 
;           the addresses stored in RDI and RSI for a number of four bytes equal  
;           to the value stored in RDX representing the length of four byte  
;           items comparisons.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer to the first four bytes 
;           causing dissimilarity from the end of the memory region pointed 
;           to by RDI.
;
;           The function is optimized so it compares 8-bytes at a time whenever
;           possible and the remaining bytes which will be less than 2 four  
;           bytes will be compared four bytes at a time.
;
;*****************************************************************************


rmemcmp32:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    
    ; Check if the size of the memory to be compared is bigger than eight bytes
    ; in which case we need to do four byte-based comparison and skip 
    ; optimization

    cmp rdx,2
    jg rmemcmp_normal32
    
    ; If we are here then the memory region size is smaller than 8 bytes 
    ; and hence we will compare four bytes at a time.

    ; Set loop counter register RCX to the number of four bytes to be compared 
    ; stored in RDX

    mov rcx,rdx

    ; Double double RDX to represent the number of single bytes, and then 
    ; decrement RDX  by 4 and add it to RDI and RSI so they point to the last 
    ; four bytes in both memory regions subject to comparison.

    add rdx,rdx
    add rdx,rdx
    sub rdx,4
    add rdi,rdx
    add rsi,rdx

    ; we now compare the remainder bytes two bytes at a time in downhill 
    ; (using STD) direction using REPE CMPSD

    std
    rep cmpsd
    ; if no mismatch encountered adjust stack
    je rmemcmp_adjust32

    ; else reverse four bytes in RDI and RSI to identify the address of the
    ; four bytes that caused the mismatch, and push the address stored in RDI
    ; on the top of the stack and then jump to the stack cleant up and exit.

    add rdi,4
    add rsi,4
    push rdi    
    jmp rmemcmp_out32

    ; push a NULL value on the top of the stack to be poped to RAX before 
    ; exiting the function.

    rmemcmp_adjust32:
        xor rax,rax
        push rax
        ; Goto clean up and exit
        jmp rmemcmp_out32

    rmemcmp_normal32:
    ; If we are here then the size of the memory regions is larger than 8 bytes

    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.

    xor rax,rax
    push rax 

    ; Add RDX four times to RDI and RSI and decrement RDI and RSI by 8 so both 
    ; point to the last eight bytes item in both memory buffers subject to 
    ; comparison

    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    sub rdi,8
    sub rsi,8

    ; Divide the value in RDX (length of four bytes to be copied) by 2 to get  
    ; the number of eight byte uints in RAX and the remainder is then stored 
    ; in RDX. we divide by 2 because the length introduced in RDX is in terms 
    ; of four bytes.

    mov rax,rdx
    xor rdx,rdx
    mov rcx,2
    div rcx

    ; RAX = number of eight byte units 
    ; RDX = number of remaining bytes

    ; Set loop counter register RCX to the number of eight bytes to be compared 
    ; stored in RAX

    mov rcx, rax

    ; we now compare the calculated eight byte items one byte at a time in  
    ; downhill (using STD) direction using REPE CMPSQ
    std
    repe cmpsq
    ; if now mismatch occurs proceed with comparing the remaining bytes if
    ; any is available

    je rmemcmp_match_until_now32

    ; if we are here then a mismacth occurred, and the mismatch occurred on an
    ; eight-bytes bases, so we do not know which four byte item caused the 
    ; mismatch so we backtrack one eight-bytes by adding 12 to RDI and RSI to
    ; point to the last four bytes of the eight byte items that caused the 
    ; mismatch and we also add 2 to RDX to increase the count of the remaining 
    ; four bytes

    add rdx,2
    add rdi,12
    add rsi,12

    rmemcmp_match_until_now32:

    ; Start comparing the remaining bytes
    ; Set loop counter register RCX to the number of remaining four bytes to be  
    ; compared stored in RDX

    mov rcx,rdx
    ; we now compare the remainder bytes one byte at a time in downhill 
    ; (using STD) direction using REPE CMPSD

    std
    repe cmpsd

    ; If no mismatch occurred then goto clean up and exit the function
    je rmemcmp_out32 
    
    ; Else we increment RDI by 4 to point to the four bytes that caused the  
    ; mismatch and replace the last item on the top of the stack with RDI

    add rdi,4
    pop rax
    push rdi

    rmemcmp_out32:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the four bytes
    ; causing the mismatch in RDI

    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi

ret


;*****************************************************************************
;   rmemcmp64: compares the content of two memory regions whose addresses are 
;           stored in RDI and RSI respectively, in a downhill direction starting
;           from the the end of the memory regions and heading left towards 
;           the addresses stored in RDI and RSI for a number of eight bytes   
;           equal to the value stored in RDX representing the length of eight   
;           byte items comparisons.
;
;           If the content of the two memory regions is identical the function 
;           returns NULL, else it returns a pointer to the first eight bytes 
;           causing dissimilarity from the end of the memory region pointed 
;           to by RDI.
;
;*****************************************************************************

rmemcmp64:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    
    ; zero out RAX and push it to the stack to be poped out
    ; into RAX for return before exiting the function in case of equality.

    xor rax,rax
    push rax

    ; Add RDX eight time  to RDI and RSI and decrement RDI and RSI by 8 so both 
    ; point to the last eight bytes item in both memory buffers subject to 
    ; comparison

    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx
    add rdi,rdx
    add rsi,rdx

    sub rdi,8
    sub rsi,8

     ; Set loop counter register RCX to the number of eight bytes to be compared 
    ; stored in RDX

    mov rcx,rdx

    ; we now compare the calculated eight byte items one byte at a time in  
    ; downhill (using STD) direction using REPE CMPSB
    std
    repe cmpsq

    ; if now mismatch occurs proceed with comparing the remaining bytes if
    ; any is available
    je rmemcmp_out64

    ; if we are here then a mismacth occurred, soi we reverse the RDI by adding
    ; 8 to it so it point to the eight byte item causing the mismatch and
    ; replace the top of the stack with the pointer value in RDI

    add rdi,8
    pop rax
    push rdi
 
    rmemcmp_out64:
    ; Set the RAX (return value) to the value stored on the top of the stack
    ; which should be NULL in case of a match or the address of the eight bytes
    ; causing the mismatch in RDI

    pop rax
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi

ret

;*****************************************************************************
;   scan8: scans the content of a memory region pointed to by RDI and of length
;           of the value stored in RDX for the byte value stored in RSI. 
;           The scan is done in uphill direction start from the address stored
;           in RDI for the number of bytes equal to RDX.
;
;           if the byte is found in the memory region the function returns
;           the address of the first occurance from the start of the memory
;           region of the byte in search, else it returns NULL
;
;           Note that we have overloaded the scan8 function with scanc for 
;           character memory comparison as both deal with memory bytes 
;           scan operation.
;*****************************************************************************


scanc:
scan8:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out RAX and store SI into AX to make sure that the higher order 6
    ; byes of RAX are zeroed out
    xor rax,rax
    mov ax,si
    ; zero out the AH register to make sure that only AL has teh byte value 
    ; needed for the scan.
    mov ah,0x0

    ; Set loop counter register RCX to the number of bytes to be scanned which
    ; is stored in RDX
   
    mov rcx, rdx        

    ; we now scan the bytes one byte at a time in uphill direction 
    ; (using CLD) using REPNE SCASB
  
    cld                     
    repne scasb
    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne scanb_failed

    ; If we are here this means that the byte in AL is found and hence we
    ; decrement RDI by one to point to the byte that matches AL and set RAX
    ; to the address in RDI as return value

    dec rdi
    mov rax, rdi       
        ; Goto clean up and exit
    jmp scanb_out

    ; if we are here then the byte scan failed and we need to set the value
    ; RAX to NULL as a return value
    scanb_failed:
        mov rax,0x0
        
    scanb_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret


;*****************************************************************************
;   scan16: scans the content of a memory region pointed to by RDI and of length
;           of two bytes  stored in RDX for the two byte value stored in RSI. 
;           The scan is done in uphill direction starting from the address 
;           stored in RDI for a number of two bytes equal to RDX.
;           
;           If the two bytes are found in the memory region the function returns
;           the address of the first occurance from the start of the memory
;           region of the two bytes, else it returns NULL
;
;*****************************************************************************

scan16:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out RAX and store SI into AX to make sure that the higher order 6
    ; byes of RAX are zeroed out
    xor rax,rax
    mov ax,si

    ; Set loop counter register RCX to the number of two bytes to be scanned 
    ; which is stored in RDX

    mov rcx, rdx         

    ; we now scan the bytes two byte at a time in uphill direction 
    ; (using CLD) using REPNE SCASW
    cld                     
    repne scasw

    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne scan16_failed

    ; If we are here this means that the two bytes in AX are found and hence 
    ; we decrement RDI by 2 to point to the two bytes that matches AX and set 
    ; RAX to the address in RDI as return value

    sub rdi,2
    mov rax, rdi

    ; Goto clean up and exit
    jmp scan16_out

    ; if we are here then the two bytes scan failed and we need to set the value
    ; RAX to NULL as a return value

    scan16_failed:
        mov rax,0x0
        
    scan16_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   scan32: scans the content of a memory region pointed to by RDI and of length
;           of four bytes stored in RDX for the four byte value stored in RSI. 
;           The scan is done in uphill direction starting from the address 
;           stored in RDI for a number of four bytes equal to RDX.
;           
;           If the four bytes are found in the memory region the function 
;           returns the address of the first occurance from the start of the 
;           memory region of the four bytes, else it returns NULL
;
;*****************************************************************************

scan32:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out RAX and store ESI into EAX to make sure that the higher order 4
    ; byes of RAX are zeroed out
    xor rax,rax
    mov eax,esi

    ; Set loop counter register RCX to the number of four bytes to be scanned 
    ; which is stored in RDX

    mov rcx, rdx

    ; we now scan the bytes, four byte at a time, in uphill direction 
    ; (using CLD) using REPNE SCASD
    
    cld
    repne scasd

    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne scan32_failed

    ; If we are here this means that the four bytes in EAX are found and hence 
    ; we decrement RDI by 4 to point to the four bytes that matches EAX and set 
    ; RAX to the address in RDI as return value

    sub rdi,4
    mov rax, rdi

    ; Goto clean up and exit
    jmp scan32_out

    ; if we are here then the four bytes scan failed and we need to set the 
    ; value RAX to NULL as a return value

    scan32_failed:
        mov rax,0x0
        
    scan32_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret


;*****************************************************************************
;   scan64: scans the content of a memory region pointed to by RDI and of length
;           of eight bytes stored in RDX for the eight byte value stored in RSI. 
;           The scan is done in uphill direction starting from the address 
;           stored in RDI for a number of eight bytes equal to RDX.
;           
;           If the eight bytes are found in the memory region the function 
;           returns the address of the first occurance from the start of the 
;           memory region of the eight bytes, else it returns NULL
;
;*****************************************************************************


scan64:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Assign RSI to RAX for the search
    mov rax,rsi

    ; Set loop counter register RCX to the number of four bytes to be scanned 
    ; which is stored in RDX
    mov rcx, rdx 

    ; we now scan the bytes, four byte at a time, in uphill direction 
    ; (using CLD) using REPNE SCASD
    cld 
    repne scasq 

    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne scan64_failed

    ; If we are here this means that the four bytes in EAX are found and hence 
    ; we decrement RDI by 4 to point to the four bytes that matches EAX and set 
    ; RAX to the address in RDI as return value

    sub rdi,8
    mov rax, rdi

    ; Goto clean up and exit
    jmp scan64_out

    ; if we are here then the four bytes scan failed and we need to set the 
    ; value RAX to NULL as a return value
    scan64_failed:
        mov rax,0x0
        
    scan64_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   rscan8: scans the content of a memory region pointed to by RDI and of 
;           length of the value stored in RDX for the byte value stored in RSI. 
;           The scan is done in downhill direction starting from the address 
;           equivelant to  RDI+RDX for the number of bytes equal to RDX.
;
;           If the byte is found in the memory region the function returns
;           the address of the first occurance from the end of the memory
;           region of the byte in search, else it returns NULL
;
;           Note that we have overloaded the rscan8 function with rscanc for 
;           character memory comparison as both deal with memory bytes 
;           scan operation.
;*****************************************************************************


rscanc:
rscan8:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Zero out RAX and store SI into AX to make sure that the higher order 6
    ; byes of RAX are zeroed out
    xor rax,rax
    mov ax,si
    ; zero out the AH register to make sure that only AL has teh byte value 
    ; needed for the scan.
    mov ah,0x0


    ; Add RDX to RDI and decrement it by 1 so that RDI points to the last
    ; element in the memory region
    add rdi,rdx
    dec rdi

    ; Set loop counter register RCX to the number of bytes to be scanned which
    ; is stored in RDX
    mov rcx, rdx
    ; we now scan the bytes one byte at a time in downhill direction 
    ; (using STD) using REPNE SCASB
    std
    repne scasb
    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne rscanb_failed

    ; If we are here this means that the byte in AL is found and hence we
    ; decrement RDI by one to point to the byte that matches AL and set RAX
    ; to the address in RDI as return value

    inc rdi
    mov rax, rdi
    ; Goto clean up and exit
    jmp rscanb_out

    ; if we are here then the byte scan failed and we need to set the value
    ; RAX to NULL as a return value
    rscanb_failed:
        mov rax,0x0
        
    rscanb_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   rscan16: scans the content of a memory region pointed to by RDI and of 
;           length of the value stored in RDX for the two byte value stored 
;           in RSI. The scan is done in downhill direction starting from the  
;           address equivelant to RDI+(RDX*2) for the number of two bytes equal 
;           to RDX.
;
;           If the two bytes are found in the memory region the function returns
;           the address of the first occurance from the end of the memory
;           region of the two bytes in search, else it returns NULL
;
;*****************************************************************************

rscan16:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    ; Zero out RAX and store SI into AX to make sure that the higher order 6
    ; byes of RAX are zeroed out
    xor rax,rax
    mov ax,si

    ; Add RDX twice to RDI and decrement it by 2 so that RDI points to the last
    ; element in the memory region

    add rdi,rdx
    add rdi,rdx
    sub rdi,2

    ; Set loop counter register RCX to the number of bytes to be scanned which
    ; is stored in RDX

    mov rcx, rdx

    ; we now scan the bytes two bytes at a time in downhill direction 
    ; (using STD) using REPNE SCASW

    std
    repne scasw
    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne rscan16_failed

    ; If we are here this means that the two bytes in AX are found and hence we
    ; decrement RDI by two to point to the two bytes that matches AX and set RAX
    ; to the address in RDI as return value

    add rdi,2
    mov rax, rdi
    ; Goto clean up and exit
    jmp rscan16_out

    ; if we are here then the byte scan failed and we need to set the value
    ; RAX to NULL as a return value
    rscan16_failed:
        mov rax,0x0
        
    rscan16_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   rscan32: scans the content of a memory region pointed to by RDI and of 
;           length of the value stored in RDX for the four byte value stored 
;           in RSI. The scan is done in downhill direction starting from the  
;           address equivelant to RDI+(RDX*4) for the number of four bytes  
;           equal to RDX.
;
;           If the four bytes are found in the memory region the function 
;           returns the address of the first occurance from the end of the 
;           memory region of the four bytes in search, else it returns NULL
;
;*****************************************************************************

rscan32:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx
    ; Zero out RAX and store ESI into EAX to make sure that the higher order 4
    ; byes of RAX are zeroed out
    xor rax,rax
    mov eax,esi

    ; Add RDX four times to RDI and decrement it by 4 so that RDI points to the 
    ; last element in the memory region

    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    sub rdi,4

    ; Set loop counter register RCX to the number of bytes to be scanned which
    ; is stored in RDX
    mov rcx, rdx

    ; we now scan the bytes four bytes at a time in downhill direction 
    ; (using STD) using REPNE SCASD
    std
    repne scasd
    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne rscan32_failed

    ; If we are here this means that the four bytes in EAX is found and hence we
    ; decrement RDI by four to point to the four bytes that matches EAX and set 
    ; RAX to the address in RDI as return value

    add rdi,4
    mov rax, rdi
    ; Goto clean up and exit
    jmp rscan32_out

    ; if we are here then the byte scan failed and we need to set the value
    ; RAX to NULL as a return value
    rscan32_failed:
        mov rax,0x0
        
    rscan32_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret

;*****************************************************************************
;   rscan64: scans the content of a memory region pointed to by RDI and of 
;           length of the value stored in RDX for the eight byte value stored 
;           in RSI. The scan is done in downhill direction starting from the  
;           address equivelant to RDI+(RDX*8) for the number of eight bytes  
;           equal to RDX.
;
;           If the eight bytes are found in the memory region the function 
;           returns the address of the first occurance from the end of the 
;           memory region of the eight bytes in search, else it returns NULL
;
;*****************************************************************************


rscan64:

    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; Assign RSI to RAX for the search

    mov rax,rsi

    ; Add RDX eight times to RDI and decrement it by 8 so that RDI points to the 
    ; last element in the memory region

    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    add rdi,rdx
    sub rdi,8

    ; Set loop counter register RCX to the number of bytes to be scanned which
    ; is stored in RDX
    mov rcx, rdx

    ; we now scan the bytes eight bytes at a time in downhill direction 
    ; (using STD) using REPNE SCASQ
    std
    repne scasq
    ; if scan stops check if it stopped for mismatch and hence the scan failed
    ; and we jump to where we can handle the failed scan 
    jne rscan64_failed

    ; If we are here this means that the eight bytes in RAX is found and hence 
    ; we decrement RDI by four to point to the eight bytes that matches RAX and  
    ; set RAX to the address in RDI as return value

    add rdi,8
    mov rax, rdi
    ; Goto clean up and exit
    jmp rscan64_out

    ; if we are here then the byte scan failed and we need to set the value
    ; RAX to NULL as a return value
    rscan64_failed:
        mov rax,0x0
        
    rscan64_out:
    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret


;*****************************************************************************
;   strlen: scans the content of a memory region pointed to by RDI for the
;           ocurrenc of a NULL characher, and returns the address of the NULL
;           character with respect to the value in RDI which represnts the
;           length of the NULL termintanted string pointed to by RDI
;
;
;*****************************************************************************


strlen:
    ; Reserve registers by pushing them into the stack
    push rsi
    push rdi
    push rdx
    push rcx

    ; set AL to NULL to be used by the search
    mov al, 0x0
    ; set Loop counter RCX to the largest value possible (2^64)-1
    mov rcx, -1
    ; clear the loop direction to incremnt RDI as the loop runs
    cld
    ;scan while RDI points to a byte that is not NULL
    repne scasb    
    ;set RAX to (2^64)-1
    mov rax, -2         
    ; when the NULL charcter is found RCX will conatin ((2^64)-2) - number of
    ; bytes visited execluding the NULL byte (that is why we subtract 2)
    ; if we subtract the value in RCX from RAX,we end up by the lenght
    ; of the NULL trerminated string pointed to by RDI which is the return 
    ; value stored in the return register RAX
    sub rax, rcx

    ; Restore registers values by poping from the stack
    pop rcx
    pop rdx
    pop rdi
    pop rsi
ret


EQUAL_ANY	equ 0000b
RANGES		 equ 0100b
EQUAL_EACH	    equ 1000b
EQUAL_ORDERED	    equ 1100b
NEGATIVE_POLARITY equ 010000b
BYTE_MASK	 equ 1000000b

strcmp_sse42:
  ; Using __fastcall convention, rdi = string1, rsi = string2
  mov rcx,rdi
  mov rdx,rsi
  mov rax, rcx
  sub rax, rdx ; eax = ecx - edx
  sub rdx, 16

STRCMP_LOOP:
    add rdx,16
    movdqu xmm0,[rdx] ; find the first *different* bytes, hence negative polarity
    pcmpistri xmm0, [rdx + rax], EQUAL_EACH + NEGATIVE_POLARITY
    ja STRCMP_LOOP

  jc STRCMP_DIFF

  ; the strings are equal
  xor rax, rax
  ret
STRCMP_DIFF:
  ; subtract the first different bytes
  add rax, rdx
  movzx rax, byte[rax + rcx]
  movzx rdx, byte[rdx + rcx]
  sub rax, rdx
ret


strlen_sse42:
  mov rcx,rdi
  mov rax, -16
  mov rdx, rcx
  pxor xmm0, xmm0

STRLEN_LOOP:
    add eax, 16
    pcmpistri xmm0, [rdx + rax], EQUAL_EACH
    jnz STRLEN_LOOP

  add rax, rcx
  ret