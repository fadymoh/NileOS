#ifndef INTERRUPTCONTEXT_H_
#define INTERRUPTCONTEXT_H_

#include "includes.h"

typedef struct InterruptContext_s
{
        // taking a snapshot of the regitsers before the interrupts. 
        uint64_t r15,r14,r13,r12,r11,r10,r9,r8,rdi, rsi, rbp , rdx, rcx, rbx, rax;
        // Interrupt number and error code
        uint64_t interrupt_number, error_code;
        // The 5 items stored in the stack upon interrupt and which should be poped by the iretq
        uint64_t return_address, cs, eflags, rsp, ss;
}InterruptContext;

#endif