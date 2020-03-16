#include "Console.h"


bool console_preInialize(Console * p_console, Service* p_service)
{
    p_console->video_buffer = (volatile uint16_t*) VIDEO_RAM_BUFFER_ADDRESS;
    p_console->x_max= DEFAULT_VIDEO_MAX_X;
    p_console->y_max = DEFAULT_VIDEO_MAX_Y;
    
    terminal_initialize(&(p_console->default_terminal),
                        p_console->default_terminal_buffer,
                        p_console->x_max,
                        p_console->y_max,
                        DEFAULT_TERMINAL_Y_ROWS);

    terminal_initialize(&(p_console->second_terminal),
                        p_console->default_terminal_buffer2,
                        p_console->x_max,
                        p_console->y_max,
                        DEFAULT_TERMINAL_Y_ROWS);

    p_console->terminals[0] = &p_console->default_terminal;
    p_console->terminals[1] = &p_console->second_terminal;
    p_console->terminals_max_count = 5;
    p_console->terminals_current_count = 2 ;
    p_console->current_terminal_index = 0 ;
    
    p_service->add_service(p_service, console_addStringToCurrentTerminal, add_string);
    p_service->add_service(p_service, console_addHexaToCurrentTerminal, add_hexa);
    p_service->add_service(p_service, console_addHexa128ToCurrentTerminal, add_hexa128);
    p_service->add_service(p_service, console_switchTerminal, switch_terminal);
    p_service->add_service(p_service, console_writeToTerminal, add_to_terminal);

    return true;
    
}

bool console_addStringToCurrentTerminal (void* p_console_ptr)
{
    Console* p_console = (Console*) p_console_ptr;
    const char* p_string = p_console->params.p_string;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;

    Terminal * current_terminal = console_getCurentTerminal();
    terminal_putString (current_terminal,p_string,p_background_color,p_foreground_color );
    memcpy16((uint16_t *)p_console->video_buffer,terminal_getViewPort(current_terminal),p_console->x_max * p_console->y_max);
    return true;
}


bool console_writeToTerminal(void* p_console_ptr){
    
    Console* p_console = (Console*) p_console_ptr;
    const char* p_string = p_console->params.p_string;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;
    int newIndex = p_console->params.newIndex;

    int oldIndex = p_console->current_terminal_index;
    p_console->current_terminal_index = newIndex;
    Terminal * current_terminal = console_getCurentTerminal();
    terminal_putString (current_terminal,p_string,p_background_color,p_foreground_color );

    p_console->current_terminal_index = oldIndex;
}

bool console_switchTerminal(void* p_console_ptr)
{
    Console* p_console = (Console*) p_console_ptr; 
    p_console->current_terminal_index = p_console->params.newIndex;
    Terminal * current_terminal = console_getCurentTerminal();
    memcpy16((uint16_t *)p_console->video_buffer,terminal_getViewPort(current_terminal),p_console->x_max * p_console->y_max);
}

bool console_addHexaToCurrentTerminal (void* p_console_ptr)
{
    
    Console* p_console = (Console*) p_console_ptr;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;
    const uint64_t p_hexa = p_console->params.p_hexa;
    Terminal * current_terminal = console_getCurentTerminal();
    terminal_putHexa (current_terminal,p_hexa,p_background_color,p_foreground_color,true);
    memcpy16((uint16_t *)p_console->video_buffer,terminal_getViewPort(current_terminal),p_console->x_max * p_console->y_max);
    return true;
}

bool console_addHexa128ToCurrentTerminal (void* p_console_ptr)
{
    Console* p_console = (Console*) p_console_ptr;
    uint8_t p_background_color = p_console->params.p_background_color;
    uint8_t p_foreground_color = p_console->params.p_foreground_color;
    const uint64_t p_hexa = p_console->params.p_hexa;
    Terminal * current_terminal = console_getCurentTerminal();
    terminal_putHexa128 (current_terminal,p_hexa,p_background_color,p_foreground_color,true);
    memcpy16((uint16_t *)p_console->video_buffer,terminal_getViewPort(current_terminal),p_console->x_max * p_console->y_max);
    return true;
}