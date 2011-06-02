#include <dma.h>

/************************/
/* Funciones exportadas */
/************************/

int dma_setup() {
// set DMA channel 2 to transfer data from 0x1000 - 0x33ff in memory
  // paging must map this _physical_ memory elsewhere and _pin_ it from paging to disk!
  // set the counter to 0x23ff, the length of a track on a 1.44 MiB floppy - 1 (assuming 512 byte sectors)
  // transfer length = counter + 1
      outb(0x0a,0x06);      // mask DMA channel 2 and 0 (assuming 0 is already masked)
      outb(0x0c,0xFF);      // reset the master flip-flop
      outb(0x04,0xc8);         // address to 0 (low byte)
      outb(0x04,0xfc);      // address to 0x10 (high byte)
      outb(0x0c,0xFF);      // reset the master flip-flop (again!!!)
      outb(0x05,512 & 0xF);      // count to 0x23ff (low byte)
      outb(0x05,512 >> 8);      // count to 0x23ff (high byte),
      outb(0x81,0x9);        // external page register to 0 for total address of 00 10 00
      outb(0x0a,0x01);      // unmask DMA channel 2
      return 0; 
}

void dma_set_floppy_read(void* dest) {
    outb(0x0a,0x06);     // mask DMA channel 2 and 0 (assuming 0 is already masked)
    outb(0x0b,0x56);     // 01010110
                      // single transfer, address increment, autoinit, read, channel2)
    outb(0x0a,0x01);     // unmask DMA channel 2
}

/*
int dma_setup() {
  dma_set_mask(DMA_MASK_ALL); //Enmascaro todos los DRQ
  dma_set_cmd_reg(1, 0, 1);
  dma_set_mask(DMA_MASK_CH2); //Desenmascaro solo DRQ2 (Floppy)
  return 0;
}

void dma_set_floppy_read(void* dest) {
  dma_set_mask(DMA_MASK_ALL);
  dma_set_mode(2, 1, 0, 0, 1); //Ch2, write to mem, demand mode, increase, no auto-init
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop
  outb(DMAIO_ADDR, (uint_32)dest & 0xF); //Escribo el primer bit de la dirección
  outb(DMAIO_ADDR, ((uint_32)dest >> 4)); //Escribo el segundo bit de la dirección
  outb(DMAIO_TOP, ((uint_32)dest >> 8) & 0xF); //Escribo el registro de página con los últimos 4 bits
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop nuevamente
  outb(DMAIO_COUNT, 511 & 0xF); //Escribo el primer byte de conteo
  outb(DMAIO_COUNT, 511 >> 4); //Escribo el segundo byte de conteo
  dma_set_mask(DMA_MASK_CH2);
}

void dma_set_floppy_write(void* src) {
  dma_set_mask(DMA_MASK_ALL);
  dma_set_mode(2, 2, 0, 0, 0); //Ch2, read from mem, demand mode, increase, no auto-init
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop
  outb(DMAIO_ADDR, (uint_32)src & 0xF); //Escribo el primer bit de la dirección
  outb(DMAIO_ADDR, ((uint_32)src >> 4)); //Escribo el segundo bit de la dirección
  outb(DMAIO_TOP, ((uint_32)src >> 8) & 0xF); //Escribo el registro de página con los últimos 4 bits
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop nuevamente
  outb(DMAIO_COUNT, 511 & 0xF); //Escribo el primer byte de conteo
  outb(DMAIO_COUNT, 511 >> 4); //Escribo el segundo byte de conteo
  dma_set_mask(DMA_MASK_CH2);
}
*/

/**********************/
/* Funciones internas */
/**********************/

int dma_set_cmd_reg(char ce, char dreq_low, char dack_high) {
  uint_8 cmd = 0x0;
  cmd |= ((ce > 0) << 2);
  cmd |= ((dreq_low > 0) << 6);
  cmd |= ((dack_high > 0) << 7);
  return 0;
}

int dma_set_mode(char ch, char tra, char auto_init, char down, char mode) {
  dma_set_mask(0xF);
  uint_8 mode_b = 0x0;
  mode_b |= (ch & 0x3);
  mode_b |= ((tra & 0x3) << 2);
  mode_b |= ((auto_init > 0) << 4);
  mode_b |= ((down > 0) << 5);
  mode_b |= ((mode & 0x3) << 6);
  outb(DMAIO_MODE, mode_b);
  dma_set_mask(0xB);
  return 0;
}

int dma_read_mask() {
  return inb(DMAIO_MASK);
}
void dma_set_mask(uint_8 mask) {
  outb(DMAIO_MASK, mask & 0xF);
}

