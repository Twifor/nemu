#include <setjmp.h>
#include "nemu.h"

extern jmp_buf jbuf;

void concat(tmp_push_, SUFFIX) (uint32_t val);

void raise_intr(uint8_t NO) {
	printf("int %x\n", NO);//debug use

	//Interrupt start


	

	longjmp(jbuf, 1);
}
