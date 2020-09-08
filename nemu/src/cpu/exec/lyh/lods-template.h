#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
	swaddr_write(reg_l(R_EAX), DATA_BYTE, swaddr_read(reg_l(R_ESI), DATA_BYTE));
	if (cpu.DF == 0) reg_l (R_ESI) += DATA_BYTE;
	else reg_l (R_ESI) -= DATA_BYTE;
	
	print_asm("loads_%s", str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
