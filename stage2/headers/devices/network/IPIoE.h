#ifndef IPIOE_H
#define IPIOE_H

#include "Ethernet.h"
#include "Spinlock.h"
#define IPIoE_OPCODE_REQUEST 1
#define IPIoE_OPCODE_REPLY 2

// FLAGS
#define ENCODED  0x1
#define ENCRYPTED 0x2

#define IPIoE_QUEUE_LENGTH 1

typedef struct
{
    uint8_t version : 4, IHL : 4;
    uint8_t opcode : 8;
    uint16_t dataLength : 16;
    uint16_t messageID : 16;
    uint16_t checksum : 16;
    uint16_t sourceCore : 16;
    uint16_t destinationCore : 16;
    uint16_t flags : 16;
    uint8_t serviceNumber : 8;
    uint8_t methodNumber : 8;
} IPIOE_Packet;



typedef struct 
{
    uint8_t smac[6];
    uint8_t dmac[6];
    IPIOE_Packet ipioe_p;
    char data[1000]; //changed
    
} IPIoE_Queue_Entry;


typedef struct 
{
    IPIoE_Queue_Entry ipioe_queue[IPIoE_QUEUE_LENGTH];
    uint16_t queue_size;
    uint16_t queue_index;
    uint16_t queue_end;
    spinlock_t lock;
    bool is_running;
} Worker;




IPIOE_Packet * getIPIOEPacketFromSKB(void *skb);
uint8_t * getIPIOEPacketBodyFromSKB(void *skb);
void processIPIOEPacket (NetworkDriver * p_networkDriver,void * skb);
int sendIPIoE_Packet(NetworkDriver*p_networkDriver , uint8_t opcode, uint8_t * mac, uint16_t coreid, uint16_t score_id, uint8_t service, uint8_t method, void* data, uint16_t datalen);
void DumpIPIoE_Packet(IPIOE_Packet *packet);

void wait_IPIoE();
void wait_IPIoE_messageId(uint16_t message_id);
IPIoE_Queue_Entry* workerDequeue(uint16_t core_id);

#endif