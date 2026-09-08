#include <kernel/limine/limine.h>
#include <kernel/framebuffer.h>
#include <kernel/terminal.h>

#include <hcf.h>

void kernel_main () {
    limine_initialize ();
    framebuffer_initialize();
    terminal_initialize();

    hcf ();
}