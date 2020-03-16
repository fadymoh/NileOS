#ifndef SERVICETRANSPORTLAYER_H
#define SERVICETRANSPORTLAYER_H

#include "service.h"
#include "includes.h"

typedef struct 
{
    int max_services;
    int curr_count;
    Service* kernel_services[256];
}service_transport_kernel;

typedef struct 
{
    int max_services;
    int curr_count;
    Service* user_services[256];
}service_transport_user;

bool init_service_transport(service_transport_kernel*);
bool register_service_to_kernel(service_transport_kernel*, Service*, int);
bool dispatch_kernel(service_transport_kernel*, int service, int method_num);


#endif