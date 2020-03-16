#ifndef TSSDESCRIPTOR_H_
#define TSSDESCRIPTOR_H_

#include "includes.h"
#include "defines.h"


struct TSSDescriptor_s
{        // TSS Descriptor Attributes
        uint32_t limit_0_15 : 16;
        uint32_t base_0_15  : 16;
        uint32_t base_16_23 : 8;
        uint32_t type       : 4;
        uint32_t            : 1;
        uint32_t dpl        : 2;
        uint32_t present    : 1;
        uint32_t limit_16_19: 4;
        uint32_t available  : 1;
        uint32_t            : 2;
        uint32_t granularity: 1;
        uint32_t base_24_31 : 8;
        uint32_t base_32_63 : 32;
        uint32_t            : 32;
} __attribute__ ((__packed__))  ;

typedef struct TSSDescriptor_s TSSDescriptor;
/*
 * This is a setter method which sets the limit data members
 * 
 * 
 */


void setTSSDescriptorLimit (TSSDescriptor * p_tssDescriptor, uint32_t p_limit);


/*
 * This is a setter method which sets the base data members
 * 
 * 
 */


void setTSSDescriptorBase (TSSDescriptor * p_tssDescriptor,uint64_t p_base);

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
                                uint8_t p_dpl);

#endif