#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE result = op_src->val - 1;
	OPERAND_W(op_src, result);

	concat(updateCPU_, SUFFIX) (result);
	cpu.CF = op_src->val < 1;
	int s1, s2;
	int len = (DATA_BYTE << 3) - 1;
	s1 = op_src->val >> len;
	s2 = 0;
    cpu.OF = (s1 != s2 && s2 == cpu.SF);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
