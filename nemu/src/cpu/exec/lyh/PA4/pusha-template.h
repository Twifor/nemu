#include "cpu/exec/template-start.h"

//#define instr pusha

void concat(tmp_push_, SUFFIX) (uint32_t val) {
	reg_l(R_ESP) -= DATA_BYTE;
	swaddr_write(reg_l(R_ESP), DATA_BYTE, (DATA_TYPE) val, R_SS);
}


make_helper(concat(pusha_, SUFFIX)) {
	if (DATA_BYTE == 2)
	{
		uint16_t tmp = reg_w(R_SP);
		concat(tmp_push_, SUFFIX) (reg_w(R_AX));
		concat(tmp_push_, SUFFIX) (reg_w(R_CX));
		concat(tmp_push_, SUFFIX) (reg_w(R_DX));
		concat(tmp_push_, SUFFIX) (tmp);
		concat(tmp_push_, SUFFIX) (reg_w(R_BP));
		concat(tmp_push_, SUFFIX) (reg_w(R_SI));
		concat(tmp_push_, SUFFIX) (reg_w(R_DI));
	}
	else if (DATA_BYTE == 4)
	{
		uint32_t tmp = reg_w(R_ESP);
		concat(tmp_push_, SUFFIX) (reg_l(R_EAX));
		concat(tmp_push_, SUFFIX) (reg_l(R_ECX));
		concat(tmp_push_, SUFFIX) (reg_l(R_EDX));
		concat(tmp_push_, SUFFIX) (tmp);
		concat(tmp_push_, SUFFIX) (reg_l(R_EBP));
		concat(tmp_push_, SUFFIX) (reg_l(R_ESI));
		concat(tmp_push_, SUFFIX) (reg_l(R_EDI));
	}
	print_asm("pusha");
	return 1;
}

#include "cpu/exec/template-end.h"
