#include <proc.h>
#include <i386.h>

proc_cpuid_dev cpuid_dev;

char* cpuid_msg = "Paseee capooo \0";

sint_32 proc_cpuid_read(chardev* this, void* buf, uint_32 size) {
	int i;
	proc_cpuid_dev* this_proc = (proc_cpuid_dev*) this;
	for(i= 0; i < size ; i++){
	    ((char *) buf)[i] = this_proc->buff[i + this_proc->fp];
	}
	//limites???
	this_proc->fp += size;
	((char *) buf)[size-1] = '\0';
	return size;
}

sint_32 proc_cpuid_write(chardev* this, const void* buf, uint_32 size) {
	return 0;
}

uint_32 proc_cpuid_flush(chardev* this) {
    cpuid_dev.busy = 0;
	return 0;
}


chardev* proc_cpuid_open(void) {
    cpuid_dev.fp = 0;
    cpuid_dev.busy = 1;
    cpuid_dev.buff = cpuid_msg;
    cpuid_dev.dev.clase = DEV_ID_PROC_CPUID;
    cpuid_dev.dev.read = &proc_cpuid_read;
    cpuid_dev.dev.write = NULL;
    cpuid_dev.dev.flush = &proc_cpuid_flush;
    return (chardev*)&cpuid_dev;
}
