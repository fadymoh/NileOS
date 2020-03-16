include $(CODE_HOME)/makefile.vars

Debug: stage1.bin

cleanDebug:
	rm -rf $(BIN)/stage1.bin

stage1.bin: kernel.asm
	$(NASM) $(DEBUG) kernel.asm -f bin -o $(BIN)/stage1.bin
