#include <kernel/log.h>
#include <kernel/hcf.h>
#include <stddef.h>
#include <stdint.h>

// Also read:
// gdt.c
// https://wiki.osdev.org/Interrupt_Descriptor_Table
// https://wiki.osdev.org/Interrupts_Tutorial
// https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/05_InterruptHandling.md

/*
    a.k.a. IDT Descriptor

    ┌─────────────────┬──────────────┐
    │ 79           16 │ 15         0 │
    ├─────────────────┼──────────────┤
    │ Offset (64bit)  │ size (16bit) │
    └─────────────────┴──────────────┘
    Size  : Size of table in bytes
    Offset: Linear address of IDT
*/
struct idtr_struct {
    uint16_t size;
    uint64_t offset;
} __attribute__ ((packed));
typedef struct idtr_struct idtr_t;


/*
    a.k.a. Gate Descriptor

                                                                  ┌───────────── 8bit ───────────┐                   ┌── 3bit ──┐
    ┌───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
    │ 127                                                                                                                    96 │
    ├───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
    │                                                         Reserved                                                          │
    ├───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
    │ 95                                                                                                                     64 │
    ├───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
    │                                                          Offset                                                           │
    ├─────────────────────────────────────────────────────────────┬────┬────────┬────┬───────────┬───────────────────┬──────────┤
    │ 63                                                       48 │ 47 │ 46  45 │ 44 │ 43     40 │ 39             35 │ 34    32 │
    ├─────────────────────────────────────────────────────────────┼────┼────────┼────┼───────────┼───────────────────┼──────────┤
    │                           Offset                            │ P  │  DPL   │ 0  │ Gate Type │      Reserved     │   IST    │
    ├─────────────────────────────────────────────────────────────┼────┴────────┴────┴───────────┴───────────────────┴──────────┤
    │ 31                                                       16 │ 15                                                        0 │
    ├─────────────────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────┤
    │                      Segment Selector                       │                           Offset                            │
    └─────────────────────────────────────────────────────────────┴─────────────────────────────────────────────────────────────┘

    Offset: 64bit value. Address of the entry point of ISR (Interrupt Service Routine)
    Selector: A segment selector which must point to valid code segment in your GDT
    IST: 3bit value which is offset into IST, which is stored Task Stage Segment. Set to 0 because im not using hardware context switching.
    Gate Type: 4bit value which defines type of gate this IDT represents.
    - 0b1110 or 0xE: 64-bit Interrupt Gate
    - 0b1111 or 0xF: 64-bit Trap Gate
    DPL: A 2bit value which defines the CPU Privilege Levels which are allowed to access this interrupt via the INT instruction. Hardware interrupts ignore this mechanism.
    P: Present bit. Must be set (1) for the descriptor to be valid.
    
    64bit interrupt gate: 0b1001110 - 0x8E
                            P  Type
                             DLP
    64bit trap gate:      0b1001111 - 0x8F
*/
typedef struct {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t  ist; // Always 0
    uint8_t  type_attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__ ((packed)) idt_entry_t;

typedef struct {
    uint64_t rax;  uint64_t rbx;  uint64_t rcx;  uint64_t rdx;
    uint64_t rsi;  uint64_t rdi;  uint64_t rsp_; uint64_t rbp;
    uint64_t r8;   uint64_t r9;   uint64_t r10;  uint64_t r11;
    uint64_t r12;  uint64_t r13;  uint64_t r14;  uint64_t r15;


    uint64_t vector_number;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t flags;
    uint64_t rsp;
    uint64_t ss;
} idt_interrupt_cpu_status_t;

void idt_interrupt_dispatch (idt_interrupt_cpu_status_t* context) {
    switch (context->vector_number) {
        case 0:  kernel_writestring ("[  WARN  ] Interrupt: Divide by zero\n");                   break;
        case 1:  kernel_writestring ("[  WARN  ] Interrupt: Debug\n");                            break;
        case 2:  kernel_writestring ("[  WARN  ] Interrupt: Non-Maskable Interrupt\n");           break;
        case 3:  kernel_writestring ("[  WARN  ] Interrupt: Breakpoint\n");                       break;
        case 4:  kernel_writestring ("[  WARN  ] Interrupt: Overflow\n");                         break;
        case 5:  kernel_writestring ("[  WARN  ] Interrupt: Bound Range Exceeded\n");             break;
        case 6:  kernel_writestring ("[  WARN  ] Interrupt: Invalid Opcode\n");                   break;
        case 7:  kernel_writestring ("[  WARN  ] Interrupt: Device not available\n");             break;
        case 8:  kernel_writestring ("[  ERROR ] Interrupt: Double Fault\n");                     break;
        case 9:  kernel_writestring ("[  ERROR ] Interrupt: Unused (x87 Segment Overrun)\n");     break;
        case 10: kernel_writestring ("[  WARN  ] Interrupt: Invalid TSS\n");                      break;
        case 11: kernel_writestring ("[  WARN  ] Interrupt: Invalid Segment Not Present\n");      break;
        case 12: kernel_writestring ("[  WARN  ] Interrupt: Stack-Segment Fault\n");              break;
        case 13: kernel_writestring ("[  WARN  ] Interrupt: General Protection\n");               break;
        case 14: kernel_writestring ("[  WARN  ] Interrupt: Page Fault\n");                       break;
        case 15: kernel_writestring ("[  WARN  ] Interrupt: Intel reserved\n");                   break;
        case 16: kernel_writestring ("[  WARN  ] Interrupt: x87 FPU error\n");                    break;
        case 17: kernel_writestring ("[  WARN  ] Interrupt: Alignment Check\n");                  break;
        case 18: kernel_writestring ("[  ERROR ] Interrupt: Machine Check\n");                    break;
        case 19: kernel_writestring ("[  WARN  ] Interrupt: SIMD (SSE/AVX) error\n");             break;
        case 20: kernel_writestring ("[  WARN  ] Interrupt: Virtualization Exception\n");         break;
        case 21: kernel_writestring ("[  WARN  ] Interrupt: Control Protection Exception\n");     break;
        /* 22-27 Reserved for future use */
        case 22: kernel_writestring ("[  ERROR ] Interrupt: Reserved (22)\n");                    break;
        case 23: kernel_writestring ("[  ERROR ] Interrupt: Reserved (23)\n");                    break;
        case 24: kernel_writestring ("[  ERROR ] Interrupt: Reserved (24)\n");                    break;
        case 25: kernel_writestring ("[  ERROR ] Interrupt: Reserved (25)\n");                    break;
        case 26: kernel_writestring ("[  ERROR ] Interrupt: Reserved (26)\n");                    break;
        case 27: kernel_writestring ("[  ERROR ] Interrupt: Reserved (27)\n");                    break;

        case 28: kernel_writestring ("[  WARN  ] Interrupt: Hypervisor Injection Exception\n");   break;
        case 29: kernel_writestring ("[  WARN  ] Interrupt: VMM Communication Exception\n");      break;
        case 30: kernel_writestring ("[  WARN  ] Interrupt: Security Exception\n");               break;
        /*    31 Reserved for future use */
        case 31: kernel_writestring ("[  ERROR ] Interrupt: Reserved (31)\n");                    break;
        default: kernel_printf    ("[  DEBG  ] Interrupt: %u\n", context->vector_number);       break;
    }

    // Print some diagnostics info
    if (context->vector_number <= 31) {
        kernel_printf ("           Error code=0x%x, Vector number=%u\n", context->error_code, context->vector_number);
        kernel_printf ("           rip=0x%X   cs=0x%X  flg=0x%X  rsp=0x%X  ss=0x%X\n\n", context->rip, context->cs, context->flags, context->rsp, context->ss);
        kernel_printf ("           rax=0x%X  rbx=0x%X  rcx=0x%X  rdx=0x%X\n", context->rax, context->rbx, context->rcx, context->rdx);
        kernel_printf ("           rsi=0x%X  rdi=0x%X  rsp=0x%X  rbp=0x%X\n", context->rsi, context->rdi, context->rsp, context->rbp);
        kernel_printf ("            r8=0x%X   r9=0x%X  r10=0x%X  r11=0x%X\n", context->r8,  context->r9,  context->r10, context->r11);
        kernel_printf ("           r12=0x%X  r13=0x%X  r14=0x%X  r15=0x%X\n", context->r12, context->r13, context->r14, context->r15);
    }

    if (context->vector_number == 9 || context->vector_number == 15 || (22 <= context->vector_number && context->vector_number <= 27) || context->vector_number == 31)
        kernel_writestring ("[  INFO  ] Achievement Unlocked: How Did We Get Here?\n");
    if (context->vector_number == 8 || context->vector_number == 9 || context->vector_number == 18 || (22 <= context->vector_number && context->vector_number <= 27) || context->vector_number == 31)
        hcf();
    
    kernel_writestring ("[  TODO  ] Recover from traps/faults\n");   
    hcf ();
}

void idt_flush_idtr (const idtr_t* gdtr);

static void idt_populate_entry (idt_entry_t* entry, void* isr, uint8_t flags) {
    entry->offset_low    = (uint64_t)isr & 0xFFFF;
    entry->offset_middle = ((uint64_t)isr >> 16) & 0xFFFF;
    entry->offset_high   = ((uint64_t)isr >> 32) & 0xFFFFFFFF;

    entry->segment_selector = 0x08; // Second is ring0 code segment
    entry->ist = 0;
    entry->type_attributes = flags;
    entry->reserved = 0;
}

idt_entry_t idt_entries[256];
idtr_t      idt_idtr;

extern char vector_0_handler[];

void idt_initialize () {
    for (size_t i = 0; i < 256; i++)
        idt_populate_entry (&idt_entries[i], vector_0_handler + (i * 32), 0x8E);
    idt_idtr.offset = (uint64_t)idt_entries;
    idt_idtr.size = sizeof (idt_entries) - 1;
    idt_flush_idtr (&idt_idtr);

    kernel_writestring ("[  INFO  ] IDT Initialized!\n");
}