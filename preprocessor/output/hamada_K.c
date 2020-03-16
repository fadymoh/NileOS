#include "hamada.h"
#include "Console.h"

void hamada1 (void* s1)
{
	hamadaStruct* s2 = (hamadaStruct*) s1;
	s2->x = 1;
	printk("%d", s2->x);
}

void hamada2 (void* s1)
{
	hamadaStruct* s2 = (hamadaStruct*) s1;
	s2->y = 20;
	printk("%d", s2->y);
}
