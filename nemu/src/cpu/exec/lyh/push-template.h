#include "cpu/exec/template-start.h"

#define instr push

void do_execute() {
	if(DATA_BYTE == 2) {
		reg_l(R_ESP) -= 2;
		swaddr_write(reg_l(R_ESP), 2, op_src->val);
	} else {
		reg_l(R_ESP) -= 4;
		swaddr_write(reg_l(R_ESP), 4, op_src->val);
	}
	print_asm_template1();
}
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif
make_instr_helper(i)

#include "cpu/exec/template-end.h"
