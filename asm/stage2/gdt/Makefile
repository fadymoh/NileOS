include $(CODE_HOME)/makefile.vars

main: gdt.o 
	
gdt.o: gdt.asm
	$(NASM) $(DEBUG) gdt.asm -felf64 -o $(OBJECT)/gdt.o


