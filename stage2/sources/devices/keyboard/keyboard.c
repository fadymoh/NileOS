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
    //get scancode with "timeout"
    //printk("got into the keyboard handler\n");

    for(i = 1000; i > 0; i++) {
        // Check if scan code is ready
        if((inportb(0x64) & 1) == 0) continue;
        // Get the scan code
        scancode = inportb(0x60);
        break;
    }
   // printk("scanCode %d\n", scancode);

    if(i > 0) {
        if(scancode & 0x80) {
            // Key release
        }
        else if (scancode == 72) // up arrow
        {
            // // Key down
            // printk("Key pressed %c\n", kbdus[scancode]);
            printk_debug("a7la kalam wla eh\n");
            // // Send message to the focus window
        } else if (scancode == 80) // down arrow
        {
            printk_network("la msh a7la kalam\n");
        }
        else if (scancode == 75) // left
        {
            if (kernel.console.params.newIndex == 2)
                kernel.console.params.newIndex = 0;
            else
                kernel.console.params.newIndex = (kernel.console.current_terminal_index + kernel.console.terminals_max_count - 1) % kernel.console.terminals_max_count;
            dispatch_kernel(&kernel.service_transporter, console_t, switch_terminal);
        }
        else if (scancode == 77) // right
        {
            kernel.console.params.newIndex = (kernel.console.current_terminal_index + 1) % kernel.console.terminals_max_count;
            dispatch_kernel(&kernel.service_transporter, console_t, switch_terminal);           
        }
    }
}
void keyboard_init()
{
    uint8_t irq_num = 32 + 1;
    mapAPICIRQ(&kernel.apicManager.apics[0], irq_num - IRQ0, irq_num);

    kernel.interruptManager.params.p_interruptNumber = irq_num;
    kernel.interruptManager.params.p_interruptHandler = keyboard_handler;
    dispatch_kernel(&kernel.service_transporter, interruptManager_t, register_interrupt);
}

