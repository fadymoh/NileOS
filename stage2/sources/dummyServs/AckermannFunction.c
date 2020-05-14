#include "AckermannFunction.h"
#include "Kernel.h"
extern Kernel kernel;

int initAckermannFunctionService(AckermannFunction *AckermannFunction, Service *p_service)
{
    AckermannFunction->numberOfTimesToCallFunction = 5;
    p_service->service_method_count = 1;

    p_service->add_serviceTest(p_service, CallAckermannFunction, AckFunction_t, "Call Ackermann's Function");

    p_service->serviceTestParameterDescription(p_service, AckFunction_t, "FirstValue", "uint64_t", sizeof(uint64_t), false, false, true, false);
    p_service->serviceTestParameterDescription(p_service, AckFunction_t, "SecondValue", "uint64_t", sizeof(uint64_t), false, false, true, false);
    p_service->serviceTestParameterDescription(p_service, AckFunction_t, "Result", "uint64_t", sizeof(uint64_t), false, false, true, true);

    p_service->parserMethod = ParseAckermannFunctionXML;
}

void AckermannFunctionDiscovery(void *p_ackermannFunctionService)
{
    Service *ackermannFunction = (Service *)p_ackermannFunctionService;
    printk("<Ackermann Function Service>\n");
    for (int i = 0; i < ackermannFunction->service_method_count; ++i)
    {
        printk("\t<%s>\n", ackermannFunction->serviceString);
        printk("\t\t<parameters>\n");

        for (int j = 0; j < ackermannFunction->testMethod[i].parametersCount; ++j)
        {
            PrintParameter(&(ackermannFunction)->testMethod[i].parameters[j]);
        }
        printk("\t\t</parameters>\n");
        printk("\t\t<returns>\n");
        for (int j = 0; j < ackermannFunction->testMethod[i].returnsCount; ++j)
        {
            PrintParameter(&(ackermannFunction)->testMethod[i].returns[j]);
        }
        printk("\t\t</returns>\n");
        printk("\t</%s>\n", ackermannFunction->serviceString);
    }
    printk("</Ackermann Function Service>\n");
}

void *ParseAckermannFunctionXML(void *p_AckermannFunction, char *xmlString, int location)
{
    AckermannFunctionParams *temp = kmalloc(&kernel.memoryAllocator, sizeof(AckermannFunctionParams));
    printk("XML to be Parsed %s\n", xmlString);
    ParseXMLString(xmlString);

    Service *ackermannFunctionService = (Service *)p_AckermannFunction;
    for (int i = 0; i < ackermannFunctionService->testMethod[location].parametersCount; ++i)
    {
        kernel.xmlService.params.tagDirectory = ackermannFunctionService->testMethod[location].parameters[i].tag;
        getValueFromTagMessage((void *)&kernel.xmlService);
        if (strcmp(ackermannFunctionService->testMethod[location].parameters[i].tag, "FirstValue") == NULL)
        {
            temp->m = atoi(kernel.xmlService.returns.value);
            printk("from: %s  |  temp->m: %d\n", kernel.xmlService.returns.value, temp->m);
        }
        else if (strcmp(ackermannFunctionService->testMethod[location].parameters[i].tag, "SecondValue") == NULL)
        {
            temp->n = atoi(kernel.xmlService.returns.value);
            printk("from: %s  |  temp->n: %d\n", kernel.xmlService.returns.value, temp->n);
        }
    }

    return temp;
}

char *CallAckermannFunction(void *p_AckermannFunctionService, void *p_params)
{
    AckermannFunctionParams *temp = p_params;
    AckermannFunction *ackermannFunctionService = p_AckermannFunctionService;
    uint64_t res = 0;

    for (int i = 0; i < ackermannFunctionService->numberOfTimesToCallFunction; ++i)
    {
        res = ack(temp->m, temp->n);
        printk("Calling ACK of %d, %d which has a value of: %d\n", temp->m, temp->n, res);
    }
    Parameter *returnTag = &kernel.service_transporter.kernel_services[ackermannFunctionService_t]->testMethod[AckFunction_t].returns[0];

    return ConvertTagToXML(1, returnTag, &res);
}

int ack(int m, int n)
{
    if (m == 0)
    {
        return n + 1;
    }
    else if ((m > 0) && (n == 0))
    {
        return ack(m - 1, 1);
    }
    else if ((m > 0) && (n > 0))
    {
        return ack(m - 1, ack(m, n - 1));
    }
}