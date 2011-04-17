[bits 32]

[GLOBAL timerTick]
[EXTERN isr_timer_tick]

[GLOBAL keyboard]
[EXTERN isr_keyboard]


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
