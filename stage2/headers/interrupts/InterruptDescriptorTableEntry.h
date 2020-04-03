#ifndef INTERRUPTDESCRIPTORTABLEENTRY_H_
#define INTERRUPTDESCRIPTORTABLEENTRY_H_

#include "includes.h"

struct InterruptDescriptorTableEntry_s
{
       uint16_t base_low;    // The lower 16 bits of the address of the interrupt handler routine
       uint16_t selector;    // The kernel segment selector
       uint8_t reserved_ist; // Reserved 8-bit that should be always 0
       uint8_t flags;        // Interrupt flags
       uint16_t base_mid;    // The middle 16 bits of the address of the interrupt handler routine
       uint32_t base_high;   // The higher 32 bits of the address of the interrupt handler routine
       uint32_t reserved;    //  Reserved 32-bits
} __attribute__((packed));

typedef struct InterruptDescriptorTableEntry_s InterruptDescriptorTableEntry;

#endif /* INTERRUPTDESCRIPTORTABLEENTRY_H_ */
