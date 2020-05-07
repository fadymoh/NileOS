#include "IPIoE.h"
#include "Kernel.h"

extern Kernel kernel;

IPIOE_Packet *getIPIOEPacketFromSKB(void *skb)
{
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    return (IPIOE_Packet *)(((uint64_t)(networkPacket->ethernetPacket)) + ETH_HLEN);
}

uint8_t *getIPIOEPacketBodyFromSKB(void *skb)
{
    NetworkPacket *networkPacket = (NetworkPacket *)skb;
    IPIOE_Packet *ipoe_header = (IPIOE_Packet *)(((uint64_t)(networkPacket->ethernetPacket)) + ETH_HLEN);
    return ((uint8_t *)ipoe_header) + (ipoe_header->IHL * 4);
}

// sending an IPIoE
// acknowledging getting an IPIoE
void processIPIOEPacket(NetworkDriver *p_networkDriver, void *skb)
{

    EthernetPacket *ethernetPacket = getEthernetPacketFromSKB(skb);
    IPIOE_Packet *ipioe_p = getIPIOEPacketFromSKB(skb);
    printk(" got an IPIoE packet!\n");
    //DumpIPIoE_Packet(ipioe_p);
    //char *d = ipioe_p + sizeof(IPIOE_Packet);

    // printk("Data: ");
    // for (int i = 0; i < 50; ++i)
    // {
    //     printk("%c", d[i]);
    // }
    // printk("\n");
    // spin lock
    // checksum
    // check ba2y l 7aga
    if ((ipioe_p->opcode) == IPIoE_OPCODE_REQUEST)
    {
        // printk("Inside Request\n");
        // PROCESS Packet
        DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);

        int cur_core_id = kernel.apicManager.returns.core_id;
        //printk("Inside IPIoE.. core ID: %d ", cur_core_id);

        int core_id = ipioe_p->destinationCore;
        Worker *worker = &kernel.apicManager.apics[core_id].worker;

        // Queue is full
        if (worker->queue_size == IPIoE_QUEUE_LENGTH)
            return;

        // adjust pointers
        IPIoE_Queue_Entry *entry = &worker->ipioe_queue[worker->queue_end];
        worker->queue_end = (worker->queue_end + 1) % IPIoE_QUEUE_LENGTH;
        worker->queue_size++;

        // allocate shared memory for the data
        // Parameter* param = &kernel.service_transporter.kernel_services[sharedMemory_t]->testMethod[allocatedSharedMemory_t].parameters[0];
        // Parameter* returns = &kernel.service_transporter.kernel_services[sharedMemory_t]->testMethod[allocatedSharedMemory_t].returns[0];
        // //char* parameterTag = ConvertTagToXML(1, param, ipioe_p->dataLength);
        // char* parameterTag = kmalloc(&kernel.memoryAllocator, strlen(param->tag) * 2 + 7 + ipioe_p->dataLength);
        // sprintf("<%s> ", param->tag);
        // fast_memcpy(parameterTag + strlen(parameterTag), itos(ipioe_p->dataLength), 2);
        // sprintf(" </%s>", param->tag);
        // char* response = DispatchKernelTest(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t, parameterTag);

        // // parse XML response
        // ParseXMLString(response);
        // kernel.xmlService.params.tagDirectory = returns->tag;
        // getValueFromTagMessage(&kernel.xmlService);
        // char* data = kernel.xmlService.returns.value;
        // uint64_t* return_address;
        // uint64_t outLength;
        // if (returns->base64Encoded)
        //     return_address = base64_decode(data, strlen(data), &outLength);

        // printk("return_address: %d\n", *(return_address));
        fast_memcpy(&entry->ipioe_p, (void *)ipioe_p, sizeof(IPIOE_Packet));
        fast_memcpy(entry->smac, ethernetPacket->h_source, 6);
        fast_memcpy(entry->dmac, ethernetPacket->h_dest, 6);
        fast_memcpy(entry->data, (void *)ipioe_p + sizeof(IPIOE_Packet), ipioe_p->dataLength);

        if (core_id != cur_core_id)
        {
            //printk("core_id != cur_core_id\n");

            SendFixedIPI(&kernel.apicManager.apics[core_id], IPIOE_IPI);
        }
        else
        {
            kernel.apicManager.apics[cur_core_id].fired_interrupts[IPIOE_IPI] = 1;
            //printk("core_id == cur_core_id\n");
        }

        // copy the data

        // printk("got ")

        //uint8* data = alloc_sharemem(size)
        // memcpy(data, getIPIOEPacketBodyFromSKB(skb)) // MUST HAVE CORRECT IHL
        // kernel.apicManager.apics[destination_core].queuemanager.lock
        // indx = kernel.apicManager.apics[destination_core].queuemanager.qeue.size)++
        // IpioE_queue_entry* ipqe = kernel.apicManager.apics[destination_core].queuemanager.queue.array[indx]
        // ipqe-> smac = asdasd;
        // ipqe-> dmac = asdads;
        // memcpy(ipqe-> packet, ipioe_p);
        // ipqe->data = data;
        // size = size+1+
        // kernel.apicManager.apics[destination_core].queuemanager.unlock

        // //sendipi to this core

        //  //On the destination core:
        // kernel.apicManager.apics[destination_core].queuemanager.lock
        // indx = kernel.apicManager.apics[destination_core].queuemanager.qeue.index
        // IpioE_queue_entry* ipqe = kernel.apicManager.apics[destination_core].queuemanager.queue.array[indx]
        // COPY THIS into a new entry object
        // indx = (index + 1)% MAX_Size
        // kernel.apicManager.apics[destination_core].queuemanager.unlock

        // // go to IPIoE handler
        // if(mycore->worker->isrunnig)
        // exit

        // else
        //     worker->dowork()
        // // execute the fucntion

        // struct worker{
        //     bool isrunning
        //     queue* ipioeq
        // }

        // dowork(){
        //     isrunning = true;

        //     loop over currently available messages
        //     lock()
        //     if(non)
        //         exit
        //     else
        //     {
        //         unlock()
        //     }

        //     // in this part

        //     // end of this part
        //     isrunning = false;
        //     unlock();
        // }

        // Check Destination Core!
        // Allocate shared space for the data...
        // Copy data from original packet to this new location
        // Go to current index of the destination core's qeueue
        // Store the new Entry in the queue
        // Send IPI 100 to destination Core to check the inbound queue

        // datastructure contains a pointer to an entry inside the registry service +
        // ip, macaddress, cores, services running on each core.

        // ON DESTINATION CORE
        // Destination Core now processes the message (We mean the STL-K on this core)
        // Checks the service
        // dispatchkernal
        // raga3ly data
    }
    else if ((ipioe_p->opcode) == IPIoE_OPCODE_REPLY)
    {
    }
}

void DumpIPIoE_Packet(IPIOE_Packet *packet)
{
    printk("Version: %d | IHL: %d | Opcode: %d | Data Length: %d\n \
            Message ID: %d | Checksum: %d\n \
            Source Core: %d |  Destination Core: %d\n \
            Flags: %d | Service Number: %d | Method Number: %d\n | Data: %s\n",
           packet->version, packet->IHL, packet->opcode, packet->dataLength, packet->messageID,
           packet->checksum, packet->sourceCore, packet->destinationCore, packet->flags,
           packet->serviceNumber, packet->methodNumber, (void *)packet + sizeof(IPIOE_Packet));
}

int sendIPIoE_Packet(NetworkDriver *p_networkDriver, uint8_t opcode, uint8_t *mac, uint16_t coreid, uint16_t score_id, uint8_t service, uint8_t method, void *data, uint16_t datalen)
{
    printk("starting to send the ipioe packet\n");
    NetworkPacket *np = kmalloc(&kernel.memoryAllocator, sizeof(NetworkPacket));
    np->ethernetPacket = kmalloc(&kernel.memoryAllocator, sizeof(EthernetPacket));

    // np->ethernetPacket = ethernetPacket;
    np->packet_size = ETH_HLEN + sizeof(IPIOE_Packet) + datalen;

    // FILL OUT ETHERNET PACKET DETAILS
    fast_memcpy(np->ethernetPacket->h_dest, mac, 6);
    fast_memcpy(np->ethernetPacket->h_source, ((E1000 *)p_networkDriver->driver)->mac, 6);
    np->ethernetPacket->h_proto = htons(ETHERTYPE_IPIOE);

    IPIOE_Packet *ipioe_p = np->ethernetPacket->h_data;

    // FILL OUT HEADER DETAILS
    ipioe_p->serviceNumber = service;
    ipioe_p->dataLength = datalen;
    ipioe_p->destinationCore = coreid;
    ipioe_p->sourceCore = score_id;
    ipioe_p->methodNumber = method;

    ipioe_p->opcode = opcode;

    ipioe_p->checksum = 0x0000;
    ipioe_p->flags = 0x0000;
    ipioe_p->IHL = 0x00;
    ipioe_p->messageID = 1;
    ipioe_p->version = 1;

    fast_memcpy((void *)(ipioe_p) + 16, data, datalen);

    //DumpIPIoE_Packet(ipioe_p);
    // DumpEthernetPacketData(np);

    if (np->packet_size > 1400)
    {
        printk("EXCEEDEEEED\n");
    }

    p_networkDriver->send(p_networkDriver, np->ethernetPacket, np->packet_size);
    return 1;
}

void wait_IPIoE()
{
    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
    uint16_t current_apic_id = kernel.apicManager.returns.core_id;

    kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] = 0;

    while (kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] == 0)
    {
        kernel.apicManager.apics[current_apic_id].worker.is_running = false;
        asm volatile("sti;");
        asm volatile("hlt;");
        asm volatile("wbinvd;");
        asm volatile("cli;");
        sendAPICEOI(&kernel.apicManager.apics[current_apic_id]);
    }

    kernel.apicManager.apics[current_apic_id].worker.is_running = true;
    kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] = 0;
}

void wait_IPIoE_messageId(uint16_t message_id)
{

    DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
    uint16_t current_apic_id = kernel.apicManager.returns.core_id;

    kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] = 0;

    //printk("Before halting on core: %d\n", current_apic_id);
    while (kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] == 0 || kernel.apicManager.apics[current_apic_id].worker.ipioe_queue[0].ipioe_p.messageID != message_id) // Should check the whole queue not just first one
    {
        //if (kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] == 0)
        // printk("The IPI Fired is: %d\n", kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI]);
        //printk("MessageID: %d\n", kernel.apicManager.apics[current_apic_id].worker.ipioe_queue[0].ipioe_p.messageID);
        kernel.apicManager.apics[current_apic_id].worker.is_running = false;
        asm volatile("sti;");
        asm volatile("hlt;");
        asm volatile("wbinvd;");
        asm volatile("cli;");
        sendAPICEOI(&kernel.apicManager.apics[current_apic_id]);
    }
    // enableInterrupts();
    // // // disableInterrupts();
    // sendAPICEOI(&kernel.apicManager.apics[0]);
    // sendAPICEOI(&kernel.apicManager.apics[1]);

    //printk("After halting\n");

    kernel.apicManager.apics[current_apic_id].worker.is_running = true;
    kernel.apicManager.apics[current_apic_id].fired_interrupts[IPIOE_IPI] = 0;
}

IPIoE_Queue_Entry *workerDequeue(uint16_t core_id)
{
    // spinlock

    IPIoE_Queue_Entry *ipioe_Q = &kernel.apicManager.apics[core_id].worker.ipioe_queue[kernel.apicManager.apics[core_id].worker.queue_index];

    // Copy Packet
    IPIoE_Queue_Entry *ipioe_p = kmalloc(&kernel.memoryAllocator, sizeof(IPIoE_Queue_Entry));
    // DumpIPIoE_Packet(&(ipioe_Q->ipioe_p));

    fast_memcpy(ipioe_p->smac, ipioe_Q->smac, 6);
    fast_memcpy(ipioe_p->dmac, ipioe_Q->dmac, 6);
    fast_memcpy(&ipioe_p->ipioe_p, &ipioe_Q->ipioe_p, sizeof(IPIOE_Packet));
    // ipioe_p->data = kmalloc(&kernel.memoryAllocator, ipioe_Q->ipioe_p.dataLength);
    fast_memcpy(ipioe_p->data, ipioe_Q->data, ipioe_Q->ipioe_p.dataLength);

    // Increment Queue Index
    kernel.apicManager.apics[core_id].worker.queue_index = (kernel.apicManager.apics[core_id].worker.queue_index + 1) % IPIoE_QUEUE_LENGTH;
    kernel.apicManager.apics[core_id].worker.queue_size--;

    //unlock

    return ipioe_p;
}