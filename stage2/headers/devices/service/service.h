#ifndef SERVICE_H
#define SERVICE_H
#include "defines.h"

typedef void (*functionPtr)(void*);

typedef enum service_enum_t {console_t, physical_memory, pci_t, virtual_memory, apic_t, interruptManager_t, ipi_t, sharedMemory_t} service_enum;


typedef struct Service_t
{
    void* service_data;
    functionPtr service_method[MAX_FUNCTIONS];
    service_enum service_name;
    int service_method_count;
    int(*add_service)(struct Service_t*, functionPtr, int);
} Service;

int service_init(Service* service, void* service_to_point_at, int service_name);
int add_service(Service*, functionPtr, int);
#endif