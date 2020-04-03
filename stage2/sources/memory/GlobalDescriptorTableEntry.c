#include "GlobalDescriptorTableEntry.h"

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
                                   uint8_t p_base_24_31)
{
    p_globalDescriptorTableEntry->limit_0_15 = p_limit_0_15;
    p_globalDescriptorTableEntry->base_0_15 = p_base_0_15;
    p_globalDescriptorTableEntry->base_16_23 = p_base_16_23;
    p_globalDescriptorTableEntry->limit_16_19 = p_limit_16_19;
    p_globalDescriptorTableEntry->base_24_31 = p_base_24_31;

    // set the flags
    p_globalDescriptorTableEntry->available = p_available;
    p_globalDescriptorTableEntry->longmode = p_longmode;
    p_globalDescriptorTableEntry->special = p_special;
    p_globalDescriptorTableEntry->granularity = p_granularity;
    p_globalDescriptorTableEntry->access = p_access;
}