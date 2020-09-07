#include "cpu/exec/template-start.h"

#define instr pop

void do_execute() {
	printf("%s\n", REG_NAME(op_src->reg));
	OPERAND_W(op_src, MEM_R(R_ESP));
	reg_l (R_ESP) += DATA_BYTE;
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
