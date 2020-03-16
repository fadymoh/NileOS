#include "hamada.h"

//STL_K
void hamada1 (void* s1)
{
	hamadaStruct* s2 = (hamadaStruct*) s1;
	s2->x = 1;
	printk("%d", s2->x);
}
//END_STL_K


//STL_K
void hamada2 (void* s1)
{
	hamadaStruct* s2 = (hamadaStruct*) s1;
	s2->y = 20;
	printk("%d", s2->y);
}
//END_STL_K

//STL_U
void hamada3 (void* s1)
{
	hamadaStruct* s2 = (hamadaStruct*) s1;

	s2->z = 'A';
	printk("%c", s2->z);
}
//END_STL_U
