#ifndef ACPI_H_
#define ACPI_H_
#include "MemoryUtilities.h"
#include "PhysicalMemory.h"

typedef struct{
        uint8_t type;           // Type of the header 
        uint8_t length;         // Length of the APIC table
} APICHeader;

typedef struct{
        uint32_t signature; // Signature identifying the type of the header
        uint32_t length;    // Length of the table including the header
        uint8_t revision;   
        uint8_t checksum;
        uint8_t oem[6];
        uint8_t oemTableId[8];
        uint32_t oemRevision;
        uint32_t creatorId;
        uint32_t creatorRevision;
} ACPIHeader;

typedef struct{
  ACPIHeader h;
  uint32_t next;
}RSDT;

typedef struct{
  ACPIHeader h;
  uint64_t next;
}XSDT;

typedef struct{
        ACPIHeader header;      // An object representing the ACPIHeader header
        uint32_t localApicAddr; // 4K aligned MMIO Local APIC Address
        uint32_t flags;         // 1 if processor is enabled

}ACPIMadt;

typedef struct{
        char signature[8]; // 8-bytes string that represents the 
        uint8_t checksum;   // Checksum for version 1.0 table
        char OEMID[6];      // A string identifying the OEM version
        uint8_t revision;   // if 0 then APCI version 1,
                            // if 1 then APCI version 2,
                            // if 2 then APCI version 3,
        uint32_t rsdtAddr;  // Address of the Root System Descriptor Table
        
        // The following are used for ACPI V 2.0 or above 
        uint32_t length;    // Length of the table including header
        uint64_t xsdtAddr;  // Address to the table.
        uint8_t extendedChecksum;   // Checksum of the entire table
        uint8_t reserved[3];        // Ignored
} RSDPDescriptor;

typedef struct{
    RSDPDescriptor * rsdpDescriptor;
    PhysicalMemoryManager * physicalMemoryManager;
    uint64_t local_apic_addr;
    uint64_t local_apicio_addr;
    uint16_t cores_count;
} ACPI;



typedef struct {
    APICHeader          header;
    uint8_t             ioapic_id;
    uint8_t             res; /* 0 */
    uint32_t            address;
    uint32_t            gs_interrupt_base;
} APICIO;

typedef struct  {
    APICHeader          header;
    uint8_t             bus;
    uint8_t             src;
    uint32_t            gs_interrupt;
    uint16_t            mps_inti_flags;
} acpi_int_override;


bool parseAPIC (ACPI * p_acpi, ACPIMadt * madt);
bool parseAPICV1(ACPI * p_acpi);
bool parseAPICV2(ACPI * p_acpi);

bool parseACPIHeader(ACPI * p_acpi);
RSDPDescriptor * parseRSDT (uint8_t * p_rsdp);
bool init_acpi (ACPI * p_acpi);


#endif
