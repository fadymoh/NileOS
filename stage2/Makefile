include $(CODE_HOME)/makefile.vars

KernelMain.bin: KernelMain.elf KernelMain.map
	$(OBJCOPY) -R .note -R .comment -S -O binary $(ELF)/KernelMain.elf $(BIN)/KernelMain.bin

KernelMain.o: sources/KernelMain.c
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c sources/KernelMain.c -o $(OBJECT)/KernelMain.o

KernelMain.elf: subsystems KernelMain.o 
	$(GCC) $(DEBUG) $(ELF_FLAGS) $(INCLUDES) -o $(ELF)/KernelMain.elf $(OBJECT)/KernelMain.o

KernelMain.map: subsystems KernelMain.o 
	$(LD) $(DEBUG) $(LINKER_FILE) $(INCLUDES) --print-map  $(OBJECT)/KernelMain.o -o /dev/null > $(MAP)/KernelMain.map

subsystems:
	# MISC IS NEEDED
	$(MAKE) -C sources/misc CODE_HOME=$(CODE_HOME) 
	# NEEDED FOR Kernel.h
	$(MAKE) -C sources/memory CODE_HOME=$(CODE_HOME)

	# DEVICES IS NEEDED FOR "TERMINAL AND CONSOLE"
	$(MAKE) -C sources/devices CODE_HOME=$(CODE_HOME)
	
	# NEEDED FOR Kernel.h
	$(MAKE) -C sources/kernel CODE_HOME=$(CODE_HOME)

	# NEEDED FOR Kernel.h
	$(MAKE) -C sources/sync CODE_HOME=$(CODE_HOME)

	$(MAKE) -C sources/interrupts CODE_HOME=$(CODE_HOME)

	$(MAKE) -C sources/ipi CODE_HOME=$(CODE_HOME)
	$(MAKE) -C sources/xml CODE_HOME=$(CODE_HOME)
	$(MAKE) -C sources/dummyServs CODE_HOME=$(CODE_HOME)

