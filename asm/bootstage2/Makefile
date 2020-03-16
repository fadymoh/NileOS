include $(CODE_HOME)/makefile.vars

Debug: bootstage2.bin

cleanDebug:
	rm -rf $(BIN)/bootstage2.bin

bootstage2.bin: boot.asm
	$(NASM) $(DEBUG) boot.asm -f bin -o $(BIN)/bootstage2.bin

