#include "UDP.h"
#include "memcpy.h"
#include "memswap.h"
#include "checksum.h"
#include "printk.h"
#include "E1000.h"

void sendUDPPacket( NetworkDriver * p_networkDriver,
                uint8_t * mac_src, uint8_t * mac_dest, 
                uint32_t ip_src, uint32_t ip_dest,
                uint16_t port_src, uint16_t port_dest,
                uint8_t * msg,uint16_t msg_len)
{
    printk("sending UDPPacket\n");
    // NetworkPacket networkPacket;
    // EthernetPacket ethernetPacket;
    // networkPacket.ethernetPacket = & ethernetPacket;
    // uint16_t packet_size = ETH_HLEN+sizeof(ip_header_t)+sizeof(udp_header_t)+msg_len;
    // fast_memcpy(&ethernetPacket.h_source,mac_src,6);
    // fast_memcpy(&ethernetPacket.h_dest,mac_dest,6);
    // ethernetPacket.h_proto = htons(ETHERTYPE_IP);
    // ip_header_t * ip_header = getIPPacketFromSKB(&networkPacket);    
    // ip_header->ihl = 5;
    // ip_header->version = 4;
    // ip_header->tos = 0;
    // ip_header->tot_len = htons(sizeof(ip_header_t)+sizeof(udp_header_t)+msg_len);
    // ip_header->id = getRTCTimeStamp32();
    // ip_header->res = 0;
    // ip_header->flags = 2;
    // ip_header->frag_offset = 0;
    // ip_header->ttl = 64;
    
    // ip_header->saddr = ip_src;
    // ip_header->daddr = ip_dest;
    // ip_header->protocol = IP_PACKET_TYPE_UDP;
    // ip_header->check = 0;
    // ip_header->check = calcsum((uint16_t *)ip_header,ip_header->ihl*4);

/*    printk ("ip_header->ihl: %d\n",ip_header->ihl);
    printk ("ip_header->version: %d\n",ip_header->version);
    printk ("ip_header->tos: %d\n",ip_header->tos);
    printk ("ip_header->tot_len: %d\n",ip_header->tot_len);
    printk ("ip_header->id: %d\n",ip_header->id);
    printk ("ip_header->res: %d\n",ip_header->res);
    printk ("ip_header->flags: %d\n",ip_header->flags);
    printk ("ip_header->frag_offset: %d\n",ip_header->frag_offset);
    printk ("ip_header->ttl: %d\n",ip_header->ttl);
    printk ("ip_header->protocol: %d\n",ip_header->protocol);
    printk ("ip_header->check: %d\n",ip_header->check);
    printk ("ip_header->saddr: %d\n",ip_header->saddr);
    printk ("ip_header->daddr: %d\n",ip_header->daddr);
    printk ("packet_size: %d\n",packet_size);
    printk ("msg: %s\n",msg);
 */   
    
    // udp_header_t * udp_header = (udp_header_t *) getIPPacketBodyFromSKB(&networkPacket);
    // udp_header->udph_srcport = port_src;
    // udp_header->udph_destport =port_dest;
    // udp_header->udph_length = htons(sizeof(udp_header_t)+msg_len);
    // udp_header->udph_chksum = 0;
    // //fast_memcpy (((uint8_t *)udp_header)+sizeof(udp_header),msg,msg_len);
    // fast_memcpy (((uint8_t *)udp_header)+sizeof(udp_header),msg,msg_len);
    // //printk ("copied msg: %s\n",((uint8_t *)udp_header)+sizeof(udp_header));
    
    // p_networkDriver->send(p_networkDriver,&ethernetPacket,packet_size);
}
void processUDPPacket (NetworkDriver * p_networkDriver,void * skb)
{
// need to be broken down to functions for different ICMP types    
    NetworkPacket * networkPacket = (NetworkPacket *) skb;

    udp_header_t * udp_header = (udp_header_t *) getIPPacketBodyFromSKB(skb);
    if ( ntohs(udp_header->udph_destport) == KSHELL_PORT ) 
    {
          // kshell_exec(p_networkDriver,skb);
    }
    else
    {
        // We need to dispatch to processes ports.
        printk("Got a UDP packet: \n");
    }
}