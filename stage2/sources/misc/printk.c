#include "printk.h"
#include "Kernel.h"

extern Kernel kernel;

void printk(const char *str_template, ...)
{
    char str[2048];
    memset(str, 0, 2048);
    va_list args;
    va_start(args, str_template);
    sprintf_raw(str, str_template, args);
    kernel.console.params.p_string = str;
    kernel.console.params.newIndex = 0;
    kernel.console.params.p_background_color = COLOR_BLACK;
    kernel.console.params.p_foreground_color = COLOR_GREEN;
    if (kernel.console.params.newIndex != kernel.console.current_terminal_index)
    {
        // write to the terminal buffer only
        dispatch_kernel(&kernel.service_transporter, console_t, add_to_terminal);
    }
    else
    {
        dispatch_kernel(&kernel.service_transporter, console_t, add_string);
    }

    va_end(args);
}

void printk_network(const char *str_template, ...)
{
    char str[2048];
    memset(str, 0, 2048);
    va_list args;
    va_start(args, str_template);
    sprintf_raw(str, str_template, args);
    kernel.console.params.p_string = str;
    kernel.console.params.newIndex = 2;
    kernel.console.params.p_background_color = COLOR_BLACK;
    kernel.console.params.p_foreground_color = COLOR_CYAN;
    if (kernel.console.params.newIndex != kernel.console.current_terminal_index)
    {
        // write to the terminal buffer only
        dispatch_kernel(&kernel.service_transporter, console_t, add_to_terminal);
    }
    else
    {
        dispatch_kernel(&kernel.service_transporter, console_t, add_string);
    }
    va_end(args);
}

void printk_debug(const char *str_template, ...)
{
    char str[2048];
    memset(str, 0, 2048);
    va_list args;
    va_start(args, str_template);
    sprintf_raw(str, str_template, args);
    kernel.console.params.p_string = str;
    kernel.console.params.newIndex = 3;
    kernel.console.params.p_background_color = COLOR_BLACK;
    kernel.console.params.p_foreground_color = COLOR_LIGHT_RED;
    if (kernel.console.params.newIndex != kernel.console.current_terminal_index)
    {
        // write to the terminal buffer only
        dispatch_kernel(&kernel.service_transporter, console_t, add_to_terminal);
    }
    else
    {
        dispatch_kernel(&kernel.service_transporter, console_t, add_string);
    }
    va_end(args);
}

void printk_fs(const char *str_template, ...)
{
    char str[2048];
    memset(str, 0, 2048);
    va_list args;
    va_start(args, str_template);
    sprintf_raw(str, str_template, args);
    kernel.console.params.p_string = str;
    kernel.console.params.newIndex = 4;
    kernel.console.params.p_background_color = COLOR_BLACK;
    kernel.console.params.p_foreground_color = COLOR_WHITE;
    if (kernel.console.params.newIndex != kernel.console.current_terminal_index)
    {
        // write to the terminal buffer only
        dispatch_kernel(&kernel.service_transporter, console_t, add_to_terminal);
    }
    else
    {
        dispatch_kernel(&kernel.service_transporter, console_t, add_string);
    }
    va_end(args);
}