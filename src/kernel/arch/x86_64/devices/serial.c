#include <kernel/devices/io.h>
#include <kernel/log.h>
#include <stdbool.h>
#include <stddef.h>

#define PORT 0x3f8 // COM1

static int serial_is_transmit_empty() { return inb (PORT + 5) & 0x20; }
void serial_putchar (const char c) {
    while (!serial_is_transmit_empty());
    outb (PORT, c);
}

void serial_writestring (const char* str) {
    for (size_t i = 0; str[i]; i++) serial_putchar (str[i]);
}

static bool initialized = false;
bool serial_initialized () { return initialized; }
void serial_initialize () {
    // https://wiki.osdev.org/Serial_Ports
    outb (PORT + 1, 0b00000000);  // Disable all interrupts
    outb (PORT + 3, 0b10000000);  // Enable DLAB (set baud rate divisor)
    outb (PORT + 0, 0b00000001);  // Set divisor to 1 (lo byte)
    outb (PORT + 1, 0b00000000);  //                  (hi byte) -> 115200 baud
    outb (PORT + 3, 0b00000011);  // 8 bits, no parity, one stop bit
    outb (PORT + 2, 0b00000111);  // Enable FIFO, clear them, with 14-byte threshold
    outb (PORT + 4, 0b00001011);  // IRQs enabled, RTS/DSR set
    outb (PORT + 4, 0b00011110);  // Set in loopback mode, test the serial chip
    outb (PORT + 0, 0xAE      );  // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb (PORT + 0) != 0xAE) {
        kernel_writestring ("[  WARN  ] Serial is faulty, is it connected?\n");
        return;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb (PORT + 4, 0b00001111);
    initialized = 1;
    
    // Clear serial output (e.g. from firmware logs)
    // Technically, \x1b[H should have been enough, but it is not
    serial_writestring ("\x1b[2J\x1b[H");
    kernel_writestring ("[  INFO  ] Serial initialized!\n");
}