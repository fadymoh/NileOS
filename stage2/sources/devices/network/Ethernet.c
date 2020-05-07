#include "Ethernet.h"
#include "Kernel.h"
#include "memswap.h"
#include "ARP.h"
#include "IP.h"
#include "ClockUtils.h"
#include "IPIoE.h"

extern Kernel kernel;

EthernetPacket *getEthernetPacketFromSKB(void *skb)
{
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    return networkPacket->ethernetPacket;
}

void ReverseSourceAndDestination(uint8_t *source, uint8_t *destination)
{
    fast_memswap(source, destination, 6);
    uint8_t temp[6];
    memset(temp, 0, 6);
    memcpy(temp, source, 6);
    memcpy(source, destination, 6);
    memcpy(destination, temp, 6);
}

void processEthernetPacket(NetworkDriver *p_networkDriver, void *skb)
{
    // IMPORTANT: we need to check dest mac address match my mack address of broadcast.
    
    EthernetPacket *p_ethernetPacket = getEthernetPacketFromSKB(skb);
    //printk ("Got Packet: %d\n",ntohs(p_ethernetPacket->h_proto));
    if (memcmp(p_ethernetPacket->h_source, ((E1000*)p_networkDriver->driver)->mac, 6) == NULL)
        return;
        
    if (ntohs(p_ethernetPacket->h_proto) == ETHERTYPE_ARP)
        processARPPacket(p_networkDriver, skb);
    else if (ntohs(p_ethernetPacket->h_proto) == ETHERTYPE_IP)
        processIPPacket(p_networkDriver, skb);
    else if (ntohs(p_ethernetPacket->h_proto) == ETHERTYPE_IPIOE)
        processIPIOEPacket(p_networkDriver, skb);
}
