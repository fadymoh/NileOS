#ifndef TSSENTRY_H_
#define TSSENTRY_H_

#include "includes.h"
#include "defines.h"

#define TSS_BASIC_SIZE  104             // Size of the TSSEntry in bytes excluding the iomap
#define TSS_IOMAP_SIZE  (8 * 1024 + 1)  // 8K for bitmap + 1 terminating byte for convenience

struct TSSEntry_s
{
        // TSS Attributes
        uint32_t reserve1;
        uint64_t rsp0;
        uint64_t rsp1;
        uint64_t rsp2;
        uint64_t reserve2;
        uint64_t ist1;
        uint64_t ist2;
        uint64_t ist3;
        uint64_t ist4;
        uint64_t ist5;
        uint64_t ist6;
        uint64_t ist7;
        uint64_t reserve3;
        uint16_t reserve4;
        uint16_t iomap_base;
        uint8_t iomap[TSS_IOMAP_SIZE];

} __attribute__ ((__packed__));

typedef  struct TSSEntry_s  TSSEntry;

#endif /* TSSENTRY_H_ */
