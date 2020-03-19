#include "pit.h"
#include "APICManager.h"
#include "Kernel.h"
extern APIC apic;
extern Kernel kernel;

void pit_sleep(uint16_t p_sleep,uint16_t p_apic_id,uint8_t p_interrupt,bool p_do_sleep)
{

    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    uint16_t current_apic_id = kernel.apicManager.returns.core_id;


    kernel.apicManager.apics[current_apic_id].wakeup_counter = kernel.apicManager.apics[p_apic_id].pit_counter+p_sleep;
    kernel.apicManager.apics[current_apic_id].wakeup_apic_id = p_apic_id;
    kernel.apicManager.apics[current_apic_id].wakeup_interrupt = p_interrupt;    
    if ( !p_do_sleep) return;
    kernel.apicManager.apics[current_apic_id].fired_interrupts[p_interrupt] = 0;
    
    while ( kernel.apicManager.apics[current_apic_id].fired_interrupts[p_interrupt] == 0)
    {
        asm volatile("sti;");
        asm volatile("hlt;");
        asm volatile ("wbinvd;");
        asm volatile("cli;");
        sendAPICEOI(&kernel.apicManager.apics[current_apic_id]);
    
    }
    kernel.apicManager.apics[current_apic_id].fired_interrupts[p_interrupt] = 0 ;
    kernel.apicManager.apics[current_apic_id].wakeup_counter = 0;
    kernel.apicManager.apics[current_apic_id].wakeup_apic_id = 0;
}

void pit_fire(InterruptContext *p_interruptContext)
{
    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    uint8_t apic_id = kernel.apicManager.returns.core_id;
    if (p_interruptContext->interrupt_number == IRQ0)
    {
        kernel.apicManager.apics[apic_id].pit_counter++;
        kernel.pit.ticks++;
        if (kernel.apicManager.apics[apic_id].pit_counter % 100 == 0)
        {
            printk("Core: %d         Timer Tick: %d\n", apic_id, kernel.apicManager.apics[apic_id].pit_counter/100);
        }
        for (uint16_t i = 0; i < kernel.apicManager.apics_count; i++)
        {
            if (i != apic_id)
            {
                if (kernel.apicManager.apics[i].wakeup_apic_id == apic_id &&
                    kernel.apicManager.apics[i].wakeup_counter < kernel.apicManager.apics[apic_id].pit_counter)
                {
                    sendFixedIPI(&kernel.apicManager.apics[i],
                                 kernel.apicManager.apics[i].wakeup_interrupt);
                }
            }
        }
    }
    sendAPICEOI(&kernel.apicManager.apics[apic_id]);
}