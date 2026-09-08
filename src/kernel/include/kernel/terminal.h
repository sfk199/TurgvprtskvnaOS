#include <stddef.h>

void terminal_putchar (char symbol);
void terminal_writestring (const char* str);
void terminal_write (const char* data, size_t size);
void terminal_initialize ();