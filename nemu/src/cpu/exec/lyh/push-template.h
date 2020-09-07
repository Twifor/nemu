#include "cpu/exec/template-start.h"

#define instr push

void do_execute() {
	reg_l (R_ESP) -= DATA_BYTE;
	MEM_W(reg_l(R_ESP), op_src->val);
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)
make_instr_helper(i)

#include "cpu/exec/template-end.h"
