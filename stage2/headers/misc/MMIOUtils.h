/* 
 * File:        MMIOUtils.h
 * Class:       MMIOUtils
 * Type:        Interfaces
 * Author:      Karim Sobh
 * Project:     BareMetal
 * Version:     1.0
 * Copyright:   2015
 *
 * Description:
 * This is the header file for the MMIOUtils class defining its interfaces.
 * All the interfaces of this class are defined as static methods.
 * This is a miscellaneous class that groups all standalone miscellaneous
 * functions that does not need any state, which basically performs basic
 * MMIO operations.
 *
 *
 * Created on January 19, 2015
 */

#ifndef MMIOUTILS_H_
#define MMIOUTILS_H_

#include "defines.h"
#include "includes.h"

uint8_t mmio_read8 (uint64_t p_address);
uint16_t mmio_read16 (uint64_t p_address);
uint32_t mmio_read32 (uint64_t p_address);
uint64_t mmio_read64 (uint64_t p_address);
void mmio_write8 (uint64_t p_address,uint8_t p_value);
void mmio_write16 (uint64_t p_address,uint16_t p_value);
void mmio_write32 (uint64_t p_address,uint32_t p_value);
void mmio_write64 (uint64_t p_address,uint64_t p_value);

#endif /* MMIOUTILS_H_ */
