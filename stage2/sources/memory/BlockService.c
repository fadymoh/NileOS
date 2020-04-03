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
}
bool read_blocks(ATADisk *ataDisk, uint8_t *user_buffer)
{
    printk("in read blocks\n");
    // kernel.blockService.current_user_buffer_index = 0;
    uint64_t sector_number = 52825;
    uint64_t chunck = 32 * sector_unit;

    uint64_t b0_index = kernel.blockService.current_cache_index[0];
    uint64_t b1_index = kernel.blockService.current_cache_index[1];
    if (b0_index == -1 && b1_index == -1)
        read_nBlocks(ataDisk, user_buffer, 52825, 65);
    else
    {
        if (b0_index <= 52825 && b0_index + chunck > 52825)
        {
            //return b0
        }
        else if (b1_index <= 52825 && b1_index + chunck > 52825)
        {
            //return b1
        }
    }
}
bool get_file(ATADisk *ataDisk, uint8_t *user_buffer) {}

bool read_nBlocks(ATADisk *ataDisk, uint8_t *user_buffer, uint64_t seek, uint64_t num_blocks)
{
    // printk("in read nnnnnnblocks\n");
    kernel.blockService.current_read = 0;
    kernel.blockService.total_read = 0;
    kernel.blockService.target_read = num_blocks * sector_unit * 512;
    uint64_t chunck = 32 * sector_unit * 512;

    while (kernel.blockService.target_read - kernel.blockService.total_read)
    {
        // printk("in read nnnnnnblocks whiiiile\n");
        if (kernel.blockService.target_read - kernel.blockService.total_read > chunck)
        {
            kernel.blockService.current_read = chunck;
        }
        else
        {
            kernel.blockService.current_read = kernel.blockService.target_read - kernel.blockService.total_read;
        }
        bool index = kernel.blockService.primaryBuffer;

        memset(kernel.blockService.buffer[index], 0, sizeof(uint8_t) * 32 * sector_unit);
        readDMADisk(ataDisk, seek, kernel.blockService.buffer[index], 32 * sector_unit, 0, 0);
        // printk("before fast memcopy for nblocks\n");

        // printk("total read is : %d and %x\n", kernel.blockService.total_read,kernel.blockService.total_read);
        // printk("current read is : %d and %x\n", kernel.blockService.current_read,kernel.blockService.current_read);
        // fast_memcpy(user_buffer + (kernel.blockService.total_read),
        //         kernel.blockService.buffer[index],
        //         kernel.blockService.current_read);
        // printk("dma disk: %x\n", ataDisk);
        volatile bool done = false;
        while (!done)
        {
            done = ataDisk->done_read;
        }
        fast_memcpy(user_buffer + (kernel.blockService.total_read),
                    kernel.blockService.buffer[index],
                    kernel.blockService.current_read);

        // printk("read done blo: %d\n", ataDisk->done_read);

        printk("after fast memcopy for nblocks\n");
        for (int i = 0; i < 32 * sector_unit; ++i)
            printk("%c", kernel.blockService.buffer[index][i]);

        printk("\nDONE!\n");
        kernel.blockService.total_read += kernel.blockService.current_read;
        printk("prev cache %d at address %x\n", kernel.blockService.primaryBuffer, kernel.blockService.buffer[kernel.blockService.primaryBuffer]);
        // for (int i = 0; i < num_blocks * sector_unit; ++i)
        //    printk("%c",user_buffer[i]);

        kernel.blockService.primaryBuffer = !kernel.blockService.primaryBuffer;
        printk("current cache %d at address %x\n", kernel.blockService.primaryBuffer, kernel.blockService.buffer[kernel.blockService.primaryBuffer]);
    }

    //seek--> direct to dmaread
    //num of blocks * sector_unit in the dma read function
    //cache buffer --> dma read
    //append to user buffer
}
