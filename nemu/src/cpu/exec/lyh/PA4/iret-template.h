#include "cpu/exec/template-start.h"

DATA_TYPE concat(tmp_pop_, SUFFIX)();

make_helper(concat(iret_, SUFFIX)) {
	cpu.eip = concat(tmp_pop_, SUFFIX) ();//guess why?
	cpu.cs.val = concat(tmp_pop_, SUFFIX) ();
	cpu.eflags = concat(tmp_pop_, SUFFIX) ();
	if(cpu.cr0.protect_enable) loadSregCache(R_CS);
	print_asm("iret");

	void do_int3();
	printf("int end!!\n");
	do_int3();
	return 0;
}

#include "cpu/exec/template-end.h"
