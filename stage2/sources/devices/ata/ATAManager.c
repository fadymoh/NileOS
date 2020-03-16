#include "ATAManager.h"
#include "Kernel.h"
#include "APIC.h"
extern APIC apic;
extern Kernel kernel;
extern char *buffer;
PCIDevice *getDiskPCIDevice()
{
    for (int i = 0; i < kernel.pciService.total_pci_devices; i++)
    {
        if (kernel.pciService.pciDevices[i].pci_major == (uint8_t)0x1 &&
            kernel.pciService.pciDevices[i].pci_minor == (uint8_t)0x1)
        {
            return &kernel.pciService.pciDevices[i];
        }
    }
    return NULL;
}

char *getDeviceTypeName(uint16_t p_device_id)
{
    for (uint8_t i = 0; i < DEVICE_TYPE_COUNT; i++)
        if (kernel.ataManager.device_ids[i] == p_device_id)
            return kernel.ataManager.device_names[i];
    return kernel.ataManager.device_names[DEVICE_TYPE_COUNT - 1];
}

void selectDrive(uint16_t p_io_port, uint8_t p_drv_selector)
{
    outportb(p_io_port + ATA_REG_HDDEVSEL, p_drv_selector);
}

bool identifyDisk(uint16_t p_io_port, uint8_t p_drv_selector)
{
    if (p_io_port == ATA_PRIMARY_IO)
        outportb(ATA_PRIMARY_DCR_AS, 0b00000000);
    else
        outportb(ATA_SECONDARY_DCR_AS, 0b00000000);
    selectDrive(p_io_port, p_drv_selector);

    outportb(p_io_port + ATA_REG_SECCOUNT0, 0);
    outportb(p_io_port + ATA_REG_LBA0, 0);
    outportb(p_io_port + ATA_REG_LBA1, 0);
    outportb(p_io_port + ATA_REG_LBA2, 0);
    outportb(p_io_port + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

    bool ready = false;
    uint8_t status = inportb(p_io_port + ATA_REG_STATUS);
    if (status == 0 || ((status & 0b00000001) == 0x1))
        return ready;

    while (1)
    {
        status = inportb(p_io_port + ATA_REG_STATUS);
        if (status & ATA_SR_DRQ)
        {
            ready = true;
            break;
        }
        else if (status & ATA_SR_ERR)
            break;
    }
    return ready;
}

void identifyPCI()
{
    PCIDevice *pciConfigHeader = getDiskPCIDevice();
    if (pciConfigHeader != NULL)
    {
        printk("PCI ATA Detected:\n");
        uint8_t programming_interface = pciConfigHeader->pci_interface;
        if (isBitSet(programming_interface, 7))
        {
            uint16_t pci_device_id = pciConfigHeader->dev_id;
            printk("DMA is Enabled\n");
            printk("Device TYPE: %s\n", getDeviceTypeName(pci_device_id));
            if (pci_device_id == 0x1230 || pci_device_id == 0x7010 || pci_device_id == 0x7111)
            {
                printk("Enabling Bus Mastering: %x\n", pci_device_id);
                enablePCIBusMastering(pciConfigHeader);
                pciConfigWriteCommand(pciConfigHeader, 0x40, 0xA344);
                pciConfigWriteCommand(pciConfigHeader, 0x42, 0xA344);
            }
            else
                printk("Did not enable Bus Mastering: %d\n", pci_device_id);

            if ((pciConfigHeader->bars[4] & 0x1) == 0x1)
            {
                kernel.ataManager.dma_port_address = pciConfigHeader->bars[4];
                printk("DMA Bus Mastering is I/O Port Base: %x\n", kernel.ataManager.dma_port_address);

                clearStatus(kernel.ataManager.dma_port_address, 0);
                printk("DMA Bus Mastering is I/O Port Base: %x\n", kernel.ataManager.dma_port_address);
            }
            else
            {
                kernel.ataManager.dma_mmio_address = pciConfigHeader->bars[4];
                printk("DMA Bus Mastering is MMIO Base: %x\n", kernel.ataManager.dma_mmio_address);
            }
        }
        else
            printk("DMA is not Enabled\n");
    }
    else
        printk("No PCI ATA Detected\n");
}

void initATAManager()
{
    kernel.ataManager.dma_port_address = 0x0;
    kernel.ataManager.dma_mmio_address = 0x0;

    kernel.ataManager.prdt1 = (uint8_t *)kvalloc(&kernel.memoryAllocator, 0x1000);
    kernel.ataManager.prdt2 = (uint8_t *)kvalloc(&kernel.memoryAllocator, 0x1000);

    kernel.ataManager.dma_phy_address1 = (uint8_t *)kvalloc(&kernel.memoryAllocator, 0x2000000);
    kernel.ataManager.dma_phy_address2 = (uint8_t *)kvalloc(&kernel.memoryAllocator, 0x2000000);
    // printk("dma physical address 1        %x\n", kernel.ataManager.dma_phy_address1);

    memset(kernel.ataManager.dma_phy_address1, 0, 0x2000000);
    memset(kernel.ataManager.dma_phy_address2, 0, 0x2000000);

    for (uint8_t i = 0; i < DEVICE_TYPE_COUNT; i++)
    {
        memset(kernel.ataManager.device_names[i], 0, 20);
        kernel.ataManager.device_ids[i] = 0;
    }
    kernel.ataManager.device_ids[0] = 0x1230;
    kernel.ataManager.device_ids[1] = 0x2411;
    kernel.ataManager.device_ids[2] = 0x2421;
    kernel.ataManager.device_ids[3] = 0x244B;
    kernel.ataManager.device_ids[4] = 0x24DB;
    kernel.ataManager.device_ids[5] = 0x266F;
    kernel.ataManager.device_ids[6] = 0x248B;
    kernel.ataManager.device_ids[7] = 0x24CB;
    kernel.ataManager.device_ids[8] = 0x2922;
    kernel.ataManager.device_ids[9] = 0x7010;
    kernel.ataManager.device_ids[10] = 0x7111;
    kernel.ataManager.device_ids[11] = 0;
    memcpy(kernel.ataManager.device_names[0], "CNTRL_TYPE_PIIX", strlen("CNTRL_TYPE_PIIX"));
    memcpy(kernel.ataManager.device_names[1], "CNTRL_TYPE_ICH", strlen("CNTRL_TYPE_ICH"));
    memcpy(kernel.ataManager.device_names[2], "CNTRL_TYPE_ICH0", strlen("CNTRL_TYPE_ICH0"));
    memcpy(kernel.ataManager.device_names[3], "CNTRL_TYPE_ICH2", strlen("CNTRL_TYPE_ICH2"));
    memcpy(kernel.ataManager.device_names[4], "CNTRL_TYPE_ICH5", strlen("CNTRL_TYPE_ICH5"));
    memcpy(kernel.ataManager.device_names[5], "CNTRL_TYPE_ICH6", strlen("CNTRL_TYPE_ICH6"));
    memcpy(kernel.ataManager.device_names[6], "CNTRL_TYPE_ICH3", strlen("CNTRL_TYPE_ICH3"));
    memcpy(kernel.ataManager.device_names[7], "CNTRL_TYPE_ICH4", strlen("CNTRL_TYPE_ICH4"));
    memcpy(kernel.ataManager.device_names[8], "CNTRL_TYPE_ICH9", strlen("CNTRL_TYPE_ICH9"));
    memcpy(kernel.ataManager.device_names[9], "CNTRL_TYPE_PIIX3", strlen("CNTRL_TYPE_PIIX3"));
    memcpy(kernel.ataManager.device_names[10], "CNTRL_TYPE_PIIX4", strlen("CNTRL_TYPE_PIIX4"));
    memcpy(kernel.ataManager.device_names[11], "Not Found", strlen("Not Found"));
    spinlock_init(&kernel.dmaBuffer.spinlock);
    kernel.dmaBuffer.current_select = 0;
}

uint8_t detectATADisks()
{

    identifyPCI();
    uint16_t io_array[] = {ATA_PRIMARY_IO, ATA_SECONDARY_IO};
    uint8_t selector_array[] = {ATA_MASTER_DRV_SELECTOR, ATA_SLAVE_DRV_SELECTOR};
    uint8_t slavebit_array[] = {0, 1};
    uint8_t *prdt_ptrs[] = {kernel.ataManager.prdt1, kernel.ataManager.prdt2};
    uint8_t *dma_phy_address_ptrs[] = {kernel.ataManager.dma_phy_address1, kernel.ataManager.dma_phy_address2};

    uint8_t si = 0;
    uint8_t ata_count = 0;
    for (uint8_t i = 0; i < 2; i++)
        for (uint8_t j = 0; j < 2; j++)
        {
            if (identifyDisk(io_array[i], selector_array[j]))
            {

                kernel.ataManager.ataDisks[si] = (ATADisk *)kmalloc(&kernel.memoryAllocator, sizeof(ATADisk));
                printk("Detected disk %d on channel %d\n", j, i);

                initATADisk(kernel.ataManager.ataDisks[si],
                            io_array[i],
                            selector_array[j],
                            slavebit_array[j],
                            (kernel.ataManager.dma_mmio_address != 0x0 || kernel.ataManager.dma_port_address != 0x0),
                            prdt_ptrs[i], dma_phy_address_ptrs[i],
                            kernel.ataManager.dma_port_address,
                            kernel.ataManager.dma_mmio_address, i * 0x08);
                ata_count++;
            }
            else
            {
                kernel.ataManager.ataDisks[si] = NULL;
                printk("Could not detect disk %d on channel %d\n", j, i);
            }
            si++;
        }
    return ata_count;
}

ATADisk *getATADisk(uint8_t p_index)
{
    if (p_index < ATA_COUNT)
        return kernel.ataManager.ataDisks[p_index];
    else
        return NULL;
}

void ata_400ns_delay(uint16_t p_io_port)
{
    for (int i = 0; i < 4; i++)
        inportb(p_io_port + ATA_REG_STATUS);
}

void ataHandleReadInterrupt(InterruptContext *p_interruptContext)
{
    if (!kernel.dmaBuffer.enabled)
        return;
    if (p_interruptContext->interrupt_number != IRQ14 &&
        p_interruptContext->interrupt_number != IRQ15)
        return;
  
    uint8_t bm_status = inportb(kernel.dmaBuffer.ataDisk->dma_port_address + BM0_STATUS + kernel.dmaBuffer.ataDisk->bm_offset);
    if (!(bm_status & 0x4))
    {
        inportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_STATUS);
        printk(">>>>> bm_status != 0x4 - > %x\n", bm_status);
        return;
    }
    inportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_STATUS);

    uint32_t read_dma_chunk = DMA_READ_CHUNK;
    if (kernel.dmaBuffer.ataDisk->lba48)
        read_dma_chunk = DMA_READ_CHUNK_EXT;
    uint16_t start_offset = 0;
    uint16_t end_offset = 0;
    if (kernel.dmaBuffer.current_read == 0)
        kernel.dmaBuffer.current_read = read_dma_chunk;

    if (kernel.dmaBuffer.total_read == 0)
        start_offset = kernel.dmaBuffer.start_offset;

    if (kernel.dmaBuffer.total_read + kernel.dmaBuffer.current_read >= kernel.dmaBuffer.target_read)
        end_offset = kernel.dmaBuffer.end_offset;

    fast_memcpy(kernel.dmaBuffer.buffer + (kernel.dmaBuffer.total_read * 0x200),
                kernel.dmaBuffer.ataDisk->dma_phy_address + start_offset,
                (kernel.dmaBuffer.current_read * 0x200) - end_offset);

    kernel.dmaBuffer.total_read_bytes += ((kernel.dmaBuffer.current_read * 0x200) - end_offset);
    kernel.dmaBuffer.total_read += kernel.dmaBuffer.current_read;
    if (kernel.dmaBuffer.total_read < kernel.dmaBuffer.target_read)
    {
        kernel.dmaBuffer.current_address += kernel.dmaBuffer.current_read;
        if (kernel.dmaBuffer.target_read - kernel.dmaBuffer.total_read > read_dma_chunk)
            kernel.dmaBuffer.current_read = read_dma_chunk;
        else
            kernel.dmaBuffer.current_read = kernel.dmaBuffer.target_read - kernel.dmaBuffer.total_read;
        for (uint64_t i = 0;; i++)
        {
            if (readDMADiskSectors(kernel.dmaBuffer.ataDisk, kernel.dmaBuffer.current_address, kernel.dmaBuffer.current_read) == READ_DMA_SUCCESS)
                break;
            if (i > 0 && i % 100 == 0)
                printk("READ: I am stuck here: %d\n", i);
        }
    }
    else
    {
        uint64_t end_time = getRTCTimeStamp32();
        printk("Finished reading: %d sectors in %d sec\n", kernel.dmaBuffer.total_read, end_time - kernel.dmaBuffer.start_time);
        sendFixedIPI(&kernel.apicManager.apics[kernel.dmaBuffer.core_id], ATA_IPI);
    }
    inportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_STATUS);
}

void ataHandleWriteInterrupt(InterruptContext *p_interruptContext)
{
    if (!kernel.dmaBuffer.enabled || kernel.dmaBuffer.write_done)
        return;
    if (p_interruptContext->interrupt_number != IRQ14 &&
        p_interruptContext->interrupt_number != IRQ15)
        return;
    uint8_t bm_status = inportb(kernel.dmaBuffer.ataDisk->dma_port_address + BM0_STATUS + kernel.dmaBuffer.ataDisk->bm_offset);
    if (!(bm_status & 0x4))
    {
        inportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_STATUS);
        printk(">>>>> bm_status != 0x4 - > %x\n", bm_status);
        return;
    }
    inportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_STATUS);

    uint32_t write_dma_chunk = DMA_WRITE_CHUNK;
    if (kernel.dmaBuffer.ataDisk->lba48)
        write_dma_chunk = DMA_WRITE_CHUNK_EXT;
    if (kernel.dmaBuffer.total_write < kernel.dmaBuffer.target_write)
    {
        kernel.dmaBuffer.total_write += kernel.dmaBuffer.current_write;
        kernel.dmaBuffer.current_address += kernel.dmaBuffer.current_write;
        if (kernel.dmaBuffer.target_write - kernel.dmaBuffer.total_write > write_dma_chunk)
            kernel.dmaBuffer.current_write = write_dma_chunk;
        else
            kernel.dmaBuffer.current_write = kernel.dmaBuffer.target_write - kernel.dmaBuffer.total_write;

        if (kernel.dmaBuffer.current_write == 0)
            kernel.dmaBuffer.current_write = write_dma_chunk;
        memset(kernel.dmaBuffer.ataDisk->dma_phy_address, 0, 0x2000000);
        fast_memcpy(kernel.dmaBuffer.ataDisk->dma_phy_address,
                    kernel.dmaBuffer.buffer + (kernel.dmaBuffer.total_write * 0x200),
                    kernel.dmaBuffer.current_write * 0x200);

        inportb(kernel.dmaBuffer.ataDisk->dma_port_address + BM0_STATUS + kernel.dmaBuffer.ataDisk->bm_offset);
        bm_status = inportb(kernel.dmaBuffer.ataDisk->dma_port_address + BM0_STATUS + kernel.dmaBuffer.ataDisk->bm_offset);
        outportb(kernel.dmaBuffer.ataDisk->dma_port_address + BM0_COMMAND + kernel.dmaBuffer.ataDisk->bm_offset, (0 << 0));
        outportb(kernel.dmaBuffer.ataDisk->dma_port_address + BM0_STATUS + kernel.dmaBuffer.ataDisk->bm_offset, bm_status);
        for (uint64_t i = 0;; i++)
        {
            if (writeDMADiskSectors(kernel.dmaBuffer.ataDisk, kernel.dmaBuffer.current_address, kernel.dmaBuffer.current_write) == WRITE_DMA_SUCCESS)
                break;
            if (i > 0 && i % 100 == 0)
                printk("WRITE: I am stuck here: %d\n", i);
        }
    }
    else
    {
        if (kernel.dmaBuffer.ataDisk->lba48)
            outportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_COMMAND, 0xEA);
        else
            outportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_COMMAND, 0xE7);
        uint64_t end_time = getRTCTimeStamp32();
        kernel.dmaBuffer.write_done = true;
        printk("Finished writing: %d sectors in %d sec\n", kernel.dmaBuffer.total_write, end_time - kernel.dmaBuffer.start_time);

        sendFixedIPI(&kernel.apicManager.apics[kernel.dmaBuffer.core_id], ATA_IPI);
    }
    inportb(kernel.dmaBuffer.ataDisk->io_port + ATA_REG_STATUS);
}
void ataHandleInterrupt(InterruptContext *p_interruptContext)
{
    if (!kernel.dmaBuffer.enabled)
    {
        int status = 0;
        outportb(ATA_PRIMARY_DCR_AS, 0b00000000);
        outportb(ATA_SECONDARY_DCR_AS, 0b00000000);
        outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, ATA_SLAVE_DRV_SELECTOR);
        outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, ATA_MASTER_DRV_SELECTOR);
        status = inportb(ATA_PRIMARY_IO + ATA_REG_STATUS);
        return;
    }
    if (kernel.dmaBuffer.read)
        ataHandleReadInterrupt(p_interruptContext);
    else
        ataHandleWriteInterrupt(p_interruptContext);
}
