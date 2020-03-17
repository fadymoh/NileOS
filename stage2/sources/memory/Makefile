include $(CODE_HOME)/makefile.vars

main: PhysicalMemory.o \
	MemoryPageMap.o \
	KMalloc.o \
	GlobalDescriptorTable.o \
	GlobalDescriptorTableEntry.o \
	TSSDescriptor.o \
	SharedMemory.o \
	BlockService.o

	
PhysicalMemory.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c PhysicalMemory.c -o $(OBJECT)/PhysicalMemory.o
MemoryPageMap.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c MemoryPageMap.c -o $(OBJECT)/MemoryPageMap.o
KMalloc.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c KMalloc.c -o $(OBJECT)/KMalloc.o
GlobalDescriptorTable.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c GlobalDescriptorTable.c -o $(OBJECT)/GlobalDescriptorTable.o
GlobalDescriptorTableEntry.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c GlobalDescriptorTableEntry.c -o $(OBJECT)/GlobalDescriptorTableEntry.o
TSSDescriptor.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c TSSDescriptor.c -o $(OBJECT)/TSSDescriptor.o
SharedMemory.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c SharedMemory.c -o $(OBJECT)/SharedMemory.o
BlockService.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c BlockService.c -o $(OBJECT)/BlockService.o

