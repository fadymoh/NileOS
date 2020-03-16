void switchToUserMode ()
{
    asm volatile("  \
         cli; \
         mov $0x23, %ax; \
         mov %ax, %ds; \
         mov %ax, %es; \
         mov %ax, %fs; \
         mov %ax, %gs; \
                       \
         mov %rsp, %rax; \
         pushq $0x23; \
         pushq %rax; \
         pushfq; \
         popq  %rax; \
         or $0x200,%rax; \
         pushq %rax;\
         pushq $0x1B; \
         pushq $1f; \
         iretq; \
       1: \
         ");
}
void switchToUserMode2 ()
{

    asm volatile("  \
         cli; \
         mov $0x23, %ax; \
         mov %ax, %ds; \
         mov %ax, %es; \
         mov %ax, %fs; \
         mov %ax, %gs; \
                       \
         mov %rsp, %rax; \
         pushq $0x23; \
         pushq %rax; \
         pushfq; \
         popq  %rax; \
         or $0x200,%rax; \
         pushq %rax;\
         pushq $0x1B; \
         pushq $1f; \
         ");

    printk("\n%x\n%x\n"
    ,__builtin_return_address(1)
    ,__builtin_return_address(0)
    );
    asm volatile("  \
         iretq; \
         ");
    printk("finished part 10");

    asm volatile("  \
       1: \
         ");
    printk("finished part 11");

}