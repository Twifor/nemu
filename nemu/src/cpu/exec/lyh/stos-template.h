#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat(stos_, SUFFIX)) {
	MEM_W(REG(R_EDI), REG(R_EAX));
	if (cpu.DF == 0) REG (R_EDI) += DATA_BYTE;
	else REG (R_EDI) -= DATA_BYTE;
	
	print_asm("stos%s", str(SUFFIX));

	return 1;
}

#include "cpu/exec/template-end.h"
