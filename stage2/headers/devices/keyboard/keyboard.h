#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "Kernel.h"

void keyboard_handler(InterruptContext * r);

void keyboard_init();

#endif
