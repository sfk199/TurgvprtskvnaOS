#ifndef KERNEL_DEVICES_PIC
#define KERNEL_DEVICES_PIC

#include <stdint.h>
void pic_handle_irq (uint8_t irq_line); // Do not use, exported for idt.c
void pic_initialize ();

#endif