include $(CODE_HOME)/makefile.vars

main: Terminal.o \
	Console.o

Terminal.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c Terminal.c -o $(OBJECT)/Terminal.o

Console.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c Console.c -o $(OBJECT)/Console.o
