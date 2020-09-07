#include "cpu/exec/template-start.h"

#define instr clt

make_helper(concat(clt_, SUFFIX)) {
	if(DATA_BYTE == 2) {
		if((int16_t)cpu.gpr[0]._16 < 0) cpu.gpr[2]._16 = 0xffff;
		else cpu.gpr[2]._16 = 0;
	} else if(DATA_BYTE == 4) { 
		if((int32_t)cpu.eax < 0) cpu.edx = 0xffffffff;
		else cpu.edx = 0;
	}
	print_asm("clt" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
