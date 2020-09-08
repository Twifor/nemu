#include "cpu/exec/template-start.h"

#define instr jmp

make_helper(concat(jmp_i_, SUFFIX)) {
	int len = concat(decode_i_, SUFFIX)(eip + 1);
	cpu.eip += (DATA_TYPE_S)op_src->val;
	print_asm("jmp 0x%x", cpu.eip + len + 1);
	return len + 1;
}

make_helper(concat(jmp_rm_, SUFFIX)) {
	int len = concat(decode_rm_, SUFFIX)(eip + 1);
	cpu.eip = (DATA_TYPE_S)op_src->val - len - 1;
	print_asm("jmp %s", op_src->str);
	return len + 1;
}


#include "cpu/exec/template-end.h"
