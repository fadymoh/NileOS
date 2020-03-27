#include "MemoryPageMap.h"
#include "Kernel.h"
extern Kernel kernel;

#define ptl1 0b0000000000000000111111111000000000000000000000000000000000000000
#define ptl2 0b0000000000000000000000000111111111000000000000000000000000000000
#define ptl3 0b0000000000000000000000000000000000111111111000000000000000000000
#define ptl4 0b0000000000000000000000000000000000000000000111111111000000000000

#define clr_offset4 0b1111111111111111111111111111111111111111111111111111000000000000
#define clr_offset 0b1111111111111111111111111111111111111111111000000000000000000000
#define set_offset 0b0000000000000000000000000000000000000000000111111111111111111111

bool initVirtualMemoryService(MemoryPageTable **p_memoryPageTable, Service *p_service)
{

    uint64_t total_memory_size = kernel.physicalMemoryManager.usable_memory_size + kernel.physicalMemoryManager.reserved_memory_size;
    uint64_t total_number_of_frames = total_memory_size / FOUR_KiB_MEMORY_PAGE_SIZE;
    uint64_t total_number_of_l4_blocks = (total_number_of_frames + 512 - 1) / 512;
    Level_4 *l4 = &(kernel.L4);

    l4->PTE_ptr = (TablePage *)kvalloc(&(kernel.memoryAllocator), sizeof(TablePage) * total_number_of_l4_blocks);
    l4->numBlocks = total_number_of_l4_blocks;
    l4->bit_map = (uint8_t *)kvalloc(&(kernel.memoryAllocator), sizeof(uint8_t) * total_number_of_l4_blocks);

    memset(l4->PTE_ptr, 0, total_number_of_l4_blocks * sizeof(TablePage));
    memset(l4->bit_map, 255, total_number_of_l4_blocks * sizeof(uint8_t)); // 255 to make all the 8 bits 1's

    /* ──────────────────────────────────────────────────────────────────────────────── */

    MemoryPageTable *bootstrapCorePageTable = *p_memoryPageTable;

    bootstrapCorePageTable->PMD_ptr = (TablePage *)kvalloc(&(kernel.memoryAllocator), sizeof(TablePage) * PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE);
    bootstrapCorePageTable->map_start_address = 0x0;
    bootstrapCorePageTable->map_size = kernel.physicalMemoryManager.maximum_address;
    bootstrapCorePageTable->physicalMemoryManager = &kernel.physicalMemoryManager;

    memset(bootstrapCorePageTable->PGD, 0, PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
    memset(bootstrapCorePageTable->PUD, 0, PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
    memset((uint64_t *)bootstrapCorePageTable->PMD_ptr, 255, sizeof(TablePage) * PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE);

    bspLinkFirst3Levels(bootstrapCorePageTable);

    mapKernelUsableMemory(bootstrapCorePageTable);
    mapReservedMemory(bootstrapCorePageTable);

    p_service->add_service(p_service, LinkThirdStageToCollection_Wrapper, linkThirdStageToCollection);
    p_service->add_service(p_service, LinkThirdStageToCollectionPhysical_Wrapper, linkThirdStageToCollectionPhysical);
    p_service->add_service(p_service, LinkFourthStageToFrame_Wrapper, linkFourthStageToFrame);
    p_service->add_service(p_service, LinkFourthStageToFramePhysical_Wrapper, linkFourthStageToFramePhysical);

    printk("Second Stage mapping successfull! \n");
}
bool LinkThirdStageToCollection_Wrapper(void *p_virtualMemory)
{
    // void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address

    LinkThirdStageToCollection(&kernel.physicalMemoryManager,
                               p_virtualMemory,
                               ((MemoryPageTable *)p_virtualMemory)->params.start_address);
}
bool LinkThirdStageToCollectionPhysical_Wrapper(void *p_virtualMemory)
{
    LinkThirdStageToCollection_physical(&kernel.physicalMemoryManager,
                                        p_virtualMemory,
                                        ((MemoryPageTable *)p_virtualMemory)->params.start_address,
                                        ((MemoryPageTable *)p_virtualMemory)->params.physical_address);
}

bool LinkFourthStageToFrame_Wrapper(void *p_virtualMemory)
{
    LinkFourthStageToFrame(&kernel.physicalMemoryManager,
                           p_virtualMemory,
                           ((MemoryPageTable *)p_virtualMemory)->params.start_address);
}

bool LinkFourthStageToFramePhysical_Wrapper(void *p_virtualMemory)
{
    LinkFourthStageToFrame_physical(&kernel.physicalMemoryManager,
                                    p_virtualMemory,
                                    ((MemoryPageTable *)p_virtualMemory)->params.start_address,
                                    ((MemoryPageTable *)p_virtualMemory)->params.physical_address);
}

uint64_t v2p(uint64_t v_address, uint64_t PGD[])
{
    uint16_t ptl1_index = (v_address & ptl1) >> 39;
    uint16_t ptl2_index = (v_address & ptl2) >> 30;
    uint16_t ptl3_index = (v_address & ptl3) >> 21;
    uint16_t ptl4_index = (v_address & ptl4) >> 12;
    uint64_t *pud = (uint64_t *)(PGD[ptl1_index] & clr_offset4);
    uint64_t *pmd = (uint64_t *)(pud[ptl2_index] & clr_offset4);
    uint64_t *pte = (uint64_t *)(pmd[ptl3_index] & clr_offset4);

    uint64_t p_address = pte[ptl4_index];

    return p_address;
}

uint64_t v2p2m(uint64_t v_address)
{
    uint16_t ptl1_index = (v_address & ptl1) >> 39;
    uint16_t ptl2_index = (v_address & ptl2) >> 30;
    uint16_t ptl3_index = (v_address & ptl3) >> 21;

    uint64_t *pud = (uint64_t *)(kernel.memoryPageMap->PGD[ptl1_index] & 0xFFFFFFFFFFFFF000);
    uint64_t *pmd = (uint64_t *)(pud[ptl2_index] & 0xFFFFFFFFFFFFF000);

    uint64_t p_address = pmd[ptl3_index];

    return (p_address & 0xFFFFFFFFFFFFF000) + (set_offset & v_address);
}

void enablePageDirectory(uint64_t page_directory_address)
{
    // Load page directory address to register CR3
    asm volatile("mov %0, %%cr3" ::"r"(page_directory_address));
    uint64_t cr0;
    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));                // Get CR0 register value
    cr0 |= 0x80000000;                        //Bit 31 for enabling Paging
    asm volatile("mov %0, %%cr0" ::"r"(cr0)); // Enable paging
}

void mapLimitedUsableMemory(LimitedMemoryPageMap *p_memoryPageMap, uint64_t p_start_address, uint64_t p_size)
{ // we need to adjust virtual to physical addresses mapping
    uint64_t physical_address = 0x0;
    uint64_t virtual_address = 0x0;
    uint16_t pgd_index = 0;
    uint16_t pud_index = 0;
    uint16_t pmd_index = 0;
    p_memoryPageMap->mapped_pages = 0;
    p_memoryPageMap->last_virtual_address = 0;
    for (physical_address = p_start_address; physical_address < p_start_address + p_size;)
    {
        kernel.physicalMemoryManager.params.p_physical_address = physical_address;
        kernel.physicalMemoryManager.params.p_size = TWO_MiB_MEMORY_PAGE_SIZE;
        if (isUsableAddress(p_memoryPageMap->physicalMemoryManager))
        {
            pgd_index = virtual_address >> (48 - 9);
            pud_index = (virtual_address >> (48 - 18)) & 0b0000000111111111;
            pmd_index = (virtual_address >> (48 - 27)) & 0b0000000111111111;
            p_memoryPageMap->PGD[pgd_index] = ((uint64_t) & (p_memoryPageMap->PUD[pgd_index] /*[pud_index]*/) | MEMORY_PAGE_TABLE_ENTRY_PRESENT | MEMORY_PAGE_TABLE_ENTRY_RW | MEMORY_PAGE_TABLE_ENTRY_USER);
            p_memoryPageMap->PUD[pgd_index][pud_index] = ((uint64_t) & (p_memoryPageMap->PMD[(pgd_index * PAGE_TABLE_UNIT_SIZE) + pud_index]) | MEMORY_PAGE_TABLE_ENTRY_PRESENT | MEMORY_PAGE_TABLE_ENTRY_RW | MEMORY_PAGE_TABLE_ENTRY_USER);
            p_memoryPageMap->PMD[(pgd_index * PAGE_TABLE_UNIT_SIZE) + pud_index].tpage[pmd_index] = (physical_address |
                                                                                                     MEMORY_PAGE_TABLE_ENTRY_2MIB |
                                                                                                     MEMORY_PAGE_TABLE_ENTRY_PRESENT |
                                                                                                     MEMORY_PAGE_TABLE_ENTRY_RW |
                                                                                                     MEMORY_PAGE_TABLE_ENTRY_USER);
            p_memoryPageMap->last_virtual_address = virtual_address + TWO_MiB_MEMORY_PAGE_SIZE - 1;
            p_memoryPageMap->mapped_pages++;
            virtual_address += TWO_MiB_MEMORY_PAGE_SIZE;
            physical_address += TWO_MiB_MEMORY_PAGE_SIZE;
        }
        else
            physical_address += FOUR_KiB_MEMORY_PAGE_SIZE;
    }
}
void mapLimitedPages(LimitedMemoryPageMap *p_memoryPageMap, bool p_enable_dir)
{
    mapLimitedUsableMemory(p_memoryPageMap, p_memoryPageMap->map_start_address, p_memoryPageMap->map_size);

    if (p_enable_dir)
        enablePageDirectory((uint64_t) & (p_memoryPageMap->PGD));
}

bool isReservedMemory(PhysicalMemoryEntry entry)
{
    return (entry.start > 0x0100000 && (entry.type == E820_MEMORY_ENTRY_TYPE_RESEARVED || entry.type == E820_MEMORY_ENTRY_TYPE_ACPI_RECLAINMED || entry.type == E820_MEMORY_ENTRY_TYPE_ACPI_NVS));
}
void mapReservedMemory(MemoryPageTable *p_memoryPageMap)
{
    //! maybe it overlaps m3 shmem
    uint64_t physical_address = 0x0;
    uint64_t last_available_vaddress = (PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE);
    last_available_vaddress *= PAGE_TABLE_UNIT_SIZE;
    last_available_vaddress *= PAGE_TABLE_UNIT_SIZE;
    last_available_vaddress -= 1;
    last_available_vaddress *= FOUR_KiB_MEMORY_PAGE_SIZE;

    uint64_t i = 0;
    uint64_t rmap_count = 0;

    for (i = 0; i < kernel.physicalMemoryManager.memory_entries; i++)
    {
        if (isReservedMemory(kernel.physicalMemoryManager.physicalMemoryEntry[i]))
        {

            uint64_t num_frames_needed = kernel.physicalMemoryManager.physicalMemoryEntry[i].size / FOUR_KiB_MEMORY_PAGE_SIZE;
            uint64_t start_vaddress = last_available_vaddress - (num_frames_needed * FOUR_KiB_MEMORY_PAGE_SIZE);
            last_available_vaddress = start_vaddress - FOUR_KiB_MEMORY_PAGE_SIZE;

            kernel.physicalMemoryManager.physicalMemoryEntry[i].vstart = start_vaddress;
            physical_address = kernel.physicalMemoryManager.physicalMemoryEntry[i].start;
            for (; physical_address < kernel.physicalMemoryManager.physicalMemoryEntry[i].start +
                                          kernel.physicalMemoryManager.physicalMemoryEntry[i].size;)
            {
                LinkFourthStageToFrame_physical(&kernel.physicalMemoryManager, p_memoryPageMap, start_vaddress, physical_address);
                start_vaddress += FOUR_KiB_MEMORY_PAGE_SIZE;
                physical_address += FOUR_KiB_MEMORY_PAGE_SIZE;
            }
        }
        else if (kernel.physicalMemoryManager.physicalMemoryEntry[i].type != E820_MEMORY_ENTRY_TYPE_HOLE)
            kernel.physicalMemoryManager.physicalMemoryEntry[i].vstart = kernel.physicalMemoryManager.physicalMemoryEntry[i].start;
    }
}

void pageMapStage1()
{
    LimitedMemoryPageMap *memoryPageMap = (LimitedMemoryPageMap *)KERNEL_MEMORY_MAP_ADDRESS;
    uint64_t pmd_address = KERNEL_MEMORY_MAP_ADDRESS + sizeof(LimitedMemoryPageMap);

    if ((pmd_address & 0xFFFFFFFFFFFFF000) < pmd_address)
    {
        pmd_address &= 0xFFFFFFFFFFFFF000;
        pmd_address += 0x1000;
    }

    memoryPageMap->PMD = (TablePage *)pmd_address;

    kernel.memoryPageMap = memoryPageMap;

    memoryPageMap->map_start_address = 0x0;
    // ask about this
    memoryPageMap->map_size = KERNELKMALLOC_INITIAL_MEMORY_ADDRESS + KERNELKMALLOC_INITIAL_MEMORY_SIZE; // We need 100 MiB after Ram File system for kernel initialization and then we can adjust our memory tables;

    memoryPageMap->physicalMemoryManager = &kernel.physicalMemoryManager;

    // 1 * 512 * 8 bytes for PGD
    memset(memoryPageMap->PGD, 0, PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
    // 4 * 512 * 8 bytes for PUD
    memset(memoryPageMap->PUD, 0, PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
    // 16 * 512 * 8 bytes for PMD
    memset((uint64_t *)memoryPageMap->PMD, 0, MAX_PMD_COUNT * sizeof(TablePage));
    // 2 * 512 * 8 bytes for PTE
    memset(memoryPageMap->PTE, 0, PAGE_TABLE_UNIT_SIZE * MIN_PTE_COUNT * sizeof(uint64_t));
    //
    memset(memoryPageMap->PUD4, 0, PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
    memset(memoryPageMap->PMD4, 0, PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
    mapLimitedPages(memoryPageMap, true);
    spinlock_init(&kernel.memoryAllocator.spinlock);
    kernel.memoryAllocator.kernel_memory_start_address = KERNELKMALLOC_INITIAL_MEMORY_ADDRESS;
    kernel.memoryAllocator.kernel_memory_current_address = KERNELKMALLOC_INITIAL_MEMORY_ADDRESS;
    kernel.memoryAllocator.kernel_memory_end_address = KERNELKMALLOC_INITIAL_MEMORY_ADDRESS + KERNELKMALLOC_INITIAL_MEMORY_SIZE - 1;
}

void bspLinkFirst3Levels(MemoryPageTable *p_memoryPageMap)
{
    uint64_t physical_address = 0x0;
    uint16_t pgd_index = 0;
    uint16_t pud_index = 0;
    uint16_t pmd_index = 0;
    p_memoryPageMap->mapped_pages = 0;
    p_memoryPageMap->last_virtual_address = 0;

    uint64_t last_available_vaddress = (PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE);
    last_available_vaddress *= PAGE_TABLE_UNIT_SIZE;
    last_available_vaddress *= PAGE_TABLE_UNIT_SIZE;
    last_available_vaddress -= 1;
    last_available_vaddress *= FOUR_KiB_MEMORY_PAGE_SIZE;

    uint64_t p_start_address = 0x0;

    for (physical_address = p_start_address; physical_address <= last_available_vaddress;)
    {
        pgd_index = physical_address >> (48 - 9);
        pud_index = (physical_address >> (48 - 18)) & 0b0000000111111111;
        pmd_index = (physical_address >> (48 - 27)) & 0b0000000111111111;
        p_memoryPageMap->PGD[pgd_index] = ((uint64_t) & (p_memoryPageMap->PUD[pgd_index] /*[pud_index]*/) | MEMORY_PAGE_TABLE_ENTRY_PRESENT | MEMORY_PAGE_TABLE_ENTRY_RW | MEMORY_PAGE_TABLE_ENTRY_USER);
        p_memoryPageMap->PUD[pgd_index][pud_index] = ((uint64_t) & (p_memoryPageMap->PMD_ptr[(pgd_index * PAGE_TABLE_UNIT_SIZE) + pud_index]) | MEMORY_PAGE_TABLE_ENTRY_PRESENT | MEMORY_PAGE_TABLE_ENTRY_RW | MEMORY_PAGE_TABLE_ENTRY_USER);

        p_memoryPageMap->last_virtual_address = physical_address + (ONE_GIG_VALUE)-1;
        p_memoryPageMap->mapped_pages++;
        physical_address += (ONE_GIG_VALUE);
    }
}

void AllocateSharedMemoryTwoMb(uint64_t virtualAddressStart, uint64_t size)
{

    //! needs to be changed to include specific cores

    for (uint64_t start = virtualAddressStart; start <= virtualAddressStart + size; start += (TWO_MiB_MEMORY_PAGE_SIZE))
    {
        uint64_t address;
        uint64_t idx = 0;
        for (int coreIndex = 0; coreIndex < kernel.acpi.cores_count; ++coreIndex)
        {
            if (idx == 0)
            {
                address = LinkThirdStageToCollection((void *)&kernel.physicalMemoryManager, kernel.coresPageTables_ptr[coreIndex], start);

                idx++;
            }
            else
            {
                // if (start == virtualAddressStart)
                //     printk("Allocated Physical Address: %x\n", address);

                LinkThirdStageToCollection_physical((void *)&kernel.physicalMemoryManager, kernel.coresPageTables_ptr[coreIndex], start, address);
            }
        }
    }
}

void DeallocateSharedMemoryTwoMb(uint64_t virtualAddressStart, uint64_t size)
{
    //! needs to be changed to include specific cores
    for (int coreIndex = 0; coreIndex < kernel.acpi.cores_count; ++coreIndex)
    {
        for (uint64_t start = virtualAddressStart; start <= virtualAddressStart + size; start += (TWO_MiB_MEMORY_PAGE_SIZE))
        {
            DeallocateVirtualMemoryCollection((void *)&kernel.physicalMemoryManager, kernel.coresPageTables_ptr[coreIndex], start);
        }
    }
}

void DeallocateVirtualMemoryCollection(void *p_physical_memory, MemoryPageTable *page_table, uint64_t start_address)
{
    PhysicalMemoryManager *p_physical_memory_ptr = (PhysicalMemoryManager *)p_physical_memory;

    uint64_t pgd_index = start_address >> (48 - 9);
    uint64_t pud_index = (start_address >> (48 - 18)) & 0b0000000111111111;
    uint64_t pmd_index = (start_address >> (48 - 27)) & 0b0000000111111111;
    uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;
    TablePage *TP = (TablePage *)&(page_table->PMD_ptr[idx]);

    uint64_t physical_memory_address = TP->tpage[pmd_index];

    p_physical_memory_ptr->params.p_physical_address = physical_memory_address;
    deallocateOnePhysicalFrameCollection(p_physical_memory_ptr);

    TP->tpage[pmd_index] = 0xFFFFFFFFFFFFFFFF;
    TP->tpage[pmd_index] = 0xFFFFFFFFFFFFFFF0;

    return true;
}

uint64_t allocateVirtualMemoryBlock(Level_4 *l4)
{

    for (uint64_t i = 0; i < l4->numBlocks / 8; i++)
    {

        uint8_t bt = l4->bit_map[i];
        int idx = -1;

        //TO DO: using anding with (num -1) to get the first bit that is 1
        if (bt == 0)
            continue;

        if (bt <= 255 && bt >= 128)
            idx = 0; // if bit 0 is 1
        else if (bt <= 127 && bt >= 64)
            idx = 1; // if bit 1 is 1
        else if (bt <= 63 && bt >= 32)
            idx = 2; // if bit 2 is 1
        else if (bt <= 31 && bt >= 16)
            idx = 3; // if bit 3 is 1
        else if (bt <= 15 && bt >= 8)
            idx = 4; // if bit 4 is 1
        else if (bt <= 7 && bt >= 4)
            idx = 5; // if bit 5 is 1
        else if (bt <= 3 && bt >= 2)
            idx = 6; // if bit 6 is 1
        else if (bt <= 1 && bt >= 0)
            idx = 7; // if bit 7 is 1

        l4->bit_map[i] &= (char)(~((char)(1 << (7 - (idx)))));
        return (uint64_t)&l4->PTE_ptr[(i * 8) + idx];
    }

    return -1;
}

bool deallocateVirtualMemoryBlock(Level_4 *l4, uint64_t address)
{
    int block_index = (address - (uint64_t)l4->PTE_ptr) / FOUR_KiB_MEMORY_PAGE_SIZE;
    int idx1 = block_index / 8;
    int idx2 = block_index % 8;

    if (((l4->bit_map[idx1]) >> (7 - idx2)) % 2) // we are checking if this bit is 1 by shifting it to the right most bit and then checking if it is odd or even
        return 0;

    l4->bit_map[idx1] |= (1 << (7 - idx2)); // setting the corresponding bit to one to indicate that it is now free
    return 1;
}

uint64_t LinkThirdStageToCollection(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address)
{
    PhysicalMemoryManager *p_physical_memory_ptr = (PhysicalMemoryManager *)p_physical_memory;

    uint64_t pgd_index = start_address >> (48 - 9);
    uint64_t pud_index = (start_address >> (48 - 18)) & 0b0000000111111111;
    uint64_t pmd_index = (start_address >> (48 - 27)) & 0b0000000111111111;
    uint64_t physical_memory_address = allocateOnePhysicalFrameCollection(p_physical_memory);
    uint64_t returned_padd = physical_memory_address;

    uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;

    TablePage *TP = (TablePage *)&(bsp_table->PMD_ptr[idx]);
    TP->tpage[pmd_index] = (physical_memory_address |
                            MEMORY_PAGE_TABLE_ENTRY_2MIB |
                            MEMORY_PAGE_TABLE_ENTRY_PRESENT |
                            MEMORY_PAGE_TABLE_ENTRY_RW |
                            MEMORY_PAGE_TABLE_ENTRY_USER);



    return returned_padd;
}

bool LinkThirdStageToCollection_physical(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address, uint64_t physical_address)
{
    PhysicalMemoryManager *p_physical_memory_ptr = (PhysicalMemoryManager *)p_physical_memory;

    uint64_t pgd_index = start_address >> (48 - 9);
    uint64_t pud_index = (start_address >> (48 - 18)) & 0b0000000111111111;
    uint64_t pmd_index = (start_address >> (48 - 27)) & 0b0000000111111111;

    uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;

    TablePage *TP_PMD = (TablePage *)&(bsp_table->PMD_ptr[idx]);
    TP_PMD->tpage[pmd_index] = (physical_address |
                                MEMORY_PAGE_TABLE_ENTRY_2MIB |
                                MEMORY_PAGE_TABLE_ENTRY_PRESENT |
                                MEMORY_PAGE_TABLE_ENTRY_RW |
                                MEMORY_PAGE_TABLE_ENTRY_USER);

    return true;
}

bool LinkFourthStageToFrame(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address)
{
    PhysicalMemoryManager *p_physical_memory_ptr = (PhysicalMemoryManager *)p_physical_memory;
    uint64_t pgd_index = start_address >> (48 - 9);
    uint64_t pud_index = (start_address >> (48 - 18)) & 0b0000000111111111;
    uint64_t pmd_index = (start_address >> (48 - 27)) & 0b0000000111111111;
    uint64_t pte_index = (start_address >> (48 - 36)) & 0b0000000111111111;

    uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;

    TablePage *TP_PMD = (TablePage *)&(bsp_table->PMD_ptr[idx]);

    if (TP_PMD->tpage[pmd_index] == 0xFFFFFFFFFFFFFFFF)
    {
        uint64_t returned_pte = allocateVirtualMemoryBlock(&kernel.L4);
        TP_PMD->tpage[pmd_index] = (returned_pte | MEMORY_PAGE_TABLE_ENTRY_PRESENT | MEMORY_PAGE_TABLE_ENTRY_RW | MEMORY_PAGE_TABLE_ENTRY_USER);
    }

    TablePage *TP_PTE = (TablePage *)(TP_PMD->tpage[pmd_index] & clr_offset4);
    uint64_t physical_memory_address = allocateOnePhysicalFrame(p_physical_memory);

    TP_PTE->tpage[pte_index] = (physical_memory_address |
                                MEMORY_PAGE_TABLE_ENTRY_2MIB | //! should be 4kb ONLY!
                                MEMORY_PAGE_TABLE_ENTRY_PRESENT |
                                MEMORY_PAGE_TABLE_ENTRY_RW |
                                MEMORY_PAGE_TABLE_ENTRY_USER);

    return true;
}

bool LinkFourthStageToFrame_physical(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address, uint64_t physical_address)
{
    PhysicalMemoryManager *p_physical_memory_ptr = (PhysicalMemoryManager *)p_physical_memory;

    uint64_t pgd_index = start_address >> (48 - 9);
    uint64_t pud_index = (start_address >> (48 - 18)) & 0b0000000111111111;
    uint64_t pmd_index = (start_address >> (48 - 27)) & 0b0000000111111111;
    uint64_t pte_index = (start_address >> (48 - 36)) & 0b0000000111111111;

    uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;
    uint64_t *pud = (uint64_t *)(bsp_table->PGD[pgd_index] & clr_offset4);
    uint64_t *pmd = (uint64_t *)(pud[pud_index] & clr_offset4);

    uint64_t pte_address = bsp_table->PMD_ptr[idx].tpage[pmd_index];

    if (pte_address == 0xFFFFFFFFFFFFFFFF)
    {
        uint64_t returned_pte = allocateVirtualMemoryBlock(&kernel.L4);
        bsp_table->PMD_ptr[idx].tpage[pmd_index] = (returned_pte | MEMORY_PAGE_TABLE_ENTRY_PRESENT | MEMORY_PAGE_TABLE_ENTRY_RW | MEMORY_PAGE_TABLE_ENTRY_USER);
        pte_address = bsp_table->PMD_ptr[idx].tpage[pmd_index];
    }

    TablePage *TP_PTE = (TablePage *)(pte_address & clr_offset4);

    TP_PTE->tpage[pte_index] = (physical_address |
                                MEMORY_PAGE_TABLE_ENTRY_2MIB | //! should be 4kb ONLY!
                                MEMORY_PAGE_TABLE_ENTRY_PRESENT |
                                MEMORY_PAGE_TABLE_ENTRY_RW |
                                MEMORY_PAGE_TABLE_ENTRY_USER);

    return true;
}

bool mapKernelUsableMemory(MemoryPageTable *pagetable)
{
    int num_collections = KERNEL_MEMORY_SIZE / FOUR_KiB_MEMORY_PAGE_SIZE;
    int current_address = 0;
    // ONE TO ONE MAPPING FOR "KERNEL MEMORY SIZE"
    for (int i = 0; i < num_collections; i++)
    {
        LinkFourthStageToFrame_physical((void *)&kernel.physicalMemoryManager, pagetable, current_address, current_address);
        current_address += FOUR_KiB_MEMORY_PAGE_SIZE;
    }
}

bool buildApCoresPageTables(int cores_count)
{
    kernel.coresPageTables_ptr = kvalloc(&(kernel.memoryAllocator), sizeof(MemoryPageTable *) * (cores_count));
    kernel.coresPageTables_ptr[0] = kernel.bootstrapCorePageTable_ptr;

    for (int i = 1; i < cores_count; i++)
    {
        kernel.coresPageTables_ptr[i] = kvalloc(&(kernel.memoryAllocator), sizeof(MemoryPageTable));
        kernel.coresPageTables_ptr[i]->PMD_ptr = (TablePage *)kvalloc(&(kernel.memoryAllocator),
                                                                      sizeof(TablePage) * PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE);

        kernel.coresPageTables_ptr[i]->map_start_address = 0x0;
        kernel.coresPageTables_ptr[i]->map_size = kernel.physicalMemoryManager.maximum_address;
        kernel.coresPageTables_ptr[i]->physicalMemoryManager = &kernel.physicalMemoryManager;

        memset(kernel.coresPageTables_ptr[i]->PGD, 0, PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));
        memset(kernel.coresPageTables_ptr[i]->PUD, 0, PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE * sizeof(uint64_t));

        memset((uint64_t *)kernel.coresPageTables_ptr[i]->PMD_ptr, 255, sizeof(TablePage) * PGD_ENTRIES_LIMIT * PAGE_TABLE_UNIT_SIZE);

        bspLinkFirst3Levels(kernel.coresPageTables_ptr[i]);
        mapKernelUsableMemory(kernel.coresPageTables_ptr[i]);
        mapReservedMemory(kernel.coresPageTables_ptr[i]);
    }

    return true;
}

void refreshReserved ()
{
    for ( uint8_t i = 0 ; i < kernel.acpi.cores_count ; i ++)
    {
        mapReservedMemory (kernel.coresPageTables_ptr[i]);
        dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
        int core_id = kernel.apicManager.returns.core_id;
        if (i == core_id) enablePageDirectory((uint64_t)&(kernel.coresPageTables_ptr[i]->PGD));
    }
}