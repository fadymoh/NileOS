#include "service1.h"


//STL_K
void method1 (void* s1)
{
	service1* s2 = (service1*) s1;
	s2->x = 1;
}
//END_STL_K


//STL_K
void method2 (void* s1)
{
	service1* s2 = (service1*) s1;
	s2->y = 20;
}
//END_STL_K

//STL_U
void method3 (void* s1)
{
	service1* s2 = (service1*) s1;
	s2->z = 'A';
	printk("%c", s2->z);
}
//END_STL_U
