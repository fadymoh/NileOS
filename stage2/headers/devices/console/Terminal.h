#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "defines.h"
#include "includes.h"
#include "MemoryUtilities.h"

typedef struct
{
        uint16_t *terminal_buffer; // Pointer to the video ram buffer
        uint32_t terminal_buffer_size;
        uint16_t y_rows;
        uint8_t x_position; // The current screen horizontal position
        uint8_t y_position; // The current screen vertical position
        uint8_t x_max;      // The maximum screen horizontal position
        uint8_t y_max;      // The maximum screen vertical position

} Terminal;

void terminal_initialize(Terminal *p_terminal, uint16_t *p_terminal_buffer,
                         uint8_t p_x_max, uint8_t p_y_max, uint16_t p_y_rows);

void terminal_putCharacterAt(Terminal *p_terminal,
                             const char p_character,
                             uint8_t p_x_position,
                             uint8_t p_y_position,
                             uint8_t p_background_color,
                             uint8_t p_foreground_color);

void terminal_putCharacterAndAdvanceString(Terminal *p_terminal,
                                           const char p_character,
                                           uint8_t p_background_color,
                                           uint8_t p_foreground_color);

void terminal_putString(Terminal *p_terminal,
                        const char *p_string,
                        uint8_t p_background_color,
                        uint8_t p_foreground_color);

void terminal_putHexa(Terminal *p_terminal,
                      uint64_t p_hexa,
                      uint8_t p_background_color,
                      uint8_t p_foreground_color,
                      bool p_show_x_flag);

void terminal_putHexa128(Terminal *p_terminal,
                         uint128_t p_hexa,
                         uint8_t p_background_color,
                         uint8_t p_foreground_color,
                         bool p_show_x_flag);

void terminal_setPosition(Terminal *p_terminal,
                          uint8_t p_x_position, uint8_t p_y_position);

void terminal_clearScreen(Terminal *p_terminal, uint8_t p_background_color);

uint16_t *terminal_getViewPort(Terminal *p_terminal);

#endif