#ifndef NETWORKDRIVER_H_
#define NETWORKDRIVER_H_

#include "includes.h"
#include "defines.h"


#define MAX_SCOPE_COUNT 0x4       

typedef struct NetworkDriver NetworkDriver;
typedef int (*sendPacket) (NetworkDriver * p_NetworkDriver,const void * p_data, uint16_t p_len);
typedef int (*pollPacket) (NetworkDriver * p_NetworkDriver);

struct NetworkDriver{
    void * driver;
    char type;
    uint8_t  scope;
    sendPacket send;
    pollPacket poll;
};

bool setNetworkDriverScope(NetworkDriver * networkDriver,char * scope);

#endif
