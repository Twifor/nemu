#include "cpu/exec/template-start.h"

DATA_TYPE concat(tmp_pop_, SUFFIX) () {
	DATA_TYPE ans =  swaddr_read(reg_l(R_ESP), DATA_BYTE, R_SS);
	reg_l (R_ESP) += DATA_BYTE;
	return ans;
}


make_helper(concat(popa_, SUFFIX)) {
	if (DATA_BYTE == 2)
	{
		reg_w(R_DI) = concat(tmp_pop_, SUFFIX)();
		reg_w(R_SI) = concat(tmp_pop_, SUFFIX)();
		reg_w(R_BP) = concat(tmp_pop_, SUFFIX)();
		concat(tmp_pop_, SUFFIX)();
		reg_w(R_BX) = concat(tmp_pop_, SUFFIX)();
		reg_w(R_DX) = concat(tmp_pop_, SUFFIX)();
		reg_w(R_CX) = concat(tmp_pop_, SUFFIX)();
		reg_w(R_AX) = concat(tmp_pop_, SUFFIX)();
	}
	else if (DATA_BYTE == 4)
	{
		reg_l(R_EDI) = concat(tmp_pop_, SUFFIX)();
		reg_l(R_ESI) = concat(tmp_pop_, SUFFIX)();
		reg_l(R_EBP) = concat(tmp_pop_, SUFFIX)();
		concat(tmp_pop_, SUFFIX)();
		reg_l(R_EBX) = concat(tmp_pop_, SUFFIX)();
		reg_l(R_EDX) = concat(tmp_pop_, SUFFIX)();
		reg_l(R_ECX) = concat(tmp_pop_, SUFFIX)();
		reg_l(R_EAX) = concat(tmp_pop_, SUFFIX)();
	}
	print_asm("popa");
	return 1;
}

#include "cpu/exec/template-end.h"
