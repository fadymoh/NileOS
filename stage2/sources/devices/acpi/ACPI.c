#include "ACPI.h"
#include "Kernel.h"
extern Kernel kernel;
bool parseAPIC (ACPI * p_acpi, ACPIMadt * madt)
{
    
    // Get the start and the end pointer of the MADT
    uint8_t *p = (uint8_t *)(madt + 1);
    uint8_t *end = (uint8_t *)madt + madt->header.length;
    
    // Save the Local APIC address into a data member for future use
    p_acpi->local_apic_addr = madt->localApicAddr;
    kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apic_addr;
    if ( !isAddress(&kernel.physicalMemoryManager))
    {
//        printk ("Adding physical address: %x\n",p_acpi->local_apic_addr);
    kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apic_addr;
    kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE;

        addPhysicalMemoryEntry(&kernel.physicalMemoryManager);
    }
    //else printk ("Failed adding physical address: %x\n",p_acpi->local_apic_addr);
    // initialize the count of the cores
    p_acpi->cores_count = 0;
    
    while (p < end) // Loop through the whole address range
    {
        APICHeader *header = (APICHeader *)p; // Cast the current pointer location 
                                              // to APICHeader class definition
        uint8_t type = header->type;        // Get the header type
        uint8_t length = header->length;    // Get the header length
        if (type == APIC_TYPE_LOCAL_APIC)   // If Local APIC
        {
//            LocalAPIC *s = (LocalAPIC *)p;
            if ( /*s*/ p != NULL) p_acpi->cores_count ++;  // increment core count
        }
        else if (type == APIC_TYPE_IO_APIC) // If IO APIC
        {
            APICIO * apicio = (APICIO *) header;
            p_acpi->local_apicio_addr = apicio->address;
            kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apicio_addr;
            if ( !isAddress(&kernel.physicalMemoryManager))
            {
                kernel.physicalMemoryManager.params.p_physical_address = p_acpi->local_apicio_addr;
                kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE;
                addPhysicalMemoryEntry(&kernel.physicalMemoryManager);
            }   
  /*          console_addStringToCurrentTerminal(&kernel.console,"APIC_TYPE_IO_APIC:",COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addHexaToCurrentTerminal(&kernel.console,apicio->address,COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);            
            console_addStringToCurrentTerminal(&kernel.console,"apicio->gs_interrupt_base: ",COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addHexaToCurrentTerminal(&kernel.console,apicio->gs_interrupt_base,COLOR_BLUE,COLOR_LIGHT_8BROWN);
            console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);            */
        }
        else if (type == APIC_TYPE_INTERRUPT_OVERRIDE) // If APIC Interrupt
        {
/*                acpi_int_override * acpi_int_ov = (acpi_int_override *) header;
                
            console_addStringToCurrentTerminal(&kernel.console,"acpi_int_ov->bus:",COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addHexaToCurrentTerminal(&kernel.console,acpi_int_ov->bus,COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);            
            console_addStringToCurrentTerminal(&kernel.console,"acpi_int_ov->src:",COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addHexaToCurrentTerminal(&kernel.console,acpi_int_ov->src,COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);            
            console_addStringToCurrentTerminal(&kernel.console,"acpi_int_ov->gs_interrupt:",COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addHexaToCurrentTerminal(&kernel.console,acpi_int_ov->gs_interrupt,COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);            
            console_addStringToCurrentTerminal(&kernel.console,"acpi_int_ov->mps_inti_flags:",COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addHexaToCurrentTerminal(&kernel.console,acpi_int_ov->mps_inti_flags,COLOR_BLUE,COLOR_LIGHT_BROWN);
            console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);            
*/                
        }
        else    // Unknown APIC structure
        {
        }
        p += length; // Advance header pointer
    }
    // Return true
    return true;

}


bool parseAPICV1(ACPI * p_acpi)
{
        p_acpi->physicalMemoryManager->params.p_physical_address = p_acpi->rsdpDescriptor->rsdtAddr;
        ACPIHeader * rsdt =  (ACPIHeader *) getVirtualAddress(p_acpi->physicalMemoryManager);
        // Get the start and the end pointer of the RSDT
        // For ACPI V1 pointer should be uint32_t

        uint32_t *p = (uint32_t *)(rsdt+1);
        uint32_t *end = (uint32_t *)(((uint8_t*)rsdt) + rsdt->length);

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
//                video->putString("Found FADT\n",COLOR_RED,COLOR_BLACK);

            }
            else if (signature == 0x43495041) // MADT Table
            {
                // Parse the MADT table
//                console_addStringToCurrentTerminal(&kernel.console,"MADT\n",COLOR_BLUE,COLOR_LIGHT_BROWN);
                
                ACPIMadt * acpiMadt = (ACPIMadt *) address;
                parseAPIC(p_acpi,acpiMadt);
            }
            else // Unknown RSDT signature
            {
            }
        }
        // Return true
        return true;
}

bool parseAPICV2(ACPI * p_acpi)
{
         p_acpi->physicalMemoryManager->params.p_physical_address = p_acpi->rsdpDescriptor->xsdtAddr;
        ACPIHeader * rsdt = (ACPIHeader *) getVirtualAddress(p_acpi->physicalMemoryManager);

        // Get the start and the end pointer of the RSDT
        // For ACPI V1 pointer should be uint32_t
        uint64_t *p = (uint64_t *)(rsdt+1);
  /*      console_addStringToCurrentTerminal(&kernel.console,"rsdt[0]:",COLOR_RED,COLOR_LIGHT_BROWN);     
        console_addHexaToCurrentTerminal(&kernel.console,((uint64_t*)rsdt)[0],COLOR_BLUE,COLOR_LIGHT_BROWN);
        console_addStringToCurrentTerminal(&kernel.console,"\n",COLOR_BLUE,COLOR_LIGHT_BROWN);
*/
        uint64_t *end = (uint64_t *)(((uint8_t*)rsdt) + rsdt->length);

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
                    printk("Detected FADT\n");
            }
            else if (signature == 0x43495041) // MADT Table
            {
                printk("Detetected MADT\n");

                ACPIMadt * acpiMadt = (ACPIMadt *) address;
                parseAPIC(p_acpi,acpiMadt);
            }
            else // Unknown RSDT signature
            {
            }
        }
        // Return true
        return true;
}

bool parseACPIHeader(ACPI * p_acpi)
{
    
    if ( p_acpi->rsdpDescriptor->revision == 0 || p_acpi->rsdpDescriptor->xsdtAddr == 0x0 ) // Version 1
    {
        printk("RSDT DETECTED\n");     
        return parseAPICV1(p_acpi);
    }
    // Currently we support version 1 but code for other version will
    // be added in the future

    if ( p_acpi->rsdpDescriptor->revision >= 1 ) // version 2 or 3 
    {
        printk("XSDT DETECTED\n");     
        return parseAPICV2(p_acpi);        
    }
     else return false; // If version is not supported
}


RSDPDescriptor *  parseRSDT (uint8_t * p_rsdp)
{

    uint8_t sum = 0;
    uint8_t i = 0;
    // Perform check sum
    // printk("p_rsdp: %x\n", p_rsdp);
    // while(1);
    for ( i = 0; i < 20; ++i) sum += p_rsdp[i];
    
    // If check sum is not zero return false;
    if (sum) return NULL;
    // Else set the check sum validation flag
    
    RSDPDescriptor * rsdt = (RSDPDescriptor * )p_rsdp;

/*    memcpy((uint8_t *)rsdpDescriptor->signature,ptr,8);
    ptr += 8;
    rsdpDescriptor->checksum = ptr[0];
    ptr++;
    memcpy((uint8_t *)rsdpDescriptor->OEMID,ptr,6);
    ptr += 6;
    rsdpDescriptor->revision = ptr[0];
    ptr ++;
    rsdpDescriptor->rsdtAddr = ((uint32_t*) ptr)[0];
    ptr += 4;
    rsdpDescriptor->length = ((uint32_t*) ptr)[0];
    ptr += 4;
    rsdpDescriptor->xsdtAddr = ((uint64_t*) ptr)[0];
    ptr += 8;
    rsdpDescriptor->extendedChecksum = ptr[0];
    ptr++;
    memcpy((uint8_t *)rsdpDescriptor->reserved,ptr,3);
    return true;*/
    return rsdt;
}


bool init_acpi (ACPI * p_acpi)
{
    
    uint8_t * ptr = (uint8_t *)0x000e0000;
    uint8_t *end = (uint8_t *)0x000fffff;
    while (ptr < end) // Loop on the whole address range
    {
        if (memcmp8(ptr,(const uint8_t *)RSD_SIGNATURE,strlen(RSD_SIGNATURE)) == NULL) // Compare current memory location with signature
        {
           p_acpi->rsdpDescriptor = parseRSDT (ptr);
           if ( p_acpi->rsdpDescriptor != NULL) 
           {
               return parseACPIHeader(p_acpi);
           }
          else return false;
        }
        ptr += 16; // Advance pointer by 16 bytes as the APIC is located at a 16 bytes aligned address
    }

    // Return true
    return false;
}
