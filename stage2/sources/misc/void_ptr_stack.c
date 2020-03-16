#include "void_ptr_stack.h"

void void_ptr_stack_init (void_ptr_stack_t * stack)
{
    for ( uint16_t i = 0 ; i  < MAX_VOID_STACK_SIZE ; i ++)
        stack->stack[stack->stack_ptr] = NULL;
    stack->stack_ptr = 0;
}

bool void_ptr_stack_push (void_ptr_stack_t * stack , void * item)
{
    if (stack->stack_ptr <= MAX_VOID_STACK_SIZE)
    {
            stack->stack[stack->stack_ptr] = item;
            stack->stack_ptr ++;
            return true;
    }
    return false;
}

void * void_ptr_stack_pop (void_ptr_stack_t * stack)
{
    if (stack->stack_ptr > 0)
    {
            void * item  = stack->stack[stack->stack_ptr-1];
            stack->stack_ptr --;
            return item;
    }
    return NULL;
}
