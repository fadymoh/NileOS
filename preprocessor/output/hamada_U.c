#include "hamada.h"
#include "Console.h"

void hamada3 (void* s1)
{
	hamadaStruct* s2 = (hamadaStruct*) s1;

	s2->z = 'A';
	printk("%c", s2->z);
}
