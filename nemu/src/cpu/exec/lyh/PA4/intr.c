#include <setjmp.h>
#include "nemu.h"

extern jmp_buf jbuf;

void tmp_push_l (uint32_t val);

void raise_intr(uint8_t NO) {
	printf("\nint %x\n", NO);//debug use

	//Interrupt start

	tmp_push_l(cpu.eflags);
	tmp_push_l(cpu.cs.val);
	tmp_push_l(cpu.eip);

	uint32_t base_addr = cpu.idtr.base_addr;
	uint32_t idx = base_addr + (NO << 3);//offset

	Gate gate;
	gate.val1 = lnaddr_read(idx, 4);//linear address
	gate.val2 = lnaddr_read(idx + 4, 4);

	cpu.cs.val = gate.selector;
	loadSregCache(R_CS);//load segment

	uint32_t addr = cpu.cs.cache.base + (gate.offset1 | (gate.offset2 << 16));

	cpu.eip = addr;//go

	longjmp(jbuf, 1);
}
