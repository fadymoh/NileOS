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

void syscall(InterruptContext *p_interruptContext)
{

  DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
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

  DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);

  enablePageDirectory((uint64_t) & (kernel.coresPageTables_ptr[kernel.apicManager.returns.core_id]->PGD));
  int core_id = kernel.apicManager.returns.core_id;
  //printk("Starting up Core # %d\n", core_id);

  kernel.apicManager.params.p_start_stack = p_start_stack;
  kernel.apicManager.params.p_end_stack = p_end_stack;
  DispatchKernel(&kernel.service_transporter, apic_t, move_initial_kernel_stack);

  kernel.globalDescriptorTablePointer = &globalDescriptorTablePointer;
  initGlobalDescriptorTable();
  gdtInit();
  tssFlush();
  idtInit();

  DispatchKernel(&kernel.service_transporter, apic_t, touch_core);
  //printk(" ..... Done\n");

  int sleep_time = core_id * 150 + 500;

  // printk("Now switching to usermode\n");
  //switchToUserMode();
  // printk("Now in usermode\n");

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
  InitServiceTransportLayer(service_transporter);

  /*
     * ─── REGISTER SERVICES ───────────────────────────────────────────
     */

  PhysicalMemoryManager *physicalMemoryManager = &(kernel.physicalMemoryManager);
  service_init(&physicalMemoryService, (void *)physicalMemoryManager, physical_memory_t);
  bool statusOfPhysicalMemory = InitializePhysicalMemory(physicalMemoryManager, &physicalMemoryService);
  RegisterServiceToKernel(service_transporter, &physicalMemoryService, physical_memory_t);

  PCIService *pci_manager = &(kernel.pciService);
  service_init(&pciService, (void *)pci_manager, pci_t);
  initializePCIService(pci_manager, &pciService);
  RegisterServiceToKernel(service_transporter, &pciService, pci_t);

  // //Stage 1 Map
  PageMapStage1();

  Console *console = &(kernel.console);
  service_init(&consoleService, (void *)console, console_t); //
  console_preInialize(console, &consoleService);
  RegisterServiceToKernel(service_transporter, &consoleService, console_t);
  //DispatchKernel(&kernel.service_transporter, physical_memory_t, print_physical_memory);

  //collecting pci hardware
  DispatchKernel(&kernel.service_transporter, pci_t, collect_hw);
  int totalPCI_Devices = pci_manager->total_pci_devices;
  pci_manager->params.p_summary = true;

  //initializing physical memory bitmap
  DispatchKernel(&kernel.service_transporter, physical_memory_t, create_physical_pages_bitmap);
  DispatchKernel(&kernel.service_transporter, physical_memory_t, initialize_physical_memory_frame);

  /*
     * ─── INITIALIZING VIRTUAL MEMORY SERVICE ─────────────────────────
     */

  kernel.bootstrapCorePageTable_ptr = (MemoryPageTable *)kvalloc(&(kernel.memoryAllocator), sizeof(MemoryPageTable));
  MemoryPageTable *virtualMemory = kernel.bootstrapCorePageTable_ptr;
  service_init(&virtualMemoryService, (void *)virtualMemory, virtual_memory_t);
  InitVirtualMemoryService(&(kernel.bootstrapCorePageTable_ptr), &virtualMemoryService);
  RegisterServiceToKernel(service_transporter, &virtualMemoryService, virtual_memory_t);

  /*
     * ─── BUILDING AP CORES PAGE TABLES ────────────────────────────────────
     */

  enablePageDirectory((uint64_t) & (kernel.bootstrapCorePageTable_ptr->PGD));
  kernel.acpi.physicalMemoryManager = &kernel.physicalMemoryManager;

  if (init_acpi(&kernel.acpi))
  {
    MapReservedMemory(kernel.bootstrapCorePageTable_ptr);
    BuildApCoresPageTables(kernel.acpi.cores_count);

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
  initAPICManager(apicManager, &apicManagerService);
  RegisterServiceToKernel(service_transporter, &apicManagerService, apic_t);

  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ0 - IRQ0, IRQ0);

  initGlobalDescriptorTable();
  gdtInit();
  tssFlush();

  //printing pci devices
  kernel.pciService.params.p_summary = true;
  DispatchKernel(&kernel.service_transporter, pci_t, print_all_pci);

  InterruptManager *interruptManager = &(kernel.interruptManager);
  service_init(&interruptService, (void *)interruptManager, interruptManager_t);
  InitializeInterruptManager(interruptManager, &interruptService);
  RegisterServiceToKernel(service_transporter, &interruptService, interruptManager_t);

  printk("Finished Setting up IVT\n");

  //starting up APIC
  kernel.apicManager.params.p_start_stack = p_start_stack;
  kernel.apicManager.params.p_end_stack = p_end_stack;
  DispatchKernel(&kernel.service_transporter, apic_t, move_initial_kernel_stack);

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
  DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  kernel.interruptManager.params.p_interruptNumber = IRQ15;
  kernel.interruptManager.params.p_interruptHandler = ataHandleInterrupt;
  DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ14 - IRQ0, IRQ14);
  mapAPICIRQ(&kernel.apicManager.apics[0], IRQ15 - IRQ0, IRQ15);

  kernel.dmaBuffer.enabled = true;
  uint8_t *buffer = kmalloc(&kernel.memoryAllocator, sizeof(uint8_t) * 512 * sectors + 2);
  init_BlockService();
  //read_blocks(kernel.ataManager.ataDisks[0], buffer);

  //printk("start reading\n");
  //readDMADisk(kernel.ataManager.ataDisks[0], 52825, buffer, sectors, 0, 0);

  // for (int i = 0; i < 512*75776; ++i)
  //   printk("%c",buffer[i]);
  printk("\nDONE!\n");

  //TODO: end ata trial 1

  //syscall interrupt registering
  kernel.interruptManager.params.p_interruptNumber = 0x80;
  kernel.interruptManager.params.p_interruptHandler = syscall;
  DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  Ipi *ipi_manager = &kernel.ipiManager;
  service_init(&ipiService, (void *)ipi_manager, ipi_t);
  InitializeIPI(ipi_manager, &ipiService);
  RegisterServiceToKernel(service_transporter, &ipiService, ipi_t);

  // printk("Core 0 is now about to wake up core 1\n");
  // ipi_manager->params.receiverCore_id = 1;
  // ipi_manager->params.p_irq = WAKEUP_IPI;
  // DispatchKernel(&kernel.service_transporter, ipi_t, send_ipi);
  // printk("Core 0 done sending wake up interrupt\n");

  kernel.interruptManager.params.p_interruptNumber = 0;
  kernel.interruptManager.params.p_interruptHandler = zero_handler;
  DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  kernel.interruptManager.params.p_interruptNumber = 13;
  kernel.interruptManager.params.p_interruptHandler = fault_handler;
  DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

  // userModeDemo();

  generateServices();
  service1 *s1 = ((service1 *)kernel.void_pointers[1]);

  printk_debug("printing values before dispatching service functions: X: %d, Y: %d\n", s1->x, s1->y);
  DispatchKernel(&kernel.service_transporter, service1_t, void_method2);
  printk_debug("printing values after dispatching service functions: X: %d, Y: %d\n", s1->x, s1->y);

  SharedMemory *sharedMemory = &(kernel.sharedMemory);
  service_init(&sharedMemoryService, (void *)sharedMemory, sharedMemory_t);
  initSharedMemoryService(sharedMemory, &sharedMemoryService);
  RegisterServiceToKernel(service_transporter, &sharedMemoryService, sharedMemory_t);

  enableAPICTimer(&kernel.apicManager.apics[0]);
  DispatchKernel(&kernel.service_transporter, apic_t, startup_APIC);

  DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
  printk(" THIS IS CORE #%d\n", kernel.apicManager.returns.core_id);

  printk("Done executing\n");

  keyboard_init();

  rtl8139_init();

  // e1000Scan();

  // e1000StartLink((E1000 *)kernel.e1000->driver);

  // while (kernel.apicManager.apics[0].pit_counter / 100 <= 6)
  //   ;

  // printk("It is 6\n");
  // e1000StartLink((E1000 *)kernel.e1000->driver);

  // kernel.ipiManager.params.receiverCore_id = 0;
  // kernel.ipiManager.params.p_irq = 11 + 32;
  // DispatchKernel(&kernel.service_transporter, ipi_t, send_ipi);
}

void userModeDemo()
{
  switchToUserMode();
  asm volatile("hlt;");
}