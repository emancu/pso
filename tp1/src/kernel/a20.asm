BITS 16
%include "bios.mac"

section .text

habilitando: db 'Habilitando A20........'
habilitando_len equ $ - habilitando

deshabilitando: db 'Desabilitando A20......'
deshabilitando_len equ $ - deshabilitando

checkeando: db 'Checkeando A20.........'
checkeando_len equ $ - checkeando

mensajeOK: db 'OK!'
mensajeOK_len equ $ - mensajeOK

mensajeFAIL: db 'FALLO!'
mensajeFAIL_len equ $ - mensajeFAIL

; Rutina para deshabilitar la a 20
; No toca ningun flag y deja los registros tal cual estaban
global disable_A20
disable_A20:
		pushf
		pusha
		bios_write deshabilitando, deshabilitando_len, 0x07, 0, 3
        call    a20wait
        mov     al,0xAD
        out     0x64,al
        call    a20wait
        mov     al,0xD0
        out     0x64,al
        call    a20wait2
        in      al,0x60
        push    ax
        call    a20wait
        mov     al,0xD1
        out     0x64,al
        call    a20wait
        pop     ax
        and     al,0xFD		;si pongo en 0 el bit 1, deshabilito
        out     0x60,al
        call    a20wait
        mov     al,0xAE
        out     0x64,al
        call    a20wait
        bios_write mensajeOK, mensajeOK_len, 0x0A, 0, 4
		popa
		popf
		ret

; Rutina para habilitar la a20
; No toca ningun flag y deja los registros tal cual estaban
global enable_A20
enable_A20:
		pushf
		pusha
		bios_write habilitando, habilitando_len, 0x07, 0, 3
        call    a20wait
        mov     al,0xAD
        out     0x64,al
        call    a20wait
        mov     al,0xD0
        out     0x64,al
        call    a20wait2
        in      al,0x60
        push    ax
        call    a20wait
        mov     al,0xD1
        out     0x64,al
        call    a20wait
        pop     ax
        or      al,2		;si pongo en 1 el bit 1, habilito
        out     0x60,al
        call    a20wait
        mov     al,0xAE
        out     0x64,al
        call    a20wait
        bios_write mensajeOK, mensajeOK_len, 0x0A, 0, 4
		popa
		popf
		ret
 
a20wait:
        in      al,0x64
        test    al,2
        jnz     a20wait
        ret
 
a20wait2:
        in      al,0x64
        test    al,1
        jz      a20wait2
        ret


; Rutina para checkear la a20
; No toca ningun flag y deja los registros tal cual estaban
; La idea es:
;		escribir 0x00 en la posicion 0x0000:0x0500 (direccion fisica 0x00000500)
;		escribir 0xFF en la posicion 0xFFFF:0x0510 (direccion fisica 0x00100500)
;		leer la posicion 0x0000:0x5000 (direccion fisica 0x00000500)
;		si es un 0x00 entonces no hay wraparround, es decir, la A20 esta habilitada
;		si es un 0xFF entonces hay wraparound, es decir, los pines 20 en adelante estan harcodeados a 0.
;		
global check_A20
check_A20:
	    pushf
	    push 	fs
	    push 	gs
	    push 	di
	    push 	si
		bios_write checkeando, checkeando_len, 0x07, 0, 3
	    xor 	ax, ax ; ax = 0
    	mov 	fs, ax
	    not 	ax ; ax = 0xFFFF
	    mov 	gs, ax
	    mov 	di, 0x0500
	    mov 	si, 0x0510
	    mov 	al, byte [fs:di]
	    push 	ax
	    mov 	al, byte [gs:si]
	    push	ax
	    mov 	byte [fs:di], 0x00
    	mov 	byte [gs:si], 0xFF
	    cmp 	byte [fs:di], 0xFF
	 	je 		.falla
		bios_write mensajeOK, mensajeOK_len, 0x0A, 0, 4
		jmp .sigue
		.falla:
			bios_write mensajeFAIL, mensajeFAIL_len, 0x0C, 0, 4
			jmp .falla
		.sigue:
	    pop 	ax
    	mov 	byte [gs:si], al
	    pop 	ax
	    mov 	byte [fs:di], al
	    mov 	ax, 0
	    je 		check_a20__exit
	    mov 	ax, 1

check_a20__exit:
	    pop 	si
	    pop 	di
	    pop 	gs
	    pop 	fs
	    popf
		ret


