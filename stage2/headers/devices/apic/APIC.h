#ifndef APIC_H_
#define APIC_H_
#include "MemoryUtilities.h"
#include "PhysicalMemory.h"
#include "CoreUtils.h"
#include "ClockUtils.h"
#include "Spinlock.h"
#include "GlobalDescriptorTablePointer.h"

#define LAPIC_SVR_APIC_ENABLE 0x100 | 0xff

#define sfence() __asm__ __volatile__("sfence" :: \
                                          : "memory")

#define IO_APIC_MMREG_IOREGSEL 0x00
#define IO_APIC_MMREG_IOWIN 0x10

typedef struct
{
        uint16_t apic_id;
        uint64_t apicio_base;
        uint64_t lapic_base;
        uint64_t lapic_reg;
        uint64_t lapic_apicver;
        uint64_t lapic_taskprior;
        uint64_t lapic_eoi;
        uint64_t lapic_ldr;
        uint64_t lapic_dfr;
        uint64_t lapic_svr;
        uint64_t lapic_esr;
        uint64_t lapic_icr0;
        uint64_t lapic_icr1;
        uint64_t lapic_lvt_tmr;
        uint64_t lapic_lvt_perf;
        uint64_t lapic_lvt_lint0;
        uint64_t lapic_lvt_lint1;
        uint64_t lapic_lvt_err;
        uint64_t lapic_tmr_init_cnt;
        uint64_t lapic_tmr_curr_cnt;
        uint64_t lapic_tmr_div;
        uint64_t lapic_last;
        uint64_t lapic_disable;
        uint64_t lapic_sw_enable;
        uint64_t lapic_cpufocus;
        uint64_t lapic_nmi;
        uint64_t tmr_periodic;
        uint64_t tmr_basediv;
        bool bsp;
        bool timer_enabled;
        bool touch_flag;
        uint64_t stack_ptr;
        uint64_t kstack_ptr;
        volatile uint64_t pit_counter;
        uint64_t wakeup_counter;
        uint16_t wakeup_apic_id;
        uint16_t wakeup_interrupt;
        spinlock_t ipioe_spinlock;
        bool ipioe_auto_replies;
        bool ipioe_raised;
        uint8_t fired_interrupts[256];
        uint8_t role[5];

} APIC;

uint32_t ioapicRead(APIC *apic, uint8_t p_index);
void ioapicWrite(APIC *apic, uint8_t p_index, uint32_t p_value);
void initialize_apic(APIC *apic, uint16_t p_apic_id, uint64_t p_local_apic, bool p_bsp);
void initAPICIO(APIC *apic);
void disableAPICTimer(APIC *apic);
void enableAPICTimer(APIC *apic);
void sendAPICEOI(APIC *apic);
void mapAPICIRQ(APIC *apic, uint8_t p_irq, uint8_t p_vector);
bool startup(APIC *apic);

#endif