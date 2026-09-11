#ifndef KERNEL_DEVICES_SERIAL
#define KERNEL_DEVICES_SERIAL

#include <stdbool.h>
void serial_putchar (const char c);
void serial_writestring (const char* str);
bool serial_initialized ();
void serial_initialize ();

#endif