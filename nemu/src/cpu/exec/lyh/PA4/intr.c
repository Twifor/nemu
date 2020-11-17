#include <setjmp.h>
#include "nemu.h"

extern jmp_buf jbuf;

void raise_intr(uint8_t NO) {
	printf("int!! %x\n", NO);

	longjmp(jbuf, 1);
}
