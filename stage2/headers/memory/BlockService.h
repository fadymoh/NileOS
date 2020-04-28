#ifndef BLOCKSERVICE_H_
#define BLOCKSERVICE_H_
#include "defines.h"
#include "includes.h"
#include "ATADisk.h"

#define sector_unit 2048
typedef struct BlockService_s
{
  uint8_t *buffer[2];
  bool primaryBuffer;
  uint64_t start_sector;
  uint64_t current_cache_index[2];
  uint64_t current_read;
  uint64_t total_read;
  uint64_t target_read;
  uint64_t blocks_to_read_number;
  uint64_t skip_start[2];
  uint64_t skip_end[2];
} BlockService;

bool init_BlockService();
bool read_blocks(ATADisk *ataDisk, uint8_t *user_buffer);
bool get_file(ATADisk *ataDisk, uint8_t *user_buffer);
bool read_nBlocks(ATADisk *ataDisk, uint8_t *user_buffer, uint64_t num_blocks);
#endif