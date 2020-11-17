#include "cpu/exec/template-start.h"

DATA_TYPE concat(tmp_pop2_, SUFFIX) () {
	DATA_TYPE ans =  swaddr_read(reg_l(R_ESP), DATA_BYTE, R_SS);
	reg_l (R_ESP) += DATA_BYTE;
	return ans;
}


make_helper(concat(iret_, SUFFIX)) {
	cpu.eip = concat(tmp_pop2_, SUFFIX) ();
	cpu.cs.val = concat(tmp_pop2_, SUFFIX) ();
	cpu.eflags = concat(tmp_pop2_, SUFFIX) ();
	if(cpu.cr0.protect_enable) loadSregCache(R_CS);
	print_asm("iret");
	return 1;
}

#include "cpu/exec/template-end.h"
