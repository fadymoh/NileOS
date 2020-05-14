#include "GlobalDescriptorTable.h"
#include "Kernel.h"

extern Kernel kernel;

void initGlobalDescriptorTable()
{
    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);

    uint16_t apic_id = kernel.apicManager.returns.core_id;
    // Sets the limit and the base of the globalDescriptorTablePointer.
    // The limit is the size of the GDT table and the base of the memory address
    // of the GlobalDescriptorTable object
    // printk("inside IGDTP: %x\n", kernel.globalDescriptorTable[apic_id]);

    kernel.globalDescriptorTablePointer->limit = (sizeof(GlobalDescriptorTableEntry) * GDT_SIZE) - 1;
    kernel.globalDescriptorTablePointer->base = (uintptr_t)&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries;
    // Initializing the globalDescriptorTableEntries array memory bytes to zeros
    memset((uint8_t *)&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries, 0, sizeof(GlobalDescriptorTableEntry) * GDT_SIZE);

    // Defining the first NULL entry for the GDT descriptor
    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // Defining the GDNT Kernel mode Code Segment Descriptor
    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[1], 0xffffU, 0, 0, AR_PRESENT | AR_CODE | DPL_KERNEL | AR_READABLE, 0x0fU, 0, 1, 0, 1, 0);
    // Defining the GDNT Kernel mode Data Segment Descriptor
    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[2], 0xffffU, 0, 0, AR_PRESENT | AR_DATA | DPL_KERNEL | AR_WRITABLE, 0x0fU, 0, 0, 1, 1, 0);
    // Defining the GDNT user mode Code Segment Descriptor

    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[3], 0xffffU, 0, 0, AR_PRESENT | AR_CODE | DPL_USER | AR_READABLE, 0x0fU, 0, 1, 0, 1, 0);

    // Defining the GDNT user mode Data Segment Descriptor
    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[4], 0xffffU, 0, 0, AR_PRESENT | AR_DATA | DPL_USER | AR_READABLE, 0x0fU, 0, 0, 1, 1, 0);

    // Defining two NULL descriptor for the Kernel TSS
    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[5], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    setGlobalDescriptorTableEntry(&kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[6], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // pointing the TSS Descriptor pointer to the 5th element of the GDT
    memset(&(kernel.globalDescriptorTable[apic_id].tssEntry), 0, sizeof(TSSEntry));
    kernel.globalDescriptorTable[apic_id].tssDescriptor = (TSSDescriptor *)&(kernel.globalDescriptorTable[apic_id].globalDescriptorTableEntries[5]);
    memset(&kernel.globalDescriptorTable[apic_id].tssEntry, 0, sizeof(TSSEntry));
    initializeTSSDescriptor(kernel.globalDescriptorTable[apic_id].tssDescriptor,
                            (uint64_t) & (kernel.globalDescriptorTable[apic_id].tssEntry), TSS_BASIC_SIZE - 1, AR_TSS, 1, PL_KERNEL);
    kernel.globalDescriptorTable[apic_id].tssEntry.rsp0 = kernel.apicManager.apics[apic_id].kstack_ptr;
}