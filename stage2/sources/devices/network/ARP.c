#include "Kernel.h"
#include "ARP.h"
#include "Ethernet.h"
#include "memswap.h"
#include "memcpy.h"

extern Kernel kernel;
arp_table_entry arpTable[512];
NetworkPacket *savedPacket = 0;

arp_header_t *getARPPacketFromSKB(void *skb)
{
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    return (arp_header_t *)(((uint64_t)(networkPacket->ethernetPacket)) + ETH_HLEN);
}

void DumpEthernetPacketData(NetworkPacket *packet)
{
    printk("Ethernet Packet:\n");
    printk("Packet Going from MAC Address: %y:%y:%y:%y:%y:%y\n",
           packet->ethernetPacket->h_source[0],
           packet->ethernetPacket->h_source[1],
           packet->ethernetPacket->h_source[2],
           packet->ethernetPacket->h_source[3],
           packet->ethernetPacket->h_source[4],
           packet->ethernetPacket->h_source[5]);
    printk("Packet Going to MAC Address: %y:%y:%y:%y:%y:%y\n",
           packet->ethernetPacket->h_dest[0],
           packet->ethernetPacket->h_dest[1],
           packet->ethernetPacket->h_dest[2],
           packet->ethernetPacket->h_dest[3],
           packet->ethernetPacket->h_dest[4],
           packet->ethernetPacket->h_dest[5]);
    //arp_header_t *arp_packet = (arp_header_t *)(((uint64_t)(packet->ethernetPacket)) + ETH_HLEN);
    //DumpARPPacketData(arp_packet);
}

void DumpARPPacketData(arp_header_t *packet)
{

    printk("ARP Packet:\n");

    printk("AR_HLN: %d | AR_HRD: %d | AR_OP: %d | AR_PLN: %d | AR_PRO: %d\n",
           packet->ar_hln,
           packet->ar_hrd,
           packet->ar_op,
           packet->ar_pln,
           packet->ar_pro);

    printk("Packet Going from MAC Address: %y:%y:%y:%y:%y:%y | IP Address: %d.%d.%d.%d\n",
           packet->sender_hw_address[0],
           packet->sender_hw_address[1],
           packet->sender_hw_address[2],
           packet->sender_hw_address[3],
           packet->sender_hw_address[4],
           packet->sender_hw_address[5],
           packet->sender_protocol_address[0],
           packet->sender_protocol_address[1],
           packet->sender_protocol_address[2],
           packet->sender_protocol_address[3]);

    printk("Packet Going to MAC Address: %y:%y:%y:%y:%y:%y | IP Address: %d.%d.%d.%d\n",
           packet->target_hw_address[0],
           packet->target_hw_address[1],
           packet->target_hw_address[2],
           packet->target_hw_address[3],
           packet->target_hw_address[4],
           packet->target_hw_address[5],
           packet->target_protocol_address[0],
           packet->target_protocol_address[1],
           packet->target_protocol_address[2],
           packet->target_protocol_address[3]);
}

void processARPPacket(NetworkDriver *p_networkDriver, void *skb)
{
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    arp_header_t *arp_header = getARPPacketFromSKB(skb);
    printk(">>>>>> Processing arp packet: %d\n", networkPacket->packet_size);
    // arp_header->target_protocol_address
    // if ( p_networkDriver == getNetworkDriverByIPAddress(arp_header->target_protocol_address))
    {
        // uint32_t sourceMACAddress[6];
        uint32_t destinationMACAddress[6];
        uint32_t destinationIPAddress[4];

        memcpy(destinationMACAddress, networkPacket->ethernetPacket->h_source, 6);
        memcpy(destinationIPAddress, arp_header->sender_protocol_address, 4);
        ReverseSourceAndDestination(networkPacket->ethernetPacket->h_source, networkPacket->ethernetPacket->h_dest);
        //uint8_t * mac = getMacByIPAddress(arp_header->target_protocol_address);

        if (htons(arp_header->ar_hrd) == ARPHRD_ETHER && htons(arp_header->ar_op) == ARPOP_REQUEST && arp_header->ar_pln == 4)
        {
            // uint32_t myIP = 0x0e02000a;
            // if (memcmp(arp_header->target_protocol_address, &myIP, 4) == NULL)
            {
                printk("processing arp of my IP Address\n");
                fast_memcpy(networkPacket->ethernetPacket->h_source, ((E1000 *)p_networkDriver->driver)->mac, 6);
                fast_memcpy(networkPacket->ethernetPacket->h_dest, destinationMACAddress, 6);

                // set opcode
                arp_header->ar_op = ntohs(ARPOP_REPLY);

                // set target ip address
                fast_memswap(arp_header->target_protocol_address,
                             arp_header->sender_protocol_address, arp_header->ar_pln);

                // unicast to the destination hardware address
                fast_memcpy(arp_header->target_hw_address,
                            destinationMACAddress, arp_header->ar_hln);

                // set my own mac address
                fast_memcpy(arp_header->sender_hw_address, ((E1000 *)p_networkDriver->driver)->mac, 6);

                // statically set my own IP address due to the inavailability of the DHCP server
                arp_header->sender_protocol_address[0] = 192;
                arp_header->sender_protocol_address[1] = 168;
                arp_header->sender_protocol_address[2] = 1;
                arp_header->sender_protocol_address[3] = 4;

                // set lengths
                arp_header->ar_hln = 6;
                arp_header->ar_pln = 4;

                // set hardware type
                arp_header->ar_hrd = htons(0x01);

                // set protocol to ipv4
                arp_header->ar_pro = htons(0x0800);

                p_networkDriver->send(p_networkDriver, networkPacket->ethernetPacket, networkPacket->packet_size);
            }
        }
    }
    //else printk ("Got failing ARP Packet: %d\n",networkPacket->packet_size);
}

// void processARPPacket(NetworkDriver *p_networkDriver, void *skb)
// {
//     E1000 *p_e1000 = (E1000 *)p_networkDriver->driver;
//     NetworkPacket *networkPacket = (NetworkPacket *)skb;
//     arp_header_t *arpHeader = getARPPacketFromSKB(skb);
//     printk(">>>>>> Processing arp packet: %d\n", networkPacket->packet_size);

//     printk("Before Processing!\n");
//     DumpEthernetPacketData(networkPacket);
//     uint32_t sourceMACAddress[8];
//     uint32_t destinationMACAddress[8];
//     memcpy(sourceMACAddress, networkPacket->ethernetPacket->h_source, 6);
//     memcpy(destinationMACAddress, networkPacket->ethernetPacket->h_data, 6);

//     // set source and destination mac address
//     ReverseSourceAndDestination(networkPacket->ethernetPacket->h_source, networkPacket->ethernetPacket->h_dest);
//     uint8_t *mac = ((E1000 *)p_networkDriver->driver)->mac;
//     fast_memcpy(networkPacket->ethernetPacket->h_source, mac, 6);
//     fast_memcpy(networkPacket->ethernetPacket->h_dest, sourceMACAddress, 6);

//     if (htons(arpHeader->ar_hrd) == ARPHRD_ETHER && htons(arpHeader->ar_op) == ARPOP_REQUEST && arpHeader->ar_pln == 4)
//     {
//         // set opcode
//         arpHeader->ar_op = ntohs(ARPOP_REPLY);

//         fast_memswap(arpHeader->target_protocol_address,
//                      arpHeader->sender_protocol_address, arpHeader->ar_pln);

//         arpHeader->sender_protocol_address[0] = 192;
//         arpHeader->sender_protocol_address[1] = 168;
//         arpHeader->sender_protocol_address[2] = 1;
//         arpHeader->sender_protocol_address[3] = 100;

//         fast_memcpy(arpHeader->target_hw_address,
//                     arpHeader->sender_hw_address, arpHeader->ar_hln);
//         // set source mac address
//         fast_memcpy(arpHeader->sender_hw_address, mac, 6);
//         printk("\nAfter Processing\n");
//         DumpEthernetPacketData(networkPacket);
//         p_networkDriver->send(p_networkDriver, networkPacket->ethernetPacket, networkPacket->packet_size);
//     }
//     else if (htons(arpHeader->ar_hrd) == ARPHRD_ETHER && htons(arpHeader->ar_op) == ARPOP_REPLY)
//     {
//         printk("Got ARP Reply\n");
//     }
//     else
//     {
//         printk("Got failing ARP Packet: %d\n", networkPacket->packet_size);
//     }
// }

void RequestMACAddress(NetworkDriver *p_networkDriver, uint8_t *ipAddress)
{

    // if(savedPacket != 0){
    //     DumpEthernetPacketData(savedPacket);
    //     p_networkDriver->send(p_networkDriver, savedPacket->ethernetPacket, savedPacket->packet_size);
    //     return;
    // }

    savedPacket = kmalloc(&kernel.memoryAllocator, sizeof(NetworkPacket));
    savedPacket->ethernetPacket = kmalloc(&kernel.memoryAllocator, sizeof(EthernetPacket));
    savedPacket->packet_size = ETH_HLEN + sizeof(arp_header_t);
    EthernetPacket *ethPack = savedPacket->ethernetPacket;

    // ARP HEADERS
    arp_header_t *arphead = ethPack->h_data;
    arphead->ar_hrd = 0x0100; // ethernet
    arphead->ar_pro = 0x0008; // ipv4
    arphead->ar_hln = 6;      // mac
    arphead->ar_pln = 4;      // ipv4
    arphead->ar_op = 0x0100;  // request

    // Source MAC Address
    fast_memcpy(arphead->sender_hw_address, ((E1000 *)p_networkDriver->driver)->mac, 6);

    // Destination Mac Address
    arphead->target_hw_address[0] = 0;
    arphead->target_hw_address[1] = 0;
    arphead->target_hw_address[2] = 0;
    arphead->target_hw_address[3] = 0;
    arphead->target_hw_address[4] = 0;
    arphead->target_hw_address[5] = 0;
    arphead->target_hw_address[6] = 0;

    //Source IP Address
    arphead->sender_protocol_address[0] = 192;
    arphead->sender_protocol_address[1] = 168;
    arphead->sender_protocol_address[2] = 1;
    arphead->sender_protocol_address[3] = 100;

    //Destination IP Address
    arphead->target_protocol_address[0] = 192;
    arphead->target_protocol_address[1] = 168;
    arphead->target_protocol_address[2] = 1;
    arphead->target_protocol_address[3] = 1;

    // ETHERNET HEADERS

    // Source Mac Address
    fast_memcpy(ethPack->h_source, ((E1000 *)p_networkDriver->driver)->mac, 6);
    //fast_memcpy(ethPack->h_data, arphead, sizeof(arp_header_t));

    // Destination Mac Address
    ethPack->h_dest[0] = 0xFF;
    ethPack->h_dest[1] = 0xFF;
    ethPack->h_dest[2] = 0xFF;
    ethPack->h_dest[3] = 0xFF;
    ethPack->h_dest[4] = 0xFF;
    ethPack->h_dest[5] = 0xFF;
    ethPack->h_dest[6] = 0xFF;

    // Protocol
    ethPack->h_proto = 1544;

    // Send Packet
    DumpEthernetPacketData(savedPacket);
    p_networkDriver->send(p_networkDriver, savedPacket->ethernetPacket, savedPacket->packet_size);
    return;

    NetworkPacket networkPacket;
    EthernetPacket ethernetPacket;
    arp_header_t arpHeader;
    networkPacket.ethernetPacket = &ethernetPacket;
    //(arp_header_t *)(((uint64_t)(networkPacket->ethernetPacket)) + ETH_HLEN)
    uint16_t packet_size = ETH_HLEN + sizeof(arp_header_t);
    fast_memcpy(ethernetPacket.h_source, ((E1000 *)p_networkDriver->driver)->mac, 6);
    printk("Packet Going from MAC Address: %y:%y:%y:%y:%y:%y\n",
           ethernetPacket.h_source[0],
           ethernetPacket.h_source[1],
           ethernetPacket.h_source[2],
           ethernetPacket.h_source[3],
           ethernetPacket.h_source[4],
           ethernetPacket.h_source[5]);
    uint8_t destinationMACAddress[6];
    destinationMACAddress[0] = 0xFF;
    destinationMACAddress[1] = 0xFF;
    destinationMACAddress[2] = 0xFF;
    destinationMACAddress[3] = 0xFF;
    destinationMACAddress[4] = 0xFF;
    destinationMACAddress[5] = 0xFF;

    uint8_t destinationARPMACAddress[6];
    destinationARPMACAddress[0] = 0x00;
    destinationARPMACAddress[1] = 0x00;
    destinationARPMACAddress[2] = 0x00;
    destinationARPMACAddress[3] = 0x00;
    destinationARPMACAddress[4] = 0x00;
    destinationARPMACAddress[5] = 0x00;

    fast_memcpy(ethernetPacket.h_dest, destinationMACAddress, 6);
    ethernetPacket.h_proto = 1544;
    printk("Packet Going from to Address: %y:%y:%y:%y:%y:%y\n",
           ethernetPacket.h_dest[0],
           ethernetPacket.h_dest[1],
           ethernetPacket.h_dest[2],
           ethernetPacket.h_dest[3],
           ethernetPacket.h_dest[4],
           ethernetPacket.h_dest[5]);
    arpHeader.ar_hrd = 0x0100; // ethernet
    arpHeader.ar_pro = 0x0008; // ipv4
    arpHeader.ar_hln = 6;      // mac
    arpHeader.ar_pln = 4;      // ipv4
    arpHeader.ar_op = 0x0100;  // request

    arpHeader.sender_protocol_address[0] = 10;
    arpHeader.sender_protocol_address[1] = 0;
    arpHeader.sender_protocol_address[2] = 2;
    arpHeader.sender_protocol_address[3] = 14;
    fast_memcpy(arpHeader.sender_hw_address, ((E1000 *)p_networkDriver->driver)->mac, 6);
    fast_memcpy(arpHeader.target_hw_address, destinationARPMACAddress, 6);
    fast_memcpy(arpHeader.target_protocol_address, ipAddress, 4);
    fast_memcpy(ethernetPacket.h_data, &arpHeader, sizeof(arp_header_t));

    arp_header_t *temp = getARPPacketFromSKB((void *)&networkPacket);
    DumpEthernetPacketData(&networkPacket);
    p_networkDriver->send(p_networkDriver, networkPacket.ethernetPacket, networkPacket.packet_size);
}