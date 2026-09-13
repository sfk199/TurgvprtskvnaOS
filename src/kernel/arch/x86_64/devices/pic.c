#include <kernel/devices/io.h>
#include <kernel/log.h>

#define PIC1        0x20
#define PIC2        0xA0
#define PIC1_CMD    PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_CMD    PIC2
#define PIC2_DATA   (PIC2+1)

#define PIC_EOI     0x20

void pic_send_eoi (uint8_t irq) {
    // TODO: Check if I have to shift the irq number
    if (irq >= 8) outb (PIC2_CMD, PIC_EOI);
    outb (PIC1_CMD, PIC_EOI);
    io_wait ();
}

void pic_init () {
    outb (PIC1, 0x11);
    outb (PIC2, 0x11);
    io_wait ();

    outb (PIC1_DATA, 0x20);
    outb (PIC2_DATA, 0x28);
    io_wait ();

    outb (PIC1_DATA, 0x04);
    outb (PIC2_DATA, 0x02);
    io_wait ();

    outb (PIC1_DATA, 0b1);
    outb (PIC2_DATA, 0b1);
    io_wait ();

    // Masks everything
    outb (PIC1_DATA, 0xff);
    outb (PIC2_DATA, 0xff); 
    io_wait ();
}

void pic_irq_set_mask (uint8_t irq_line) {
    uint16_t port;

    if (irq_line < 8)
        port = PIC1_DATA;
    else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    outb (port, inb (port) | (1 << irq_line));
    io_wait ();
}

void pic_irq_clear_mask (uint8_t irq_line) {
    uint16_t port;

    if (irq_line < 8)
        port = PIC1_DATA;
    else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    outb (port, inb (port) & ~(1 << irq_line));
    io_wait ();
}

void pic_handle_irq (uint8_t irq_line) {
    kernel_printf ("[  WARN  ] Received IRQ %u (You should not have)\n", irq_line);
    pic_send_eoi (irq_line);
}

void pic_initialize () {
    pic_init (); // Inits, and masks everything    
    asm ("sti"); // Enable interrupts
    
    kernel_printf ("[  INFO  ] PICs remapped and disabled!\n");
}