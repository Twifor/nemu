#include "cpu/exec/template-start.h"

#define instr ret

make_helper(ret_c3) {
	cpu.eip = MEM_R(reg_l(R_ESP)) - 1;
	reg_l(R_ESP) += 4;
	print_asm("ret");
	return 1;
}

#include "cpu/exec/template-end.h"
