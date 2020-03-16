#ifndef IPI_H
#define IPI_H

#include "service.h"
#include "defines.h"
#include "includes.h"


enum ipi_exposed_functions {send_ipi} ; 


typedef struct
{
        // args
        struct{
                uint16_t receiverCore_id;
                uint8_t p_irq;
        } params;
}Ipi;


bool InitializeIPI(Ipi* ipi_ptr, Service* p_service);
void sendIPI(void* p_ipi_ptr);


#endif