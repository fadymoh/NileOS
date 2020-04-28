#include "pit.h"
#include "APICManager.h"
#include "Kernel.h"
extern APIC apic;
extern Kernel kernel;

void pit_sleep(uint16_t p_sleep, uint16_t p_apic_id, uint8_t p_interrupt, bool p_do_sleep)
{

    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
    uint16_t current_apic_id = kernel.apicManager.returns.core_id;

    kernel.apicManager.apics[current_apic_id].wakeup_counter = kernel.apicManager.apics[p_apic_id].pit_counter + p_sleep;
    kernel.apicManager.apics[current_apic_id].wakeup_apic_id = p_apic_id;
    kernel.apicManager.apics[current_apic_id].wakeup_interrupt = p_interrupt;
    if (!p_do_sleep)
        return;
    kernel.apicManager.apics[current_apic_id].fired_interrupts[p_interrupt] = 0;

    while (kernel.apicManager.apics[current_apic_id].fired_interrupts[p_interrupt] == 0)
    {
        asm volatile("sti;");
        asm volatile("hlt;");
        asm volatile("wbinvd;");
        asm volatile("cli;");
        sendAPICEOI(&kernel.apicManager.apics[current_apic_id]);
    }
    kernel.apicManager.apics[current_apic_id].fired_interrupts[p_interrupt] = 0;
    kernel.apicManager.apics[current_apic_id].wakeup_counter = 0;
    kernel.apicManager.apics[current_apic_id].wakeup_apic_id = 0;
}

void pit_fire(InterruptContext *p_interruptContext)
{
    // this function gets called on every pit_fire interrupt which is quite alot??
    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
    uint8_t apic_id = kernel.apicManager.returns.core_id;
    if (p_interruptContext->interrupt_number == IRQ0)
    {
        kernel.apicManager.apics[apic_id].pit_counter++;
        if (kernel.apicManager.apics[apic_id].pit_counter % 100 == 0)
        {
           // printk("Core: %d         Timer Tick: %d\n", apic_id, kernel.apicManager.apics[apic_id].pit_counter);
            kernel.pit.ticks++;
        }
        // if (kernel.apicManager.apics[apic_id].pit_counter % 500 == 0)
        // {
        //     printk("dispatching IPI to network!\n");
        //     kernel.ipiManager.params.receiverCore_id = 0;
        //     kernel.ipiManager.params.p_irq = 11 + 32;
        //     DispatchKernel(&kernel.service_transporter, ipi_t, send_ipi);
        //     //e1000Scan();
        // }
        for (uint16_t i = 0; i < kernel.apicManager.apics_count; i++)
        {
            if (i != apic_id)
            {
                if (kernel.apicManager.apics[i].wakeup_apic_id == apic_id &&
                    kernel.apicManager.apics[i].wakeup_counter < kernel.apicManager.apics[apic_id].pit_counter)
                {
                    kernel.ipiManager.params.receiverCore_id = i;
                    kernel.ipiManager.params.p_irq = kernel.apicManager.apics[i].wakeup_interrupt;
                    DispatchKernel(&kernel.service_transporter, ipi_t, send_ipi);
                }
            }
        }
    }
    sendAPICEOI(&kernel.apicManager.apics[apic_id]);
}