#include "APICManager.h"
#include "Kernel.h"
#include "numbers.h"

extern Kernel kernel;

void initAPICManager_service(APICManager *apicManager, Service *p_service)
{

    kernel.apicManager.apics_count = apicManager->params.p_apics_count;
    kernel.apicManager.apics = (APIC *)kmalloc(&kernel.memoryAllocator, sizeof(APIC) * apicManager->params.p_apics_count);
    for (uint8_t i = 0; i < kernel.apicManager.apics_count; i++)
    {
        kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.local_apic_addr;
        initialize_apic(&kernel.apicManager.apics[i], i,
                        getVirtualAddress(&kernel.physicalMemoryManager),
                        (i == 0) ? true : false);
    }
    initAPICIO(&kernel.apicManager.apics[0]);
    printk("Done APICIO\n");

    p_service->add_service(p_service, startupAPIC, startupAPIC_s);
    p_service->add_service(p_service, getCurrentCoreId, getCurrentCoreId_s);
    p_service->add_service(p_service, touchCore, touchCore_s);
    p_service->add_service(p_service, moveInitialKernelStack, moveInitialKernelStack_s);

    printk("APIC Manager Success! \n");
}

void startupAPIC(void *p_apicManager)
{
    for (uint8_t i = 1; i < kernel.apicManager.apics_count; i++)
        startup(&kernel.apicManager.apics[i]);
}

void getCurrentCoreId(void *p_apicManager)
{
    kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.local_apic_addr;
    uint64_t lapic_reg = getVirtualAddress(&kernel.physicalMemoryManager);
    kernel.apicManager.returns.core_id = getAPICId(lapic_reg + 0x0020);
}

bool touchCore(void *p_apicManager)
{
    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    uint64_t apic_id = kernel.apicManager.returns.core_id;
    if (apic_id < kernel.apicManager.apics_count)
    {
        initAPICIO(&kernel.apicManager.apics[apic_id]);
        kernel.apicManager.apics[apic_id].touch_flag = true;
        return true;
    }
    else
        return false;
}

bool moveInitialKernelStack(void *p_apicManager)
{
    uint64_t p_start_stack = ((APICManager *)p_apicManager)->params.p_start_stack;
    uint64_t p_end_stack = ((APICManager *)p_apicManager)->params.p_end_stack;
    dispatch_kernel(&kernel.service_transporter, apic_t, getCurrentCoreId_s);
    APIC *apic = &kernel.apicManager.apics[kernel.apicManager.returns.core_id];
    // If current stack is larger than the new stack, fail return false
    if (p_start_stack - p_end_stack > KERNEL_STACK_SIZE_IN_PAGES * MEM_PAGE_SIZE)
        return false;
    // Allocate aligned memory for the stack and store address in kernel_stack_end as
    // the stack grows upwards
    // Get the offset difference between the old current stack and the new one
    uint64_t stack_diff = apic->stack_ptr - p_start_stack;
    // Get the current stack pointer stored in the rsp register and current base pointer stores in the rbp register
    uint64_t old_stack_pointer;
    asm volatile("mov %%rsp, %0"
                 : "=r"(old_stack_pointer));
    // Calculate the new stack pointer and the new base pointer
    uint64_t new_stack_pointer = old_stack_pointer + stack_diff;

    // Declare pointers to the old and new stacks start addresses
    uint64_t *src = (uint64_t *)old_stack_pointer;
    uint64_t *dest = (uint64_t *)new_stack_pointer;
    // Loop to Copy data upwards. We copy 8 bytes in on go
    for (uint64_t i = ((p_start_stack - old_stack_pointer) / 8) - 1;; i--)
    {
        // If the current 8 bytes might represent a base pointer adjust it
        // using the calculated offset
        if (src[i] <= p_start_stack && src[i] >= old_stack_pointer)
            dest[i] = src[i] + stack_diff;
        // Else copy it as is
        else
            dest[i] = src[i];
        // When we reach the top of the stack exist the loop
        if (i == 0)
            break;
    }
    // Set the new stack pointer and the new base pointers to the rsp and rbp
    // registers respectively
    asm volatile("         \
        cli;                 \
        mov %0, %%rsp;       \
        sti;                 "
                 :
                 : "r"(new_stack_pointer));
    return true;
}
