// Pretty much everything is copy-pasted from:
// https://wiki.osdev.org/8259_PIC
// As noted on https://wiki.osdev.org/APIC
// Disable the 8259 PIC properly. This is nearly as important as setting up the APIC. You do this in two steps: masking all interrupts and remapping the IRQs

#include <kernel/devices/io.h>
#include <kernel/log.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define CASCADE_IRQ 2

/*
arguments:
    offset1 - vector offset for master PIC
        vectors on the master become offset1..offset1+7
    offset2 - same for slave PIC: offset2..offset2+7
*/
static void pic_remap (int offset1, int offset2) {
    outb (PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait ();
    outb (PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait ();
    outb (PIC1_DATA, offset1);                   // ICW2: Master PIC vector offset
    io_wait ();
    outb (PIC2_DATA, offset2);                   // ICW2: Slave PIC vector offset
    io_wait ();
    outb (PIC1_DATA, 1 << CASCADE_IRQ);          // ICW3: tell Master PIC that there is a slave PIC at IRQ2
    io_wait ();
    outb (PIC2_DATA, CASCADE_IRQ);               // ICW3: tell Slave PIC its cascade identity
    io_wait ();
    
    outb(PIC1_DATA, ICW4_8086);                  // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    io_wait ();
    outb(PIC2_DATA, ICW4_8086);
    io_wait ();

    // Unmask both PICs.
    outb (PIC1_DATA, 0);
    outb (PIC2_DATA, 0);
    io_wait ();
}

static void pic_disable () {
    outb (PIC1_DATA, 0xff);
    outb (PIC2_DATA, 0xff);
}

void pic_initialize () {
    // 0 - 31 (0x00 - 0x1F) is reserved by intel. remap to 0x20 and 0x28
    pic_remap (0x20, 0x28);
    // This enables PICs, we should disable it for APIC (not sure if its safe to remove unmasking part, just to be safe)
    pic_disable ();
    
    kernel_printf ("[  INFO  ] PICs remapped and disabled!\n");
}