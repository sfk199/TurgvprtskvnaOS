#include <kernel/terminal.h>
#include <kernel/serial.h>

void kernel_writestring (const char* data) {
    if (terminal_initialized ())
        terminal_writestring (data);
    if (serial_initialized ())
        serial_writestring (data);
}