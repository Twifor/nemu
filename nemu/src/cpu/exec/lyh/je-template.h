#include "cpu/exec/template-start.h"

#define instr je

void do_execute() {
	if(cpu.ZF == 1) cpu.eip += op_src->imm;
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
