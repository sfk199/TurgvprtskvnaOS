#ifndef KERNEL_LOG
#define KERNEL_LOG

void kernel_writestring (const char* data);
void kernel_putchar (const char data);
void kernel_printf (const char* format, ...);

#endif