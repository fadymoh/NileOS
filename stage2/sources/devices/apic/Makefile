include $(CODE_HOME)/makefile.vars

main: APIC.o \
	APICManager.o
	
APIC.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c APIC.c -o $(OBJECT)/APIC.o

APICManager.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c APICManager.c -o $(OBJECT)/APICManager.o


