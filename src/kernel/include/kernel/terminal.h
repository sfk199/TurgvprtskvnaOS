#ifndef KERNEL_TERMINAL
#define KERNEL_TERMINAL

#include <stdbool.h>
#include <stddef.h>

void terminal_putchar (char symbol);
void terminal_writestring (const char* str);
bool terminal_initialized ();
void terminal_initialize ();

#endif