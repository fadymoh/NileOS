#include "XMLParser.h"
#include "MemoryUtilities.h"
#include "Kernel.h"
extern Kernel kernel;
xml_node_t *getNewXMLNode(xml_heap_t *xml_heap)
{
        if (xml_heap->current_node >= MAX_XML_NODES)
                return NULL;
        xml_heap->xml_nodes[xml_heap->current_node].content = NULL;
        xml_heap->xml_nodes[xml_heap->current_node].child_start = NULL;
        xml_heap->xml_nodes[xml_heap->current_node].parent = NULL;
        xml_heap->xml_nodes[xml_heap->current_node].child_end = NULL;
        xml_heap->xml_nodes[xml_heap->current_node].right = NULL;
        xml_heap->xml_nodes[xml_heap->current_node].tag = NULL;
        return &xml_heap->xml_nodes[xml_heap->current_node++];
}

char *parse_open_tag(xml_heap_t *xml_heap)
{
        char *ptr = xml_heap->xml_source;
        xml_heap->index++;
        char *return_ptr = &(ptr[xml_heap->index]);
        for (; xml_heap->index < xml_heap->len; xml_heap->index++)
        {
                if (ptr[xml_heap->index] == ' ')
                        ptr[xml_heap->index] = 0;
                else if (ptr[xml_heap->index] == '>')
                {
                        ptr[xml_heap->index] = 0;
                        break;
                }
        }
        return return_ptr;
}

char *parse_close_tag(xml_heap_t *xml_heap)
{
        char *ptr = xml_heap->xml_source;
        xml_heap->index += 2;
        char *return_ptr = &(ptr[xml_heap->index]);
        for (; xml_heap->index < xml_heap->len; xml_heap->index++)
        {
                if (ptr[xml_heap->index] == ' ')
                        ptr[xml_heap->index] = 0;
                else if (ptr[xml_heap->index] == '>')
                {
                        ptr[xml_heap->index] = 0;
                        break;
                }
        }
        return return_ptr;
}

int parse_xml(xml_heap_t *xml_heap)
{
        void_ptr_stack_t stack;
        void_ptr_stack_init(&stack);
        xml_node_t *parent_node = NULL;
        xml_node_t *xml_node = NULL;

        char *ptr = xml_heap->xml_source;
        for (; xml_heap->index < xml_heap->len; xml_heap->index++)
        {
                if (ptr[xml_heap->index] == '<' && ptr[xml_heap->index + 1] == '/')
                {
                        if (xml_heap->current_content != NULL)
                        {
                                ptr[xml_heap->index] = 0;
                                parent_node->content = xml_heap->current_content;
                        }
                        if (parent_node != NULL && parent_node->parent != NULL)
                                parent_node = parent_node->parent;

                        char *tag = parse_close_tag(xml_heap);
                        char *t = void_ptr_stack_pop(&stack);
                        if (strcmp(tag, t) != NULL)
                        {
                                printk("XML Error 1:\n");
                                //return 0; // should generate an error here
                                return 0;
                        }
                        xml_heap->current_content = NULL;
                }
                else if (ptr[xml_heap->index] == '<' && ptr[xml_heap->index + 1] != '?')
                {
                        char *tag = parse_open_tag(xml_heap);

                        xml_node = getNewXMLNode(xml_heap);
                        if (xml_node == NULL)
                        {
                                printk("XML Error 2\n");
                                return 0;
                                //return 0; // should generate an error here
                        }

                        xml_node->tag = tag;
                        if (parent_node == NULL)
                                parent_node = xml_node;
                        else
                        {
                                if (parent_node->child_start == NULL)
                                {
                                        parent_node->child_start = xml_node;
                                        parent_node->child_end = xml_node;
                                }
                                else
                                {
                                        parent_node->child_end->right = xml_node;
                                        parent_node->child_end = xml_node;
                                }
                                xml_node->parent = parent_node;
                                parent_node = xml_node;
                        }
                        if (tag[strlen(tag) - 1] == '/' || ptr[xml_heap->index - 1] == '/')
                        {
                                if (tag[strlen(tag) - 1] == '/')
                                        tag[strlen(tag) - 1] = 0;
                                xml_heap->current_content = NULL;
                                if (parent_node != NULL && parent_node->parent != NULL)
                                        parent_node = parent_node->parent;
                        }
                        else
                        {
                                if (!void_ptr_stack_push(&stack, tag))
                                {
                                        printk("XML Error 3");
                                        return 0;
                                        //return 0; // should generate an error here
                                }

                                xml_heap->current_content = &(ptr[xml_heap->index + 1]);
                        }
                }
        }

        //printk("Finished Parsing XML File\n");
        return 1;
}

void xml_heap_init(xml_heap_t *xml_heap, char *xml_source)
{
        xml_heap->len = strlen(xml_source);
        //char* xml_source_copy = kmalloc(&kernel.memoryAllocator, xml_heap->len);
       // fast_memcpy(xml_source_copy, xml_source, xml_heap->len);
        xml_heap->xml_source = xml_source;
        xml_heap->current_node = 0;
        xml_heap->index = 0;
        xml_heap->current_content = NULL;
}