#ifndef SERVICE_H
#define SERVICE_H
#include "defines.h"
#include "includes.h"

typedef void (*functionPtr)(void *);
typedef void *(*ParserPtr)(void *, void *, int);
typedef void *(*testingPtr)(void *, void *, ...);

typedef enum service_enum_t
{
    console_t,
    physical_memory_t,
    pci_t,
    virtual_memory_t,
    apic_t,
    interruptManager_t,
    ipi_t,
    sharedMemory_t,
    // ata manager
    // network driver
    // block service
    xmlService_t,
    ackermannFunctionService_t

} service_enum;

typedef struct Parameter_t
{
    char tag[50];
    char type[20];
    bool encrypted;
    bool base64Encoded;
    bool variableSize;
    uint64_t sizeOfParameter;
} Parameter;

typedef struct MethodStructure_t
{
    testingPtr methodPtr;
    Parameter parameters[15];
    int parametersCount;
    Parameter returns[15];
    int returnsCount;
} MethodStructure;

typedef struct Service_t
{
    void *service_data;
    functionPtr service_method[MAX_FUNCTIONS];
    ParserPtr parserMethod;
    MethodStructure testMethod[10];
    service_enum service_name;
    int service_method_count;
    char serviceString[100];
    int (*add_service)(struct Service_t *, functionPtr, int);
    int (*add_serviceTest)(struct Service_t *, testingPtr, int, char *);
    void (*serviceTestParameterDescription)(struct Service_t *, int, char *, char *, uint64_t, bool, bool, bool, bool);
    char *(*ConvertTagToXML)(char *, void *);
} Service;

int service_init(Service *service, void *service_to_point_at, int service_name);
int add_service(Service *, functionPtr, int);
int add_service_test(Service *service, testingPtr fnptr, int location, char *name);
char *ConvertTagToXML(int argc, ...);
void add_serviceTestParameterDescription(Service *service, int location, char *tag, char *type, uint64_t size, bool variableSize, bool encrypted, bool base64Encoded, bool isReturn);
void printParameter(Parameter *parameter);

#endif