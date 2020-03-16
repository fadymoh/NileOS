#ifndef INTERRUPTMANAGER_H_
#define INTERRUPTMANAGER_H_

#include "InterruptDescriptorTable.h"
#include "service.h"

enum interruptManager_exposed_functions {register_interrupt, service_interrupt} ; 

typedef void InterruptHandler (InterruptContext * p_interruptContext);

typedef struct InterruptManager_s
{
    InterruptHandler * interruptHandlers[IDT_SIZE];

    struct 
    {
        uint8_t p_interruptNumber;
        InterruptHandler* p_interruptHandler;
        InterruptContext* p_interruptContext;
    } params;

}InterruptManager;


void initializeInterruptManager (InterruptManager * p_interruptManager, Service* p_service);
bool registerInterrupt(void* p_interruptManager);
bool serviceInterrupt (void* p_interruptManager);

#endif