include $(CODE_HOME)/makefile.vars

main: MemoryUtilities_asm.o \
        memcpy_asm.o \
		memswap_asm.o \
        checksum_asm.o
	
MemoryUtilities_asm.o: MemoryUtilities.asm
	$(NASM) $(DEBUG) MemoryUtilities.asm -felf64 -o $(OBJECT)/MemoryUtilities_asm.o

memcpy_asm.o: memcpy.asm
	$(NASM) $(DEBUG) memcpy.asm -felf64 -o $(OBJECT)/memcpy_asm.o

memswap_asm.o: memswap.asm
	$(NASM) $(DEBUG) memswap.asm -felf64 -o $(OBJECT)/memswap_asm.o

checksum_asm.o: memswap.asm
	$(NASM) $(DEBUG) checksum.asm -felf64 -o $(OBJECT)/checksum_asm.o
