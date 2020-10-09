#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_, SUFFIX)) {
	swaddr_t s1 = REG(R_EAX), s2 = swaddr_read(reg_l(R_EDI), DATA_BYTE, R_ES);
	uint32_t res = s1 - s2;
	if (cpu.DF == 0) reg_l(R_EDI) += DATA_BYTE;
	else reg_l(R_EDI) -= DATA_BYTE;
	concat(updateCPU_, SUFFIX) (res);
	int len = (DATA_BYTE << 3) - 1;
	cpu.CF = s1 < s2;
    cpu.OF = ((s1 >> len) != (s2 >> len) && (s2 >> len) == cpu.SF);
	print_asm("scas%s", str(SUFFIX));

	return 1;
}

#include "cpu/exec/template-end.h"
