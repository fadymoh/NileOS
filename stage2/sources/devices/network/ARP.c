#include "Kernel.h"
#include "ARP.h"
#include "Ethernet.h"
#include "memswap.h"
#include "memcpy.h"

extern Kernel kernel;

arp_header_t * getARPPacketFromSKB(void *skb)
{
    NetworkPacket * networkPacket = (NetworkPacket *) skb;
    return (arp_header_t *)(((uint64_t)(networkPacket->ethernetPacket))+ETH_HLEN);
}

void processARPPacket (NetworkDriver * p_networkDriver,void * skb)
{
    printk("processing ARP Packet\n");
//     NetworkPacket * networkPacket = (NetworkPacket *) skb;
//     arp_header_t * arp_header = getARPPacketFromSKB(skb);
//     //printk (">>>>>> Processing arp packet: %d\n",networkPacket->packet_size);
//    // arp_header->target_protocol_address
//    // if ( p_networkDriver == getNetworkDriverByIPAddress(arp_header->target_protocol_address))
//     {
//         //ethernetPacketReverseSourceAndDestination(networkPacket->ethernetPacket);
//         //uint8_t * mac = getMacByIPAddress(arp_header->target_protocol_address);
//         //fast_memcpy (networkPacket->ethernetPacket->h_source,mac,6);
//         if ( htons(arp_header->ar_hrd) == ARPHRD_ETHER 
//                 && htons(arp_header->ar_op) == ARPOP_REQUEST && arp_header->ar_pln == 4)
//         {
//             printk("geit hena\n");
//             // arp_header->ar_op = ntohs(ARPOP_REPLY);
//             // fast_memswap(arp_header->target_protocol_address,
//             //                 arp_header->sender_protocol_address,arp_header->ar_pln);
//             // fast_memcpy (arp_header->target_hw_address, 
//             //                 arp_header->sender_hw_address,arp_header->ar_hln);
//             //fast_memcpy (arp_header->sender_hw_address,mac,6);
//             p_networkDriver->send(p_networkDriver,networkPacket->ethernetPacket,networkPacket->packet_size);
//         }
//     }
//    // else printk ("Got failing ARP Packet: %d\n",networkPacket->packet_size);
}
