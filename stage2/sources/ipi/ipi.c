#include "ipi.h"
#include "Kernel.h"

extern Kernel kernel;

bool InitializeIPI(Ipi *ipi_ptr, Service *p_service)
{
    p_service->add_service(p_service, sendIPI, send_ipi);

    return true;
}

void sendIPI(void *p_ipi_ptr)
{
    Ipi *ipi_obj = (Ipi *)p_ipi_ptr;
    SendFixedIPI(&kernel.apicManager.apics[ipi_obj->params.receiverCore_id], ipi_obj->params.p_irq);
}

void SendFixedIPI(APIC *apic, uint8_t p_irq)
{
    uint32_t _core_id = apic->apic_id << 24;
    writeLocalAPIC(apic->lapic_icr1, _core_id);
    writeLocalAPIC(apic->lapic_icr0, 0b00000000000000000100000000000000 | p_irq);
    sfence();
    for (; readLocalAPIC(apic->lapic_icr0) & 0x1000;);
}