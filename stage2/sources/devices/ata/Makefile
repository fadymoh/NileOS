include $(CODE_HOME)/makefile.vars

main: ATADisk.o \
        ATAManager.o
	
ATADisk.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ATADisk.c -o $(OBJECT)/ATADisk.o
    
ATAManager.o: 
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c ATAManager.c -o $(OBJECT)/ATAManager.o    

