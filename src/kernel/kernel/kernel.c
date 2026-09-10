#include <kernel/limine/limine.h>
#include <kernel/framebuffer.h>
#include <kernel/terminal.h>
#include <kernel/serial.h>
#include <kernel/descriptor_tables.h>

#include <kernel/hcf.h>
#include <kernel/log.h>

void kernel_main () {
    limine_initialize ();
    framebuffer_initialize ();
    terminal_initialize ();
    serial_initialize ();
    gdt_initialize ();
    idt_initialize ();

    hcf ();
}