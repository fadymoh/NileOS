#include "isr_stub.h"
#include "Kernel.h"
extern Kernel kernel;

extern void idt_handler(InterruptContext *p_interruptContext)
{
    disableInterrupts();
    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    uint8_t core_id = kernel.apicManager.returns.core_id;

    if (p_interruptContext->interrupt_number == 14) //if the interrupt is page fault, we stall forever
    {
        printk("Page Fault: %x\n", p_interruptContext->return_address);
        for (;;);
    }
    
    kernel.apicManager.apics[core_id].fired_interrupts[p_interruptContext->interrupt_number] = 1;


    kernel.interruptManager.params.p_interruptContext = p_interruptContext;
    dispatch_kernel(&kernel.service_transporter, interruptManager_t, service_interrupt);

    sendAPICEOI(&kernel.apicManager.apics[core_id]); //do something we don't know yet.
    enableInterrupts();
}