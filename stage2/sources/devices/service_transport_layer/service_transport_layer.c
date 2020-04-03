#include "service_transport_layer.h"
#include "printk.h"


bool InitServiceTransportLayer(service_transport_kernel* services_of_kernel)
{
    services_of_kernel->max_services = 256;
    services_of_kernel->curr_count = 0;
    return true;
}
bool RegisterServiceToKernel(service_transport_kernel* services_of_kernel,Service* service_to_register, int location)
{

    if(services_of_kernel->curr_count == services_of_kernel->max_services)
        return false;
    
    services_of_kernel->curr_count++;
    services_of_kernel->kernel_services[location] = service_to_register;
    return true;
}
bool DispatchKernel(service_transport_kernel* services_of_kernel, int service, int method_num)
{
    (*((services_of_kernel->kernel_services[service])->service_method[method_num]))(services_of_kernel->kernel_services[service]->service_data);  
    return true;
}