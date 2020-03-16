#include "service1.h"
#include "Console.h"

void method1 (void* s1)
{
	service1* s2 = (service1*) s1;
	s2->x = 1;
}

void method2 (void* s1)
{
	service1* s2 = (service1*) s1;
	s2->y = 20;
}
