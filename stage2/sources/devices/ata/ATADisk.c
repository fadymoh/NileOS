#include "ATADisk.h"
#include "Kernel.h"
extern Kernel kernel;

uint8_t writePIODiskSectors(ATADisk *p_ataDisk, uint64_t addr, uint8_t *p_buffer, uint16_t p_sector_count)
{
    uint16_t io_port = p_ataDisk->io_port;
    if (io_port == ATA_PRIMARY_IO)
        outportb(ATA_PRIMARY_DCR_AS, 0b00000010);
    else
        outportb(ATA_SECONDARY_DCR_AS, 0b00000010);

    if (p_ataDisk->lba48)
        outportb(io_port + ATA_REG_HDDEVSEL, 0xE0 | (p_ataDisk->slavebit << 4));
    else
        outportb(io_port + ATA_REG_HDDEVSEL, 0xE0 | (p_ataDisk->slavebit << 4) | ((addr >> 24) & 0x0F));

    outportb(io_port + ATA_REG_FEATURES, 0x00);
    outportb(io_port + ATA_REG_SECCOUNT1, (uint8_t)(p_sector_count >> 8));
    outportb(io_port + ATA_REG_LBA3, (uint8_t)(addr >> 24));
    outportb(io_port + ATA_REG_LBA4, (uint8_t)(addr >> 32));
    outportb(io_port + ATA_REG_LBA5, (uint8_t)(addr >> 40));
    outportb(io_port + ATA_REG_SECCOUNT0, (uint8_t)p_sector_count);
    outportb(io_port + ATA_REG_LBA0, (uint8_t)addr);
    outportb(io_port + ATA_REG_LBA1, (uint8_t)(addr >> 8));
    outportb(io_port + ATA_REG_LBA2, (uint8_t)(addr >> 16));
    if (p_ataDisk->lba48)
        outportb(io_port + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO_EXT);
    else
        outportb(io_port + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    if (inportb(io_port + ATA_REG_STATUS))
    {
        for (uint64_t i = 0;; i++)
        {
            uint8_t status = inportb(io_port + ATA_REG_STATUS);
            if (status & ATA_SR_DRQ)
                break;
            else if (status & ATA_SR_ERR)
            {
                printk("Write PIO failed: %x\n", status);
                return WRITE_PIO_FAIL;
            }
        }
    }
    printk_fs("Writing %d sectors\n", p_sector_count);
    for (uint64_t idx = 0; idx < 256 * p_sector_count; idx++)
    {
        uint16_t writeword = p_buffer[idx * 2] | (p_buffer[idx * 2 + 1] << 8);
        outportw(io_port, writeword);
        if (idx % 256 == 0 && idx > 0)
        {
            if (p_ataDisk->lba48)
                outportb(io_port + ATA_REG_COMMAND, 0xEA);
            else
                outportb(io_port + ATA_REG_COMMAND, 0xE7);
        }
    }
    if (p_ataDisk->lba48)
        outportb(io_port + ATA_REG_COMMAND, 0xEA);
    else
        outportb(io_port + ATA_REG_COMMAND, 0xE7);

    for (int idx = 0; idx < 10; idx++)
        ide_400ns_delay(p_ataDisk);
    return WRITE_PIO_SUCCESS;
}

uint8_t readPIODiskSectors(ATADisk *p_ataDisk, uint64_t addr, uint8_t *p_buffer, uint16_t p_sector_count)
{
    uint16_t io_port = p_ataDisk->io_port;
    if (io_port == ATA_PRIMARY_IO)
        outportb(ATA_PRIMARY_DCR_AS, 0b00000010);
    else
        outportb(ATA_SECONDARY_DCR_AS, 0b00000010);
    
    if (p_ataDisk->lba48)
        outportb(io_port + ATA_REG_HDDEVSEL, 0xE0 | (p_ataDisk->slavebit << 4));
    else
        outportb(io_port + ATA_REG_HDDEVSEL, 0xE0 | (p_ataDisk->slavebit << 4) | ((addr >> 24) & 0x0F));

    outportb(io_port + ATA_REG_FEATURES, 0x00);
    outportb(io_port + ATA_REG_SECCOUNT1, (uint8_t)(p_sector_count >> 8));
    outportb(io_port + ATA_REG_LBA3, (uint8_t)(addr >> 24));
    outportb(io_port + ATA_REG_LBA4, (uint8_t)(addr >> 32));
    outportb(io_port + ATA_REG_LBA5, (uint8_t)(addr >> 40));
    outportb(io_port + ATA_REG_SECCOUNT0, (uint8_t)p_sector_count);
    outportb(io_port + ATA_REG_LBA0, (uint8_t)addr);
    outportb(io_port + ATA_REG_LBA1, (uint8_t)(addr >> 8));
    outportb(io_port + ATA_REG_LBA2, (uint8_t)(addr >> 16));
    if (p_ataDisk->lba48)
        outportb(io_port + ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);
    else
        outportb(io_port + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    int idx = 0;
    uint16_t sector_count = p_sector_count;
    if (sector_count == 0 ) sector_count = 256;
    for ( int x = 0 ; x < sector_count ; x++)
    {
        if (inportb(io_port + ATA_REG_STATUS))
        {
            for ( int i = 0 ; ; i++)
            {
                uint8_t status = inportb(io_port+ATA_REG_STATUS);
                if ( status & ATA_SR_DRQ ) break;
                else if(status & ATA_SR_ERR) return READ_PIO_FAIL;
                ide_400ns_delay(p_ataDisk);
            }
        }        
        for (; idx < 256*(x+1); idx++)
        {
            uint16_t readword = inportw(io_port);
            p_buffer[idx * 2] = (uint8_t)readword;
            p_buffer[idx * 2 + 1] = (uint8_t)(readword >> 8);
        }
    }

    printk_fs("PIO Read Sucess\n");
    return READ_PIO_SUCCESS;
}


uint8_t writeDMADiskSectors(ATADisk *p_ataDisk, uint64_t addr, uint16_t p_sector_count)
{
    uint16_t io_port = p_ataDisk->io_port;
    uint32_t dma_port_address = p_ataDisk->dma_port_address;
    uint8_t *dma_phy_address = p_ataDisk->dma_phy_address;
    uint32_t *ptr = (uint32_t *)p_ataDisk->prdt;

    uint8_t bm_status = inportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset);
    bm_status = inportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset);
    outportb(dma_port_address + BM0_COMMAND + p_ataDisk->bm_offset, (0 << 0));
    outportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset, bm_status);

    uint64_t sectors = p_sector_count;

    uint16_t i = 0;
    for (; sectors > 0; i += 2)
    {
        if (sectors > 128)
        {
            ptr[i] = (uint64_t)dma_phy_address + ((i / 2) * 0x10000);
            ptr[i + 1] = 0x00000000 | (128 * 0x200);
            sectors -= 128;
        }
        else if (sectors <= 128)
        {
            ptr[i] = (uint64_t)dma_phy_address + ((i / 2) * 0x10000);
            ptr[i + 1] = 0x80000000 | (sectors * 0x200);
            sectors = 0;
        }
    }
    outportb(dma_port_address + BM0_COMMAND + p_ataDisk->bm_offset, 0);
    outportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset, (1 << 2) | (1 << 1));
    outportl(dma_port_address + BM0_ADDRESS + p_ataDisk->bm_offset, (uint64_t)ptr);
    uint16_t control_base = 0;
    if (io_port == ATA_PRIMARY_IO)
        control_base = ATA_PRIMARY_DCR_AS;
    else
        control_base = ATA_SECONDARY_DCR_AS;

    uint8_t select = (0xE0 | (p_ataDisk->slavebit << 4));
    if (kernel.dmaBuffer.current_select != select || true)
    {
        if (p_ataDisk->lba48)
            outportb(io_port + ATA_REG_HDDEVSEL, select);
        else
            outportb(io_port + ATA_REG_HDDEVSEL, (select | ((addr >> 24) & 0x0F)));
        ide_400ns_delay(p_ataDisk);
        kernel.dmaBuffer.current_select = select;
    }
    uint8_t status = inportb(control_base + ATA_REG_ALT_STATUS);
    if (status)
    {
        while (true)
        {
            if (status & ATA_SR_DRDY)
                break;
            status = inportb(control_base + ATA_REG_ALT_STATUS);
        }
    }

    outportb(io_port + ATA_REG_FEATURES, 0x00);
    outportb(io_port + ATA_REG_SECCOUNT1, (uint8_t)(p_sector_count >> 8));
    outportb(io_port + ATA_REG_LBA3, (uint8_t)(addr >> 24));
    outportb(io_port + ATA_REG_LBA4, (uint8_t)(addr >> 32));
    outportb(io_port + ATA_REG_LBA5, (uint8_t)(addr >> 40));
    outportb(io_port + ATA_REG_SECCOUNT0, (uint8_t)p_sector_count);
    outportb(io_port + ATA_REG_LBA0, (uint8_t)addr);
    outportb(io_port + ATA_REG_LBA1, (uint8_t)(addr >> 8));
    outportb(io_port + ATA_REG_LBA2, (uint8_t)(addr >> 16));
    if (p_ataDisk->lba48)
        outportb(io_port + ATA_REG_COMMAND, ATA_CMD_WRITE_DMA_EXT);
    else
        outportb(io_port + ATA_REG_COMMAND, ATA_CMD_WRITE_DMA);
    status = inportb(control_base + ATA_REG_ALT_STATUS);

    if (status)
    {
        for (uint64_t i = 0;; i++)
        {
            if ((status & ATA_SR_BSY))
            {
            }
            else if (status & ATA_SR_ERR)
            {
                if (p_ataDisk->bm_offset == 0)
                    outportb(ATA_PRIMARY_DCR_AS, 0b00000000);
                else
                    outportb(ATA_SECONDARY_DCR_AS, 0b00000000);
                if (p_ataDisk->slavebit)
                    outportb(io_port + ATA_REG_HDDEVSEL, ATA_SLAVE_DRV_SELECTOR);
                else
                    outportb(io_port + ATA_REG_HDDEVSEL, ATA_MASTER_DRV_SELECTOR);
                ide_400ns_delay(p_ataDisk);
                status = inportb(io_port + ATA_REG_STATUS);
                kernel.dmaBuffer.current_select = 0;
                printk_fs("DMA Write Error\n");

                return WRITE_DMA_FAIL;
            }
            else if (status & ATA_SR_DRQ)
                break;
            status = inportb(control_base + ATA_REG_ALT_STATUS);
            if (!(status & ATA_SR_DRQ) && i > 5000)
            {
                printk_fs("stuck in write dma check status loop: %d, status: %x\n", i, status);
                if (i > 100000)
                {
                    if (p_ataDisk->bm_offset == 0)
                        outportb(ATA_PRIMARY_DCR_AS, 0b00000000);
                    else
                        outportb(ATA_SECONDARY_DCR_AS, 0b00000000);
                    if (p_ataDisk->slavebit)
                        outportb(io_port + ATA_REG_HDDEVSEL, ATA_SLAVE_DRV_SELECTOR);
                    else
                        outportb(io_port + ATA_REG_HDDEVSEL, ATA_MASTER_DRV_SELECTOR);
                    ide_400ns_delay(p_ataDisk);
                    status = inportb(io_port + ATA_REG_STATUS);
                    kernel.dmaBuffer.current_select = 0;
                    printk_fs("DMA Write Error with stuck\n");

                    return WRITE_DMA_FAIL;
                }
            }
        }
    }
    if (status & ATA_SR_DRQ)
    {
        outportb(dma_port_address + BM0_COMMAND + p_ataDisk->bm_offset, (0 << 3) | (1 << 0));

        return WRITE_DMA_SUCCESS;
    }

    return WRITE_DMA_FAIL;
}
uint8_t readDMADiskSectors(ATADisk *p_ataDisk, uint64_t addr, uint16_t p_sector_count)
{
    for (;;)
    {
        uint16_t io_port = p_ataDisk->io_port;
        uint32_t dma_port_address = p_ataDisk->dma_port_address;
        uint8_t *dma_phy_address = p_ataDisk->dma_phy_address;
        uint32_t *ptr = (uint32_t *)p_ataDisk->prdt;
        uint64_t sectors = p_sector_count;
        uint8_t bm_status = inportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset);
        
        memset(dma_phy_address, 0, 0x2000000);
        bm_status = inportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset);
        outportb(dma_port_address + BM0_COMMAND + p_ataDisk->bm_offset, (0 << 0));
        outportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset, bm_status);

    
        for (uint16_t i = 0; sectors > 0; i += 2)
        {
            if (sectors > 128)
            {
                ptr[i] = (uint64_t)dma_phy_address + ((i / 2) * 0x10000);
                ptr[i + 1] = 0x00000000 | (128 * 0x200);
                sectors -= 128;
            }
            else if (sectors <= 128)
            {
                ptr[i] = (uint64_t)dma_phy_address + ((i / 2) * 0x10000);
                ptr[i + 1] = 0x80000000 | (sectors * 0x200);
                sectors = 0;
            }
        }
        outportb(dma_port_address + BM0_COMMAND + p_ataDisk->bm_offset, 0);
        outportb(dma_port_address + BM0_STATUS + p_ataDisk->bm_offset, (1 << 2) | (1 << 1));
        outportl(dma_port_address + BM0_ADDRESS + p_ataDisk->bm_offset, (uint32_t)ptr);
        uint16_t control_base = ATA_SECONDARY_DCR_AS;
        if (io_port == ATA_PRIMARY_IO)
            control_base = ATA_PRIMARY_DCR_AS;

        uint8_t select = (0xE0 | (p_ataDisk->slavebit << 4));
        if (kernel.dmaBuffer.current_select != select || true)
        {
            if (p_ataDisk->lba48)
                outportb(io_port + ATA_REG_HDDEVSEL, select);
            else
                outportb(io_port + ATA_REG_HDDEVSEL, (select | ((addr >> 24) & 0x0F)));
            kernel.dmaBuffer.current_select = select;
        }
        uint8_t status = inportb(control_base + ATA_REG_ALT_STATUS);
        uint64_t stuck = 0;
        if (status)
        {
            while (true)
            {
                if (status & ATA_SR_DRDY)
                    break;
                status = inportb(control_base + ATA_REG_ALT_STATUS);
                if (stuck > 2000)
                {
                    printk_fs("stuck in DRDY select loop: %x\n", status);
                    break;
                }
                stuck++;
            }
            if (!(status & ATA_SR_DRDY))
                continue;
        }
        outportb(io_port + ATA_REG_FEATURES, 0x00);
        outportb(io_port + ATA_REG_SECCOUNT1, (uint8_t)(p_sector_count >> 8) & 0xFF);
        outportb(io_port + ATA_REG_LBA3, (uint8_t)(addr >> 24));
        outportb(io_port + ATA_REG_LBA4, (uint8_t)(addr >> 32));
        outportb(io_port + ATA_REG_LBA5, (uint8_t)(addr >> 40));
        outportb(io_port + ATA_REG_SECCOUNT0, (uint8_t)p_sector_count & 0xFF);
        outportb(io_port + ATA_REG_LBA0, (uint8_t)addr);
        outportb(io_port + ATA_REG_LBA1, (uint8_t)(addr >> 8));
        outportb(io_port + ATA_REG_LBA2, (uint8_t)(addr >> 16));

        if (p_ataDisk->lba48)
            outportb(io_port + ATA_REG_COMMAND, ATA_CMD_READ_DMA_EXT);
        else
            outportb(io_port + ATA_REG_COMMAND, ATA_CMD_READ_DMA); // 0xC8 for reading

        status = inportb(control_base + ATA_REG_ALT_STATUS);
        if (status)
        {
            for (uint64_t i = 0;; i++)
            {
                if ((status & ATA_SR_BSY))
                {
                }
                else if (status & ATA_SR_ERR)
                {
                    printk_fs("p_sector count: %d, addr: %d\n", p_sector_count, addr);
                    for (;;);
                    if (p_ataDisk->bm_offset == 0)
                        outportb(ATA_PRIMARY_DCR_AS, 0b00000000);
                    else
                        outportb(ATA_SECONDARY_DCR_AS, 0b00000000);
                    if (p_ataDisk->slavebit)
                        outportb(io_port + ATA_REG_HDDEVSEL, ATA_SLAVE_DRV_SELECTOR);
                    else
                        outportb(io_port + ATA_REG_HDDEVSEL, ATA_MASTER_DRV_SELECTOR);
                    ide_400ns_delay(p_ataDisk);
                    status = inportb(io_port + ATA_REG_STATUS);
                    kernel.dmaBuffer.current_select = 0;
                    return READ_DMA_FAIL;
                }
                else if (status & ATA_SR_DRQ)
                    break;
                status = inportb(control_base + ATA_REG_ALT_STATUS);
                if (!(status & ATA_SR_DRQ) && i > 5000)
                {
                    printk_fs("stuck in read dma check status loop: %d, status: %x\n", i, status);
                    break;
                }
            }
        }
        if (status & ATA_SR_DRQ)
        {
            outportb(dma_port_address + BM0_COMMAND + p_ataDisk->bm_offset, (1 << 3) | (1 << 0));
            break;
        }
    }
    return READ_DMA_SUCCESS;
}

void initATADisk(ATADisk *p_ataDisk, uint16_t p_io_port, uint8_t p_drv_selector, uint8_t p_slavebit,
                 bool p_dma, uint8_t *p_prdt, uint8_t *p_dma_phy_address,
                 uint32_t p_dma_port_address, uint32_t p_dma_mmio_address, uint8_t p_bm_offset)
{
    p_ataDisk->io_port = p_io_port;
    p_ataDisk->drv_selector = p_drv_selector;
    p_ataDisk->slavebit = p_slavebit;
    memset(p_ataDisk->serial, 0, 21);
    memset(p_ataDisk->fw_revision, 0, 10);
    memset(p_ataDisk->ata_desc, 0, 512);
    for (uint16_t idx = 0; idx < 256; idx++)
    {
        uint16_t readword = inportw(p_ataDisk->io_port);
        p_ataDisk->ata_desc[idx * 2] = (uint8_t)readword;
        p_ataDisk->ata_desc[idx * 2 + 1] = (uint8_t)(readword >> 8);
    }
    printk("Device Type: %d\n", p_ataDisk->ata_desc[ATA_IDENT_DEVICETYPE]);
    p_ataDisk->dma = p_dma;
    p_ataDisk->prdt = p_prdt;
    p_ataDisk->dma_phy_address = p_dma_phy_address;

    p_ataDisk->dma_port_address = p_dma_port_address;
    p_ataDisk->dma_mmio_address = p_dma_mmio_address;
    p_ataDisk->bm_offset = p_bm_offset;

    p_ataDisk->capabilities = p_ataDisk->ata_desc[ATA_IDENT_CAPABILITIES] |
                              (p_ataDisk->ata_desc[ATA_IDENT_CAPABILITIES + 1] << 8) |
                              (p_ataDisk->ata_desc[ATA_IDENT_CAPABILITIES + 2] << 16) |
                              (p_ataDisk->ata_desc[ATA_IDENT_CAPABILITIES + 3] << 24);
    p_ataDisk->commandsets = p_ataDisk->ata_desc[ATA_IDENT_COMMANDSETS] |
                             (p_ataDisk->ata_desc[ATA_IDENT_COMMANDSETS + 1] << 8) |
                             (p_ataDisk->ata_desc[ATA_IDENT_COMMANDSETS + 2] << 16) |
                             (p_ataDisk->ata_desc[ATA_IDENT_COMMANDSETS + 3] << 24);

    uint8_t idx = ATA_IDENT_MAX_LBA;
    p_ataDisk->lba48 = false;
    if (p_ataDisk->commandsets & (1 << 26))
    {
        idx = ATA_IDENT_MAX_LBA_EXT;
        p_ataDisk->lba48 = true;
    }
    p_ataDisk->disk_size = p_ataDisk->ata_desc[idx] |
                           (p_ataDisk->ata_desc[idx + 1] << 8) |
                           (p_ataDisk->ata_desc[idx + 2] << 16) |
                           (p_ataDisk->ata_desc[idx + 3] << 24);
    for (uint16_t idx = 20; idx < 40; idx++)
        p_ataDisk->serial[(idx - 20)] = (char)p_ataDisk->ata_desc[idx];
    for (uint16_t idx = 46; idx < 54; idx++)
        p_ataDisk->fw_revision[(idx - 46)] = (char)p_ataDisk->ata_desc[idx];
    for (uint16_t idx = 54; idx < 54 + 40; idx++)
        p_ataDisk->model_no[(idx - 54)] = (char)p_ataDisk->ata_desc[idx];
}

void ide_400ns_delay(ATADisk *p_ataDisk)
{
    for (int i = 0; i < 4; i++)
        inportb(p_ataDisk->io_port + ATA_REG_STATUS);
}

void readATAStatus(ATADisk *p_ataDisk)
{
    inportb(p_ataDisk->io_port + ATA_REG_STATUS);
}
uint8_t writeDiskSectors(ATADisk *p_ataDisk, uint64_t p_addr, uint8_t *p_buffer, uint8_t p_sector_count)
{
    if (p_ataDisk->dma)
        return writeDMADiskSectors(p_ataDisk, p_addr, p_sector_count);
    else
        return writePIODiskSectors(p_ataDisk, p_addr, p_buffer, p_sector_count);
}
uint8_t readDiskSectors(ATADisk *p_ataDisk, uint64_t p_addr, uint8_t *p_buffer, uint8_t p_sector_count)
{
    if (p_ataDisk->dma)
        return readDMADiskSectors(p_ataDisk, p_addr, p_sector_count);
    else
        return readPIODiskSectors(p_ataDisk, p_addr, p_buffer, p_sector_count);
}
uint8_t readDMADisk(ATADisk *p_ataDisk, uint64_t p_addr, uint8_t *p_buffer, uint64_t p_sector_count, uint16_t p_start_offset, uint16_t p_end_offset)
{
    kernel.dmaBuffer.ataDisk->done_read = false;
    kernel.dmaBuffer.buffer = p_buffer;
    kernel.dmaBuffer.current_address = p_addr;
    kernel.dmaBuffer.lba48 = p_ataDisk->lba48;
    uint32_t read_dma_chunk = DMA_READ_CHUNK;
    if (kernel.dmaBuffer.lba48)
        read_dma_chunk = DMA_READ_CHUNK_EXT;

    kernel.dmaBuffer.start_offset = p_start_offset;
    kernel.dmaBuffer.end_offset = p_end_offset;

    if (p_sector_count > read_dma_chunk) //check if the target read is more than 32MB
        kernel.dmaBuffer.current_read = read_dma_chunk;
    else
        kernel.dmaBuffer.current_read = p_sector_count;

    kernel.dmaBuffer.start_time = getRTCTimeStamp32();
    kernel.dmaBuffer.total_read = 0;
    kernel.dmaBuffer.total_read_bytes = 0;
    kernel.dmaBuffer.target_read = p_sector_count;
    kernel.dmaBuffer.ataDisk = p_ataDisk;

    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    kernel.dmaBuffer.core_id = kernel.apicManager.returns.core_id;
    kernel.dmaBuffer.read = true;
    while (readDMADiskSectors(kernel.dmaBuffer.ataDisk, kernel.dmaBuffer.current_address, kernel.dmaBuffer.current_read) != READ_DMA_SUCCESS);

    return READ_DMA_SUCCESS;
}
uint8_t writeDMADisk(ATADisk *p_ataDisk, uint64_t p_addr, uint8_t *p_buffer, uint64_t p_sector_count)
{
    kernel.dmaBuffer.buffer = p_buffer;
    kernel.dmaBuffer.current_address = p_addr;
    kernel.dmaBuffer.lba48 = p_ataDisk->lba48;
    uint32_t write_dma_chunk = DMA_WRITE_CHUNK;
    if (kernel.dmaBuffer.lba48)
        write_dma_chunk = DMA_WRITE_CHUNK_EXT;
    kernel.dmaBuffer.current_read = 0;
    kernel.dmaBuffer.total_read = 0;
    kernel.dmaBuffer.target_read = 0;
    if (p_sector_count > write_dma_chunk)
        kernel.dmaBuffer.current_write = write_dma_chunk;
    else
        kernel.dmaBuffer.current_write = p_sector_count;
    kernel.dmaBuffer.start_time = getRTCTimeStamp32();

    kernel.dmaBuffer.total_write = 0;
    kernel.dmaBuffer.target_write = p_sector_count;
    kernel.dmaBuffer.ataDisk = p_ataDisk;

    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    kernel.dmaBuffer.core_id = kernel.apicManager.returns.core_id;
    kernel.dmaBuffer.read = false;
    fast_memcpy(kernel.dmaBuffer.ataDisk->dma_phy_address,
                kernel.dmaBuffer.buffer + (kernel.dmaBuffer.total_write * 0x200),
                kernel.dmaBuffer.current_write * 0x200);
    for (;;)
        if (writeDMADiskSectors(kernel.dmaBuffer.ataDisk, kernel.dmaBuffer.current_address, kernel.dmaBuffer.current_write) == WRITE_DMA_SUCCESS)
            break;
    return WRITE_DMA_SUCCESS;
}
void copyDMABuffer(ATADisk *p_ataDisk, uint8_t *p_buffer, uint16_t p_sector_count)
{
    memcpy(p_buffer, p_ataDisk->dma_phy_address, p_sector_count * 0x200);
}
