; PSO head
; Define un header del formato de archivo PSO

extern main
extern _end ; Esto es una cosa que define el ld (o alguien) que apunta al final del espacio de memoria
extern __pso_end_disk ; Esto se define en pso_tail

section .text
__pso_start:
	db "PSO",0   ; Tenemos magic chars y todo!
	dd __pso_start    ; Comienzo del espacio virtual
	dd __pso_end_disk ; Fin del espacio virtual en disco
	dd _end           ; Fin del espacio virtual usado por la app
	dd _start      ; Entry point (definido como main(void))

_start:
	push DWORD 0 ; argc
	push DWORD 0 ; argv
	push DWORD 0 ; env
	call main
	; exit(eax)
	; Completar aqu'i la forma en la que llame a la syscall loader_exit(eax)
	jmp $
