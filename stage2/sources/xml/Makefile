include $(CODE_HOME)/makefile.vars

main: XMLParser.o \
    XQuery.o \
	XMLService.o

XMLParser.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c XMLParser.c -o $(OBJECT)/XMLParser.o

XQuery.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c XQuery.c -o $(OBJECT)/XQuery.o

XMLService.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c XMLService.c -o $(OBJECT)/XMLService.o
