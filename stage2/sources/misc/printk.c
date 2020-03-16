#include "printk.h"
#include "Kernel.h"

extern Kernel kernel;

void printk (const char * str_template,...)
{
    char str [2048];
    memset (str,0,2048);
    va_list args;
    va_start(args,str_template);
    sprintf_raw(str,str_template,args);
    kernel.console.params.p_string = str;
    kernel.console.params.p_background_color = COLOR_BLACK;
    kernel.console.params.p_foreground_color = COLOR_GREEN;
    dispatch_kernel(&kernel.service_transporter, console_t, add_string);

    va_end(args);
}
