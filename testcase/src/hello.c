#include <stdio.h>

#include "trap.h"

int main() {
	set_bp();
	printf("%s", "Hello world!\n");

	printf("%d + %d = %d\n", 1, 1, 2);

	return 0;
}
