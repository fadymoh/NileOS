#ifndef MEMORYPAGEMAP_H_
#define MEMORYPAGEMAP_H_

#include "PhysicalMemory.h"

enum virtualMemory_exposed_functions
{
    linkThirdStageToCollection,
    linkThirdStageToCollectionPhysical,
    linkFourthStageToFrame,
    linkFourthStageToFramePhysical
};

typedef struct
{
    uint64_t present : 1;  // Page present in memory
    uint64_t rw : 1;       // Read-only if clear, readwrite if set
    uint64_t user : 1;     // Supervisor level only if clear
    uint64_t pwt : 1;      // Page Level Write-Through
    uint64_t pcd : 1;      // Page Level Cache-Disabled
    uint64_t accessed : 1; // Has the page been accessed since last refresh?
    uint64_t dirty : 1;    // Has the page been written to since last refresh?
    uint64_t ps : 1;       // Page Size
    uint64_t g : 1;        // Global Page
    uint64_t avl : 3;      // Available for software (Reserved)
    uint64_t frame : 52;   // Frame address (shifted right 12 bits)
} pte;

typedef struct TablePage
{
    uint64_t tpage[PAGE_TABLE_UNIT_SIZE];
} TablePage;

typedef struct
{
    uint64_t PGD[PAGE_TABLE_UNIT_SIZE];
    uint64_t PUD[PGD_ENTRIES_LIMIT][PAGE_TABLE_UNIT_SIZE];
    TablePage *PMD_ptr;

    uint64_t map_start_address;
    uint64_t map_size;
    PhysicalMemoryManager *physicalMemoryManager;
    uint64_t mapped_pages;
    uint64_t last_virtual_address;

    // args
    struct
    {
        void *p_physical_memory;
        //MemoryPageTable *bsp_table;
        uint64_t start_address;
        uint64_t physical_address;
        // uint64_t numberOfGigaToAllocatedShared;
    } params;

} MemoryPageTable;

typedef struct
{
    uint64_t PGD[PAGE_TABLE_UNIT_SIZE];                //Multi level: level 0
    uint64_t PUD4[PAGE_TABLE_UNIT_SIZE];               // level 1
    uint64_t PMD4[PAGE_TABLE_UNIT_SIZE];               //level 2
    uint64_t PTE[MIN_PTE_COUNT][PAGE_TABLE_UNIT_SIZE]; //level 3 and final
    uint64_t PUD[PGD_ENTRIES_LIMIT][PAGE_TABLE_UNIT_SIZE];
    TablePage *PMD;
    uint64_t map_start_address;
    uint64_t map_size;
    PhysicalMemoryManager *physicalMemoryManager;
    uint64_t mapped_pages;
    uint64_t last_virtual_address;
} LimitedMemoryPageMap;

typedef struct
{
    uint64_t numBlocks;
    TablePage *PTE_ptr;
    uint8_t *bit_map;

} Level_4;

uint64_t v2p(uint64_t v_address, uint64_t[]);
uint64_t v2p2m(uint64_t v_address);
void mapLimitedUsableMemory(LimitedMemoryPageMap *p_memoryPageMap, uint64_t p_start_address, uint64_t p_size);
void mapLimitedPages(LimitedMemoryPageMap *p_memoryPageMap, bool p_enable_dir);

//------------E7naaaaaa -----------------

uint64_t allocateVirtualMemoryBlock(Level_4 *l4);
void bspLinkFirst3Levels(MemoryPageTable *p_memoryPageMap);
bool deallocateVirtualMemoryBlock(Level_4 *l4, uint64_t address);
bool mapKernelUsableMemory(MemoryPageTable *pagetable);

bool buildApCoresPageTables(int cores_count);
void pageMapStage1();
void mapReservedMemory(MemoryPageTable *p_memoryPageMap);
void enablePageDirectory(uint64_t page_directory_address);

uint64_t LinkThirdStageToCollection(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address);
bool LinkThirdStageToCollection_physical(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address, uint64_t physical_address);
bool LinkFourthStageToFrame(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address);
bool LinkFourthStageToFrame_physical(void *p_physical_memory, MemoryPageTable *bsp_table, uint64_t start_address, uint64_t physical_address);
/*
 * ─── TO BE SERVICED  ───────────────────────────────────────────────────────────
 */

bool initVirtualMemoryService(MemoryPageTable **p_memoryPageTable, Service *p_service);
bool LinkThirdStageToCollection_Wrapper(void *p_virtualMemory);
bool LinkThirdStageToCollectionPhysical_Wrapper(void *p_virtualMemory);
bool LinkFourthStageToFrame_Wrapper(void *p_virtualMemory);
bool LinkFourthStageToFramePhysical_Wrapper(void *p_virtualMemory);
void DeallocateVirtualMemoryCollection(void *p_physical_memory, MemoryPageTable *page_table, uint64_t start_address);

void AllocateSharedMemoryTwoMb(uint64_t virtualAddressStart, uint64_t size);
void DeallocateSharedMemoryTwoMb(uint64_t virtualAddressStart, uint64_t size);
void refreshReserved();

#endif