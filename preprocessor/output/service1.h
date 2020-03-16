#include <stdio.h>

typedef struct {

	int x;
	int y;
	char z;

}service1;

void method1(void* s1);
void method2(void* s1);
void method3(void* s1);
enum service1_exposed_functions { void_method1, void_method2, void_method3 };

