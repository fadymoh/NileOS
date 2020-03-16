#include "InterruptManager.h"
#include "Kernel.h"

extern Kernel kernel;
void initializeInterruptManager(InterruptManager *p_console, Service *p_service)
{
    // Initialize the InterruptHandlers Objects List
    setupInterruptDescriptorTable();

    p_service->add_service(p_service, registerInterrupt, register_interrupt);
    p_service->add_service(p_service, serviceInterrupt, service_interrupt);
}
bool registerInterrupt(void *p_interruptManager)
{
    disableInterrupts(); // Disable all interrupts (cli)

    InterruptManager *interruptManager = (InterruptManager *)p_interruptManager;
    uint8_t interruptNumber = (*interruptManager).params.p_interruptNumber;

    InterruptHandler* interruptHandler = (InterruptHandler*)(*(*interruptManager).params.p_interruptHandler);
    // kernel.interruptManager.interruptHandlers[interruptNumber] = interruptHandler; // Assign the handler
   
    kernel.interruptManager.interruptHandlers[interruptNumber] = interruptManager->params.p_interruptHandler; // Assign the handler
    // If the Interrupt is an IRQ one then clear its mask to allow interrupts firing
    printk("Register Interrupt number %d, Handler: %x\n", interruptNumber, interruptManager->params.p_interruptHandler);


    enableInterrupts(); // Enable all interrupts (sti)

    return true;
}
bool serviceInterrupt(void *p_interruptManager)
{

    InterruptManager *interruptManager = (InterruptManager *)p_interruptManager;
    InterruptContext *p_interruptContext = interruptManager->params.p_interruptContext;
    uint8_t int_num = p_interruptContext->interrupt_number;


    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    uint8_t core_id = kernel.apicManager.returns.core_id;
    // printk("Interrupt number %d is in handling AGAIN in core %d\n", int_num, core_id);


    if (kernel.interruptManager.interruptHandlers[int_num] != NULL) // If there is a handler assigned
    {
        
        // printk("Servicing Interrupt number %d, Handler: %x\n", int_num, kernel.interruptManager.interruptHandlers[int_num]);

        // Invoke the handler fire method
        kernel.interruptManager.interruptHandlers[int_num]((*interruptManager).params.p_interruptContext);
        return true; // Return true
    }
    else
        return false; // Else fail and return false
}
