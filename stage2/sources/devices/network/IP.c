#include "IP.h"
#include "Ethernet.h"
#include "Kernel.h"
#include "memswap.h"
#include "memcpy.h"
#include "printk.h"
#include "checksum.h"
#include "memswap.h"
#include "ICMP.h"

ip_header_t * getIPPacketFromSKB(void *skb)
{
    NetworkPacket * networkPacket = (NetworkPacket *) skb;
    return (ip_header_t *)(((uint64_t)(networkPacket->ethernetPacket))+ETH_HLEN);
}

uint8_t * getIPPacketBodyFromSKB(void *skb)
{
    NetworkPacket * networkPacket = (NetworkPacket *) skb;
    ip_header_t * ip_header =  (ip_header_t *)(((uint64_t)(networkPacket->ethernetPacket))+ETH_HLEN);
    return ((uint8_t *)ip_header)+(ip_header->ihl*4);
}
void reverseIPSourceAndDestination(ip_header_t * ip_header)
{
    uint32_t temp = ip_header->daddr;
    ip_header->daddr = ip_header->saddr;
    ip_header->saddr = temp;
}
void processIPPacket (NetworkDriver * p_networkDriver,void * skb)
{
    ip_header_t * ip_header = getIPPacketFromSKB(skb);
/*s    printk("IP Header Size : %d\n",ip_header->ihl*4);
    printk("IP Header Checksum : %d\n",ip_header->check);
    printk("IP Header Protocol : %d\n",ip_header->protocol);
    ip_header->check = 0;
    uint16_t cs = calcsum((uint16_t *)ip_header,ip_header->ihl*4);
    printk("Calculated Checksum : %d\n",cs);
    cs = ip_hdr_checksum((uint16_t *)ip_header);
    printk("ASM Calculated Checksum : %d\n",cs);*/
//    printk("IP Protocol : %d\n",ip_header->protocol);
    //if ( getNetworkDriverByIPAddress((uint8_t *)&ip_header->daddr) == p_networkDriver)
    {
     //   if ( ip_header->protocol == IP_PACKET_TYPE_ICMP ) processICMPPacket(p_networkDriver,skb);
        //else if ( ip_header->protocol == IP_PACKET_TYPE_UDP ) processUDPPacket(p_networkDriver,skb);
        //else printk ("ip_header->protocol: %d\n",ip_header->protocol);
    }
    //else printk ("Invalid IP Address for driver: %d\n",ip_header->daddr);
    
}
