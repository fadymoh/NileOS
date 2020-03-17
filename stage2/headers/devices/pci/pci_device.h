#ifndef PCI_DEVICE_H
#define PCI_DEVICE_H

#include "defines.h"
#include "includes.h"
#include "Ports.h"


// --- Actions -------------------------------------------------------- //

// Enable ROM address.
#define PCI_ROM_ADDRESS_ENABLE          0x01
// The device is I/O-based (can perform I/O operations using the ports)
#define PCI_COMMAND_IO                  0x01
// The device is memory-based (can perform I/O operations by a memory-mapped buffer)
#define PCI_COMMAND_MEMORY              0x02
// Enable bus master ( 32-bit DMA).
#define PCI_COMMAND_MASTER              0x04

// --- Resources ------------------------------------------------------ //

// First base address register (BARs). Every PCI device has up to 6 base
// addresses (6 for normal devices, 2 for PCI to PCI bridges and
// only 1 for cardbuses).
#define PCI_BASE_ADDRESS_0              0x10
// The base address is I/O-based (it is a port value).
#define PCI_BASE_ADDRESS_SPACE          0x01
// The base address is memory-based ( it is a memory address value).
#define PCI_BASE_ADDRESS_SPACE_MEMORY   0x00
// The device is memory-based (perform I/O operations by a memory-mapped buffer).
#define PCI_IO_RESOURCE_MEM             0x00
// The device is I/O-based (perform I/O operations using the ports).
#define PCI_IO_RESOURCE_IO              0x01
// Used for return when uint8_t PCIConfigHeader::getPCIBarType(uint8_t p_num)
// is used with p_num >= 6
#define PCI_BAR_OUT_OF_RANGE            0x04

// This is a normal PCI device.
#define PCI_HEADER_TYPE_NORMAL          0
// This is a bridge PCI device.
#define PCI_HEADER_TYPE_BRIDGE          1
// This is a card-bus PCI device.
#define PCI_HEADER_TYPE_CARDBUS         2


// Number of BARs in a normal PCI device header
#define BAR_COUNT                       6

#define PCI_COMMAND_OFFSET              4

#define PCI_STATUS_OFFSET               6

// Port number for PCI command address
#define PCI_ADDRESS     0xCF8
// Port number for PCI data address
#define PCI_DATA        0xCFC
// A variable defining the maximum number of PCI devices that can exist in 
// a computer
#define MAX_PCI_DEVICES 256*32*8

#define MAX_PCI_BUSES            256
#define MAX_PCI_BUS_DEVICES      32
#define MAX_PCI_DEVICE_FUNCTIONS 8

#define PCI_BAR_MEM  0x0
#define PCI_BAR_IO   0x1
#define PCI_BAR_NONE 0x3

#define PCI_HEADER_SIZE     0x3f


typedef struct PCIDevice_t
{
    // Notice that the fields are switched e.g. Vendor ID followed by Device ID

        uint16_t vend_id;       // Vendor ID
        uint16_t dev_id;        // Device ID
        uint16_t command;       // Command
        uint16_t status;        // Status
        uint8_t rev;            // revision
        uint8_t pci_interface;  // Prog IF
        uint8_t pci_minor;      // Subclass
        uint8_t pci_major;      // Class Code
        uint8_t clg;            // Cache Line Size
        uint8_t latency;        // Latency Timer
        uint8_t header;         // Header Type
        uint8_t bist;           // BIST (Built-in Self Test)
        uint32_t bars[6];       // 6 Base Address Registers
        uint32_t reserved[2];   // 8 reserved bytes
        uint32_t romaddr;       // Expansion ROM Base Address
        uint32_t reserved2[2];  // 8 reserved bytes
        uint8_t int_line;       // Interrupt Line
        uint8_t int_pin;        // Interrupt Pin
        uint8_t mingnt;         // Min Grant
        uint8_t max_lat;        // Max Latency
        uint8_t data[192];

        int bus;                // The bus number that this device is on
        int device;             // Device Number
        int func;               // Function Number
}PCIDevice;


uint16_t pciConfigReadWord ( uint8_t p_bus, uint8_t p_device, uint8_t p_function, uint8_t p_offset);
void pciConfigWriteWord ( uint8_t p_bus, uint8_t p_device, uint8_t p_function, uint8_t p_offset,uint16_t p_value);
void pciConfig_initialize (PCIDevice* p_pciConfigHeader,uint8_t p_bus, uint8_t p_device,uint8_t p_function);
void pciConfigWriteCommand (PCIDevice* p_pciConfigHeader,uint32_t p_address,uint16_t p_command);

uint16_t readStatus (PCIDevice* p_pciConfigHeader);
uint16_t pciConfigReadCommand (PCIDevice* p_pciConfigHeader);
bool enablePCIBusMastering (PCIDevice* p_pciConfigHeader);
uint8_t getPCIBarType(PCIDevice* p_pciConfigHeader,uint8_t p_num);
uint32_t getPCIBar(PCIDevice* p_pciConfigHeader,uint8_t p_type);
void pciPrint(PCIDevice* p_pciConfigHeader);



#endif