#include <serial.h>
#include <tipos.h>
#include <i386.h>
#include <isr.h>
#include <pic.h>
#include <debug.h>

#define SP_PORT 0x03F8

/* Serial Controler sub-SP_PORTs */
#define PORT_DATA  0 /* R/W - DATA flow */
#define PORT_IER   1 /* R/W - Interrupt Enable Register */
#define PORT_IIR   2 /* R   - Interrupt Id Register */
#define PORT_FCTRL 2 /*   W - FIFO Control */
#define PORT_LCTRL 3 /* R/W - Line Control */
#define PORT_MCTRL 4 /* R/W - MODEM Control */
#define PORT_LSTAT 5 /* R/W - Line Status */
#define PORT_MSTAT 6 /* R/W - MODEM Status */
#define PORT_SCRAT 7 /* R/W - Scratch ¿eh? */
#define PORT_DL_LSB 0 /* Divisor latch - LSB (need DLAB=1) */
#define PORT_DL_MSB 1 /* Divisor latch - MSB (need DLAB=1)  */

/*** REMEMBER: Don't use drugs while designing a chip:
 * 
 * 8.10 SCRATCHPAD REGISTER
 * This 8-bit Read Write Register does not control the UART
 * in anyway It is intended as a scratchpad register to be used
 * by the programmer to hold data temporarily
 */

/* Line Control Bits (type selection, and DLAB) */
#define LC_BIT5 0x00
#define LC_BIT6 0x01
#define LC_BIT7 0x02
#define LC_BIT8 0x03

#define LC_PARITY   0x08
#define LC_PARITY_E 0x10

#define LC_PARITY_O 0x00
#define LC_PARITY_ST 0x20
#define LC_BREAK    0x40
#define LC_DLAB     0x80

/* Line Status Bits */
#define LS_DR   0x01  /* Data Ready */
#define LS_OE   0x02  /* Overrun Error indicator (reading) */
#define LS_PE   0x04  /* Parity Error */
#define LS_FE   0x08  /* Framing Error (invalid stop bit) */
#define LS_BI   0x10  /* Break Interrupt */
#define LS_THRE 0x20  /* Transmitter Holding Register Empty */
#define LS_TEMT 0x40  /* Transmitter Empty */

/* FIFO Control Register Bits */
#define FC_FIFO       0x01 /* Enable FIFO (in y out) */
#define FC_CL_RCVFIFO 0x02 /* Clear RCVR FIFO */
#define FC_CL_XMTFIFO 0x04 /* Clear XMIT FIFO */
#define FC_MODE1      0x08 /* No tengo ni la más puta idea de qué hace este bit */
#define FC_TRIGGER_01 0x00 /* Trigger at 1-byte */
#define FC_TRIGGER_04 0x40 /* Trigger at 4-bytes */
#define FC_TRIGGER_08 0x80 /* Trigger at 8-bytes */
#define FC_TRIGGER_14 0xC0 /* Trigger at 14-bytes */

/* Iterrupt Id Bits */
#define II_nINTPEND  0x01 /* No interrupt pending */
#define II_ID_MASK   0x0E /* Mascara de IDs */
#define II_ID_RLS    0x06 /* Int ID: Receiver Line Status Interrupt */
#define II_ID_RDA    0x04 /* Int ID: Receiver Data Available */
#define II_ID_CTI    0x0C /* Int ID: Character Timeout Indicator */
#define II_ID_THRE   0x02 /* Int ID: Transmitter Holding Register Empty */
#define II_ID_MODEM  0x00 /* Int ID: MODEM Status */
#define II_INT_TOUT  0x08

/* Interrupt Enable Bits */
#define IE_RDA   0x01 /* Int Enable: Receiver Data Available */
#define IE_THRE  0x02 /* Int Enable: Transmitter Holding Register Empty */
#define IE_RLS   0x04 /* Int Enable: Receiver Line Status */
#define IE_MODEM 0x08 /* Int Enable: MODEM Status */


/** Char device **/

sint_32 serial_read(chardev* this, void* buf, uint_32 size) {
	return 0;
}

sint_32 serial_write(chardev* this, const void* buf, uint_32 size) {
	return 0;
}

uint_32 serial_flush(chardev* this) {
	return 0;
}

chardev* serial_open(int nro) { /* 0 para COM1, 1 para COM2, ... */

	return NULL;
}

/** Init **/
void serial_init() {
	
}

