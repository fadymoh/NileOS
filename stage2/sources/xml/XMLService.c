#include "XMLService.h"
#include "Kernel.h"
extern Kernel kernel;

void parseAFile(void *xmlservice_ptr)
{
  printk(" fe eh\n");
    XMLService *xmlservice = (XMLService *)xmlservice_ptr;
    printk("Reading System Configuration .....\n");

    char *config_buffer = xmlservice->buffer;
    config_buffer = (char *)kvalloc(&kernel.memoryAllocator, (xmlservice->params.sectors_count_to_read + 1) * SECTOR_SIZE);
    memset(config_buffer, 0, (xmlservice->params.sectors_count_to_read + 1) * SECTOR_SIZE);
    sprintf(config_buffer, "<SystemConfig>\n");

    kernel.dmaBuffer.enabled = true;
    kernel.dmaBuffer.total_read = 0;

    while (readDMADisk(kernel.ataManager.ataDisks[0], xmlservice->params.sectors_start, 
    config_buffer + strlen(config_buffer), 
    xmlservice->params.sectors_count_to_read,
     0, 0) == READ_DMA_FAIL);

    int ATA_core_id = xmlservice->params.ata_core_id; 
     kernel.apicManager.apics[ATA_core_id].fired_interrupts[ATA_IPI] = 0;
  while (kernel.apicManager.apics[ATA_core_id].fired_interrupts[ATA_IPI] == 0)
  {
    asm volatile("sti;");
    asm volatile("hlt;");
    asm volatile("cli;");
    sendAPICEOI(&kernel.apicManager.apics[ATA_core_id]);
    if (kernel.dmaBuffer.total_read == kernel.dmaBuffer.target_read)
      break;
  }

  kernel.apicManager.apics[0].fired_interrupts[ATA_IPI] = 0;
  kernel.dmaBuffer.enabled = false;


  sprintf(config_buffer + strlen(config_buffer), "</SystemConfig>\n");
  printk("Configuration read ....\n");

  xml_heap_t * xml_heap = (xml_heap_t *)kvalloc(&kernel.memoryAllocator, sizeof(xml_heap_t));
  xml_heap_init(xml_heap, config_buffer);

  volatile int done = 0;

  done = parse_xml(xml_heap);

  if (done)
  {
    kernel.sys_xml_confg = xml_heap;
    printk("Parsing Successful\n");
  }
  else
  {
    kernel.sys_xml_confg = NULL;
    printk("Error parsing XML \n");
  }

  printk("DONE XML PARSING!\n");

}

void getValueFromTag(void * xmlservice_ptr)
{
    XMLService *xmlservice = (XMLService *)xmlservice_ptr;
    printk("Starting to Query the XML \n");

    char q[1024];
    memset(q, 0, sizeof(q));
    sprintf(q, "/SystemConfig/%s", xmlservice->params.tagDirectory);
    xml_node_t *xml_node = exec_xquery(q, kernel.sys_xml_confg);

    if (xml_node != NULL){

      printk("Data Retrieved is: %s\n", xml_node->content);
      strcpy(xmlservice->returns.value, xml_node->content);

  }

  printk("Finished executing queries\n");

}

void initializeXMLService(Service *p_service)
{
    p_service->add_service(p_service, parseAFile ,ParseFile);
    p_service->add_service(p_service, getValueFromTag, GetValueFromTag); 
}