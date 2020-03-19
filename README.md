# Nile Operating System

NileOS is a distributed asymmetric tickless operating system for big data processing. It is based on research conducted by Dr. Karim Sobh, and Dr. Amr Elkadi at the American University in Cairo.

# Plan
#### Mem management  
&#160; &#160; &#160; &#160;1 physical memory management [✔]  
&#160; &#160; &#160; &#160;2 virtual memory management [✔]  
&#160; &#160; &#160; &#160;3 kmalloc [✔]
&#160; &#160; &#160; &#160;4 shared memory service [✔]
&#160; &#160; &#160; &#160;5 Heap service [x]

#### Filesystem  
&#160; &#160; &#160; &#160;1 ATA/DMA Driver PIO Mode [✔]  
&#160; &#160; &#160; &#160;2 ATA/DMA Driver DMA Mode [✔]  
&#160; &#160; &#160; &#160;3 Block Service [x]  

#### Miscellaneous  
&#160; &#160; &#160; &#160;1 Usermode [✔]  
&#160; &#160; &#160; &#160;2 spinlock [✔]  
&#160; &#160; &#160; &#160;3 Systemcalls [✔]  
&#160; &#160; &#160; &#160;4 Real time clock [✔]  
&#160; &#160; &#160; &#160;5 Keyboard Driver [✔]  
&#160; &#160; &#160; &#160;6 Multi-Terminal Console [✔]  
&#160; &#160; &#160; &#160;7 Multi-Core Setup [✔]  
&#160; &#160; &#160; &#160;8 Have a shell [x]  
&#160; &#160; &#160; &#160;9 Roles for the cores based on configuration [x]  

#### Network protocols  
&#160; &#160; &#160; &#160;1 RTL8139 Driver  [✔]  
&#160; &#160; &#160; &#160;2 E1000  [x]  
&#160; &#160; &#160; &#160;3 IP [x] 
&#160; &#160; &#160; &#160;4 ARP [x]  
&#160; &#160; &#160; &#160;5 Ping [x]  
&#160; &#160; &#160; &#160;6 IPIOE [x]  

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
