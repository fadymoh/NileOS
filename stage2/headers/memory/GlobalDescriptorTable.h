#ifndef GLOBALDESCRIPTORTABLE_H_
#define GLOBALDESCRIPTORTABLE_H_

#include "GlobalDescriptorTableEntry.h"
#include "TSSEntry.h"
#include "TSSDescriptor.h"


// Privilege level
#define PL_KERNEL   0       // Kernel Level
#define PL_USER     3       // User Level

// GDT Descriptor flags
#define AR_PRESENT    (1 << 7)
#define AR_DATA       (2 << 3)
#define AR_CODE       (3 << 3)
#define AR_WRITABLE   (1 << 1)
#define AR_READABLE   (1 << 1)

// Descriptor Types
#define AR_TSS        (0x09U)
#define AR_INTERRUPT  (0x0eU)
#define AR_TRAP       (0x0fU)

// DPL (Descriptor Privilege Level)
#define DPL_KERNEL  (PL_KERNEL << 5)    // Kernel Mode
#define DPL_USER    (PL_USER << 5)      // User Mode

#define GDT_SIZE    7       // Number of entries in the GDT


typedef struct GlobalDescriptorTable
{
        // An array of GlobalDescriptorTableEntry that represents the GDT Entries
        GlobalDescriptorTableEntry globalDescriptorTableEntries[GDT_SIZE];

        // A pointer of type TSSDescriptor and should point to the last 2 entries
        // of the GDT table
        TSSDescriptor * tssDescriptor;
        // A TSSEntry object that should contain the Kernel TSS Entry
        TSSEntry tssEntry;

}GlobalDescriptorTable;



void initGlobalDescriptorTable ();
void initAPGlobalDescriptorTable ();

#endif /* GLOBALDESCRIPTORTABLE_H_ */
