#include "TSSDescriptor.h"

/* 
 * This is a setter method which sets the limit data members
 */
void setTSSDescriptorLimit (TSSDescriptor * p_tssDescriptor, uint32_t p_limit)
{
    p_tssDescriptor->limit_0_15 = p_limit & 0xffffU; 
    p_tssDescriptor->limit_16_19 = (p_limit >> 16) & 0x0fU;
}

/* 
 * This is a setter method which sets the base data members
 * 
 * 
 */


void setTSSDescriptorBase (TSSDescriptor * p_tssDescriptor,uint64_t p_base)
{
    p_tssDescriptor->base_0_15 = p_base & 0xffffU; 
    p_tssDescriptor->base_16_23 = ((p_base) >> 16) & 0xffU;
    p_tssDescriptor->base_24_31 = ((p_base) >> 24) & 0xffU;
    p_tssDescriptor->base_32_63 = ((p_base) >> 32);
}
/*

 * This is an initialization method which initializes a set of data members.
 * 
 * This method receives 5 parameters:
 *              uint64_t p_base
 *              uint32_t p_limit
 *              uint8_t p_type
 *              uint8_t p_present
 *              uint8_t p_dpl
 * 
 */

void initializeTSSDescriptor (TSSDescriptor * p_tssDescriptor, uint64_t p_base,
                                uint32_t p_limit,
                                uint8_t p_type,
                                uint8_t p_present,
                                uint8_t p_dpl)
{
    setTSSDescriptorBase (p_tssDescriptor,p_base);
    setTSSDescriptorLimit (p_tssDescriptor,p_limit);
    p_tssDescriptor->type = p_type;
    p_tssDescriptor->present = p_present;
    p_tssDescriptor->dpl = p_dpl;
    p_tssDescriptor->available = 1;
}


