#include "BlockService.h"
#include "Kernel.h"
extern Kernel kernel;

/*
typedef struct
{
  uint64_t* buffer[2];
  bool primaryBuffer;
  uint64_t* seeksTable;
  uint64_t* current_cache_index[2];
  uint64_t current_read;
  uint64_t total_read;
  uint64_t target_read;

} BlockService;
*/
bool init_BlockService()
{
    kernel.blockService.buffer[0] = (uint8_t *)kvalloc(&kernel.memoryAllocator, sizeof(uint8_t) * 32 * sector_unit);
    kernel.blockService.buffer[1] = (uint8_t *)kvalloc(&kernel.memoryAllocator, sizeof(uint8_t) * 32 * sector_unit);
    memset(kernel.blockService.buffer[0], 0, sizeof(uint8_t) * 32 * sector_unit);
    memset(kernel.blockService.buffer[1], 0, sizeof(uint8_t) * 32 * sector_unit);

    kernel.blockService.primaryBuffer = false;
    kernel.blockService.current_cache_index[0] = -1;
    kernel.blockService.current_cache_index[1] = -1;
    kernel.blockService.skip_start[0] = -1;
    kernel.blockService.skip_start[1] = -1;
    kernel.blockService.skip_end[0] = -1;
    kernel.blockService.skip_end[1] = -1;
}
bool read_blocks(ATADisk *ataDisk, uint8_t *user_buffer)
{
    printk("in read blocks\n");
    uint64_t start_sector = kernel.blockService.start_sector;
    uint64_t end_sector = kernel.blockService.blocks_to_read_number * sector_unit + start_sector;
    uint64_t chunck = 32 * sector_unit;

    uint64_t b0_index = kernel.blockService.current_cache_index[0];
    uint64_t b1_index = kernel.blockService.current_cache_index[1];
    printk("blocks to read: %d\n", kernel.blockService.blocks_to_read_number);
    printk("blocks to read: %d\n", kernel.blockService.blocks_to_read_number);

    if (b0_index == -1 && b1_index == -1)
    {
        printk("reading without cache\n");
        read_nBlocks(ataDisk, user_buffer, kernel.blockService.blocks_to_read_number);
    }
    else
    {
        printk("reading with cache\n");
        for (int i = 0; i < 2; i++)
        {
            if (kernel.blockService.current_cache_index[i] <= start_sector && kernel.blockService.current_cache_index[i] + chunck > start_sector)
            {
                uint64_t user_start = start_sector - kernel.blockService.current_cache_index[i];

                uint64_t user_end = end_sector - user_start;
                if (user_end > chunck)
                {
                    user_end = chunck - user_start;
                }
                fast_memcpy(user_buffer,
                            kernel.blockService.buffer[i] + user_start * 512,
                            user_end * 512);
                kernel.blockService.skip_start[i] = start_sector;
                kernel.blockService.skip_end[i] = start_sector + user_end;
                printk("current cache index: %d\n", kernel.blockService.current_cache_index[i]);
                printk("sector start: %d\n", start_sector);
                printk("sector end: %d\n", end_sector);
                printk("user start: %d\n", user_start);
                printk("user end: %d\n", user_end);
                printk("skip start: %d\n", start_sector);
                printk("skip end: %d\n", start_sector + user_end);

            }
            else if (kernel.blockService.current_cache_index[i] > start_sector && kernel.blockService.current_cache_index[i] < start_sector + end_sector)
            {
                uint64_t user_start = kernel.blockService.current_cache_index[i] - start_sector;
                uint64_t user_end = end_sector - user_start;
                if (user_end > chunck)
                {
                    user_end = chunck - user_start;
                }
                fast_memcpy(user_buffer + user_start * 512,
                            kernel.blockService.buffer[i],
                            user_end * 512);
                kernel.blockService.skip_start[i] = user_start;
                kernel.blockService.skip_end[i] = user_start + user_end;
                printk("current cache index: %d\n", kernel.blockService.current_cache_index[i]);
                printk("sector start: %d\n", start_sector);
                printk("sector end: %d\n", end_sector);
                printk("user start: %d\n", user_start);
                printk("user end: %d\n", user_end);
                printk("skip start: %d\n", user_start);
                printk("skip end: %d\n", user_start + user_end);
            }
        }
        read_nBlocks(ataDisk, user_buffer, kernel.blockService.blocks_to_read_number);
        kernel.blockService.skip_start[0] = -1;
        kernel.blockService.skip_start[1] = -1;
        kernel.blockService.skip_end[0] = -1;
        kernel.blockService.skip_end[1] = -1;
    }
}
bool get_file(ATADisk *ataDisk, uint8_t *user_buffer) {}

bool read_nBlocks(ATADisk *ataDisk, uint8_t *user_buffer, uint64_t num_blocks)
{
    printk("in read nnnnnnblocks\n");
    uint64_t current_read = 0;
    uint64_t total_read = 0;
    uint64_t target_read = num_blocks * sector_unit * 512;
    uint64_t chunck = 32 * sector_unit * 512;
    uint64_t start_sector = kernel.blockService.start_sector;
    if (total_read + start_sector >= kernel.blockService.skip_start[0] && total_read < kernel.blockService.skip_end[0])
    {
        total_read += (kernel.blockService.skip_end[0] - start_sector) * 512;
        printk("start sector: %d\n", start_sector);
        printk("end sector: %d\n", kernel.blockService.skip_end[0]);
        // printk("the difference: %d\n", kernel.blockService.skip_end[0]-start_sector);
        // printk("the difference with 512: %d\n", (start_sector-kernel.blockService.skip_end[0])*512);
        printk("the addition of 512: %d\n", total_read);
        printk("without 512: %d\n", total_read / 512);
    }
    if (total_read + start_sector >= kernel.blockService.skip_start[1] && total_read < kernel.blockService.skip_end[1])
    {
        total_read += (kernel.blockService.skip_end[1] - start_sector) * 512;
    }
    // printk("skip start 0:%d,  skip end 0: %d\n", kernel.blockService.skip_start[0], kernel.blockService.skip_end[0]);
    // printk("skip start 1:%d,  skip end 1: %d\n", kernel.blockService.skip_start[1], kernel.blockService.skip_end[1]);

    // printk("total read is:%d\n", total_read / 512);
    // printk("target read is:%d\n", target_read / 512);

    while (target_read - total_read > 0)
    {
        printk("in read nnnnnnblocks whiiiile\n");
        for (int i = 0; i < 4; i++)
        {
            int l = i % 2;
            if ((total_read + start_sector) * 512 >= kernel.blockService.skip_start[l] * 512 &&
                total_read < kernel.blockService.skip_end[l] * 512)
            {
                total_read += (kernel.blockService.skip_end[l] - (start_sector + total_read / 512)) * 512;
            }
        }
        printk("continue normally: %d\n", total_read / 512);
        if (target_read - total_read > chunck)
        {
            current_read = chunck;
        }
        else
        {
            current_read = target_read - total_read;
        }
        bool index = kernel.blockService.primaryBuffer;
        // printk("current read: %d\n", current_read / 512);
        printk("read starting sector number: %d\n", start_sector + (total_read / 512));

        memset(kernel.blockService.buffer[index], 0, sizeof(uint8_t) * 32 * sector_unit);
        

        // printk("total read is : %d and %x\n", total_read, total_read);
        // printk("current read is : %d and %x\n", current_read, current_read);
        //! syscall start
        spinlock_lock(&kernel.dmaBuffer.spinlock);
        printk ("syscall_read_file after lock\n");
       
        kernel.dmaBuffer.enabled = true;
        kernel.dmaBuffer.total_read = 0;
        // printk ("1:before readDMADisk: %x\n",ataDisk);
        while (readDMADisk(ataDisk, kernel.blockService.start_sector+ (total_read / 512) , kernel.blockService.buffer[index], 32 * sector_unit, 0, 0)
         == READ_DMA_FAIL)
            ;
        // printk ("1:after readDMADisk\n");
        kernel.apicManager.apics[0].fired_interrupts[ATA_IPI] = 0;
        while (kernel.apicManager.apics[0].fired_interrupts[ATA_IPI] == 0)
        {
            // printk("stuck\n");
            asm volatile("sti;");
            asm volatile("hlt;");
            asm volatile("cli;");
            DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
            sendAPICEOI(&kernel.apicManager.apics[kernel.apicManager.returns.core_id]);
            if (kernel.dmaBuffer.total_read == kernel.dmaBuffer.target_read)
                break;
        }
        printk("Out of syscall_read_file loop\n");
        kernel.apicManager.apics[0].fired_interrupts[ATA_IPI] = 0;
        kernel.dmaBuffer.enabled = false;
        spinlock_unlock(&kernel.dmaBuffer.spinlock);
        //!syscall end
        //        printk("before fast memcopy for nblocks\n");
        fast_memcpy(user_buffer + (total_read),
                    kernel.blockService.buffer[index],
                    current_read);

        //        printk("read done blo: %d\n", ataDisk->done_read);

        printk("after fast memcopy for nblocks\n");
        // for (int i = 0; i < 5 * sector_unit; ++i)
        //     printk("%c", user_buffer[i]);

        // printk("\nDONE!\n");
        //!should add the sector start
        kernel.blockService.current_cache_index[index] = total_read / 512;
        
        total_read += current_read;
        // printk("prev cache %d at address %x\n", kernel.blockService.primaryBuffer, kernel.blockService.buffer[kernel.blockService.primaryBuffer]);

        kernel.blockService.primaryBuffer = !kernel.blockService.primaryBuffer;
        // printk("current cache %d at address %x\n", kernel.blockService.primaryBuffer, kernel.blockService.buffer[kernel.blockService.primaryBuffer]);
    }
}
