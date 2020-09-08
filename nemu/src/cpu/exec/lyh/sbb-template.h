#include "cpu/exec/template-start.h"

#define instr sbb

void do_execute() {
	if (op_src->size == 1 && op_dest->size != 1)op_src->val = (int8_t)op_src->val;//sign-extend
	op_src->val += cpu.CF;
	DATA_TYPE ans = op_dest->val - op_src->val;
	OPERAND_W(op_dest, ans);
	concat(updateCPU_, SUFFIX) (ans);
	int len = (DATA_BYTE << 3) - 1;
	cpu.CF = op_dest->val < op_src->val;
    cpu.OF = ((op_dest->val >> len) != (op_src->val >> len) && (op_src->val >> len) == cpu.SF);
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
make_instr_helper(i2a)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"
