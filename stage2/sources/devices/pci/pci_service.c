#include "pci_service.h"
#include "Kernel.h"

extern Kernel kernel;

void initializePCIService(PCIService *p_PCIService_ptr, Service *p_service)
{
    p_PCIService_ptr->total_pci_devices = 0;
    p_PCIService_ptr->total_pci_devices2 = 0;

    p_service->add_service(p_service, CollectHWInventory, collect_hw);
    p_service->add_service(p_service, GetPCIDevice, get_pci_device);
    p_service->add_service(p_service, GetPCIDeviceAtIndex, get_pci_device_index);
    p_service->add_service(p_service, GetPCIDeviceCount, get_pci_device_count);
    p_service->add_service(p_service, PrintAllPCI, print_all_pci);
}
void ScanBusesForCount(PCIService *p_pciService)
{
    PCIDevice pciDevice;
    for (uint16_t bus = 0; bus < MAX_PCI_BUSES; bus++)
    {
        for (uint8_t i = 0; i < MAX_PCI_BUS_DEVICES; i++)
        {
            for (uint8_t j = 0; j < MAX_PCI_DEVICE_FUNCTIONS; j++)
            {
                PciConfigInitialize(&pciDevice, bus, i, j);
                if (pciDevice.vend_id == 0xFFFF)
                    continue;
                else
                {
                    p_pciService->total_pci_devices += 1;
                    p_pciService->total_pci_devices2++;
                }
            }
        }
    }
}

void ScanBusesForAllocation(PCIService *p_pciService)
{
    uint16_t allocated_pci_devices = 0;

    for (uint16_t bus = 0; bus < MAX_PCI_BUSES; bus++)
    {
        for (uint8_t i = 0; i < MAX_PCI_BUS_DEVICES; i++)
        {
            for (uint8_t j = 0; j < MAX_PCI_DEVICE_FUNCTIONS; j++)
            {
                if (allocated_pci_devices < p_pciService->total_pci_devices) 
                {
                    // initialize the next PCI device header
                    PciConfigInitialize(&p_pciService->pciDevices[allocated_pci_devices], bus, i, j);

                    // If vendor id is 0xFFFF then invalid device and we should skip it
                    // So the next valid device will overwrite it
                    if (p_pciService->pciDevices[allocated_pci_devices].vend_id == 0xFFFF)
                        continue;
                    // Else increment allocated_pci_devices to allocate the next PCI device
                    allocated_pci_devices++;

                    // If bit 7 of the PCI header type is set then this means that the device
                    // has multiple functions so we need to scan them as well.
                    if ((p_pciService->pciDevices[allocated_pci_devices].header & 0x80) != 0)
                    {
                        // It is a multi-function device, so check remaining functions
                        // We will do that later
                    }
                }
                else
                    break;
            }
        }
    }
}

void CollectHWInventory(void *pci_service)
{
    ScanBusesForCount((PCIService *)pci_service);
    ScanBusesForAllocation((PCIService *)pci_service);
}

PCIDevice *GetPCIDevice(void *pci_service)
{

    PCIService *p_pciService = (PCIService *)pci_service;
    uint16_t p_vendor = p_pciService->params.p_vendor;
    uint16_t p_device = p_pciService->params.p_device;
    for (uint8_t i = 0; i < p_pciService->total_pci_devices; i++)
    {
        if (p_pciService->pciDevices[i].vend_id == p_vendor &&
            p_pciService->pciDevices[i].dev_id == p_device)
        {
            p_pciService->returns.pciDevice_ptr = &p_pciService->pciDevices[i];
            return &p_pciService->pciDevices[i];
        }
    }

    p_pciService->returns.pciDevice_ptr = NULL;
    return NULL;
}

PCIDevice *GetPCIDeviceAtIndex(void *pci_service)
{
    PCIService *p_pciService = (PCIService *)pci_service;
    uint16_t p_vendor = p_pciService->params.p_vendor;
    uint16_t p_device = p_pciService->params.p_device;
    uint16_t p_index = p_pciService->params.p_index;

    uint16_t ind = 0;
    for (uint8_t i = 0; i < p_pciService->total_pci_devices; i++)
    {
        if (p_pciService->pciDevices[i].vend_id == p_vendor &&
            p_pciService->pciDevices[i].dev_id == p_device)
        {
            if (ind == p_index)
            {
                kernel.pciService.returns.pciDevice_ptr = &p_pciService->pciDevices[i];
                return &p_pciService->pciDevices[i];
            }
            else
                ind++;
        }
    }
    kernel.pciService.returns.pciDevice_ptr = NULL;
    return NULL;
}

uint16_t GetPCIDeviceCount(void *pci_service)
{
    uint16_t count = 0;
    PCIService *p_pciService = (PCIService *)pci_service;
    uint16_t p_vendor = p_pciService->params.p_vendor;
    uint16_t p_device = p_pciService->params.p_device;
    for (uint8_t i = 0; i < p_pciService->total_pci_devices; i++)
    {
        if (p_pciService->pciDevices[i].vend_id == p_vendor &&
            p_pciService->pciDevices[i].dev_id == p_device)
            count++;
    }
    
    kernel.pciService.returns.count = count;
    return count;
}

void PrintAllPCI(void *pci_service)
{
    PCIService *p_pciService = (PCIService *)pci_service;
    bool p_summary = p_pciService->params.p_summary;
    uint16_t total_pcis = p_pciService->total_pci_devices;
    printk("Total Number of PCI Devices: %d\n", total_pcis);
    for (int i = 0; i < total_pcis; ++i)
    {
        PciPrint(&p_pciService->pciDevices[i]);
    }
}