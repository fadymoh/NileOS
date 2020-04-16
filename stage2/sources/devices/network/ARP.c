#include "Kernel.h"
#include "ARP.h"
#include "Ethernet.h"
#include "memswap.h"
#include "memcpy.h"

extern Kernel kernel;
arp_table_entry arpTable[512];

arp_header_t *getARPPacketFromSKB(void *skb)
{
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    return (arp_header_t *)(((uint64_t)(networkPacket->ethernetPacket)) + ETH_HLEN);
}

void processARPPacket(NetworkDriver *p_networkDriver, void *skb)
{
    E1000 *p_e1000 = (E1000 *)p_networkDriver->driver;
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    arp_header_t *arpHeader = getARPPacketFromSKB(skb);
    printk(">>>>>> Processing arp packet: %d\n", networkPacket->packet_size);

    uint8_t destinationMACAddress[6];
    uint8_t destinationProtocolAddress[4];
    fast_memcpy(destinationMACAddress, arpHeader->sender_hw_address, 6);
    fast_memcpy(destinationProtocolAddress, arpHeader->sender_protocol_address, 4);

    // printk("Packet coming from MAC Address: %y:%y:%y:%y:%y:%y | IP Address: %d.%d.%d.%d\n",
    //        destinationMACAddress[0],
    //        destinationMACAddress[1],
    //        destinationMACAddress[2],
    //        destinationMACAddress[3],
    //        destinationMACAddress[4],
    //        destinationMACAddress[5],
    //        destinationProtocolAddress[0],
    //        destinationProtocolAddress[1],
    //        destinationProtocolAddress[2],
    //        destinationProtocolAddress[3]);
    printk("proto: %d\n", networkPacket->ethernetPacket->h_proto);
    // set source and destination mac address
    ReverseSourceAndDestination(networkPacket->ethernetPacket->h_source, networkPacket->ethernetPacket->h_dest);
    uint8_t *mac = ((E1000 *)p_networkDriver->driver)->mac;
    fast_memcpy(networkPacket->ethernetPacket->h_source, mac, 6);

    if (htons(arpHeader->ar_hrd) == ARPHRD_ETHER && htons(arpHeader->ar_op) == ARPOP_REQUEST && arpHeader->ar_pln == 4)
    {
        // set opcode
        arpHeader->ar_op = ntohs(ARPOP_REPLY);

        fast_memswap(arpHeader->target_protocol_address,
                     arpHeader->sender_protocol_address, arpHeader->ar_pln);

        arpHeader->sender_protocol_address[0] = 10;
        arpHeader->sender_protocol_address[1] = 0;
        arpHeader->sender_protocol_address[2] = 2;
        arpHeader->sender_protocol_address[3] = 14;

        fast_memcpy(arpHeader->target_hw_address,
                    arpHeader->sender_hw_address, arpHeader->ar_hln);
        // set source mac address
        fast_memcpy(arpHeader->sender_hw_address, mac, 6);

        printk("Packet Going from MAC Address: %y:%y:%y:%y:%y:%y | IP Address: %d.%d.%d.%d\n",
               arpHeader->sender_hw_address[0],
               arpHeader->sender_hw_address[1],
               arpHeader->sender_hw_address[2],
               arpHeader->sender_hw_address[3],
               arpHeader->sender_hw_address[4],
               arpHeader->sender_hw_address[5],
               arpHeader->sender_protocol_address[0],
               arpHeader->sender_protocol_address[1],
               arpHeader->sender_protocol_address[2],
               arpHeader->sender_protocol_address[3]);

        printk("Packet Going to MAC Address: %y:%y:%y:%y:%y:%y | IP Address: %d.%d.%d.%d\n",
               arpHeader->target_hw_address[0],
               arpHeader->target_hw_address[1],
               arpHeader->target_hw_address[2],
               arpHeader->target_hw_address[3],
               arpHeader->target_hw_address[4],
               arpHeader->target_hw_address[5],
               arpHeader->target_protocol_address[0],
               arpHeader->target_protocol_address[1],
               arpHeader->target_protocol_address[2],
               arpHeader->target_protocol_address[3]);
        p_networkDriver->send(p_networkDriver, networkPacket->ethernetPacket, networkPacket->packet_size);
    }
    else if (htons(arpHeader->ar_hrd) == ARPHRD_ETHER && htons(arpHeader->ar_op) == ARPOP_REPLY)
    {
        printk("Got ARP Reply\n");
    }
    else
    {
        printk("Got failing ARP Packet: %d\n", networkPacket->packet_size);
    }
}

void RequestMACAddress(NetworkDriver *p_networkDriver, uint8_t *ipAddress)
{
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
    printk("arp_op: %d\n", arpHeader.ar_op);
    arpHeader.sender_protocol_address[0] = 10;
    arpHeader.sender_protocol_address[1] = 0;
    arpHeader.sender_protocol_address[2] = 2;
    arpHeader.sender_protocol_address[3] = 14;
    fast_memcpy(arpHeader.sender_hw_address, ((E1000 *)p_networkDriver->driver)->mac, 6);
    fast_memcpy(arpHeader.target_hw_address, destinationMACAddress, 6);
    fast_memcpy(arpHeader.target_protocol_address, ipAddress, 4);
    fast_memcpy(ethernetPacket.h_data, &arpHeader, sizeof(arp_header_t));

    printk("Packet Going to IP Address: %d.%d.%d.%d\n",
           arpHeader.target_protocol_address[0],
           arpHeader.target_protocol_address[1],
           arpHeader.target_protocol_address[2],
           arpHeader.target_protocol_address[3]);
    arp_header_t *temp = getARPPacketFromSKB((void *)&networkPacket);
    printk("arp_op: %d\n", temp->ar_op);

    p_networkDriver->send(p_networkDriver, networkPacket.ethernetPacket, networkPacket.packet_size);
}