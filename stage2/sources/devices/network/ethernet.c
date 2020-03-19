#include "ethernet.h"


extern Kernel kernel; 


int ethernet_send_packet(uint8_t * dst_mac_addr, uint8_t * data, int len, uint16_t protocol) {
    uint8_t src_mac_addr[6];
    ethernet_frame_t * frame = kmalloc(&kernel.memoryAllocator,sizeof(ethernet_frame_t) + len);
    void * frame_data = (void*)frame + sizeof(ethernet_frame_t);

    // Get source mac address from network card driver
    get_mac_addr(src_mac_addr);

    // Fill in source and destination mac address
    fast_memcpy(frame->src_mac_addr, src_mac_addr, 6);
    fast_memcpy(frame->dst_mac_addr, dst_mac_addr, 6);

    // Fill in data
    fast_memcpy(frame_data, data, len);

    // Fill in type
    frame->type = htons(protocol);

    // Send packet
    //rtl8139_send_packet(frame, sizeof(ethernet_frame_t) + len);
    //kfree(frame);

    //qemu_printf("Sent an ethernet packet, it looks like this\n");
    //xxd(frame, sizeof(ethernet_frame_t) + len);

    return len;
}

void ethernet_handle_packet(ethernet_frame_t * packet, int len) {
    void * data = (void*) packet + sizeof(ethernet_frame_t);
    int data_len = len - sizeof(ethernet_frame_t);
    printk_network("SRC MAC Address: %x:%x:%x:%x:%x:%x\n", packet->src_mac_addr[0], packet->src_mac_addr[1], packet->src_mac_addr[2], packet->src_mac_addr[3], packet->src_mac_addr[4], packet->src_mac_addr[5]);
    printk_network("DST MAC Address: %x:%x:%x:%x:%x:%x\n", packet->dst_mac_addr[0], packet->dst_mac_addr[1], packet->dst_mac_addr[2], packet->dst_mac_addr[3], packet->dst_mac_addr[4], packet->dst_mac_addr[5]);
    // ARP packet
    if(ntohs(packet->type) == ETHERNET_TYPE_ARP) {
        printk_network("(ARP Packet)\n");
      //  arp_handle_packet(data, data_len);
    }
    // IP packets(could be TCP, UDP or others)
    if(ntohs(packet->type) == ETHERNET_TYPE_IP) {
        printk_network("(IP Packet)\n");
       // ip_handle_packet(data, data_len);
    }
}

/*
 * Initialize the ethernet layer
 * */
void ethernet_init() {
    return;
}
