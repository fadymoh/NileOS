#ifndef ATAMANAGER_H_
#define ATAMANAGER_H_

#include "ATADisk.h"
#include "InterruptContext.h"
#include "pci_device.h"
#include "Spinlock.h"

#define DEVICE_TYPE_COUNT 12
#define CNTRL_TYPE_PIIX 0x1230
#define CNTRL_TYPE_ICH 0x2411
#define CNTRL_TYPE_ICH0 0x2421
#define CNTRL_TYPE_ICH2 0x244B
#define CNTRL_TYPE_ICH5 0x24DB
#define CNTRL_TYPE_ICH6 0x266F
#define CNTRL_TYPE_ICH3 0x248B
#define CNTRL_TYPE_ICH4 0x24CB
#define CNTRL_TYPE_ICH9R 0x2922
#define CNTRL_TYPE_PIIX3 0x7010
#define CNTRL_TYPE_PIIX4 0x7111

typedef struct DMABuffer_s
{
    ATADisk *ataDisk;
    uint8_t *buffer;
    uint64_t current_read;
    uint64_t total_read;
    uint64_t total_read_bytes;
    uint64_t target_read;
    uint64_t current_write;
    uint64_t total_write;
    uint64_t target_write;
    uint64_t current_address;
    uint64_t start_address;
    uint16_t start_offset;
    uint16_t end_offset;
    uint64_t start_time;
    uint8_t core_id;
    bool lba48;
    bool read;
    spinlock_t spinlock;
    uint64_t current_select;
    bool write_done;
    bool enabled;
    bool read_done;
} DMABuffer;

typedef struct ATAManager_s
{
    ATADisk *ataDisks[ATA_COUNT];
    uint32_t dma_port_address;
    uint32_t dma_mmio_address;
    char device_names[12][20];
    uint16_t device_ids[12];
    uint8_t *prdt1;
    uint8_t *prdt2;
    uint8_t *dma_phy_address1;
    uint8_t *dma_phy_address2;
} ATAManager;

PCIDevice *getDiskPCIDevice();
char *getDeviceTypeName(uint16_t p_device_id);
void selectDrive(uint16_t p_io_port, uint8_t p_drv_selector);
bool identifyDisk(uint16_t p_io_port, uint8_t p_drv_selector);
void identifyPCI();
void initATAManager();
uint8_t detectATADisks();
ATADisk *getATADisk(uint8_t p_index);

void ataHandleReadInterrupt(InterruptContext *p_interruptContext);
void ataHandleWriteInterrupt(InterruptContext *p_interruptContext);
void ataHandleInterrupt(InterruptContext *p_interruptContext);

void ata_400ns_delay(uint16_t p_io_port);

#endif /* ATAMANAGER_H */
