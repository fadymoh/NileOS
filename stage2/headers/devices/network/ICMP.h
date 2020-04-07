#ifndef ICMPPACKET_H_
#define ICMPPACKET_H_

#include "defines.h"
#include "includes.h"
#include "NetworkDriver.h"

#define ICMP_ECHOREPLY          0
#define ICMP_ECHO               8

typedef struct icmp_header_t {
    //! ICMP message type.
    uint8_t icmp_type;
    //! ICMP operation code.
    uint8_t icmp_code;
    //! ICMP checksum.
    uint16_t icmp_chk;
} icmp_header_t;
            
typedef struct icmp_packet_t
{
    icmp_header_t icmp_header;
    union icmp_t
    {
        struct icmp_ping_t
        {
             //! The PING id.
             uint16_t ping_id;
             //! The PING sequence number.
             uint16_t ping_seq;
        } icmp_ping_t;
        struct icmp_redirect_t
        {
             //! The PING id.
             uint32_t ip_address;
             //! The PING sequence number.
             uint8_t ip_hdr [64];
        } icmp_redirect_t;

    }icmp_t;
}icmp_packet_t;

void processICMPPacket (NetworkDriver * p_networkDriver,void * skb);

#endif