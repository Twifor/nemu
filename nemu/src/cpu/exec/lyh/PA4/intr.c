#include <setjmp.h>
#include "nemu.h"

extern jmp_buf jbuf;

void tmp_push_l (uint32_t val);

void raise_intr(uint8_t NO) {
	printf("int %x\n", NO);//debug use

	tmp_push_l(cpu.eflags);
	tmp_push_l(cpu.cs.val);
	tmp_push_l(cpu.eip);

	uint32_t base_addr = cpu.idtr.base_addr;
	printf("%x\n",base_addr);
	
	//Interrupt start
	

	

	longjmp(jbuf, 1);
}
