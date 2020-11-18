#include "cpu/exec/template-start.h"

#define instr jae

make_helper(concat(jae_i_, SUFFIX)) {
	int len = concat(decode_i_, SUFFIX) (eip + 1);
	print_asm("jae 0x%x", (DATA_TYPE_S)op_src->imm + cpu.eip + len + 1);
	if(cpu.CF == 0) cpu.eip += (DATA_TYPE_S)op_src->imm;
	return len + 1;
}

#include "cpu/exec/template-end.h"
