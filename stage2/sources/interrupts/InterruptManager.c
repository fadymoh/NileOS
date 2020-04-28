#include "InterruptManager.h"
#include "Kernel.h"

extern Kernel kernel;

void InitializeInterruptManager(InterruptManager *p_InterruptManager, Service *p_service)
{
    // Initialize the InterruptHandlers Objects List
    SetupInterruptDescriptorTable();

    p_service->add_service(p_service, RegisterInterrupt, register_interrupt);
    p_service->add_service(p_service, ServiceInterrupt, service_interrupt);
}

bool RegisterInterrupt(void *p_interruptManager)
{
    disableInterrupts();

    InterruptManager *interruptManager = (InterruptManager *)p_interruptManager;
    uint8_t interruptNumber = (*interruptManager).params.p_interruptNumber;

    InterruptHandler *interruptHandler = (InterruptHandler *)(*(*interruptManager).params.p_interruptHandler);

    kernel.interruptManager.interruptHandlers[interruptNumber] = interruptManager->params.p_interruptHandler; // Assign the handler

    printk_debug("Register Interrupt number %d, Handler: %x\n", interruptNumber, interruptManager->params.p_interruptHandler);

    enableInterrupts();

    return true;
}

bool ServiceInterrupt(void *p_interruptManager)
{
    InterruptManager *interruptManager = (InterruptManager *)p_interruptManager;
    InterruptContext *p_interruptContext = interruptManager->params.p_interruptContext;
    uint8_t int_num = p_interruptContext->interrupt_number;

    if (kernel.interruptManager.interruptHandlers[int_num] != NULL) // If there is a handler assigned
    {
        // Invoke the handler fire method
        kernel.interruptManager.interruptHandlers[int_num]((*interruptManager).params.p_interruptContext);

        return true;
    }
    else
        return false;
}
