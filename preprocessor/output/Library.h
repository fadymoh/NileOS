
#include "service.h"

#include "Kernel.h"
#include "service1.h"
#include "hamada.h"
typedef enum user_services_enum_t { service1_t = 10, hamadaStruct_t } user_service_enum;


extern Kernel kernel;

service1 service1_object;
Service service1Service;
hamadaStruct hamadaStruct_object;
Service hamadaStructService;

bool generateServices()
{
	kernel.void_pointers[0] = (void*) &service1_object;
	service_init(&service1Service, (void *)&service1_object, service1_t);
	service1Service.add_service(&service1Service, method1, void_method1);
	service1Service.add_service(&service1Service, method2, void_method2);
	service1Service.add_service(&service1Service, method3, void_method3);
	RegisterServiceToKernel(&kernel.service_transporter, &service1Service, service1_t);

	kernel.void_pointers[1] = (void*) &hamadaStruct_object;
	service_init(&hamadaStructService, (void *)&hamadaStruct_object, hamadaStruct_t);
	hamadaStructService.add_service(&hamadaStructService, hamada1, void_hamada1);
	hamadaStructService.add_service(&hamadaStructService, hamada2, void_hamada2);
	hamadaStructService.add_service(&hamadaStructService, hamada3, void_hamada3);
	RegisterServiceToKernel(&kernel.service_transporter, &hamadaStructService, hamadaStruct_t);
}
