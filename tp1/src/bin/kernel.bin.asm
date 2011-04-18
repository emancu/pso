
bin/kernel.bin.elf:     file format elf32-i386


Disassembly of section .text:

00001200 <start>:
extern enable_A20

global start
; start MUST be at the very begining of this file
start: 
	call enable_A20
    1200:	e8 e5 00 0f 01       	call   10f12ea <_end+0x10eea8a>

	lgdt [GDT_DESC]
    1205:	16                   	push   ss
    1206:	40                   	inc    eax
    1207:	21 0f                	and    DWORD PTR [edi],ecx

	mov eax, cr0
    1209:	20 c0                	and    al,al
	or eax, 0x1
    120b:	66 0d 01 00          	or     ax,0x1
    120f:	00 00                	add    BYTE PTR [eax],al
	mov cr0, eax
    1211:	0f 22 c0             	mov    cr0,eax

	jmp 0x08:modo_protegido
    1214:	ea 19 12 08 00 66 b8 	jmp    0xb866:0x81219

00001219 <modo_protegido>:

BITS 32
modo_protegido:
	mov 	ax, 0x10
    1219:	66 b8 10 00          	mov    ax,0x10
	mov 	ds, ax
    121d:	8e d8                	mov    ds,eax
	mov 	es, ax
    121f:	8e c0                	mov    es,eax
	mov 	fs, ax
    1221:	8e e0                	mov    fs,eax
	mov 	ss, ax
    1223:	8e d0                	mov    ss,eax
	mov 	esp, 0xA0000
    1225:	bc 00 00 0a 00       	mov    esp,0xa0000
	mov 	ebp, 0xA0000
    122a:	bd 00 00 0a 00       	mov    ebp,0xa0000

	call 	kernel_init
    122f:	e8 ac 01 00 00       	call   13e0 <kernel_init>
	jmp $
    1234:	e9 fb ff ff ff       	jmp    1234 <modo_protegido+0x1b>
    1239:	90                   	nop
    123a:	90                   	nop
    123b:	90                   	nop
    123c:	90                   	nop
    123d:	90                   	nop
    123e:	90                   	nop
    123f:	90                   	nop

00001240 <habilitando>:
BITS 16
%include "bios.mac"

section .text

habilitando: db 'Habilitando A20........'
    1240:	48 61 62 69 6c 69 74 61 6e 64 6f 20 41 32 30 2e     Habilitando A20.
    1250:	2e 2e 2e 2e 2e 2e 2e                                .......

00001257 <deshabilitando>:
habilitando_len equ $ - habilitando

deshabilitando: db 'Desabilitando A20......'
    1257:	44 65 73 61 62 69 6c 69 74 61 6e 64 6f 20 41 32     Desabilitando A2
    1267:	30 2e 2e 2e 2e 2e 2e                                0......

0000126e <checkeando>:
deshabilitando_len equ $ - deshabilitando

checkeando: db 'Checkeando A20.........'
    126e:	43 68 65 63 6b 65 61 6e 64 6f 20 41 32 30 2e 2e     Checkeando A20..
    127e:	2e 2e 2e 2e 2e 2e 2e                                .......

00001285 <mensajeOK>:
checkeando_len equ $ - checkeando

mensajeOK: db 'OK!'
    1285:	4f 4b 21                                            OK!

00001288 <mensajeFAIL>:
mensajeOK_len equ $ - mensajeOK

mensajeFAIL: db 'FALLO!'
    1288:	46 41 4c 4c 4f 21                                   FALLO!

0000128e <disable_A20>:

; Rutina para deshabilitar la a 20
; No toca ningun flag y deja los registros tal cual estaban
global disable_A20
disable_A20:
		pushf
    128e:	9c                   	pushf  
		pusha
    128f:	60                   	pusha  
		bios_write deshabilitando, deshabilitando_len, 0x07, 0, 3
    1290:	b8 01 13 bb 07       	mov    eax,0x7bb1301
    1295:	00 b9 17 00 ba 00    	add    BYTE PTR [ecx+0xba0017],bh
    129b:	00 80 c6 03 bd 57    	add    BYTE PTR [eax+0x57bd03c6],al
    12a1:	12 cd                	adc    cl,ch
    12a3:	10 e8                	adc    al,ch
        call    a20wait
    12a5:	9b                   	fwait
    12a6:	00 b0 ad e6 64 e8    	add    BYTE PTR [eax-0x179b1953],dh
        mov     al,0xAD
        out     0x64,al
        call    a20wait
    12ac:	94                   	xchg   esp,eax
    12ad:	00 b0 d0 e6 64 e8    	add    BYTE PTR [eax-0x179b1930],dh
        mov     al,0xD0
        out     0x64,al
        call    a20wait2
    12b3:	94                   	xchg   esp,eax
    12b4:	00 e4                	add    ah,ah
        in      al,0x60
    12b6:	60                   	pusha  
        push    ax
    12b7:	50                   	push   eax
        call    a20wait
    12b8:	e8 87 00 b0 d1       	call   d1b01344 <_end+0xd1afeae4>
        mov     al,0xD1
        out     0x64,al
    12bd:	e6 64                	out    0x64,al
        call    a20wait
    12bf:	e8 80 00 58 24       	call   24581344 <_end+0x2457eae4>
        pop     ax
        and     al,0xFD		;si pongo en 0 el bit 1, deshabilito
    12c4:	fd                   	std    
        out     0x60,al
    12c5:	e6 60                	out    0x60,al
        call    a20wait
    12c7:	e8 78 00 b0 ae       	call   aeb01344 <_end+0xaeafeae4>
        mov     al,0xAE
        out     0x64,al
    12cc:	e6 64                	out    0x64,al
        call    a20wait
    12ce:	e8 71 00 b8 01       	call   1b81344 <_end+0x1b7eae4>
        bios_write mensajeOK, mensajeOK_len, 0x0A, 0, 4
    12d3:	13 bb 0a 00 b9 03    	adc    edi,DWORD PTR [ebx+0x3b9000a]
    12d9:	00 ba 00 00 80 c6    	add    BYTE PTR [edx-0x39800000],bh
    12df:	04 bd                	add    al,0xbd
    12e1:	85 12                	test   DWORD PTR [edx],edx
    12e3:	cd 10                	int    0x10
		popa
    12e5:	61                   	popa   
		popf
    12e6:	9d                   	popf   
		ret
    12e7:	c3                   	ret    

000012e8 <enable_A20>:

; Rutina para habilitar la a20
; No toca ningun flag y deja los registros tal cual estaban
global enable_A20
enable_A20:
		pushf
    12e8:	9c                   	pushf  
		pusha
    12e9:	60                   	pusha  
		bios_write habilitando, habilitando_len, 0x07, 0, 3
    12ea:	b8 01 13 bb 07       	mov    eax,0x7bb1301
    12ef:	00 b9 17 00 ba 00    	add    BYTE PTR [ecx+0xba0017],bh
    12f5:	00 80 c6 03 bd 40    	add    BYTE PTR [eax+0x40bd03c6],al
    12fb:	12 cd                	adc    cl,ch
    12fd:	10 e8                	adc    al,ch
        call    a20wait
    12ff:	41                   	inc    ecx
    1300:	00 b0 ad e6 64 e8    	add    BYTE PTR [eax-0x179b1953],dh
        mov     al,0xAD
        out     0x64,al
        call    a20wait
    1306:	3a 00                	cmp    al,BYTE PTR [eax]
        mov     al,0xD0
    1308:	b0 d0                	mov    al,0xd0
        out     0x64,al
    130a:	e6 64                	out    0x64,al
        call    a20wait2
    130c:	e8 3a 00 e4 60       	call   60e4134b <_end+0x60e3eaeb>
        in      al,0x60
        push    ax
    1311:	50                   	push   eax
        call    a20wait
    1312:	e8 2d 00 b0 d1       	call   d1b01344 <_end+0xd1afeae4>
        mov     al,0xD1
        out     0x64,al
    1317:	e6 64                	out    0x64,al
        call    a20wait
    1319:	e8 26 00 58 0c       	call   c581344 <_end+0xc57eae4>
        pop     ax
        or      al,2		;si pongo en 1 el bit 1, habilito
    131e:	02 e6                	add    ah,dh
        out     0x60,al
    1320:	60                   	pusha  
        call    a20wait
    1321:	e8 1e 00 b0 ae       	call   aeb01344 <_end+0xaeafeae4>
        mov     al,0xAE
        out     0x64,al
    1326:	e6 64                	out    0x64,al
        call    a20wait
    1328:	e8 17 00 b8 01       	call   1b81344 <_end+0x1b7eae4>
        bios_write mensajeOK, mensajeOK_len, 0x0A, 0, 4
    132d:	13 bb 0a 00 b9 03    	adc    edi,DWORD PTR [ebx+0x3b9000a]
    1333:	00 ba 00 00 80 c6    	add    BYTE PTR [edx-0x39800000],bh
    1339:	04 bd                	add    al,0xbd
    133b:	85 12                	test   DWORD PTR [edx],edx
    133d:	cd 10                	int    0x10
		popa
    133f:	61                   	popa   
		popf
    1340:	9d                   	popf   
		ret
    1341:	c3                   	ret    

00001342 <a20wait>:
 
a20wait:
        in      al,0x64
    1342:	e4 64                	in     al,0x64
        test    al,2
    1344:	a8 02                	test   al,0x2
        jnz     a20wait
    1346:	75 fa                	jne    1342 <a20wait>
        ret
    1348:	c3                   	ret    

00001349 <a20wait2>:
 
a20wait2:
        in      al,0x64
    1349:	e4 64                	in     al,0x64
        test    al,1
    134b:	a8 01                	test   al,0x1
        jz      a20wait2
    134d:	74 fa                	je     1349 <a20wait2>
        ret
    134f:	c3                   	ret    

00001350 <check_A20>:
;		si es un 0x00 entonces no hay wraparround, es decir, la A20 esta habilitada
;		si es un 0xFF entonces hay wraparound, es decir, los pines 20 en adelante estan harcodeados a 0.
;		
global check_A20
check_A20:
	    pushf
    1350:	9c                   	pushf  
	    push 	fs
    1351:	0f a0                	push   fs
	    push 	gs
    1353:	0f a8                	push   gs
	    push 	di
    1355:	57                   	push   edi
	    push 	si
    1356:	56                   	push   esi
		bios_write checkeando, checkeando_len, 0x07, 0, 3
    1357:	b8 01 13 bb 07       	mov    eax,0x7bb1301
    135c:	00 b9 17 00 ba 00    	add    BYTE PTR [ecx+0xba0017],bh
    1362:	00 80 c6 03 bd 6e    	add    BYTE PTR [eax+0x6ebd03c6],al
    1368:	12 cd                	adc    cl,ch
    136a:	10 31                	adc    BYTE PTR [ecx],dh
	    xor 	ax, ax ; ax = 0
    136c:	c0 8e e0 f7 d0 8e e8 	ror    BYTE PTR [esi-0x712f0820],0xe8
    	mov 	fs, ax
	    not 	ax ; ax = 0xFFFF
	    mov 	gs, ax
	    mov 	di, 0x0500
    1373:	bf 00 05 be 10       	mov    edi,0x10be0500
	    mov 	si, 0x0510
    1378:	05 64 8a 05 50       	add    eax,0x50058a64
	    mov 	al, byte [fs:di]
	    push 	ax
	    mov 	al, byte [gs:si]
    137d:	65 8a 04 50          	mov    al,BYTE PTR gs:[eax+edx*2]
	    push	ax
	    mov 	byte [fs:di], 0x00
    1381:	64 c6 05 00 65 c6 04 	mov    BYTE PTR fs:0x4c66500,0xff
    1388:	ff 
    	mov 	byte [gs:si], 0xFF
	    cmp 	byte [fs:di], 0xFF
    1389:	64 80 3d ff 74 17 b8 	cmp    BYTE PTR fs:0xb81774ff,0x1
    1390:	01 
	 	je 		.falla
		bios_write mensajeOK, mensajeOK_len, 0x0A, 0, 4
    1391:	13 bb 0a 00 b9 03    	adc    edi,DWORD PTR [ebx+0x3b9000a]
    1397:	00 ba 00 00 80 c6    	add    BYTE PTR [edx-0x39800000],bh
    139d:	04 bd                	add    al,0xbd
    139f:	85 12                	test   DWORD PTR [edx],edx
    13a1:	cd 10                	int    0x10
		jmp .sigue
    13a3:	e9 17 00 b8 01       	jmp    1b813bf <_end+0x1b7eb5f>

000013a6 <check_A20.falla>:
		.falla:
			bios_write mensajeFAIL, mensajeFAIL_len, 0x0C, 0, 4
    13a6:	b8 01 13 bb 0c       	mov    eax,0xcbb1301
    13ab:	00 b9 06 00 ba 00    	add    BYTE PTR [ecx+0xba0006],bh
    13b1:	00 80 c6 04 bd 88    	add    BYTE PTR [eax-0x7742fb3a],al
    13b7:	12 cd                	adc    cl,ch
    13b9:	10 e9                	adc    cl,ch
			jmp .falla
    13bb:	e9 ff 58 65 88       	jmp    88656cbf <_end+0x8865445f>

000013bd <check_A20.sigue>:
		.sigue:
	    pop 	ax
    13bd:	58                   	pop    eax
    	mov 	byte [gs:si], al
    13be:	65 88 04 58          	mov    BYTE PTR gs:[eax+ebx*2],al
	    pop 	ax
	    mov 	byte [fs:di], al
    13c2:	64 88 05 b8 00 00 74 	mov    BYTE PTR fs:0x740000b8,al
	    mov 	ax, 0
	    je 		check_a20__exit
    13c9:	03 b8 01 00 5e 5f    	add    edi,DWORD PTR [eax+0x5f5e0001]

000013cd <check_a20__exit>:
	    mov 	ax, 1

check_a20__exit:
	    pop 	si
    13cd:	5e                   	pop    esi
	    pop 	di
    13ce:	5f                   	pop    edi
	    pop 	gs
    13cf:	0f a9                	pop    gs
	    pop 	fs
    13d1:	0f a1                	pop    fs
	    popf
    13d3:	9d                   	popf   
		ret
    13d4:	c3                   	ret    
    13d5:	90                   	nop
    13d6:	90                   	nop
    13d7:	90                   	nop
    13d8:	90                   	nop
    13d9:	90                   	nop
    13da:	90                   	nop
    13db:	90                   	nop
    13dc:	90                   	nop
    13dd:	90                   	nop
    13de:	90                   	nop
    13df:	90                   	nop

000013e0 <kernel_init>:
#include "klib_machine.h"

extern void* _end;
/* Entry-point del modo protegido luego de cargar los registros de
 * segmento y armar un stack */
void kernel_init(void) {
    13e0:	55                   	push   ebp
    13e1:	89 e5                	mov    ebp,esp
    13e3:	83 ec 18             	sub    esp,0x18
  gdt_init();
    13e6:	e8 45 00 00 00       	call   1430 <gdt_init>
  clear_screen();
    13eb:	e8 70 02 00 00       	call   1660 <clear_screen>
  vga_write(vga_rows-1, vga_cols/4-1, " OS by: E. Marcusso, A. Mataloni, M. Miguel ", VGA_FC_BLUE | VGA_FC_LIGHT | VGA_BC_WHITE);
    13f0:	0f b7 05 28 1f 00 00 	movzx  eax,WORD PTR ds:0x1f28
    13f7:	c7 44 24 0c 79 00 00 	mov    DWORD PTR [esp+0xc],0x79
    13fe:	00 
    13ff:	c7 44 24 08 a0 1e 00 	mov    DWORD PTR [esp+0x8],0x1ea0
    1406:	00 
    1407:	66 c1 e8 02          	shr    ax,0x2
    140b:	83 e8 01             	sub    eax,0x1
    140e:	0f b7 c0             	movzx  eax,ax
    1411:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    1415:	0f b7 05 2a 1f 00 00 	movzx  eax,WORD PTR ds:0x1f2a
    141c:	83 e8 01             	sub    eax,0x1
    141f:	0f b7 c0             	movzx  eax,ax
    1422:	89 04 24             	mov    DWORD PTR [esp],eax
    1425:	e8 06 01 00 00       	call   1530 <vga_write>
  print_machine_stat();
  return;
}
    142a:	c9                   	leave  
 * segmento y armar un stack */
void kernel_init(void) {
  gdt_init();
  clear_screen();
  vga_write(vga_rows-1, vga_cols/4-1, " OS by: E. Marcusso, A. Mataloni, M. Miguel ", VGA_FC_BLUE | VGA_FC_LIGHT | VGA_BC_WHITE);
  print_machine_stat();
    142b:	e9 d0 09 00 00       	jmp    1e00 <print_machine_stat>

00001430 <gdt_init>:
};

gdt_descriptor GDT_DESC = {sizeof(gdt)-1, (uint_32)&gdt};

void gdt_init(void) {
  gdt[5] = make_gdt_entry(&tss_entry, sizeof(tss), GDT_ATTR_P | GDT_ATTR_DPL0 | GDT_ATTR_TYPE_TSS); // TSS
    1430:	ba e0 25 00 00       	mov    edx,0x25e0
    1435:	89 d0                	mov    eax,edx
    1437:	c1 e8 10             	shr    eax,0x10
    143a:	c7 05 2c 21 00 00 00 	mov    DWORD PTR ds:0x212c,0x0
    1441:	00 00 00 
    1444:	a2 2c 21 00 00       	mov    ds:0x212c,al
    1449:	0f b6 05 2d 21 00 00 	movzx  eax,BYTE PTR ds:0x212d
  // El resto no está inicializado ;)
};

gdt_descriptor GDT_DESC = {sizeof(gdt)-1, (uint_32)&gdt};

void gdt_init(void) {
    1450:	55                   	push   ebp
    1451:	89 e5                	mov    ebp,esp
  gdt[5] = make_gdt_entry(&tss_entry, sizeof(tss), GDT_ATTR_P | GDT_ATTR_DPL0 | GDT_ATTR_TYPE_TSS); // TSS
    1453:	66 89 15 2a 21 00 00 	mov    WORD PTR ds:0x212a,dx
    145a:	c1 ea 18             	shr    edx,0x18
    145d:	83 e0 f0             	and    eax,0xfffffff0
    1460:	83 c8 89             	or     eax,0xffffff89
    1463:	66 c7 05 28 21 00 00 	mov    WORD PTR ds:0x2128,0x68
    146a:	68 00 
    146c:	a2 2d 21 00 00       	mov    ds:0x212d,al
    1471:	88 15 2f 21 00 00    	mov    BYTE PTR ds:0x212f,dl

}
    1477:	5d                   	pop    ebp
    1478:	c3                   	ret    
    1479:	90                   	nop
    147a:	90                   	nop
    147b:	90                   	nop
    147c:	90                   	nop
    147d:	90                   	nop
    147e:	90                   	nop
    147f:	90                   	nop

00001480 <idt_init>:

idt_entry idt[128] = {};

idt_descriptor IDT_DESC = {sizeof(idt)-1, (uint_32)&idt};

void idt_init(void) {
    1480:	55                   	push   ebp
    1481:	89 e5                	mov    ebp,esp
	//Rellenar la IDT
	// Carga el IDTR
	return;
}
    1483:	5d                   	pop    ebp
    1484:	c3                   	ret    
    1485:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1489:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001490 <idt_register>:

void idt_register(int intr, void (*isr)(void), int pl ) {
    1490:	55                   	push   ebp
    1491:	89 e5                	mov    ebp,esp
	//Agregar la intr a la IDT
}
    1493:	5d                   	pop    ebp
    1494:	c3                   	ret    
    1495:	90                   	nop
    1496:	90                   	nop
    1497:	90                   	nop
    1498:	90                   	nop
    1499:	90                   	nop
    149a:	90                   	nop
    149b:	90                   	nop
    149c:	90                   	nop
    149d:	90                   	nop
    149e:	90                   	nop
    149f:	90                   	nop

000014a0 <pic_reset>:
#include <i386.h>

#define PIC1_PORT 0x20
#define PIC2_PORT 0xA0

void pic_reset(uint_8 addr_pic1, uint_8 addr_pic2) {
    14a0:	55                   	push   ebp

/*** Implementaciones Inline ***/

/* out** */
LS_INLINE void outb(int port, uint_8 data) {
  __asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
    14a1:	ba 20 00 00 00       	mov    edx,0x20
    14a6:	89 e5                	mov    ebp,esp
    14a8:	b8 11 00 00 00       	mov    eax,0x11
    14ad:	ee                   	out    dx,al
    14ae:	b2 21                	mov    dl,0x21
    14b0:	0f b6 45 08          	movzx  eax,BYTE PTR [ebp+0x8]
    14b4:	ee                   	out    dx,al
    14b5:	b8 04 00 00 00       	mov    eax,0x4
    14ba:	ee                   	out    dx,al
    14bb:	b8 01 00 00 00       	mov    eax,0x1
    14c0:	ee                   	out    dx,al
    14c1:	b8 ff ff ff ff       	mov    eax,0xffffffff
    14c6:	ee                   	out    dx,al
    14c7:	b8 11 00 00 00       	mov    eax,0x11
    14cc:	b2 a0                	mov    dl,0xa0
    14ce:	ee                   	out    dx,al
    14cf:	b2 a1                	mov    dl,0xa1
    14d1:	0f b6 45 0c          	movzx  eax,BYTE PTR [ebp+0xc]
    14d5:	ee                   	out    dx,al
    14d6:	b8 02 00 00 00       	mov    eax,0x2
    14db:	ee                   	out    dx,al
    14dc:	b8 01 00 00 00       	mov    eax,0x1
    14e1:	ee                   	out    dx,al
    14e2:	b8 ff ff ff ff       	mov    eax,0xffffffff
    14e7:	ee                   	out    dx,al
	outb(PIC2_PORT+1, 0x02); /* PIC2 Slave, ingresa Int x IRQ2 */
	outb(PIC2_PORT+1, 0x01); /* Modo 8086 */
	outb(PIC2_PORT+1, 0xFF); /* Enmasca todas! */

	return;
}
    14e8:	5d                   	pop    ebp
    14e9:	c3                   	ret    
    14ea:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

000014f0 <pic_enable>:

void pic_enable() {
    14f0:	55                   	push   ebp
    14f1:	ba 21 00 00 00       	mov    edx,0x21
    14f6:	89 e5                	mov    ebp,esp
    14f8:	31 c0                	xor    eax,eax
    14fa:	ee                   	out    dx,al
    14fb:	b2 a1                	mov    dl,0xa1
    14fd:	ee                   	out    dx,al
	outb(PIC1_PORT+1, 0x00);
	outb(PIC2_PORT+1, 0x00);
}
    14fe:	5d                   	pop    ebp
    14ff:	c3                   	ret    

00001500 <pic_disable>:

void pic_disable() {
    1500:	55                   	push   ebp
    1501:	ba 21 00 00 00       	mov    edx,0x21
    1506:	89 e5                	mov    ebp,esp
    1508:	b8 ff ff ff ff       	mov    eax,0xffffffff
    150d:	ee                   	out    dx,al
    150e:	b2 a1                	mov    dl,0xa1
    1510:	ee                   	out    dx,al
	outb(PIC1_PORT+1, 0xFF);
	outb(PIC2_PORT+1, 0xFF);
}
    1511:	5d                   	pop    ebp
    1512:	c3                   	ret    
    1513:	90                   	nop
    1514:	90                   	nop
    1515:	90                   	nop
    1516:	90                   	nop
    1517:	90                   	nop
    1518:	90                   	nop
    1519:	90                   	nop
    151a:	90                   	nop
    151b:	90                   	nop
    151c:	90                   	nop
    151d:	90                   	nop
    151e:	90                   	nop
    151f:	90                   	nop

00001520 <vga_init>:
const uint_16 vga_rows = 25;
uint_8* vga_limit = (uint_8*) 0xB8FA0;

uint_16 fila = 0;

void vga_init(void) {
    1520:	55                   	push   ebp
    1521:	89 e5                	mov    ebp,esp
}
    1523:	5d                   	pop    ebp
    1524:	c3                   	ret    
    1525:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1529:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001530 <vga_write>:

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
    1530:	55                   	push   ebp
    1531:	89 e5                	mov    ebp,esp
    1533:	8b 4d 10             	mov    ecx,DWORD PTR [ebp+0x10]
    1536:	57                   	push   edi
    1537:	0f b7 45 08          	movzx  eax,WORD PTR [ebp+0x8]
    153b:	56                   	push   esi
    153c:	0f b7 7d 0c          	movzx  edi,WORD PTR [ebp+0xc]
    1540:	53                   	push   ebx
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
    1541:	8b 35 6c 25 00 00    	mov    esi,DWORD PTR ds:0x256c
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    1547:	0f b6 11             	movzx  edx,BYTE PTR [ecx]
uint_16 fila = 0;

void vga_init(void) {
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
    154a:	0f b6 5d 14          	movzx  ebx,BYTE PTR [ebp+0x14]
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    154e:	84 d2                	test   dl,dl
    1550:	74 4e                	je     15a0 <vga_write+0x70>

void vga_init(void) {
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
    1552:	0f b7 c0             	movzx  eax,ax
    1555:	0f b7 ff             	movzx  edi,di
    1558:	8d 04 80             	lea    eax,[eax+eax*4]
    155b:	c1 e0 04             	shl    eax,0x4
    155e:	01 f8                	add    eax,edi
    1560:	8d 04 46             	lea    eax,[esi+eax*2]
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    1563:	3b 05 70 25 00 00    	cmp    eax,DWORD PTR ds:0x2570
    1569:	72 20                	jb     158b <vga_write+0x5b>
    156b:	eb 33                	jmp    15a0 <vga_write+0x70>
    156d:	8d 76 00             	lea    esi,[esi+0x0]
    if (msg[str] == '\n') { //Avanzo una línea el puntero
		video = (uint_8*)(video + (vga_cols * 2));
    } else { //Escribo en pantalla
      *video++ = msg[str];
    1570:	88 10                	mov    BYTE PTR [eax],dl
      *video++ = attr;
    1572:	88 58 01             	mov    BYTE PTR [eax+0x1],bl
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    1575:	0f b6 51 01          	movzx  edx,BYTE PTR [ecx+0x1]
    if (msg[str] == '\n') { //Avanzo una línea el puntero
		video = (uint_8*)(video + (vga_cols * 2));
    } else { //Escribo en pantalla
      *video++ = msg[str];
      *video++ = attr;
    1579:	83 c0 02             	add    eax,0x2
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    157c:	84 d2                	test   dl,dl
    157e:	74 20                	je     15a0 <vga_write+0x70>
    1580:	83 c1 01             	add    ecx,0x1
    1583:	3b 05 70 25 00 00    	cmp    eax,DWORD PTR ds:0x2570
    1589:	73 15                	jae    15a0 <vga_write+0x70>
    if (msg[str] == '\n') { //Avanzo una línea el puntero
    158b:	80 fa 0a             	cmp    dl,0xa
    158e:	75 e0                	jne    1570 <vga_write+0x40>
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    1590:	0f b6 51 01          	movzx  edx,BYTE PTR [ecx+0x1]
    if (msg[str] == '\n') { //Avanzo una línea el puntero
		video = (uint_8*)(video + (vga_cols * 2));
    1594:	05 a0 00 00 00       	add    eax,0xa0
}

void vga_write(uint_16 f, uint_16 c, const char* msg, uint_8 attr) {
  uint_8* video = (uint_8*)(vga_addr + vga_cols * 2 * f + c * 2);
  int str = 0;
  while(msg[str] != '\0' && video < vga_limit) {
    1599:	84 d2                	test   dl,dl
    159b:	75 e3                	jne    1580 <vga_write+0x50>
    159d:	8d 76 00             	lea    esi,[esi+0x0]
      *video++ = msg[str];
      *video++ = attr;
    }
    str++;
  }
}
    15a0:	5b                   	pop    ebx
    15a1:	5e                   	pop    esi
    15a2:	5f                   	pop    edi
    15a3:	5d                   	pop    ebp
    15a4:	c3                   	ret    
    15a5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    15a9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000015b0 <move_scr_up>:
  }
}

void move_scr_up() {
  uint_8* video = vga_addr;
  while (video < (vga_limit - (vga_cols*2))) {
    15b0:	8b 0d 70 25 00 00    	mov    ecx,DWORD PTR ds:0x2570
    amount *= 2;
  }
}

void move_scr_up() {
  uint_8* video = vga_addr;
    15b6:	a1 6c 25 00 00       	mov    eax,ds:0x256c
    va_start(argp, attr);
    amount *= 2;
  }
}

void move_scr_up() {
    15bb:	55                   	push   ebp
    15bc:	89 e5                	mov    ebp,esp
  uint_8* video = vga_addr;
  while (video < (vga_limit - (vga_cols*2))) {
    15be:	81 e9 a0 00 00 00    	sub    ecx,0xa0
    15c4:	39 c8                	cmp    eax,ecx
    15c6:	89 c2                	mov    edx,eax
    15c8:	73 28                	jae    15f2 <move_scr_up+0x42>
    15ca:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    *video = *(video+(vga_cols*2));
    15d0:	0f b6 90 a0 00 00 00 	movzx  edx,BYTE PTR [eax+0xa0]
    15d7:	88 10                	mov    BYTE PTR [eax],dl
  }
}

void move_scr_up() {
  uint_8* video = vga_addr;
  while (video < (vga_limit - (vga_cols*2))) {
    15d9:	8b 15 70 25 00 00    	mov    edx,DWORD PTR ds:0x2570
    *video = *(video+(vga_cols*2));
    video++;
    15df:	83 c0 01             	add    eax,0x1
  }
}

void move_scr_up() {
  uint_8* video = vga_addr;
  while (video < (vga_limit - (vga_cols*2))) {
    15e2:	81 ea a0 00 00 00    	sub    edx,0xa0
    15e8:	39 d0                	cmp    eax,edx
    15ea:	72 e4                	jb     15d0 <move_scr_up+0x20>
    15ec:	8b 15 6c 25 00 00    	mov    edx,DWORD PTR ds:0x256c
    *video = *(video+(vga_cols*2));
    video++;
  }
  char bkg = *(video-1);
  while (video < vga_addr + vga_rows * vga_cols * 2) {
    15f2:	81 c2 a0 0f 00 00    	add    edx,0xfa0
  uint_8* video = vga_addr;
  while (video < (vga_limit - (vga_cols*2))) {
    *video = *(video+(vga_cols*2));
    video++;
  }
  char bkg = *(video-1);
    15f8:	0f b6 48 ff          	movzx  ecx,BYTE PTR [eax-0x1]
  while (video < vga_addr + vga_rows * vga_cols * 2) {
    15fc:	39 d0                	cmp    eax,edx
    15fe:	73 19                	jae    1619 <move_scr_up+0x69>
    *video++ = 0x00;
    1600:	c6 00 00             	mov    BYTE PTR [eax],0x0
    *video++ = bkg;
    1603:	88 48 01             	mov    BYTE PTR [eax+0x1],cl
  while (video < (vga_limit - (vga_cols*2))) {
    *video = *(video+(vga_cols*2));
    video++;
  }
  char bkg = *(video-1);
  while (video < vga_addr + vga_rows * vga_cols * 2) {
    1606:	8b 15 6c 25 00 00    	mov    edx,DWORD PTR ds:0x256c
    va_start(argp, attr);
    amount *= 2;
  }
}

void move_scr_up() {
    160c:	83 c0 02             	add    eax,0x2
  while (video < (vga_limit - (vga_cols*2))) {
    *video = *(video+(vga_cols*2));
    video++;
  }
  char bkg = *(video-1);
  while (video < vga_addr + vga_rows * vga_cols * 2) {
    160f:	81 c2 a0 0f 00 00    	add    edx,0xfa0
    1615:	39 d0                	cmp    eax,edx
    1617:	72 e7                	jb     1600 <move_scr_up+0x50>
    *video++ = 0x00;
    *video++ = bkg;
  }
}
    1619:	5d                   	pop    ebp
    161a:	c3                   	ret    
    161b:	90                   	nop
    161c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00001620 <fill_screen>:
	}
	
	if (fila < vga_rows) fila++;
}

void fill_screen(char color, char bright) {
    1620:	55                   	push   ebp
  uint_8* video = vga_addr;
    1621:	a1 6c 25 00 00       	mov    eax,ds:0x256c
	}
	
	if (fila < vga_rows) fila++;
}

void fill_screen(char color, char bright) {
    1626:	89 e5                	mov    ebp,esp

uint_8 make_format(char blink, char front, char back, char bright) {
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
  fmt |= ((back & 0x7) << 4);
    1628:	0f be 55 08          	movsx  edx,BYTE PTR [ebp+0x8]
}

uint_8 make_format(char blink, char front, char back, char bright) {
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
    162c:	80 7d 0c 00          	cmp    BYTE PTR [ebp+0xc],0x0
    1630:	0f 9f c1             	setg   cl
  fmt |= ((back & 0x7) << 4);
    1633:	83 e2 07             	and    edx,0x7
}

uint_8 make_format(char blink, char front, char back, char bright) {
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
    1636:	c1 e1 03             	shl    ecx,0x3
  fmt |= ((back & 0x7) << 4);
    1639:	c1 e2 04             	shl    edx,0x4
    163c:	09 d1                	or     ecx,edx
    163e:	66 90                	xchg   ax,ax

void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    *video++ = 0x00;
    1640:	c6 00 00             	mov    BYTE PTR [eax],0x0
    *video++ = fmt;
    1643:	88 48 01             	mov    BYTE PTR [eax+0x1],cl
}

void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    1646:	8b 15 6c 25 00 00    	mov    edx,DWORD PTR ds:0x256c
	}
	
	if (fila < vga_rows) fila++;
}

void fill_screen(char color, char bright) {
    164c:	83 c0 02             	add    eax,0x2
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    164f:	81 c2 a0 0f 00 00    	add    edx,0xfa0
    1655:	39 d0                	cmp    eax,edx
    1657:	72 e7                	jb     1640 <fill_screen+0x20>
    *video++ = 0x00;
    *video++ = fmt;
  }
}
    1659:	5d                   	pop    ebp
    165a:	c3                   	ret    
    165b:	90                   	nop
    165c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00001660 <clear_screen>:
	
	if (fila < vga_rows) fila++;
}

void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
    1660:	a1 6c 25 00 00       	mov    eax,ds:0x256c
    *video++ = 0x00;
    *video++ = fmt;
  }
}

void clear_screen() {
    1665:	55                   	push   ebp
    1666:	89 e5                	mov    ebp,esp

void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    *video++ = 0x00;
    1668:	c6 00 00             	mov    BYTE PTR [eax],0x0
    *video++ = fmt;
    166b:	c6 40 01 00          	mov    BYTE PTR [eax+0x1],0x0
}

void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    166f:	8b 15 6c 25 00 00    	mov    edx,DWORD PTR ds:0x256c
    *video++ = 0x00;
    *video++ = fmt;
  }
}

void clear_screen() {
    1675:	83 c0 02             	add    eax,0x2
}

void fill_screen(char color, char bright) {
  uint_8* video = vga_addr;
  char fmt = make_format(0x0, 0x0, color, bright);
  while(video < vga_addr + vga_cols*2*vga_rows) {
    1678:	81 c2 a0 0f 00 00    	add    edx,0xfa0
    167e:	39 d0                	cmp    eax,edx
    1680:	72 e6                	jb     1668 <clear_screen+0x8>
  }
}

void clear_screen() {
  fill_screen(0,0);
}
    1682:	5d                   	pop    ebp
    1683:	c3                   	ret    
    1684:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    168a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00001690 <make_format>:
  vga_write(f, c, buff, attr);
  va_end(argp);
  return 0;
}

uint_8 make_format(char blink, char front, char back, char bright) {
    1690:	55                   	push   ebp
    1691:	89 e5                	mov    ebp,esp
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
    1693:	80 7d 14 00          	cmp    BYTE PTR [ebp+0x14],0x0
  vga_write(f, c, buff, attr);
  va_end(argp);
  return 0;
}

uint_8 make_format(char blink, char front, char back, char bright) {
    1697:	53                   	push   ebx
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
  fmt |= ((back & 0x7) << 4);
  fmt |= ((blink > 0) << 7);
    1698:	0f be 55 10          	movsx  edx,BYTE PTR [ebp+0x10]
  vga_write(f, c, buff, attr);
  va_end(argp);
  return 0;
}

uint_8 make_format(char blink, char front, char back, char bright) {
    169c:	0f b6 45 0c          	movzx  eax,BYTE PTR [ebp+0xc]
  uint_8 fmt = (uint_8)0;
  fmt |= ((front & 0x7) << 0);
  fmt |= ((bright > 0) << 3);
    16a0:	0f 9f c3             	setg   bl
    16a3:	c1 e3 03             	shl    ebx,0x3
  fmt |= ((back & 0x7) << 4);
  fmt |= ((blink > 0) << 7);
    16a6:	80 7d 08 00          	cmp    BYTE PTR [ebp+0x8],0x0
    16aa:	0f 9f c1             	setg   cl
    16ad:	83 e2 07             	and    edx,0x7
    16b0:	c1 e2 04             	shl    edx,0x4
    16b3:	83 e0 07             	and    eax,0x7
    16b6:	09 d0                	or     eax,edx
    16b8:	09 d8                	or     eax,ebx
    16ba:	c1 e1 07             	shl    ecx,0x7
    16bd:	09 c8                	or     eax,ecx
  return fmt;
}
    16bf:	5b                   	pop    ebx
    16c0:	5d                   	pop    ebp
    16c1:	c3                   	ret    
    16c2:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    16c9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000016d0 <printf_resolver>:
void clear_screen() {
  fill_screen(0,0);
}


int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
    16d0:	55                   	push   ebp
    16d1:	89 e5                	mov    ebp,esp
    16d3:	57                   	push   edi
    16d4:	56                   	push   esi
    16d5:	53                   	push   ebx
    16d6:	83 ec 3c             	sub    esp,0x3c
    16d9:	0f b7 45 08          	movzx  eax,WORD PTR [ebp+0x8]
    16dd:	0f b7 55 0c          	movzx  edx,WORD PTR [ebp+0xc]
    16e1:	8b 75 18             	mov    esi,DWORD PTR [ebp+0x18]
    16e4:	8b 5d 1c             	mov    ebx,DWORD PTR [ebp+0x1c]
    16e7:	66 89 45 ce          	mov    WORD PTR [ebp-0x32],ax
    16eb:	0f b6 45 10          	movzx  eax,BYTE PTR [ebp+0x10]
    16ef:	66 89 55 cc          	mov    WORD PTR [ebp-0x34],dx
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
    16f3:	c7 45 e4 00 00 00 00 	mov    DWORD PTR [ebp-0x1c],0x0
void clear_screen() {
  fill_screen(0,0);
}


int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
    16fa:	88 45 cb             	mov    BYTE PTR [ebp-0x35],al
  char buff[amount];
    16fd:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
    1700:	83 c0 1e             	add    eax,0x1e
    1703:	83 e0 f0             	and    eax,0xfffffff0
    1706:	29 c4                	sub    esp,eax
    1708:	8d 54 24 1f          	lea    edx,[esp+0x1f]
    170c:	83 e2 f0             	and    edx,0xfffffff0
    170f:	89 55 d0             	mov    DWORD PTR [ebp-0x30],edx
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
    1712:	89 34 24             	mov    DWORD PTR [esp],esi
    1715:	e8 26 04 00 00       	call   1b40 <strlen>
  while(i < len) {
    171a:	85 c0                	test   eax,eax

int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
    171c:	89 45 d4             	mov    DWORD PTR [ebp-0x2c],eax
  while(i < len) {
    171f:	7e 57                	jle    1778 <printf_resolver+0xa8>
    1721:	31 c0                	xor    eax,eax
          break;

          break;
        case 's':
          str = va_arg(argp, char*);
          if (str_into_string(buff, &count, amount, str) < 0)
    1723:	89 da                	mov    edx,ebx
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
    1725:	0f b6 0c 06          	movzx  ecx,BYTE PTR [esi+eax*1]
    1729:	80 f9 25             	cmp    cl,0x25
    172c:	75 26                	jne    1754 <printf_resolver+0x84>
    172e:	66 90                	xchg   ax,ax
      i++;
    1730:	8d 58 01             	lea    ebx,[eax+0x1]
      switch(fmt[i]) {
    1733:	0f b6 44 06 01       	movzx  eax,BYTE PTR [esi+eax*1+0x1]
    1738:	83 e8 63             	sub    eax,0x63
    173b:	3c 15                	cmp    al,0x15
    173d:	76 71                	jbe    17b0 <printf_resolver+0xe0>
    173f:	89 d7                	mov    edi,edx
          if (str_into_string(buff, &count, amount, str) < 0)
            return -1;
          arg++;
          break;
      }
      i++;
    1741:	8d 43 01             	lea    eax,[ebx+0x1]
int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    1744:	39 45 d4             	cmp    DWORD PTR [ebp-0x2c],eax
    1747:	7e 2f                	jle    1778 <printf_resolver+0xa8>
    if (fmt[i] == '%') { //Special action needed
    1749:	0f b6 0c 06          	movzx  ecx,BYTE PTR [esi+eax*1]
int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    174d:	89 fa                	mov    edx,edi
    if (fmt[i] == '%') { //Special action needed
    174f:	80 f9 25             	cmp    cl,0x25
    1752:	74 dc                	je     1730 <printf_resolver+0x60>
          arg++;
          break;
      }
      i++;
    } else { //No special action is taken, char is copied
      if (count >= amount) return -1;
    1754:	8b 5d e4             	mov    ebx,DWORD PTR [ebp-0x1c]
    1757:	39 5d 14             	cmp    DWORD PTR [ebp+0x14],ebx
    175a:	0f 8e 8a 00 00 00    	jle    17ea <printf_resolver+0x11a>
      buff[count] = fmt[i];
    1760:	8b 7d d0             	mov    edi,DWORD PTR [ebp-0x30]
      i++;
    1763:	83 c0 01             	add    eax,0x1
          break;
      }
      i++;
    } else { //No special action is taken, char is copied
      if (count >= amount) return -1;
      buff[count] = fmt[i];
    1766:	88 0c 1f             	mov    BYTE PTR [edi+ebx*1],cl
      i++;
      count++;
    1769:	83 c3 01             	add    ebx,0x1
    176c:	89 d7                	mov    edi,edx
int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    176e:	39 45 d4             	cmp    DWORD PTR [ebp-0x2c],eax
      i++;
    } else { //No special action is taken, char is copied
      if (count >= amount) return -1;
      buff[count] = fmt[i];
      i++;
      count++;
    1771:	89 5d e4             	mov    DWORD PTR [ebp-0x1c],ebx
int printf_resolver(uint_16 f, uint_16 c, uint_8 attr, int amount, const char* fmt, va_list argp) {
  char buff[amount];
  char ch;
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    1774:	7f d3                	jg     1749 <printf_resolver+0x79>
    1776:	66 90                	xchg   ax,ax
      buff[count] = fmt[i];
      i++;
      count++;
    }
  }
  buff[count] = '\0';
    1778:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
    177b:	8b 55 d0             	mov    edx,DWORD PTR [ebp-0x30]
    177e:	c6 04 02 00          	mov    BYTE PTR [edx+eax*1],0x0
  vga_write(f, c, buff, attr);
    1782:	0f b6 45 cb          	movzx  eax,BYTE PTR [ebp-0x35]
    1786:	89 54 24 08          	mov    DWORD PTR [esp+0x8],edx
    178a:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    178e:	0f b7 45 cc          	movzx  eax,WORD PTR [ebp-0x34]
    1792:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    1796:	0f b7 45 ce          	movzx  eax,WORD PTR [ebp-0x32]
    179a:	89 04 24             	mov    DWORD PTR [esp],eax
    179d:	e8 8e fd ff ff       	call   1530 <vga_write>
  va_end(argp);
  return 0;
}
    17a2:	8d 65 f4             	lea    esp,[ebp-0xc]
      i++;
      count++;
    }
  }
  buff[count] = '\0';
  vga_write(f, c, buff, attr);
    17a5:	31 c0                	xor    eax,eax
  va_end(argp);
  return 0;
}
    17a7:	5b                   	pop    ebx
    17a8:	5e                   	pop    esi
    17a9:	5f                   	pop    edi
    17aa:	5d                   	pop    ebp
    17ab:	c3                   	ret    
    17ac:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  char* str;
  int x, i = 0, count = 0, len = strlen(fmt), arg = 0;
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
    17b0:	0f b6 c0             	movzx  eax,al
    17b3:	ff 24 85 d0 1e 00 00 	jmp    DWORD PTR [eax*4+0x1ed0]
    17ba:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
            return -1;
          arg++;
          break;
        case 'x':
          x = va_arg(argp, int);
          if (hex_into_string(buff, &count, amount, x) < 0)
    17c0:	8b 02                	mov    eax,DWORD PTR [edx]
          if (dec_into_string(buff, &count, amount, x) < 0)
            return -1;
          arg++;
          break;
        case 'x':
          x = va_arg(argp, int);
    17c2:	8d 7a 04             	lea    edi,[edx+0x4]
          if (hex_into_string(buff, &count, amount, x) < 0)
    17c5:	8d 55 e4             	lea    edx,[ebp-0x1c]
    17c8:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    17cc:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
    17cf:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
    17d3:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
    17d7:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
    17da:	89 04 24             	mov    DWORD PTR [esp],eax
    17dd:	e8 4e 04 00 00       	call   1c30 <hex_into_string>
    17e2:	85 c0                	test   eax,eax
    17e4:	0f 89 57 ff ff ff    	jns    1741 <printf_resolver+0x71>
  }
  buff[count] = '\0';
  vga_write(f, c, buff, attr);
  va_end(argp);
  return 0;
}
    17ea:	8d 65 f4             	lea    esp,[ebp-0xc]
      i++;
      count++;
    }
  }
  buff[count] = '\0';
  vga_write(f, c, buff, attr);
    17ed:	b8 ff ff ff ff       	mov    eax,0xffffffff
  va_end(argp);
  return 0;
}
    17f2:	5b                   	pop    ebx
    17f3:	5e                   	pop    esi
    17f4:	5f                   	pop    edi
    17f5:	5d                   	pop    ebp
    17f6:	c3                   	ret    
    17f7:	90                   	nop
          break;

          break;
        case 's':
          str = va_arg(argp, char*);
          if (str_into_string(buff, &count, amount, str) < 0)
    17f8:	8b 02                	mov    eax,DWORD PTR [edx]
          arg++;
          break;

          break;
        case 's':
          str = va_arg(argp, char*);
    17fa:	8d 7a 04             	lea    edi,[edx+0x4]
          if (str_into_string(buff, &count, amount, str) < 0)
    17fd:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    1801:	8b 55 14             	mov    edx,DWORD PTR [ebp+0x14]
    1804:	8d 45 e4             	lea    eax,[ebp-0x1c]
    1807:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    180b:	89 54 24 08          	mov    DWORD PTR [esp+0x8],edx
    180f:	8b 55 d0             	mov    edx,DWORD PTR [ebp-0x30]
    1812:	89 14 24             	mov    DWORD PTR [esp],edx
    1815:	e8 d6 04 00 00       	call   1cf0 <str_into_string>
    181a:	85 c0                	test   eax,eax
    181c:	0f 89 1f ff ff ff    	jns    1741 <printf_resolver+0x71>
    1822:	eb c6                	jmp    17ea <printf_resolver+0x11a>
    1824:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
          arg++;
          break;
        case 'd':
        case 'i':
          x = va_arg(argp, int);
          if (dec_into_string(buff, &count, amount, x) < 0)
    1828:	8b 02                	mov    eax,DWORD PTR [edx]
            return -1;
          arg++;
          break;
        case 'd':
        case 'i':
          x = va_arg(argp, int);
    182a:	8d 7a 04             	lea    edi,[edx+0x4]
          if (dec_into_string(buff, &count, amount, x) < 0)
    182d:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    1831:	8b 55 14             	mov    edx,DWORD PTR [ebp+0x14]
    1834:	8d 45 e4             	lea    eax,[ebp-0x1c]
    1837:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    183b:	89 54 24 08          	mov    DWORD PTR [esp+0x8],edx
    183f:	8b 55 d0             	mov    edx,DWORD PTR [ebp-0x30]
    1842:	89 14 24             	mov    DWORD PTR [esp],edx
    1845:	e8 46 04 00 00       	call   1c90 <dec_into_string>
    184a:	85 c0                	test   eax,eax
    184c:	0f 89 ef fe ff ff    	jns    1741 <printf_resolver+0x71>
    1852:	eb 96                	jmp    17ea <printf_resolver+0x11a>
    1854:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
        case 'c':
          ch = (char)va_arg(argp, int);
          if (char_into_string(buff, &count, amount, ch) < 0)
    1858:	0f be 02             	movsx  eax,BYTE PTR [edx]
  while(i < len) {
    if (fmt[i] == '%') { //Special action needed
      i++;
      switch(fmt[i]) {
        case 'c':
          ch = (char)va_arg(argp, int);
    185b:	8d 7a 04             	lea    edi,[edx+0x4]
          if (char_into_string(buff, &count, amount, ch) < 0)
    185e:	8d 55 e4             	lea    edx,[ebp-0x1c]
    1861:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    1865:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
    1868:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
    186c:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
    1870:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
    1873:	89 04 24             	mov    DWORD PTR [esp],eax
    1876:	e8 45 04 00 00       	call   1cc0 <char_into_string>
    187b:	85 c0                	test   eax,eax
    187d:	0f 89 be fe ff ff    	jns    1741 <printf_resolver+0x71>
    1883:	e9 62 ff ff ff       	jmp    17ea <printf_resolver+0x11a>
    1888:	90                   	nop
    1889:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00001890 <printf>:
    *video++ = 0x00;
    *video++ = bkg;
  }
}

void printf(const char* fmt, ...) {
    1890:	55                   	push   ebp
    1891:	89 e5                	mov    ebp,esp
    1893:	57                   	push   edi
    1894:	56                   	push   esi
    1895:	53                   	push   ebx
    1896:	83 ec 2c             	sub    esp,0x2c
	va_list argp;
	int amount = 256;
	va_start(argp, fmt);
	if (fila == vga_rows) 
    1899:	0f b7 05 74 25 00 00 	movzx  eax,WORD PTR ds:0x2574
    *video++ = 0x00;
    *video++ = bkg;
  }
}

void printf(const char* fmt, ...) {
    18a0:	8b 75 08             	mov    esi,DWORD PTR [ebp+0x8]
    18a3:	8d 7d 0c             	lea    edi,[ebp+0xc]
	va_list argp;
	int amount = 256;
	va_start(argp, fmt);
	if (fila == vga_rows) 
    18a6:	66 83 f8 19          	cmp    ax,0x19
    18aa:	74 64                	je     1910 <printf+0x80>
		move_scr_up();
    18ac:	bb 00 01 00 00       	mov    ebx,0x100
    18b1:	eb 0e                	jmp    18c1 <printf+0x31>
    18b3:	90                   	nop
    18b4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
	
	while(printf_resolver(fila, 0, VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, amount, fmt, argp) < 0) {
		va_start(argp, fmt);
		amount *= 2;
    18b8:	0f b7 05 74 25 00 00 	movzx  eax,WORD PTR ds:0x2574
    18bf:	01 db                	add    ebx,ebx
	int amount = 256;
	va_start(argp, fmt);
	if (fila == vga_rows) 
		move_scr_up();
	
	while(printf_resolver(fila, 0, VGA_BC_BLACK | VGA_FC_WHITE | VGA_FC_LIGHT, amount, fmt, argp) < 0) {
    18c1:	0f b7 c0             	movzx  eax,ax
    18c4:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
    18c8:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
    18cc:	89 5c 24 0c          	mov    DWORD PTR [esp+0xc],ebx
    18d0:	c7 44 24 08 0f 00 00 	mov    DWORD PTR [esp+0x8],0xf
    18d7:	00 
    18d8:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
    18df:	00 
    18e0:	89 04 24             	mov    DWORD PTR [esp],eax
    18e3:	e8 e8 fd ff ff       	call   16d0 <printf_resolver>
    18e8:	85 c0                	test   eax,eax
    18ea:	78 cc                	js     18b8 <printf+0x28>
		va_start(argp, fmt);
		amount *= 2;
	}
	
	if (fila < vga_rows) fila++;
    18ec:	0f b7 05 74 25 00 00 	movzx  eax,WORD PTR ds:0x2574
    18f3:	66 83 f8 18          	cmp    ax,0x18
    18f7:	77 09                	ja     1902 <printf+0x72>
    18f9:	83 c0 01             	add    eax,0x1
    18fc:	66 a3 74 25 00 00    	mov    ds:0x2574,ax
}
    1902:	83 c4 2c             	add    esp,0x2c
    1905:	5b                   	pop    ebx
    1906:	5e                   	pop    esi
    1907:	5f                   	pop    edi
    1908:	5d                   	pop    ebp
    1909:	c3                   	ret    
    190a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
void printf(const char* fmt, ...) {
	va_list argp;
	int amount = 256;
	va_start(argp, fmt);
	if (fila == vga_rows) 
		move_scr_up();
    1910:	e8 9b fc ff ff       	call   15b0 <move_scr_up>
    1915:	0f b7 05 74 25 00 00 	movzx  eax,WORD PTR ds:0x2574
    191c:	eb 8e                	jmp    18ac <printf+0x1c>
    191e:	66 90                	xchg   ax,ax

00001920 <vga_printf>:
    }
    str++;
  }
}

void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) {
    1920:	55                   	push   ebp
    1921:	89 e5                	mov    ebp,esp
    1923:	57                   	push   edi
    1924:	56                   	push   esi
    1925:	53                   	push   ebx
    1926:	bb 00 01 00 00       	mov    ebx,0x100
    192b:	83 ec 3c             	sub    esp,0x3c
    192e:	0f b7 45 08          	movzx  eax,WORD PTR [ebp+0x8]
    1932:	0f b6 7d 14          	movzx  edi,BYTE PTR [ebp+0x14]
    1936:	0f b7 75 0c          	movzx  esi,WORD PTR [ebp+0xc]
    193a:	89 45 e4             	mov    DWORD PTR [ebp-0x1c],eax
  va_list argp;
  int amount = 256;
  va_start(argp, attr);
  while(printf_resolver(f, c, attr, amount, format, argp) < 0) {
    193d:	eb 03                	jmp    1942 <vga_printf+0x22>
    193f:	90                   	nop
    va_start(argp, attr);
    amount *= 2;
    1940:	01 db                	add    ebx,ebx

void vga_printf(uint_16 f, uint_16 c, const char* format, uint_8 attr, ...) {
  va_list argp;
  int amount = 256;
  va_start(argp, attr);
  while(printf_resolver(f, c, attr, amount, format, argp) < 0) {
    1942:	8d 45 18             	lea    eax,[ebp+0x18]
    1945:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
    1949:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
    194c:	89 5c 24 0c          	mov    DWORD PTR [esp+0xc],ebx
    1950:	89 7c 24 08          	mov    DWORD PTR [esp+0x8],edi
    1954:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
    1958:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
    195c:	8b 45 e4             	mov    eax,DWORD PTR [ebp-0x1c]
    195f:	89 04 24             	mov    DWORD PTR [esp],eax
    1962:	e8 69 fd ff ff       	call   16d0 <printf_resolver>
    1967:	85 c0                	test   eax,eax
    1969:	78 d5                	js     1940 <vga_printf+0x20>
    va_start(argp, attr);
    amount *= 2;
  }
}
    196b:	83 c4 3c             	add    esp,0x3c
    196e:	5b                   	pop    ebx
    196f:	5e                   	pop    esi
    1970:	5f                   	pop    edi
    1971:	5d                   	pop    ebp
    1972:	c3                   	ret    
    1973:	90                   	nop
    1974:	90                   	nop
    1975:	90                   	nop
    1976:	90                   	nop
    1977:	90                   	nop
    1978:	90                   	nop
    1979:	90                   	nop
    197a:	90                   	nop
    197b:	90                   	nop
    197c:	90                   	nop
    197d:	90                   	nop
    197e:	90                   	nop
    197f:	90                   	nop

00001980 <sched_init>:
	// ...etc... (se puede modificar todo este struct)
} sched_task;
sched_task tasks[MAX_PID];


void sched_init(void) {
    1980:	55                   	push   ebp
    1981:	89 e5                	mov    ebp,esp
}
    1983:	5d                   	pop    ebp
    1984:	c3                   	ret    
    1985:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1989:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001990 <sched_load>:

void sched_load(pid pd) {
    1990:	55                   	push   ebp
    1991:	89 e5                	mov    ebp,esp
}
    1993:	5d                   	pop    ebp
    1994:	c3                   	ret    
    1995:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1999:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000019a0 <sched_unblock>:

void sched_unblock(pid pd) {
    19a0:	55                   	push   ebp
    19a1:	89 e5                	mov    ebp,esp
}
    19a3:	5d                   	pop    ebp
    19a4:	c3                   	ret    
    19a5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    19a9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000019b0 <sched_exit>:

int sched_exit() {
    19b0:	55                   	push   ebp
	return 0;
}
    19b1:	31 c0                	xor    eax,eax
}

void sched_unblock(pid pd) {
}

int sched_exit() {
    19b3:	89 e5                	mov    ebp,esp
	return 0;
}
    19b5:	5d                   	pop    ebp
    19b6:	c3                   	ret    
    19b7:	89 f6                	mov    esi,esi
    19b9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000019c0 <sched_block>:
int sched_block() {
    19c0:	55                   	push   ebp
	return 0;
}
    19c1:	31 c0                	xor    eax,eax
}

int sched_exit() {
	return 0;
}
int sched_block() {
    19c3:	89 e5                	mov    ebp,esp
	return 0;
}
    19c5:	5d                   	pop    ebp
    19c6:	c3                   	ret    
    19c7:	89 f6                	mov    esi,esi
    19c9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000019d0 <sched_tick>:
int sched_tick() {
    19d0:	55                   	push   ebp
	return 0;
}
    19d1:	31 c0                	xor    eax,eax
	return 0;
}
int sched_block() {
	return 0;
}
int sched_tick() {
    19d3:	89 e5                	mov    ebp,esp
	return 0;
}
    19d5:	5d                   	pop    ebp
    19d6:	c3                   	ret    
    19d7:	90                   	nop
    19d8:	90                   	nop
    19d9:	90                   	nop
    19da:	90                   	nop
    19db:	90                   	nop
    19dc:	90                   	nop
    19dd:	90                   	nop
    19de:	90                   	nop
    19df:	90                   	nop

000019e0 <mm_mem_alloc>:
#include <mm.h>

void* mm_mem_alloc() {
    19e0:	55                   	push   ebp
  return NULL;
}
    19e1:	31 c0                	xor    eax,eax
#include <mm.h>

void* mm_mem_alloc() {
    19e3:	89 e5                	mov    ebp,esp
  return NULL;
}
    19e5:	5d                   	pop    ebp
    19e6:	c3                   	ret    
    19e7:	89 f6                	mov    esi,esi
    19e9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

000019f0 <mm_mem_kalloc>:
void* mm_mem_kalloc() {
    19f0:	55                   	push   ebp
  return NULL;
}
    19f1:	31 c0                	xor    eax,eax
#include <mm.h>

void* mm_mem_alloc() {
  return NULL;
}
void* mm_mem_kalloc() {
    19f3:	89 e5                	mov    ebp,esp
  return NULL;
}
    19f5:	5d                   	pop    ebp
    19f6:	c3                   	ret    
    19f7:	89 f6                	mov    esi,esi
    19f9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001a00 <mm_mem_free>:
void mm_mem_free(void* page) {
    1a00:	55                   	push   ebp
    1a01:	89 e5                	mov    ebp,esp
}
    1a03:	5d                   	pop    ebp
    1a04:	c3                   	ret    
    1a05:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1a09:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001a10 <mm_dir_new>:

mm_page* mm_dir_new(void) {
    1a10:	55                   	push   ebp
  return NULL;
}
    1a11:	31 c0                	xor    eax,eax
  return NULL;
}
void mm_mem_free(void* page) {
}

mm_page* mm_dir_new(void) {
    1a13:	89 e5                	mov    ebp,esp
  return NULL;
}
    1a15:	5d                   	pop    ebp
    1a16:	c3                   	ret    
    1a17:	89 f6                	mov    esi,esi
    1a19:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001a20 <mm_dir_free>:
void mm_dir_free(mm_page* d) {
    1a20:	55                   	push   ebp
    1a21:	89 e5                	mov    ebp,esp
}
    1a23:	5d                   	pop    ebp
    1a24:	c3                   	ret    
    1a25:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1a29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001a30 <mm_init>:


extern void* _end; // Puntero al fin del c'odigo del kernel.bin (definido por LD).
void mm_init(void) {
    1a30:	55                   	push   ebp
    1a31:	89 e5                	mov    ebp,esp

}
    1a33:	5d                   	pop    ebp
    1a34:	c3                   	ret    
    1a35:	90                   	nop
    1a36:	90                   	nop
    1a37:	90                   	nop
    1a38:	90                   	nop
    1a39:	90                   	nop
    1a3a:	90                   	nop
    1a3b:	90                   	nop
    1a3c:	90                   	nop
    1a3d:	90                   	nop
    1a3e:	90                   	nop
    1a3f:	90                   	nop

00001a40 <debug_kernelpanic>:
};

bool in_panic = FALSE;
void debug_kernelpanic(const uint_32* stack, const exp_state* expst) {
	/* No permite panics anidados */
	if (in_panic) while(1) hlt();
    1a40:	83 3d c8 25 00 00 00 	cmp    DWORD PTR ds:0x25c8,0x0
	"Floating point exception",
	"Alignment check"
};

bool in_panic = FALSE;
void debug_kernelpanic(const uint_32* stack, const exp_state* expst) {
    1a47:	55                   	push   ebp
    1a48:	89 e5                	mov    ebp,esp
	/* No permite panics anidados */
	if (in_panic) while(1) hlt();
    1a4a:	74 07                	je     1a53 <debug_kernelpanic+0x13>
    1a4c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
  __asm __volatile("str %0" : "=r" (sel) : );
  return sel;
}

LS_INLINE void hlt(void) {
  __asm __volatile("hlt" : : );
    1a50:	f4                   	hlt    
    1a51:	eb fd                	jmp    1a50 <debug_kernelpanic+0x10>
	in_panic = TRUE;
    1a53:	c7 05 c8 25 00 00 01 	mov    DWORD PTR ds:0x25c8,0x1
    1a5a:	00 00 00 

	// Completar
}
    1a5d:	5d                   	pop    ebp
    1a5e:	c3                   	ret    
    1a5f:	90                   	nop

00001a60 <debug_init>:


void debug_init(void) {
    1a60:	55                   	push   ebp
    1a61:	89 e5                	mov    ebp,esp
	/* Registra todas las excepciones para sí */
}
    1a63:	5d                   	pop    ebp
    1a64:	c3                   	ret    
    1a65:	90                   	nop
    1a66:	90                   	nop
    1a67:	90                   	nop
    1a68:	90                   	nop
    1a69:	90                   	nop
    1a6a:	90                   	nop
    1a6b:	90                   	nop
    1a6c:	90                   	nop
    1a6d:	90                   	nop
    1a6e:	90                   	nop
    1a6f:	90                   	nop

00001a70 <loader_init>:
task task_table[MAX_PID];

/* pid "actual" */
uint_32 cur_pid = 0;

void loader_init(void) {
    1a70:	55                   	push   ebp
    1a71:	89 e5                	mov    ebp,esp
	
}
    1a73:	5d                   	pop    ebp
    1a74:	c3                   	ret    
    1a75:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1a79:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001a80 <loader_load>:

pid loader_load(pso_file* f, int pl) {
    1a80:	55                   	push   ebp
	return 0;
}
    1a81:	31 c0                	xor    eax,eax

void loader_init(void) {
	
}

pid loader_load(pso_file* f, int pl) {
    1a83:	89 e5                	mov    ebp,esp
	return 0;
}
    1a85:	5d                   	pop    ebp
    1a86:	c3                   	ret    
    1a87:	89 f6                	mov    esi,esi
    1a89:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001a90 <loader_enqueue>:

void loader_enqueue(int* cola) {
    1a90:	55                   	push   ebp
    1a91:	89 e5                	mov    ebp,esp
}
    1a93:	5d                   	pop    ebp
    1a94:	c3                   	ret    
    1a95:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1a99:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001aa0 <loader_unqueue>:

void loader_unqueue(int* cola) {
    1aa0:	55                   	push   ebp
    1aa1:	89 e5                	mov    ebp,esp
}
    1aa3:	5d                   	pop    ebp
    1aa4:	c3                   	ret    
    1aa5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1aa9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001ab0 <loader_exit>:

void loader_exit(void) {
    1ab0:	55                   	push   ebp
    1ab1:	89 e5                	mov    ebp,esp
}
    1ab3:	5d                   	pop    ebp
    1ab4:	c3                   	ret    
    1ab5:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1ab9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001ac0 <loader_switchto>:

void loader_switchto(pid pd) {
    1ac0:	55                   	push   ebp
    1ac1:	89 e5                	mov    ebp,esp
}
    1ac3:	5d                   	pop    ebp
    1ac4:	c3                   	ret    
    1ac5:	90                   	nop
    1ac6:	90                   	nop
    1ac7:	90                   	nop
    1ac8:	90                   	nop
    1ac9:	90                   	nop
    1aca:	90                   	nop
    1acb:	90                   	nop
    1acc:	90                   	nop
    1acd:	90                   	nop
    1ace:	90                   	nop
    1acf:	90                   	nop

00001ad0 <task_task1_pso>:
incbin %2
global task_%1_pso_end
task_%1_pso_end:
%endmacro

include_task task1, "tasks/task1.pso"
    1ad0:	50                   	push   eax
    1ad1:	53                   	push   ebx
    1ad2:	4f                   	dec    edi
    1ad3:	00 00                	add    BYTE PTR [eax],al
    1ad5:	00 40 00             	add    BYTE PTR [eax+0x0],al
    1ad8:	38 00                	cmp    BYTE PTR [eax],al
    1ada:	40                   	inc    eax
    1adb:	00 38                	add    BYTE PTR [eax],bh
    1add:	00 40 00             	add    BYTE PTR [eax+0x0],al
    1ae0:	14 00                	adc    al,0x0
    1ae2:	40                   	inc    eax
    1ae3:	00 68 00             	add    BYTE PTR [eax+0x0],ch
    1ae6:	00 00                	add    BYTE PTR [eax],al
    1ae8:	00 68 00             	add    BYTE PTR [eax+0x0],ch
    1aeb:	00 00                	add    BYTE PTR [eax],al
    1aed:	00 68 00             	add    BYTE PTR [eax+0x0],ch
    1af0:	00 00                	add    BYTE PTR [eax],al
    1af2:	00 e8                	add    al,ch
    1af4:	08 00                	or     BYTE PTR [eax],al
    1af6:	00 00                	add    BYTE PTR [eax],al
    1af8:	e9 fb ff ff ff       	jmp    1af8 <task_task1_pso+0x28>
    1afd:	90                   	nop
    1afe:	90                   	nop
    1aff:	90                   	nop
    1b00:	55                   	push   ebp
    1b01:	89 e5                	mov    ebp,esp
    1b03:	eb fe                	jmp    1b03 <task_task1_pso+0x33>

00001b05 <task_task1_pso_end>:
    1b05:	90                   	nop
    1b06:	90                   	nop
    1b07:	90                   	nop
    1b08:	90                   	nop
    1b09:	90                   	nop
    1b0a:	90                   	nop
    1b0b:	90                   	nop
    1b0c:	90                   	nop
    1b0d:	90                   	nop
    1b0e:	90                   	nop
    1b0f:	90                   	nop

00001b10 <sem_wait>:
#include <sem.h>

void sem_wait(sem_t* s) {
    1b10:	55                   	push   ebp
    1b11:	89 e5                	mov    ebp,esp
}
    1b13:	5d                   	pop    ebp
    1b14:	c3                   	ret    
    1b15:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1b19:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001b20 <sem_signaln>:
void sem_signaln(sem_t* s) {
    1b20:	55                   	push   ebp
    1b21:	89 e5                	mov    ebp,esp
}
    1b23:	5d                   	pop    ebp
    1b24:	c3                   	ret    
    1b25:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    1b29:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001b30 <sem_broadcast>:
void sem_broadcast(sem_t* s) {
    1b30:	55                   	push   ebp
    1b31:	89 e5                	mov    ebp,esp
}
    1b33:	5d                   	pop    ebp
    1b34:	c3                   	ret    
    1b35:	90                   	nop
    1b36:	90                   	nop
    1b37:	90                   	nop
    1b38:	90                   	nop
    1b39:	90                   	nop
    1b3a:	90                   	nop
    1b3b:	90                   	nop
    1b3c:	90                   	nop
    1b3d:	90                   	nop
    1b3e:	90                   	nop
    1b3f:	90                   	nop

00001b40 <strlen>:
#ifndef __KLIB_PRINT_C__
#define __KLIB_PRINT_C__

#include "lib_str.h"

int strlen(const char* str) {
    1b40:	55                   	push   ebp
  int len = 0;
  while(str[len] != '\0')
    1b41:	31 c0                	xor    eax,eax
#ifndef __KLIB_PRINT_C__
#define __KLIB_PRINT_C__

#include "lib_str.h"

int strlen(const char* str) {
    1b43:	89 e5                	mov    ebp,esp
    1b45:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
  int len = 0;
  while(str[len] != '\0')
    1b48:	80 3a 00             	cmp    BYTE PTR [edx],0x0
    1b4b:	74 0c                	je     1b59 <strlen+0x19>
    1b4d:	8d 76 00             	lea    esi,[esi+0x0]
    len++;
    1b50:	83 c0 01             	add    eax,0x1

#include "lib_str.h"

int strlen(const char* str) {
  int len = 0;
  while(str[len] != '\0')
    1b53:	80 3c 02 00          	cmp    BYTE PTR [edx+eax*1],0x0
    1b57:	75 f7                	jne    1b50 <strlen+0x10>
    len++;
  return len;
}
    1b59:	5d                   	pop    ebp
    1b5a:	c3                   	ret    
    1b5b:	90                   	nop
    1b5c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]

00001b60 <num_into_string>:
    return -1;
  }
  return 0;
}

int num_into_string(char* str, int* index, int lim, int num, int base) {
    1b60:	55                   	push   ebp
    1b61:	89 e5                	mov    ebp,esp
    1b63:	57                   	push   edi
    1b64:	56                   	push   esi
  int dignum = 1, acum = base, val;
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    1b65:	be 01 00 00 00       	mov    esi,0x1
    return -1;
  }
  return 0;
}

int num_into_string(char* str, int* index, int lim, int num, int base) {
    1b6a:	53                   	push   ebx
    1b6b:	83 ec 04             	sub    esp,0x4
    1b6e:	8b 4d 14             	mov    ecx,DWORD PTR [ebp+0x14]
    1b71:	8b 7d 0c             	mov    edi,DWORD PTR [ebp+0xc]
  int dignum = 1, acum = base, val;
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    1b74:	8b 5d 18             	mov    ebx,DWORD PTR [ebp+0x18]
    1b77:	89 ca                	mov    edx,ecx
    1b79:	89 c8                	mov    eax,ecx
    1b7b:	c1 fa 1f             	sar    edx,0x1f
    1b7e:	f7 7d 18             	idiv   DWORD PTR [ebp+0x18]
    1b81:	85 c0                	test   eax,eax
    1b83:	7e 21                	jle    1ba6 <num_into_string+0x46>
    1b85:	89 7d f0             	mov    DWORD PTR [ebp-0x10],edi
    1b88:	8b 7d 18             	mov    edi,DWORD PTR [ebp+0x18]
    1b8b:	90                   	nop
    1b8c:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    dignum += 1;
    acum *= base;
    1b90:	0f af df             	imul   ebx,edi
}

int num_into_string(char* str, int* index, int lim, int num, int base) {
  int dignum = 1, acum = base, val;
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    1b93:	89 ca                	mov    edx,ecx
    1b95:	c1 fa 1f             	sar    edx,0x1f
    1b98:	89 c8                	mov    eax,ecx
    dignum += 1;
    1b9a:	83 c6 01             	add    esi,0x1
}

int num_into_string(char* str, int* index, int lim, int num, int base) {
  int dignum = 1, acum = base, val;
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    1b9d:	f7 fb                	idiv   ebx
    1b9f:	85 c0                	test   eax,eax
    1ba1:	7f ed                	jg     1b90 <num_into_string+0x30>
    1ba3:	8b 7d f0             	mov    edi,DWORD PTR [ebp-0x10]
    acum *= base;
  }
  acum /= base;

  if (num < 0) dignum++;
  if (*index + dignum >= lim) return -1;
    1ba6:	8b 17                	mov    edx,DWORD PTR [edi]
    dignum += 1;
    acum *= base;
  }
  acum /= base;

  if (num < 0) dignum++;
    1ba8:	89 c8                	mov    eax,ecx
    1baa:	c1 e8 1f             	shr    eax,0x1f
    1bad:	01 c6                	add    esi,eax
  if (*index + dignum >= lim) return -1;
    1baf:	b8 ff ff ff ff       	mov    eax,0xffffffff
    1bb4:	01 d6                	add    esi,edx
    1bb6:	3b 75 10             	cmp    esi,DWORD PTR [ebp+0x10]
    1bb9:	7d 53                	jge    1c0e <num_into_string+0xae>

  if (num < 0) {
    1bbb:	85 c9                	test   ecx,ecx
    1bbd:	78 59                	js     1c18 <num_into_string+0xb8>
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    dignum += 1;
    acum *= base;
  }
  acum /= base;
    1bbf:	89 da                	mov    edx,ebx
    1bc1:	89 d8                	mov    eax,ebx
    1bc3:	c1 fa 1f             	sar    edx,0x1f
    1bc6:	f7 7d 18             	idiv   DWORD PTR [ebp+0x18]
  if (num < 0) {
    str[*index] = '-';
    (*index)++;
  }

  while (acum > 0) {
    1bc9:	85 c0                	test   eax,eax
  char* digits = "0123456789ABCDEF";
  while (num / acum > 0) {
    dignum += 1;
    acum *= base;
  }
  acum /= base;
    1bcb:	89 c3                	mov    ebx,eax
  if (num < 0) {
    str[*index] = '-';
    (*index)++;
  }

  while (acum > 0) {
    1bcd:	7e 3d                	jle    1c0c <num_into_string+0xac>
    1bcf:	8b 37                	mov    esi,DWORD PTR [edi]
    1bd1:	89 4d f0             	mov    DWORD PTR [ebp-0x10],ecx
    1bd4:	8d 74 26 00          	lea    esi,[esi+eiz*1+0x0]
    val = num / acum;
    1bd8:	8b 55 f0             	mov    edx,DWORD PTR [ebp-0x10]
    str[*index] = digits[val];
    1bdb:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
    str[*index] = '-';
    (*index)++;
  }

  while (acum > 0) {
    val = num / acum;
    1bde:	89 d0                	mov    eax,edx
    1be0:	c1 fa 1f             	sar    edx,0x1f
    1be3:	f7 fb                	idiv   ebx
    str[*index] = digits[val];
    1be5:	0f b6 90 60 20 00 00 	movzx  edx,BYTE PTR [eax+0x2060]
    (*index)++;
    num -= val * acum;
    1bec:	0f af c3             	imul   eax,ebx
    (*index)++;
  }

  while (acum > 0) {
    val = num / acum;
    str[*index] = digits[val];
    1bef:	88 14 31             	mov    BYTE PTR [ecx+esi*1],dl
    (*index)++;
    1bf2:	8b 37                	mov    esi,DWORD PTR [edi]
    num -= val * acum;
    acum /= base;
    1bf4:	89 da                	mov    edx,ebx
    1bf6:	c1 fa 1f             	sar    edx,0x1f
  }

  while (acum > 0) {
    val = num / acum;
    str[*index] = digits[val];
    (*index)++;
    1bf9:	83 c6 01             	add    esi,0x1
    1bfc:	89 37                	mov    DWORD PTR [edi],esi
    num -= val * acum;
    1bfe:	29 45 f0             	sub    DWORD PTR [ebp-0x10],eax
    acum /= base;
    1c01:	89 d8                	mov    eax,ebx
    1c03:	f7 7d 18             	idiv   DWORD PTR [ebp+0x18]
  if (num < 0) {
    str[*index] = '-';
    (*index)++;
  }

  while (acum > 0) {
    1c06:	85 c0                	test   eax,eax
    val = num / acum;
    str[*index] = digits[val];
    (*index)++;
    num -= val * acum;
    acum /= base;
    1c08:	89 c3                	mov    ebx,eax
  if (num < 0) {
    str[*index] = '-';
    (*index)++;
  }

  while (acum > 0) {
    1c0a:	7f cc                	jg     1bd8 <num_into_string+0x78>
    1c0c:	31 c0                	xor    eax,eax
    (*index)++;
    num -= val * acum;
    acum /= base;
  }
  return 0;
}
    1c0e:	83 c4 04             	add    esp,0x4
    1c11:	5b                   	pop    ebx
    1c12:	5e                   	pop    esi
    1c13:	5f                   	pop    edi
    1c14:	5d                   	pop    ebp
    1c15:	c3                   	ret    
    1c16:	66 90                	xchg   ax,ax

  if (num < 0) dignum++;
  if (*index + dignum >= lim) return -1;

  if (num < 0) {
    str[*index] = '-';
    1c18:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
    1c1b:	c6 04 10 2d          	mov    BYTE PTR [eax+edx*1],0x2d
    (*index)++;
    1c1f:	83 07 01             	add    DWORD PTR [edi],0x1
    1c22:	eb 9b                	jmp    1bbf <num_into_string+0x5f>
    1c24:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    1c2a:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00001c30 <hex_into_string>:

int dec_into_string(char* str, int* index, int lim, int num) {
  return num_into_string(str, index, lim, num, 10);
}

int hex_into_string(char* str, int* index, int lim, int num) {
    1c30:	55                   	push   ebp
    1c31:	89 e5                	mov    ebp,esp
    1c33:	53                   	push   ebx
    1c34:	83 ec 14             	sub    esp,0x14
    1c37:	8b 5d 0c             	mov    ebx,DWORD PTR [ebp+0xc]
    1c3a:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
  str[*index] = '0';
    1c3d:	8b 13                	mov    edx,DWORD PTR [ebx]
    1c3f:	c6 04 10 30          	mov    BYTE PTR [eax+edx*1],0x30
  (*index)++;
    1c43:	8b 13                	mov    edx,DWORD PTR [ebx]
    1c45:	8d 4a 01             	lea    ecx,[edx+0x1]
    1c48:	89 0b                	mov    DWORD PTR [ebx],ecx
  str[*index] = 'x';
    1c4a:	c6 44 10 01 78       	mov    BYTE PTR [eax+edx*1+0x1],0x78
  (*index)++;
    1c4f:	83 03 01             	add    DWORD PTR [ebx],0x1
  if (num_into_string(str, index, lim, num, 16) < 0) {
    1c52:	8b 55 14             	mov    edx,DWORD PTR [ebp+0x14]
    1c55:	c7 44 24 10 10 00 00 	mov    DWORD PTR [esp+0x10],0x10
    1c5c:	00 
    1c5d:	89 5c 24 04          	mov    DWORD PTR [esp+0x4],ebx
    1c61:	89 04 24             	mov    DWORD PTR [esp],eax
    1c64:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
    1c68:	8b 55 10             	mov    edx,DWORD PTR [ebp+0x10]
    1c6b:	89 54 24 08          	mov    DWORD PTR [esp+0x8],edx
    1c6f:	e8 ec fe ff ff       	call   1b60 <num_into_string>
    1c74:	85 c0                	test   eax,eax
    1c76:	78 08                	js     1c80 <hex_into_string+0x50>
    1c78:	31 c0                	xor    eax,eax
    (*index) -= 2;
    return -1;
  }
  return 0;
}
    1c7a:	83 c4 14             	add    esp,0x14
    1c7d:	5b                   	pop    ebx
    1c7e:	5d                   	pop    ebp
    1c7f:	c3                   	ret    
  str[*index] = '0';
  (*index)++;
  str[*index] = 'x';
  (*index)++;
  if (num_into_string(str, index, lim, num, 16) < 0) {
    (*index) -= 2;
    1c80:	83 2b 02             	sub    DWORD PTR [ebx],0x2
    1c83:	b8 ff ff ff ff       	mov    eax,0xffffffff
    return -1;
    1c88:	eb f0                	jmp    1c7a <hex_into_string+0x4a>
    1c8a:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]

00001c90 <dec_into_string>:
  while(str[len] != '\0')
    len++;
  return len;
}

int dec_into_string(char* str, int* index, int lim, int num) {
    1c90:	55                   	push   ebp
    1c91:	89 e5                	mov    ebp,esp
    1c93:	83 ec 14             	sub    esp,0x14
  return num_into_string(str, index, lim, num, 10);
    1c96:	8b 45 14             	mov    eax,DWORD PTR [ebp+0x14]
    1c99:	c7 44 24 10 0a 00 00 	mov    DWORD PTR [esp+0x10],0xa
    1ca0:	00 
    1ca1:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
    1ca5:	8b 45 10             	mov    eax,DWORD PTR [ebp+0x10]
    1ca8:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
    1cac:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
    1caf:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    1cb3:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
    1cb6:	89 04 24             	mov    DWORD PTR [esp],eax
    1cb9:	e8 a2 fe ff ff       	call   1b60 <num_into_string>
}
    1cbe:	c9                   	leave  
    1cbf:	c3                   	ret    

00001cc0 <char_into_string>:
    acum /= base;
  }
  return 0;
}

int char_into_string(char* str, int* index, int lim, char c) {
    1cc0:	55                   	push   ebp
  if (*index >= lim) return -1;
    1cc1:	b8 ff ff ff ff       	mov    eax,0xffffffff
    acum /= base;
  }
  return 0;
}

int char_into_string(char* str, int* index, int lim, char c) {
    1cc6:	89 e5                	mov    ebp,esp
    1cc8:	8b 55 0c             	mov    edx,DWORD PTR [ebp+0xc]
    1ccb:	53                   	push   ebx
    1ccc:	8b 5d 14             	mov    ebx,DWORD PTR [ebp+0x14]
  if (*index >= lim) return -1;
    1ccf:	8b 0a                	mov    ecx,DWORD PTR [edx]
    1cd1:	3b 4d 10             	cmp    ecx,DWORD PTR [ebp+0x10]
    1cd4:	7d 0b                	jge    1ce1 <char_into_string+0x21>
  str[*index] = c;
    1cd6:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
    1cd9:	88 1c 08             	mov    BYTE PTR [eax+ecx*1],bl
  (*index)++;
    1cdc:	31 c0                	xor    eax,eax
    1cde:	83 02 01             	add    DWORD PTR [edx],0x1
  return 0;
}
    1ce1:	5b                   	pop    ebx
    1ce2:	5d                   	pop    ebp
    1ce3:	c3                   	ret    
    1ce4:	8d b6 00 00 00 00    	lea    esi,[esi+0x0]
    1cea:	8d bf 00 00 00 00    	lea    edi,[edi+0x0]

00001cf0 <str_into_string>:

int str_into_string(char* str, int* index, int lim, char* str2) {
    1cf0:	55                   	push   ebp

#include "lib_str.h"

int strlen(const char* str) {
  int len = 0;
  while(str[len] != '\0')
    1cf1:	31 d2                	xor    edx,edx
  str[*index] = c;
  (*index)++;
  return 0;
}

int str_into_string(char* str, int* index, int lim, char* str2) {
    1cf3:	89 e5                	mov    ebp,esp
    1cf5:	57                   	push   edi
    1cf6:	56                   	push   esi
    1cf7:	53                   	push   ebx
    1cf8:	83 ec 04             	sub    esp,0x4
    1cfb:	8b 5d 14             	mov    ebx,DWORD PTR [ebp+0x14]
    1cfe:	8b 75 0c             	mov    esi,DWORD PTR [ebp+0xc]

#include "lib_str.h"

int strlen(const char* str) {
  int len = 0;
  while(str[len] != '\0')
    1d01:	80 3b 00             	cmp    BYTE PTR [ebx],0x0
    1d04:	74 16                	je     1d1c <str_into_string+0x2c>
    1d06:	8b 7d 08             	mov    edi,DWORD PTR [ebp+0x8]
    1d09:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]
    len++;
    1d10:	83 c2 01             	add    edx,0x1

#include "lib_str.h"

int strlen(const char* str) {
  int len = 0;
  while(str[len] != '\0')
    1d13:	80 3c 13 00          	cmp    BYTE PTR [ebx+edx*1],0x0
    1d17:	75 f7                	jne    1d10 <str_into_string+0x20>
    1d19:	89 7d 08             	mov    DWORD PTR [ebp+0x8],edi
  return 0;
}

int str_into_string(char* str, int* index, int lim, char* str2) {
  int len = strlen(str2), i = 0;
  if (*index + len >= lim) return -1;
    1d1c:	8b 0e                	mov    ecx,DWORD PTR [esi]
    1d1e:	8d 04 0a             	lea    eax,[edx+ecx*1]
    1d21:	89 45 f0             	mov    DWORD PTR [ebp-0x10],eax
    1d24:	8b 7d f0             	mov    edi,DWORD PTR [ebp-0x10]
    1d27:	b8 ff ff ff ff       	mov    eax,0xffffffff
    1d2c:	3b 7d 10             	cmp    edi,DWORD PTR [ebp+0x10]
    1d2f:	7d 27                	jge    1d58 <str_into_string+0x68>
  while(i < len) {
    1d31:	85 d2                	test   edx,edx
    1d33:	74 21                	je     1d56 <str_into_string+0x66>
    1d35:	8b 7d 08             	mov    edi,DWORD PTR [ebp+0x8]
    1d38:	31 c0                	xor    eax,eax
    1d3a:	89 55 f0             	mov    DWORD PTR [ebp-0x10],edx
    1d3d:	8d 76 00             	lea    esi,[esi+0x0]
    str[*index] = str2[i];
    1d40:	0f b6 14 03          	movzx  edx,BYTE PTR [ebx+eax*1]
    i++;
    1d44:	83 c0 01             	add    eax,0x1

int str_into_string(char* str, int* index, int lim, char* str2) {
  int len = strlen(str2), i = 0;
  if (*index + len >= lim) return -1;
  while(i < len) {
    str[*index] = str2[i];
    1d47:	88 14 0f             	mov    BYTE PTR [edi+ecx*1],dl
    i++;
    (*index)++;
    1d4a:	8b 0e                	mov    ecx,DWORD PTR [esi]
    1d4c:	83 c1 01             	add    ecx,0x1
    1d4f:	89 0e                	mov    DWORD PTR [esi],ecx
}

int str_into_string(char* str, int* index, int lim, char* str2) {
  int len = strlen(str2), i = 0;
  if (*index + len >= lim) return -1;
  while(i < len) {
    1d51:	3b 45 f0             	cmp    eax,DWORD PTR [ebp-0x10]
    1d54:	75 ea                	jne    1d40 <str_into_string+0x50>
    1d56:	31 c0                	xor    eax,eax
    str[*index] = str2[i];
    i++;
    (*index)++;
  }
  return 0;
}
    1d58:	83 c4 04             	add    esp,0x4
    1d5b:	5b                   	pop    ebx
    1d5c:	5e                   	pop    esi
    1d5d:	5f                   	pop    edi
    1d5e:	5d                   	pop    ebp
    1d5f:	c3                   	ret    

00001d60 <rcr0>:

LS_INLINE void lcr0(uint_32 val) {
  __asm __volatile("movl %0,%%cr0" : : "r" (val));
}

LS_INLINE uint_32 rcr0(void) {
    1d60:	55                   	push   ebp
    1d61:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("movl %%cr0,%0" : "=r" (val));
    1d63:	0f 20 c0             	mov    eax,cr0
  return val;
}
    1d66:	5d                   	pop    ebp
    1d67:	c3                   	ret    
    1d68:	90                   	nop
    1d69:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00001d70 <rcr2>:

LS_INLINE void lcr2(uint_32 val) {
  __asm __volatile("movl %0,%%cr2" : : "r" (val));
}

LS_INLINE uint_32 rcr2(void) {
    1d70:	55                   	push   ebp
    1d71:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("movl %%cr2,%0" : "=r" (val));
    1d73:	0f 20 d0             	mov    eax,cr2
  return val;
}
    1d76:	5d                   	pop    ebp
    1d77:	c3                   	ret    
    1d78:	90                   	nop
    1d79:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00001d80 <rcr3>:

LS_INLINE void lcr3(uint_32 val) {
  __asm __volatile("movl %0,%%cr3" : : "r" (val));
}

LS_INLINE uint_32 rcr3(void) {
    1d80:	55                   	push   ebp
    1d81:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("movl %%cr3,%0" : "=r" (val));
    1d83:	0f 20 d8             	mov    eax,cr3
  return val;
}
    1d86:	5d                   	pop    ebp
    1d87:	c3                   	ret    
    1d88:	90                   	nop
    1d89:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00001d90 <rcr4>:

LS_INLINE void lcr4(uint_32 val) {
  __asm __volatile("movl %0,%%cr4" : : "r" (val));
}

LS_INLINE uint_32 rcr4(void) {
    1d90:	55                   	push   ebp
    1d91:	89 e5                	mov    ebp,esp
  uint_32 cr4;
  __asm __volatile("movl %%cr4,%0" : "=r" (cr4));
    1d93:	0f 20 e0             	mov    eax,cr4
  return cr4;
}
    1d96:	5d                   	pop    ebp
    1d97:	c3                   	ret    
    1d98:	90                   	nop
    1d99:	8d b4 26 00 00 00 00 	lea    esi,[esi+eiz*1+0x0]

00001da0 <rcs>:
  __asm __volatile("xchg %%bx, %%bx" : :);
}

//Segment registers

LS_INLINE uint_32 rcs(void) {
    1da0:	55                   	push   ebp
    1da1:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("mov %%cs, %0" : "=r" (val));
    1da3:	8c c8                	mov    eax,cs
  return val;
}
    1da5:	5d                   	pop    ebp
    1da6:	c3                   	ret    
    1da7:	89 f6                	mov    esi,esi
    1da9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001db0 <res>:

LS_INLINE uint_32 res(void) {
    1db0:	55                   	push   ebp
    1db1:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("mov %%es, %0" : "=r" (val));
    1db3:	8c c0                	mov    eax,es
  return val;
}
    1db5:	5d                   	pop    ebp
    1db6:	c3                   	ret    
    1db7:	89 f6                	mov    esi,esi
    1db9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001dc0 <rds>:

LS_INLINE uint_32 rds(void) {
    1dc0:	55                   	push   ebp
    1dc1:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("mov %%ds, %0" : "=r" (val));
    1dc3:	8c d8                	mov    eax,ds
  return val;
}
    1dc5:	5d                   	pop    ebp
    1dc6:	c3                   	ret    
    1dc7:	89 f6                	mov    esi,esi
    1dc9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001dd0 <rfs>:

LS_INLINE uint_32 rfs(void) {
    1dd0:	55                   	push   ebp
    1dd1:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("mov %%fs, %0" : "=r" (val));
    1dd3:	8c e0                	mov    eax,fs
  return val;
}
    1dd5:	5d                   	pop    ebp
    1dd6:	c3                   	ret    
    1dd7:	89 f6                	mov    esi,esi
    1dd9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001de0 <rgs>:

LS_INLINE uint_32 rgs(void) {
    1de0:	55                   	push   ebp
    1de1:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("mov %%gs, %0" : "=r" (val));
    1de3:	8c e8                	mov    eax,gs
  return val;
}
    1de5:	5d                   	pop    ebp
    1de6:	c3                   	ret    
    1de7:	89 f6                	mov    esi,esi
    1de9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001df0 <rss>:

LS_INLINE uint_32 rss(void) {
    1df0:	55                   	push   ebp
    1df1:	89 e5                	mov    ebp,esp
  uint_32 val;
  __asm __volatile("mov %%ss, %0" : "=r" (val));
    1df3:	8c d0                	mov    eax,ss
  return val;
}
    1df5:	5d                   	pop    ebp
    1df6:	c3                   	ret    
    1df7:	89 f6                	mov    esi,esi
    1df9:	8d bc 27 00 00 00 00 	lea    edi,[edi+eiz*1+0x0]

00001e00 <print_machine_stat>:
#ifndef __KLIB_MACHINE_C__
#define __KLIB_MACHINE_C__

#include "klib_machine.h"

void print_machine_stat() {
    1e00:	55                   	push   ebp
    1e01:	89 e5                	mov    ebp,esp
    1e03:	83 ec 28             	sub    esp,0x28
  printf("Machine status\n");
    1e06:	c7 04 24 71 20 00 00 	mov    DWORD PTR [esp],0x2071
    1e0d:	e8 7e fa ff ff       	call   1890 <printf>
  printf("    cr0: %x | cr2: %x | cr3: %x | cr4: %x \n", rcr0, rcr2, rcr3, rcr4);
    1e12:	c7 44 24 10 90 1d 00 	mov    DWORD PTR [esp+0x10],0x1d90
    1e19:	00 
    1e1a:	c7 44 24 0c 80 1d 00 	mov    DWORD PTR [esp+0xc],0x1d80
    1e21:	00 
    1e22:	c7 44 24 08 70 1d 00 	mov    DWORD PTR [esp+0x8],0x1d70
    1e29:	00 
    1e2a:	c7 44 24 04 60 1d 00 	mov    DWORD PTR [esp+0x4],0x1d60
    1e31:	00 
    1e32:	c7 04 24 90 20 00 00 	mov    DWORD PTR [esp],0x2090
    1e39:	e8 52 fa ff ff       	call   1890 <printf>
  __asm __volatile("ltr %0" : : "r" (sel));
}

LS_INLINE uint_16 rtr(void) {
  uint_16 sel;
  __asm __volatile("str %0" : "=r" (sel) : );
    1e3e:	66 0f 00 c8          	str    ax
  //printf("    gdtr: %x\n", rgdt);
  //printf("    ldtr: %x\n", rldt);
  //printf("    idtr: %x\n", ridt);
  printf("    tr: %x\n", rtr());
    1e42:	0f b7 c0             	movzx  eax,ax
    1e45:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
    1e49:	c7 04 24 81 20 00 00 	mov    DWORD PTR [esp],0x2081
    1e50:	e8 3b fa ff ff       	call   1890 <printf>
  printf("\n");
    1e55:	c7 04 24 7f 20 00 00 	mov    DWORD PTR [esp],0x207f
    1e5c:	e8 2f fa ff ff       	call   1890 <printf>
  printf("    cs: %x | ds: %x | es: %x | fs: %x | gs: %x | ss: %x \n", rcs, rds, res, rfs, rgs, rss);
    1e61:	c7 44 24 18 f0 1d 00 	mov    DWORD PTR [esp+0x18],0x1df0
    1e68:	00 
    1e69:	c7 44 24 14 e0 1d 00 	mov    DWORD PTR [esp+0x14],0x1de0
    1e70:	00 
    1e71:	c7 44 24 10 d0 1d 00 	mov    DWORD PTR [esp+0x10],0x1dd0
    1e78:	00 
    1e79:	c7 44 24 0c b0 1d 00 	mov    DWORD PTR [esp+0xc],0x1db0
    1e80:	00 
    1e81:	c7 44 24 08 c0 1d 00 	mov    DWORD PTR [esp+0x8],0x1dc0
    1e88:	00 
    1e89:	c7 44 24 04 a0 1d 00 	mov    DWORD PTR [esp+0x4],0x1da0
    1e90:	00 
    1e91:	c7 04 24 bc 20 00 00 	mov    DWORD PTR [esp],0x20bc
    1e98:	e8 f3 f9 ff ff       	call   1890 <printf>
}
    1e9d:	c9                   	leave  
    1e9e:	c3                   	ret    
    1e9f:	90                   	nop
