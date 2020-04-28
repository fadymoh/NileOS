#ifndef XQUERY_H_
#define XQUERY_H_
#include "XMLParser.h"
#define MAX_XQUERY_PATHS 32
#define MAX_XQUERY_TAG_NAME 128
typedef struct xquery_item_t
{
        char tag[MAX_XQUERY_TAG_NAME];
        uint16_t index;

}xquery_item_t;

typedef struct xquery_t
{
        xquery_item_t xquery_items [MAX_XQUERY_PATHS];
        uint8_t items_count;
}xquery_t;
void parse_xquery_item (char * xquery_item,xquery_t * xquery);
xml_node_t * getXMLNode(xml_node_t * xml_node,char * tag,uint32_t index);
xml_node_t * exec_xquery (const char * xquery_str,xml_heap_t * xml_heap);

#endif