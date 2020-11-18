#include "cpu/exec/template-start.h"

#define instr in

uint32_t pio_read(ioaddr_t, size_t);

make_helper(concat(in_i2a_, SUFFIX)) {
	uint8_t imm = instr_fetch(eip + 1, 1);
	concat(reg_, SUFFIX)(R_EAX) = pio_read(imm, DATA_BYTE);
	printf("\nin  (%d->%x) %x\n", DATA_BYTE, imm, concat(reg_, SUFFIX)(R_EAX));
	print_asm("in %x", imm);
	return 2;
}

make_helper(concat(in_, SUFFIX)) {
	concat(reg_, SUFFIX)(R_EAX) = pio_read(reg_w(R_DX), DATA_BYTE);
	printf("\nin  (%d->%x) %x\n", DATA_BYTE, reg_w(R_DX), concat(reg_, SUFFIX)(R_EAX));
	print_asm("in");
	return 1;
}

#include "cpu/exec/template-end.h"
