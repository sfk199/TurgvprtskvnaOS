section .text
global idt_flush_idtr

; Also read:
; https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/05_InterruptHandling.md

extern idt_interrupt_dispatch

idt_interrupt_stub:
   push r15
   push r14
   push r13
   push r12
   push r11
   push r10
   push r9
   push r8
   push rbp
   push rsp
   push rdi
   push rsi
   push rdx
   push rcx
   push rbx
   push rax

   mov rdi, rsp
   sub rsp, 8 ; SysV ABI needs 16-aligned stack :pensive:
   call idt_interrupt_dispatch
   add rsp, 8
   mov rsp, rax

   pop rax
   pop rbx
   pop rcx
   pop rdx
   pop rsi
   pop rdi
   pop rsp
   pop rbp
   pop r8
   pop r9
   pop r10
   pop r11
   pop r12
   pop r13
   pop r14
   pop r15

   add rsp, 16

   iret

%assign i 0 
%rep 256

align 32
global vector_%+i%+_handler
vector_%+i%+_handler:
   ; some vectors dont have error code
%if (i <= 7) || (i == 9) || (i == 16) || (18 <= i && i <= 20) || (i == 28) || (i >= 32)
   push 0
%endif
   ; vector number
   push i
   jmp idt_interrupt_stub

%assign i i+1
%endrep