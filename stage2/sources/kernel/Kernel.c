
void switchToUserMode()
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