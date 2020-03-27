#include "E1000.h"
#include "Kernel.h"

static uint16_t e1000_models[E1000_MODELS_COUNT] = {E1000_DEV,
                                                    E1000_DEV1,
                                                    E1000_DEV2,
                                                    E1000_DEV3,
                                                    E1000_DEV4};
extern Kernel kernel;
void e1000WriteCommand(E1000 *p_e1000, uint16_t p_address, uint32_t p_value)
{
    if (p_e1000->bar_type == 0)
    {
        mmio_write32(p_e1000->mem_base + p_address, p_value);
    }
    else
    {
        outportl(p_e1000->io_base, p_address);
        outportl(p_e1000->io_base + 4, p_value);
    }
}
uint32_t e1000ReadCommand(E1000 *p_e1000, uint16_t p_address)
{
    if (p_e1000->bar_type == 0)
    {
        return mmio_read32(p_e1000->mem_base + p_address);
    }
    else
    {
        outportl(p_e1000->io_base, p_address);
        return inportl(p_e1000->io_base + 4);
    }
}

void e1000DetectEEProm(E1000 *p_e1000)
{
    uint32_t val = 0;
    e1000WriteCommand(p_e1000, REG_EEPROM, 0x1);
    printk_network("Detecting EEPROM\n");
    for (int i = 0; i < 1000 && !p_e1000->eerprom_exists; i++) ///while( val & 0x2 || val & 0x10)
    {

        val = e1000ReadCommand(p_e1000, REG_EEPROM);
        if (val & 0x10)
            p_e1000->eerprom_exists = true;
        else
            p_e1000->eerprom_exists = false;
    }
    //return p_e1000->eerprom_exists;
}

uint32_t e1000EEpromRead(E1000 *p_e1000, uint8_t addr)
{
    uint16_t data = 0;
    uint32_t tmp = 0;
    if (p_e1000->eerprom_exists)
    {
        e1000WriteCommand(p_e1000, REG_EEPROM, (1) | ((uint32_t)(addr) << 8));
        while (!((tmp = e1000ReadCommand(p_e1000, REG_EEPROM)) & (1 << 4)))
            ;
    }
    else
    {
        e1000WriteCommand(p_e1000, REG_EEPROM, (1) | ((uint32_t)(addr) << 2));
        while (!((tmp = e1000ReadCommand(p_e1000, REG_EEPROM)) & (1 << 1)))
            ;
    }
    data = (uint16_t)((tmp >> 16) & 0xFFFF);
    return data;
}

void e1000ReadMACAddress(E1000 *p_e1000)
{
    if (p_e1000->eerprom_exists)
    {
        printk_network("EEPROM Exists\n");
        uint32_t temp;
        temp = e1000EEpromRead(p_e1000, 0);
        printk_network("EEPROM Exists 1\n");

        p_e1000->mac[0] = temp & 0xff;
        p_e1000->mac[1] = temp >> 8;
        temp = e1000EEpromRead(p_e1000, 1);
        printk_network("EEPROM Exists 2\n");

        p_e1000->mac[2] = temp & 0xff;
        p_e1000->mac[3] = temp >> 8;
        temp = e1000EEpromRead(p_e1000, 2);
        printk_network("EEPROM Exists 3\n");

        p_e1000->mac[4] = temp & 0xff;
        p_e1000->mac[5] = temp >> 8;
    }
    else
    {
        printk_network("EEPROM Doesnt Exists\n");

        uint8_t *mem_base_mac_8 = (uint8_t *)(p_e1000->mem_base + 0x5400);
        uint32_t *mem_base_mac_32 = (uint32_t *)(p_e1000->mem_base + 0x5400);
        if (mem_base_mac_32[0] != 0)
        {
            for (int i = 0; i < 6; i++)
            {
                p_e1000->mac[i] = mem_base_mac_8[i];
            }
        }
        else
            return;
    }
    printk_network("MAC Address: %y:%y:%y:%y:%y:%y",
                          p_e1000->mac[0],
                          p_e1000->mac[1],
                          p_e1000->mac[2],
                          p_e1000->mac[3],
                          p_e1000->mac[4],
                          p_e1000->mac[5]);
    return;
}

void e1000Scan()
{
    uint16_t e1000_index = 0;
    for (uint8_t i = 0; i < E1000_MODELS_COUNT; i++)
    {
        kernel.pciService.params.p_vendor = INTEL_VEND;
        kernel.pciService.params.p_device = e1000_models[i];
        dispatch_kernel(&kernel.service_transporter, pci_t, GET_PCI_DEVICE_COUNT);
        uint16_t count = kernel.pciService.returns.count;
        if (count > 0)
        {
            for (uint16_t j = 0; j < count; j++)
            {
                kernel.pciService.params.p_vendor = INTEL_VEND;
                kernel.pciService.params.p_device = e1000_models[i];
                kernel.pciService.params.p_index = j;
                dispatch_kernel(&kernel.service_transporter, pci_t, GET_PCI_DEVICE_INDEX);
                PCIDevice *e1000PCIDevice = kernel.pciService.returns.pciDevice_ptr;
                if (e1000PCIDevice != NULL)
                {
                    kernel.e1000 = (NetworkDriver *)kmalloc(&kernel.memoryAllocator, sizeof(NetworkDriver));
                    if (kernel.e1000 != NULL)
                    {
                        kernel.e1000->driver = e1000Init(e1000PCIDevice);
                        kernel.e1000->type = E1000_TYPE;
                        //kernel.e1000->send = e1000SendPacket;
                        //kernel.e1000->poll = e1000HandleReceive;
                        if (kernel.e1000->driver != NULL)
                            e1000_index++;
                    }
                }
            }
        }
    }
}

void e1000PrintMac(E1000 *p_e1000)
{
    printk_network("MAC Address From String: %s\n", p_e1000->mac_str);
}

void e1000StartLink(E1000 *p_e1000)
{
    uint32_t val;
    val = e1000ReadCommand(p_e1000, REG_CTRL);
    e1000WriteCommand(p_e1000, REG_CTRL, val | ECTRL_SLU | ECTRL_ASDE | ECTRL_FD | ECTRL_100M | ECTRL_FRCSPD);

    e1000WriteCommand(p_e1000, 0x0028, 0x0);
    e1000WriteCommand(p_e1000, 0x002C, 0x0);
    e1000WriteCommand(p_e1000, 0x0030, 0x0);
    e1000WriteCommand(p_e1000, 0x0170, 0x0);

    uint32_t status_reg = e1000ReadCommand(p_e1000, REG_STATUS);
}

void e1000PrintStatus(uint32_t status_reg)
{
    printk_network("e1000 NIC Status:\n");
    uint8_t extract = status_reg & 0b00000000000000000000000000000001;
    if (extract == 1)
        printk_network("Full Duplex\n");
    else
        printk_network("Half Duplex\n");
    extract = ((uint32_t)(status_reg) >> 1) & 0b00000000000000000000000000000001;
    if (extract == 1)
        printk_network("Link Up\n");
    else
        printk_network("Link Down\n");

    extract = ((uint32_t)(status_reg) >> 3) & 0b00000000000000000000000000000001;
    if (extract == 1)
        printk_network("TXOFF\n");
    else
        printk_network("TXON\n");

    extract = ((uint32_t)(status_reg) >> 5) & 0b00000000000000000000000000000011;
    if (extract == 0)
        printk_network("10Mb/s\n");
    else if (extract == 1)
        printk_network("100Mb/s\n");
    else if (extract == 2)
        printk_network("1000Mb/s\n");
    else
        printk_network("1000Mb/s\n");

    extract = ((uint32_t)(status_reg) >> 7) & 0b00000000000000000000000000000011;
    if (extract == 0)
        printk_network("AutoDetected: 10Mb/s\n");
    else if (extract == 1)
        printk_network("AutoDetected: 100Mb/s\n");
    else if (extract == 2)
        printk_network("AutoDetected: 1000Mb/s\n");
    else
        printk_network("AutoDetected: 1000Mb/s\n");
}

void e1000RXInit(E1000 *p_e1000)
{
    uint8_t *ptr;
    struct e1000_rx_desc *descs;

    ptr = (uint8_t *)(kvalloc(&kernel.memoryAllocator, sizeof(struct e1000_rx_desc) * E1000_NUM_RX_DESC + 16));
    descs = (struct e1000_rx_desc *)ptr;
    for (int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        p_e1000->rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i * 16);
        p_e1000->rx_descs[i]->addr = (uint64_t)(uint8_t *)(kvalloc(&kernel.memoryAllocator, RX_PACKET_SIZE));
        p_e1000->rx_vaddr[i] = p_e1000->rx_descs[i]->addr;
        p_e1000->rx_descs[i]->addr = v2p2m(p_e1000->rx_descs[i]->addr);
        p_e1000->rx_descs[i]->status = 0;
    }
    ptr = (uint8_t *)v2p2m((uint64_t)ptr);

    e1000WriteCommand(p_e1000, REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32));
    e1000WriteCommand(p_e1000, REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

    e1000WriteCommand(p_e1000, REG_RXDESCLO, (uint64_t)ptr);
    e1000WriteCommand(p_e1000, REG_RXDESCHI, 0);

    e1000WriteCommand(p_e1000, REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);

    e1000WriteCommand(p_e1000, REG_RXDESCHEAD, 0);
    e1000WriteCommand(p_e1000, REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
    p_e1000->rx_cur = 0;
    e1000WriteCommand(p_e1000, REG_RCTRL, 0x601801A);
}

void e1000TXInit(E1000 *p_e1000)
{
    uint8_t *ptr;
    struct e1000_tx_desc *descs;
    ptr = (uint8_t *)(kvalloc(&kernel.memoryAllocator, sizeof(struct e1000_tx_desc) * E1000_NUM_TX_DESC + 16));

    descs = (struct e1000_tx_desc *)ptr;
    for (int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        p_e1000->tx_descs[i] = (struct e1000_tx_desc *)((uint8_t *)descs + i * 16);
        p_e1000->tx_descs[i]->addr = 0;
        p_e1000->tx_descs[i]->cmd = 0;
        p_e1000->tx_descs[i]->status = TSTA_DD;
    }
    ptr = (uint8_t *)v2p2m((uint64_t)ptr);

    e1000WriteCommand(p_e1000, REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32));
    e1000WriteCommand(p_e1000, REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

    //now setup total length of descriptors
    e1000WriteCommand(p_e1000, REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);

    //setup numbers
    e1000WriteCommand(p_e1000, REG_TXDESCHEAD, 0);
    e1000WriteCommand(p_e1000, REG_TXDESCTAIL, 0);
    p_e1000->tx_cur = 0;
    e1000WriteCommand(p_e1000, REG_TCTRL, 0x3183F0FA);
    e1000WriteCommand(p_e1000, REG_TIPG, 0x00602006);
}

void e1000InterruptHandler(InterruptContext *p_interruptContext)
{
    //    console_addStringToCurrentTerminal(&kernel.console,"E1000 Fired on \n",COLOR_RED,COLOR_LIGHT_BROWN);
    //    printk_network (">>>>>>>> E1000 Fired on: %d \n",getCurrentCoreId());
    //for (uint8_t i = 0; kernel.e1000 != NULL; i++)
    //{

        if (kernel.e1000->type == E1000_TYPE &&
            ((E1000 *)kernel.e1000->driver)->int_line == p_interruptContext->interrupt_number)
        {
            uint32_t status = e1000ReadCommand(kernel.e1000->driver, REG_ICR);
            if (status & 0x04)
            {
                printk_network("-- >> start link\n");
                e1000StartLink(kernel.e1000);
            }
            else if (status & 0x10)
            {
                printk_network("Good threshhold\n");
                //e1000HandleReceive(kernel.e1000);
            }
            else if (status & 0x80)
            {
                                printk_network("Good threshhold 2\n");

                //e1000HandleReceive(kernel.e1000);
            }
        }
    //}

    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    int core_id = kernel.apicManager.returns.core_id;

    sendAPICEOI(&kernel.apicManager.apics[core_id]);
}

void e1000EnableInterrupt(E1000 * p_e1000)
{
    e1000WriteCommand(p_e1000,REG_IMASK ,0x1F6DC);
    e1000WriteCommand(p_e1000,REG_IMASK ,0xff & ~4);
    e1000ReadCommand(p_e1000,REG_ICR);
    
}

E1000 *e1000Init(PCIDevice *p_pciConfigHeader)
{
    E1000 *p_e1000 = (E1000 *)kmalloc(&kernel.memoryAllocator, sizeof(E1000));
    memset(p_e1000->mac_str, 0, 18);
    spinlock_init(&p_e1000->spinlock);
    p_e1000->bar_type = getPCIBarType(p_pciConfigHeader, 0);
    p_e1000->io_base = getPCIBar(p_pciConfigHeader, PCI_BAR_IO) & ~1;
    p_e1000->mem_base = getPCIBar(p_pciConfigHeader, PCI_BAR_MEM) & ~3;
    kernel.physicalMemoryManager.params.p_physical_address = p_e1000->mem_base;
    dispatch_kernel(&kernel.service_transporter, physical_memory, isaddress);
    if (p_e1000->bar_type == 0 && !kernel.physicalMemoryManager.returns.isAddress)
    {
        kernel.physicalMemoryManager.params.p_physical_address = p_e1000->mem_base;
        kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE * 20;
        dispatch_kernel(&kernel.service_transporter, physical_memory, AddPhysicalMemoryEntry);
        refreshReserved();
    }

    if (enablePCIBusMastering(p_pciConfigHeader))
        printk_network("NIC enablePCIBusMastering: succeeded\n");
    else
        printk_network("NIC: enablePCIBusMastering: failed\n");

    kernel.physicalMemoryManager.params.p_physical_address = p_e1000->mem_base;
    dispatch_kernel(&kernel.service_transporter, physical_memory, GetVirtualAddress);
    p_e1000->mem_base = kernel.physicalMemoryManager.returns.virtualAddress;

    e1000DetectEEProm(p_e1000);
    e1000ReadMACAddress(p_e1000);

    //uint32_t status_reg = e1000ReadCommand(p_e1000, REG_STATUS);
    //e1000PrintStatus(status_reg);
    printk_network("Starting Interface \n");
    e1000StartLink(p_e1000);

    e1000WriteCommand(p_e1000, 0x000C4, 1000000000 / (3 * 256));

    for (int i = 0; i < 0x80; i++)
    {
        e1000WriteCommand(p_e1000, 0x5200 + i * 4, 0);
    }

    printk_network("Registering Interrupt\n");
    kernel.interruptManager.params.p_interruptNumber = p_pciConfigHeader->int_line + IRQ0;
    kernel.interruptManager.params.p_interruptHandler = e1000InterruptHandler;
    dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);
    e1000RXInit(p_e1000);
    e1000TXInit(p_e1000);
    p_e1000->int_line = p_pciConfigHeader->int_line + IRQ0;
    bool wire_flag = false;
    /*
    for (uint8_t i = 0; i < kernel.apicManager.apics_count; i++)
    {
        for (uint8_t j = 0; j < 5; j++)
        {
            //if (kernel.apicManager.apics[i].role[j] == ROLE_NET)
            //{*/
    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);

    int core_id = kernel.apicManager.returns.core_id;

    if (core_id == 0)
    {
        printk_network("Wiring Network IRQ: %d to Core: %d\n", p_pciConfigHeader->int_line, 0);
        mapAPICIRQ(&kernel.apicManager.apics[0],
                   p_pciConfigHeader->int_line,
                   p_pciConfigHeader->int_line + IRQ0);
    }
    //wire_flag = true;
    /*
            break;
            // }
        }
    }
    */
    /*
    if (!wire_flag)
    {
        printk_network("Wiring Network IRQ: %d to Core: 0\n", p_pciConfigHeader->int_line);
        mapAPICIRQ(&kernel.apicManager.apics[0],
                   p_pciConfigHeader->int_line,
                   p_pciConfigHeader->int_line + IRQ0);
    }*/
    printk_network("Setting up RX/TX buffers\n");
    e1000EnableInterrupt(p_e1000);
    uint32_t status_reg = e1000ReadCommand(p_e1000, REG_STATUS);
    e1000PrintStatus(status_reg);

    return p_e1000;
}