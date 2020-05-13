#include "E1000.h"
#include "Kernel.h"

static uint16_t e1000_models[E1000_MODELS_COUNT] = {E1000_DEV,
                                                    E1000_DEV1,
                                                    E1000_DEV2,
                                                    E1000_DEV3,
                                                    E1000_DEV4};
extern Kernel kernel;

/*
 * Function:  e1000WriteCommand 
 * --------------------
 *  Writes a 32 bit value to a register
 *
 *  p_address: Register Offset
 *  p_value: 32 bit value to be written
 *  returns: void
 *
 */
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

/*
 * Function:  e1000ReadCommand 
 * --------------------
 *  reads a 32 bit value from a register
 *
 *  p_address: Register Offset
 *  
 *  returns: value read
 *
 */
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

/*
 * Function:  e1000DetectEEProm 
 * --------------------
 *  Detects whether E1000 can read
 *  from EEProm or not. It does so
 *  by writing to the EEPROM register
 *  and reading from the register for
 *  until a certain timeout. Bool in
 *  the e1000 data structure is adjusted
 *  accordingly
 *  p_address: Register Offset
 *  
 *  returns: void
 *
 */
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
}

/*
 * Function:  e1000EEpromRead 
 * --------------------
 *  Reads 2 bytes from the
 *  EEProm register
 *  addr: 
 *  
 *  returns: data read
 *
 */
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

/*
 * Function:  e1000ReadMACAddress 
 * --------------------
 *  Reads the MAC address from the
 *  EEPROM register location or from
 *  0x5400 if EEPROM does not exist
 *
 *  returns: data read
 *
 */
void e1000ReadMACAddress(E1000 *p_e1000)
{
    if (p_e1000->eerprom_exists)
    {
        printk_network("EEPROM Exists\n");
        uint32_t temp;
        temp = e1000EEpromRead(p_e1000, 0);

        p_e1000->mac[0] = temp & 0xff;
        p_e1000->mac[1] = temp >> 8;
        temp = e1000EEpromRead(p_e1000, 1);

        p_e1000->mac[2] = temp & 0xff;
        p_e1000->mac[3] = temp >> 8;
        temp = e1000EEpromRead(p_e1000, 2);

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
    printk_network("MAC Address: %y:%y:%y:%y:%y:%y\n",
                   p_e1000->mac[0],
                   p_e1000->mac[1],
                   p_e1000->mac[2],
                   p_e1000->mac[3],
                   p_e1000->mac[4],
                   p_e1000->mac[5]);
    return;
}

/*
 * Function:  e1000SendPacket 
 * --------------------
 *  Sends a packet after adjusting
 *  the descriptors
 *  p_data: ethernet packet to be sent
 *  p_len: payload size
 *  returns: 
 *
 */
int e1000SendPacket(NetworkDriver *p_networkDriver, const void *p_data, uint16_t p_len)
{
    // we need to spinlock in case of concurrency
    E1000 *p_e1000 = (E1000 *)p_networkDriver->driver;
    spinlock_lock(&p_e1000->spinlock);
    p_e1000->tx_descs[p_e1000->tx_cur]->addr = v2p2m((uint64_t)p_data);
    p_e1000->tx_descs[p_e1000->tx_cur]->length = p_len;
    p_e1000->tx_descs[p_e1000->tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
    p_e1000->tx_descs[p_e1000->tx_cur]->status = 0;
    uint16_t current_head = e1000ReadCommand(p_e1000, REG_TXDESCHEAD);
    uint16_t old_cur = p_e1000->tx_cur;

    if (old_cur + 1 == current_head)
    {
        printk("E1000 buffer overflow\n");
    }
    p_e1000->tx_cur = (p_e1000->tx_cur + 1) % E1000_NUM_TX_DESC;
    e1000WriteCommand(p_e1000, REG_TXDESCTAIL, p_e1000->tx_cur);
    if (p_e1000->tx_descs[old_cur]->status & 0xff)
    {
        spinlock_unlock(&p_e1000->spinlock);
        return 0;
    }
    while (!(p_e1000->tx_descs[old_cur]->status & 0xff));
    spinlock_unlock(&p_e1000->spinlock);
    return 0;
}

/*
 * Function:  e1000Scan 
 * --------------------
 *  Goes through the defined E1000
 *  models given above, and checks
 *  the pci devices initially discovered
 *  and initiallizes the driver if found
 *  
 *  returns: void
 *
 */
void e1000Scan()
{
    uint16_t e1000_index = 0;
    for (uint8_t i = 0; i < E1000_MODELS_COUNT; i++)
    {
        kernel.pciService.params.p_vendor = INTEL_VEND;
        kernel.pciService.params.p_device = e1000_models[i];
        DispatchKernel(&kernel.service_transporter, pci_t, get_pci_device_count);
        uint16_t count = kernel.pciService.returns.count;
        if (count > 0)
        {
            for (uint16_t j = 0; j < count; j++)
            {
                kernel.pciService.params.p_vendor = INTEL_VEND;
                kernel.pciService.params.p_device = e1000_models[i];
                kernel.pciService.params.p_index = j;
                DispatchKernel(&kernel.service_transporter, pci_t, get_pci_device_index);
                PCIDevice *e1000PCIDevice = kernel.pciService.returns.pciDevice_ptr;
                if (e1000PCIDevice != NULL)
                {
                    kernel.e1000 = (NetworkDriver *)kmalloc(&kernel.memoryAllocator, sizeof(NetworkDriver));
                    if (kernel.e1000 != NULL)
                    {
                        kernel.e1000->driver = e1000Init(e1000PCIDevice);
                        kernel.e1000->type = E1000_TYPE;
                        kernel.e1000->send = e1000SendPacket;
                        kernel.e1000->poll = e1000HandleReceive;
                        if (kernel.e1000->driver != NULL)
                            e1000_index++;
                    }
                }
            }
        }
    }
}

/*
 * Function:  e1000StartLink 
 * --------------------
 *  Does the necessary configurations
 *  per the references intel manual pages
 *
 *  returns: void
 *
 */
void e1000StartLink(E1000 *p_e1000)
{
    uint32_t val;
    val = e1000ReadCommand(p_e1000, REG_CTRL);
    e1000WriteCommand(p_e1000, REG_CTRL, val | ECTRL_SLU | ECTRL_ASDE | ECTRL_FD | ECTRL_100M | ECTRL_FRCSPD);

    // 0x28 is Flow Control Address High
    // 0x2c is Flow Control Address Low
    // 0x30 is Flow Control Type
    // 0x170 is Flow Control Transmit Timer Value

    // Page 284 in the documentation
    e1000WriteCommand(p_e1000, 0x0030, 0x8808);
    // Page 283 in the documentation
    e1000WriteCommand(p_e1000, 0x0028, 0x00000100);
    e1000WriteCommand(p_e1000, 0x002C, 0x00C28001);
    // Page 285 in the documentation
    e1000WriteCommand(p_e1000, 0x0170, 0x0);
}

/*
 * Function:  e1000PrintStatus 
 * --------------------
 *  Prints the status of the driver
 *  
 * status_reg: read value from the
 *             status register
 *
 *  returns: void
 *
 */
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

/*
 * Function:  e1000RXInit 
 * --------------------
 *  Initializes the receive descriptors
 *
 *  p_e1000: pointer to the e1000 driver
 *
 *  returns: void
 *
 */
void e1000RXInit(E1000 *p_e1000)
{
    uint8_t *ptr;
    struct e1000_rx_desc *descs;

    /* disable receives while setting up the descriptors */
    uint32_t rctl = e1000ReadCommand(p_e1000, REG_RCTRL);
    e1000WriteCommand(p_e1000, REG_RCTRL, rctl & ~RCTL_EN);

    /* set the Receive Delay Timer Register */
    // according to linux, anything but zero will make the hardware hang
    e1000WriteCommand(p_e1000, REG_RCTRL, 0);

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

    e1000WriteCommand(p_e1000, REG_RXDESCLO, (uint32_t)((uint64_t)ptr & 0x00000000ffffffffULL));
    e1000WriteCommand(p_e1000, REG_RXDESCHI, (uint32_t)((uint64_t)ptr >> 32));

    e1000WriteCommand(p_e1000, REG_RXDESCLEN, (E1000_NUM_RX_DESC * 16));

    e1000WriteCommand(p_e1000, REG_RXDESCHEAD, 0);
    e1000WriteCommand(p_e1000, REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
    p_e1000->rx_cur = 0;

    /* Enable Receives */
    e1000WriteCommand(p_e1000, REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
    //e1000WriteCommand(p_e1000, REG_RCTRL, 0x601801A);
}

/*
 * Function:  e1000TXInit 
 * --------------------
 *  Initializes the transmit descriptors
 *
 *  p_e1000: pointer to the e1000 driver
 *
 *  returns: void
 *
 */
void e1000TXInit(E1000 *p_e1000)
{
    uint8_t *ptr;
    struct e1000_tx_desc *descs;
    uint32_t tctl;
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
    e1000WriteCommand(p_e1000, REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0x00000000ffffffffULL));

    //setup numbers
    e1000WriteCommand(p_e1000, REG_TXDESCHEAD, 0);
    e1000WriteCommand(p_e1000, REG_TXDESCTAIL, 0);
    e1000WriteCommand(p_e1000, REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);

    p_e1000->tx_cur = 0;
    /* Program the Transmit Control Register */

    // tctl = e1000ReadCommand(p_e1000, REG_TCTRL);
    // tctl &= ~TCTL_CT;
    // tctl |= TCTL_PSP | E1000_TCTL_RTLC |
    //         (TCTL_COLLISION_THRESHOLD << TCTL_CT_SHIFT);

    // e1000WriteCommand(p_e1000, REG_TCTRL, tctl);
    e1000WriteCommand(p_e1000, REG_TCTRL, 0b0110000000000111111000011111010);
    e1000WriteCommand(p_e1000, REG_TIPG, 0x0060200A);
}

/*
 * Function:  e1000InterruptHandler 
 * --------------------
 *  Handles E1000 interrupts
 *
 *  p_interruptContext: pointer to the interrupt context
 *
 *  returns: void
 *
 */
void e1000InterruptHandler(InterruptContext *p_interruptContext)
{
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
            //printk("Good threshhold\n");
            e1000HandleReceive(kernel.e1000);
        }
        else if (status & 0x80)
        {
           // printk("Good threshhold 2\n");

            e1000HandleReceive(kernel.e1000);
        }
    }

    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
    int core_id = kernel.apicManager.returns.core_id;

    E1000 *p_e1000 = (E1000 *)kernel.e1000->driver;

    // clear interrupts
    e1000WriteCommand(p_e1000, REG_ICR, e1000ReadCommand(p_e1000, REG_ICR) | 0xFFFFFFFF);

    sendAPICEOI(&kernel.apicManager.apics[core_id]);
}

/*
 * Function:  e1000EnableInterrupt 
 * --------------------
 *  Enables interrupts on the given driver
 *
 *  p_e1000: pointer to the e1000 driver
 *
 *  returns: void
 *
 */
void e1000EnableInterrupt(E1000 *p_e1000)
{
    // clearing the Interrupt mask
    e1000WriteCommand(p_e1000, REG_IMASK, 0x0000);
    e1000WriteCommand(p_e1000, REG_IMASK, 0x009D);
    // clearing any pending interrupt events
    e1000ReadCommand(p_e1000, REG_ICR);
}

/*
 * Function:  e1000EnableInterrupt 
 * --------------------
 *  Handles receiving of the packet by
 *  processing it and sending it to
 *  processEthernetPacket
 *  
 *  p_networkDriver: pointer to the generic driver
 *
 *  returns: void
 *
 */
void e1000HandleReceive(NetworkDriver *p_networkDriver)
{
    // we need to spinlock if latched to more than one APIC
    E1000 *p_e1000 = (E1000 *)p_networkDriver->driver;

    uint16_t current_head = e1000ReadCommand(p_e1000, REG_RXDESCHEAD);
    if (current_head == p_e1000->rx_cur)
        return;
    int counter = 0;
    //printk("handling receive! %d\n", counter++);
    while ((p_e1000->rx_descs[p_e1000->rx_cur]->status & 0x1))
    {
        kernel.pit.packets_received++;
        uint8_t *buf = (uint8_t *)p_e1000->rx_descs[p_e1000->rx_cur]->addr;
        uint16_t len = p_e1000->rx_descs[p_e1000->rx_cur]->length;
        NetworkPacket networkPacket;
        networkPacket.ethernetPacket = (EthernetPacket *)buf;
        networkPacket.packet_size = len;
        current_head = e1000ReadCommand(p_e1000, REG_RXDESCHEAD);
        //printk("current head: %d\n", current_head);

        p_e1000->rx_descs[p_e1000->rx_cur]->status = 0;
        uint16_t old_cur = p_e1000->rx_cur;
        p_e1000->rx_cur = (p_e1000->rx_cur + 1) % E1000_NUM_RX_DESC;
        processEthernetPacket(p_networkDriver, (void *)&networkPacket);

        e1000WriteCommand(p_e1000, REG_RXDESCTAIL, old_cur);
    }
}

/*
 * Function:  e1000Init 
 * --------------------
 *  Handles receiving of the packet by
 *  processing it and sending it to
 *  processEthernetPacket
 *  
 *  p_pciConfigHeader: pointer to the PCIDevice
 *
 *  returns: pointer to the initialized E1000 driver
 *
 */
E1000 *e1000Init(PCIDevice *p_pciConfigHeader)
{
    E1000 *p_e1000 = (E1000 *)kmalloc(&kernel.memoryAllocator, sizeof(E1000));
    memset(p_e1000->mac_str, 0, 18);
    spinlock_init(&p_e1000->spinlock);
    p_e1000->bar_type = GetPCIBarType(p_pciConfigHeader, 0);
    p_e1000->io_base = GetPCIBar(p_pciConfigHeader, PCI_BAR_IO) & ~1;
    p_e1000->mem_base = GetPCIBar(p_pciConfigHeader, PCI_BAR_MEM) & ~3;
    kernel.physicalMemoryManager.params.p_physical_address = p_e1000->mem_base;
    DispatchKernel(&kernel.service_transporter, physical_memory_t, is_address);
    if (p_e1000->bar_type == 0 && !kernel.physicalMemoryManager.returns.is_address)
    {
        kernel.physicalMemoryManager.params.p_physical_address = p_e1000->mem_base;
        kernel.physicalMemoryManager.params.p_size = FOUR_KiB_MEMORY_PAGE_SIZE * 20;
        DispatchKernel(&kernel.service_transporter, physical_memory_t, add_physical_memory_entry);
        RefreshReserved();
    }

    if (EnablePCIBusMastering(p_pciConfigHeader))
        printk_network("Enabling PCIBusMastering: succeeded\n");
    else
        printk_network("Enabling PCIBusMastering: failed\n");

    kernel.physicalMemoryManager.params.p_physical_address = p_e1000->mem_base;
    DispatchKernel(&kernel.service_transporter, physical_memory_t, get_virtual_address);
    p_e1000->mem_base = kernel.physicalMemoryManager.returns.virtualAddress;

    e1000DetectEEProm(p_e1000);
    e1000ReadMACAddress(p_e1000);

    printk_network("Starting Interface\n");
    e1000StartLink(p_e1000);

    // i believe this is incorrect to have as it slows down the driver!
    //e1000WriteCommand(p_e1000, 0x000C4, 1000000000 / (3 * 256));

    for (int i = 0; i < 0x80; i++)
    {
        e1000WriteCommand(p_e1000, 0x5200 + i * 4, 0);
    }
  
    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);

    int core_id = kernel.apicManager.returns.core_id;
    printk_network("Registering Interrupt\n");
    kernel.interruptManager.params.p_interruptNumber = p_pciConfigHeader->int_line + IRQ0;
    kernel.interruptManager.params.p_interruptHandler = e1000InterruptHandler;
    DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);
    printk_network("Wiring Network IRQ: %d to Core: %d\n", p_pciConfigHeader->int_line, core_id);
    
    printk_network("Setting up RX/TX buffers\n");
    e1000RXInit(p_e1000);
    e1000TXInit(p_e1000);
    p_e1000->int_line = p_pciConfigHeader->int_line + IRQ0;

    // should allow here support for having multiple network card!
    mapAPICIRQ(&kernel.apicManager.apics[core_id],
               p_pciConfigHeader->int_line,
               p_pciConfigHeader->int_line + IRQ0);
    e1000EnableInterrupt(p_e1000);
    uint32_t status_reg = e1000ReadCommand(p_e1000, REG_STATUS);
    e1000PrintStatus(status_reg);

    return p_e1000;
}
