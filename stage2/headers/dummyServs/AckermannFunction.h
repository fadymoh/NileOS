#ifndef ACKERMANNFUNCTION_H
#define ACKERMANNFUNCTION_H

#include "service.h"

enum ACKERMANNFUNCTIONExposedFunctions
{
    AckFunction_t
};

typedef struct
{
    uint64_t m;
    uint64_t n;
} AckermannFunctionParams;

typedef struct AckermannFunction_t
{
    uint64_t numberOfTimesToCallFunction;
} AckermannFunction;

void *ParseAckermannFunctionXML(void *p_AckermannFunction, char *xmlString, int location);
char *CallAckermannFunction(void *p_AckermannFunction, void *p_params);
int initAckermannFunctionService(AckermannFunction *ackermannFunction, Service *p_service);

#endif