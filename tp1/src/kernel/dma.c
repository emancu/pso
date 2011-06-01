#include <dma.h>


int dma_setup() {
  dma_set_mask(DMA_MASK_ALL); //Enmascaro todos los DRQ
  dma_set_cmd_reg(1, 0, 1);
  dma_set_mask(DMA_MASK_CH2); //Desenmascaro solo DRQ2 (Floppy)
  return 0;
}

int dma_set_cmd_reg(char ce, char dreq_low, char dack_high) {
  uint_8 cmd = 0x0;
  cmd |= ((ce > 0) << 2);
  cmd |= ((dreq_low > 0) << 6);
  cmd |= ((dreq_high > 0) << 7);
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

void dma_set_floppy_read(uint_32 dest, uint_32 count) {
  dma_set_mask(DMA_MASK_ALL);
  dma_set_mode(2, 1, 0, 0, 0); //Ch2, write to mem, demand mode, increase, no auto-init
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop
  outb(DMAIO_ADDR, src & 0xF); //Escribo el primer bit de la dirección
  outb(DMAIO_ADDR, (src >> 4)); //Escribo el segundo bit de la dirección
  outb(DMAIO_TOP, (src >> 8) & 0xF); //Escribo el registro de página con los últimos 4 bits
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop nuevamente
  outb(DMAIO_COUNT, src & 0xF); //Escribo el primer byte de conteo
  outb(DMAIO_COUNT, src >> 4); //Escribo el segundo byte de conteo
  dma_set_mask(DMA_MASK_CH2);
}

void dma_set_floppy_write(uint_32 src, count) {
  dma_set_mask(DMA_MASK_ALL);
  dma_set_mode(2, 2, 0, 0, 0); //Ch2, read from mem, demand mode, increase, no auto-init
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop
  outb(DMAIO_ADDR, src & 0xF); //Escribo el primer bit de la dirección
  outb(DMAIO_ADDR, (src >> 4)); //Escribo el segundo bit de la dirección
  outb(DMAIO_TOP, (src >> 8) & 0xF); //Escribo el registro de página con los últimos 4 bits
  outb(DMAIO_FLIPFLOP, 0xFF); //Reseteo el flip flop nuevamente
  outb(DMAIO_COUNT, src & 0xF); //Escribo el primer byte de conteo
  outb(DMAIO_COUNT, src >> 4); //Escribo el segundo byte de conteo
  dma_set_mask(DMA_MASK_CH2);
}
