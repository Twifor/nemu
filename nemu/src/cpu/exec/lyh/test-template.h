#include "cpu/exec/template-start.h"

#define instr test

void do_execute() {
	cpu.OF = cpu.CF = 0;
	concat(updateCPU_, SUFFIX)((DATA_TYPE)(op_dest->val & op_src->val));
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
