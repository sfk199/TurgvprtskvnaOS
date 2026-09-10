section .text
global gdt_flush_gdtr

gdt_flush_gdtr:
   lgdt  [rdi]
   
   ; reload CS via far return
   ; Copy&Pasted from https://wiki.osdev.org/GDT_Tutorial
   push 0x08                 ; Push code segment to stack, 0x08 is a stand-in for your code segment
   lea rax, [rel .reload_cs] ; Load address of .reload_cs into RAX
   push rax                  ; Push this value to the stack
   retfq                     ; Perform a far return, RETFQ or LRETQ depending on syntax

.reload_cs:
   ; Reload data segment registers
   mov   ax, 0x10 ; 0x10 is a stand-in for your data segment
   mov   ds, ax
   mov   es, ax
   mov   fs, ax
   mov   gs, ax
   mov   ss, ax
   ret