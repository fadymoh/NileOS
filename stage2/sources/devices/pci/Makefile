include $(CODE_HOME)/makefile.vars

main: pci_device.o \
	pci_service.o

pci_device.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c pci_device.c -o $(OBJECT)/pci_device.o

pci_service.o:
	$(GCC) $(DEBUG) $(GCC_FLAGS) $(INCLUDES) -c pci_service.c -o $(OBJECT)/pci_service.o
