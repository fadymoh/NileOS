#include "pci_device.h"

uint16_t PciConfigReadWord(uint8_t p_bus, uint8_t p_device, uint8_t p_function, uint8_t p_offset)
{
    // Compose the address:
    //  Bit 23-16 : bus (so we shift left 16 bits))
    //  Bit 15-11 : device (so we shift left 11 bits))
    //  Bit 10-8 : function (so we shift left 8 bits))
    //  Bit 7-2 : so we clear the last two bytes by & 0xfc
    //  Bit 31 : Enable bit, and to set it we | 0x80000000
    uint32_t l_address = (uint32_t)((p_bus << 16) | (p_device << 11) |
                                    (p_function << 8) | (p_offset & 0xfc) | ((uint32_t)0x80000000));
    // Write the address to the address port
    outportl(PCI_ADDRESS, l_address);
    // Read and return data from data port
    return (uint16_t)((inportl(PCI_DATA) >> ((p_offset & 2) * 8)) & 0xffff);
}

void PciConfigWriteWord(uint8_t p_bus, uint8_t p_device, uint8_t p_function, uint8_t p_offset, uint16_t p_value)
{
    // Compose the address:
    //  Bit 23-16 : bus (so we shift left 16 bits))
    //  Bit 15-11 : device (so we shift left 11 bits))
    //  Bit 10-8 : function (so we shift left 8 bits))
    //  Bit 7-2 : so we clear the last two bytes by & 0xfc
    //  Bit 31 : Enable bit, and to set it we | 0x80000000
    uint32_t l_address = (uint32_t)((p_bus << 16) | (p_device << 11) |
                                    (p_function << 8) | (p_offset & 0xfc) | ((uint32_t)0x80000000));
    // Write the address to the address port
    outportl(PCI_ADDRESS, l_address);
    // Write the value to the data port
    outportl(PCI_DATA, p_value);
}

void PciConfigInitialize(PCIDevice *p_pciConfigHeader, uint8_t p_bus, uint8_t p_device, uint8_t p_function)
{
    // Initialize a pointer to itself
    p_pciConfigHeader->bus = 0;
    p_pciConfigHeader->device = 0;
    p_pciConfigHeader->func = 0;
    PCIDevice *l_hdr = p_pciConfigHeader;

    // Initialize object internal state
    p_pciConfigHeader->bus = p_bus;
    p_pciConfigHeader->device = p_device;
    p_pciConfigHeader->func = p_function;

    for (int i = 0, j = 0; i < PCI_HEADER_SIZE; i = i + 2, j++)
    {
        ((uint16_t *)l_hdr)[j] = PciConfigReadWord(p_pciConfigHeader->bus, p_pciConfigHeader->device, p_pciConfigHeader->func, i);
    }
}

void PciConfigWriteCommand(PCIDevice *p_pciConfigHeader, uint32_t p_address, uint16_t p_command)
{
    p_pciConfigHeader->command = PciConfigReadWord(p_pciConfigHeader->bus,
                                                   p_pciConfigHeader->device,
                                                   p_pciConfigHeader->func, p_address);
    PciConfigWriteWord(p_pciConfigHeader->bus,
                       p_pciConfigHeader->device,
                       p_pciConfigHeader->func, p_address,
                       p_pciConfigHeader->command | p_command);
}

uint16_t ReadStatus(PCIDevice *p_pciConfigHeader)
{
    p_pciConfigHeader->status = PciConfigReadWord(p_pciConfigHeader->bus, p_pciConfigHeader->device, p_pciConfigHeader->func, PCI_STATUS_OFFSET);
    return p_pciConfigHeader->status;
}

uint16_t PciConfigReadCommand(PCIDevice *p_pciConfigHeader)
{
    p_pciConfigHeader->command = PciConfigReadWord(p_pciConfigHeader->bus, p_pciConfigHeader->device, p_pciConfigHeader->func, PCI_COMMAND_OFFSET);
    return p_pciConfigHeader->command;
}

bool EnablePCIBusMastering(PCIDevice *p_pciConfigHeader)
{
    p_pciConfigHeader->command = PciConfigReadWord(p_pciConfigHeader->bus, p_pciConfigHeader->device, p_pciConfigHeader->func, 4);
    PciConfigWriteWord(p_pciConfigHeader->bus, p_pciConfigHeader->device, p_pciConfigHeader->func, 4, p_pciConfigHeader->command | 0x00000006);
    p_pciConfigHeader->command = PciConfigReadWord(p_pciConfigHeader->bus, p_pciConfigHeader->device, p_pciConfigHeader->func, 4);

    if (p_pciConfigHeader->command & 0x00000006)
        return true;
    else
        return false;
}

uint8_t GetPCIBarType(PCIDevice *p_pciConfigHeader, uint8_t p_num)
{
    // If bar number is within range
    if (p_num < BAR_COUNT)
    {
        uint32_t bar = p_pciConfigHeader->bars[p_num];
        return bar & 0x1; // Check the last bit
    }
    return PCI_BAR_OUT_OF_RANGE;
}

uint32_t GetPCIBar(PCIDevice *p_pciConfigHeader, uint8_t p_type)
{
    for (int i = 0; i < BAR_COUNT; i++)
    {
        if ((p_pciConfigHeader->bars[i] & 0x1) == p_type)
            return p_pciConfigHeader->bars[i];
    }
    return 0xFFFFFFFF;
}

void PciPrint(PCIDevice *p_pciConfigHeader)
{
    printk(" %x/%x     Int-line:%d   Int-ping:%d, Programable: %x\n",
           p_pciConfigHeader->vend_id,
           p_pciConfigHeader->dev_id,
           p_pciConfigHeader->int_line,
           p_pciConfigHeader->int_pin,
           p_pciConfigHeader->pci_interface);
    printk("bar[0]=%x, bar[1]=%x, bar[2]=%x, bar[3]=%x, bar[4]=%x, bar[5]=%x\n",
           p_pciConfigHeader->bars[0],
           p_pciConfigHeader->bars[1],
           p_pciConfigHeader->bars[2],
           p_pciConfigHeader->bars[3],
           p_pciConfigHeader->bars[4],
           p_pciConfigHeader->bars[5]);
}