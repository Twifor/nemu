#include "cpu/exec/template-start.h"

make_helper(mynop) {
	print_asm("nop");
	return 1;
}

#include "cpu/exec/template-end.h"
