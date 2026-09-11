#include <kernel/limine/limine.h>
#include <kernel/devices/display/framebuffer.h>
#include <kernel/devices/display/terminal.h>
#include <kernel/devices/serial.h>

#include <kernel/memory/hhdm.h>
#include <kernel/memory/descriptor_tables.h>
#include <kernel/acpi.h>

#include <kernel/hcf.h>
#include <kernel/log.h>

void kernel_main () {
    limine_initialize ();
    framebuffer_initialize ();
    terminal_initialize ();
    serial_initialize ();

    hhdm_initialize ();
    gdt_initialize ();
    idt_initialize ();
    acpi_initialize();

    hcf ();
}