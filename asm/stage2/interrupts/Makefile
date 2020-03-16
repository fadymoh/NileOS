include $(CODE_HOME)/makefile.vars

main: idt.o \
	isr.o 
	
idt.o: idt.asm
	$(NASM) $(DEBUG) idt.asm -felf64 -o $(OBJECT)/idt.o

isr.o: isr.asm
	$(NASM) $(DEBUG) isr.asm -felf64 -o $(OBJECT)/isr.o
