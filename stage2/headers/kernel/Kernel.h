#ifndef KERNEL_H_
#define KERNEL_H_

#include "includes.h"
#include "defines.h"
#include "PhysicalMemory.h"
#include "MemoryPageMap.h"
#include "Console.h"
#include "KMalloc.h"
#include "Spinlock.h"
#include "service_transport_layer.h"
#include "pci_service.h"
#include "ACPI.h"
#include "APICManager.h"
#include "GlobalDescriptorTable.h"
#include "InterruptManager.h"
#include "GlobalDescriptorTablePointer.h"
#include "pit.h"
#include "ATAManager.h"
#include "ipi.h"
#include "SharedMemory.h"
#include "BlockService.h"
#include "rtl8139.h"
#include "keyboard.h"

typedef struct 
{
    service_transport_kernel service_transporter;
    //----------------------------
    PhysicalMemoryManager physicalMemoryManager;
    PCIService pciService;
    Console console;


    MemoryPageTable* bootstrapCorePageTable_ptr;
    MemoryPageTable** coresPageTables_ptr;
    Level_4 L4;

    //----------------------------
    volatile bool debug;
    MemoryAllocator memoryAllocator;
    /* ──────────────────────────────────────────────────────────────────────────────── */
    
    ACPI acpi;
    APICManager apicManager;
        
    GlobalDescriptorTablePointer * globalDescriptorTablePointer;
    GlobalDescriptorTable * globalDescriptorTable;
    InterruptDescriptorTable interruptDescriptorTable;
    InterruptDescriptorTablePointer * interruptDescriptorTablePointer;
    LimitedMemoryPageMap * memoryPageMap;

    InterruptManager interruptManager;

    Ipi ipiManager;
    PIT pit;

    //ATA
    ATAManager ataManager;
    DMABuffer dmaBuffer;
    void* void_pointers[256];

    SharedMemory sharedMemory;
    BlockService blockService;
    
    } Kernel;

void switchToUserMode ();

#endif