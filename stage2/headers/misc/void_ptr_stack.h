#ifndef VOID_PTR_STACK_H_
#define VOID_PTR_STACK_H_
#include "defines.h"
#include "includes.h"
#include "MemoryUtilities.h"

#define MAX_VOID_STACK_SIZE 128

typedef struct void_ptr_stack_t
{
        void * stack[MAX_VOID_STACK_SIZE];
        uint16_t stack_ptr;
} void_ptr_stack_t;

void void_ptr_stack_init (void_ptr_stack_t * stack);
bool void_ptr_stack_push (void_ptr_stack_t * stack , void * item);
void * void_ptr_stack_pop (void_ptr_stack_t * stack);

#endif