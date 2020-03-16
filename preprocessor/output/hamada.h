typedef struct {

	int x;
	int y;
	char z;

}hamadaStruct;

void hamada1(void* s1);
void hamada2(void* s1);
void hamada3(void* s1);
enum hamadaStruct_exposed_functions { void_hamada1, void_hamada2, void_hamada3 };

