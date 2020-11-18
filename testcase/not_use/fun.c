#include "trap.h"

void fun(int x, int y, int c, int d) {
	if(x > 100) return;
	fun(x + 1, y + 1, c + 1, d + 1);
}
int main() {
	fun(1, 2, 3, 4);
	return 0;
}
