#include "CoreUtils.h"
#include "Ports.h"

uint32_t createMask(uint32_t p_value, uint8_t p_start, uint8_t p_end)
{
    return MID(p_value, p_start, p_end);
}

uint32_t getAPICId(uint64_t p_reg)
{
    uint32_t cpuid = readLocalAPIC(p_reg);
    cpuid = createMask(cpuid, 24, 31);
    return cpuid;
}

void writeLocalAPIC(uint64_t p_reg, uint32_t p_val)
{
    *(volatile uint32_t *)((uint64_t)p_reg) = p_val;
}

uint32_t readLocalAPIC(uint64_t p_reg)
{
    return (uint32_t)(*(volatile uint32_t *)(p_reg));
}
