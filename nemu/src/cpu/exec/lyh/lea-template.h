#include "cpu/exec/template-start.h"

#define instr lea

void do_execute() {
	OPERAND_W(op_dest, op_src->addr);	//not op_src->val !!
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
