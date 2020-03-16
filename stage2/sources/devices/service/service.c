#include "service.h"

int service_init(Service* service, void* service_to_point_at, int service_name)
{
    service->service_name = service_name;
    service->service_data = service_to_point_at;
    service->add_service = add_service;
    return 1;
}

int add_service(Service* service, functionPtr fnptr, int location)
{
    if (location >= MAX_FUNCTIONS || fnptr == NULL)
        return 0;
    service->service_method[location] = fnptr;
    return 1;
}
