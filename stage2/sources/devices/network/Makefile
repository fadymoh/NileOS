include $(CODE_HOME)/makefile.vars

main: Ethernet.o \
        ARP.o \
        IP.o \
		rtl8139.o \
		E1000.o \
		NetworkDriver.o \
		UDP.o \
		TCP.o \
		ICMP.o \
		IPIoE.o \

Ethernet.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c Ethernet.c -o $(OBJECT)/Ethernet.o

ARP.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ARP.c -o $(OBJECT)/ARP.o

IP.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c IP.c -o $(OBJECT)/IP.o

rtl8139.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c rtl8139.c -o $(OBJECT)/rtl8139.o

E1000.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c E1000.c -o $(OBJECT)/E1000.o

NetworkDriver.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c NetworkDriver.c -o $(OBJECT)/NetworkDriver.o

UDP.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c UDP.c -o $(OBJECT)/UDP.o

TCP.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c TCP.c -o $(OBJECT)/TCP.o

ICMP.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ICMP.c -o $(OBJECT)/ICMP.o

IPIoE.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c IPIoE.c -o $(OBJECT)/IPIoE.o