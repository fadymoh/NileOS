#ifndef PIT_H_
#define PIT_H_
#include "defines.h"
#include "includes.h"
#include "Ports.h"
#include "InterruptContext.h"

typedef struct PIT_s
{
    uint32_t frequency;
    uint64_t ticks;
    uint64_t packets_received;
    uint64_t compare_packets_received;
} PIT;

void pit_fire(InterruptContext *p_interruptContext);
void setTimer ();
void pit_sleep(uint16_t p_sleep,uint16_t p_apic_id,uint8_t p_interrupt,bool p_do_sleep);
void initializePit ();
void setFrequencyTimer ();
// void fault_handler(InterruptContext *p_interruptContext);

#endif