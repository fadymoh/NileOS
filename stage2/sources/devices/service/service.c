#include "service.h"
#include "stdio.h"
#include "Kernel.h"

extern Kernel kernel;

int service_init(Service *service, void *service_to_point_at, int service_name)
{
    service->service_name = service_name;
    service->service_data = service_to_point_at;
    if (service_name != sharedMemory_t && service_name != multiplyService_t)
        service->add_service = add_service;
    else
    {
        service->add_serviceTest = add_service_test;
        service->serviceTestParameterDescription = add_serviceTestParameterDescription;
    }

    for (int i = 0; i < service->service_method_count; ++i)
    {
        service->testMethod[i].parametersCount = 0;
        service->testMethod[i].returnsCount = 0;
    }

    return 1;
}

int add_service(Service *service, functionPtr fnptr, int location)
{
    if (location >= MAX_FUNCTIONS || fnptr == NULL)
        return 0;
    service->service_method[location] = fnptr;
    return 1;
}

int add_service_test(Service *service, testingPtr fnptr, int location, char *name)
{
    strcpy(service->serviceString, name);
    if (location >= MAX_FUNCTIONS || fnptr == NULL)
        return 0;

    service->testMethod[location].methodPtr = fnptr;

    return 1;
}

void PrintParameter(Parameter *parameter)
{
    char *encryptionStatus = ((parameter->encrypted) ? "True" : "False");
    char *encodedStatus = ((parameter->base64Encoded) ? "True" : "False");
    char *variableSizeStatus = ((parameter->variableSize) ? "True" : "False");
    printk("\t\t\t<%s>\n \
    \t\t\t\t<type> %s </type>\n \
    \t\t\t\t<encrypted> %s </encrypted>\n \
    \t\t\t\t<encoded> %s </encoded>\n \
    \t\t\t\t<variable size> %s </variable size>\n \
    \t\t\t\t<size of parameter> %d </size of parameter>\n \
    \t\t\t</%s>\n",
           parameter->tag, parameter->type, encryptionStatus, encodedStatus, variableSizeStatus, parameter->sizeOfParameter, parameter->tag);
}

void add_serviceTestParameterDescription(Service *service, int location, char *tag, char *type, uint64_t size, bool variableSize, bool encrypted, bool base64Encoded, bool isReturn)
{
    if (!isReturn)
    {
        int parameterLocation = service->testMethod[location].parametersCount++;
        memcpy(service->testMethod[location].parameters[parameterLocation].tag, tag, strlen(tag));
        memcpy(service->testMethod[location].parameters[parameterLocation].type, type, strlen(type));
        service->testMethod[location].parameters[parameterLocation].sizeOfParameter = size;
        service->testMethod[location].parameters[parameterLocation].variableSize = variableSize;
        service->testMethod[location].parameters[parameterLocation].encrypted = encrypted;
        service->testMethod[location].parameters[parameterLocation].base64Encoded = base64Encoded;
    }
    else
    {
        int returnLocation = service->testMethod[location].returnsCount++;
        memcpy(service->testMethod[location].returns[returnLocation].tag, tag, strlen(tag));
        memcpy(service->testMethod[location].returns[returnLocation].type, type, strlen(type));
        service->testMethod[location].returns[returnLocation].sizeOfParameter = size;
        service->testMethod[location].returns[returnLocation].variableSize = variableSize;
        service->testMethod[location].returns[returnLocation].encrypted = encrypted;
        service->testMethod[location].returns[returnLocation].base64Encoded = base64Encoded;
    }
}

char *ConvertTagToXML(int argc, ...)
{
    //need to work on a shared memory not kmalloc

    char str[2048];
    memset(str, 0, 2048);
    va_list args;
    va_start(args, argc);
    uint64_t maximumTotalSize = 0;

    for (int i = 0; i < argc; ++i)
    {
        Parameter *param = va_arg(args, Parameter *);
        void *value = va_arg(args, void *);
        uint64_t sizeOfData = param->sizeOfParameter;

        if (param->variableSize)
            sizeOfData = va_arg(args, uint64_t);
        maximumTotalSize += strlen(param->tag) * 2 + 9;
        maximumTotalSize += (sizeOfData * 4 / 3);
    }
    va_start(args, argc);
    //printk("total size allocated before 4/3: %d\n", maximumTotalSize);
    maximumTotalSize *= 4 / 3;

    char *buffer = kmalloc(&kernel.memoryAllocator, maximumTotalSize); //needs to be allocated before the  loop
    char *end;
    end = buffer;
    memset(buffer, 0x0, maximumTotalSize);
    for (int i = 0; i < argc; ++i)
    {
        Parameter *param = va_arg(args, Parameter *);
        void *value = va_arg(args, void *);
        uint64_t sizeOfData = param->sizeOfParameter;

        if (param->variableSize)
            sizeOfData = va_arg(args, uint64_t);

        char *tag = param->tag;

        uint64_t tagLength = strlen(tag);
        uint64_t encodedDataLength = 0;
        uint64_t decodedDataLength = 0;
        char *encodedData;
        //printk_network("Data: %d\n", (uint64_t)value);
        printk_network("Before: \n");
        for (int i = 0; i < 8; i++)
            printk_network("%d | ", ((char *)value)[i]);
        printk_network("\n");
        encodedData = base64_encode(value, sizeOfData, &encodedDataLength);

        char *decodedData = base64_decode(encodedData, encodedDataLength, &decodedDataLength);
        printk_network("After: \n");

        for (int i = 0; i < 8; i++)
            printk_network("%d | ", (decodedData)[i]);
        printk_network("\n");
        sprintf(end, "<%s> ", tag);
        end += (tagLength + 3);

        memcpy(end, encodedData, encodedDataLength);
        end += encodedDataLength;
        sprintf(end, " </%s>\n", tag);
        end += (tagLength + 5);
    }
    *end = 0;
    //sprintf(buffer + strlen(buffer), "\0");
    //printk("Buffer: %s\n", buffer);

    return buffer;
}