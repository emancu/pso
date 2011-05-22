export LC_ALL=C

# Programs
OBJDUMP=objdump
OBJCOPY=objcopy
CC=gcc
MKFS=/sbin/mkfs.vfat

# Parámetros de programas
NASMFLAGS=-I include/ -g -ggdb -D__KERNEL__
CFLAGS=-m32 -g -ggdb -Wall -Werror  -O0 -I include/ \
  -fno-zero-initialized-in-bss -fno-stack-protector -fno-strict-aliasing -ffreestanding -D__KERNEL__
CXXFLAGS=-O2
LDFLAGS=-static -m elf_i386 -nostdlib

# Directorios
DIRBOOT=boot/
DIRKERN=kernel/
DIROUT=bin/
DIRTMP=/tmp/

# Boot loader
BIN_BOOT=$(DIRBOOT)floppyboot.bin

# Kernel
OBJS_KERN=kernel/kinit.o kernel/a20.o kernel/kernel.o kernel/gdt.o kernel/idt.o \
  kernel/isr.o kernel/pic.o kernel/vga.o kernel/sched.o \
  kernel/mm.o kernel/debug.o kernel/loader.o kernel/tasks.o \
  kernel/syscalls.o kernel/sem.o kernel/lib_str.o kernel/klib_machine.o kernel/tss.o kernel/loader_switch.o \
  kernel/device.o kernel/con.o kernel/proc.o kernel/fs.o kernel/serial.o kernel/fat12.o
BIN_KERN=$(DIROUT)kernel.bin
DUMP_KERN=$(DIROUT)kernel.bin.asm $(DIROUT)kernel.bin.elf $(DIROUT)kernel.bin.dbg
SYM_KERN=$(DIROUT)kernel.sym

# Tareas
TASKS=tasks/task1.pso tasks/task2.pso
TASKS_ELF:=$(TASKS:.pso=.elf)
OBJS_TASKS:=$(TASKS:.pso=.o) tasks/pso_head.o tasks/pso_tail.o tasks/syscalls.o

# Imagenes
DISK_LABEL="KERNEL  PSO"
IMG_BASE=$(DIROUT)base.img
IMG_FLOPPY=$(DIROUT)floppy.img

.PHONY: all clean

all: $(IMG_FLOPPY) $(DUMP_KERN) $(SYM_KERN)

# Bootloader - disk
### TODO ###

# Kernel
kernel/%.o: kernel/%.asm
	nasm $(NASMFLAGS) -felf32 -I $(DIRKERN) -o $@ $<

kernel/tasks.o: kernel/tasks.asm $(TASKS) $(TASKS_ELF)
	nasm $(NASMFLAGS) -felf32 -I $(DIRKERN) -o $@ $<

tasks/pso_head.o: tasks/pso_head.asm
	nasm $(NASMFLAGS:__KERNEL__=__TASK__) -felf32 -I $(DIRKERN) -o $@ $<

tasks/pso_tail.o: tasks/pso_tail.asm
	nasm $(NASMFLAGS:__KERNEL__=__TASK__) -felf32 -I $(DIRKERN) -o $@ $<

tasks/syscalls.o: kernel/syscalls.asm
	nasm $(NASMFLAGS:__KERNEL__=__TASK__) -felf32 -I $(DIRKERN) -o $@ $<

tasks/%.pso: tasks/%.elf
	$(OBJCOPY) -S -O binary $< $@

tasks/%.o: tasks/%.c
	$(CC) $(CFLAGS:__KERNEL__=__TASK__) -c -o $@ $<

tasks/%.elf: tasks/pso_head.o tasks/syscalls.o tasks/%.o tasks/pso_tail.o $(BIN_LIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0x400000 -o $@ tasks/pso_head.o tasks/syscalls.o $(@:.elf=.o) $(LDPSO) tasks/pso_tail.o

$(BIN_KERN).elf: $(OBJS_KERN) 
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x1200 -o $@ $(OBJS_KERN) $(LDPSO)

$(BIN_KERN): $(BIN_KERN).elf
	$(OBJCOPY) -S -O binary $< $@

$(BIN_KERN).asm: $(BIN_KERN).elf
	$(OBJDUMP) -S -M intel $< >$@
	
$(BIN_KERN).dbg: $(BIN_KERN).elf
	$(OBJCOPY) --only-keep-debug $< $@

$(SYM_KERN): $(BIN_KERN).elf 
	nm $< | cut -d ' ' -f 1,3 > $@

# Images
$(IMG_BASE):
	@dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	$(MKFS) -F 12 -f 2 -n $(DISK_LABEL) $@

$(IMG_FLOPPY): $(BIN_BOOT) $(BIN_KERN) $(IMG_BASE)
	[ -f $(IMG_FLOPPY) ] || dd if=$(IMG_BASE) of=$@ bs=512 count=2880 2>/dev/null
	dd if=$< of=$@ bs=1 count=3 conv=notrunc 2>/dev/null
	dd if=$< of=$@ bs=1 count=450 seek=62 skip=62 conv=notrunc 2>/dev/null
	mcopy -obi $@ $(BIN_KERN) ::kernel.bin
	for T in $(TASKS); do mcopy -obi $@ $$T ::`basename $$T`; done;

# Otros
clean:
	rm -f $(BIN_KERN) $(IMG_FLOPPY) $(IMG_BASE) $(OBJS_KERN) $(DUMP_KERN) $(SYM_KERN) $(OBJS_TASKS) $(TASKS) $(TASKS:.pso=.elf)
	#rm -f deps

# Dependencias
deps: Makefile
	makedepend -I include/  kernel/*.c  tasks/*.c $(OBJS_LIB:.o=.c) -f- > deps
	for f in kernel/*.asm; do nasm $(NASMFLAGS) -felf32 -I $(DIRKERN) -M $$f; done >> deps 2>/dev/null
#	$(CC) $(CFLAGS) -MM kernel/*.c $(OBJS_LIB:.o=.c) > deps

-include deps

# DO NOT DELETE
