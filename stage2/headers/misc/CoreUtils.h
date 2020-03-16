/* 
 * File:        CoreUtils.h
 * Class:       CoreUtils
 * Type:        Interfaces
 * Author:      Karim Sobh
 * Project:     BareMetal
 * Version:     1.0
 * Copyright:   2015
 *
 * Description:
 * This is the header file for the CoreUtils class defining its interfaces.
 * All the interfaces of this class are defined as static methods.
 * This is a miscellaneous class that groups all standalone miscellaneous
 * functions that does not need any state, which basically performs basic
 * operations related to CPU core management.
 *
 *
 * Created on January 19, 2015
 */

#ifndef COREUTILS_H_
#define COREUTILS_H_

#include "defines.h"
#include "includes.h"
//#include "Disk.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))


uint32_t createMask(uint32_t p_value,uint8_t p_start, uint8_t p_end);
uint32_t getAPICId(uint64_t p_reg);        
void writeLocalAPIC(uint64_t p_reg, uint32_t p_val);
uint32_t readLocalAPIC(uint64_t p_reg);

#endif /* COREUTILS_H_ */
