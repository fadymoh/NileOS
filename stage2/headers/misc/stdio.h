#ifndef STDIO_H_
#define STDIO_H_


#include "includes.h"
#include "defines.h"
#include "MemoryUtilities.h"


void sprintf_raw (char * str, const char * str_template,va_list arfs);
void sprintf (char * str, const char * str_template,...);
void printk (const char * str_template,...);


#endif
