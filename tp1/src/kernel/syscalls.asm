; Tabla de syscalls
;

; Este macro genera la definicion de una syscall y hace la interrupcion
; Recibe el nombre de la funcion de usuario de la syscall y el 
; número de syscall
%macro sys_def 2
%1:
[EXTERN sys_%1]
[GLOBAL %1]
;  pushad
  mov eax, %2
  int 0x30
;  popad
  ret
%endmacro

sys_def palloc, 0x30
