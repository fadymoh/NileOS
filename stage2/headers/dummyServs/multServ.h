#ifndef MULTSERV_H
#define MULTSERV_H

#include "service.h"

enum multServExposedFunctions
{
    GETMult
};

typedef struct
{
    uint64_t x;
    uint64_t y;
} MultServParams;

typedef struct MultService_t
{
    uint64_t multiplier_factor;
    uint64_t numberOfTimesToMultiply;
} MultService;

void *ParseMultServXML(void *p_MultiplyService, char *xmlString, int location);
char *Multiply(void *p_MultiplyService, void *p_params);
int initMultiplyService(MultService *multserv, Service *p_service);

#endif