#include "cpu/exec/template-start.h"

make_helper(sti) {
	cpu.IF = 1;
	print_asm("sti");
	return 1;
}

#include "cpu/exec/template-end.h"
