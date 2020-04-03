#ifndef GLOBALDESCRIPTORTABLEPOINTER_H_
#define GLOBALDESCRIPTORTABLEPOINTER_H_

#include "includes.h"
#include "defines.h"

struct GlobalDescriptorTablePointer_s
{
    uint16_t limit; // GDT Limit
    uint32_t base;  // GDT Base Address
} __attribute__((__packed__));

typedef struct GlobalDescriptorTablePointer_s GlobalDescriptorTablePointer;
#endif /* GLOBALDESCRIPTORTABLEPOINTER_H_ */
