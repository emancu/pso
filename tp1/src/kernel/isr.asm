[bits 32]

[GLOBAL timerTick]
[EXTERN isr_timer_tick]

[GLOBAL keyboard]
[EXTERN isr_keyboard]

[GLOBAL debug_kernel_panic]
[EXTERN debug_kernelpanic]

timerTick:
    cli
    pushad

    ;llamamos a la rutina en C
    call isr_timer_tick

    popad
    sti
    iret

keyboard:
    cli
    pushad

    ;llamamos a la rutina en C
    call isr_keyboard

    popad
    sti
    iret

debug_kernel_panic:
  cti;
  pushad;

  push dword 0xF; org_ss
  push dword 0x9;org_esp;
  push dword 0xa;eflags;
  push dword 0xA;org_cs;
  push dword 0xA;org_eip;
  push dword 0xA;errcode;
  push eax;
  push ecx;
  push edx;
  push ebx;
  push esp;
  push ebp;
  push esi;
  push edi;
  push ss;
  push ds;
  push es;
  push fs;
  push gs;


  push esp; puntero a la estructura
  mov eax, esp
  sub eax, 4*28
  push eax; puntero al stack

  call debug_kernelpanic

  xchg bx,bx

  add esp, 4*21; borramos los push

  popad;
  sti;
  iret;
