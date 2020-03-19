#include "Console.h"
#include "Kernel.h"
extern Kernel kernel;

bool console_preInialize(Console *p_console, Service *p_service)
{
    p_console->video_buffer = (volatile uint16_t *)VIDEO_RAM_BUFFER_ADDRESS;
    p_console->x_max = DEFAULT_VIDEO_MAX_X;
    p_console->y_max = DEFAULT_VIDEO_MAX_Y;
    p_console->terminals_max_count = 5;
    p_console->terminals_current_count = 5;
    p_console->current_terminal_index = 0;
    p_console->terminals = (Terminal**) kmalloc(&kernel.memoryAllocator, sizeof(Terminal*) * p_console->terminals_max_count);
    p_console->terminal_buffer = (uint16_t**) kmalloc(&kernel.memoryAllocator, sizeof(uint16_t*) * p_console->terminals_max_count);

    for (int i = 0; i < p_console->terminals_max_count; ++i)
    {
        p_console->terminals[i] = (Terminal*) kmalloc(&kernel.memoryAllocator, sizeof(Terminal));
        p_console->terminal_buffer[i] = (uint16_t*) kmalloc(&kernel.memoryAllocator, sizeof(uint16_t) * DEFAULT_VIDEO_MAX_X * DEFAULT_VIDEO_MAX_Y);
        terminal_initialize((p_console->terminals[i]),
                        p_console->terminal_buffer[i],
                        p_console->x_max,
                        p_console->y_max,
                        DEFAULT_TERMINAL_Y_ROWS);

    }

    p_service->add_service(p_service, console_addStringToCurrentTerminal, add_string);
    p_service->add_service(p_service, console_addHexaToCurrentTerminal, add_hexa);
    p_service->add_service(p_service, console_addHexa128ToCurrentTerminal, add_hexa128);
    p_service->add_service(p_service, console_switchTerminal, switch_terminal);
    p_service->add_service(p_service, console_writeToTerminal, add_to_terminal);
    p_service->add_service(p_service, console_clearScreen, clear_screen);
    return true;
}

bool console_addStringToCurrentTerminal(void *p_console_ptr)
{
    Console *p_console = (Console *)p_console_ptr;
    const char *p_string = p_console->params.p_string;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;

    Terminal *current_terminal = p_console->terminals[p_console->current_terminal_index];
    terminal_putString(current_terminal, p_string, p_background_color, p_foreground_color);
    memcpy16((uint16_t *)p_console->video_buffer, terminal_getViewPort(current_terminal), p_console->x_max * p_console->y_max);
    
    return true;
}

bool console_writeToTerminal(void *p_console_ptr)
{

    Console *p_console = (Console *)p_console_ptr;
    const char *p_string = p_console->params.p_string;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;

    Terminal *current_terminal = p_console->terminals[p_console->params.newIndex];
    terminal_putString(current_terminal, p_string, p_background_color, p_foreground_color);

    return true;
}

bool console_switchTerminal(void *p_console_ptr)
{
    Console *p_console = (Console *)p_console_ptr;
    p_console->current_terminal_index = (p_console->params.newIndex == 1) ? 2 : p_console->params.newIndex;
    Terminal *current_terminal = p_console->terminals[p_console->current_terminal_index];
    memcpy16((uint16_t *)p_console->video_buffer, terminal_getViewPort(current_terminal), p_console->x_max * p_console->y_max);

    return true;
}

bool console_addHexaToCurrentTerminal(void *p_console_ptr)
{
    Console *p_console = (Console *)p_console_ptr;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;
    const uint64_t p_hexa = p_console->params.p_hexa;
    Terminal *current_terminal = p_console->terminals[p_console->current_terminal_index];
    terminal_putHexa(current_terminal, p_hexa, p_background_color, p_foreground_color, true);
    memcpy16((uint16_t *)p_console->video_buffer, terminal_getViewPort(current_terminal), p_console->x_max * p_console->y_max);
    return true;
}

bool console_addHexa128ToCurrentTerminal(void *p_console_ptr)
{
    Console *p_console = (Console *)p_console_ptr;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;
    const uint64_t p_hexa = p_console->params.p_hexa;
    Terminal *current_terminal = p_console->terminals[p_console->current_terminal_index];
    terminal_putHexa128(current_terminal, p_hexa, p_background_color, p_foreground_color, true);
    memcpy16((uint16_t *)p_console->video_buffer, terminal_getViewPort(current_terminal), p_console->x_max * p_console->y_max);

    return true;
}

bool console_clearScreen(void *p_console_ptr)
{
    Console *p_console = (Console *)p_console_ptr;
    int8_t p_background_color = COLOR_BLACK;
    Terminal *current_terminal = p_console->terminals[p_console->current_terminal_index];
    terminal_clearScreen(current_terminal, p_background_color);
    memcpy16((uint16_t *)p_console->video_buffer, terminal_getViewPort(current_terminal), p_console->x_max * p_console->y_max);

    return true;
}