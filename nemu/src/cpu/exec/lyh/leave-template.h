#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat(leave_, SUFFIX)) {
	if(DATA_BYTE == 2)	{
		cpu.gpr[4]._16 = cpu.gpr[5]._16;//sp = bp
		cpu.gpr[5]._16 = MEM_R(reg_l(R_ESP), R_SS);//POP
		reg_l(R_ESP) += 2;
	} else if(DATA_BYTE == 4) {
		cpu.gpr[4]._32 = cpu.gpr[5]._32;//sp = bp
		cpu.gpr[5]._32 = MEM_R(reg_l(R_ESP), R_SS);//POP
		reg_l(R_ESP) += 4;
	}
	print_asm("leave");
	return 1;
}

#include "cpu/exec/template-end.h"
