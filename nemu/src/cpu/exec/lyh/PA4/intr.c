#include <setjmp.h>
#include "nemu.h"

extern jmp_buf jbuf;

void tmp_push_l (uint32_t val);

void raise_intr(uint8_t NO) {
	printf("int %x\n", NO);//debug use

	tmp_push_l(cpu.eflags);
	
	//Interrupt start
	

	

	longjmp(jbuf, 1);
}
