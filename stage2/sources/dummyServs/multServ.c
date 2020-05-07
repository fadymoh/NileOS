#include "multServ.h"
#include "Kernel.h"
extern Kernel kernel;

int initMultiplyService(MultService *multserv, Service *p_service)
{
    multserv->multiplier_factor = 2;
    multserv->numberOfTimesToMultiply = 2;
    p_service->service_method_count = 1;

    p_service->add_serviceTest(p_service, Multiply, GETMult, "Multiply");
    p_service->serviceTestParameterDescription(p_service, GETMult, "FirstValue", "uint64_t", sizeof(uint64_t), false, false, true, false);

    p_service->serviceTestParameterDescription(p_service, GETMult, "SecondValue", "uint64_t", sizeof(uint64_t), false, false, true, false);

    p_service->serviceTestParameterDescription(p_service, GETMult, "Result", "uint64_t", sizeof(uint64_t), false, false, true, true);

    p_service->parserMethod = ParseMultServXML;
}

void *ParseMultServXML(void *p_MultiplyService, char *xmlString, int location)
{
    MultServParams *temp = kmalloc(&kernel.memoryAllocator, sizeof(MultServParams));
    ParseXMLString(xmlString);
    Service *multService = (Service *)p_MultiplyService;
    for (int i = 0; i < multService->testMethod[location].parametersCount; ++i)
    {
        kernel.xmlService.params.tagDirectory = multService->testMethod[location].parameters[i].tag;
        getValueFromTagMessage((void *)&kernel.xmlService);
        if (strcmp(multService->testMethod[location].parameters[i].tag, "FirstValue") == NULL)
        {
            temp->x = atoi(kernel.xmlService.returns.value);
        }
        else if (strcmp(multService->testMethod[location].parameters[i].tag, "SecondValue") == NULL)
        {
            temp->y = atoi(kernel.xmlService.returns.value);
        }
    }

    return temp;
}

char *Multiply(void *p_MultiplyService, void *p_params)
{
    MultServParams *temp = p_params;
    MultService *multService = p_MultiplyService;
    uint64_t res = temp->x * temp->y;

    // for (int i = 0; i < multService->numberOfTimesToMultiply; ++i)
    // {
    //     res *= multService->multiplier_factor;
    // }
    res = ack(4, 4);
    printk("ACK of 3, 5: %d\n", res);
    Parameter *returnTag = &kernel.service_transporter.kernel_services[multiplyService_t]->testMethod[GETMult].returns[0];

    return ConvertTagToXML(1, returnTag, &res);
}

int ack(int m, int n) 
{ 
    if (m == 0){ 
        return n + 1; 
    } 
    else if((m > 0) && (n == 0)){ 
        return ack(m - 1, 1); 
    } 
    else if((m > 0) && (n > 0)){ 
        return ack(m - 1, ack(m, n - 1)); 
    }
} 