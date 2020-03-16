include $(CODE_HOME)/makefile.vars

main: InterruptDescriptorTable.o \
		isr_stub.o \
        InterruptManager.o
	
InterruptDescriptorTable.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c InterruptDescriptorTable.c -o $(OBJECT)/InterruptDescriptorTable.o

isr_stub.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c isr_stub.c -o $(OBJECT)/isr_stub.o

InterruptManager.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c InterruptManager.c -o $(OBJECT)/InterruptManager.o
