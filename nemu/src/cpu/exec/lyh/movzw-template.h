#include "cpu/exec/template-start.h"

#define instr movzw

static void do_execute() {
	DATA_TYPE sv = op_src->val & 0xffff;//zero-extend
	OPERAND_W(op_dest, sv);
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
