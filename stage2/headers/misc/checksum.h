#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include "defines.h"
#include "includes.h"

uint16_t calcsum( uint16_t * buffer, uint64_t length);
extern uint32_t ip_hdr_checksum(uint16_t * buffer);

#endif