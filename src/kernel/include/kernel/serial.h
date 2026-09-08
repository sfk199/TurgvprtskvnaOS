#ifndef KERNEL_SERIAL
#define KERNEL_SERIAL

#include <stdbool.h>

void serial_writestring (const char* str);
bool serial_initialized ();
void serial_initialize ();

#endif