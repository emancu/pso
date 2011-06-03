#include <tipos.h>
#include <i386.h>
#include <fdd.h>
#include <debug.h>
#include <loader.h>
#include <sem.h>

#define FDD_PORT 0x3F0

/* Intel 82077A ports: */
#define PORT_SRA  0x00  /* R   - Status Register A */
#define PORT_SRB  0x01  /* R   - Status Register B */
#define PORT_DOR  0x02  /* R/W - Digital Output Register */
#define PORT_TDR  0x03  /* R/W - Tape Drive Register */
#define PORT_MSR  0x04  /* R   - Main Status Register */
#define PORT_DSR  0x04  /*   W - Data-rate Select Register (ojo al piojo que es el mismo que el MSR) */
#define PORT_DATA 0x05  /* R/W - "Data", FIFO... esto es como tirás un "file miarchivo" y te dice: "miarchivo: data". */
#define PORT_RSVD 0x06  /* -/- - Este está porque 7 no es potencia de 2. */
#define PORT_DIR  0x07  /* R   - Digital Input Register */
#define PORT_CCR  0x07  /*   W - Configuration Control Register (es el mismo que el DIR) */

/* MSR bits: Main Status Register */
#define FDD_MSR_ACTA 0x01
#define FDD_MSR_ACTB 0x02
#define FDD_MSR_ACTC 0x04
#define FDD_MSR_ACTD 0x08
#define FDD_MSR_BUSY 0x10
#define FDD_MSR_NDMA 0x20
#define FDD_MSR_DIO  0x40
#define FDD_MSR_MRQ  0x80

/* DOR bits: Digital Output Register */
#define FDD_DOR_DR0  0x01
#define FDD_DOR_DR1  0x02
#define FDD_DOR_nREST 0x04
#define FDD_DOR_DMA  0x08
#define FDD_DOR_MOTA 0x10
#define FDD_DOR_MOTB 0x20
#define FDD_DOR_MOTC 0x40
#define FDD_DOR_MOTD 0x80

/* Available Commands (5 bits) */
#define FDD_CMD_READ_TRACK     0x02
#define FDD_CMD_SPECIFY        0x03
#define FDD_CMD_SENSE_DRV      0x04
#define FDD_CMD_WRITE          0x05
#define FDD_CMD_READ           0x06
#define FDD_CMD_RECALIBRATE    0x07
#define FDD_CMD_SENSE_INT      0x08
#define FDD_CMD_WRITE_DEL_DATA 0x09
#define FDD_CMD_READ_DEL_DATA  0x0C
#define FDD_CMD_READ_ID        0x0A
#define FDD_CMD_FORMAT_TRACK   0x0D
#define FDD_CMD_DUMPREG        0x0E
#define FDD_CMD_SEEK           0x0F
#define FDD_CMD_VERSION        0x10
#define FDD_CMD_SCAN_EQUAL     0x11
#define FDD_CMD_PERPENDICULAR_MODE 0x12
#define FDD_CMD_CONFIGURE      0x13
#define FDD_CMD_LOCK 0x14
#define FDD_CMD_VERIFY         0x16
#define FDD_CMD_SCAN_LOW_OR_EQUAL 0x19
#define FDD_CMD_SCAN_HIGH_OR_EQUAL 0x1D

#define FDD_CMD_MT   0x80
#define FDD_CMD_MFM  0x40
#define FDD_CMD_SKIP 0x20


/* Error codes */
#define FDD_ERROR_TIMEOUT -2
#define FDD_ERROR_VERSION_NOT_CORRECT -3
#define FDD_ERROR_LOCKCMD -4
#define FDD_ERROR_SENSE_INT_INVALID -5
#define FDD_ERROR_RECALIBRATE_NOTRK0 -6
#define FDD_ERROR_RECALIBRATE_WDEV -7
#define FDD_ERROR_READ_WRESPARAM -8
#define FDD_ERROR_READ_ABNORM -9
#define FDD_ERROR_READ_DE -10
#define FDD_ERROR_READ_ND -11
#define FDD_ERROR_READ_DD -12

/* Device variables */

fdc_stat fdc;
char floppy_buf[512];

/* Block device */

sint_32 fdd_block_read(blockdev* this, uint_32 pos, void* buf, uint_32 size) {
	return 0;
}

sint_32 fdd_block_write(blockdev* this, uint_32 pos, const void* buf, uint_32 size)  {
	return -1;
}



/* Devuelve un blockdev de la disketera en cuesti'on */
blockdev* fdd_open(int nro) {
	return NULL;
}

/************************/
/* Funciones exportadas */
/************************/

int fdd_read_sector(fdc_stat* fdc, char c, char h, char r, char eot, char mt, void* dest, char drv) {
  uint_32 count = FDD_DEFAULT_TIMEOUT;
  uint_8 cmd = 0x0 | FDD_CMD_READ | FDD_CMD_MFM | ((mt > 0) << 7);
  uint_8 cmd2 = 0x0 | ((h & 0x1) << 2) | (drv & 0x3);
  uint_8 st0, st1, st2, ret_c, ret_h, ret_r, ret_n;
  uint_8 msr;
  int st;

  if (!(fdc->dor & ((drv & 0xF) << 4))) { //Verifico si no está prendido el motor
    fdd_mot_en(fdc, drv, 1);
    while(count--); //TODO: Sleep para esperar 300ms
  }
  outb(FDD_PORT+PORT_CCR, 0x0); //Se establece el data rate en ccr (default es 0)
  outb(FDD_PORT+PORT_DOR, 0x4); //Se establece el data rate en ccr (default es 0)
  fdd_dev_sel(fdc, drv);
  //TODO: Recalibrar?
  fdd_recalibrate(fdc, drv);

  dma_set_floppy_read(dest);

  //~ fdd_print_status(fdc);
  //breakpoint();

  //Se envían los comandos
  st = fdd_send_byte(cmd);
  if (st < 0) return st;
  st = fdd_send_byte(cmd2);
  if (st < 0) return st;
  st = fdd_send_byte(c);
  if (st < 0) return st;
  st = fdd_send_byte(h);
  if (st < 0) return st;
  st = fdd_send_byte(r);
  if (st < 0) return st;
  st = fdd_send_byte(2);
  if (st < 0) return st;
  st = fdd_send_byte(eot);
  if (st < 0) return st;
  st = fdd_send_byte(0x1b);
  if (st < 0) return st;
  st = fdd_send_byte(0xff);
  if (st < 0) return st;

  //~ fdd_print_status(fdc);
  //Actualizo fdc
  fdc->c = c;
  fdc->h = h;
  fdc->r = r;
  fdc->n = 2;

  msr = inb(FDD_PORT+PORT_MSR);
  printf(" >FDC: reading - msr = %x - dma_status = %x", msr, inb(0x08));

	//~ count = FDD_DEFAULT_TIMEOUT;
	//~ while(count-- );

  fdd_wait_for_interrupt(0); //TODO: Interrupt no maneja timeout ni sleep

  msr = inb(FDD_PORT+PORT_MSR);
  printf(" >FDC: after interrupt - msr = %x - dma_status = %x", msr, inb(0x08));
  breakpoint();
  //~ msr = inb(FDD_PORT+PORT_MSR);
  // printf(" >FDC: reading - msr = %x", msr);
  // while (!(msr & FDD_MSR_MRQ) || (msr & FDD_MSR_BUSY) || (msr & (1 << drv))) {
    // msr = inb(FDD_PORT+PORT_MSR);
    // printf(" >FDC: reading - msr = %x", msr);
    // //breakpoint();
  // }

  //TODO: Se deberían manejar reintentos

  //Obtengo resultados
  st0 = fdd_get_byte(&st);
  if (st < 0) return st;
  st1 = fdd_get_byte(&st);
  if (st < 0) return st;
  st2 = fdd_get_byte(&st);
  if (st < 0) return st;
  ret_c = fdd_get_byte(&st);
  if (st < 0) return st;
  ret_h = fdd_get_byte(&st);
  if (st < 0) return st;
  ret_r = fdd_get_byte(&st);
  if (st < 0) return st;
  ret_n = fdd_get_byte(&st);
  if (st < 0) return st;

  //Actualizo fdc
  fdc->st0 = st0;
  fdc->st1 = st1;
  fdc->st2 = st2;

  //Verifico errores en los registros de estado
  if ((st0 & 0xC0) != 0)
    return FDD_ERROR_READ_ABNORM;

  if (st1 & 0x20)
    return FDD_ERROR_READ_DE;

  if (st1 & 0x04)
    return FDD_ERROR_READ_ND;

  if (st2 & 0x20)
    return FDD_ERROR_READ_DD;

  return 0;
}

/******************/
/* Interrupciones */
/******************/

char fdd_int_recv = 0;

void isr_fdd_c() {
  printf("* FDD: interrupt! (%d)", error_num);
  fdd_int_recv = 1;
  outb(0x20, 0x20);
  // //breakpoint();
  return;
}

//TODO: Puede ser necesario implementarlo con bloqueo del timer tick
void fdd_wait_for_interrupt(uint_32 timeout) {
  //TODO: implementar timeout
  while(fdd_int_recv == 0); //TODO: implementar con sleep
  fdd_int_recv = 0;
}

/**************************************************/
/* Funciones generales de comunicación con el fdc */
/**************************************************/

int fdd_send_byte(uint_8 byte) {
	uint_32 timeout = FDD_DEFAULT_TIMEOUT;
	char msr = 0;
	while (timeout-- && !((msr & FDD_MSR_MRQ) && !(msr & FDD_MSR_DIO))) {
    //TODO: Faltaría un sleep
		msr = inb(FDD_PORT+PORT_MSR);
	}
  if (!timeout)
    return FDD_ERROR_TIMEOUT;

  outb(FDD_PORT+PORT_DATA, byte);
  return 0;
}

uint_8 fdd_get_byte(int* stat) {
  uint_32 timeout = FDD_DEFAULT_TIMEOUT;
  char msr = 0;
  while (timeout-- && !((msr & FDD_MSR_MRQ) && (msr & FDD_MSR_DIO))) {
    //TODO: Faltaría un sleep
    msr = inb(FDD_PORT+PORT_MSR);
  }
  if (!timeout) {
    *stat = FDD_ERROR_TIMEOUT;
    return 0;
  }

  *stat = 0;
  return inb(FDD_PORT+PORT_DATA);
}

/*********************************/
/* Funciones de comandos del fdc */
/*********************************/

int fdd_version() {
  int st;
  uint_8 ver;
  st = fdd_send_byte(FDD_CMD_VERSION);
  if (st < 0)
    return st;
  ver = fdd_get_byte(&st);
  if (st < 0)
    return st;
  return ver;
}

void fdd_mot_en(fdc_stat* fdc, uint_8 motor, char st) {
  uint_8 dor = fdc->dor;
  if (st)
    dor |= (1 << (motor+4));
  else
    dor &= ~(1 << (motor+4));
  outb(FDD_PORT+PORT_DOR, dor);
  fdc->dor = dor;
}

void fdd_dev_sel(fdc_stat* fdc, uint_8 dev) {
  uint_8 dor = fdc->dor;
  dor &= ~0x3; //Limpio los primeros dos bits
  dor |= (dev & 0x3); //Seteo los primeros dos bits filtrando 'dev'
  fdc->dor = dor;
  outb(FDD_PORT+PORT_DOR, dor);
}

//En duda... al parecer hay que usar configure
void fdd_dma_en(fdc_stat* fdc, char st) {
  uint_8 dor = fdc->dor;
  if (st)
    dor &= ~0x8;
  else
    dor |= 0x8;
  outb(FDD_PORT+PORT_DOR, dor);
  fdc->dor = dor;
}

char get_dma_st() {
  uint_8 msr = inb(FDD_PORT+PORT_MSR);
  return (msr & 0x20)? 0 : 1;
}


int fdd_configure(fdc_stat* fdc, char eis, char efifo, char poll, char fifothr, char pretrk) {
  int st;
  char param1 = 0x0; //Estas variables tendran los parámetros del comando configure
  param1 |= ((eis > 0) << 6); //Preparo param1 con los valores de las variables
  param1 |= ((efifo <= 0) << 5); //Si me llega <= 0 significa deshabilitá polling, entonces pongo un 1
  param1 |= ((poll <= 0) << 4); //Si me llega <= 0 signifca deshabilitá polling, entonces pongo un 1
  param1 |= (fifothr & 0xF);

  st = fdd_send_byte(FDD_CMD_CONFIGURE); //Envío el comando configure
  if (st < 0)
    return st;
  st = fdd_send_byte(0x00); //El comando continúa con un 0
  if (st < 0)
    return st;
  st = fdd_send_byte(param1); //Envío el primer parámetro
  if (st < 0)
    return st;
  fdc->config1 = param1;
  st = fdd_send_byte(pretrk); //Envío el primer parámetro
  if (st < 0)
    return st;
  fdc->config2 = pretrk;
  return 0;
}

int fdd_specify(fdc_stat* fdc, char srt, char hut, char hlt, char nd) {
  char param1 = 0x0, param2 = 0x0;
  int st;
  param1 = ((srt & 0x0F) << 4) | (hut & 0x0F);
  param2 = ((hlt & 0x7F) << 1) | (nd > 0);
  st = fdd_send_byte(FDD_CMD_SPECIFY);
    if (st < 0) return st;
  st = fdd_send_byte(param1);
    if (st < 0) return st;
  st = fdd_send_byte(param2);
    if (st < 0) return st;
  fdc->srt_hut = param1;
  fdc->hlt_nd = param2;
  return 0;
}

int fdd_dumpreg(fdc_stat* fdc) {
  int st;
  st = fdd_send_byte(FDD_CMD_DUMPREG);
    if (st < 0) return st;
  fdc->pcn0 = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->pcn1 = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->pcn2 = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->pcn3 = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->srt_hut = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->hlt_nd = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->sc_eot = fdd_get_byte(&st);
    if (st < 0) return st;
  fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->config1 = fdd_get_byte(&st);
    if (st < 0) return st;
  fdc->config2 = fdd_get_byte(&st);
    if (st < 0) return st;

  return 0;
}

int fdd_lock(fdc_stat* fdc, char lock) {
  int st, ret;
  st = fdd_send_byte(FDD_CMD_LOCK | ((lock > 0) << 7));
  if (st < 0)
    return st;
  ret = fdd_get_byte(&st);
  if (st < 0)
    return st;
  if (ret != (0x0 | ((lock > 0) << 4)))
    return FDD_ERROR_LOCKCMD;
  return 0;
}

int fdd_sense_interrupt_status(fdc_stat* fdc) {
  int st;
  uint_8 pnc, st0;
  st = fdd_send_byte(FDD_CMD_SENSE_INT);
  if (st < 0) return st;
  st0 = fdd_get_byte(&st);
  if (st < 0) return st;
  pnc = fdd_get_byte(&st);
  if (st < 0) return st;
  if (st0 == 0x80) return FDD_ERROR_SENSE_INT_INVALID;
  fdc->st0 = st0;
  return 0;
}

int fdd_recalibrate(fdc_stat* fdc, char ds) {
  int st;
  uint_8 msr;
  fdd_mot_en(fdc, ds, 1);
  fdd_dev_sel(fdc, ds);
  st = fdd_send_byte(FDD_CMD_RECALIBRATE); //Envío el comando
  if (st < 0) return st;
  st = fdd_send_byte(ds & 0x3); //Envío el drive
  if (st < 0) return st;
  
  msr = inb(FDD_PORT+PORT_MSR);
  //TODO: Tengo que chequear que termine la sección de commando? Aparentemente si.
  while (msr & 0x10) {
    msr = inb(FDD_PORT+PORT_MSR);
  }

  //TODO: las interrupciones podrían estar desactivadas
  fdd_wait_for_interrupt(0); //Ver de trabajar con sleep (might take up to 3s)


  //TODO: Esto loopea por siempre
  // while (msr & (1 << (ds & 0x3))) { //Armo la máscara con un 1 en el DRV x BUSY
    // //TODO: Falta sleep (o manejar esto por interrupciones)
    // msr = inb(FDD_PORT+PORT_MSR);
  // }

  st = fdd_sense_interrupt_status(fdc);
  if (st < 0) return st;
  if ((fdc->st0 & 0xC0) == 0x4) { //TODO: Verificar EC da error siempre, verifico IC.
    printf("* FDC: Recalibrate NOTRK0");
    return FDD_ERROR_RECALIBRATE_NOTRK0;
  }
  if ((fdc->st0 & 0x3) != ds)
    return FDD_ERROR_RECALIBRATE_WDEV;
  return 0;
}

/****************************/
/* Lectura de stado del fdc */
/****************************/

void fdd_update_R_regs(fdc_stat* fdc) {
  fdc->srA = inb(FDD_PORT+PORT_SRA);
  fdc->srB = inb(FDD_PORT+PORT_SRB);
  fdc->msr = inb(FDD_PORT+PORT_MSR);
  fdc->dir = inb(FDD_PORT+PORT_DIR);
}

int fdd_update_status(fdc_stat* fdc) {
  fdd_update_R_regs(fdc);
  int st = fdd_dumpreg(fdc);
  if (st < 0) return st;
  return 0;
}

void fdd_print_status(fdc_stat* fdc) {
  int st = fdd_update_status(fdc);
  if (st < 0) {
    printf("! FDC: Error en fdd_update_status.");
    return;
  }
  printf("Fdd internal status: ");
  printf(" st0 %x | st1 %x | c %x | h %x | r %x | n %x", fdc->st0, fdc->st1, fdc->c, fdc->h, fdc->r, fdc->n);
  printf(" dor %x | srA %x | srB %x | dsr %x | dir %x | ccr %x | msr %x", fdc->dor, fdc->srA, fdc->srB, fdc->dsr, fdc->dir, fdc->ccr, fdc->msr);
  printf(" config1 %x | config2 %x", fdc->config1, fdc->config2);
  printf(" pcn0 %x | pcn1 %x | pcn2 %x | pcn3 %x", fdc->pcn0, fdc->pcn1, fdc->pcn2, fdc->pcn3);
  printf(" str_hut %x | hlt_nd %x | sc_eot %x | perp %x", fdc->srt_hut, fdc->hlt_nd, fdc->sc_eot, fdc->perpendicular);
}

int fdd_full_reset(fdc_stat* fdc) {
  //Establezco DOR en 0x40 para dehabilitar los motores y las interrupciones.
  //De otra forma el reset provocará un IRQ6 (la datasheet no dice nada, pero esto aparece en wiki osdev)
  outb(FDD_PORT+PORT_DOR, 0x0C);
  //Establezco DSR en 0x80, provocando un reset
  outb(FDD_PORT+PORT_DSR, 0x80);
  //Establezco el CCR en 0
  outb(FDD_PORT+PORT_CCR, 0x0);
 
  outb(FDD_PORT+PORT_DSR, 0x0);

  printf("Just reset: msr %x | dir %x", inb(FDD_PORT+PORT_MSR), inb(FDD_PORT+PORT_DIR));
  ////breakpoint();

  fdc->ccr = 0x0;
  fdc->dsr = 0x0; //El bit 8 se desactiva luego del reset
  fdc->dor = 0x0C;

  printf(" >FDC_RESET: Waiting for interrupt.");
  fdd_wait_for_interrupt(0);

  outb(FDD_PORT+PORT_CCR, 0x0);
  outb(FDD_PORT+PORT_DSR, 0x0);
  outb(FDD_PORT+PORT_DOR, 0x0C);
  printf("Just reset - after int: msr %x | dir %x", inb(FDD_PORT+PORT_MSR), inb(FDD_PORT+PORT_DIR));
  ////breakpoint();

  int st;
  uint_8 count = 4;
  //Senseo los drives
  printf(" >FDC_RESET: Sensing drivers.");
  while(count--) {
    st = fdd_sense_interrupt_status(fdc);
    if (st < 0) return st;
  }
   // fdd_print_status(fdc);
   // //breakpoint();
  
  printf(" >FDC_RESET: Configuring fdc.");
  //Configuro el fdc con *configure*
  st = fdd_configure(fdc, 1, 1, 0, 8, 0x0);
  if (st < 0)
    return st;
  // fdd_print_status(fdc);
  // //breakpoint();

  printf(" >FDC_RESET: Specifing fdc.");
  //Especifico el fdc con *specify*
  st = fdd_specify(fdc, 8, 5, 15, 0);
  if (st < 0)
    return st;
  // fdd_print_status(fdc);
  // //breakpoint();

  printf(" >FDC_RESET: Recalibrating devices.");
  //Recalibro los devices
  count = 4;
  while(count--) {
    st = fdd_recalibrate(fdc, count);
    if (st < 0 && st != FDD_ERROR_RECALIBRATE_NOTRK0) return st;
  }
  // fdd_print_status(fdc);
  // //breakpoint();
  
  printf(" >FDC_RESET: Shutting motors.");
  //Apago todos los motores
  count = 4;
  while(count--) {
    fdd_mot_en(fdc, count, 0);
  }
  // fdd_print_status(fdc);
  // //breakpoint();

  printf(" >FDC_RESET: Choosing drive 0.");
  //Eligo el drive 0
  fdd_dev_sel(fdc, 0);
  // //breakpoint();
  // fdd_print_status(fdc);

  return 0;
}

/** Init **/

//TODO: Parecería andar bien... dir, srA y srB tienen valores extraños al final. 
void fdd_init(void) {
  int st;
	//Chequeo la versión
  printf("----FDC Initialization ----");
  printf("FDC: Checking fdc version");
  uint_8 ver = fdd_version();
  if (ver != 0x90) {
    if (ver == FDD_ERROR_TIMEOUT) {
      printf("! FDC: Error de timeout al chequear versión.");
    } else {
      printf("! FDC: Error de fdc, versión inválida.");
    }
  }
  printf("FDC: Version check complete.");

  //Registro la interrupción
  printf("FDC: Registering floppy interrupt");
  idt_register(38, &isr_fdd, 0);

  // fdd_print_status(&fdc);
  st = 0;

  //breakpoint();
  // Reseteo el fdc 
  printf("FDC: Reseting fdc...");
  st = fdd_full_reset(&fdc);
  if (st < 0)
    printf("! FDC: Error during fdc reset: %d", st);
  else
    printf("FDC: Reset successful.");

  //Seteo el dma
  printf("FDC: Initializing DMA controller.");
  dma_setup();
  printf("FDC: DMA initialization done.");

	fdd_print_status(&fdc);
  //Test de lectura del floppy
  printf("Floppy test read: buffer = %x", &floppy_buf);
  st = fdd_read_sector(&fdc, 0, 0, 1, 1, 0, &floppy_buf, 0);
  breakpoint();
  if (st < 0)
    printf("! FDC: Error reading sector 1 (%d)", st);
  
}
