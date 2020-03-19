#ifndef PRINTK_H_
#define PRINTK_H_

#include "stdio.h"

void printk (const char * str_template,...);
void printk_debug (const char * str_template,...);
void printk_network (const char * str_template,...);
void printk_fs (const char * str_template,...);

#endif
