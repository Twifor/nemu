#include "cpu/exec/template-start.h"


make_helper(cld) {
	cpu.DF = 0;
	print_asm("cld");
	return 1;
}

#include "cpu/exec/template-end.h"
