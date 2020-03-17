#include "Kernel.h"
#include "memcpy.h"
#include "memswap.h"
#include "stdio.h"
#include "printk.h"
#include "checksum.h"
#include "Library.h"
#include "rtl8139.h"

#define sectors 4096
Kernel kernel;
InterruptDescriptorTablePointer interruptDescriptorTablePointer;

Service consoleService;
Service physicalMemoryService;
Service pciService;
Service virtualMemoryService;
Service apicManagerService;
Service interruptService;
Service ipiService;
Service sharedMemoryService;

GlobalDescriptorTablePointer globalDescriptorTablePointer;

extern void gdtInit();
extern void tssFlush();
extern void idtInit();
void userModeDemo();

#define ptl1 0b0000000000000000111111111000000000000000000000000000000000000000
#define ptl2 0b0000000000000000000000000111111111000000000000000000000000000000
#define ptl3 0b0000000000000000000000000000000000111111111000000000000000000000
#define ptl4 0b0000000000000000000000000000000000000000000111111111000000000000

#define clr_offset4 0b1111111111111111111111111111111111111111111111111111000000000000
#define clr_offset 0b1111111111111111111111111111111111111111111000000000000000000000
#define set_offset 0b0000000000000000000000000000000000000000000111111111111111111111

void syscall(InterruptContext *p_interruptContext)
{

  dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
  int core_id = kernel.apicManager.returns.core_id;

  if (p_interruptContext->interrupt_number == 0x80)
  {
    printk("We're now in syscall, interrupt: %d,rax: %d,rsi: %d , rdi: %d, core: %d\n", p_interruptContext->interrupt_number, p_interruptContext->rax, p_interruptContext->rsi, p_interruptContext->rdi, core_id);
    pit_sleep(p_interruptContext->rdi, p_interruptContext->rsi, WAKEUP_IPI, true);
  }
}

void zero_handler(InterruptContext *p_interruptContext)
{
  if (p_interruptContext->interrupt_number == 0)
  {
    printk("Interrupt: division by zero\n");

    asm volatile("hlt;");
  }
}

void fault_handler(InterruptContext *p_interruptContext)
{
  if (p_interruptContext->interrupt_number == 13)
  {
    printk("Fault went on!\n");

    asm volatile("hlt;");
  }
}

extern void ap_kernel_main(uint64_t p_start_stack, uint64_t p_end_stack)
{
  enablePageDirectory((uint64_t) & (kernel.bootstrapCorePageTable_ptr->PGD));

  dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);

  enablePageDirectory((uint64_t) & (kernel.coresPageTables_ptr[kernel.apicManager.returns.core_id]->PGD));
  int core_id = kernel.apicManager.returns.core_id;
  //printk("Starting up Core # %d\n", core_id);

  kernel.apicManager.params.p_start_stack = p_start_stack;
  kernel.apicManager.params.p_end_stack = p_end_stack;
  dispatch_kernel(&kernel.service_transporter, apic_t, moveInitialKernelStack_s);

  kernel.globalDescriptorTablePointer = &globalDescriptorTablePointer;
  initGlobalDescriptorTable();
  gdtInit();
  tssFlush();
  idtInit();

  dispatch_kernel(&kernel.service_transporter, apic_t, touchCore_s);
  //printk(" ..... Done\n");

  // uint64_t* xxxxx = (uint64_t*) (START_OFFSET);
  // *xxxxx = 91010921;

  // uint64_t pgd_index = START_OFFSET >> (48 - 9);
  // uint64_t pud_index = (START_OFFSET >> (48 - 18)) & 0b0000000111111111;
  // uint64_t pmd_index = (START_OFFSET >> (48 - 27)) & 0b0000000111111111;
  // uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;
  // TablePage *TP = (TablePage *)&(kernel.coresPageTables_ptr[core_id]->PMD_ptr[idx]);
  // uint64_t physical_memory_address = TP->tpage[pmd_index];

  // printk("In AP core %d, virtual add: %x,  physical add: %x\n", core_id, xxxxx, physical_memory_address);

  // printk("Shared Memory inside AP Core(%d): %d\n",core_id, *xxxxx);

  int sleep_time = core_id * 150 + 500;

  // printk("Now switching to usermode\n");
  switchToUserMode();
  // printk("Now in usermode\n");

  // printk("Core %d is about to sleep for %d...\n", core_id, sleep_time);
  uint64_t rax_val = 0; // SYSCALL ID FOR SLEEP
  uint64_t rdi_val = sleep_time;
  uint64_t rsi_val = 0;
  uint64_t ret;
  uint64_t others = 0;

  asm volatile("mov %2,%%r8\n"
               "mov %3,%%r9\n"
               "int $0x80"
               : "=a"(ret)
               : "0"(rax_val), "r"(others), "r"(others), "b"(others), "c"(others), "d"(others), "S"(rsi_val), "D"(rdi_val));

  // printk("Core %d is now up again...\n", core_id);

  //  printk("Core %d is about to sleep for %d...\n", core_id, sleep_time);
  // pit_sleep(sleep_time, 0 ,WAKEUP_IPI, true);
  // printk("Core %d is now up again...\n", core_id);
  while (1)
    ;
}

extern void bsp_kernel_main(uint64_t p_start_stack, uint64_t p_end_stack)
{
  init_fast_memcpy_handlers();
  init_fast_memswap_handlers();
  service_transport_kernel *service_transporter = &(kernel.service_transporter);
  init_service_transport(service_transporter);

  /*
     * ─── REGISTER SERVICES ───────────────────────────────────────────
     */
  Console *console = &(kernel.console);
  service_init(&consoleService, (void *)console, console_t); //
  console_preInialize(console, &consoleService);
  register_service_to_kernel(service_transporter, &consoleService, console_t);

  PhysicalMemoryManager *physicalMemoryManager = &(kernel.physicalMemoryManager);
  service_init(&physicalMemoryService, (void *)physicalMemoryManager, physical_memory);
  bool statusOfPhysicalMemory = initializePhysicalMemory(physicalMemoryManager, &physicalMemoryService);
  register_service_to_kernel(service_transporter, &physicalMemoryService, physical_memory);

  PCIService *pci_manager = &(kernel.pciService);
  service_init(&pciService, (void *)pci_manager, pci_t);
  initializePCIService(pci_manager, &pciService);
  register_service_to_kernel(service_transporter, &pciService, pci_t);

  //printing physical memory
  if (!statusOfPhysicalMemory)
  {
    printk("status of physical memory is false\n");
    return;
  }
  dispatch_kernel(&kernel.service_transporter, physical_memory, PrintPhysicalMemory);

  //Stage 1 Map
  pageMapStage1();

  //collecting pci hardware
  dispatch_kernel(&kernel.service_transporter, pci_t, COLLECT_HW);
  int totalPCI_Devices = pci_manager->total_pci_devices;
  pci_manager->params.p_summary = true;

  //initializing physical memory bitmap
  dispatch_kernel(&kernel.service_transporter, physical_memory, CreatePhysicalPagesBitMap);
  dispatch_kernel(&kernel.service_transporter, physical_memory, InitializePhysicalMemoryFrame);

  /*
     * ─── INITIALIZING VIRTUAL MEMORY SERVICE ─────────────────────────
     */

  kernel.bootstrapCorePageTable_ptr = (MemoryPageTable *)kvalloc(&(kernel.memoryAllocator), sizeof(MemoryPageTable));
  MemoryPageTable *virtualMemory = kernel.bootstrapCorePageTable_ptr;
  service_init(&virtualMemoryService, (void *)virtualMemory, virtual_memory);
  initVirtualMemoryService(&(kernel.bootstrapCorePageTable_ptr), &virtualMemoryService);
  register_service_to_kernel(service_transporter, &virtualMemoryService, virtual_memory);

  /*
     * ─── BUILDING AP CORES PAGE TABLES ────────────────────────────────────
     */

  enablePageDirectory((uint64_t) & (kernel.bootstrapCorePageTable_ptr->PGD));
  kernel.acpi.physicalMemoryManager = &kernel.physicalMemoryManager;
  if (init_acpi(&kernel.acpi))
  {
    mapReservedMemory(kernel.bootstrapCorePageTable_ptr);
    buildApCoresPageTables(kernel.acpi.cores_count);
    kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.rsdpDescriptor->xsdtAddr;
    kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.rsdpDescriptor->rsdtAddr;
    kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.local_apic_addr;
    kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.local_apicio_addr;
  }
  else
  {
    return;
  }

  // allocate memory for each core's GDT
  kernel.globalDescriptorTable = (GlobalDescriptorTable *)kmalloc(&kernel.memoryAllocator, sizeof(GlobalDescriptorTable) * kernel.acpi.cores_count);
  kernel.globalDescriptorTablePointer = &globalDescriptorTablePointer;

  /*
 * ─── INITIALIZE APIC MANAGER SERVICE ────────────────────────────────────────────
 */

  APICManager *apicManager = &(kernel.apicManager);
  service_init(&apicManagerService, (void *)apicManager, apic_t);
  kernel.apicManager.params.p_apics_count = kernel.acpi.cores_count;
  initAPICManager_service(apicManager, &apicManagerService);
  register_service_to_kernel(service_transporter, &apicManagerService, apic_t);

  //Initialize GDT  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ13 - IRQ0, IRQ13);
  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ0 - IRQ0, IRQ0);

  initGlobalDescriptorTable();
  gdtInit();
  tssFlush();

  //printing pci devices
  kernel.pciService.params.p_summary = true;
  dispatch_kernel(&kernel.service_transporter, pci_t, PCI_SERVICE_PRINT);

  InterruptManager *interruptManager = &(kernel.interruptManager);
  service_init(&interruptService, (void *)interruptManager, interruptManager_t);
  initializeInterruptManager(interruptManager, &interruptService);
  register_service_to_kernel(service_transporter, &interruptService, interruptManager_t);

  printk("Finished Setting up IVT\n");

  //starting up APIC
  kernel.apicManager.params.p_start_stack = p_start_stack;
  kernel.apicManager.params.p_end_stack = p_end_stack;
  dispatch_kernel(&kernel.service_transporter, apic_t, moveInitialKernelStack_s);
  //TODO:: ata trial 1startupAPIC_s
  initATAManager();
  uint8_t ata_disks = detectATADisks();
  printk("Number of ATA Disks Detected: %d\n", ata_disks);

  // uint8_t buffer[512*6+ 2];
  // readPIODiskSectors(kernel.ataManager.ataDisks[0], 52825, buffer, 6);
  // buffer[512*6 + 1] = 0;

  // printk(buffer);

  // for (int i = 0; i < 512; ++i)
  // {
  //   printk("%c", (char)buffer[i]);
  // }

  kernel.interruptManager.params.p_interruptNumber = IRQ14;
  kernel.interruptManager.params.p_interruptHandler = ataHandleInterrupt;
  dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  kernel.interruptManager.params.p_interruptNumber = IRQ15;
  kernel.interruptManager.params.p_interruptHandler = ataHandleInterrupt;
  dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ14 - IRQ0, IRQ14);
  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ15 - IRQ0, IRQ15);
  // printk("Hooked ATA IRQ Interrupt Handlers\n");
  // printk("dma physical address 1        %x\n", kernel.ataManager.ataDisks[0]->dma_phy_address);

  kernel.dmaBuffer.enabled = true;
  uint8_t *buffer = kmalloc(&kernel.memoryAllocator, sizeof(uint8_t) * 512 * sectors + 2);
  init_BlockService();
  //read_blocks(kernel.ataManager.ataDisks[0], buffer);

  //printk("start reading\n");
  //readDMADisk(kernel.ataManager.ataDisks[0], 52825, buffer, sectors, 0, 0);

  // printk("ana hena mstanya interrupt yegi f hktb paragraph taweel 3ala ad m2dar 7ata");

  buffer[512 * sectors + 1] = 0;
  // for (int i = 0; i < 512*75776; ++i)
  //   printk("%c",buffer[i]);
  printk("\nDONE!\n");

  //TODO: end ata trial 1

  //syscall interrupt registering
  kernel.interruptManager.params.p_interruptNumber = 0x80;
  kernel.interruptManager.params.p_interruptHandler = syscall;
  dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  Ipi *ipi_manager = &kernel.ipiManager;
  service_init(&ipiService, (void *)ipi_manager, ipi_t);
  InitializeIPI(ipi_manager, &ipiService);
  register_service_to_kernel(service_transporter, &ipiService, ipi_t);

  // printk("Core 0 is now about to wake up core 1\n");
  // ipi_manager->params.receiverCore_id = 1;
  // ipi_manager->params.p_irq = WAKEUP_IPI;
  // dispatch_kernel(&kernel.service_transporter, ipi_t, send_ipi);
  // printk("Core 0 done sending wake up interrupt\n");

  kernel.interruptManager.params.p_interruptNumber = 0;
  kernel.interruptManager.params.p_interruptHandler = zero_handler;
  dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  kernel.interruptManager.params.p_interruptNumber = 13;
  kernel.interruptManager.params.p_interruptHandler = fault_handler;
  dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  // userModeDemo();

  //  generateServices();
  //  service1* s1 = ((service1*) kernel.void_pointers[1]);

  //  printk("X: %d, Y: %d\n", s1->x, s1->y);
  //  dispatch_kernel(&kernel.service_transporter, service1_t, void_method2);
  //  printk("X: %d, Y: %d\n", s1->x, s1->y);

  // int volatile x = 1;
  // int volatile y = 0;
  // printk("Division By Zero: %d\n", x/y);

  SharedMemory *sharedMemory = &(kernel.sharedMemory);
  service_init(&sharedMemoryService, (void *)sharedMemory, sharedMemory_t);
  initSharedMemoryService(sharedMemory, &sharedMemoryService);
  register_service_to_kernel(service_transporter, &sharedMemoryService, sharedMemory_t);

  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 9;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);

  // uint64_t addr = kernel.sharedMemory.returns_t.virtualAddress;
  // uint64_t *ptr = (uint64_t *)(addr);
  // *ptr = 59862312;

  // uint64_t pgd_index = addr >> (48 - 9);
  // uint64_t pud_index = (addr >> (48 - 18)) & 0b0000000111111111;
  // uint64_t pmd_index = (addr >> (48 - 27)) & 0b0000000111111111;
  // uint64_t idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;
  // TablePage *TP = (TablePage *)&(kernel.coresPageTables_ptr[0]->PMD_ptr[idx]);
  // uint64_t physical_memory_address = TP->tpage[pmd_index];

  // printk("In Boostrap core, virtual add: %x, physical add: %x\n", ptr, physical_memory_address);

  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 3;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);

  // addr = kernel.sharedMemory.returns_t.virtualAddress;
  // ptr = (uint64_t *)(addr);
  // *ptr = 4928104192;

  // pgd_index = addr >> (48 - 9);
  // pud_index = (addr >> (48 - 18)) & 0b0000000111111111;
  // pmd_index = (addr >> (48 - 27)) & 0b0000000111111111;
  // idx = pgd_index * PAGE_TABLE_UNIT_SIZE + pud_index;
  // TP = (TablePage *)&(kernel.coresPageTables_ptr[0]->PMD_ptr[idx]);
  // physical_memory_address = TP->tpage[pmd_index];

  // printk("In Boostrap core, virtual add: %x, physical add: %x\n", ptr, physical_memory_address);

  //enableAPICTimer(&kernel.apicManager.apics[0]);
  //dispatch_kernel(&kernel.service_transporter, apic_t, startupAPIC_s);

  dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
  printk(" THIS IS CORE #%d\n", kernel.apicManager.returns.core_id);

  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 5;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);

  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 2;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);

  // uint64_t addr = kernel.sharedMemory.returns_t.virtualAddress;
  // uint64_t *ptr = (uint64_t *)(addr);
  // *ptr = 1;

  // kernel.sharedMemory.params_t.virtualAddress = kernel.sharedMemory.returns_t.virtualAddress;
  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 2;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, deallocateSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);
  // *ptr = 99;

  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 9;
  // kernel.sharedMemory.params_t.virtualAddress = START_OFFSET;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, deallocateSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);
  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 5;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);
  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 5;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);
  // kernel.sharedMemory.params_t.numberOfBytes = 2097152 * 4;
  // dispatch_kernel(&kernel.service_transporter, sharedMemory_t, allocatedSharedMemory_t);
  // printk("return address: %x\n", kernel.sharedMemory.returns_t.virtualAddress);

  // printk("Done executing\n");

 rtl8139_init();
}

void userModeDemo()
{
  switchToUserMode();
  asm volatile("hlt;");
}