; Tabla de syscalls
;
%define PUSHAD_LEN 8*4

; Este macro genera la definicion de una syscall y hace la interrupcion
; Recibe el nombre de la funcion de usuario de la syscall y el
; número de syscall
%macro sys_def 2-3
%1:
[EXTERN sys_%1]
;para que es esta declaracion martin??? ale (10/5)
[GLOBAL %1]
;TODO VER SI NO HAY QUE HACER PUSHAD PARA GUARDAR TODO
  ;pushad
  ;xchg bx,bx

  ;para los parametros de las syscalls cuando se usa pushad
  ;mov edx, [esp + 36]
  ;mov ecx, [esp + 40]
  ;mov ebx, [esp + 44]
  ;mov edi, [esp + 48]

  ;sin pushad (nesecito usar eax para el resultado)
  mov edx, [esp + 4]
  mov ecx, [esp + 8]
  mov ebx, [esp + 12]
  mov edi, [esp + 16]

  mov eax, %2
  ;xchg bx, bx
  int 0x30
  ;popad
  ret
%endmacro

sys_def palloc, 0x30

sys_def getpid, 0x31

sys_def exit, 0x32

sys_def read, 0x33

sys_def write, 0x34

sys_def seek, 0x35

sys_def close, 0x36

sys_def open, 0x37

;Para el fork lo hago aparte porque necesito el eip actual
[EXTERN sys_fork]
[GLOBAL fork]
fork:
 ;Pusheo el eip de la tarea al lo que luego es el primer parámetro (ver isr_syscall)
  mov edx, [esp] 

  mov eax, 0x38
  ;xchg bx, bx
  int 0x30
  ;popad
  ret

sys_def run, 0x39

