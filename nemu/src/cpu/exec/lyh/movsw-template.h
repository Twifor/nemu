#include "cpu/exec/template-start.h"

#define instr movsw

static void do_execute() {
	DATA_TYPE_S sv = (int16_t)(op_src->val & 0xffff);	//sign-extend
	OPERAND_W(op_dest, sv);
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
