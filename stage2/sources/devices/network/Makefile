include $(CODE_HOME)/makefile.vars

main: rtl8139.o \
		ethernet.o

rtl8139.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c rtl8139.c -o $(OBJECT)/rtl8139.o
ethernet.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ethernet.c -o $(OBJECT)/ethernet.o

