ENTRY(_start)
 
SECTIONS
 {
	.text BLOCK(4K) : ALIGN(4K)
	{		KernelWrapper.o (.text) /* This needs to be the first file for
                                            stage1 to be able to switch to
                                            stage2
 */	gdt.o (.text)
	idt.o (.text)
	isr.o (.text)
	pit.o (.text)
	PhysicalMemory.o (.text)
	MemoryUtilities_asm.o (.text)
	ClockUtils.o (.text)
	MemoryUtilities.o (.text)
	memcpy_asm.o (.text)
	memcpy.o (.text)
	memswap_asm.o (.text)
	checksum_asm.o (.text)
	pci_device.o (.text)
	pci_service.o (.text)
	memswap.o (.text)
	ipi.o (.text)
	Terminal.o (.text)
	Console.o (.text)
	ACPI.o (.text)
	APIC.o (.text)
	ATADisk.o (.text)
	ATAManager.o (.text)
	TSSDescriptor.o (.text)
	GlobalDescriptorTableEntry.o (.text)
	GlobalDescriptorTable.o (.text)
	InterruptManager.o (.text)
	InterruptDescriptorTable.o (.text)
	isr_stub.o (.text)
	APICManager.o (.text)
	Ports.o (.text)
	CoreUtils.o (.text)
	NetUtils.o (.text)
	MMIOUtils.o (.text)
	stdio.o (.text)
	printk.o (.text)
	service1_K.o (.text)
	checksum.o (.text)
	MemoryPageMap.o (.text)
	SharedMemory.o (.text)
	BlockService.o (.text)
	Service.o (.text)
	ServiceTransport.o (.text)
	numbers.o (.text)
	rtl8139.o (.text)
	Spinlock.o (.text)
	KMalloc.o (.text)
	E1000.o (.text)
	ARP.o (.text)
	IP.o (.text)
	ICMP.o (.text)
	Ethernet.o (.text)
	NetworkDriver.o (.text)
	keyboard.o (.text)
	void_ptr_stack.o (.text)
	XMLParser.o (.text)
	XQuery.o (.text)
	XMLService.o (.text)
	service1_K.o (.text)
	service1_U.o (.text)
	hamada_K.o (.text)
	hamada_U.o (.text)

	Kernel.o (.text)
		. = ALIGN(4K);
	}
		.rodata BLOCK(4K) : ALIGN(4K)
	{
	*(.rodata)
		. = ALIGN(4K);
	}
		.data BLOCK(4K) : ALIGN(4K)
	{
	*(.data)
		. = ALIGN(4K);
	}
		.bss BLOCK(4K) : ALIGN(4K)
	{
	*(COMMON)
	*(.bss)
	*(.bootstrap_stack)
		. = ALIGN(4K);
	}

	end = .;
}