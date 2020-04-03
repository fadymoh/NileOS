#ifndef PHYSICALMEMORY_H_
#define PHYSICALMEMORY_H_

#include "defines.h"
#include "includes.h"
#include "MemoryUtilities.h"
#include "service.h"
#include "KMalloc.h"

enum PhysicalMemory_exposed_functions
{
    create_physical_pages_bitmap,
    print_physical_memory,
    add_physical_memory_entry,
    get_virtual_address,
    is_usable_address,
    is_address,
    initialize_physical_memory_frame,
    allocate_physical_frames,
    deallocate_physical_frame
};

static char mem_types[6][40] = {"E820_MEMORY_ENTRY_TYPE_USABLE",
                                "E820_MEMORY_ENTRY_TYPE_RESEARVED",
                                "E820_MEMORY_ENTRY_TYPE_ACPI_RECLAINMED",
                                "E820_MEMORY_ENTRY_TYPE_ACPI_NVS",
                                "E820_MEMORY_ENTRY_TYPE_BAD_MEMORY",
                                "E820_MEMORY_ENTRY_TYPE_HOLE"};

static char mem_types_desc[6][40] = {"USABLE",
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

} PhysicalMemoryEntry;
typedef struct
{
    uint16_t available_frames;
    uint64_t initial_physical_address;
} PhysicalMemoryFrameCollection;
typedef struct
{
    PhysicalMemoryEntry *physicalMemoryEntry;
    char *physical_memory_bitmap;
    uint16_t memory_entries;
    uint64_t usable_memory_size;
    uint64_t reserved_memory_size;
    uint64_t hole_memory_size;
    uint64_t maximum_address;
    uint64_t reserved_plus_usable_size;
    uint64_t available_frame;
    /* ──────────────────────────────────────────────────────────────────────────────── */
    PhysicalMemoryFrameCollection *physical_memory_frame_collection_ptr;
    uint16_t number_physical_memory_frame_collection;
    /* ──────────────────────────────────────────────────────────────────────────────── */

    //args for the functions
    struct
    {
        uint64_t p_physical_address;
        uint64_t p_size;
        uint64_t number_of_frames_to_allocate;
    } params;

    struct
    {
        bool is_address;
        bool is_usable_address;
        uint64_t virtualAddress;
    } returns;

} PhysicalMemoryManager;

void IsAddress(void *p_physicalmemory_ptr);
void IsUsableAddress(void *p_physicalmemory_ptr);
void GetVirtualAddress(void *p_physicalmemory_ptr);
bool InitializePhysicalMemory(PhysicalMemoryManager *p_physicalmemory_ptr, Service *p_service);
void PrintPhysicalMemory(void *p_physicalmemory_ptr);
void AddPhysicalMemoryEntry(void *p_physicalmemory_ptr);
void CreatePhysicalPagesBitMap(void *p_physicalmemory_ptr);
void InitializePhysicalMemoryFrames(void *p_physical_memory_ptr);
uint64_t AllocateOnePhysicalFrame(void *p_physicalmemory_ptr);
void AllocatePhysicalFrames(void *p_physicalmemory_ptr);
void DeallocatePhysicalFrame(void *p_physicalmemory_ptr);
uint64_t AllocateUsableCollection(void *p_physicalmemory_ptr);
uint64_t AllocateOnePhysicalFrameCollection(void *p_physicalmemory_ptr);
void DeallocateOnePhysicalFrameCollection(void *p_physicalmemory_ptr);

void SetBit(char *initial_bitmap_address, int bit_number);
void ClearBit(char *initial_bitmap_address, int bit_number);
int GetBit(char *initial_bitmap_address, int bit_number);
#endif
