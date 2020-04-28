#ifndef XMLPARSER_H_
#define XMLPARSER_H_
#include "void_ptr_stack.h"
#define MAX_XML_NODES 1024*128
typedef struct xml_node_t
{
        char * tag;
        struct xml_node_t * parent;
        struct xml_node_t * right;
        struct xml_node_t * child_start;
        struct xml_node_t * child_end;
        char * content;
} xml_node_t;


typedef struct xml_heap_t
{
        char * xml_source;
        xml_node_t xml_nodes [MAX_XML_NODES];
        uint64_t current_node;
        uint64_t index;
        uint64_t len;
        char * current_content;
}xml_heap_t;

xml_node_t * getNewXMLNode (xml_heap_t * xml_heap);
char * parse_open_tag (xml_heap_t * xml_heap);
char * parse_close_tag (xml_heap_t * xml_heap);
int parse_xml (xml_heap_t * xml_heap);
void xml_heap_init (xml_heap_t * xml_heap,char * xml_source);

#endif
