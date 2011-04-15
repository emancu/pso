%include "bios.mac"
%include "fat12.mac"
; %include "definiciones.asm"

; Posiciones donde se cargan las cosas
;%define ADDR_DATA  0xB000
%define SEG_DATA   0x9B00
%define ADDR_STACK 0x5000 
%define ADDR_KERN  0x1200
%define ADDR_BOOT  0x1000
%define ADDR_BOOTSEC 0x7c00
%define SEG_VIDEO  0xB800

BITS 16
ORG ADDR_BOOT
BOOT:
	cli
	JMP short start ;Jump over FAT12 struct

TIMES 0x40 - ($ - $$) DB 0x90 ; NOPs

LOADING: DB "Loading ["
disk_err_reset:
	; Reset Floppy A
	xor dx, dx
	xor ah, ah
	int 0x13
	ret
TIMES 11 - ($ - disk_err_reset) DB " "
	DB "]"
LOADING_LEN equ $-LOADING

start:
	; Copia el boot sector a un lugar sensato
	mov cx, 512
	mov si, ADDR_BOOTSEC
	mov di, ADDR_BOOT
	rep movsb
	jmp main-(ADDR_BOOTSEC-ADDR_BOOT)

main:
	bios_clrscr 0

	bios_write LOADING, LOADING_LEN, 0x07, 0, 0
	bios_write BOOT+FAT12_VolumeLabel, 11, 0x0f, 9, 0

	; Pila de 512 bytes en un lugar que no jode
	mov ax, SEG_DATA
	mov es, ax
	mov ss, ax
	mov sp, ADDR_STACK - 4

	; Calcula el espacio ocupado por las FATs
	mov ax, WORD [BOOT+FAT12_SectorsPerFAT]
	mul BYTE [BOOT+FAT12_NumberOfFATs]
	push ax; Salva el tamaño de las FATs

	; Calcula la cantidad de sectores en el Root dir
	mov cx, [BOOT+FAT12_RootEntries]
	shr cx, 4 ; Cada entrie ocupa 32 bytes, entonces hay 16 entries por sector

	add cx, ax
	mov bl, cl

	push cx ; Number of sectors to read
	mov bp, 20; Number of disk-read retries
	; Levanta las dos FATs a memoria y el root directory completo ((9+9+14)*512 = 16KB)
_load_FAT:
	pop ax
	push ax

	mov cx, 2 ; Sector 2, Cilindro 0, Cabeza 0, floppy 0
	xor dx, dx
	mov ah, 0x02
	xor bx, bx ; Levanta en la dirección es:bx
	int 0x13

	jnc _load_FAT_ok
	; disk fail
	call disk_err_reset
	dec bp
	jnz _load_FAT
_load_FAT_fail:
	; Bocha errores de disco
	mov di, msg_disk_error
	jmp blue_screen
msg_disk_error: db "Floppy error",0

KERNEL: db "KERNEL  BIN"  ; Archivo que estamos buscando

_load_FAT_ok:
	pop ax ; Load junk

	cld
	; Busca el kernel.bin en el root directory
	pop bx ; recupera el tamaño de las FATs
	shl bx, 9 ; 512 bytes per sector
	
	mov dx, [BOOT+FAT12_RootEntries]
_file_search_loop:
	mov si, KERNEL
	mov di, bx
	cmp BYTE [es:di], 0x00 
	je _file_not_found ; Si es cero entonces todas las entries de ahí pa abajo son "free"
	mov cx, 11
	repe cmpsb
	je _file_found
	
	add bx, FAT_DE__ ; Skip one entrie
	dec dx
	jnz _file_search_loop
	jmp _file_not_found
	
_file_not_found:
	bios_disk_park 0x00
	mov di, msg_notfound
	jmp blue_screen

_file_found: ; Found at BX addr
	mov ax, [es:bx+FAT_DE_FirstCluster]
	push ax
	mov di, ADDR_KERN >> 4
	mov es, di
	; Destination: 0x0000:ADDR_KERN

_kern_load_loop_err:
	call disk_err_reset
	pop ax
_kern_load_loop:
	push ax
	add ax, 0x1f ; Offset que saltea los sectores con headers
;%macro bios_disk_read 3 ; LBA sector, es:dest_addr, n_sectors
	bios_disk_read ax, 0, 1
	jc _kern_load_loop_err
	add di, 0x200 >> 4
	mov es, di
	pop bx
	mov cx, bx
	shr bx, 1
	add bx, cx
	mov ax, [ss:bx]
	and cl, 0x01
	jz _kern_load_noshift
	shr ax, 4
_kern_load_noshift:
	and ah, 0x0F
	cmp ax, 0x0FF8
	jb _kern_load_loop

_kern_load_end:
	jmp ADDR_KERN

;msg_notfound: db "File KERNEL.BIN not found! Panic Attack!",0
msg_notfound: db "KERNEL.BIN not found! Panic Attack!",0

%define scr_color 0x1f

blue_screen: ; recives null terminated string in stack
	push di
	bios_clrscr scr_color
	pop di
	mov si, di
	; strlen
	xor ax, ax
	mov es, ax
	xor cx, cx
	dec cx
	repnz scasb
	not cx
	dec cx

	push cx
	; Draw box (5 x (cx+2))

	xor dx, dx
	mov ax, SEG_VIDEO
	mov es, ax
	mov di, (10 * 80) * 2
	mov dl, 80
	add cl, 2
	sub dl, cl
	mov bx, dx
	and dl, -2
	add di, dx

	sub bl, 2
	shl bl, 1
	mov ax, (scr_color << 8) | 201
	stosw
	mov al, 205
	rep stosw
	mov al, 187
	stosw
	add di, bx
	mov al, 186
	stosw
	inc di
	inc di
	pop cx
	mov dx, cx
	add dl, 0x2
	mov ah, scr_color | 0x80
_blue_loop:
	lodsb
	stosw
	loop _blue_loop
	mov ah, scr_color
	inc di
	inc di
	mov al, 186
	stosw
	add di, bx
	mov cx, dx
	mov al, 200
	stosw
	mov al, 205
	rep stosw
	mov al, 188
	stosw
	jmp SHORT $

TIMES 0200h - 2 - ($ - $$) DB 0 ;Zerofill up to 510 bytes

DW 0AA55h ;Boot Sector signature

;OPTIONAL:
;To Zerofill up to the size of a standard 1.44MB, 3.5" floppy disk
;TIMES 1474560 - ($ - $$) DB 0
