#ifndef SERVICETRANSPORTLAYER_H
#define SERVICETRANSPORTLAYER_H

#include "service.h"
#include "includes.h"

typedef struct
{
    int max_services;
    int curr_count;
    Service *kernel_services[256];
} service_transport_kernel;

typedef struct
{
    int max_services;
    int curr_count;
    Service *user_services[256];
} service_transport_user;

bool InitServiceTransportLayer(service_transport_kernel *);
bool RegisterServiceToKernel(service_transport_kernel *, Service *, int);
bool DispatchKernel(service_transport_kernel *, int service, int method_num);
char* DispatchKernelTest(service_transport_kernel *services_of_kernel, int service, int method_num, void* location_of_message);

#endif