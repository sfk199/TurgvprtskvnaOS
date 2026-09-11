#include <kernel/log.h>
#include <stdint.h>

// Also read:
// https://wiki.osdev.org/Global_Descriptor_Table
// https://wiki.osdev.org/GDT_Tutorial
// https://github.com/Bowlslaw/osdev-tutorial/blob/master/4-gdt.md

/*
    a.k.a. GDT Descriptor

    ┌─────────────────┬──────────────┐
    │ 79           16 │ 15         0 │
    ├─────────────────┼──────────────┤
    │ Offset (64bit)  │ size (16bit) │
    └─────────────────┴──────────────┘
    Size  : Size of table in bytes **minus 1**.
    Offset: Linear address of GDT
*/
struct gdtr_struct {
    uint16_t size;
    uint64_t offset;
} __attribute__ ((packed));
typedef struct gdtr_struct gdtr_t;


/*
    a.k.a. Segment Descriptor
                   ┌──── 8bit ─────┐
    ┌──────────────┬───────┬───────┬──────────────┬──────────────┐
    │ 63        56 │ 55 52 │ 51 48 │ 47        40 │ 39        32 │
    ├──────────────┼───────┼───────┼──────────────┼──────────────┤
    │     Base     │ Flags │ Limit │ Access Byte  │     Base     │
    ├──────────────┴───────┴───────┼──────────────┴──────────────┤
    │ 31                        16 │ 15                        0 │
    ├──────────────────────────────┼─────────────────────────────┤
    │             Base             │            Limit            │
    └──────────────────────────────┴─────────────────────────────┘

    Base : 0 in long mode, in protected: A 32-bit value containing linear address where segment begins. 
    Limit: 0xFFFFF in long mode, in protected: A 20-bit value, tells the maximum addressable unit, either in 1 byte units, or in 4KiB pages. (depends on granularity)
    In 64-bit mode, the Base and Limit values are ignored, each descriptor covers the entire linear address space regardless of what they are set to. 
*/
struct gdt_entry_struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access_byte;
    uint8_t  limit_high_flags; // limit_high first, then flags
    uint8_t  base_high;
} __attribute__ ((packed));
typedef struct gdt_entry_struct gdt_entry_t;


void gdt_flush_gdtr (const gdtr_t* gdtr);

static void gdt_populate_entry (gdt_entry_t* entry, uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t flags) {
    entry->base_low    = base & 0xFFFF;
    entry->base_middle = (base >> 16) & 0xFF;
    entry->base_high   = (base >> 24) & 0xFF;

    entry->limit_low        = limit & 0xFFFF;
    entry->limit_high_flags =
        ((limit >> 16) & 0x0F) | ((flags & 0x0F) << 4);
    
    entry->access_byte = access_byte;
}

gdt_entry_t gdt_entries[5];
gdtr_t      gdt_gdtr;

void gdt_initialize () {
    gdt_populate_entry (&gdt_entries[0], 0, 0x00000, 0x00, 0x0); // Null
    gdt_populate_entry (&gdt_entries[1], 0, 0xFFFFF, 0x9A, 0xA); // ring0 code segment
    gdt_populate_entry (&gdt_entries[2], 0, 0xFFFFF, 0x92, 0xC); // ring0 data segment
    gdt_populate_entry (&gdt_entries[3], 0, 0xFFFFF, 0xF2, 0xC); // ring3 data segment
    gdt_populate_entry (&gdt_entries[4], 0, 0xFFFFF, 0xFA, 0xA); // ring3 code segment

    gdt_gdtr.offset = (uint64_t)gdt_entries;
    gdt_gdtr.size = sizeof (gdt_entries) - 1;
    gdt_flush_gdtr (&gdt_gdtr);

    kernel_writestring ("[  INFO  ] GDT Initialized!\n");
}