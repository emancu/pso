[bits 32]

syscall_dir: resq 1 

%include "interrupt.mac"

[EXTERN debug_kernelpanic]
[EXTERN syscall_list]

isr_define_ep isr_timerTick, isr_timerTick_c

isr_dkp_ep isr_softPanic, 0x39

;isr_dkp_ep isr_keyboard, 2
isr_define_ep isr_keyboard, isr_timerTick_c

[GLOBAL isr_syscall]
isr_syscall:
  xchg bx, bx
  lea eax, [syscall_list+eax*4]
  mov eax, [eax]
  mov [syscall_dir], eax
  call [syscall_dir]
  iret
