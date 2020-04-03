#include "SharedMemory.h"
#include "Kernel.h"
extern Kernel kernel;

bool initSharedMemoryService(SharedMemory *sharedMemory, Service *p_service)
{
    memset(sharedMemory->bitMap, 0, sizeof(char) * (NUMBER_OF_TABLE_ENTRIES / 8));
    spinlock_init(&sharedMemory->spinlock);

    p_service->add_service(p_service, AllocateSharedMemory, allocatedSharedMemory_t);
    p_service->add_service(p_service, DeallocateSharedMemory, deallocateSharedMemory_t);
}

void AllocateSharedMemory(void *p_sharedMemory)
{
    SharedMemory *sharedMemory = (SharedMemory *)p_sharedMemory;
    uint64_t numberOfBytes = sharedMemory->params_t.numberOfBytes;
    int64_t numberOfPagesToAllocate =
        (numberOfBytes + (PAGE_TABLE_SIZE_BYTES)-1) / (PAGE_TABLE_SIZE_BYTES);
    // spinlock here
    spinlock_lock(&sharedMemory->spinlock);
    int64_t returnIndexInBitMap = AllocateConsecutivePages(sharedMemory->bitMap, NUMBER_OF_TABLE_ENTRIES / 8, numberOfPagesToAllocate);
    printk("Allocated at the beginning of: %d\n", returnIndexInBitMap);

    if (returnIndexInBitMap >= 0)
    {
        bool status = SetBitsToValue(sharedMemory->bitMap, NUMBER_OF_TABLE_ENTRIES / 8, returnIndexInBitMap, numberOfPagesToAllocate, 1);
        if (!status)
        {
            printk("Failed to allocate memory in the shared service!\n");
            sharedMemory->returns_t.virtualAddress = UINT64_MAX;
            return;
        }
        uint64_t virtualAddress = START_OFFSET + returnIndexInBitMap * TWO_MiB_MEMORY_PAGE_SIZE;
        AllocateSharedMemoryTwoMb(virtualAddress, numberOfBytes);

        uint64_t pgd_index = virtualAddress >> (48 - 9);
        uint64_t pud_index = (virtualAddress >> (48 - 18)) & 0b0000000111111111;
        uint64_t pmd_index = (virtualAddress >> (48 - 27)) & 0b0000000111111111;
        uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;
        TablePage *TP = (TablePage *)&(kernel.coresPageTables_ptr[0]->PMD_ptr[idx]);
        uint64_t physical_memory_address = TP->tpage[pmd_index];

        printk("Physical memory address value: %x\n", physical_memory_address);
        // spinlock release
        spinlock_unlock(&sharedMemory->spinlock);
        sharedMemory->returns_t.virtualAddress = virtualAddress;
        return;
    }
    // spinlock release
    spinlock_unlock(&sharedMemory->spinlock);
    printk("Failed to allocate");

    sharedMemory->returns_t.virtualAddress = UINT64_MAX;
    return;
}

bool DeallocateSharedMemory(void *p_sharedMemory)
{
    SharedMemory *sharedMemory = (SharedMemory *)p_sharedMemory;
    uint64_t numberOfBytes = sharedMemory->params_t.numberOfBytes;

    uint64_t numberOfPagesToDeallocate =
        (sharedMemory->params_t.numberOfBytes + (PAGE_TABLE_SIZE_BYTES)-1) / (PAGE_TABLE_SIZE_BYTES);
    uint64_t virtualAddress = sharedMemory->params_t.virtualAddress;
    uint64_t startIndexOfDeallocation = (virtualAddress - START_OFFSET);
    startIndexOfDeallocation = startIndexOfDeallocation / (TWO_MiB_MEMORY_PAGE_SIZE) / 8;
    printk("Deallocating at Index: %d\n", startIndexOfDeallocation);
    spinlock_lock(&sharedMemory->spinlock);
    bool status = SetBitsToValue(sharedMemory->bitMap, NUMBER_OF_TABLE_ENTRIES / 8, startIndexOfDeallocation, numberOfPagesToDeallocate, 0);
    if (!status)
    {
        printk("Failed to deallocate memory in the shared service!\n");
        sharedMemory->returns_t.virtualAddress = UINT64_MAX;
        spinlock_unlock(&sharedMemory->spinlock);
        return;
    }

    DeallocateSharedMemoryTwoMb(virtualAddress, numberOfBytes);
    sharedMemory->returns_t.virtualAddress = UINT64_MAX;
    spinlock_unlock(&sharedMemory->spinlock);
    return;
}

int64_t AllocateConsecutivePages(char bitMap[], int length, int numberOfConsecutivePages)
{
    char currentChar;
    char currentBit;
    const char lastBit = 0;
    int consecutiveBits = 0;

    for (int charIndex = 0; charIndex < length; charIndex++)
    {
        currentChar = bitMap[charIndex];
        for (int bitIndex = 0; bitIndex < 8; bitIndex++)
        {
            currentBit = currentChar & (1 << (7 - bitIndex));

            if (currentBit != lastBit)
            {
                consecutiveBits = 0;
            }
            else
            {
                consecutiveBits++;
            }

            if (consecutiveBits == numberOfConsecutivePages)
            {
                printk("charIndex: %d\n", charIndex);
                printk("bitIndex: %d\n", bitIndex);
                printk("numberOfConsecutivePages: %d\n", numberOfConsecutivePages);
                int res = charIndex * 8 + bitIndex + 1 - consecutiveBits;
                printk("Result: %d\n", res);
                return res;
            }
        }
    }

    return -1;
}

bool SetBitsToValue(char bitMap[], int length, int64_t returnIndexInBitMap, int numberOfPagesToSet, bool value)
{
    uint64_t charIndex = returnIndexInBitMap / 8;
    uint64_t bitIndex = returnIndexInBitMap % 8;
    uint64_t tempPagesToAllocate = numberOfPagesToSet;
    while (tempPagesToAllocate--)
    {
        if (value == 1)
        {
            bitMap[charIndex] = bitMap[charIndex] | (1 << (7 - bitIndex));
        }
        else
        {
            bitMap[charIndex] = bitMap[charIndex] & (~(1 << (7 - bitIndex)));
        }

        bitIndex++;

        if (bitIndex == 8)
        {
            charIndex++;
            if (charIndex == length)
            {
                return false;
            }
            bitIndex = 0;
        }
    }

    return true;
}