#ifndef E1000_H
#define E1000_H

#include "E1000_Defines.h"
#include "Spinlock.h"
#include "MMIOUtils.h"
#include "NetworkDriver.h"
#include "pci_device.h"

#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 8
 
#define E1000_TYPE      'E'


struct e1000_rx_desc {
        volatile uint64_t addr;
        volatile uint16_t length;
        volatile uint16_t checksum;
        volatile uint8_t status;
        volatile uint8_t errors;
        volatile uint16_t special;
} __attribute__((packed));
 
struct e1000_tx_desc {
        volatile uint64_t addr;
        volatile uint16_t length;
        volatile uint8_t cso;
        volatile uint8_t cmd;
        volatile uint8_t status;
        volatile uint8_t css;
        volatile uint16_t special;
} __attribute__((packed));

typedef struct 
{
    uint8_t bar_type;
    uint16_t io_base;
    uint64_t  mem_base;
    bool eerprom_exists;
    uint8_t mac [6];
    char mac_str[18];
    uint64_t rx_vaddr[E1000_NUM_RX_DESC];
    struct e1000_rx_desc *rx_descs[E1000_NUM_RX_DESC];
    struct e1000_tx_desc *tx_descs[E1000_NUM_TX_DESC];
    uint16_t rx_cur;
    uint16_t tx_cur;
    spinlock_t spinlock;
    uint8_t int_line;
}E1000;

void e1000WriteCommand(E1000 * p_e1000, uint16_t p_address, uint32_t p_value);
uint32_t e1000ReadCommand(E1000 * p_e1000,uint16_t p_address);

void e1000DetectEEProm(E1000 * p_e1000);
uint32_t e1000EEpromRead(E1000 * p_e1000,uint8_t addr);
void e1000ReadMACAddress(E1000 * p_e1000);
void e1000PrintMac(E1000 * p_e1000);
void e1000StartLink (E1000 * p_e1000);
//void e1000EnableInterrupt(E1000 * p_e1000);
//void e1000RXInit(E1000 * p_e1000);
//void e1000TXInit(E1000 * p_e1000);
//void e1000HandleReceive(NetworkDriver * p_networkDriver);
//void e1000InterruptHandler(InterruptContext * p_interruptContext);
//int e1000SendPacket(NetworkDriver * p_networkDriver,const void * p_data, uint16_t p_len);
E1000 * e1000Init (PCIDevice * p_pciConfigHeader);
void e1000Scan ();
//void e1000RestartAll();
//void e1000_print_regs(E1000 * p_e1000);
//void e1000WakeupAll();

#endif