include $(CODE_HOME)/makefile.vars

main: MemoryUtilities.o \
    Ports.o \
    ClockUtils.o \
    CoreUtils.o \
    MMIOUtils.o \
    NetUtils.o \
    memcpy.o \
    memswap.o \
    void_ptr_stack.o \
    stdio.o \
    printk.o \
    checksum.o \
    numbers.o

MemoryUtilities.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c MemoryUtilities.c -o $(OBJECT)/MemoryUtilities.o

Ports.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c Ports.c -o $(OBJECT)/Ports.o

ClockUtils.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ClockUtils.c -o $(OBJECT)/ClockUtils.o

CoreUtils.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c CoreUtils.c -o $(OBJECT)/CoreUtils.o

MMIOUtils.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c MMIOUtils.c -o $(OBJECT)/MMIOUtils.o

memcpy.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c memcpy.c -o $(OBJECT)/memcpy.o

memswap.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c memswap.c -o $(OBJECT)/memswap.o

void_ptr_stack.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c void_ptr_stack.c -o $(OBJECT)/void_ptr_stack.o

stdio.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c stdio.c -o $(OBJECT)/stdio.o
    
printk.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c printk.c -o $(OBJECT)/printk.o
    
checksum.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c checksum.c -o $(OBJECT)/checksum.o

numbers.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c numbers.c -o $(OBJECT)/numbers.o
    
NetUtils.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c NetUtils.c -o $(OBJECT)/NetUtils.o