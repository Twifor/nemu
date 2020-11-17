#include "cpu/exec/template-start.h"


make_helper(std) {
	cpu.DF = 1;
	print_asm("std");
	return 1;
}

#include "cpu/exec/template-end.h"
