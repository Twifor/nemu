#include "cpu/exec/template-start.h"

#define instr setge

void do_execute() {
	OPERAND_W(op_src, cpu.ZF == cpu.OF);
	print_asm_template1();
}

make_instr_helper(rm);
#include "cpu/exec/template-end.h"
