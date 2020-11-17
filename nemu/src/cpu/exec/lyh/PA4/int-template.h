#include "cpu/exec/template-start.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

void raise_intr(uint8_t NO);


make_helper(my_int) {
	void do_int3();
	do_int3();
	uint8_t NO = instr_fetch(eip + 1, 1);
	cpu.eip += 2;
	print_asm("int　%x", NO);
	raise_intr(NO);

	return 0;
	//add 2 to cpu.eip just now
}

#include "cpu/exec/template-end.h"
