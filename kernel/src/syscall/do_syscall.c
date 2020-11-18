#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
uint32_t mm_brk(uint32_t);
int fs_ioctl(int, uint32_t, void *);

static void sys_brk(TrapFrame *tf) {
	tf->eax = mm_brk(tf->ebx);
}

static void sys_ioctl(TrapFrame *tf) {
	tf->eax = fs_ioctl(tf->ebx, tf->ecx, (void *)tf->edx);
}

static void sys_write(TrapFrame *tf) {
	if(tf->ebx == 1 || tf->ebx == 2) {
	#ifdef HAS_DEVICE
		void serial_printc(char ch);
		int i = 0;
		for(i = 0; i < tf->edx; i++) serial_printc(*((char *)((char *)tf->ecx + i)));
	#else
		asm volatile(".byte 0xd6" : : "a"(2), "c"(tf->ecx), "d"(tf->edx));
	#endif
		tf->eax = tf->edx;
	} else {
		panic("Unhandled system call: id = %d, eip = 0x%08x", tf->eax, tf->eip);
	}
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The `add_irq_handle' system call is artificial. We use it to
		 * let user program register its interrupt handlers. But this is
		 * very dangerous in a real operating system. Therefore such a
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			Log("New handler %x", tf->ebx);
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;
		case SYS_ioctl: sys_ioctl(tf); break;
		
		case SYS_write: sys_write(tf); break;
			
		/* TODO: Add more system calls. */

		default: panic("Unhandled system call: id = %d, eip = 0x%08x", tf->eax, tf->eip);
	}
}

