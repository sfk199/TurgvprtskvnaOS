#ifndef KERNEL_DEVICES_IO
#define KERNEL_DEVICES_IO

#include <stdint.h>
void outb (uint16_t port, uint8_t val);
uint8_t inb (uint16_t port);
void io_wait ();

#endif