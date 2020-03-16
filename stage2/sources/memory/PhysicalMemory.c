#include "PhysicalMemory.h"
#include "Kernel.h"
extern Kernel kernel;

bool initializePhysicalMemory(PhysicalMemoryManager *p_physicalmemory_ptr, Service *p_service)
{
    uint16_t i = 0;

    p_physicalmemory_ptr->memory_entries = 0;
    p_physicalmemory_ptr->usable_memory_size = 0;
    p_physicalmemory_ptr->reserved_memory_size = 0;
    p_physicalmemory_ptr->maximum_address = 0;
    p_physicalmemory_ptr->hole_memory_size = 0;
    p_physicalmemory_ptr->physicalMemoryEntry = NULL;
    void *ptr1 = (void *)0x0;
    char *temp = scanmem(ptr1, E820_MEMORY_INDICATOR, 1024 * 1024, E820_MEMORY_INDICATOR_LENGTH - 1);
    if (temp != NULL)
    {
        p_physicalmemory_ptr->physicalMemoryEntry = (PhysicalMemoryEntry *)((temp) + E820_MEMORY_INDICATOR_LENGTH);

        p_physicalmemory_ptr->memory_entries = *((uint16_t *)((temp) + E820_MEMORY_INDICATOR_LENGTH + E820_MEMORY_ENTRY_TABLE_SIZE));
        for (i = 0; i < p_physicalmemory_ptr->memory_entries; i++)
        {
            p_physicalmemory_ptr->physicalMemoryEntry[i].end = p_physicalmemory_ptr->physicalMemoryEntry[i].start +
                                                               p_physicalmemory_ptr->physicalMemoryEntry[i].size;

            if (p_physicalmemory_ptr->physicalMemoryEntry[i].type == E820_MEMORY_ENTRY_TYPE_USABLE)
            {
                p_physicalmemory_ptr->usable_memory_size += p_physicalmemory_ptr->physicalMemoryEntry[i].size;
                if (p_physicalmemory_ptr->physicalMemoryEntry[i].start + p_physicalmemory_ptr->physicalMemoryEntry[i].size > p_physicalmemory_ptr->maximum_address)
                    p_physicalmemory_ptr->maximum_address = p_physicalmemory_ptr->physicalMemoryEntry[i].start + p_physicalmemory_ptr->physicalMemoryEntry[i].size;
            }
            else if (p_physicalmemory_ptr->physicalMemoryEntry[i].type == E820_MEMORY_ENTRY_TYPE_RESEARVED ||
                     p_physicalmemory_ptr->physicalMemoryEntry[i].type == E820_MEMORY_ENTRY_TYPE_ACPI_NVS ||
                     p_physicalmemory_ptr->physicalMemoryEntry[i].type == E820_MEMORY_ENTRY_TYPE_ACPI_RECLAINMED)
                p_physicalmemory_ptr->reserved_memory_size += p_physicalmemory_ptr->physicalMemoryEntry[i].size;

            /*   if ( i > 0 ) {  // Check Previous
                
                        if ( p_physicalMemoryManager->physicalMemoryEntry[i-1].start + p_physicalMemoryManager->physicalMemoryEntry[i-1].size < p_physicalMemoryManager->physicalMemoryEntry[i].start)
                        {
                            p_physicalMemoryManager->physicalMemoryEntry[i-1].size = p_physicalMemoryManager->physicalMemoryEntry[i].start - p_physicalMemoryManager->physicalMemoryEntry[i-1].start;
                        p_physicalMemoryManager->physicalMemoryEntry[i-1].end = p_physicalMemoryManager->physicalMemoryEntry[i-1].start + p_physicalMemoryManager->physicalMemoryEntry[i-1].size;
                        }
            }*/
        }
        //initializing all the functions needed area

        p_service->add_service(p_service, printPhysicalMemory, PrintPhysicalMemory);
        p_service->add_service(p_service, addPhysicalMemoryEntry, AddPhysicalMemoryEntry);
        p_service->add_service(p_service, getVirtualAddress, GetVirtualAddress);
        p_service->add_service(p_service, isUsableAddress, isUsable);
        p_service->add_service(p_service, isAddress, isaddress);
        p_service->add_service(p_service, createPhysicalPagesBitMap, CreatePhysicalPagesBitMap);
        p_service->add_service(p_service, initializePhysicalMemoryFrame, InitializePhysicalMemoryFrame);
        p_service->add_service(p_service, allocatePhysicalFrames, AllocatePhysicalFrames);
        p_service->add_service(p_service, deallocatePhysicalFrame, DeallocatePhysicalFrame);
        //---------------------------------------------

        p_physicalmemory_ptr->reserved_plus_usable_size = p_physicalmemory_ptr->reserved_memory_size + p_physicalmemory_ptr->usable_memory_size;

        return true;
    }
    else
        return false;
}

void printPhysicalMemory(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;

    printk("Number of memory entries: %d\n", p_physicalMemoryManager->memory_entries);
    printk("Number Usable Memory: %d MB\n", p_physicalMemoryManager->usable_memory_size / (1024 * 1024));
    printk("Number Reserved Memory: %d KB\n", p_physicalMemoryManager->reserved_memory_size / 1024);

    for (int i = 0; i < p_physicalMemoryManager->memory_entries; i++)
    {
        printk("Start: %x      Size: %x       Type: %s     vstart: %x\n",
               p_physicalMemoryManager->physicalMemoryEntry[i].start,
               p_physicalMemoryManager->physicalMemoryEntry[i].size,
               mem_types_desc[p_physicalMemoryManager->physicalMemoryEntry[i].type - 1],
               p_physicalMemoryManager->physicalMemoryEntry[i].vstart);
    }
}

bool isAddress(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;

    uint16_t i = 0;
    for (i = 0; i < p_physicalMemoryManager->memory_entries; i++)
    {
        if (p_physicalMemoryManager->params.p_physical_address >= p_physicalMemoryManager->physicalMemoryEntry[i].start &&
            p_physicalMemoryManager->params.p_physical_address < p_physicalMemoryManager->physicalMemoryEntry[i].start +
                                                                     p_physicalMemoryManager->physicalMemoryEntry[i].size)
            return true;
    }
    return false;
}

bool isUsableAddress(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    for (uint16_t i = 0; i < p_physicalMemoryManager->memory_entries; i++)
    {
        if ((p_physicalMemoryManager->params.p_physical_address >= p_physicalMemoryManager->physicalMemoryEntry[i].start &&
                 p_physicalMemoryManager->params.p_physical_address + p_physicalMemoryManager->params.p_size < p_physicalMemoryManager->physicalMemoryEntry[i].end &&
                 (p_physicalMemoryManager->physicalMemoryEntry[i].type == E820_MEMORY_ENTRY_TYPE_USABLE) ||
             p_physicalMemoryManager->params.p_physical_address < FIRST_MEMORY_1_MiB))
            return true;
    }
    return false;
}

uint64_t getVirtualAddress(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    uint16_t i = 0;
    for (i = 0; i < p_physicalMemoryManager->memory_entries; i++)
    {
        if (p_physicalMemoryManager->params.p_physical_address >= p_physicalMemoryManager->physicalMemoryEntry[i].start &&
            p_physicalMemoryManager->params.p_physical_address < p_physicalMemoryManager->physicalMemoryEntry[i].end)
        {
            return p_physicalMemoryManager->physicalMemoryEntry[i].vstart +
                   (p_physicalMemoryManager->params.p_physical_address - p_physicalMemoryManager->physicalMemoryEntry[i].start);
        }
    }
    return 0x0;
}

void addPhysicalMemoryEntry(void *p_physicalmemory_ptr)
{

    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    if (!isAddress(p_physicalmemory_ptr)) //sending the original void* pointer. Need to validate that it works!
    {
        //printk ("addPhysicalMemoryEntry: %x\n",p_physical_address);
        p_physicalMemoryManager->physicalMemoryEntry[p_physicalMemoryManager->memory_entries].start = p_physicalMemoryManager->params.p_physical_address;
        p_physicalMemoryManager->physicalMemoryEntry[p_physicalMemoryManager->memory_entries].size = p_physicalMemoryManager->params.p_size;
        p_physicalMemoryManager->physicalMemoryEntry[p_physicalMemoryManager->memory_entries].type = E820_MEMORY_ENTRY_TYPE_RESEARVED;
        p_physicalMemoryManager->physicalMemoryEntry[p_physicalMemoryManager->memory_entries].end =
            p_physicalMemoryManager->physicalMemoryEntry[p_physicalMemoryManager->memory_entries].start +
            p_physicalMemoryManager->physicalMemoryEntry[p_physicalMemoryManager->memory_entries].size;
        p_physicalMemoryManager->memory_entries++;
    }
    //else printk ("!!!!! addPhysicalMemoryEntry: %x\n",p_physical_address);
}

uint64_t getReservedAndUsableMemorySize(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    uint64_t total = p_physicalMemoryManager->usable_memory_size + p_physicalMemoryManager->reserved_memory_size;
    return total;
}
void set_bit(char *initial_bitmap_address, int bit_number, int v)
{
    // get char index
    char *indexed_bitmap = &(initial_bitmap_address[bit_number / 8]);
    *(indexed_bitmap) |= (1 << (7 - (bit_number % 8)));
    int num = 0 | *(indexed_bitmap);

    if (!v)
        return;
}
void clear_bit(char *initial_bitmap_address, int bit_number, int v)
{
    // get char index
    char *indexed_bitmap = &(initial_bitmap_address[bit_number / 8]);
    *(indexed_bitmap) &= (char)(~((char)(1 << (7 - (bit_number % 8)))));

    int num = 0 + (unsigned char)*((unsigned char *)indexed_bitmap);
}
int get_bit(char *initial_bitmap_address, int bit_number, int v)
{
    char *indexed_bitmap = &(initial_bitmap_address[bit_number / 8]);
    int num = 0 + (unsigned char)*((unsigned char *)indexed_bitmap);

    return (*(indexed_bitmap) >> (7 - (bit_number % 8))) & 1;
}
void createPhysicalPagesBitMap(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    uint64_t total_memory = getReservedAndUsableMemorySize(p_physicalmemory_ptr);
    int number_of_page_frames = total_memory / FOUR_KiB_MEMORY_PAGE_SIZE;
    printk("number of page frames: %d\n", number_of_page_frames);
    int physical_memory_start = 0x0;
    /*
     * ─── ALLOCATE MEMORY FOR BITMAP ─────────────────────────────────────────────────
     */
    p_physicalMemoryManager->physical_memory_bitmap = kmalloc(&(kernel.memoryAllocator),
                                                              number_of_page_frames / 8);
    /* ──────────────────────────────────────────────────────────────────────────────── */

    int numberofusableframes = 0;
    int kernelFrames = KERNEL_MEMORY_SIZE / FOUR_KiB_MEMORY_PAGE_SIZE;
    for (int page_frame_number = 0; page_frame_number < number_of_page_frames; ++page_frame_number)
    {
        uint64_t address = page_frame_number * FOUR_KiB_MEMORY_PAGE_SIZE;
        kernel.physicalMemoryManager.params.p_physical_address = address;
        kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE;
        if (isUsableAddress(p_physicalmemory_ptr) && page_frame_number >= kernelFrames)
        {
            // set the bit to 1
            set_bit(p_physicalMemoryManager->physical_memory_bitmap, page_frame_number, 0);
        }
        else
        {
            // set the bit to 0
            clear_bit(p_physicalMemoryManager->physical_memory_bitmap, page_frame_number, 0);
        }
    }

    for (int page_frame_number = 0; page_frame_number < number_of_page_frames; ++page_frame_number)
    {
        if (get_bit(p_physicalMemoryManager->physical_memory_bitmap, page_frame_number, 0))
        {
            ++numberofusableframes;
        }
    }
    //printk("number of usable frames: %d\n", numberofusableframes);
    //printk("number of frames overall: %d\n", number_of_page_frames);
    p_physicalMemoryManager->available_frame = numberofusableframes;
    // printk("number of usable frames: %d\n", numberofusableframes);
    // printk("number of usable frames from memory: %d\n", p_physicalMemoryManager->usable_memory_size / FOUR_KiB_MEMORY_PAGE_SIZE);
    // printk("total number of frames: %d\n", number_of_page_frames);
}

void initializePhysicalMemoryFrame(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    p_physicalMemoryManager->physical_memory_frame_collection_ptr = (PhysicalMemoryFrameCollection *)kmalloc(&(kernel.memoryAllocator),
                                                                                                             sizeof(PhysicalMemoryFrameCollection) * (p_physicalMemoryManager->available_frame / 512));
    int number_of_page_frames = p_physicalMemoryManager->available_frame;
    int mx_physical_memory_frame_collections = p_physicalMemoryManager->number_physical_memory_frame_collection =
        number_of_page_frames / 512;

    for (int page_frame_number = 0; page_frame_number < mx_physical_memory_frame_collections; ++page_frame_number)
    {
        int available_frames = 0;
        int end_of_frames = page_frame_number * 512 + 512;
        for (int frames_start = page_frame_number * 512; frames_start < end_of_frames; ++frames_start)
        {
            if (get_bit(p_physicalMemoryManager->physical_memory_bitmap, frames_start, 0))
            {
                ++available_frames;
            }
        }
        // assign to a physical frame collection
        p_physicalMemoryManager->physical_memory_frame_collection_ptr[page_frame_number].available_frames = available_frames;
        p_physicalMemoryManager->physical_memory_frame_collection_ptr[page_frame_number].initial_physical_address =
            page_frame_number * 512 * FOUR_KiB_MEMORY_PAGE_SIZE;
    }
}

/*
 * ─── RETURN THE PHYSICAL ADDRESS ────────────────────────────────────────────────
 */
uint64_t allocateOnePhysicalFrame(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    // spin lock

    /* ──────────────────────────────────────────────────────────────────────────────── */
    int size = p_physicalMemoryManager->number_physical_memory_frame_collection;
    for (int i = 0; i < size; ++i)
    {
        if (p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].available_frames > 0)
        {
            int start_bit_index = (p_physicalMemoryManager->physical_memory_frame_collection_ptr[i]
                                       .initial_physical_address /
                                   512 / FOUR_KiB_MEMORY_PAGE_SIZE) *
                                  512;
            int end_bit_address = start_bit_index + 512;

            for (int page_number = start_bit_index; page_number < end_bit_address; ++page_number)
            {
                if (get_bit(p_physicalMemoryManager->physical_memory_bitmap, page_number, 1))
                {
                    clear_bit(p_physicalMemoryManager->physical_memory_bitmap, page_number, 1);
                    p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].available_frames--;

                    return page_number * FOUR_KiB_MEMORY_PAGE_SIZE;
                }
            }
        }
    }
    // spin unlock

    return -1;
}
void allocatePhysicalFrames(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    int number_of_frames_to_allocate = p_physicalMemoryManager->params.number_of_frames_to_allocate;
    for (int i = 0; i < number_of_frames_to_allocate; ++i)
    {
        uint64_t physical_address = allocateOnePhysicalFrame(p_physicalmemory_ptr);
        if (physical_address == -1)
            printk("failed to allocate a page\n");
        // else
        //    printk("address allocated: %d \n", physical_address);
    }
}

void deallocatePhysicalFrame(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;

    //Spin lock hereee

    uint64_t frame_to_deall = p_physicalMemoryManager->params.p_physical_address / 4096; //to get the frame.. each frame is 4KB

    if (get_bit(p_physicalMemoryManager->physical_memory_bitmap, frame_to_deall, 1))
    {
        printk("deallocating of an unallocated memory.. should not happen!\n");
        return;
    }

    //printk("Frame to De-allocate: %d\n", frame_to_deall);
    set_bit(p_physicalMemoryManager->physical_memory_bitmap, frame_to_deall, 1);

    uint64_t address_of_collection = (frame_to_deall / 512) * TWO_MiB_MEMORY_PAGE_SIZE; //not sure.. double check!
    //printk("Status of dealloc frame: %d\n", get_bit(p_physicalMemoryManager->physical_memory_bitmap, frame_to_deall, 1));
    //  printk("address of collection: %d\n", address_of_collection);

    int size = p_physicalMemoryManager->number_physical_memory_frame_collection;
    for (int i = 0; i < size; ++i)
    {
        if (p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].initial_physical_address == address_of_collection)
        {
            p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].available_frames++;

            //printk("Physical Memory Collection index: %d\n", i);
            // printk("Physical Memory Collection available frames: %d\n", p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].available_frames);

            // should sort afterwards
        }
    }

    //Spin Unlock hereeeeee
}

void deallocateOnePhysicalFrameCollection(void *p_physicalmemory_ptr)
{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    uint64_t frame_to_deall = p_physicalMemoryManager->params.p_physical_address; //to get the frame.. each frame is 4KB

    for (uint64_t start = frame_to_deall; start < frame_to_deall + TWO_MiB_MEMORY_PAGE_SIZE; start += FOUR_KiB_MEMORY_PAGE_SIZE)
    {
        p_physicalMemoryManager->params.p_physical_address = start; 
        deallocatePhysicalFrame(p_physicalmemory_ptr);
    }
}

/*
 * ─── ALLOCATING A COLLECTION THAT HAS 512 FREE PHYSICAL FRAMES ──────────────────
 */

uint64_t allocateOnePhysicalFrameCollection(void *p_physicalmemory_ptr)
{

    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;
    // spin lock

    int size = p_physicalMemoryManager->number_physical_memory_frame_collection;
    // printk("Size: %d\n", size);

    for (int i = 0; i < size; ++i)
    {
        if (p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].available_frames == 512)
        {
            int start_bit_index = (p_physicalMemoryManager->physical_memory_frame_collection_ptr[i]
                                       .initial_physical_address /
                                   512 / FOUR_KiB_MEMORY_PAGE_SIZE) *
                                  512;
            int end_bit_address = start_bit_index + 512;

            for (int page_number = start_bit_index; page_number < end_bit_address; ++page_number)
            {
                if (get_bit(p_physicalMemoryManager->physical_memory_bitmap, page_number, 0)) //reduant check.. but let's leave it for now
                {
                    clear_bit(p_physicalMemoryManager->physical_memory_bitmap, page_number, 0);
                    p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].available_frames--;
                }
                else
                {
                    printk("There is a problem that happned when allocating a frame. a bit in between was already cleared");
                }
            }
            //printk("index of collection: %d\n", i);
            return p_physicalMemoryManager->physical_memory_frame_collection_ptr[i].initial_physical_address;
        }
    }

    return -1;
}

/*
 * ─── GETS YOU A PHYSICAL MEMORY COLLECTION 2MB ─────────────────────────────────
 */

uint64_t AllocateUsableCollection(void *p_physicalmemory_ptr)

{
    PhysicalMemoryManager *p_physicalMemoryManager = (PhysicalMemoryManager *)p_physicalmemory_ptr;

    //uint64_t start_address_after_kernel = kernel.memoryAllocator.kernel_memory_end_address;

    uint64_t physical_address = allocateOnePhysicalFrameCollection(p_physicalMemoryManager);

    return physical_memory;
    //if (physical_address == -1)
    //printk("failed to allocate a collection\n");
    //else
    //printk("address allocated: %d \n", physical_address);
}