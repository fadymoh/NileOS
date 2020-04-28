#include "XQuery.h"
#include "numbers.h"
#include "Kernel.h"
extern Kernel kernel;

void parse_xquery_item (char * xquery_item,xquery_t * xquery)
{
    char * open_bracket = scanc(xquery_item,'[',strlen(xquery_item));
    char * close_bracket = scanc(xquery_item,']',strlen(xquery_item));

    if ( open_bracket != NULL && close_bracket != NULL
                    && open_bracket+1 < close_bracket)
    {
            open_bracket[0] = 0;
            close_bracket[0] = 0;
            memset (xquery->xquery_items[xquery->items_count].tag,0,128);
            strcpy (xquery->xquery_items[xquery->items_count].tag,xquery_item);
            xquery->xquery_items[xquery->items_count].index = atoi(open_bracket+1);
            xquery->items_count++;
    }
    else
    {
            memset (xquery->xquery_items[xquery->items_count].tag,0,128);
            strcpy (xquery->xquery_items[xquery->items_count].tag,xquery_item);
            xquery->xquery_items[xquery->items_count].index = 0;
            xquery->items_count++;
    }
}

xml_node_t * getXMLNode(xml_node_t * xml_node,char * tag,uint32_t index)
{
    xml_node_t * xml_node_ptr = xml_node;
    uint32_t i = 0 ;
    

    for ( ; xml_node_ptr != NULL; xml_node_ptr = xml_node_ptr->right)
    {

        if ( strcmp (xml_node_ptr->tag,tag) == 0 )
        {
            if (i == index ) return xml_node_ptr;
            else i++;
        }
    }
    return NULL;
}

xml_node_t * exec_xquery (const char * xquery_str,xml_heap_t * xml_heap)
{
    char q [1024];
    memset (q, 0,1024);
    strcpy (q,xquery_str);
    uint64_t len = strlen (q); 

    char * ptr = (char *) q;
    char * start = ptr;
    xquery_t xquery;
    xquery.items_count = 0;

    xml_node_t * xml_node1 = &(xml_heap->xml_nodes[0]);

    
    for ( uint64_t i  = 0 ; i < len ; i ++)
    {

        if ( ptr[i] == '/')
        {
            ptr[i] = 0;
            if ( strcmp(start,"")!= 0) parse_xquery_item(start,&xquery);
            start = &(ptr[i+1]);
        }
    }
    if ( strcmp (start,"")!= 0) parse_xquery_item(start,&xquery);
    xml_node_t * xml_node = &(xml_heap->xml_nodes[0]);

    xml_node = getXMLNode(xml_node,xquery.xquery_items[0].tag,xquery.xquery_items[0].index);
  

    for ( uint64_t i = 1 ; xml_node != NULL && i < xquery.items_count ; i++)
    {
        xml_node = getXMLNode(xml_node->child_start,xquery.xquery_items[i].tag,xquery.xquery_items[i].index);
    }
    return xml_node;
}