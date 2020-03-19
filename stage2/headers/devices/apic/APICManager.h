#ifndef APICMANAGER_H_
#define APICMANAGER_H_

#include "APIC.h"
#include "CoreUtils.h"
#include "ClockUtils.h"
enum apic_exposed_functions {startupAPIC_s,
                             getCurrentCoreId_s,
                             moveInitialKernelStack_s,
                             touchCore_s}; 
typedef struct{
    APIC * apics;
    uint8_t apics_count;
    struct{
        uint64_t p_start_stack;
        uint64_t p_end_stack;
        uint8_t p_apics_count;
    } params;
    struct{
        uint32_t core_id;
    } returns;


} APICManager;

void initAPICManager(APICManager* apicManager, Service* p_service);
void startupAPIC(void* p_apicManager);
void getCurrentCoreId(void* p_apicManager);
bool touchCore (void* p_apicManager);
bool moveInitialKernelStack (void* p_apicManager);

#endif
