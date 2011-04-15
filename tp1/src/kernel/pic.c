#include <pic.h>
#include <i386.h>

#define PIC1_PORT 0x20
#define PIC2_PORT 0xA0

void pic_reset(uint_8 addr_pic1, uint_8 addr_pic2) {
	outb(PIC1_PORT+0, 0x11); /* IRQs activas x flanco, cascada, y ICW4 */
	outb(PIC1_PORT+1, addr_pic1); /* Addr */
	outb(PIC1_PORT+1, 0x04); /* PIC1 Master, Slave ingresa Int.x IRQ2 */
	outb(PIC1_PORT+1, 0x01); /* Modo 8086 */
	outb(PIC1_PORT+1, 0xFF); /* Enmasca todas! */

	outb(PIC2_PORT+0, 0x11); /* IRQs activas x flanco, cascada, y ICW4 */
	outb(PIC2_PORT+1, addr_pic2); /* Addr */
	outb(PIC2_PORT+1, 0x02); /* PIC2 Slave, ingresa Int x IRQ2 */
	outb(PIC2_PORT+1, 0x01); /* Modo 8086 */
	outb(PIC2_PORT+1, 0xFF); /* Enmasca todas! */

	return;
}

void pic_enable() {
	outb(PIC1_PORT+1, 0x00);
	outb(PIC2_PORT+1, 0x00);
}

void pic_disable() {
	outb(PIC1_PORT+1, 0xFF);
	outb(PIC2_PORT+1, 0xFF);
}
