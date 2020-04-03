
#include "service.h"

#include "Kernel.h"
#include "hamada.h"
#include "service1.h"
typedef enum user_services_enum_t { hamadaStruct_t = 10, service1_t } user_service_enum;


extern Kernel kernel;

hamadaStruct hamadaStruct_object;
Service hamadaStructService;
service1 service1_object;
Service service1Service;

bool generateServices()
{
	kernel.void_pointers[0] = (void*) &hamadaStruct_object;
	service_init(&hamadaStructService, (void *)&hamadaStruct_object, hamadaStruct_t);
	hamadaStructService.add_service(&hamadaStructService, hamada1, void_hamada1);
	hamadaStructService.add_service(&hamadaStructService, hamada2, void_hamada2);
	hamadaStructService.add_service(&hamadaStructService, hamada3, void_hamada3);
	RegisterServiceToKernel(&kernel.service_transporter, &hamadaStructService, hamadaStruct_t);

	kernel.void_pointers[1] = (void*) &service1_object;
	service_init(&service1Service, (void *)&service1_object, service1_t);
	service1Service.add_service(&service1Service, method1, void_method1);
	service1Service.add_service(&service1Service, method2, void_method2);
	service1Service.add_service(&service1Service, method3, void_method3);
	RegisterServiceToKernel(&kernel.service_transporter, &service1Service, service1_t);
}
