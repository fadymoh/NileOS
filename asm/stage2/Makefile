include $(CODE_HOME)/makefile.vars

main: subsystems \
	KernelWrapper.o 
	
KernelWrapper.o: KernelWrapper.asm
	$(NASM) $(DEBUG) KernelWrapper.asm -felf64 -o $(OBJECT)/KernelWrapper.o

subsystems:
	$(MAKE) -C misc CODE_HOME=$(CODE_HOME)
	$(MAKE) -C gdt CODE_HOME=$(CODE_HOME)
	$(MAKE) -C interrupts CODE_HOME=$(CODE_HOME)
	