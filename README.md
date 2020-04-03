# Nile Operating System

NileOS is a distributed asymmetric tickless operating system for big data processing. It is based on research conducted by Dr. Karim Sobh, and Dr. Amr Elkadi at the American University in Cairo.

# Plan
#### Mem management  
- [x] physical memory management  
- [x] virtual memory management  
- [x] shared memory service  
- [x] kmalloc  
- [ ] Heap service  

#### Filesystem  
- [x] ATA/DMA Driver PIO Mode  
- [x] ATA/DMA Driver DMA Mode  
- [x] Block Service  

#### Miscellaneous  
- [x] Usermode  
- [x] spinlock  
- [x] Systemcalls  
- [x] Real time clock  
- [x] Keyboard Driver  
- [x] Multi-Terminal Console  
- [x] Multi-Core Setup  
- [ ] Have a shell  
- [ ] Roles for the cores based on configuration  

#### Network protocols  
- [x] RTL8139 Driver  
- [ ] E1000  
- [ ] IP  
- [ ] ARP 
- [ ] Ping  
- [ ] IPIOE  

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

Make sure to first have
- x86_64-elf-gcc cross compiler (The target being x86_64-elf-gcc)
- Qemu
- VirtualBox
- NASM
- Qemu Bridge Helper

What things you need to do to get NileOS running
```shell
sudo brctl addbr br0
sudo cat > /etc/qemu/bridge.conf
allow br0
allow virbr0
Exit the cat command
sudo make runvm
```

## Authors

* **Ahmed El Rouby** [ahmadelrouby](https://github.com/ahmadelrouby)
* **Areej Mostafa**
* **Andrew Khalaf** [andrewkhalaf](https://github.com/andrewkhalaf)
* **Fady Mohamed** [fadymoh](https://github.com/fadymoh)
* **Nour Ghaly**

## Acknowledgments

* Doctor Karim Sobh, and Doctor Amr Elkadi were our advisors for the graduation project.
* OSDev website for the thorough explanations and wikis. (https://wiki.osdev.org)
