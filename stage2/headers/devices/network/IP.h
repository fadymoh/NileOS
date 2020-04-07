#ifndef IPPacket_H_
#define IPPacket_H_

#include "defines.h"
#include "includes.h"
#include "NetworkDriver.h"
#define	IP_PACKET_TYPE_ICMP  	0x1
#define	IP_PACKET_TYPE_UDP  	0x11
#define	IP_PACKET_TYPE_TCP  	0x6

typedef struct ip_header_t {
    uint8_t     ihl:4,
                version:4;
    uint8_t     tos;
    uint16_t    tot_len;
    uint16_t    id;
    uint8_t     res:5,
                flags:3;
    uint8_t     frag_offset;
    
    uint8_t     ttl;
    uint8_t     protocol;
    uint16_t    check;
    uint32_t    saddr;
    uint32_t    daddr;
} ip_header_t;

ip_header_t * getIPPacketFromSKB(void *skb);
uint8_t * getIPPacketBodyFromSKB(void *skb);
void reverseIPSourceAndDestination(ip_header_t * ip_header);
void processIPPacket (NetworkDriver * p_networkDriver,void * skb);

#endif