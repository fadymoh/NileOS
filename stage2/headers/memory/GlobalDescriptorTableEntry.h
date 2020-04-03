#ifndef GLOBALDESCRIPTORTABLEENTRY_H_
#define GLOBALDESCRIPTORTABLEENTRY_H_

#include "includes.h"
#include "defines.h"

struct GlobalDescriptorTableEntry_s
{
    // GDT Entry Attribute Fields
    uint32_t limit_0_15 : 16; // Limit field bits 0-15
    uint32_t base_0_15 : 16;  // Base address bits 0-15
    uint32_t base_16_23 : 8;  // Base address bits 16-23
    uint32_t access : 8;      // Access
    uint32_t limit_16_19 : 4; // Limit field bits 16-19
    uint32_t available : 1;   // Flags
    uint32_t longmode : 1;
    uint32_t special : 1;
    uint32_t granularity : 1;
    uint32_t base_24_31 : 8; // Base address bits 24-31
} __attribute__((__packed__));

typedef struct GlobalDescriptorTableEntry_s GlobalDescriptorTableEntry;

void setGlobalDescriptorTableEntry(GlobalDescriptorTableEntry *p_globalDescriptorTableEntry,
                                   uint16_t p_limit_0_15,
                                   uint16_t p_base_0_15,
                                   uint8_t p_base_16_23,
                                   uint8_t p_access,
                                   uint8_t p_limit_16_19,
                                   uint8_t p_available,
                                   uint8_t p_longmode,
                                   uint8_t p_special,
                                   uint8_t p_granularity,
                                   uint8_t p_base_24_31);

#endif /* GLOBALDESCRIPTORTABLEENTRY_H_ */
