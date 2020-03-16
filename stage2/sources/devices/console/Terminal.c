#include "Terminal.h"


void terminal_initialize(Terminal * p_terminal,uint16_t * p_terminal_buffer,uint8_t p_x_max,uint8_t p_y_max,uint16_t p_y_rows)
{
    p_terminal->terminal_buffer_size = p_x_max * p_y_rows;
    if ( p_terminal_buffer != NULL) p_terminal->terminal_buffer = p_terminal_buffer;
    // else allocate
    p_terminal->x_position = 0;
    p_terminal->y_position = 0;
    p_terminal->x_max = p_x_max;
    p_terminal->y_max = p_y_max;
    p_terminal->y_rows = p_y_rows;
    terminal_clearScreen(p_terminal,COLOR_BLACK);
}

void terminal_putCharacterAt (Terminal * p_terminal,
                            const char p_character, 
                            uint8_t p_x_position, 
                            uint8_t p_y_position, 
                            uint8_t p_background_color, 
                            uint8_t p_foreground_color)
{
    uint16_t color16 = (p_foreground_color | p_background_color << 4) << 8;
    volatile uint16_t * location = p_terminal->terminal_buffer + ((p_y_position* p_terminal->x_max) + p_x_position);
    *location = p_character | color16;

}

void terminal_putCharacter (Terminal * p_terminal,
                            const char p_character, 
                            uint8_t p_background_color, 
                            uint8_t p_foreground_color)
{
    terminal_putCharacterAt (p_terminal,p_character, p_terminal->x_position, p_terminal->y_position, p_background_color, p_foreground_color);
}


void terminal_putCharacterAndAdvanceString (Terminal * p_terminal,
                                        const char p_character, 
                                        uint8_t p_background_color, 
                                        uint8_t p_foreground_color)
{
     if ( p_character == '\n') 
    {
        if ( p_terminal->y_position < p_terminal->y_rows-1)
            p_terminal->y_position++; 
        else
        {
//                p_terminal->y_position = 0;
            memmove16(p_terminal->terminal_buffer,p_terminal->terminal_buffer+p_terminal->x_max,(p_terminal->y_rows-1)*p_terminal->x_max);
            memset16(p_terminal->terminal_buffer+((p_terminal->y_rows-1)*p_terminal->x_max),0,p_terminal->x_max);
        }
        p_terminal->x_position = 0; 
    }
     else
    {
        terminal_putCharacter(p_terminal,p_character,p_background_color,p_foreground_color);
        if ( p_terminal->x_position < p_terminal->x_max) 
            p_terminal->x_position++; 
        else 
        {               
            if ( p_terminal->y_position < p_terminal->y_rows-1)
                p_terminal->y_position++; 
            else
            {
//                p_terminal->y_position = 0;
                memmove16(p_terminal->terminal_buffer,p_terminal->terminal_buffer+p_terminal->x_max,(p_terminal->y_rows-1)*p_terminal->x_max);
                memset16(p_terminal->terminal_buffer+((p_terminal->y_rows-1)*p_terminal->x_max),0,p_terminal->x_max);
            }
            p_terminal->x_position = 0;
        }
    }    

}

void terminal_putString (Terminal * p_terminal,
                        const char* p_string, 
                        uint8_t p_background_color, 
                        uint8_t p_foreground_color )
{
    uint64_t i;
    for ( i = 0 ; p_string[i] != 0 ; i ++) // Loop until a NULL character is encountered
        terminal_putCharacterAndAdvanceString(p_terminal,p_string[i],p_background_color,p_foreground_color);
}



void terminal_putHexa (Terminal * p_terminal,
                    uint64_t p_hexa, 
                    uint8_t p_background_color, 
                    uint8_t p_foreground_color, 
                    bool p_show_x_flag )
{
    uint64_t l_tmp;
    bool l_no_zeros = true;
    int i;
    
    if ( p_show_x_flag ) // if p_show_x_flag is true print "0x" before number
    {
        terminal_putCharacterAndAdvanceString(p_terminal,'0',p_background_color,p_foreground_color);
        terminal_putCharacterAndAdvanceString(p_terminal,'x',p_background_color,p_foreground_color);
    }
    
    if (p_hexa == 0 )
    {
        terminal_putCharacterAndAdvanceString(p_terminal,'0',p_background_color,p_foreground_color);
        terminal_putCharacterAndAdvanceString(p_terminal,'0',p_background_color,p_foreground_color);
        return;
    }

    for (i = 60; i<= 60 && i >= 0; i -= 4)
    {
        l_tmp = (p_hexa >> i) & 0xF;
        
        if (l_tmp == 0 && l_no_zeros) continue;
        if (l_tmp >= 0xA)
        {
            l_no_zeros = false;
            terminal_putCharacterAndAdvanceString (p_terminal,l_tmp-0xA+'a',p_background_color,p_foreground_color);
        }
        else
        {
            l_no_zeros = false;
            terminal_putCharacterAndAdvanceString(p_terminal,l_tmp+'0',p_background_color,p_foreground_color);
        }
    }
}


void terminal_putHexa128 (Terminal * p_terminal,
                    uint128_t p_hexa, 
                    uint8_t p_background_color, 
                    uint8_t p_foreground_color, 
                    bool p_show_x_flag )
{
    uint8_t l_tmp;
    bool l_no_zeros = true;
    int i;
    
    if ( p_show_x_flag ) // if p_show_x_flag is true print "0x" before number
    {
        terminal_putCharacterAndAdvanceString(p_terminal,'0',p_background_color,p_foreground_color);
        terminal_putCharacterAndAdvanceString(p_terminal,'x',p_background_color,p_foreground_color);
    }
    
    if (p_hexa == 0 )
    {
        terminal_putCharacterAndAdvanceString(p_terminal,'0',p_background_color,p_foreground_color);
        terminal_putCharacterAndAdvanceString(p_terminal,'0',p_background_color,p_foreground_color);
        return;
    }

    for (i = 124; i<= 124 && i >= 0; i -= 4)
    {
        l_tmp = (p_hexa >> i) & 0xF;
        
        if (l_tmp == 0 && l_no_zeros) continue;
        if (l_tmp >= 0xA)
        {
            l_no_zeros = false;
            terminal_putCharacterAndAdvanceString (p_terminal,l_tmp-0xA+'a',p_background_color,p_foreground_color);
        }
        else
        {
            l_no_zeros = false;
            terminal_putCharacterAndAdvanceString(p_terminal,l_tmp+'0',p_background_color,p_foreground_color);
        }
    }
}

void terminal_setPosition(Terminal * p_terminal,
                        uint8_t p_x_position,uint8_t p_y_position)
{
        p_terminal->x_position = p_x_position;
        p_terminal->y_position = p_y_position;

}



void terminal_clearScreen (Terminal * p_terminal,uint8_t p_background_color)
{
    int x,y;
    for ( y = 0 ; y  < p_terminal->y_max ; y ++) 
        for ( x = 0 ; x  < p_terminal->x_max ; x ++)
            terminal_putCharacterAt (p_terminal,' ', x, y, p_background_color, COLOR_WHITE);
}

uint16_t * terminal_getViewPort (Terminal * p_terminal)
{
    if (p_terminal->y_position < p_terminal->y_max ) return p_terminal->terminal_buffer;
    else return (p_terminal->terminal_buffer + ((p_terminal->y_position - p_terminal->y_max)*p_terminal->x_max));
}