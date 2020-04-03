#ifndef INTERRUPTDESCRIPTORTABLE_H_
#define INTERRUPTDESCRIPTORTABLE_H_

#include "InterruptDescriptorTableEntry.h"
#include "InterruptDescriptorTablePointer.h"
#include "isr_stub.h"

#define IDT_SIZE 256 // Size of the interrupt descriptor table

struct InterruptDescriptorTable_s
{
        // An array of 256 IDT Entires.
        InterruptDescriptorTableEntry interruptDescriptorTableEntries[IDT_SIZE];
        // A Pointer to InterruptDescriptorTablePointer which is used for loading
        // the table via the lidt assembly instruction
};

typedef struct InterruptDescriptorTable_s InterruptDescriptorTable;

void IrqSetMask(uint8_t p_irq_line);
void IrqClearMask(uint8_t p_irq_line);

void SetInterruptDescriptorTableEntry(uint8_t p_number, uint64_t p_base, uint16_t p_selector, uint8_t p_flags);

void SetupInterruptDescriptorTable();
#endif /* INTERRUPTDESCRIPTORTABLE_H_ */
