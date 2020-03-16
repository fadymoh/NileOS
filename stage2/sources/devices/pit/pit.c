#include "pit.h"
#include "APICManager.h"
#include "Kernel.h"
extern APIC apic;
extern Kernel kernel;

void setTimer ()
{
    // uint32_t divisor = 1193180 / kernel.pit.frequency;

    // // Send the command byte.
    // outportb(0x43, 0x34);

    // // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    // uint8_t l = (uint8_t)(divisor & 0xFF);
    // uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // // Send the frequency divisor.
    // outportb(0x40, l);
    // outportb(0x40, h);
}


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
void initializePit ()
{
    // kernel.pit.frequency = 50;
    // kernel.pit.ticks = 0;
    // kernel.pit.packets_received = 1;
    // kernel.pit.compare_packets_received = 0;
    // registerInterrupt(IRQ0,pit_fire);
    // setTimer();
}
void setFrequencyTimer (uint32_t p_frequency)
{
//    if (kernel.pit.frequency != p_frequency)
//     {
//         if ( p_frequency > 2000)
//             kernel.pit.frequency = 2000;
//         else kernel.pit.frequency = p_frequency;
//         setTimer();
//     }
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
    sendAPICEOI(&kernel.apicManager.apics[apic_id]); // Important: need to be changed the current apic
}