#include "service1.h"
#include "Console.h"

void method3 (void* s1)
{
	service1* s2 = (service1*) s1;
	s2->z = 'A';
	printk("%c", s2->z);
}
