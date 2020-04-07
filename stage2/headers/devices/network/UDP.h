#ifndef UDPPACKET_H_
#define UDPPACKET_H_

#include "defines.h"
#include "includes.h"
#include "NetworkDriver.h"

#define KSHELL_PORT    777

typedef struct psuedo_udp_hdr_t {
    uint32_t saddr;
    uint32_t daddr;
    uint8_t reserved;
    uint8_t protocol;
    uint16_t udp_len;
}psuedo_udp_hdr_t;

typedef struct udp_header_t {
    uint16_t udph_srcport;
    uint16_t udph_destport;
    uint16_t udph_length;
    uint16_t udph_chksum;
}udp_header_t;



void sendUDPPacket( NetworkDriver * p_networkDriver,
                uint8_t * mac_src, uint8_t * mac_dest, 
                uint32_t ip_src, uint32_t ip_dest,
                uint16_t port_src, uint16_t port_dest,
                uint8_t * msg,uint16_t msg_len);
void processUDPPacket (NetworkDriver * p_networkDriver,void * skb);


#endif