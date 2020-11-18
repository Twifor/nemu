#include "cpu/exec/template-start.h"

void device_update();

make_helper(hlt) {
	printf("sss\n");
	while(!cpu.IF || !cpu.INTR) device_update();
	print_asm("hlt");
	return 1;
}

#include "cpu/exec/template-end.h"
