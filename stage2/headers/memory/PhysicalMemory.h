#ifndef PHYSICALMEMORY_H_
#define PHYSICALMEMORY_H_

#include "defines.h"
#include "includes.h"
#include "MemoryUtilities.h"
#include "service.h"
#include "KMalloc.h"

enum PhysicalMemory_exposed_functions{CreatePhysicalPagesBitMap, PrintPhysicalMemory,
                                     AddPhysicalMemoryEntry, GetVirtualAddress, isUsable, 
                                     isaddress, InitializePhysicalMemoryFrame,
                                     AllocatePhysicalFrames,DeallocatePhysicalFrame };

static char mem_types [6][40] = {"E820_MEMORY_ENTRY_TYPE_USABLE",
    "E820_MEMORY_ENTRY_TYPE_RESEARVED",
    "E820_MEMORY_ENTRY_TYPE_ACPI_RECLAINMED",
    "E820_MEMORY_ENTRY_TYPE_ACPI_NVS",
    "E820_MEMORY_ENTRY_TYPE_BAD_MEMORY",
    "E820_MEMORY_ENTRY_TYPE_HOLE"};

static char mem_types_desc [6][40] = {"USABLE",
    "RESEARVED",
    "ACPI_RECLAINMED",
    "ACPI_NVS",
    "BAD_MEMORY",
    "HOLE"};



typedef struct
{
    uint64_t start;
    uint64_t size;   
    uint32_t type;
    uint32_t unused;
    uint64_t vstart;
    uint64_t end;
    
        
}PhysicalMemoryEntry;
typedef struct
{
    uint16_t available_frames;
    uint64_t initial_physical_address;
}PhysicalMemoryFrameCollection;
typedef struct
{
    PhysicalMemoryEntry * physicalMemoryEntry;
    char* physical_memory_bitmap;
    uint16_t memory_entries;
    uint64_t usable_memory_size;
    uint64_t reserved_memory_size;
    uint64_t hole_memory_size;
    uint64_t maximum_address;
    uint64_t reserved_plus_usable_size;
    uint64_t available_frame;
    /* ──────────────────────────────────────────────────────────────────────────────── */
    PhysicalMemoryFrameCollection* physical_memory_frame_collection_ptr;
    uint16_t number_physical_memory_frame_collection;
    /* ──────────────────────────────────────────────────────────────────────────────── */

    //args for the functions
    struct {
        uint64_t p_physical_address;
        uint64_t p_size;
        uint64_t number_of_frames_to_allocate;
    } params;

    struct {
        bool isAddress;
        uint64_t virtualAddress;
    } returns;

}PhysicalMemoryManager;



bool isAddress(void* p_physicalmemory_ptr);
bool isUsableAddress(void* p_physicalmemory_ptr);
uint64_t getVirtualAddress(void* p_physicalmemory_ptr);
bool initializePhysicalMemory(PhysicalMemoryManager* p_physicalmemory_ptr, Service* p_service);
void printPhysicalMemory (void* p_physicalmemory_ptr);
void addPhysicalMemoryEntry(void* p_physicalmemory_ptr);
void createPhysicalPagesBitMap(void* p_physicalmemory_ptr);
void initializePhysicalMemoryFrame(void* p_physical_memory_ptr);
uint64_t allocateOnePhysicalFrame(void * p_physicalmemory_ptr);
void allocatePhysicalFrames(void* p_physicalmemory_ptr);
void deallocatePhysicalFrame(void * p_physicalmemory_ptr);
uint64_t AllocateUsableCollection(void *p_physicalmemory_ptr);
uint64_t allocateOnePhysicalFrameCollection(void *p_physicalmemory_ptr);
void deallocateOnePhysicalFrameCollection(void *p_physicalmemory_ptr);

#endif
