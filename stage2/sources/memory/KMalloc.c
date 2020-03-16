#include "KMalloc.h"
//#include "Kernel.h"
//extern Kernel kernel;

uint64_t kmalloc(MemoryAllocator * p_memoryAllocator, uint64_t p_size)
{
    spinlock_lock(&p_memoryAllocator->spinlock);
    if (  p_memoryAllocator->kernel_memory_current_address+p_size < p_memoryAllocator->kernel_memory_end_address)
    {
        uint64_t cur = p_memoryAllocator->kernel_memory_current_address;        
        p_memoryAllocator->kernel_memory_current_address += p_size;
        spinlock_unlock(&p_memoryAllocator->spinlock);        
        return cur;
    }
    spinlock_unlock(&p_memoryAllocator->spinlock);
    return 0x0;
}
uint64_t kvalloc(MemoryAllocator * p_memoryAllocator,uint64_t p_size)
{
    spinlock_lock(&p_memoryAllocator->spinlock);    
    uint64_t cur = p_memoryAllocator->kernel_memory_current_address;
    if ((cur & 0xFFFFFFFFFFFFF000) < cur )
    {
        cur &= 0xFFFFFFFFFFFFF000;
        cur += 0x1000;
    }
    
    if (  cur+p_size < p_memoryAllocator->kernel_memory_end_address)
    {
        p_memoryAllocator->kernel_memory_current_address = cur+p_size;
        spinlock_unlock(&p_memoryAllocator->spinlock);        
        return cur;
    }
    spinlock_unlock(&p_memoryAllocator->spinlock);
    return 0x0;
    
}

uint64_t malloc(MemoryAllocator * p_memoryAllocator,uint64_t p_size)
{
    spinlock_lock(&p_memoryAllocator->spinlock);
    uint64_t cur = 0 ;
    if (  p_memoryAllocator->user_memory_current_address+p_size < p_memoryAllocator->user_memory_end_address)
    {
        cur = p_memoryAllocator->user_memory_current_address;        
        p_memoryAllocator->user_memory_current_address += p_size;
        spinlock_unlock(&p_memoryAllocator->spinlock);        
        return cur;
    }
    spinlock_unlock(&p_memoryAllocator->spinlock);
    printk ("malloc: p_size = %d\n",p_size);
    printk ("malloc: cur = %x\n",cur);
    printk ("malloc: p_memoryAllocator->user_memory_end_address = %x\n",p_memoryAllocator->user_memory_end_address);  
    return 0x0;
}
uint64_t valloc(MemoryAllocator * p_memoryAllocator,uint64_t p_size)
{
    spinlock_lock(&p_memoryAllocator->spinlock);    
    uint64_t cur = p_memoryAllocator->user_memory_current_address;
    if ((cur & 0xFFFFFFFFFFFFF000) < cur )
    {
        cur &= 0xFFFFFFFFFFFFF000;
        cur += 0x1000;
    }
    
    if (  cur+p_size < p_memoryAllocator->user_memory_end_address)
    {
        p_memoryAllocator->user_memory_current_address = cur+p_size;
        spinlock_unlock(&p_memoryAllocator->spinlock);
        return cur;
    }
    spinlock_unlock(&p_memoryAllocator->spinlock);
    printk ("valloc: p_size = %d\n",p_size);
    printk ("valloc: cur = %x\n",cur);
    printk ("valloc: p_memoryAllocator->user_memory_end_address = %x\n",p_memoryAllocator->user_memory_end_address);
    return 0x0;
}

void memoryAllocatorDump (MemoryAllocator * p_memoryAllocator, bool p_summary)
{
    spinlock_lock(&p_memoryAllocator->spinlock);
    if ( !p_summary )
    {
        printk ("kernel_memory_start_address= %x\n",p_memoryAllocator->kernel_memory_start_address);
        printk ("kernel_memory_current_address= %x\n",p_memoryAllocator->kernel_memory_current_address);
        printk ("kernel_memory_end_address= %x\n",p_memoryAllocator->kernel_memory_end_address);
        printk ("user_memory_start_address= %x\n",p_memoryAllocator->user_memory_start_address);
        printk ("user_memory_current_address= %x\n",p_memoryAllocator->user_memory_current_address);
        printk ("user_memory_end_address= %x\n",p_memoryAllocator->user_memory_end_address);
    }
    printk ("User Memory size= %d/%d MB\n",(p_memoryAllocator->user_memory_end_address-p_memoryAllocator->user_memory_start_address)/(1024*1024),p_memoryAllocator->user_memory_end_address/(1024*1024));
    spinlock_unlock(&p_memoryAllocator->spinlock);
}

void memoryAllocatorTouch (MemoryAllocator * p_memoryAllocator)
{
    spinlock_lock(&p_memoryAllocator->spinlock);
    printk("++++++++++++++++++++++++ Touch here\n");
    p_memoryAllocator->touch ++;
    spinlock_unlock(&p_memoryAllocator->spinlock);
}
void memoryAllocatorUntouch (MemoryAllocator * p_memoryAllocator)
{
    spinlock_lock(&p_memoryAllocator->spinlock);
    if ( p_memoryAllocator->touch > 0 ) p_memoryAllocator->touch --;
    if ( p_memoryAllocator->touch == 0) 
    {
        printk("------------------------ Untouch here\n");
        p_memoryAllocator->user_memory_current_address = p_memoryAllocator->user_memory_start_address;
        memset_user_memory(p_memoryAllocator);
    }
    spinlock_unlock(&p_memoryAllocator->spinlock);
}

void memoryAllocatorAdjust (MemoryAllocator * p_memoryAllocator)
{
    spinlock_lock(&p_memoryAllocator->spinlock);
    uint64_t kernel_used_memory = p_memoryAllocator->kernel_memory_current_address - p_memoryAllocator->kernel_memory_start_address;
    kernel_used_memory = kernel_used_memory*1.1;
    p_memoryAllocator->kernel_memory_end_address = kernel_used_memory+p_memoryAllocator->kernel_memory_start_address;
    p_memoryAllocator->user_memory_start_address = p_memoryAllocator->kernel_memory_end_address+1;
    
    if ((p_memoryAllocator->user_memory_start_address & 0xFFFFFFFFFFFFF000) < p_memoryAllocator->user_memory_start_address )
    {
        p_memoryAllocator->user_memory_start_address &= 0xFFFFFFFFFFFFF000;
        p_memoryAllocator->user_memory_start_address += 0x1000;
    }
    p_memoryAllocator->user_memory_current_address = p_memoryAllocator->user_memory_start_address;
    spinlock_unlock(&p_memoryAllocator->spinlock);    
}

void memset_user_memory (MemoryAllocator * p_memoryAllocator)
{
    memset ((uint8_t *)p_memoryAllocator->user_memory_start_address,0,(p_memoryAllocator->user_memory_end_address-p_memoryAllocator->user_memory_start_address));
}