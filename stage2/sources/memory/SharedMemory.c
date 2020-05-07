#include "SharedMemory.h"
#include "Kernel.h"
extern Kernel kernel;

bool initSharedMemoryService(SharedMemory *sharedMemory, Service *p_service)
{
    memset(sharedMemory->bitMap, 0, sizeof(char) * (NUMBER_OF_TABLE_ENTRIES / 8));
    spinlock_init(&sharedMemory->spinlock);
    p_service->service_method_count = 2;

    p_service->add_serviceTest(p_service, AllocateSharedMemory, allocatedSharedMemory_t, "Allocate Shared Memory");
    p_service->serviceTestParameterDescription(p_service, allocatedSharedMemory_t, "numberOfBytes", "uint64_t", sizeof(uint64_t), false, false, true, false);
    p_service->serviceTestParameterDescription(p_service, allocatedSharedMemory_t, "virtualAddress", "uint64_t", sizeof(uint64_t), false, false, true, true);

    p_service->add_serviceTest(p_service, DeallocateSharedMemory, deallocateSharedMemory_t, "Deallocate Shared Memory");
    p_service->serviceTestParameterDescription(p_service, deallocateSharedMemory_t, "numberOfBytes", "uint64_t", sizeof(uint64_t), false, false, true, false);
    p_service->serviceTestParameterDescription(p_service, deallocateSharedMemory_t, "virtualAddress", "uint64_t", sizeof(uint64_t), false, false, true, false);
    p_service->serviceTestParameterDescription(p_service, deallocateSharedMemory_t, "DeallocationStatus", "bool", sizeof(bool), false, false, true, true);

    p_service->parserMethod = ParseSharedMemoryXML;
}

void SharedMemoryDiscoveryService(void *p_sharedMemory)
{
    Service *sharedMemory = (Service *)p_sharedMemory;
    printk("<Shared Memory Service>\n");
    for (int i = 0; i < sharedMemory->service_method_count; ++i)
    {
        printk("\t<%s>\n", sharedMemory->serviceString);
        printk("\t\t<parameters>\n");

        for (int j = 0; j < sharedMemory->testMethod[i].parametersCount; ++j)
        {
            PrintParameter(&(sharedMemory)->testMethod[i].parameters[j]);
        }
        printk("\t\t</parameters>\n");
        printk("\t\t<returns>\n");
        for (int j = 0; j < sharedMemory->testMethod[i].returnsCount; ++j)
        {
            PrintParameter(&(sharedMemory)->testMethod[i].returns[j]);
        }
        printk("\t\t</returns>\n");
        printk("\t</%s>\n", sharedMemory->serviceString);
    }
    printk("</Shared Memory Service>\n");
}

void *ParseSharedMemoryXML(void *p_sharedMemory, char *xmlString, int location)
{
    params_t *temp = kmalloc(&kernel.memoryAllocator, sizeof(params_t));
    ParseXMLString(xmlString);
    Service *sharedMemory = (Service *)p_sharedMemory;
    for (int i = 0; i < sharedMemory->testMethod[location].parametersCount; ++i)
    {
        kernel.xmlService.params.tagDirectory = sharedMemory->testMethod[location].parameters[i].tag;
        getValueFromTagMessage((void *)&kernel.xmlService);
        if (strcmp(sharedMemory->testMethod[location].parameters[i].tag, "numberOfBytes") == NULL)
        {
            temp->numberOfBytes = atoi(kernel.xmlService.returns.value);
        }
        else if (strcmp(sharedMemory->testMethod[location].parameters[i].tag, "virtualAddress") == NULL)
        {
            temp->virtualAddress = atoi(kernel.xmlService.returns.value);
        }
    }

    return temp;
}

char *AllocateSharedMemory(void *p_sharedMemory, void *p_params)
{
    SharedMemory *sharedMemory = (SharedMemory *)p_sharedMemory;
    params_t *parameters = (params_t *)p_params;
    uint64_t numberOfBytes = parameters->numberOfBytes;
    printk("Number of Bytes: %d\n", numberOfBytes);
    int64_t numberOfPagesToAllocate =
        (numberOfBytes + (PAGE_TABLE_SIZE_BYTES)-1) / (PAGE_TABLE_SIZE_BYTES);
    // spinlock here
    spinlock_lock(&sharedMemory->spinlock);
    int64_t returnIndexInBitMap = AllocateConsecutivePages(sharedMemory->bitMap, NUMBER_OF_TABLE_ENTRIES / 8, numberOfPagesToAllocate);
    printk("Allocated at the beginning of: %d\n", returnIndexInBitMap);
    uint64_t virtualAddress = START_OFFSET + returnIndexInBitMap * TWO_MiB_MEMORY_PAGE_SIZE;

    if (returnIndexInBitMap >= 0)
    {
        bool status = SetBitsToValue(sharedMemory->bitMap, NUMBER_OF_TABLE_ENTRIES / 8, returnIndexInBitMap, numberOfPagesToAllocate, 1);
        if (!status)
        {
            printk("Failed to allocate memory in the shared service!\n");
            virtualAddress = UINT64_MAX;

            return ConvertTagToXML(1, &kernel.service_transporter.kernel_services[sharedMemory_t]->testMethod[allocatedSharedMemory_t].parameters[0], virtualAddress);
        }
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
        printk("virtual Address: %x\n", virtualAddress);

        return ConvertTagToXML(1, &kernel.service_transporter.kernel_services[sharedMemory_t]->testMethod[allocatedSharedMemory_t].parameters[0], virtualAddress);
    }
    // spinlock release
    spinlock_unlock(&sharedMemory->spinlock);
    printk("Failed to allocate\n");
    virtualAddress = UINT64_MAX;

    return ConvertTagToXML(1, &kernel.service_transporter.kernel_services[sharedMemory_t]->testMethod[allocatedSharedMemory_t].parameters[0], virtualAddress);
}

char *DeallocateSharedMemory(void *p_sharedMemory, void *p_params)
{
    SharedMemory *sharedMemory = (SharedMemory *)p_sharedMemory;
    params_t *parameters = (params_t *)p_params;

    uint64_t numberOfBytes = parameters->numberOfBytes;

    uint64_t numberOfPagesToDeallocate =
        (parameters->numberOfBytes + (PAGE_TABLE_SIZE_BYTES)-1) / (PAGE_TABLE_SIZE_BYTES);
    uint64_t virtualAddress = parameters->virtualAddress;

    uint64_t startIndexOfDeallocation = (virtualAddress - START_OFFSET);
    startIndexOfDeallocation = startIndexOfDeallocation / (TWO_MiB_MEMORY_PAGE_SIZE) / 8;
    printk("Deallocating at Index: %d\n", startIndexOfDeallocation);
    spinlock_lock(&sharedMemory->spinlock);
    bool status = SetBitsToValue(sharedMemory->bitMap, NUMBER_OF_TABLE_ENTRIES / 8, startIndexOfDeallocation, numberOfPagesToDeallocate, 0);
    if (!status)
    {
        printk("Failed to deallocate memory in the shared service!\n");

        spinlock_unlock(&sharedMemory->spinlock);
        return ConvertTagToXML("status", &status, 1);
    }

    DeallocateSharedMemoryTwoMb(virtualAddress, numberOfBytes);
    spinlock_unlock(&sharedMemory->spinlock);

    return ConvertTagToXML("status", &status, 1);
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
                // printk("charIndex: %d\n", charIndex);
                // printk("bitIndex: %d\n", bitIndex);
                // printk("numberOfConsecutivePages: %d\n", numberOfConsecutivePages);
                int res = charIndex * 8 + bitIndex + 1 - consecutiveBits;
                //printk("Result: %d\n", res);
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