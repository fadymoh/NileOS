#include "ipi.h"
#include "Kernel.h"


extern Kernel kernel;

bool InitializeIPI(Ipi* ipi_ptr, Service* p_service){
    p_service->add_service(p_service, sendIPI, send_ipi);

    return true;
}

void sendIPI(void* p_ipi_ptr){
    Ipi* ipi_obj = (Ipi*) p_ipi_ptr;
    sendFixedIPI(&kernel.apicManager.apics[ipi_obj->params.receiverCore_id], ipi_obj->params.p_irq);
}