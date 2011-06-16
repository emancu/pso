#include <serial.h>

#define COM1 0x03F8
#define COM2 0x02F8
#define COM3 0x03E8
#define COM4 0x02E8

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

/** Private functions **/
static int serial_received(int port){
  return inb(port + PORT_LSTAT) & 1;
}

static int is_transmit_empty(int port) {
   return inb(port + PORT_LSTAT) & 0x20;
}

static int port_addr(int com){
  if(com == 0) return COM1;
  if(com == 1) return COM2;
  if(com == 2) return COM3;
  return COM4;
}

static unsigned char serial_buffer[SERIAL_BUFF_SIZE];
static int serial_buf_pos;
static int serial_buf_end;

/** Char device **/
sint_32 serial_read(chardev* this, void* buf, uint_32 size) {
  // Verificamos que no se esté tratando de leer más de lo que permite el buffer
  if (size > SERIAL_BUFF_SIZE)
    return SERIAL_ERROR_READTOOLARGE;

  int i;
  char * buff = (char *) buf;
    printf("En el buffer habia: %s", serial_buffer);
  // Quizas un semaforo para copiar data ?
  do{
    buff[i++] = serial_buffer[serial_buf_pos++];
    serial_buf_pos %= SERIAL_BUFF_SIZE;
  }while(i < size && serial_buf_pos != serial_buf_end );

  return i;
}

sint_32 serial_write(chardev* this, const void* buf, uint_32 size) {
  uint_32 i;
  int port = ((chardev_serial*) this)->port;
  char * buff = (char *) buf;

  while (is_transmit_empty(port) == 0);

  for(i=0; i< size;i++)
    outb(port + PORT_DATA, buff[i]);

  return i;
}

uint_32 serial_flush(chardev* this) {
  return 0;
}

void isr_serial_c(){
  uint_8 c;
  inb(COM1 + PORT_IIR);
  printf("INT!!!");
  /* if the buffer is full, exit */
  if ((serial_buf_end + 1) % SERIAL_BUFF_SIZE == serial_buf_pos){
    printf("Buffer full");
    return;
  }
  // printf(" pos %d    end %d", serial_buf_pos, serial_buf_end);
  //
// FIXME: Como se de que puerto viene ?
  while (serial_received(COM1)) {
    c = inb(COM1 + PORT_DATA);
    serial_buffer[serial_buf_end++] = c;
    serial_buf_end %= SERIAL_BUFF_SIZE;

    if ((serial_buf_end +1) % SERIAL_BUFF_SIZE == serial_buf_pos){
      printf("se lleno.. deberia leer un poquito..");
      break;
    }
  }

  // printf("ilei %s", serial_buffer);
  // printf(" pos %d    end %d", serial_buf_pos, serial_buf_end);
  outb(0x20,0x20);
}


/* 0 para COM1, 1 para COM2, ... */
chardev* serial_open(int nro) {
  // char* buf[50];
  int port = port_addr(nro);
  chardev_serial* new_serial = (chardev_serial *) mm_mem_kalloc();

  new_serial->port = port;
  new_serial->dev.read = &serial_read;
  new_serial->dev.write = &serial_write;
  new_serial->dev.flush = &serial_flush;

  configure_serial_port(port);

  // serial_write((chardev*)new_serial, "Emiliano_campeon", 16);
  // printf("serial_read => %d", serial_read((chardev*)new_serial, buf, 20));

  // printf("Leimos %s", buf);

  return (chardev*)new_serial;
}

/** Init **/
void serial_init() {
  idt_register(36, &isr_serial, 0);
  serial_buf_pos = serial_buf_end = 0;
  serial_buffer[serial_buf_pos++] = 'H';
  reset_serial_port(COM1);
  reset_serial_port(COM2);
  reset_serial_port(COM3);
  reset_serial_port(COM4);
}

void reset_serial_port(int serial_port){
  outb(serial_port + PORT_IER,    0x00);    // Disable all interrupts
  outb(serial_port + PORT_IIR,    0x01);    // Clear Interrupt Id Register
  outb(serial_port + PORT_FCTRL,  0x00);    // Reset FIFO Control
  outb(serial_port + PORT_LSTAT,  0x60);    // Line status register
}

void configure_serial_port(int serial_port){
  outb(serial_port + PORT_IER, 0x01);       // Enable all interrupts
  outb(serial_port + PORT_LCTRL, 0x80);     // Enable DLAB (set baud rate divisor)
  outb(serial_port + PORT_DL_LSB, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outb(serial_port + PORT_DL_MSB, 0x00);    // (hi byte)
  outb(serial_port + PORT_LCTRL, 0x03);     // 8 bits, no parity, one stop bit. Disable DLAB
  outb(serial_port + PORT_FCTRL, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(serial_port + PORT_MCTRL, 0x0B);    // IRQs enabled, RTS/DSR set
  printf("Puerto %x Serial configurado", serial_port);
}

