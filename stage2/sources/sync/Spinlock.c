#include "Spinlock.h"
#include "CoreUtils.h"

uint64_t spinlock_xchg_64(void * ptr, uint64_t x)
{
	__asm__ __volatile__("xchgq %0,%1"
				:"=r" ((uint64_t) x)
				:"m" (*(volatile uint64_t *)ptr), "0" ( x)
				:"memory");

	return x;
}

uint32_t spinlock_xchg_32(void *ptr, uint32_t x)
{
	__asm__ __volatile__("xchgl %0,%1"
				:"=r" ((uint32_t) x)
				:"m" (*(volatile uint32_t *)ptr), "0" (x)
				:"memory");
	return x;
}

uint16_t spinlock_xchg_16(void *ptr, uint16_t x)
{
	__asm__ __volatile__("xchgw %0,%1"
				:"=r" ((uint16_t) x)
				:"m" (*(volatile uint16_t *)ptr), "0" (x)
				:"memory");

	return x;
}


void spinlock_init (spinlock_t * spinlock)
{
    spinlock->core_id=0;
    spinlock->lapic = 0;
    spinlock->stack = 0;
    spinlock->spin_lock = 0;

}
void spinlock_lock(spinlock_t * spinlock)
{
    if ( spinlock->lapic > 0)
    {
        if ( spinlock->core_id == getAPICId(spinlock->lapic)+1) 
        {
            spinlock->stack++;
            return;
        }
    }
    while (1)
    {
        if (!spinlock_xchg_32(&spinlock->spin_lock, SL_EBUSY)) 
        {
            spinlock->stack ++;
            if ( spinlock->lapic > 0)
            {
                spinlock->core_id = getAPICId(spinlock->lapic)+1;
            }
            return;            
        }
        while (spinlock->spin_lock) 
        {
            cpu_relax();
        }
    }
}
void spinlock_unlock (spinlock_t * spinlock)
{
    if ( spinlock->lapic > 0)
    {
        if ( spinlock->core_id != getAPICId(spinlock->lapic)+1) 
        {
            return;
        }
    }
    spinlock->stack -- ;
    if ( spinlock->stack == 0)
    {
        barrier();
        spinlock->spin_lock = 0;    
        spinlock->core_id=0;
    }
}
uint16_t spinlock_trylock(spinlock_t * spinlock)
{
    if (spinlock->lapic > 0)
    {
        if ( spinlock->core_id == getAPICId(spinlock->lapic)+1) 
            return spinlock->core_id;
    }
    return spinlock_xchg_64(&spinlock->spin_lock, SL_EBUSY);
}
