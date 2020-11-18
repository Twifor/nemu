#include "cpu/exec/template-start.h"

void device_update();

make_helper(hlt) {
	printf("hlt\n");
	while(!cpu.IF || !cpu.INTR);
	print_asm("hlt");
	return 1;
}

#include "cpu/exec/template-end.h"
