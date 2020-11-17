#include "cpu/exec/template-start.h"

make_helper(cli) {
	cpu.IF = 0;
	print_asm("cli");
	return 1;
}

#include "cpu/exec/template-end.h"
