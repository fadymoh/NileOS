#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "defines.h"
#include "includes.h"
#include "NetworkDriver.h"


//#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
//#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
//#define ETH_FRAME_LEN_WITH_TCP_SUDO	1600		/* Max. octets in frame sans FCS */


#define	ETHERTYPE_IP	    0x0800	/* IP protocol */
#define ETHERTYPE_ARP	    0x0806	/* Addr. resolution protocol */
#define ETHERTYPE_NDFS      0x0811
#define ETHERTYPE_IPIOE     0x0812
#define ETHERTYPE_GIPIOE    0x0813


typedef struct{
        uint8_t     h_dest [ETH_ALEN];          /* destination eth addr	*/
        uint8_t     h_source [ETH_ALEN];	/* source ether addr	*/
        uint16_t    h_proto;
        uint8_t     h_data [ETH_FRAME_LEN_WITH_TCP_SUDO];
}EthernetPacket;

EthernetPacket * getEthernetPacketFromSKB(void *skb);
void processEthernetPacket (NetworkDriver * p_networkDriver,void * skb);

#endif