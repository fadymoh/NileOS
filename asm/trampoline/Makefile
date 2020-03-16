include $(CODE_HOME)/makefile.vars

Debug: trampoline.bin

cleanDebug:
	rm -rf $(BIN)/trampoline.bin

trampoline.bin: trampoline.asm
	$(NASM) $(DEBUG) trampoline.asm -f bin -o $(BIN)/trampoline.bin

