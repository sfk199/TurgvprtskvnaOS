#include <kernel/devices/display/terminal.h>
#include <kernel/devices/serial.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

void kernel_writestring (const char* data) {
    if (terminal_initialized ())
        terminal_writestring (data);
    if (serial_initialized ())
        serial_writestring (data);
}

void kernel_putchar (const char data) {
    if (terminal_initialized ())
        terminal_putchar (data);
    if (serial_initialized ())
        serial_putchar (data);
}

static char dec2hexchar[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

// I know... doesn't align with standard...
void kernel_printf (const char* format, ...) {
    va_list args;
    va_start (args, format);
    
    while (*format != '\0') {
        if (*format == '%') {
            format++;
            switch (*format) {
                case '\0':
                    break;
                case 'c':
                    kernel_putchar ((char) va_arg (args, int));
                    break;
                case 's':
                    kernel_writestring (va_arg (args, const char*));
                    break;
                case 'u': {
                    uint64_t num = va_arg (args, uint64_t);
                    char buf[33]; memset (buf, 0, 33);
                    for (size_t i = 31; ; i--) {
                        buf[i] = '0' + (num % 10); num /= 10;

                        if (num == 0) {
                            kernel_writestring (&buf[i]);
                            break;
                        }
                    }
                    break;
                }
                case 'x': {
                    uint64_t num = va_arg (args, uint64_t);
                    char buf[17]; memset (buf, 0, 17);
                    for (size_t i = 15; ; i--) {
                        buf[i] = dec2hexchar[num % 16]; num /= 16;

                        if (num == 0) {
                            kernel_writestring (&buf[i]);
                            break;
                        }
                    }
                    break;
                }
                case 'X': {
                    uint64_t num = va_arg (args, uint64_t);
                    char buf[17]; memset (buf, 0, 17);
                    for (size_t i = 15; ; i--) {
                        buf[i] = dec2hexchar[num % 16]; num /= 16;
                        if (i == 0) break;
                    }
                    kernel_writestring (buf);
                    break;
                }
                case '%':
                    kernel_putchar (*format);
                    break;
                default:
                    kernel_putchar ('%');
                    kernel_putchar (*format);
                    break;
            }
        } else
            kernel_putchar (*format);
        format++;
    }
}