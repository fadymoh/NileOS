#include "ICMP.h"
#include "IP.h"
#include "memcpy.h"
#include "memswap.h"
#include "checksum.h"
#include "printk.h"
#include "E1000.h"

void processICMPPacket (NetworkDriver * p_networkDriver,void * skb)
{
    printk("Processing ICMP Packet\n");
    NetworkPacket * networkPacket = (NetworkPacket *) skb;
    ip_header_t * ip_header = getIPPacketFromSKB(skb);
    uint16_t payload_size = ntohs(ip_header->tot_len)-(ip_header->ihl*4);
// need to be broken down to functions for different ICMP types    
    icmp_header_t * icmp_header = (icmp_header_t *)getIPPacketBodyFromSKB(skb);
    if ( icmp_header->icmp_type == ICMP_ECHO )
    {
//        printk ("ICMP payload_size: %d\n",payload_size);
        uint32_t destinationMACAddress[6];

        memcpy(destinationMACAddress, networkPacket->ethernetPacket->h_source, 6);
        
        ReverseSourceAndDestination(networkPacket->ethernetPacket->h_source, networkPacket->ethernetPacket->h_dest);
        //uint8_t * mac = getMacByIPAddress((uint8_t *) &ip_header->daddr);
        fast_memcpy (networkPacket->ethernetPacket->h_source,((E1000*)p_networkDriver->driver)->mac,6);
        fast_memcpy (networkPacket->ethernetPacket->h_dest,destinationMACAddress,6);
        reverseIPSourceAndDestination(ip_header);        
        icmp_header->icmp_type = 0;
        icmp_header->icmp_code = 0;
        icmp_header->icmp_chk = 0;
        icmp_header->icmp_chk = calcsum((uint16_t *)icmp_header,payload_size);
        ip_header->check = 0;
        ip_header->check = calcsum((uint16_t *)ip_header,ip_header->ihl*4);
        //ip_header->check = ip_hdr_checksum((uint16_t *)ip_header);        
        p_networkDriver->send(p_networkDriver,networkPacket->ethernetPacket,networkPacket->packet_size);
    }
    else printk ("icmp_header->icmp_type: %d\n",icmp_header->icmp_type);
}
