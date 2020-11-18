#include "cpu/exec/template-start.h"

make_helper(hlt) {
	while(!cpu.IF || !cpu.INTR);
	print_asm("hlt");
	return 1;
}

#include "cpu/exec/template-end.h"
