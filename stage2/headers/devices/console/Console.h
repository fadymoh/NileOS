#ifndef CONSOLE_H_
#define CONSOLE_H_


#include "Terminal.h"
#include "service.h"

enum console_exposed_functions {add_string, add_hexa, add_hexa128, switch_terminal, add_to_terminal, clear_screen} ; 

typedef struct
{
        volatile uint16_t * video_buffer; // Pointer to the video ram buffer        
        uint8_t x_max;      // The maximum screen horizontal position
        uint8_t y_max;      // The maximum screen vertical position    
        uint16_t** terminal_buffer;

        //uint16_t default_terminal_buffer2 [DEFAULT_TERMINAL_Y_ROWS*DEFAULT_VIDEO_MAX_X]; // WILL BE DELETED

        Terminal default_terminal;
        Terminal second_terminal;

        Terminal** terminals;
        uint8_t terminals_max_count;
        uint8_t terminals_current_count;
        uint8_t current_terminal_index;

        // args
        struct{
                uint8_t p_background_color;
                uint8_t p_foreground_color;
                uint64_t p_hexa;
                int newIndex;
                char* p_string;
        } params;
}Console;

    
    
#define console_getCurentTerminal() ((p_console->current_terminal_index < p_console->terminals_current_count) ? p_console->terminals[p_console->current_terminal_index] : &(p_console->default_terminal))
bool console_preInialize(Console * p_console, Service* p_service);

bool console_addStringToCurrentTerminal(void* p_console_ptr);

bool console_addHexaToCurrentTerminal (void* p_console_ptr);

bool console_addHexa128ToCurrentTerminal (void* p_console_ptr);

bool console_switchTerminal(void* p_console_ptr);

bool console_writeToTerminal(void* p_console_ptr);

bool console_clearScreen(void *p_console_ptr);
#endif