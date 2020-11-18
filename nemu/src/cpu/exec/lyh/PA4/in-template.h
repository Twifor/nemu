#include "cpu/exec/template-start.h"

//#define instr in

make_helper(concat(in_i2a_, SUFFIX)) {
	//uint8_t imm = instr_fetch(eip + 1, 1);
	print_asm_template2();
	return 2;
}

make_helper(concat(in_, SUFFIX)) {
	return 1;
}

#include "cpu/exec/template-end.h"
