#include "service_transport_layer.h"
#include "printk.h"

bool InitServiceTransportLayer(service_transport_kernel *services_of_kernel)
{
    services_of_kernel->max_services = 256;
    services_of_kernel->curr_count = 0;
    return true;
}

bool RegisterServiceToKernel(service_transport_kernel *services_of_kernel, Service *service_to_register, int location)
{

    // printk(" in registring\n");
    if (services_of_kernel->curr_count == services_of_kernel->max_services)
        return false;

    services_of_kernel->curr_count++;
    if (location == multiplyService_t)
    {
        // printk("Trying to dispatch:\n");
        // printk("registering: %x\n", service_to_register);
    }
    // printk("tab ehhhh\n");
    services_of_kernel->kernel_services[location] = service_to_register;
    return true;
}

bool DispatchKernel(service_transport_kernel *services_of_kernel, int service, int method_num)
{
    (*((services_of_kernel->kernel_services[service])->service_method[method_num]))(services_of_kernel->kernel_services[service]->service_data);
    return true;
}

char* DispatchKernelTest(service_transport_kernel *services_of_kernel, int service, int method_num, void *location_of_message)
{
    void *params =
        (services_of_kernel->kernel_services[service])->parserMethod(services_of_kernel->kernel_services[service], location_of_message, method_num);
    return (*((services_of_kernel->kernel_services[service])->testMethod[method_num].methodPtr))(services_of_kernel->kernel_services[service]->service_data, params);

    // char * xml_to_return = ConvertTagToXML(
    //     (services_of_kernel->kernel_services[service])->testMethod[method_num].returnTags[0], returned, sizeof(returned)

    // return true;
}