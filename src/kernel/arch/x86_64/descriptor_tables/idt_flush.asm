section .text
global idt_flush_idtr

idt_flush_idtr:
   lidt  [rdi]
   ret