#include "isr_stub.h"
#include "Kernel.h"
extern Kernel kernel;

extern void IdtHandler(InterruptContext *p_interruptContext)
{
    // if(p_interruptContext->interrupt_number != 32)
    //     printk("Inside IDT HANDLER %d \n", p_interruptContext->interrupt_number);
    disableInterrupts();
    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
    uint8_t core_id = kernel.apicManager.returns.core_id;

    if (p_interruptContext->interrupt_number == 14) //if the interrupt is page fault, we stall forever
    {
        printk("Page Fault: %x\n", p_interruptContext->return_address);
        printk_network("Page Fault: %x\n", p_interruptContext->return_address);
        printk_fs("Page Fault: %x\n", p_interruptContext->return_address);
        printk_debug("Page Fault: %x\n", p_interruptContext->return_address);
        for (;;);
    }
    
    kernel.apicManager.apics[core_id].fired_interrupts[p_interruptContext->interrupt_number] = 1;


    kernel.interruptManager.params.p_interruptContext = p_interruptContext;
    DispatchKernel(&kernel.service_transporter, interruptManager_t, service_interrupt);

    sendAPICEOI(&kernel.apicManager.apics[core_id]);
    enableInterrupts();

    // if(p_interruptContext->interrupt_number != 32)
    //     printk("Inside IDT HANDLER %d\n", p_interruptContext->interrupt_number);
}