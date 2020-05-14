#include "Kernel.h"
#include "memcpy.h"
#include "memswap.h"
#include "stdio.h"
#include "printk.h"
#include "checksum.h"
#include "Library.h"
#include "rtl8139.h"
#define sectors 2048
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
Service XMLParserService;
Service AckermannFunctionService;

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
  printk("Starting up Core # %d\n", core_id);

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

  //  printk("Now switching to usermode\n");
  // switchToUserMode();
  // printk("Now in usermode\n");
  // asm volatile("hlt;");

  //  userModeDemo();

  // uint64_t rax_val = 0; // SYSCALL ID FOR SLEEP
  // uint64_t rdi_val = sleep_time;
  // uint64_t rsi_val = 0;
  // uint64_t ret;
  // uint64_t others = 0;

  // asm volatile("mov %2,%%r8\n"
  //              "mov %3,%%r9\n"
  //              "int $0x80"
  //              : "=a"(ret)
  //              : "0"(rax_val), "r"(others), "r"(others), "b"(others), "c"(others), "d"(others), "S"(rsi_val), "D"(rdi_val));

  // printk("Core %d is now up again...\n", core_id);

  // printk("Core %d is about to sleep for %d...\n", core_id, sleep_time);
  //  pit_sleep(sleep_time, 0 ,WAKEUP_IPI, true);
  //  printk("Core %d is now up again...\n", core_id);
  if (core_id == 1)
  {
    while (1)
    {
      if (kernel.down_arrow == true)
      {
        uint8_t mac[6];
        mac[0] = 0x08;
        mac[1] = 0x00;
        mac[2] = 0x27;
        mac[3] = 0x24;
        mac[4] = 0x41;
        mac[5] = 0xb3;

        uint16_t sourceCore = 1;
        uint16_t destinationCore = 2;

        char dataToBeSend[100] = "<params><FirstValue> 3 </FirstValue><SecondValue> 12 </SecondValue><params>";
        uint16_t msgId = sendIPIoE_Packet(kernel.e1000, IPIoE_OPCODE_REQUEST, mac, destinationCore, sourceCore, ackermannFunctionService_t, AckFunction_t, dataToBeSend, strlen(dataToBeSend));
        printk("before wait\n");
        wait_IPIoE_messageId(msgId);
        printk("after wait\n");
        IPIoE_Queue_Entry *ipioe_p = workerDequeue(sourceCore);
        printk("returned XML: %s\n", ipioe_p->data);

        ParseXMLString(ipioe_p->data);
        Parameter *ret = &kernel.service_transporter.kernel_services[ackermannFunctionService_t]->testMethod[AckFunction_t].returns[0];

        kernel.xmlService.params.tagDirectory = ret->tag;

        getValueFromTagMessage(&kernel.xmlService);
        char *parsingResult = kernel.xmlService.returns.value;
        printk("Parsing Result: %s\nValue in Bytes: %d\n", parsingResult, strlen(parsingResult));
        uint64_t outLength = 0;
        uint64_t *decodedResult = base64_decode(parsingResult, strlen(parsingResult), &outLength);

        printk("The result taken from the xml is: %d\n", *decodedResult);
        kernel.down_arrow = false;
      }
      if (kernel.up_arrow == true)
      {
        uint8_t mac[6];
        mac[0] = 0x08;
        mac[1] = 0x00;
        mac[2] = 0x27;
        mac[3] = 0x0c;
        mac[4] = 0x1b;
        mac[5] = 0x93;

        uint16_t sourceCore = 1;
        uint16_t destinationCore = 2;






















        char dataToBeSend[100] = "<params><FirstValue> 3 </FirstValue><SecondValue> 12 </SecondValue><params>";
        uint16_t msgId = sendIPIoE_Packet(kernel.e1000, IPIoE_OPCODE_REQUEST, mac, 
                          destinationCore, sourceCore, ackermannFunctionService_t, AckFunction_t,
                          dataToBeSend, strlen(dataToBeSend));
        printk("before wait\n");
        wait_IPIoE_messageId(msgId);
        printk("after wait\n");
        IPIoE_Queue_Entry *ipioe_p = workerDequeue(sourceCore);
        printk("returned XML: %s\n", ipioe_p->data);

        ParseXMLString(ipioe_p->data);
        Parameter *ret = &kernel.service_transporter.kernel_services[ackermannFunctionService_t]
                          ->testMethod[AckFunction_t].returns[0];
        kernel.xmlService.params.tagDirectory = ret->tag;

        getValueFromTagMessage(&kernel.xmlService);
        char *parsingResult = kernel.xmlService.returns.value;
        printk("Parsing Result: %s\nValue in Bytes: %d\n", parsingResult, strlen(parsingResult));
        uint64_t outLength = 0;
        uint64_t *decodedResult = base64_decode(parsingResult, strlen(parsingResult), &outLength);

        printk("The result taken from the xml is: %d\n", *decodedResult);
        kernel.up_arrow = false;
      }
    }
  }














  if (core_id == 2)
  {
    while (1)
    {
      if (kernel.apicManager.apics[core_id].worker.queue_size == 0)
        wait_IPIoE();

      IPIoE_Queue_Entry *ipioe_p = workerDequeue(core_id);

      // DumpIPIoE_Packet(&ipioe_p->ipioe_p);

      // printk("WOKE UP.. Packet Coming from MAC Address: %y:%y:%y:%y:%y:%y\n",
      //        ipioe_p->smac[0],
      //        ipioe_p->smac[1],
      //        ipioe_p->smac[2],
      //        ipioe_p->smac[3],
      //        ipioe_p->smac[4],
      //        ipioe_p->smac[5]);
      // printk("WOKE UP.. Packet Going to MAC Address: %y:%y:%y:%y:%y:%y\n",
      //        ipioe_p->dmac[0],
      //        ipioe_p->dmac[1],
      //        ipioe_p->dmac[2],
      //        ipioe_p->dmac[3],
      //        ipioe_p->dmac[4],
      //        ipioe_p->dmac[5]);
      printk("Core %d received a new IPIoE\n", core_id);
      // Process the actual thing
      char *response = DispatchKernelTest(&kernel.service_transporter, ipioe_p->ipioe_p.serviceNumber, ipioe_p->ipioe_p.methodNumber, ipioe_p->data);

      uint16_t msgId = sendIPIoE_Packet(kernel.e1000, IPIoE_OPCODE_REQUEST, ipioe_p->smac, ipioe_p->ipioe_p.sourceCore, ipioe_p->ipioe_p.destinationCore, 10, 1, response, strlen(response));
    }
  }
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

  // Stage 1 Map
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

  Ipi *ipi_manager = &kernel.ipiManager;
  service_init(&ipiService, (void *)ipi_manager, ipi_t);
  InitializeIPI(ipi_manager, &ipiService);
  RegisterServiceToKernel(service_transporter, &ipiService, ipi_t);
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
  //!read from disk demo
  uint8_t *buffer = kmalloc(&kernel.memoryAllocator, sizeof(uint8_t) * 15 * sectors + 2);
  init_BlockService();
  //! main method to test
  kernel.blockService.start_sector = 52825;
  kernel.blockService.blocks_to_read_number = 10;
  read_blocks(kernel.ataManager.ataDisks[0], buffer);
  // kernel.blockService.blocks_to_read_number = 15;
  // kernel.blockService.start_sector = 52825 + 30 * sectors;
  read_blocks(kernel.ataManager.ataDisks[0], buffer);

  // Service 1 Function 1 // int
  //
  // int ret = DispatchKernel(st, srv, num, pointer_to_xml);
  // for (int i = 0; i < 512; ++i)
  // {
  //   printk("%c", (char)buffer[i]);
  // }
  printk("\nDONE!\n");
  //! end of demo
  //TODO: end ata trial 1

  //syscall interrupt registering
  kernel.interruptManager.params.p_interruptNumber = 0x80;
  kernel.interruptManager.params.p_interruptHandler = syscall;
  DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

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

  // generateServices();
  // service1 *s1 = ((service1 *)kernel.void_pointers[1]);

  // printk_debug("printing values before dispatching service functions: X: %d, Y: %d\n", s1->x, s1->y);
  // DispatchKernel(&kernel.service_transporter, service1_t, void_method2);
  // printk_debug("printing values after dispatching service functions: X: %d, Y: %d\n", s1->x, s1->y);

  // SharedMemory *sharedMemory = &(kernel.sharedMemory);
  // service_init(&sharedMemoryService, (void *)sharedMemory, sharedMemory_t);
  // initSharedMemoryService(sharedMemory, &sharedMemoryService);
  // RegisterServiceToKernel(service_transporter, &sharedMemoryService, sharedMemory_t);

  enableAPICTimer(&kernel.apicManager.apics[0]);
  DispatchKernel(&kernel.service_transporter, apic_t, startup_APIC);

  DispatchKernel(&kernel.service_transporter, apic_t, get_current_core_id);
  printk(" THIS IS CORE #%d\n", kernel.apicManager.returns.core_id);

  printk("Done executing\n");

  keyboard_init();

  //rtl8139_init();

  disableInterrupts();
  e1000Scan();
  enableInterrupts();
  printk("Finished Setting up the Network Driver!\n");

  XMLService *xmlservice = &(kernel.xmlService);
  service_init(&XMLParserService, (void *)xmlservice, xmlService_t);
  initializeXMLService(&XMLParserService);
  RegisterServiceToKernel(service_transporter, &XMLParserService, xmlService_t);

  // char path[256] = "cores/core[0]/Role";
  // xmlservice->params.ata_core_id = 0;
  // xmlservice->params.sectors_count_to_read = 1;
  // xmlservice->params.sectors_start = 52825;
  // xmlservice->params.tagDirectory = path;

  // DispatchKernel(service_transporter, xmlService_t, ParseFile);
  // DispatchKernel(&kernel.service_transporter, xmlService_t, GetValueFromTag);

  // SharedMemory *sharedMemory = &(kernel.sharedMemory);
  // service_init(&sharedMemoryService, (void *)sharedMemory, sharedMemory_t);
  // initSharedMemoryService(sharedMemory, &sharedMemoryService);
  // RegisterServiceToKernel(service_transporter, &sharedMemoryService, sharedMemory_t);

  // // //DispatchKernel(service_transporter, sharedMemory_t, AllocateSharedMemory)
  // char data[100] = "<params><numberOfBytes> 8388608 </numberOfBytes></params>";
  // // data = "<numberOfBytes>10</numberOfBytes>";
  // printk("dispatching the kernel shared memory service\n");
  // DispatchKernelTest(service_transporter, sharedMemory_t, allocatedSharedMemory_t, data);
  // char data2[100] = "<params><numberOfBytes> 8388608 </numberOfBytes></params>";
  // DispatchKernelTest(service_transporter, sharedMemory_t, allocatedSharedMemory_t, data2);

  printk("Finished BSP Kernel Main\n");
  // SharedMemoryDiscoveryService(service_transporter->kernel_services[sharedMemory_t]);
  // kernel.down_arrow = false;
  // kernel.up_arrow = false;

  AckermannFunction *ackermannFunction = &(kernel.ackermannFunction);
  service_init(&AckermannFunctionService, (void *)ackermannFunction, ackermannFunctionService_t);
  initAckermannFunctionService(ackermannFunction, &AckermannFunctionService);
  RegisterServiceToKernel(service_transporter, &AckermannFunctionService, ackermannFunctionService_t);
  //AckermannFunctionDiscovery(service_transporter->kernel_services[ackermannFunctionService_t]);

  // kernel.HashTable.capacity = 10;
  // kernel.HashTable.size = 0;
  // init_array(&kernel.HashTable);
  // printk("done!\n");
  // RegistryServiceValue *firstValue = kmalloc(&kernel.memoryAllocator, sizeof(RegistryServiceValue));
  // firstValue->core_count = 2;
  // firstValue->coreID[0] = 1;
  // firstValue->coreID[1] = 2;

  // RegistryServiceValue *secondValue = kmalloc(&kernel.memoryAllocator, sizeof(RegistryServiceValue));
  // secondValue->core_count = 3;
  // secondValue->coreID[0] = 4;
  // secondValue->coreID[1] = 2;
  // secondValue->coreID[2] = 5;

  // insert(&kernel.HashTable, "Hamada", "192.168.1.0", "08:00:27:0c:1b:93", firstValue); // in case it is out of the lan
  // insert(&kernel.HashTable, "Hamada", "LOCAL", "08:00:27:24:41:b3", secondValue);      // in case it is out of the lan

  //   printk("current size: %d\n", kernel.HashTable.size);
  //   insert(&kernel.HashTable, "08:00:27:24:41:b3", secondValue);
  //   printk("current size: %d\n", kernel.HashTable.size);

  //   display(&kernel.HashTable);
  //   RegistryServiceValue *node1 = GetElementFromHashTable(&kernel.HashTable, "08:00:27:0c:1b:93");
  //   RegistryServiceValue *node2 = GetElementFromHashTable(&kernel.HashTable, "08:00:27:24:41:b3");

  //   printk(" value: coreCount: %d", node1->core_count);
  //   printk(" service: %s \n\n", node1->service);
  //   printk(" value: coreCount: %d", node2->core_count);
  //   printk(" service: %s \n\n", node2->service);
}

void userModeDemo()
{
  switchToUserMode();
  asm volatile("hlt;");
}