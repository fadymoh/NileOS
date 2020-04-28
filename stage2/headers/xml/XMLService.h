#ifndef XMLSERVICE_H
#define XMLSERVICE_H

#include "defines.h"
#include "includes.h"
#include "XMLParser.h"
#include "XQuery.h"
#include "service.h"

#define MAX_TAG_LENGTH 255
#define MAX_VALUE_LENGTH 256

enum XMLService_exposed_functions
{
        ParseFile, 
        GetValueFromTag
};

typedef struct
{
        char * buffer;

        // args
        struct
        {
                uint64_t sectors_start;
                uint64_t sectors_count_to_read;
                int ata_core_id;
                char * tagDirectory;  //momkn el moshkla henna
        } params;

        struct 
        {
            char value [MAX_VALUE_LENGTH]; 
        } returns;
        
} XMLService;


void initializeXMLService(Service *p_service); 
void parseAFile(void * xmlservice); 
void getValueFromTag(void * xmlservice);


#endif