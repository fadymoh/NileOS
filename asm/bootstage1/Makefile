include $(CODE_HOME)/makefile.vars

Debug: bootstage1.bin

cleanDebug:
	rm -rf $(BIN)/bootstage1.bin

bootstage1.bin: boot.asm
	$(NASM) $(DEBUG) boot.asm -f bin -o $(BIN)/bootstage1.bin

