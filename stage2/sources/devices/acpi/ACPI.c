#include "ACPI.h"
#include "Kernel.h"
extern Kernel kernel;

bool parseAPIC(ACPI *p_acpi, ACPIMadt *madt)
{
    // Get the start and the end pointer of the MADT
    uint8_t *p = (uint8_t *)(madt + 1);
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    // Save the Local APIC address into a data member for future use
    p_acpi->local_apic_addr = madt->localApicAddr;
    kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apic_addr;
    if (!isAddress(&kernel.physicalMemoryManager))
    {
        kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apic_addr;
        kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE;

        addPhysicalMemoryEntry(&kernel.physicalMemoryManager);
    }

    // initialize the count of the cores
    p_acpi->cores_count = 0;

    while (p < end) // Loop through the whole address range
    {
        APICHeader *header = (APICHeader *)p; // Cast the current pointer location
                                              // to APICHeader class definition
        uint8_t type = header->type;          // Get the header type
        uint8_t length = header->length;      // Get the header length
        if (type == APIC_TYPE_LOCAL_APIC)     // If Local APIC
        {
            if (p != NULL)
                p_acpi->cores_count++;
        }
        else if (type == APIC_TYPE_IO_APIC) // If IO APIC
        {
            APICIO *apicio = (APICIO *)header;
            p_acpi->local_apicio_addr = apicio->address;
            kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apicio_addr;
            if (!isAddress(&kernel.physicalMemoryManager))
            {
                kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apicio_addr;
                kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE;
                addPhysicalMemoryEntry(&kernel.physicalMemoryManager);
            }
        }
        else if (type == APIC_TYPE_INTERRUPT_OVERRIDE) // If APIC Interrupt
        {
            printk_debug("APIC Interrupt\n");
        }
        else // Unknown APIC structure
        {
            printk_debug("Unknown APIC Structure\n");
        }
        p += length; // Advance header pointer
    }

    return true;
}

bool parseAPICV1(ACPI *p_acpi)
{
    p_acpi->physicalMemoryManager->params.p_physical_address = p_acpi->rsdpDescriptor->rsdtAddr;
    ACPIHeader *rsdt = (ACPIHeader *)getVirtualAddress(p_acpi->physicalMemoryManager);

    uint32_t *p = (uint32_t *)(rsdt + 1);
    uint32_t *end = (uint32_t *)(((uint8_t *)rsdt) + rsdt->length);

    while (p < end) // Loop through the whole address range
    {

        uint64_t address = *p++; // Assign and increment header address
                                 // Convert to virtual address

        p_acpi->physicalMemoryManager->params.p_physical_address = address;

        address = getVirtualAddress(p_acpi->physicalMemoryManager);
        // Get the header signature
        uint64_t signature = ((ACPIHeader *)address)->signature;

        if (signature == 0x50434146) // FADT Table
        {
            printk_debug("Found FADT Table\n");
        }
        else if (signature == 0x43495041) // MADT Table
        {
            printk_debug("Found MADT Table\n");
            ACPIMadt *acpiMadt = (ACPIMadt *)address;
            parseAPIC(p_acpi, acpiMadt);
        }
        else // Unknown RSDT signature
        {
            printk_debug("Found Unknown RSDT Signature\n");
        }
    }
    // Return true
    return true;
}

bool parseAPICV2(ACPI *p_acpi)
{
    p_acpi->physicalMemoryManager->params.p_physical_address = p_acpi->rsdpDescriptor->xsdtAddr;
    ACPIHeader *rsdt = (ACPIHeader *)getVirtualAddress(p_acpi->physicalMemoryManager);

    uint64_t *p = (uint64_t *)(rsdt + 1);
    uint64_t *end = (uint64_t *)(((uint8_t *)rsdt) + rsdt->length);

    while (p < end) // Loop through the whole address range
    {

        uint64_t address = *p++;
        // Convert to virtual address
        p_acpi->physicalMemoryManager->params.p_physical_address = address;
        address = getVirtualAddress(p_acpi->physicalMemoryManager);

        // Get the header signature
        uint64_t signature = ((ACPIHeader *)address)->signature;

        if (signature == 0x50434146) // FADT Table
        {
            printk_debug("Found FADT Table\n");
        }
        else if (signature == 0x43495041) // MADT Table
        {
            printk_debug("Found MADT Table\n");
            ACPIMadt *acpiMadt = (ACPIMadt *)address;
            parseAPIC(p_acpi, acpiMadt);
        }
        else // Unknown RSDT signature
        {
            printk_debug("Found Unknown RSDT Signature\n");
        }
    }

    return true;
}

bool parseACPIHeader(ACPI *p_acpi)
{

    if (p_acpi->rsdpDescriptor->revision == 0 || p_acpi->rsdpDescriptor->xsdtAddr == 0x0) // Version 1
    {
        printk_debug("RSDT DETECTED\n");
        return parseAPICV1(p_acpi);
    }

    if (p_acpi->rsdpDescriptor->revision >= 1) // version 2 or 3
    {
        printk_debug("XSDT DETECTED\n");
        return parseAPICV2(p_acpi);
    }
    else
    {
        printk_debug("Version of ACPI Not Supported\n");
    }

    return false; // If version is not supported
}

RSDPDescriptor *parseRSDT(uint8_t *p_rsdp)
{

    uint8_t sum = 0;
    uint8_t i = 0;

    for (i = 0; i < 20; ++i)
        sum += p_rsdp[i];

    if (sum)
        return NULL;

    RSDPDescriptor *rsdt = (RSDPDescriptor *)p_rsdp;

    return rsdt;
}

bool init_acpi(ACPI *p_acpi)
{

    uint8_t *ptr = (uint8_t *)0x000e0000;
    uint8_t *end = (uint8_t *)0x000fffff;
    while (ptr < end) // Loop on the whole address range
    {
        if (memcmp8(ptr, (const uint8_t *)RSD_SIGNATURE, strlen(RSD_SIGNATURE)) == NULL) // Compare current memory location with signature
        {
            p_acpi->rsdpDescriptor = parseRSDT(ptr);
            if (p_acpi->rsdpDescriptor != NULL)
            {
                return parseACPIHeader(p_acpi);
            }
            else
                return false;
        }
        ptr += 16; // Advance pointer by 16 bytes as the APIC is located at a 16 bytes aligned address
    }

    return false;
}
