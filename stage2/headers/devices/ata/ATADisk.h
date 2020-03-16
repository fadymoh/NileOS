#ifndef ATADISK_H_
#define ATADISK_H_

#include "includes.h"
#include "defines.h"
#include "Ports.h"
#include "MemoryUtilities.h"
#define DMA_READ_CHUNK      256

#define DMA_READ_CHUNK_EXT  65534

#define DMA_WRITE_CHUNK      256
#define DMA_WRITE_CHUNK_EXT  65534


#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x02
#define ATA_REG_LBA3       0x03
#define ATA_REG_LBA4       0x04
#define ATA_REG_LBA5       0x05

typedef struct ATADisk_s
{
        uint8_t ata_desc [512];
        char serial [21];
        char fw_revision [10];
        char model_no [41];
        uint16_t io_port;
        uint16_t control_base;
        uint8_t drv_selector;
        uint32_t disk_size;
        uint8_t slavebit;
        bool dma;
        uint8_t * prdt;
        uint8_t * dma_phy_address;
        uint32_t dma_port_address;
        uint32_t dma_mmio_address;
        uint8_t bm_offset;
        uint32_t capabilities;
        uint32_t commandsets;        
        bool lba48;
}ATADisk;
  
uint8_t writePIODiskSectors (ATADisk * p_ataDisk,uint64_t p_addr,uint8_t * p_buffer,uint16_t p_sector_count);
uint8_t readPIODiskSectors (ATADisk * p_ataDisk,uint64_t p_addr,uint8_t * p_buffer,uint16_t p_sector_count);
uint8_t writeDMADiskSectors (ATADisk * p_ataDisk,uint64_t p_addr,uint16_t p_sector_count);
uint8_t readDMADiskSectors (ATADisk * p_ataDisk,uint64_t p_addr,uint16_t p_sector_count);

void initATADisk(ATADisk * p_ataDisk,uint16_t p_io_port,uint8_t p_drv_selector,uint8_t p_slavebit,bool p_dma,
                    uint8_t * p_prdt,uint8_t * p_dma_phy_address,
                uint32_t p_dma_port_address,uint32_t p_dma_mmio_address,uint8_t p_bm_offset);
void ide_400ns_delay(ATADisk * p_ataDisk);
void readATAStatus (ATADisk * p_ataDisk);
uint8_t writeDiskSectors (ATADisk * p_ataDisk,uint64_t p_addr,uint8_t * p_buffer,uint8_t p_sector_count);
uint8_t readDiskSectors (ATADisk * p_ataDisk,uint64_t p_addr,uint8_t * p_buffer,uint8_t p_sector_count);
void copyDMABuffer (ATADisk * p_ataDisk,uint8_t * p_buffer,uint16_t p_sector_count);


uint8_t readDMADisk (ATADisk * p_ataDisk,uint64_t p_addr,uint8_t * p_buffer,uint64_t p_sector_count,uint16_t start_offset,uint16_t end_offset);
uint8_t writeDMADisk (ATADisk * p_ataDisk,uint64_t p_addr,uint8_t * p_buffer,uint64_t p_sector_count);
        
#endif /* ATADISK_H */
