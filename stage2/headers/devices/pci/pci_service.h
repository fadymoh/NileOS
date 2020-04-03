#ifndef PCI_SERVICE_H
#define PCI_SERVICE_H

#include "pci_device.h"
#include "service.h"

enum PCIService_exposed_functions
{
    collect_hw,
    get_pci_device,
    get_pci_device_index,
    get_pci_device_count,
    print_all_pci
};

typedef struct PCIService_t
{
    PCIDevice pciDevices[MAX_PCI_BUSES];
    int total_pci_devices;  // Total number of counted devices
    int total_pci_devices2; // Total number of counted devices

    struct
    {
        uint16_t p_vendor;
        uint16_t p_device;
        uint16_t p_index;
        bool p_summary;
    } params;

    struct
    {
        PCIDevice *pciDevice_ptr;
        uint16_t count;
    } returns;

} PCIService;

void InitializePCIService(PCIService *p_PCIService_ptr, Service *p_service);

void ScanBusesForCount(PCIService *p_pciConfigHeaderManager);
void ScanBusesForAllocation(PCIService *p_pciConfigHeaderManager);
void CollectHWInventory(void *pci_service);
PCIDevice *GetPCIDevice(void *pci_service);
PCIDevice *GetPCIDeviceAtIndex(void *pci_service);
uint16_t GetPCIDeviceCount(void *pci_service);
void PrintAllPCI(void *pci_service);

#endif