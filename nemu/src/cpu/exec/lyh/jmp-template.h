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

#if DATA_BYTE == 4
make_helper(ljmp) {
	uint32_t op1 = instr_fetch(eip + 1, 4) - 7;
	uint16_t op2 = instr_fetch(eip + 5, 2);
	cpu.eip = op1;		//modify eip
	cpu.cs.val = op2;	//modify CS segment register

	loadSregCache(R_CS);

	print_asm("ljmp %x,0x%x", op2, op1 + 7);
	return 7;
}
#endif


#include "cpu/exec/template-end.h"
