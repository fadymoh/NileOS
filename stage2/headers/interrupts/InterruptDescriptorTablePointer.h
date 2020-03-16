#ifndef INTERRUPTDESCRIPTORTABLEPOINTER_H_
#define INTERRUPTDESCRIPTORTABLEPOINTER_H_

struct InterruptDescriptorTablePointer_s
{
        uint16_t limit;     // IDT Size Limit
        uint64_t base;      // IDT Base Address
        uint32_t x;
        uint16_t y;
}__attribute__ ((__packed__));
typedef struct InterruptDescriptorTablePointer_s InterruptDescriptorTablePointer;


#endif /* INTERRUPTDESCRIPTORTABLEPOINTER_H_ */ 
