#include <kernel/devices/io.h>
#include <kernel/log.h>

#define PIC1        0x20
#define PIC2        0xA0
#define PIC1_CMD    PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_CMD    PIC2
#define PIC2_DATA   (PIC2+1)

#define PIC_EOI     0x20

// static const int_ctrl_t pic;

void pic_send_eoi(uint8_t irq)
{
    // TODO: Check if I have to shift the irq number
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
    io_wait();
}

void pic_init() {
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0b1);
    outb(PIC2_DATA, 0b1);

    outb(PIC1_DATA, 0b11111111);
    outb(PIC2_DATA, 0b11111111);

    // krnlctx(interrupt_controller) = &pic;
    // info(pic_init, "Initalized!");
}

void pic_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void irq_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

void irq_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

void pic_initialize () {
    pic_init();
    for (int i = 0; i <= 1; i++)
        irq_clear_mask (i);

    // really bad pic initialization code, ill rewrite it later
    outb (0x43, 0x36);          // ch0, lo/hi, mode 3
    outb (0x40, 0xFF);          // divisor lo
    outb (0x40, 0xFF);          // divisor hi -> ~18.2 Hz
    outb (0x21, 0x00);          // unmask IRQ0 only
    outb (0xA1, 0x00);
    
    // volatile int a = 0 / 0;

    asm ("sti");
    
    kernel_printf ("[  INFO  ] PICs remapped and disabled!\n");
}