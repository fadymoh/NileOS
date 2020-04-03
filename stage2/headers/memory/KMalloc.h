#ifndef KMALLOC_H_
#define KMALLOC_H_
#include "includes.h"
#include "defines.h"
#include "Spinlock.h"

typedef struct MemoryAllocator
{
    uint64_t kernel_memory_start_address;   // Memory address to start allocating from
    uint64_t kernel_memory_current_address; // Current memory address pointer that points
                                            // to the next unallocated memory location
    uint64_t kernel_memory_end_address;     // The memory address that no memory can be allocated

    uint64_t user_memory_start_address;   // Memory address to start allocating from
    uint64_t user_memory_current_address; // Current memory address pointer that points
                                          // to the next unallocated memory location
    uint64_t user_memory_end_address;     // The memory address that no memory can be allocated
    uint16_t touch;
    spinlock_t spinlock;

} MemoryAllocator;

uint64_t kmalloc(MemoryAllocator *p_memoryAllocator, uint64_t p_size);
uint64_t kvalloc(MemoryAllocator *p_memoryAllocator, uint64_t p_size);

uint64_t malloc(MemoryAllocator *p_memoryAllocator, uint64_t p_size);
uint64_t valloc(MemoryAllocator *p_memoryAllocator, uint64_t p_size);

void memoryAllocatorDump(MemoryAllocator *p_memoryAllocator, bool p_summary);
void memoryAllocatorTouch(MemoryAllocator *p_memoryAllocator);
void memoryAllocatorUntouch(MemoryAllocator *p_memoryAllocator);
void memoryAllocatorAdjust(MemoryAllocator *p_memoryAllocator);
void memset_user_memory(MemoryAllocator *p_memoryAllocator);

#endif