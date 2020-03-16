#include "InterruptDescriptorTable.h"
#include "Kernel.h"
extern Kernel kernel;

extern InterruptDescriptorTablePointer interruptDescriptorTablePointer;

// External declaration for the global InterruptDescriptorTablePointer
// which is declared a a global variable and initialized in the
// kernel main program
// External declaration of the idtInit assembly function that loads the IDTextern

extern void idtInit();

/* void irqSetMask(uint8_t p_irq_line)
 * 
 * This method set a mask on a PIC IRQ line, which disables the IRQ line from
 * firing interrupts
 * 
 * This method receives one parameter:
 *      uint8_t p_irq_line: the IRQ line number to set mask for.
 * 
 * 
 */

//telling the pic which IRQ line to fire the interrupts on.

void irqSetMask(uint8_t p_irq_line)
{
    uint16_t port;
    uint8_t value;

    // Check the p_irq_line and exit the method if larger than 15 (PIC has 16 lines)
    if (p_irq_line > 15)
        return;
    // If p_irq_line then we need to use the master port
    if (p_irq_line < 8)
    {
        port = PIC1_DATA;
    }
    else
    { // else we use the salve port and subtract 8 to get the absolute IRQ line number
        port = PIC2_DATA;
        p_irq_line -= 8;
    }
    // Read the value on the port and set the p_irq_line bit
    value = inportb(port) | (1 << p_irq_line);
    // Write back the new value with the bit mask set
    outportb(port, value);
}

/* void irqClearMask(uint8_t p_irq_line)
 * 
 * This method clear a mask on a PIC IRQ line, which enables the IRQ line to
 * fire interrupts
 * 
 * This method receives one parameter:
 *      uint8_t p_irq_line: the IRQ line number to clear mask for.
 * 
 * 
 */

void irqClearMask(uint8_t p_irq_line)
{
    uint16_t port;
    uint8_t value;
    // Check the p_irq_line and exit the method if larger than 15 ( PIC has 16 lines)
    if (p_irq_line > 15)
        return;
    // If p_irq_line then we need to use the master port
    if (p_irq_line < 8)
    {
        port = PIC1_DATA;
    }
    else
    { // else we use the salve port and subtract 8 to get the absolute IRQ line number
        port = PIC2_DATA;
        p_irq_line -= 8;
    }
    // Read the value on the port and clears the p_irq_line bit
    value = inportb(port) & ~(1 << p_irq_line);
    // Write back the new value with the bit mask set
    outportb(port, value);
}

/* void InterruptDescriptorTable::setInterruptDescriptorTableEntry (uint8_t p_number, 
 *                                                                  uint64_t p_base, 
 *                                                                  uint16_t p_selector, 
 *                                                                  uint8_t p_flags)
 * 
 * This method initializes an IDT entry, and it receives the following 4 parameters:
 *      uint8_t p_number: The index of the IDT entry to be initialized
 *      uint64_t p_base: The address of the function to be invoked as the interrupt handler
 *      uint16_t p_selector: The code segment selector in the GDT
 *      uint8_t p_flags: the type of the interrupt handler 
 *              The higher 4 bits are as follows:
 *                  P   (1 bit) : Present
 *                  DPL (2 bits): Descriptor Privilege Level  //telling who can use this interrupt (usermode vs kernel mode)
 *                  S   (1 bit) : Storage Segment ( =0 for interrupt gates)
 *              The lower 4 bits defines the gate type:
 *                  task gate: 0x5
 *                  interrupt gate: 0xE
 *                  trap gate: 0xF
 *
 */

//START FROM HEREE!! ANDY

void setInterruptDescriptorTableEntry(uint8_t p_number, uint64_t p_base, uint16_t p_selector, uint8_t p_flags)
{
    // Set the base address
    // Store lower 16 bits of lower 32 bits
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].base_low = p_base & 0xFFFF; //anding the last 16 bits with 1 and rest and zeros
    // Store higher 16 bits of lower 32 bits
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].base_mid = (p_base >> 16) & 0xFFFF; //get the next 16 as prev
    // Store higher 32 bits
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].base_high = (p_base >> 32) & 0xFFFFFFFF; //get the final 32

    // Store Selector //! msh fahmeeenooo
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].selector = p_selector;
    // Zero out reserved field
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].reserved_ist = 0;
    // Store flags
    //! always 0x8E, 1: present, 00: DPL, 0: S (for interrupt), 1110: Interrupt Gate
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].flags = p_flags;
    // Zero out reserved field
    kernel.interruptDescriptorTable.interruptDescriptorTableEntries[p_number].reserved = 0;
}

/* void registerExceptions()
 * 
 * Register 32 exception handlers into the IDT on entries from 0 - 31.
 * 
 * All the entries have a selector equal to 0x8 (KERNEL_CODESEG) which the kernel code segment.
 * The flag of the entries are 0x8E (IDT_P_KERNEL_INTERRUPT_GATE = 10001110) :
 *          Present Bit (bit 7) : 1
 *          DPL Bits (bit 5-6) : 00
 *          Storage Bit (bit 4) : 0
 *          Type Bits (bit 0-3) : 1110
 * 
 * The assigned interrupt handlers isr0-31 are defined in asm/stage2/isr.asm and stage2/int/isr.h
 * 
 */

void registerExceptions()
{
    setInterruptDescriptorTableEntry(0, (uint64_t)isr0, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(1, (uint64_t)isr1, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(2, (uint64_t)isr2, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(3, (uint64_t)isr3, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(4, (uint64_t)isr4, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(5, (uint64_t)isr5, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(6, (uint64_t)isr6, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(7, (uint64_t)isr7, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(8, (uint64_t)isr8, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(9, (uint64_t)isr9, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(10, (uint64_t)isr10, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(11, (uint64_t)isr11, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(12, (uint64_t)isr12, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(13, (uint64_t)isr13, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(14, (uint64_t)isr14, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(15, (uint64_t)isr15, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(16, (uint64_t)isr16, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(17, (uint64_t)isr17, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(18, (uint64_t)isr18, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(19, (uint64_t)isr19, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(20, (uint64_t)isr20, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(21, (uint64_t)isr21, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(22, (uint64_t)isr22, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(23, (uint64_t)isr23, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(24, (uint64_t)isr24, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(25, (uint64_t)isr25, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(26, (uint64_t)isr26, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(27, (uint64_t)isr27, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(28, (uint64_t)isr28, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(29, (uint64_t)isr29, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(30, (uint64_t)isr30, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(31, (uint64_t)isr31, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
}

/* void registerIRQs()
 * 
 * Register 64 IRQ handlers into the IDT on entries from 32 - 95. 
 * 16 of which are PIC IRQ Handlers
 * 
 * All the entries have a selector equal to 0x8 (KERNEL_CODESEG) which the kernel code segment.
 * The flag of the entries are 0x8E (IDT_P_KERNEL_INTERRUPT_GATE = 10001110) :
 *          Present Bit (bit 7) : 1
 *          DPL Bits (bit 5-6) : 00
 *          Storage Bit (bit 4) : 0
 *          Type Bits (bit 0-3) : 1110
 * 
 * The assigned interrupt handlers irq0-63 are defined in asm/stage2/isr.asm and stage2/int/isr.h
 * 
 */

void registerIRQs()
{
    setInterruptDescriptorTableEntry(32, (uint64_t)irq0, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(33, (uint64_t)irq1, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(34, (uint64_t)irq2, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(35, (uint64_t)irq3, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(37, (uint64_t)irq5, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(38, (uint64_t)irq6, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(39, (uint64_t)irq7, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(40, (uint64_t)irq8, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(41, (uint64_t)irq9, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(42, (uint64_t)irq10, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(43, (uint64_t)irq11, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(44, (uint64_t)irq12, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(45, (uint64_t)irq13, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(46, (uint64_t)irq14, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(47, (uint64_t)irq15, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(48, (uint64_t)irq16, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(49, (uint64_t)irq17, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(50, (uint64_t)irq18, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(51, (uint64_t)irq19, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(52, (uint64_t)irq20, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(53, (uint64_t)irq21, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(54, (uint64_t)irq22, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(55, (uint64_t)irq22, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(56, (uint64_t)irq23, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(57, (uint64_t)irq24, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(58, (uint64_t)irq25, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(59, (uint64_t)irq26, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(60, (uint64_t)irq27, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(61, (uint64_t)irq28, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(62, (uint64_t)irq29, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(63, (uint64_t)irq31, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(64, (uint64_t)irq32, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(65, (uint64_t)irq33, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(66, (uint64_t)irq34, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(67, (uint64_t)irq35, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(68, (uint64_t)irq36, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(69, (uint64_t)irq37, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(70, (uint64_t)irq38, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(71, (uint64_t)irq39, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(72, (uint64_t)irq40, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(73, (uint64_t)irq41, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(74, (uint64_t)irq42, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(75, (uint64_t)irq43, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(76, (uint64_t)irq44, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(77, (uint64_t)irq45, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(78, (uint64_t)irq46, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(79, (uint64_t)irq47, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(80, (uint64_t)irq48, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(81, (uint64_t)irq49, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(82, (uint64_t)irq50, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(83, (uint64_t)irq51, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(84, (uint64_t)irq52, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(85, (uint64_t)irq53, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(86, (uint64_t)irq54, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(87, (uint64_t)irq55, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(88, (uint64_t)irq56, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(89, (uint64_t)irq57, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(90, (uint64_t)irq58, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(91, (uint64_t)irq59, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(92, (uint64_t)irq60, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(93, (uint64_t)irq61, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(94, (uint64_t)irq62, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(95, (uint64_t)irq63, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
}

/* void registerAPIRQs()
 * 
 * Register 32 Application Processor (AP) handlers into the IDT on entries from 96 - 127 to be used
 * by IPIs (Inter-Processor Interrupts) for inter-processor communication
 * 
 * All the entries have a selector equal to 0x8 (KERNEL_CODESEG) which the kernel code segment.
 * The flag of the entries are 0x8E (IDT_P_KERNEL_INTERRUPT_GATE = 10001110) :
 *          Present Bit (bit 7) : 1
 *          DPL Bits (bit 5-6) : 00
 *          Storage Bit (bit 4) : 0
 *          Type Bits (bit 0-3) : 1110
 * 
 * The assigned interrupt handlers apirq0-31 are defined in asm/stage2/isr.asm and stage2/int/isr.h
 * 
 */

void registerAPIRQs()
{
    setInterruptDescriptorTableEntry(96, (uint64_t)apirq0, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(97, (uint64_t)apirq1, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(98, (uint64_t)apirq2, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(99, (uint64_t)apirq3, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(100, (uint64_t)apirq4, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(101, (uint64_t)apirq5, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(102, (uint64_t)apirq6, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(103, (uint64_t)apirq7, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(104, (uint64_t)apirq8, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(105, (uint64_t)apirq9, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(106, (uint64_t)apirq10, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(107, (uint64_t)apirq11, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(108, (uint64_t)apirq12, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(109, (uint64_t)apirq13, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(110, (uint64_t)apirq14, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(111, (uint64_t)apirq15, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(112, (uint64_t)apirq16, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(113, (uint64_t)apirq17, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(114, (uint64_t)apirq18, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(115, (uint64_t)apirq19, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(116, (uint64_t)apirq20, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(117, (uint64_t)apirq21, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(118, (uint64_t)apirq22, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(119, (uint64_t)apirq23, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(120, (uint64_t)apirq24, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(121, (uint64_t)apirq25, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(122, (uint64_t)apirq26, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(123, (uint64_t)apirq27, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(124, (uint64_t)apirq28, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(125, (uint64_t)apirq29, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(126, (uint64_t)apirq30, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(127, (uint64_t)apirq31, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);
}

/* void registerSWIRQs()
 * 
 * Register 127 software interrupt handlers into the IDT on entries from 128 - 254 to be used
 * in implementing different system call types
 * 
 * All the entries have a selector equal to 0x8 (KERNEL_CODESEG) which the kernel code segment.
 * The flag of the entries are 0xEE (IDT_P_KERNEL_INTERRUPT_GATE = 11101110) :
 *          Present Bit (bit 7) : 1
 *          DPL Bits (bit 5-6) : 11 -> Notice the value of the DPL as this interrupt will be invoked 
 *                                  from th user space.
 *          Storage Bit (bit 4) : 0
 *          Type Bits (bit 0-3) : 1110
 * 
 * The assigned interrupt handlers swirq0-126 are defined in asm/stage2/isr.asm and stage2/int/isr.h
 * 
 */
//! why is it an interrupt gate not trap gate for user mode amd why kernel code seg if user
//! also why always used not unused not all present in the beginning
void registerSWIRQs()
{
    setInterruptDescriptorTableEntry(128, (uint64_t)swirq0, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(129, (uint64_t)swirq1, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(130, (uint64_t)swirq2, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(131, (uint64_t)swirq3, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(132, (uint64_t)swirq5, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(133, (uint64_t)swirq6, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(134, (uint64_t)swirq7, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(135, (uint64_t)swirq8, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(136, (uint64_t)swirq9, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(137, (uint64_t)swirq10, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(138, (uint64_t)swirq11, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(140, (uint64_t)swirq12, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(141, (uint64_t)swirq13, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(142, (uint64_t)swirq14, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(143, (uint64_t)swirq15, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(144, (uint64_t)swirq16, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(145, (uint64_t)swirq17, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(146, (uint64_t)swirq18, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(147, (uint64_t)swirq19, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(148, (uint64_t)swirq20, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(149, (uint64_t)swirq21, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(150, (uint64_t)swirq22, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(151, (uint64_t)swirq22, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(152, (uint64_t)swirq23, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(153, (uint64_t)swirq24, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(154, (uint64_t)swirq25, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(155, (uint64_t)swirq26, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(156, (uint64_t)swirq27, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(157, (uint64_t)swirq28, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(158, (uint64_t)swirq29, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(159, (uint64_t)swirq31, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(160, (uint64_t)swirq32, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(161, (uint64_t)swirq33, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(162, (uint64_t)swirq34, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(163, (uint64_t)swirq35, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(164, (uint64_t)swirq36, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(165, (uint64_t)swirq37, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(166, (uint64_t)swirq38, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(167, (uint64_t)swirq39, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(168, (uint64_t)swirq40, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(169, (uint64_t)swirq41, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(170, (uint64_t)swirq42, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(171, (uint64_t)swirq43, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(172, (uint64_t)swirq44, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(173, (uint64_t)swirq45, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(174, (uint64_t)swirq46, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(175, (uint64_t)swirq47, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(176, (uint64_t)swirq48, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(177, (uint64_t)swirq49, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(178, (uint64_t)swirq50, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(179, (uint64_t)swirq51, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(180, (uint64_t)swirq52, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(181, (uint64_t)swirq53, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(182, (uint64_t)swirq54, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(183, (uint64_t)swirq55, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(184, (uint64_t)swirq56, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(185, (uint64_t)swirq57, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(186, (uint64_t)swirq58, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(187, (uint64_t)swirq59, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(188, (uint64_t)swirq60, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(189, (uint64_t)swirq61, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(190, (uint64_t)swirq62, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(191, (uint64_t)swirq63, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(192, (uint64_t)swirq64, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(193, (uint64_t)swirq65, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(194, (uint64_t)swirq66, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(195, (uint64_t)swirq67, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(196, (uint64_t)swirq68, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(197, (uint64_t)swirq69, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(198, (uint64_t)swirq70, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(199, (uint64_t)swirq71, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(200, (uint64_t)swirq72, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(201, (uint64_t)swirq73, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(202, (uint64_t)swirq74, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(203, (uint64_t)swirq75, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(204, (uint64_t)swirq76, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(205, (uint64_t)swirq77, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(206, (uint64_t)swirq78, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(207, (uint64_t)swirq79, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(208, (uint64_t)swirq80, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(209, (uint64_t)swirq81, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(210, (uint64_t)swirq82, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(211, (uint64_t)swirq83, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(212, (uint64_t)swirq84, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(213, (uint64_t)swirq85, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(214, (uint64_t)swirq86, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(215, (uint64_t)swirq87, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(216, (uint64_t)swirq88, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(217, (uint64_t)swirq89, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(218, (uint64_t)swirq90, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(219, (uint64_t)swirq91, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(220, (uint64_t)swirq92, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(221, (uint64_t)swirq93, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(222, (uint64_t)swirq94, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(223, (uint64_t)swirq95, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(224, (uint64_t)swirq96, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(225, (uint64_t)swirq97, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(226, (uint64_t)swirq98, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(227, (uint64_t)swirq99, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(228, (uint64_t)swirq100, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(229, (uint64_t)swirq101, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(230, (uint64_t)swirq102, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(231, (uint64_t)swirq103, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(232, (uint64_t)swirq104, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(233, (uint64_t)swirq105, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(234, (uint64_t)swirq106, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(235, (uint64_t)swirq107, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(236, (uint64_t)swirq108, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(237, (uint64_t)swirq109, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(238, (uint64_t)swirq110, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(239, (uint64_t)swirq111, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(240, (uint64_t)swirq112, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(241, (uint64_t)swirq113, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(242, (uint64_t)swirq114, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(243, (uint64_t)swirq115, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(244, (uint64_t)swirq116, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(245, (uint64_t)swirq117, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(246, (uint64_t)swirq118, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(247, (uint64_t)swirq119, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(248, (uint64_t)swirq120, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(249, (uint64_t)swirq121, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(250, (uint64_t)swirq122, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(251, (uint64_t)swirq123, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(252, (uint64_t)swirq124, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(253, (uint64_t)swirq125, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(254, (uint64_t)swirq126, KERNEL_CODESEG, IDT_P_USER_INTERRUPT_GATE);
}

/* void setupInterruptDescriptorTable(bool p_init_pic)
 * 
 * This method performs initialization tasks for the IDT and the enablement of the 
 * the PIC.
 * 
 */

void setupInterruptDescriptorTable()
{
    // Disable all interrupts (cli)
    disableInterrupts();

    outportb(PIC2_DATA, 0xff);
    outportb(PIC1_DATA, 0xff);

    // Load Exceptions Interrupt Handlers
    registerExceptions();
    // Load IRQ Interrupt Handlers
    registerIRQs();
    // Load AP IPI Interrupt Handlers
    registerAPIRQs();
    // Load Software Interrupt Handlers
    registerSWIRQs();
    // register interrupt 255
    setInterruptDescriptorTableEntry(255, (uint64_t)isr255, KERNEL_CODESEG, IDT_P_KERNEL_INTERRUPT_GATE);

    // Assign the configured interruptDescriptorTablePointer data member of the InterruptDescriptorTable
    // To the global interruptDescriptorTablePointer to be loaded by idtInit())

    interruptDescriptorTablePointer.limit = (sizeof(InterruptDescriptorTableEntry) * IDT_SIZE) - 1;
    interruptDescriptorTablePointer.base = (uint64_t) & (kernel.interruptDescriptorTable.interruptDescriptorTableEntries);
    // Load IDT
    idtInit();

    // Clear Interrupt Mask for IRQ 2 & 7 to allow enable interrupt cascade
    // Without this interrupts on the PIC slave will not reach the CPU and its
    // interrupts will not fire
    // if ( p_init_pic)
    // {
    //     irqClearMask(IRQ2-IRQ0);
    //     irqClearMask(IRQ7-IRQ0);
    // }
}