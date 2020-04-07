#include "Ethernet.h"
#include "Kernel.h"
#include "memswap.h"
#include "ARP.h"
#include "IP.h"
#include "ClockUtils.h"

extern Kernel kernel;

EthernetPacket * getEthernetPacketFromSKB(void *skb)
{
    NetworkPacket * networkPacket = (NetworkPacket *) skb;
    return networkPacket->ethernetPacket;
}

void processEthernetPacket (NetworkDriver * p_networkDriver,void * skb)
{
    // IMPORTANT: we need to check dest mac address match my mack address of broadcast.
    
    EthernetPacket * p_ethernetPacket = getEthernetPacketFromSKB(skb);    
   //printk ("Got Packet: %d\n",ntohs(p_ethernetPacket->h_proto));


    if (ntohs(p_ethernetPacket->h_proto) == ETHERTYPE_ARP)
        processARPPacket(p_networkDriver,skb);
    else if (ntohs(p_ethernetPacket->h_proto) == ETHERTYPE_IP)
        processIPPacket(p_networkDriver,skb);
}
