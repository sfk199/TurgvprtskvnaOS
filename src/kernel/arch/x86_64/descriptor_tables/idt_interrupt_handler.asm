section .text
global idt_flush_idtr

; Also read:
; https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/05_InterruptHandling.md

extern idt_interrupt_dispatch

idt_interrupt_stub:
   push rax
   push rbx
   push rcx
   push rdx
   push rsi
   push rdi
   push rsp
   push rbp
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15

   mov rdi, rsp
   call idt_interrupt_dispatch
   mov rsp, rax

   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rbp
   pop rsp
   pop rdi
   pop rsi
   pop rdx
   pop rcx
   pop rbx
   pop rax

   add rsp, 16

   iret

%assign i 0 
%rep 256

align 16
global vector_%+i%+_handler
vector_%+i%+_handler:
   ; some vectors dont have error code
%if (i <= 7) || (i == 8) || (10 <= i && i <= 14) || (i == 17) || (i == 21) || (i == 29) || (i == 30) || (i >= 32)
   push 0
%endif
   ; vector number
   push i
   jmp idt_interrupt_stub

%assign i i+1
%endrep