/* 
 * File:        ClockUtils.h
 * Class:       ClockUtils
 * Type:        Interfaces
 * Author:      Karim Sobh
 * Project:     BareMetal
 * Version:     1.0
 * Copyright:   2015
 *
 * Description:
 * This is the header file for the ClockUtils class defining its interfaces.
 * All the interfaces of this class are defined as methods.
 * This is a miscellaneous class that groups all standalone miscellaneous
 * functions that does not need any state, which basically performs basic
 * operations related to time and CMOS RTC management
 *
 *
 * Created on January 19, 2015
 */

#ifndef CLOCKUTILS_H_
#define CLOCKUTILS_H_

#include "defines.h"
#include "includes.h"
#include "Ports.h"

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};


uint8_t getUpdateInProgressFlag();
uint8_t getRTCRegister(uint8_t p_reg);
uint32_t timeStamp32 (uint8_t p_seconds, uint8_t p_minutes,uint8_t p_hours ,uint8_t p_day , uint8_t p_month , uint16_t p_year);
uint64_t timeStamp64 (uint8_t p_seconds, uint8_t p_minutes,uint8_t p_hours ,uint8_t p_day , uint8_t p_month , uint16_t p_year);
uint32_t getRTCTimeStamp32 ();
uint64_t getRTCTimeStamp64 ();
void RTCSleep (uint16_t p_seconds);
uint64_t random (uint64_t p_max);
#endif /* CLOCKUTILS_H_ */
