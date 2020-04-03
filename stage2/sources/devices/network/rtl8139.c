#include "rtl8139.h"

extern Kernel kernel;
rtl8139_dev_t rtl8139_device;
uint32_t current_packet_ptr;

void rtl8139_handler(InterruptContext *reg)
{

    uint16_t status = inportw(rtl8139_device.io_base + 0x3e);

    if (status & TOK)
    {
        printk("Packet sent\n");
    }
    if (status & ROK)
    {
        printk("Received packet\n");
        receive_packet();
    }

    outportw(rtl8139_device.io_base + 0x3E, 0x5);
}

void read_mac_addr()
{
    uint32_t mac_part1 = inportl(rtl8139_device.io_base + 0x00);
    uint16_t mac_part2 = inportw(rtl8139_device.io_base + 0x04);
    rtl8139_device.mac_addr[0] = mac_part1 >> 0;
    rtl8139_device.mac_addr[1] = mac_part1 >> 8;
    rtl8139_device.mac_addr[2] = mac_part1 >> 16;
    rtl8139_device.mac_addr[3] = mac_part1 >> 24;

    rtl8139_device.mac_addr[4] = mac_part2 >> 0;
    rtl8139_device.mac_addr[5] = mac_part2 >> 8;
    printk("MAC Address: %x:%x:%x:%x:%x:%x\n", rtl8139_device.mac_addr[0], rtl8139_device.mac_addr[1], rtl8139_device.mac_addr[2], rtl8139_device.mac_addr[3], rtl8139_device.mac_addr[4], rtl8139_device.mac_addr[5]);
}

/*
 * Initialize the rtl8139 card driver
 * */
void rtl8139_init()
{

    kernel.pciService.params.p_device = RTL8139_DEVICE_ID;
    kernel.pciService.params.p_vendor = RTL8139_VENDOR_ID;
    DispatchKernel(&kernel.service_transporter, pci_t, get_pci_device);
    PCIDevice *pci_rtl8139_device = kernel.pciService.returns.pciDevice_ptr;

    // First get the network device using PCI

    uint32_t ret = pci_rtl8139_device->bars[0];
    rtl8139_device.bar_type = ret & 0x1;
    // Get io base or mem base by extracting the high 28/30 bits
    rtl8139_device.io_base = ret & (~0x3);
    rtl8139_device.mem_base = ret & (~0xf);
    printk("rtl8139 use %s access (base: %x)\n", (rtl8139_device.bar_type == 0) ? "mem based" : "port based", (rtl8139_device.bar_type != 0) ? rtl8139_device.io_base : rtl8139_device.mem_base);

    // Set current TSAD
    rtl8139_device.tx_cur = 0;

    if (!EnablePCIBusMastering(pci_rtl8139_device))
    {
        printk("Enabling Bus Mastering for Network Driver Failed\n");
        return;
    }

    // Send 0x00 to the CONFIG_1 register (0x52) to set the LWAKE + LWPTN to active high. this should essentially *power on* the device.
    outportb(rtl8139_device.io_base + 0x52, 0x0);

    // Soft reset
    outportb(rtl8139_device.io_base + 0x37, 0x10);
    while ((inportb(rtl8139_device.io_base + 0x37) & 0x10) != 0)
    {
        // Do nothibg here...
    }

    // Allocate receive buffer
    rtl8139_device.rx_buffer = kmalloc(&kernel.memoryAllocator, 8192 + 16 + 1500);
    memset(rtl8139_device.rx_buffer, 0x0, 8192 + 16 + 1500);
    outportl(rtl8139_device.io_base + 0x30, (uint32_t)rtl8139_device.rx_buffer);

    // Sets the TOK and ROK bits high
    outportw(rtl8139_device.io_base + 0x3C, 0x0005);

    // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
    outportl(rtl8139_device.io_base + 0x44, 0xf | (1 << 7));

    // Sets the RE and TE bits high
    outportb(rtl8139_device.io_base + 0x37, 0x0C);

    // Register and enable network interrupts
    uint32_t irq_num = pci_rtl8139_device->int_line + 32;

    mapAPICIRQ(&kernel.apicManager.apics[0], irq_num - IRQ0, irq_num);

    kernel.interruptManager.params.p_interruptNumber = irq_num;
    kernel.interruptManager.params.p_interruptHandler = rtl8139_handler;
    DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

    read_mac_addr();
}

void receive_packet()
{
    uint16_t *t = (uint16_t *)(rtl8139_device.rx_buffer + current_packet_ptr);
    // Skip packet header, get packet length
    uint16_t packet_length = *(t + 1);
    t = t + 2; //skipping header and going straight to the packet data
    void *packet = kmalloc(&kernel.memoryAllocator, packet_length);

    fast_memcpy(packet, t, packet_length);
    ethernet_handle_packet(packet, packet_length);

    current_packet_ptr = (current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;

    if (current_packet_ptr > RX_BUF_SIZE)
        current_packet_ptr -= RX_BUF_SIZE;

    outportw(rtl8139_device.io_base + CAPR, current_packet_ptr - 0x10);
}

void get_mac_addr(uint8_t *src_mac_addr)
{
    memcpy(src_mac_addr, rtl8139_device.mac_addr, 6);
}