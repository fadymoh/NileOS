#include "pci_service.h"
#include "Kernel.h"

extern Kernel kernel;

void initializePCIService(PCIService *p_PCIService_ptr, Service *p_service)
{
    p_PCIService_ptr->total_pci_devices = 0;
    p_PCIService_ptr->total_pci_devices2 = 0;

    p_service->add_service(p_service, collectHWInventory, COLLECT_HW);
    p_service->add_service(p_service, getPCIDevice, GET_PCI_DEVICE);
    p_service->add_service(p_service, getPCIDeviceAtIndex, GET_PCI_DEVICE_INDEX);
    p_service->add_service(p_service, getPCIDeviceCount, GET_PCI_DEVICE_COUNT);
    p_service->add_service(p_service, pciServicePrint, PCI_SERVICE_PRINT);
}
void scanBusesForCount(PCIService *p_pciService)
{
    PCIDevice pciDevice;
    for (uint16_t bus = 0; bus < MAX_PCI_BUSES; bus++)
    {
        for (uint8_t i = 0; i < MAX_PCI_BUS_DEVICES; i++)
        {
            for (uint8_t j = 0; j < MAX_PCI_DEVICE_FUNCTIONS; j++)
            {
                pciConfig_initialize(&pciDevice, bus, i, j);
                if (pciDevice.vend_id == 0xFFFF)
                    continue;
                else
                {
                    // printk("Found a device..\n");
                    p_pciService->total_pci_devices += 1;
                    p_pciService->total_pci_devices2++;
                }
            }
        }
    }
}

void scanBusesForAllocation(PCIService *p_pciService)
{
    uint16_t allocated_pci_devices = 0;
    // p_pciService->pciDevices = (PCIConfigHeader *) kmalloc(&kernel.memoryAllocator, sizeof(PCIConfigHeader)*p_pciService->total_pci_devices);

    for (uint16_t bus = 0; bus < MAX_PCI_BUSES; bus++)
    {
        for (uint8_t i = 0; i < MAX_PCI_BUS_DEVICES; i++)
        {
            for (uint8_t j = 0; j < MAX_PCI_DEVICE_FUNCTIONS; j++)
            {
                if (allocated_pci_devices < p_pciService->total_pci_devices) // If the allocated number of devices is equal to
                                                                             // the counted devices then stop as there are no more
                                                                             // devices to allocate
                {
                    // initialize the next PCI device header
                    pciConfig_initialize(&p_pciService->pciDevices[allocated_pci_devices], bus, i, j);

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

void collectHWInventory(void *pci_service)
{
    scanBusesForCount((PCIService *)pci_service);
    scanBusesForAllocation((PCIService *)pci_service);
}

PCIDevice *getPCIDevice(void *pci_service)
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
PCIDevice *getPCIDeviceAtIndex(void *pci_service)
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
                return &p_pciService->pciDevices[i];
            else
                ind++;
        }
    }
    return NULL;
}

uint16_t getPCIDeviceCount(void *pci_service)
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
    return count;
}

void pciServicePrint(void *pci_service)
{
    PCIService *p_pciService = (PCIService *)pci_service;
    bool p_summary = p_pciService->params.p_summary;
    uint16_t total_pcis = p_pciService->total_pci_devices;
    printk("Total Number of PCI Devices: %d\n", total_pcis);
    for (int i = 0; i < total_pcis; ++i)
    {
        pciPrint(&p_pciService->pciDevices[i]);
    }
}