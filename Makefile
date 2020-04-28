include makefile.vars

Debug: boot.flp

clean:
	rm -rf $(BIN)/* $(OBJECT)/* $(ELF)/* $(IMAGE)/*

cleanObjects:
	rm -rf $(OBJECT)/*

cleanElfs:
	rm -rf $(ELF)/*
	
cleanBins:
	rm -rf $(BIN)/*

cleanDebug:
	rm -rf $(BIN)/* $(OBJECT)/* $(ELF)/* $(IMAGE)/*

runvm: boot.flp
	#qemu-system-x86_64 -m 8192 -smp 4 -hda $(IMAGE)/boot.flp -net nic,model=rtl8139,macaddr=52:54:00:91:46:20
	qemu-system-x86_64 -m 4096 -smp 2 -hda $(IMAGE)/boot.flp 
#-net dump -netdev tap,helper=/usr/lib/qemu/qemu-bridge-helper,id=simpleos_net -device e1000,netdev=simpleos_net,id=simpleos_nic
	#qemu-system-x86_64 -m 8192 -smp 4 -hda $(IMAGE)/boot.flp -net nic,vlan=0,model=rtl8139,macaddr=52:54:00:91:46:f2 -net tap,ifname=virbr0 --enable-kvm

runvbox: boot.flp
	VBoxManage storageattach "NileOS" --storagectl "IDE" --device 0 --port 0 --type hdd --medium none
	# /Applications/VirtualBox.app/Contents/MacOS/VBoxManage storageattach "NileOS" --storagectl "IDE" --device 1 --port 0 --type hdd --medium none
#	/Applications/VirtualBox.app/Contents/MacOS/VBoxManage storageattach "NileOS" --storagectl "IDE" --device 0 --port 1 --type hdd --medium none
	VBoxManage closemedium disk $(IMAGE)/boot.vdi
#	/Applications/VirtualBox.app/Contents/MacOS/VBoxManage closemedium disk $(IMAGE)/text.vdi
	# /Applications/VirtualBox.app/Contents/MacOS/VBoxManage closemedium disk $(IMAGE)/data.vdi
	VBoxManage storageattach "NileOS" --storagectl "IDE" --device 0 --port 0 --type hdd --medium $(IMAGE)/boot.vdi
	# /Applications/VirtualBox.app/Contents/MacOS/VBoxManage storageattach "NileOS" --storagectl "IDE" --device 1 --port 0 --type hdd --medium $(IMAGE)/data.vdi
#	/Applications/VirtualBox.app/Contents/MacOS/VBoxManage storageattach "NileOS" --storagectl "IDE" --device 0 --port 1 --type hdd --medium $(IMAGE)/data.vdi
	VBoxManage startvm "NileOS" 


boot.flp: subsystems 
	$(CAT) $(BIN)/bootstage1.bin $(BIN)/bootstage2.bin $(BIN)/stage1.bin $(BIN)/trampoline.bin /dev/zero | $(DD) bs=512 count=61440 of=$(IMAGE)/boot.flp
	# $(DD) if=$(IMAGE)/ramdisk.img bs=512 seek=25 of=$(IMAGE)/boot.flp conv=notrunc,noerror
	$(DD) if=$(BIN)/KernelMain.bin bs=512 seek=51225 of=$(IMAGE)/boot.flp conv=notrunc,noerror
	$(DD) if=$(BIN)/xmltest.txt bs=512 seek=52825 of=$(IMAGE)/boot.flp conv=notrunc,noerror
	# $(DD) if=$(BIN)/bosml.bin bs=512 seek=52825 of=$(IMAGE)/boot.flp conv=notrunc,noerror
	$(DD) if=/dev/zero bs=1 count=1 of=$(IMAGE)/boot.flp conv=sync seek=122007551
	cp $(IMAGE)/boot.flp $(IMAGE)/boot.raw
	rm -rf $(IMAGE)/boot.vdi
	VBoxManage convertdd $(IMAGE)/boot.raw $(IMAGE)/boot.vdi --format VDI
	chmod a+r $(IMAGE)/boot.vdi
	chown "$(whoami)" $(IMAGE)/boot.vdi

auc: subsystems auc_ramdisk
	$(CAT) $(BIN)/bootstage1.bin $(BIN)/bootstage2.bin $(BIN)/stage1.bin $(BIN)/trampoline.bin /dev/zero | $(DD) bs=512 count=61440 of=$(IMAGE)/boot.flp
	$(DD) if=$(IMAGE)/ramdisk.img bs=512 seek=25 of=$(IMAGE)/boot.flp conv=notrunc,noerror
	$(DD) if=$(BIN)/KernelMain.bin bs=512 seek=51225 of=$(IMAGE)/boot.flp conv=notrunc,noerror
	cp $(IMAGE)/boot.flp $(IMAGE)/boot.raw
	rm -rf $(IMAGE)/boot.vdi
	/usr/bin/VBoxManage convertdd $(IMAGE)/boot.raw $(IMAGE)/boot.vdi --format VDI
	chmod a+r $(IMAGE)/boot.vdi
	chown kmsobh:kmsobh $(IMAGE)/boot.vdi



subsystems:
	# Andy
	$(MAKE) -C preprocessor CODE_HOME=$(CODE_HOME) 
	$(MAKE) -C asm/bootstage1 CODE_HOME=$(CODE_HOME)
	$(MAKE) -C asm/bootstage2 CODE_HOME=$(CODE_HOME)
	$(MAKE) -C asm/stage1 CODE_HOME=$(CODE_HOME)
	$(MAKE) -C asm/stage2 CODE_HOME=$(CODE_HOME)
	$(MAKE) -C asm/trampoline CODE_HOME=$(CODE_HOME)
	# $(MAKE) -C asm/bosml CODE_HOME=$(CODE_HOME)
	$(MAKE) -C preprocessor/output CODE_HOME=$(CODE_HOME)
	
	$(MAKE) -C stage2 CODE_HOME=$(CODE_HOME)
	# $(MAKE) -C bosml CODE_HOME=$(CODE_HOME)
	


ramdisk:
	/home/kmsobh/NetBeansProjects/RamDiskBuilder/dist/Debug/GNU-Linux-x86/ramdiskbuilder $(CODE_HOME)/rootfs $(IMAGE)/ramdisk.img

auc_ramdisk:
	/home/kmsobh/NetBeansProjects/RamDiskBuilder/dist/Debug/GNU-Linux-x86/ramdiskbuilder $(CODE_HOME)/aucfs/rootfs $(IMAGE)/ramdisk.img

cc_ramdisk:
	/home/kmsobh/NetBeansProjects/RamDiskBuilder/dist/Debug/GNU-Linux-x86/ramdiskbuilder $(CODE_HOME)/ccfs/rootfs $(IMAGE)/ramdisk.img

disk_images:
	qemu-img create -f qcow2 $(IMAGE)/hdd1.qcow2 10G
	qemu-img create -f qcow2 $(IMAGE)/hdd2.qcow2 20G
	qemu-img create -f qcow2 $(IMAGE)/hdd3.qcow2 30G
	qemu-img create -f raw $(IMAGE)/bochs_hdd1.img 1G
	qemu-img create -f raw $(IMAGE)/bochs_hdd2.img 2G
	qemu-img create -f raw $(IMAGE)/bochs_hdd3.img 3G

kernel.iso: boot.flp
	mkdir -p ./cdiso/
	rm -rf $(IMAGE)/kernel.iso
	cp $(IMAGE)/boot.flp ./cdiso/
	$(MKISOFS) -hard-disk-boot -b boot.flp -o $(IMAGE)/kernel.iso ./cdiso/
