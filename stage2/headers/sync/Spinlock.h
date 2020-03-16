#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include "includes.h"
#include "defines.h"

#define cpu_relax() asm volatile("pause\n": : :"memory")
#define barrier() asm volatile("": : :"memory")

#define SL_EBUSY 1

typedef struct spinlock_t{
    uint64_t lapic;
    uint16_t stack;
    uint16_t core_id;
    uint64_t spin_lock;
} spinlock_t;

void spinlock_init (spinlock_t * spinlock);
void spinlock_lock(spinlock_t * spinlock);
void spinlock_unlock (spinlock_t * spinlock);
uint16_t spinlock_trylock(spinlock_t * spinlock);

#endif