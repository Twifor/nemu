#include "cpu/exec/template-start.h"

void raise_intr(uint8_t NO);

make_helper(my_int) {
	uint8_t NO = instr_fetch(eip + 1, 1);
	print_asm("int　%x", NO);
	raise_intr(NO);

	return 2;
}

#include "cpu/exec/template-end.h"
