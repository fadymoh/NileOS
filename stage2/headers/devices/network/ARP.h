#ifndef ARPPACKET_H_
#define ARPPACKET_H_

#include "defines.h"
#include "includes.h"
#include "NetworkDriver.h"

#define ARPHRD_ETHER 	1	/* ethernet hardware format */
#define ARPHRD_IEEE802	6	/* token-ring hardware format */
#define ARPHRD_FRELAY 	15	/* frame relay hardware format */
#define ARPHRD_IEEE1394	24	/* IEEE1394 hardware address */
#define ARPHRD_IEEE1394_EUI64 27 /* IEEE1394 EUI-64 */

#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
#define	ARPOP_REVREQUEST 3	/* request protocol address given hardware */
#define	ARPOP_REVREPLY	4	/* response giving protocol address */
#define ARPOP_INVREQUEST 8 	/* request to identify peer */
#define ARPOP_INVREPLY	9	/* response identifying peer */




typedef struct arp_header_t {
    uint16_t ar_hrd;		/* format of hardware address */
    uint16_t ar_pro;		/* format of protocol address */
    uint8_t  ar_hln;		/* length of hardware address */
    uint8_t  ar_pln;		/* length of protocol address */
    uint16_t ar_op;                 /* one of: */
    uint8_t sender_hw_address [6];
    uint8_t sender_protocol_address [4];
    uint8_t target_hw_address [6];
    uint8_t target_protocol_address [4];
} arp_header_t;

typedef struct arp_table_entry_t
{
    uint32_t ip_addr;
    uint64_t mac_addr : 48;
} arp_table_entry;

arp_header_t * getARPPacketFromSKB(void *skb);
void processARPPacket (NetworkDriver * p_networkDriver,void * skb);
void RequestMACAddress(NetworkDriver *p_networkDriver, uint8_t* ipAddress);


#endif /* ARPPACKET_H_ */
