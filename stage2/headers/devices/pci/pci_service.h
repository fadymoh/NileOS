#ifndef PCI_SERVICE_H
#define PCI_SERVICE_H

#include "pci_device.h"
#include "service.h"

enum PCIService_exposed_functions{COLLECT_HW, GET_PCI_DEVICE, GET_PCI_DEVICE_INDEX, GET_PCI_DEVICE_COUNT, PCI_SERVICE_PRINT};


typedef struct  PCIService_t {
        PCIDevice pciDevices[MAX_PCI_BUSES]; 
        int total_pci_devices; // Total number of counted devices
        int total_pci_devices2; // Total number of counted devices

    struct {
        uint16_t p_vendor;
        uint16_t p_device;
        uint16_t p_index;
        bool p_summary;
    } params;

    struct {
        PCIDevice* pciDevice_ptr; 
    } returns;

}PCIService;



void initializePCIService(PCIService* p_PCIService_ptr, Service* p_service);

void scanBusesForCount(PCIService* p_pciConfigHeaderManager);
void scanBusesForAllocation(PCIService* p_pciConfigHeaderManager);
void collectHWInventory(void* pci_service);
PCIDevice* getPCIDevice(void* pci_service);
PCIDevice* getPCIDeviceAtIndex(void* pci_service);
uint16_t getPCIDeviceCount(void* pci_service);
void pciServicePrint (void* pci_service);





#endif