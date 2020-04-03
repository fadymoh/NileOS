#include "APIC.h"

#include "Kernel.h"
#define APIC_DISABLE 0x010000
extern Kernel kernel;

uint32_t ioapicRead(APIC *apic, uint8_t p_index)
{
    volatile uint32_t *iptr = (uint32_t *)(apic->apicio_base + IO_APIC_MMREG_IOREGSEL);
    volatile uint32_t *vptr = (uint32_t *)(apic->apicio_base + IO_APIC_MMREG_IOWIN);

    *iptr = p_index;
    return *(volatile uint32_t *)vptr;
}
void ioapicWrite(APIC *apic, uint8_t p_index, uint32_t p_value)
{
    volatile uint32_t *iptr = (uint32_t *)(apic->apicio_base + IO_APIC_MMREG_IOREGSEL);
    volatile uint32_t *vptr = (uint32_t *)(apic->apicio_base + IO_APIC_MMREG_IOWIN);

    *iptr = p_index;
    *vptr = p_value;
}

void initialize_apic(APIC *apic, uint16_t p_apic_id, uint64_t p_local_apic, bool p_bsp)
{
    apic->apic_id = p_apic_id;
    apic->lapic_base = p_local_apic;
    kernel.physicalMemoryManager.params.p_physical_address = kernel.acpi.local_apicio_addr;
    DispatchKernel(&kernel.service_transporter, physical_memory_t, get_virtual_address);
    apic->apicio_base = kernel.physicalMemoryManager.returns.virtualAddress;
    memset(apic->fired_interrupts, 0, 256);
    apic->lapic_reg = apic->lapic_base + 0x0020;
    apic->lapic_apicver = apic->lapic_base + 0x0030;
    apic->lapic_taskprior = apic->lapic_base + 0x0080;
    apic->lapic_eoi = apic->lapic_base + 0x00B0;
    apic->lapic_ldr = apic->lapic_base + 0x00D0;
    apic->lapic_dfr = apic->lapic_base + 0x00E0;
    apic->lapic_svr = apic->lapic_base + 0x00F0;
    apic->lapic_esr = apic->lapic_base + 0x280;
    apic->lapic_icr0 = apic->lapic_base + 0x300;
    apic->lapic_icr1 = apic->lapic_base + 0x310;

    apic->lapic_lvt_tmr = apic->lapic_base + 0x320;
    apic->lapic_lvt_perf = apic->lapic_base + 0x340;
    apic->lapic_lvt_lint0 = apic->lapic_base + 0x350;
    apic->lapic_lvt_lint1 = apic->lapic_base + 0x360;
    apic->lapic_lvt_err = apic->lapic_base + 0x370;
    apic->lapic_tmr_init_cnt = apic->lapic_base + 0x380;
    apic->lapic_tmr_curr_cnt = apic->lapic_base + 0x390;
    apic->lapic_tmr_div = apic->lapic_base + 0x3e0;
    apic->lapic_last = apic->lapic_base + 0x38f;
    apic->lapic_disable = 0x10000;
    apic->lapic_sw_enable = 0x100;
    apic->lapic_cpufocus = 0x200;
    apic->lapic_nmi = (4 << 8);
    apic->tmr_periodic = 0x20000;
    apic->tmr_basediv = (1 << 20);
    apic->bsp = p_bsp;
    apic->touch_flag = p_bsp;
    apic->timer_enabled = false;
    apic->stack_ptr = kvalloc(&kernel.memoryAllocator, USER_STACK_SIZE_IN_PAGES * MEM_PAGE_SIZE);
    apic->kstack_ptr = kvalloc(&kernel.memoryAllocator, KERNEL_STACK_SIZE_IN_PAGES * MEM_PAGE_SIZE);

    apic->kstack_ptr += (KERNEL_STACK_SIZE_IN_PAGES * MEM_PAGE_SIZE);
    apic->stack_ptr += (USER_STACK_SIZE_IN_PAGES * MEM_PAGE_SIZE);
    apic->stack_ptr -= 0x10;
    uint64_t *sptr = (uint64_t *)(apic->stack_ptr);
    sptr[0] = 0;

    apic->pit_counter = 0;
    apic->wakeup_counter = 0;
    apic->wakeup_apic_id = 0;

    spinlock_init(&apic->ipioe_spinlock);
    apic->ipioe_auto_replies = false;
    apic->ipioe_raised = false;
    if (!p_bsp)
    {
        uint8_t *code_segment = (uint8_t *)0x200000;
        uint8_t *core_code_segment = (uint8_t *)code_segment + (p_apic_id * 0x200000);
        fast_memcpy(core_code_segment, code_segment, 0x100000);
    }
    memset(apic->role, 0, 5);
}
void initAPICIO(APIC *apic)
{
    writeLocalAPIC(apic->lapic_taskprior, 0);
    writeLocalAPIC(apic->lapic_dfr, 0xFFFFFFFF);
    writeLocalAPIC(apic->lapic_ldr, 0x01000000);
    writeLocalAPIC(apic->lapic_svr, LAPIC_SVR_APIC_ENABLE);
    sfence();
}

void disableAPICTimer(APIC *apic)
{
    writeLocalAPIC(apic->lapic_lvt_tmr, APIC_DISABLE);
}

void enableAPICTimer(APIC *apic)
{
    kernel.interruptManager.params.p_interruptNumber = IRQ0;
    kernel.interruptManager.params.p_interruptHandler = pit_fire;
    DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);

    apic->pit_counter = 0;
    writeLocalAPIC(apic->lapic_tmr_div, 0x3);
    writeLocalAPIC(apic->lapic_tmr_init_cnt, 0xFFFFFFFF);
    RTCSleep(5);
    uint64_t current_timer = readLocalAPIC(apic->lapic_tmr_curr_cnt);
    writeLocalAPIC(apic->lapic_lvt_tmr, APIC_DISABLE);

    uint32_t apic_ap_svr = readLocalAPIC(apic->lapic_svr);
    writeLocalAPIC(apic->lapic_svr, apic_ap_svr | LAPIC_SVR_APIC_ENABLE);

    writeLocalAPIC(apic->lapic_lvt_tmr, IRQ0 | apic->tmr_periodic);
    writeLocalAPIC(apic->lapic_tmr_div, 0x3);

    writeLocalAPIC(apic->lapic_tmr_init_cnt, ((0xFFFFFFFF - current_timer) / (5 * 30)) /*0xFF000000*/); // The higher this value the slower the interrupts
    apic->timer_enabled = true;
}

void sendAPICEOI(APIC *apic)
{
    writeLocalAPIC(apic->lapic_eoi, 0b00000000000000000000000000000000);
    sfence();
}

void mapAPICIRQ(APIC *apic, uint8_t p_irq, uint8_t p_vector)
{

    const uint64_t low_index = 0x10 + (p_irq * 2);
    const uint64_t high_index = 0x10 + (p_irq * 2) + 1;

    uint32_t high = ioapicRead(apic, high_index);

    high &= ~0xff000000;
    high |= apic->apic_id << 24;

    ioapicWrite(apic, high_index, high);

    uint32_t low = ioapicRead(apic, low_index);

    low &= ~(1 << 16);
    low &= ~(1 << 11);
    low &= ~0x700;
    low &= ~0xff;
    low |= p_vector;

    ioapicWrite(apic, low_index, low);
    sfence();
}

bool startup(APIC *apic)
{
    if (apic->bsp)
        return true;
    uint32_t _core_id = apic->apic_id << 24;
    writeLocalAPIC(apic->lapic_icr1, _core_id);
    writeLocalAPIC(apic->lapic_icr0, 0b00000000000000000100010100000000 | TRAMPOLINE);
    RTCSleep(1);

    writeLocalAPIC(apic->lapic_icr1, _core_id);
    writeLocalAPIC(apic->lapic_icr0, 0b00000000000000000100011000000000 | TRAMPOLINE);
    RTCSleep(1);

    writeLocalAPIC(apic->lapic_icr0, 0b00000000000000000100011000000000 | TRAMPOLINE);

    for (volatile uint64_t xyz = 0; xyz < 0xfffffffffffffff0 && !apic->touch_flag; xyz++);

    return true;
}
