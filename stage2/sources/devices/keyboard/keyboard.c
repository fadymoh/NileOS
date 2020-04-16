#include "Kernel.h"
#include "keyboard.h"

extern Kernel kernel;

char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',       /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',   /* 39 */
    '\'', '`',   0,     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
    'm', ',', '.', '/',   0,                    /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

void keyboard_handler(InterruptContext * r)
{
    int i, scancode;

    for(i = 1000; i > 0; i++) 
    {
        // Check if scan code is ready
        if((inportb(0x64) & 1) != 0){
            scancode = inportb(0x60);
            break;
        }
    }

    if(i > 0) {
        if(scancode & 0x80) {
            // Key release
        }
        else if (scancode == 72) // up arrow
        {
            DispatchKernel(&kernel.service_transporter, console_t, clear_screen);
        } 
        else if (scancode == 80) // down arrow
        {
            uint8_t ipAddress[4];
            ipAddress[0] = 192;
            ipAddress[1] = 168;
            ipAddress[2] = 1;
            ipAddress[3] = 2;

            RequestMACAddress(kernel.e1000, ipAddress);
        }
        else if (scancode == 75) // left
        {
            if (kernel.console.params.newIndex == 2)
                kernel.console.params.newIndex = 0;
            else
                kernel.console.params.newIndex = (kernel.console.current_terminal_index + kernel.console.terminals_max_count - 1) % kernel.console.terminals_max_count;
            DispatchKernel(&kernel.service_transporter, console_t, switch_terminal);
        }
        else if (scancode == 77) // right
        {
            kernel.console.params.newIndex = (kernel.console.current_terminal_index + 1) % kernel.console.terminals_max_count;
            DispatchKernel(&kernel.service_transporter, console_t, switch_terminal);           
        }
    }
    sendAPICEOI(&kernel.apicManager.apics[0]);
}
void keyboard_init()
{
    uint8_t irq_num = 32 + 1;
    mapAPICIRQ(&kernel.apicManager.apics[0], irq_num - IRQ0, irq_num);

    kernel.interruptManager.params.p_interruptNumber = irq_num;
    kernel.interruptManager.params.p_interruptHandler = keyboard_handler;
    DispatchKernel(&kernel.service_transporter, interruptManager_t, register_interrupt);
}

