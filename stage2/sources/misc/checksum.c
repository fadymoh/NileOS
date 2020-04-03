#include "checksum.h"

uint16_t calcsum(uint16_t *buffer, uint64_t length)
{
	uint32_t sum;

	// initialize sum to zero and loop until length (in words) is 0
	for (sum = 0; length > 1; length -= 2) // sizeof() returns number of bytes, we're interested in number of words
		sum += *buffer++;				   // add 1 word of buffer to sum and proceed to the next

	// we may have an extra byte
	if (length == 1)
		sum += (uint8_t)*buffer;

	sum = (sum >> 16) + (sum & 0xFFFF); // add high 16 to low 16
	sum += (sum >> 16);					// add carry
	return ~sum;
}
