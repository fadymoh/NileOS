#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include "MemoryPageMap.h"

#define SHARED_MEMORY_BYTES ((uint64_t)2 * 1024 * 1024 * 1024)
#define PAGE_TABLE_SIZE_BYTES 2 * 1024 * 1024
#define NUMBER_OF_TABLE_ENTRIES ((SHARED_MEMORY_BYTES) / (PAGE_TABLE_SIZE_BYTES))
#define START_OFFSET ((uint64_t)2 * 1024 * 1024 * 1024)

enum sharedMemoryExposedFunctions
{
    allocatedSharedMemory_t,
    deallocateSharedMemory_t
};

typedef struct params
{
    uint64_t numberOfBytes;
    uint64_t virtualAddress;
} params_t;

struct returns
{
    uint64_t virtualAddress;
} returns_t;

typedef struct SharedMemory_t
{
    char bitMap[NUMBER_OF_TABLE_ENTRIES / 8];
    spinlock_t spinlock;

} SharedMemory;

bool initSharedMemoryService(SharedMemory *sharedMemory, Service *p_service);
char *AllocateSharedMemory(void *p_sharedMemory, void *p_params);
char *DeallocateSharedMemory(void *p_sharedMemory, void *p_params);
int64_t AllocateConsecutivePages(char bitMap[], int length, int numberOfConsecutivePages);
bool SetBitsToValue(char bitMap[], int length, int64_t returnIndexInBitMap, int numberOfPagesToSet, bool value);
void *ParseSharedMemoryXML(void* p_sharedMemory, char *xmlString, int location);
void SharedMemoryDiscoveryService(void* p_sharedMemory);

#endif