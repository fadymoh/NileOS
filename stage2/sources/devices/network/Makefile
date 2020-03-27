include $(CODE_HOME)/makefile.vars

main: rtl8139.o \
		ethernet.o \
		E1000.o \
		NetworkDriver.o

rtl8139.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c rtl8139.c -o $(OBJECT)/rtl8139.o
ethernet.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ethernet.c -o $(OBJECT)/ethernet.o
E1000.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c E1000.c -o $(OBJECT)/E1000.o
NetworkDriver.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c NetworkDriver.c -o $(OBJECT)/NetworkDriver.o

